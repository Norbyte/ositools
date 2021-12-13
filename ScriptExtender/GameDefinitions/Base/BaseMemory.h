#pragma once

#include <GameDefinitions/Base/BaseTypes.h>

BEGIN_SE()

#if defined(OSI_EOCAPP)
typedef void * (* EoCAllocFunc)(void * self, std::size_t size);
#else
typedef void * (* EoCAllocFunc)(void * self, std::size_t size, char const * file, int line, char const * function);
#endif
typedef void (* EoCFreeFunc)(void * self, void * ptr);

typedef void * (* CrtAllocFunc)(std::size_t size);
typedef void (* CrtFreeFunc)(void * ptr);

void * GameAllocRaw(std::size_t size);
void GameFree(void *);
void* CrtAllocRaw(std::size_t size);
void CrtFree(void*);

template <class T, class ...Args>
T * GameAlloc(Args&&... args)
{
	auto ptr = reinterpret_cast<T *>(GameAllocRaw(sizeof(T)));
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
	auto ptr = reinterpret_cast<T *>(GameAllocRaw(sizeof(T) * n));
	for (auto i = 0; i < n; i++) {
		new (ptr + i) T(std::forward<Args>(args)...);
	}
	return ptr;
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
		return reinterpret_cast<T *>(GameAllocRaw(cnt * sizeof(T)));
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
		return reinterpret_cast<T*>(GameAllocRaw(sizeof(T)));
	}
	template <class T>
	static T* NewRaw(std::size_t count)
	{
		return reinterpret_cast<T*>(GameAllocRaw(count * sizeof(T)));
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
