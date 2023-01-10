#pragma once

//#define TRACK_ALLOCATIONS

#include <GameDefinitions/Base/BaseTypes.h>
#if defined(TRACK_ALLOCATIONS)
#include <mutex>
#endif

BEGIN_SE()

#if defined(TRACK_ALLOCATIONS)
class MemoryAllocationTracker
{
public:
	void Alloc(void* ptr, size_t sz, char const* tag = nullptr);
	void Free(void* ptr);

	template <class T>
	void Alloc(void* ptr, size_t sz)
	{
		auto ty = typeid(T);
		Alloc(ptr, sz, ty.name());
	}

private:
	struct AllocationInfo
	{
		char const* Tag;
		size_t Size;
		bool InUse;
	};

	std::map<void*, AllocationInfo> allocs_;
	std::mutex mutex_;
};

extern MemoryAllocationTracker gMemoryAllocationTracker;

#define MARK_EXTERNAL_MEMORY(ptr) { if (ptr) { gMemoryAllocationTracker.Alloc(ptr, 0, nullptr); } }
#define MARK_EXTERNAL_STR(s) { if (s.capacity() > 15) { gMemoryAllocationTracker.Alloc(s.data(), s.size(), "ExternalString"); } }
#define MARK_EXTERNAL_WSTR(s) { if (s.capacity() > 7) { gMemoryAllocationTracker.Alloc(s.data(), s.size()*2, "ExternalWString"); } }
#else
#define MARK_EXTERNAL_MEMORY(ptr)
#define MARK_EXTERNAL_STR(s)
#define MARK_EXTERNAL_WSTR(s)
#endif

#if defined(OSI_EOCAPP)
typedef void * (* EoCAllocFunc)(void * self, std::size_t size);
#else
typedef void * (* EoCAllocFunc)(void * self, std::size_t size, char const * file, int line, char const * function);
#endif
typedef void (* EoCFreeFunc)(void * self, void * ptr);

typedef void * (* CrtAllocFunc)(std::size_t size);
typedef void (* CrtFreeFunc)(void * ptr);

void * GameAllocInternal(std::size_t size);
void * GameAllocRaw(std::size_t size);
void * GameAllocRaw(std::size_t size, char const* tag);
void GameFree(void *);
void* CrtAllocRaw(std::size_t size);
void CrtFree(void*);

template <class T, class ...Args>
T * GameAlloc(Args&&... args)
{
	auto ptr = reinterpret_cast<T *>(GameAllocInternal(sizeof(T)));
#if defined(TRACK_ALLOCATIONS)
	gMemoryAllocationTracker.Alloc(ptr, sizeof(T), typeid(T).name());
#endif
	new (ptr) T(std::forward<Args>(args)...);
	return ptr;
}

template <class T>
void GameDelete(T* obj)
{
	obj->~T();
	GameFree(obj);
}

template <class T, class ...Args>
T * GameAllocArray(std::size_t n, Args&&... args)
{
	auto ptr = reinterpret_cast<T *>(GameAllocInternal(sizeof(T) * n));
#if defined(TRACK_ALLOCATIONS)
	gMemoryAllocationTracker.Alloc(ptr, sizeof(T) * n, typeid(T).name());
#endif
	for (std::size_t i = 0; i < n; i++) {
		new (ptr + i) T(std::forward<Args>(args)...);
	}
	return ptr;
}

template <class T>
void GameDeleteArray(T* ptr, std::size_t n)
{
	for (std::size_t i = 0; i < n; i++) {
		ptr[i].~T();
	}

	GameFree(ptr);
}

template <class T>
using GameUniquePtr = std::unique_ptr<T, decltype(&GameDelete<T>)>;

template <class T, class... Args>
GameUniquePtr<T> MakeGameUnique(Args&&... args)
{
	return GameUniquePtr<T>(GameAlloc<T>(std::forward<Args>(args)...), &GameDelete<T>);
}

template <class T>
class GameAllocator
{
public:
	using value_type = T;

	inline GameAllocator() noexcept {}
	template <class U>
	inline GameAllocator(GameAllocator<U> const &) noexcept {}

	inline T * allocate(std::size_t cnt)
	{
		auto ptr = reinterpret_cast<T *>(GameAllocInternal(cnt * sizeof(T)));
#if defined(TRACK_ALLOCATIONS)
		gMemoryAllocationTracker.Alloc(ptr, cnt * sizeof(T), typeid(T).name());
#endif
		return ptr;
	}

	inline void deallocate(T * p, std::size_t cnt) noexcept
	{
		GameFree(p);
	}
};

template <class T>
class MSVCAllocator
{
public:
	using value_type = T;

	inline MSVCAllocator() noexcept {}
	template <class U>
	inline MSVCAllocator(MSVCAllocator<U> const&) noexcept {}

	inline T* allocate(std::size_t cnt)
	{
		return (T*)CrtAllocRaw(cnt * sizeof(T));
	}

	inline void deallocate(T* p, std::size_t cnt) noexcept
	{
		CrtFree(p);
	}
};

template <class T, class U>
bool operator == (GameAllocator<T> const &, GameAllocator<U> const &) noexcept
{
	return true;
}

template <class T, class U>
bool operator != (GameAllocator<T> const & x, GameAllocator<U> const & y) noexcept
{
	return !(x == y);
}

// Identifies types that can be allocated by us (using the game allocator)
template <class>
std::false_type IsAllocatable(...);

#define MARK_ALLOCATABLE(ty) template <class T> \
	std::true_type IsAllocatable(T*)


#if defined(OSI_EOCAPP)
template <class T>
using Vector = std::vector<T, GameAllocator<T>>;
#else
template <class T>
using Vector = std::vector<T, MSVCAllocator<T>>;
#endif


struct GameMemoryAllocator
{
	static void* Alloc(std::size_t size)
	{
		return GameAllocRaw(size);
	}

	template <class T>
	static T* NewRaw()
	{
		auto ptr = reinterpret_cast<T*>(GameAllocInternal(sizeof(T)));
#if defined(TRACK_ALLOCATIONS)
		gMemoryAllocationTracker.Alloc(ptr, sizeof(T), typeid(T).name());
#endif
		return ptr;
	}

	template <class T>
	static T* NewRaw(std::size_t count)
	{
		auto ptr = reinterpret_cast<T*>(GameAllocInternal(count * sizeof(T)));
#if defined(TRACK_ALLOCATIONS)
		gMemoryAllocationTracker.Alloc(ptr, count * sizeof(T), typeid(T).name());
#endif
		return ptr;
	}

	template <class T>
	static T * New()
	{
		return GameAlloc<T>();
	}

	template <class T>
	static T * New(std::size_t count)
	{
		return GameAllocArray<T>(count);
	}

	template <class T>
	static void Free(T * ptr)
	{
		return GameFree(ptr);
	}

	template <class T>
	static void FreeArray(T * ptr)
	{
		return GameFree(ptr);
	}
};

struct MSVCMemoryAllocator
{
	static void* Alloc(std::size_t size);

	template <class T>
	static T * New()
	{
		return (T *)CrtAllocRaw(sizeof(T));
	}

	template <class T>
	static T * New(std::size_t count)
	{
		return (T *)CrtAllocRaw(sizeof(T) * count);
	}

	template <class T>
	static void Free(T * ptr)
	{
		CrtFree(ptr);
	}

	template <class T>
	static void FreeArray(T * ptr)
	{
		CrtFree(ptr);
	}
};

END_SE()
