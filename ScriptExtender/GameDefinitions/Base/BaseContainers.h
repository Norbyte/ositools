#pragma once

#include <GameDefinitions/Base/BaseTypes.h>
#include <GameDefinitions/Base/BaseMemory.h>
#include <span>

BEGIN_SE()

unsigned int GetNearestLowerPrime(unsigned int num);

template <class T>
class ContiguousIterator
{
public:
	ContiguousIterator(T* p) : ptr_(p) {}

	ContiguousIterator operator ++ ()
	{
		ContiguousIterator<T> it(ptr_);
		ptr_++;
		return it;
	}

	ContiguousIterator& operator ++ (int)
	{
		ptr_++;
		return *this;
	}

	bool operator == (ContiguousIterator const& it)
	{
		return it.ptr_ == ptr_;
	}

	bool operator != (ContiguousIterator const& it)
	{
		return it.ptr_ != ptr_;
	}

	T& operator * ()
	{
		return *ptr_;
	}

	T* operator -> ()
	{
		return ptr_;
	}

private:
	T* ptr_;
};


template <class T>
class ContiguousConstIterator
{
public:
	ContiguousConstIterator(T const* p) : ptr_(p) {}

	ContiguousConstIterator operator ++ ()
	{
		ContiguousConstIterator<T> it(ptr_);
		ptr_++;
		return it;
	}

	ContiguousConstIterator& operator ++ (int)
	{
		ptr_++;
		return *this;
	}

	bool operator == (ContiguousConstIterator const& it)
	{
		return it.ptr_ == ptr_;
	}

	bool operator != (ContiguousConstIterator const& it)
	{
		return it.ptr_ != ptr_;
	}

	T const& operator * ()
	{
		return *ptr_;
	}

	T const* operator -> ()
	{
		return ptr_;
	}

private:
	T const* ptr_;
};

template <class TKey, class TValue>
struct MapInternals
{
	using KeyType = TKey;
	using ValueType = TValue;

	struct Node
	{
		Node* Next{ nullptr };
		TKey Key;
		TValue Value;
	};

	uint32_t HashSize{ 0 };
	Node** HashTable{ nullptr };
	uint32_t ItemCount{ 0 };
};

template <class TKey, class TValue>
struct RefMapInternals
{
	using KeyType = TKey;
	using ValueType = TValue;

	struct Node
	{
		Node* Next{ nullptr };
		TKey Key;
		TValue Value;
	};

	uint32_t ItemCount{ 0 };
	uint32_t HashSize{ 0 };
	Node** HashTable{ nullptr };
};

template <class TInternals>
class MapBase : private TInternals
{
public:
	using KeyType = TInternals::KeyType;
	using ValueType = TInternals::ValueType;
	using Node = TInternals::Node;

	class Iterator
	{
	public:
		Iterator(MapBase& map)
			: CurrentNode(map.HashTable), NodeListEnd(map.HashTable + map.HashSize), Element(nullptr)
		{
			while (CurrentNode < NodeListEnd && *CurrentNode == nullptr) {
				CurrentNode++;
			}

			if (CurrentNode < NodeListEnd && *CurrentNode) {
				Element = *CurrentNode;
			}
		}
			
		Iterator(MapBase& map, Node** node, Node* element)
			: CurrentNode(node), NodeListEnd(map.HashTable + map.HashSize), Element(element)
		{}

		Iterator operator ++ ()
		{
			Iterator it(*this);

			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return it;
		}

		Iterator& operator ++ (int)
		{
			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return *this;
		}

		bool operator == (Iterator const& it)
		{
			return it.CurrentNode == CurrentNode && it.Element == Element;
		}

		bool operator != (Iterator const& it)
		{
			return it.CurrentNode != CurrentNode || it.Element != Element;
		}

		KeyType & Key () const
		{
			return Element->Key;
		}

		ValueType& Value () const
		{
			return Element->Value;
		}

		Node& operator * () const
		{
			return *Element;
		}

		Node& operator -> () const
		{
			return *Element;
		}

	private:
		Node** CurrentNode, ** NodeListEnd;
		Node* Element;
	};

	class ConstIterator
	{
	public:
		ConstIterator(MapBase const& map)
			: CurrentNode(map.HashTable), NodeListEnd(map.HashTable + map.HashSize), Element(nullptr)
		{
			while (CurrentNode < NodeListEnd && *CurrentNode == nullptr) {
				CurrentNode++;
			}

			if (CurrentNode < NodeListEnd && *CurrentNode) {
				Element = *CurrentNode;
			}
		}

		ConstIterator(MapBase const& map, Node* const* node, Node const* element)
			: CurrentNode(node), NodeListEnd(map.HashTable + map.HashSize), Element(element)
		{}

		ConstIterator operator ++ ()
		{
			ConstIterator it(*this);

			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return it;
		}

		ConstIterator& operator ++ (int)
		{
			Element = Element->Next;
			if (Element == nullptr) {
				do {
					CurrentNode++;
				} while (CurrentNode < NodeListEnd && *CurrentNode == nullptr);

				if (CurrentNode < NodeListEnd && *CurrentNode) {
					Element = *CurrentNode;
				}
			}

			return *this;
		}

		bool operator == (ConstIterator const& it)
		{
			return it.CurrentNode == CurrentNode && it.Element == Element;
		}

		bool operator != (ConstIterator const& it)
		{
			return it.CurrentNode != CurrentNode || it.Element != Element;
		}

		KeyType const& Key() const
		{
			return Element->Key;
		}

		ValueType const& Value() const
		{
			return Element->Value;
		}

		Node const& operator * () const
		{
			return *Element;
		}

		Node const& operator -> () const
		{
			return *Element;
		}

	private:
		Node* const * CurrentNode, * const * NodeListEnd;
		Node const* Element;
	};

	MapBase(uint32_t hashSize = 31)
	{
		ResizeHashtable(hashSize);
	}

	MapBase(MapBase<TInternals> const& other)
	{
		ResizeHashtable(other.HashSize);

		for (auto const& pair : other) {
			Insert(pair.Key, pair.Value);
		}
	}

	~MapBase()
	{
		if (this->HashTable) {
			Clear();
			GameFree(this->HashTable);
		}
	}

	MapBase<TInternals>& operator =(MapBase<TInternals> const& other)
	{
		ResizeHashtable(other.HashSize);
		for (auto const& pair : other) {
			Insert(pair.Key, pair.Value);
		}

		return *this;
	}

	void ResizeHashtable(uint32_t hashSize)
	{
		if (this->HashTable) {
			Clear();
		}

		if (this->HashSize != hashSize) {
			if (this->HashTable) {
				GameFree(this->HashTable);
			}

			this->HashSize = hashSize;
			this->HashTable = GameAllocArray<Node*>(this->HashSize);
			memset(this->HashTable, 0, sizeof(Node*) * this->HashSize);
		}
	}

	void Clear()
	{
		this->ItemCount = 0;
		for (uint32_t i = 0; i < this->HashSize; i++) {
			auto item = this->HashTable[i];
			if (item != nullptr) {
				FreeHashChain(item);
				this->HashTable[i] = nullptr;
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

	ValueType* Insert(KeyType const& key, ValueType const& value)
	{
		auto nodeValue = Insert(key);
		*nodeValue = value;
		return nodeValue;
	}

	ValueType* Insert(KeyType const& key)
	{
		auto item = this->HashTable[Hash(key) % this->HashSize];
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
			this->HashTable[Hash(key) % this->HashSize] = node;
		} else {
			last->Next = node;
		}

		this->ItemCount++;
		return &node->Value;
	}

	ValueType* insert(std::pair<KeyType, ValueType> const& v)
	{
		auto nodeValue = Insert(v.first);
		*nodeValue = v.second;
		return nodeValue;
	}

	ValueType* Find(KeyType const& key) const
	{
		auto item = this->HashTable[Hash(key) % this->HashSize];
		while (item != nullptr) {
			if (key == item->Key) {
				return &item->Value;
			}

			item = item->Next;
		}

		return nullptr;
	}

	KeyType* FindByValue(ValueType const& value) const
	{
		for (uint32_t bucket = 0; bucket < this->HashSize; bucket++) {
			Node* item = this->HashTable[bucket];
			while (item != nullptr) {
				if (value == item->Value) {
					return &item->Key;
				}

				item = item->Next;
			}
		}

		return nullptr;
	}

	ConstIterator FindIterator(KeyType const& key) const
	{
		auto slot = Hash(key) % this->HashSize;
		auto item = this->HashTable[slot];
		while (item != nullptr) {
			if (key == item->Key) {
				return ConstIterator(*this, this->HashTable + slot, item);
			}

			item = item->Next;
		}

		return end();
	}

	Iterator FindIterator(KeyType const& key)
	{
		auto slot = Hash(key) % this->HashSize;
		auto item = this->HashTable[slot];
		while (item != nullptr) {
			if (key == item->Key) {
				return Iterator(*this, this->HashTable + slot, item);
			}

			item = item->Next;
		}

		return end();
	}

	Iterator begin()
	{
		return Iterator(*this);
	}

	Iterator end()
	{
		return Iterator(*this, this->HashTable + this->HashSize, nullptr);
	}

	ConstIterator begin() const
	{
		return ConstIterator(*this);
	}

	ConstIterator end() const
	{
		return ConstIterator(*this, this->HashTable + this->HashSize, nullptr);
	}

	inline uint32_t Count() const
	{
		return this->ItemCount;
	}
};

template <class TKey, class TValue>
using Map = MapBase<MapInternals<TKey, TValue>>;

template <class TKey, class TValue>
using RefMap = MapBase<RefMapInternals<TKey, TValue>>;


template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
struct CompactSet
{
	T* Buf{ nullptr };
	uint32_t Capacity{ 0 };
	uint32_t Size{ 0 };

	inline CompactSet() {}

	CompactSet(CompactSet const& other)
	{
		Reallocate(other.Size);
		Size = other.Size;
		for (uint32_t i = 0; i < other.Size; i++) {
			new (Buf + i) T(other.Buf[i]);
		}
	}

	~CompactSet()
	{
		if (Buf) {
			Clear();
			FreeBuffer(Buf);
		}
	}

	CompactSet& operator = (CompactSet const& other)
	{
		Clear();
		Reallocate(other.Size);
		Size = other.Size;
		for (uint32_t i = 0; i < other.Size; i++) {
			new (Buf + i) T(other.Buf[i]);
		}
		return *this;
	}

	inline T const& operator [] (uint32_t index) const
	{
		return Buf[index];
	}

	inline T& operator [] (uint32_t index)
	{
		return Buf[index];
	}

	void FreeBuffer(void* buf)
	{
		if (StoreSize) {
			if (buf != nullptr) {
				Allocator::Free((void*)((std::ptrdiff_t)buf - 8));
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
			} else {
				Buf = Allocator::template New<T>(newCapacity);
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
			new (Buf + i) T(oldBuf[i]);
		}

		for (uint32_t i = 0; i < Size; i++) {
			oldBuf[i].~T();
		}

		FreeBuffer(oldBuf);
	}

	void Remove(uint32_t index)
	{
		assert(index < Size);

		for (auto i = index; i < Size - 1; i++) {
			Buf[i] = Buf[i + 1];
		}

		Buf[Size - 1].~T();
		Size--;
	}

	void Clear()
	{
		for (uint32_t i = 0; i < Size; i++) {
			Buf[i].~T();
		}

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

template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
struct Set : public CompactSet<T, Allocator, StoreSize>
{
	uint64_t CapacityIncrementSize{ 0 };

	uint32_t CapacityIncrement() const
	{
		if (CapacityIncrementSize != 0) {
			return this->Capacity + (uint32_t)CapacityIncrementSize;
		} else if (this->Capacity > 0) {
			return 2 * this->Capacity;
		} else {
			return 1;
		}
	}

	void Add(T const& value)
	{
		if (this->Capacity <= this->Size) {
			this->Reallocate(CapacityIncrement());
		}

		new (&this->Buf[this->Size++]) T(value);
	}

	void InsertAt(uint32_t index, T const& value)
	{
		if (this->Capacity <= this->Size) {
			this->Reallocate(CapacityIncrement());
		}

		for (auto i = this->Size; i > index; i--) {
			this->Buf[i] = this->Buf[i - 1];
		}

		this->Buf[index] = value;
		this->Size++;
	}
};

template <class T, class Allocator = GameMemoryAllocator>
struct PrimitiveSmallSet : public CompactSet<T, Allocator, false>
{
	virtual ~PrimitiveSmallSet() {}

	uint32_t CapacityIncrement() const
	{
		if (this->Capacity > 0) {
			return 2 * this->Capacity;
		} else {
			return 1;
		}
	}

	void Add(T const& value)
	{
		if (this->Capacity <= this->Size) {
			this->Reallocate(CapacityIncrement());
		}

		new (&this->Buf[this->Size++]) T(value);
	}
};

template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
struct ObjectSet : public Set<T, Allocator, StoreSize>
{
	virtual ~ObjectSet() {}
};

template <class T, class Allocator = GameMemoryAllocator>
struct PrimitiveSet : public ObjectSet<T, Allocator, false>
{
};

template <class T, class Allocator = GameMemoryAllocator>
struct CompactObjectSet : public ObjectSet<T, Allocator, true>
{
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

template <class T>
struct Array
{
	void * VMT{ nullptr };
	T * Buf{ nullptr };
	uint32_t Capacity{ 0 };
	uint32_t Size{ 0 };
	uint32_t NumUsed{ 0 };
	uint32_t GrowSize{ 1 };

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
		NumUsed = a.NumUsed;
		GrowSize = a.GrowSize;
		Clear();

		if (a.Size > 0) {
			Reallocate(a.Size);
			Size = a.Size;
			for (uint32_t i = 0; i < Size; i++) {
				new (Buf + i) T(a[i]);
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
		for (uint32_t i = 0; i < Size; i++) {
			Buf[i].~T();
		}

		Size = 0;
	}

	void Reallocate(uint32_t newCapacity)
	{
		auto newBuf = GameAllocArray<T>(newCapacity);
		for (uint32_t i = 0; i < std::min(Size, newCapacity); i++) {
			new (newBuf + i) T(Buf[i]);
		}

		if (Buf != nullptr) {
			GameFree(Buf);
		}

		Buf = newBuf;
		Capacity = newCapacity;
	}

	void Add(T const & val)
	{
		if (Capacity <= Size) {
			Reallocate(CapacityIncrement());
		}

		new (&Buf[Size++]) T(val);
	}

	void Remove(uint32_t index)
	{
		assert(index < Size);

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

END_SE()