#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <cassert>
#include <optional>
#include <atomic>
#include <glm/glm.hpp>

namespace dse
{
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

	template <class T, class ...Args>
	T * GameAlloc(Args... args)
	{
		auto ptr = reinterpret_cast<T *>(GameAllocRaw(sizeof(T)));
		new (ptr) T(args...);
		return ptr;
	}

	template <class T>
	void GameDelete(T* obj)
	{
		obj->~T();
		GameFree(obj);
	}

	template <class T, class ...Args>
	T * GameAllocArray(std::size_t n, Args... args)
	{
		auto ptr = reinterpret_cast<T *>(GameAllocRaw(sizeof(T) * n));
		for (auto i = 0; i < n; i++) {
			new (ptr + i) T(args...);
		}
		return ptr;
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
			return (T*)GetStaticSymbols().CrtAlloc(cnt * sizeof(T));
		}

		inline void deallocate(T* p, std::size_t cnt) noexcept
		{
			GetStaticSymbols().CrtFree(p);
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
	std::false_type AllocatableImpl(...);

	#define MARK_ALLOCATABLE(ty) template <class T> \
		std::true_type AllocatableImpl(T*)

	template <class T>
	struct IsAllocatable : decltype(AllocatableImpl<T>(nullptr))
	{};


#if defined(OSI_EOCAPP)
	template <class T>
	using Vector = std::vector<T, GameAllocator<T>>;
#else
	template <class T>
	using Vector = std::vector<T, MSVCAllocator<T>>;
#endif

	using Vector3 = glm::vec3;

	struct NetId
	{
		static constexpr uint32_t Unassigned = 0xffffffff;

		uint32_t Id;

		inline constexpr NetId() : Id(Unassigned) {}
		inline constexpr NetId(uint32_t id) : Id(id) {}
		inline constexpr NetId(NetId const & id) : Id(id.Id) {}

		inline operator bool() const
		{
			return Id != Unassigned;
		}

		inline bool operator !() const
		{
			return Id == Unassigned;
		}

		inline constexpr uint16_t GetIndex() const
		{
			return (uint16_t)Id;
		}

		inline constexpr uint16_t GetSalt() const
		{
			return (uint16_t)(Id >> 16);
		}
	};

	constexpr NetId NetIdUnassigned{ 0xffffffff };

	using PeerId = int32_t;

	struct UserId
	{
		static constexpr int32_t Unassigned = (int32_t)0xFFFF0000;

		int32_t Id;

		inline constexpr UserId() : Id(Unassigned) {}
		inline explicit constexpr UserId(int32_t id) : Id(id) {}
		inline constexpr UserId(UserId const& id) : Id(id.Id) {}

		inline operator bool() const
		{
			return Id != Unassigned;
		}

		inline bool operator !() const
		{
			return Id == Unassigned;
		}

		inline bool operator ==(UserId const& o) const
		{
			return Id == o.Id;
		}

		inline bool operator !=(UserId const& o) const
		{
			return Id != o.Id;
		}

		inline constexpr PeerId GetPeerId() const
		{
			return (PeerId)(Id >> 16);
		}
	};

	static constexpr UserId ReservedUserId{ UserId::Unassigned };

	// Base class for game objects that cannot be copied.
	template <class T>
	class Noncopyable
	{
	public:
		inline Noncopyable() {}

		Noncopyable(const Noncopyable &) = delete;
		T & operator = (const T &) = delete;
		Noncopyable(Noncopyable &&) = delete;
		T & operator = (T &&) = delete;
	};

	// Base class for game objects that are managed entirely
	// by the game and we cannot create/copy them.
	template <class T>
	class ProtectedGameObject
	{
	public:
		ProtectedGameObject(const ProtectedGameObject &) = delete;
		T & operator = (const T &) = delete;
		ProtectedGameObject(ProtectedGameObject &&) = delete;
		T & operator = (T &&) = delete;

	protected:
		ProtectedGameObject() = delete;
		//~ProtectedGameObject() = delete;
	};

	struct FixedString
	{
		enum FromPool {};

		inline FixedString()
			: Str(nullptr)
		{}

		inline explicit FixedString(char const * s, FromPool)
			: Str(s)
		{
			IncRef();
		}

		inline FixedString(FixedString const & fs)
			: Str(fs.Str)
		{
			IncRef();
		}

		inline FixedString(FixedString && fs)
			: Str(fs.Str)
		{
			fs.Str = nullptr;
		}

		inline ~FixedString()
		{
			DecRef();
		}

		inline FixedString & operator = (FixedString const& fs)
		{
			if (fs.Str != Str) {
				DecRef();
				Str = fs.Str;
				IncRef();
			}

			return *this;
		}

		inline FixedString& operator = (FixedString && fs)
		{
			Str = fs.Str;
			if (this != &fs) {
				fs.Str = nullptr;
			}

			return *this;
		}

		inline bool operator == (FixedString const & fs) const
		{
			return Str == fs.Str;
		}

		inline bool operator != (FixedString const & fs) const
		{
			return Str != fs.Str;
		}

		inline bool operator !() const
		{
			return Str == nullptr;
		}

		inline explicit operator bool() const
		{
			return Str != nullptr;
		}

		char const * Str;

		struct Metadata
		{
			std::atomic<int64_t> RefCount;
			uint16_t Length;
			uint16_t HashKey;
		};

	private:

		inline void IncRef()
		{
			if (Str) {
				auto meta = (Metadata*)(const_cast<char *>(Str - 0x10));
				meta->RefCount++;
			}
		}

		inline void DecRef()
		{
			if (Str) {
				auto meta = (Metadata*)(const_cast<char*>(Str - 0x10));
				meta->RefCount--;
				// TODO - remove from global pool if refcount reaches 1!
			}
		}
	};

	FixedString ToFixedString(const char * s);
	FixedString MakeFixedString(const char* s);


	template <class T>
	uint64_t Hash(T const&)
	{
		static_assert(false, "Hash<T> needs to be specialized!");
	}

	template <>
	inline uint64_t Hash<uint8_t>(uint8_t const& v)
	{
		return v;
	}

	template <>
	inline uint64_t Hash<uint16_t>(uint16_t const& v)
	{
		return v;
	}

	template <>
	inline uint64_t Hash<uint32_t>(uint32_t const& v)
	{
		return v;
	}

	template <>
	inline uint64_t Hash<int32_t>(int32_t const& v)
	{
		return v;
	}

	template <>
	inline uint64_t Hash<FixedString>(FixedString const& s)
	{
		return (uint64_t)s.Str;
	}

	unsigned int GetNearestLowerPrime(unsigned int num);

	template <class TKey, class TValue>
	struct Map : public Noncopyable<Map<TKey, TValue>>
	{
		struct Node
		{
			Node * Next{ nullptr };
			TKey Key;
			TValue Value;
		};

		uint32_t HashSize{ 0 };
		Node** HashTable{ nullptr };
		uint32_t ItemCount{ 0 };

		Map() {}

		Map(uint32_t hashSize)
		{
			Init(hashSize);
		}

		~Map()
		{
			Clear();
		}

		void Init(uint32_t hashSize)
		{
			HashSize = hashSize;
			HashTable = GameAllocArray<Node *>(hashSize);
			ItemCount = 0;
			memset(HashTable, 0, sizeof(Node *) * hashSize);
		}

		void Clear()
		{
			ItemCount = 0;
			for (uint32_t i = 0; i < HashSize; i++) {
				auto item = HashTable[i];
				if (item != nullptr) {
					FreeHashChain(item);
					HashTable[i] = nullptr;
				}
			}
		}

		void FreeHashChain(Node* node)
		{
			do {
				auto next = node->Next;
				GameDelete(node);
				node = next;
			} while (node != nullptr);
		}

		TValue* Insert(TKey const& key, TValue const & value)
		{
			auto nodeValue = Insert(key);
			*nodeValue = value;
			return nodeValue;
		}

		TValue* Insert(TKey const& key)
		{
			auto item = HashTable[Hash(key) % HashSize];
			auto last = item;
			while (item != nullptr) {
				if (key == item->Key) {
					return &item->Value;
				}

				last = item;
				item = item->Next;
			}

			auto node = GameAlloc<Node>();
			node->Next = nullptr;
			node->Key = key;

			if (last == nullptr) {
				HashTable[Hash(key) % HashSize] = node;
			} else {
				last->Next = node;
			}

			ItemCount++;
			return &node->Value;
		}

		TValue * Find(TKey const& key) const
		{
			auto item = HashTable[Hash(key) % HashSize];
			while (item != nullptr) {
				if (key == item->Key) {
					return &item->Value;
				}

				item = item->Next;
			}

			return nullptr;
		}

		TKey * FindByValue(TValue const & value) const
		{
			for (uint32_t bucket = 0; bucket < HashSize; bucket++) {
				Node * item = HashTable[bucket];
				while (item != nullptr) {
					if (value == item->Value) {
						return &item->Key;
					}

					item = item->Next;
				}
			}

			return nullptr;
		}

		template <class Visitor>
		void Iterate(Visitor visitor)
		{
			for (uint32_t bucket = 0; bucket < HashSize; bucket++) {
				Node * item = HashTable[bucket];
				while (item != nullptr) {
					visitor(item->Key, item->Value);
					item = item->Next;
				}
			}
		}

		template <class Visitor>
		void Iterate(Visitor visitor) const
		{
			for (uint32_t bucket = 0; bucket < HashSize; bucket++) {
				Node* item = HashTable[bucket];
				while (item != nullptr) {
					visitor(item->Key, item->Value);
					item = item->Next;
				}
			}
		}
	};

	template <class TKey, class TValue>
	struct RefMap : public Noncopyable<RefMap<TKey, TValue>>
	{
		struct Node
		{
			Node* Next{ nullptr };
			TKey Key;
			TValue Value;
		};

		uint32_t ItemCount{ 0 };
		uint32_t HashSize{ 0 };
		Node** HashTable{ nullptr };

		RefMap(uint32_t hashSize = 31)
			: ItemCount(0), HashSize(hashSize)
		{
			HashTable = GameAllocArray<Node*>(hashSize);
			memset(HashTable, 0, sizeof(Node*) * hashSize);
		}

		~RefMap()
		{
			if (HashTable != nullptr) {
				GameFree(HashTable);
			}
		}

		void Clear()
		{
			ItemCount = 0;
			for (uint32_t i = 0; i < HashSize; i++) {
				auto item = HashTable[i];
				if (item != nullptr) {
					FreeHashChain(item);
					HashTable[i] = nullptr;
				}
			}
		}

		void FreeHashChain(Node* node)
		{
			do {
				auto next = node->Next;
				GameDelete(node);
				node = next;
			} while (node != nullptr);
		}

		TValue * Find(TKey const & key) const
		{
			auto item = HashTable[Hash(key) % HashSize];
			while (item != nullptr) {
				if (key == item->Key) {
					return &item->Value;
				}

				item = item->Next;
			}

			return nullptr;
		}

		TValue* Insert(TKey const& key)
		{
			auto item = HashTable[Hash(key) % HashSize];
			auto last = item;
			while (item != nullptr) {
				if (key == item->Key) {
					return &item->Value;
				}

				last = item;
				item = item->Next;
			}

			auto node = GameAlloc<Node>();
			node->Next = nullptr;
			node->Key = key;

			if (last == nullptr) {
				HashTable[Hash(key) % HashSize] = node;
			} else {
				last->Next = node;
			}

			ItemCount++;
			return &node->Value;
		}

		template <class Visitor>
		void Iterate(Visitor visitor)
		{
			for (uint32_t bucket = 0; bucket < HashSize; bucket++) {
				Node * item = HashTable[bucket];
				while (item != nullptr) {
					visitor(item->Key, item->Value);
					item = item->Next;
				}
			}
		}
	};

	struct GameMemoryAllocator
	{
		static void* Alloc(std::size_t size)
		{
			return GameAllocRaw(size);
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
			return (T *)GetStaticSymbols().CrtAlloc(sizeof(T));
		}

		template <class T>
		static T * New(std::size_t count)
		{
			return (T *)GetStaticSymbols().CrtAlloc(sizeof(T) * count);
		}

		template <class T>
		static void Free(T * ptr)
		{
			GetStaticSymbols().CrtFree(ptr);
		}

		template <class T>
		static void FreeArray(T * ptr)
		{
			GetStaticSymbols().CrtFree(ptr);
		}
	};

	using STDString = std::basic_string<char, std::char_traits<char>, GameAllocator<char>>;
	using STDWString = std::basic_string<wchar_t, std::char_traits<wchar_t>, GameAllocator<wchar_t>>;
	using StringView = std::string_view;
	using WStringView = std::wstring_view;

	dse::STDString ToUTF8(WStringView s);
	dse::STDWString FromUTF8(StringView s);

	template <class T>
	struct ContiguousIterator
	{
		T* Ptr;

		ContiguousIterator(T* p) : Ptr(p) {}

		ContiguousIterator operator ++ ()
		{
			ContiguousIterator<T> it(Ptr);
			Ptr++;
			return it;
		}

		ContiguousIterator& operator ++ (int)
		{
			Ptr++;
			return *this;
		}

		bool operator == (ContiguousIterator const& it)
		{
			return it.Ptr == Ptr;
		}

		bool operator != (ContiguousIterator const& it)
		{
			return it.Ptr != Ptr;
		}

		T& operator * ()
		{
			return *Ptr;
		}

		T* operator -> ()
		{
			return Ptr;
		}
	};

	template <class T>
	struct ContiguousConstIterator
	{
		T const* Ptr;

		ContiguousConstIterator(T const* p) : Ptr(p) {}

		ContiguousConstIterator operator ++ ()
		{
			ContiguousConstIterator<T> it(Ptr);
			Ptr++;
			return it;
		}

		ContiguousConstIterator& operator ++ (int)
		{
			Ptr++;
			return *this;
		}

		bool operator == (ContiguousConstIterator const& it)
		{
			return it.Ptr == Ptr;
		}

		bool operator != (ContiguousConstIterator const& it)
		{
			return it.Ptr != Ptr;
		}

		T const& operator * ()
		{
			return *Ptr;
		}

		T const* operator -> ()
		{
			return Ptr;
		}
	};

	template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
	struct CompactSet
	{
		T * Buf{ nullptr };
		uint32_t Capacity{ 0 };
		uint32_t Size{ 0 };

		inline CompactSet() {}

		CompactSet(CompactSet const& other)
		{
			Reallocate(other.Size);
			Size = other.Size;
			for (uint32_t i = 0; i < other.Size; i++) {
				Buf[i] = other.Buf[i];
			}
		}

		~CompactSet()
		{
			if (Buf) {
				for (uint32_t i = 0; i < Size; i++) {
					Buf[i].~T();
				}

				FreeBuffer(Buf);
			}
		}

		CompactSet& operator = (CompactSet const& other)
		{
			Reallocate(other.Size);
			Size = other.Size;
			for (uint32_t i = 0; i < other.Size; i++) {
				Buf[i] = other.Buf[i];
			}
			return *this;
		}

		inline T const & operator [] (uint32_t index) const
		{
			return Buf[index];
		}

		inline T & operator [] (uint32_t index)
		{
			return Buf[index];
		}

		void FreeBuffer(void * buf)
		{
			if (StoreSize) {
				if (buf != nullptr) {
					Allocator::Free((void *)((std::ptrdiff_t)buf - 8));
				}
			} else {
				if (buf != nullptr) {
					Allocator::Free(buf);
				}
			}
		}

		void RawReallocate(uint32_t newCapacity)
		{
			if (newCapacity > 0) {
				if (StoreSize) {
					auto newBuf = Allocator::Alloc(newCapacity * sizeof(T) + 8);
					*(uint64_t*)newBuf = newCapacity;

					Buf = (T*)((std::ptrdiff_t)newBuf + 8);
					for (uint32_t i = 0; i < newCapacity; i++) {
						new (Buf + i) T();
					}
				}
				else {
					Buf = Allocator::New<T>(newCapacity);
				}
			} else {
				Buf = nullptr;
			}

			Capacity = newCapacity;
		}

		void Reallocate(uint32_t newCapacity)
		{
			auto oldBuf = Buf;
			RawReallocate(newCapacity);
			for (uint32_t i = 0; i < std::min(Size, newCapacity); i++) {
				Buf[i] = oldBuf[i];
			}
			FreeBuffer(oldBuf);
		}

		void Remove(uint32_t index)
		{
			if (index >= Size) {
				ERR("Tried to remove out-of-bounds index %d!", index);
				return;
			}

			for (auto i = index; i < Size - 1; i++) {
				Buf[i] = Buf[i + 1];
			}

			Size--;
		}

		void Clear()
		{
			Size = 0;
		}

		ContiguousIterator<T> begin()
		{
			return ContiguousIterator<T>(Buf);
		}

		ContiguousConstIterator<T> begin() const
		{
			return ContiguousConstIterator<T>(Buf);
		}

		ContiguousIterator<T> end()
		{
			return ContiguousIterator<T>(Buf + Size);
		}

		ContiguousConstIterator<T> end() const
		{
			return ContiguousConstIterator<T>(Buf + Size);
		}
	};

	template <class T, class Allocator = GameMemoryAllocator>
	struct PrimitiveSet : public CompactSet<T, Allocator, false>
	{
		uint32_t CapacityIncrement() const
		{
			if (Capacity > 0) {
				return 2 * Capacity;
			} else {
				return 1;
			}
		}

		void Add(T const & value)
		{
			if (Capacity <= Size) {
				Reallocate(CapacityIncrement());
			}

			Buf[Size++] = value;
		}
	};

	template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
	struct Set : public CompactSet<T, Allocator, StoreSize>
	{
		uint64_t CapacityIncrementSize{ 0 };

		uint32_t CapacityIncrement() const
		{
			if (CapacityIncrementSize != 0) {
				return Capacity + (uint32_t)CapacityIncrementSize;
			} else if (Capacity > 0) {
				return 2 * Capacity;
			} else {
				return 1;
			}
		}

		void Add(T const & value)
		{
			if (Capacity <= Size) {
				Reallocate(CapacityIncrement());
			}

			Buf[Size++] = value;
		}

		void InsertAt(uint32_t index, T const & value)
		{
			if (Capacity <= Size) {
				Reallocate(CapacityIncrement());
			}

			for (auto i = Size; i > index; i--) {
				Buf[i] = Buf[i - 1];
			}

			Buf[index] = value;
			Size++;
		}
	};

	template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
	struct ObjectSet
	{
		void * VMT{ nullptr };
		Set<T, Allocator, StoreSize> Set;

		inline T const & operator [] (uint32_t index) const
		{
			return Set[index];
		}

		inline T & operator [] (uint32_t index)
		{
			return Set[index];
		}

		ContiguousIterator<T> begin()
		{
			return Set.begin();
		}

		ContiguousConstIterator<T> begin() const
		{
			return Set.begin();
		}

		ContiguousIterator<T> end()
		{
			return Set.end();
		}

		ContiguousConstIterator<T> end() const
		{
			return Set.end();
		}
	};

	template <class T, class Allocator = GameMemoryAllocator>
	struct CompactObjectSet
	{
		void * VMT{ nullptr };
		CompactSet<T, Allocator> Set;

		inline T const & operator [] (uint32_t index) const
		{
			return Set[index];
		}

		inline T & operator [] (uint32_t index)
		{
			return Set[index];
		}

		ContiguousIterator<T> begin()
		{
			return Set.begin();
		}

		ContiguousConstIterator<T> begin() const
		{
			return Set.begin();
		}

		ContiguousIterator<T> end()
		{
			return Set.end();
		}

		ContiguousConstIterator<T> end() const
		{
			return Set.end();
		}
	};

	template <unsigned TDWords>
	struct BitArray
	{
		uint32_t Bits[TDWords];

		inline bool Set(uint32_t index)
		{
			if (index <= 0 || index > (TDWords * 32)) {
				return false;
			}

			Bits[(index - 1) >> 5] |= (1 << ((index - 1) & 0x1f));
			return true;
		}

		inline bool Clear(uint32_t index)
		{
			if (index <= 0 || index > (TDWords * 32)) {
				return false;
			}

			Bits[(index - 1) >> 5] &= ~(1 << ((index - 1) & 0x1f));
			return true;
		}

		inline bool IsSet(uint32_t index) const
		{
			if (index <= 0 || index > (TDWords * 32)) {
				return false;
			}

			return (Bits[(index - 1) >> 5] & (1 << ((index - 1) & 0x1f))) != 0;
		}
	};

	struct Path
	{
		STDString Name;
#if !defined(OSI_EOCAPP)
		uint32_t Unknown{ 0 };
#endif
	};

	struct RuntimeStringHandle
	{
		void * VMT;
		FixedString Handle;
		STDString Str;
		STDWString WStr;
	};

	struct TranslatedString
	{
		void * VMT;
		RuntimeStringHandle Str1;
		RuntimeStringHandle Str2;
	};

	template <class T>
	struct Array
	{
		void * VMT{ nullptr };
		T * Buf{ nullptr };
		uint32_t Capacity{ 0 };
		uint32_t Size{ 0 };
		uint32_t Unkn[2]{ 0 };

		inline Array() {}

		Array(Array const& a)
		{
			CopyFrom(a);
		}

		Array& operator =(Array const& a)
		{
			CopyFrom(a);
			return *this;
		}

		void CopyFrom(Array const& a)
		{
			VMT = a.VMT;
			Unkn[0] = a.Unkn[0];
			Unkn[1] = a.Unkn[1];
			Clear();

			if (a.Size > 0) {
				Reallocate(a.Size);
				Size = a.Size;
				for (uint32_t i = 0; i < Size; i++) {
					Buf[i] = a[i];
				}
			}
		}

		inline T const & operator [] (uint32_t index) const
		{
			return Buf[index];
		}

		inline T & operator [] (uint32_t index)
		{
			return Buf[index];
		}

		uint32_t CapacityIncrement() const
		{
			if (Capacity > 0) {
				return 2 * Capacity;
			} else {
				return 1;
			}
		}

		void Clear()
		{
			Size = 0;
		}

		void Reallocate(uint32_t newCapacity)
		{
			auto newBuf = GameAllocArray<T>(newCapacity);
			for (uint32_t i = 0; i < std::min(Size, newCapacity); i++) {
				newBuf[i] = Buf[i];
			}

			if (Buf != nullptr) {
				GameFree(Buf);
			}

			Buf = newBuf;
			Capacity = newCapacity;
		}

		bool SafeAdd(T const & val)
		{
			if (Capacity <= Size) {
				Reallocate(CapacityIncrement());
			}

			if (Size < Capacity) {
				Buf[Size++] = val;
				return true;
			} else {
				return false;
			}
		}

		void Remove(uint32_t index)
		{
			if (index >= Size) {
				ERR("Tried to remove out-of-bounds index %d!", index);
				return;
			}

			for (auto i = index; i < Size - 1; i++) {
				Buf[i] = Buf[i + 1];
			}

			Size--;
		}

		ContiguousIterator<T> begin()
		{
			return ContiguousIterator<T>(Buf);
		}

		ContiguousConstIterator<T> begin() const
		{
			return ContiguousConstIterator<T>(Buf);
		}

		ContiguousIterator<T> end()
		{
			return ContiguousIterator<T>(Buf + Size);
		}

		ContiguousConstIterator<T> end() const
		{
			return ContiguousConstIterator<T>(Buf + Size);
		}
	};

	struct ObjectHandle
	{
		uint64_t Handle;

		inline ObjectHandle()
			: Handle(0)
		{}

		inline ObjectHandle(uint64_t handle)
			: Handle(handle)
		{}

		inline ObjectHandle(int64_t handle)
			: Handle((uint64_t)handle)
		{}

		inline ObjectHandle(uint64_t type, uint64_t index, uint64_t salt)
		{
			if (type >= 0x400 || salt >= 0x400000) {
				Fail("ObjectHandle type/salt overflow.");
			}

			Handle = index | (salt << 32) | (type << 54);
		}

		inline ObjectHandle(ObjectHandle const & oh)
			: Handle(oh.Handle)
		{}

		inline ObjectHandle & operator = (ObjectHandle const & oh)
		{
			Handle = oh.Handle;
			return *this;
		}

		inline bool operator == (ObjectHandle const & oh) const
		{
			return Handle == oh.Handle;
		}

		inline uint32_t GetType() const
		{
			return Handle >> 54;
		}

		inline uint32_t GetSalt() const
		{
			return (Handle >> 32) & 0x3fffff;
		}

		inline uint32_t GetIndex() const
		{
			return (uint32_t)(Handle & 0xffffffff);
		}

		explicit inline operator bool() const
		{
			return Handle != 0;
		}

		inline bool operator !() const
		{
			return Handle == 0;
		}

		explicit inline operator int64_t() const
		{
			return (int64_t)Handle;
		}
	};

	template <class T, uint32_t TypeIndex>
	struct ObjectFactory : public ProtectedGameObject<ObjectFactory<T, TypeIndex>>
	{
		/*virtual ObjectHandle * ReevaluateHandle(ObjectHandle & handle) = 0;
		virtual ObjectHandle *  GetFreeHandle(ObjectHandle & handle) = 0;
		virtual bool IsFreeIndex(uint32_t index) = 0;
		virtual bool IsReservedIndex(uint32_t index) = 0;
		virtual uint64_t ReserveIndex(uint32_t index) = 0;
		virtual uint64_t UnreserveIndex(uint32_t index) = 0;
		virtual void Destroy() = 0;*/

		void * VMT;
		Array<T *> Objects;
		Array<uint32_t> Salts;
		Set<void *> Unknown;
		ObjectSet<T *> Primitives;
		uint8_t Unknown2;
		uint32_t Unknown3;

		T * Get(ObjectHandle handle) const
		{
			if (!handle || handle.GetType() != TypeIndex) {
				return nullptr;
			}

			auto index = handle.GetIndex();
			if (index >= Objects.Size) {
				return nullptr;
			}

			if (Salts[index] != handle.GetSalt()) {
				return nullptr;
			}

			return Objects[index];
		}
	};

	struct Component : public ProtectedGameObject<Component>
	{
		virtual void Destroy() = 0;
		virtual void DestroyComponent() = 0;
		virtual void CreateComponent() = 0;
		virtual void ForceCreateComponent() = 0; // ForceComponentDefault
		virtual void * FindComponentByHandle(ObjectHandle const * oh) = 0;
		virtual void * TryFindComponentByHandle(ObjectHandle const* oh) = 0;
		virtual void * FindComponentByGuid(FixedString const * fs) = 0;
		virtual bool MoveComponentByGuid(FixedString const* fs, void* component) = 0;
		virtual void * FindComponentByNetId(NetId const & netId, bool checkSalt = true) = 0;
		virtual void UNKN() = 0;
		virtual void * GetComponentByIndex(uint64_t index) = 0;
		virtual void GetFreeHandle() = 0;
		virtual void IsFreeIndex() = 0;
		virtual void IsReservedIndex() = 0;
		virtual void ReserveIndex() = 0;
		virtual void UnreserveIndex() = 0;
	};

	struct ComponentHandle
	{
		int64_t TypeId;
		ObjectHandle Handle;

		inline bool IsValid() const
		{
			return TypeId != -1 && !!Handle;
		}
	};

	struct BaseComponent
	{
		void * VMT;
		ObjectHandle EntityObjectHandle;
		ComponentHandle Component;
	};

	struct GlobalStringTable : public ProtectedGameObject<GlobalStringTable>
	{
		struct Entry
		{
			const char * StringPtrs[10];
			Entry * Next;
			uint32_t StringPtrItems;
			uint32_t Unused;

			uint32_t Count() const;
			char const * Get(uint32_t i) const;
		};

		Entry HashTable[65521];

		const char * Find(char const * s, uint64_t length) const;
		static uint32_t Hash(char const * s, uint64_t length);
	};

	struct ScratchBuffer : public Noncopyable<ScratchBuffer>
	{
		void * Buffer{ nullptr };
		uint32_t Unkn{ 0 };
		uint32_t Size{ 0 };
		uint32_t WritePosition{ 0 };
		uint32_t ReadPosition{ 0 };
		uint32_t GrowSize{ 0 };
	};

	struct ObjectVisitor : public ProtectedGameObject<ObjectVisitor>
	{
		virtual void Destroy() = 0;
		virtual void SetFileName(Path*) = 0;
		virtual void SetFileVersion(uint32_t*) = 0;
		virtual void SetCodeVersion(uint32_t*) = 0;
		virtual Path* GetFileName() = 0;
		virtual uint32_t GetCodeVersion() = 0;
		virtual uint32_t GetFileVersion() = 0;
		virtual void SetIO() = 0;
		virtual bool IsReading() = 0;
		virtual void Save(Path*) = 0;
		virtual void Load(Path*) = 0;
		virtual void Print(const char*) = 0;
		virtual bool TryEnterRegion(FixedString const&) = 0;
		virtual bool EnterRegion(FixedString const&) = 0;
		virtual void HasRegion(FixedString const&) = 0;
		virtual void ExitRegion(FixedString const&) = 0;
		virtual bool EnterNode(FixedString const&, FixedString const&) = 0;
		virtual void HasNode(FixedString const&) = 0;
		virtual void ExitNode(FixedString const&) = 0;
		virtual void VisitCount(FixedString const&, uint32_t*) = 0;
		virtual void FindDataType(FixedString const&) = 0;
		virtual void VisitRaw(FixedString const&, void*, uint64_t, uint32_t) = 0;
		virtual void VisitTransatedFSString() = 0;
		virtual void VisitGuid(FixedString const&, void*, void const*) = 0;
		virtual void VisitTranslatedString(FixedString const&, TranslatedString&, TranslatedString const&) = 0;
		virtual void VisitSTDWString(FixedString const&, STDWString&, STDWString const&) = 0;
		virtual void VisitSTDString(FixedString const&, STDString&, STDString const&) = 0;
		virtual void VisitFixedString(FixedString const&, FixedString&, FixedString const&) = 0;
		virtual void VisitPath(FixedString const&, Path*, Path*) = 0;
		virtual void VisitWCharT() = 0;
		virtual void VisitWCharT2() = 0;
		virtual void VisitCharArray() = 0;
		virtual void VisitCharArray2() = 0;
		virtual void VisitObjectHandle(FixedString const&, void*, void const*) = 0;
		virtual void VisitQuaternion(FixedString const&, float&, float const&) = 0;
		virtual void VisitMatrix4x3(FixedString const&, float&, float const&) = 0;
		virtual void VisitMatrix3x4(FixedString const&, float&, float const&) = 0;
		virtual void VisitMatrix4x4(FixedString const&, float&, float const&) = 0;
		virtual void VisitMatrix3x3(FixedString const&, float&, float const&) = 0;
		virtual void VisitMatrix2x2(FixedString const&, float&, float const&) = 0;
		virtual void VisitVector4Unaligned(FixedString const&, float&, float const&) = 0;
		virtual void VisitVector4(FixedString const&, float&, float const&) = 0;
		virtual void VisitVector3(FixedString const&, float&, float const&) = 0;
		virtual void VisitVector2(FixedString const&, float&, float const&) = 0;
		virtual void VisitVector3i(FixedString const&, int*, int*) = 0;
		virtual void VisitVector2i(FixedString const&, int*, int*) = 0;
		virtual void VisitUInt64(FixedString const&, uint64_t&, uint64_t) = 0;
		virtual void VisitInt64(FixedString const&, int64_t&, int64_t) = 0;
		virtual void VisitFloat(FixedString const&, float&, float) = 0;
		virtual void VisitDouble(FixedString const&, double&, double) = 0;
		virtual void VisitInt32(FixedString const&, int32_t&, int32_t) = 0;
		virtual void VisitUInt32(FixedString const&, uint32_t&, uint32_t) = 0;
		virtual void VisitUInt16(FixedString const&, uint16_t&, uint16_t) = 0;
		virtual void VisitInt16(FixedString const&, int16_t&, int16_t) = 0;
		virtual void VisitBool(FixedString const&, bool&, bool) = 0;
		virtual void VisitUInt8(FixedString const&, uint8_t&, uint8_t) = 0;
		virtual void VisitInt8(FixedString const&, int8_t&, int8_t) = 0;
		virtual void VisitBuffer(FixedString const&, ScratchBuffer&) = 0;
	};
}


namespace std
{
	template<> struct hash<dse::FixedString>
	{
		typedef dse::FixedString argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<std::uintptr_t>{}((std::uintptr_t)fn.Str);
		}
	};

	template<> struct hash<dse::ObjectHandle>
	{
		typedef dse::ObjectHandle argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<uint64_t>{}(fn.Handle);
		}
	};

	template<> struct hash<dse::UserId>
	{
		typedef dse::UserId argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<int32_t>{}(fn.Id);
		}
	};

	inline ostream& operator << (ostream& out, dse::FixedString const& str)
	{
		if (str) {
			out << str.Str;
		} else {
			out << "(null)";
		}
		return out;
	}
}
