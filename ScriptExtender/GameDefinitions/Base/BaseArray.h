#pragma once

#include <GameDefinitions/Base/BaseTypes.h>
#include <GameDefinitions/Base/BaseMemory.h>
#include <span>

BEGIN_SE()

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

	bool operator == (ContiguousIterator const& it) const
	{
		return it.ptr_ == ptr_;
	}

	bool operator != (ContiguousIterator const& it) const
	{
		return it.ptr_ != ptr_;
	}

	T& operator * () const
	{
		return *ptr_;
	}

	T* operator -> () const
	{
		return ptr_;
	}

	T* get () const
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

	bool operator == (ContiguousConstIterator const& it) const
	{
		return it.ptr_ == ptr_;
	}

	bool operator != (ContiguousConstIterator const& it) const
	{
		return it.ptr_ != ptr_;
	}

	T const& operator * () const
	{
		return *ptr_;
	}

	T const* operator -> () const
	{
		return ptr_;
	}

	T const* get() const
	{
		return ptr_;
	}

private:
	T const* ptr_;
};

template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
class CompactSet
{
public:
	using value_type = T;
	using reference = T&;
	using const_reference = T const&;
	using iterator = ContiguousIterator<T>;
	using const_iterator = ContiguousConstIterator<T>;
	using difference_type = int32_t;
	using size_type = uint32_t;

	inline CompactSet() {}

	CompactSet(CompactSet const& other)
	{
		reallocate(other.size_);
		size_ = other.size_;
		for (size_type i = 0; i < other.size_; i++) {
			new (buf_ + i) T(other.buf_[i]);
		}
	}

	CompactSet(CompactSet && other)
		: buf_(other.buf_),
		capacity_(other.capacity_),
		size_(other.size_)
	{
		other.buf_ = nullptr;
		other.capacity_ = 0;
		other.size_ = 0;
	}

	~CompactSet()
	{
		if (buf_) {
			FreeBuffer(buf_, capacity_);
		}
	}

	CompactSet& operator = (CompactSet const& other)
	{
		clear();
		reallocate(other.size_);
		size_ = other.size_;
		for (size_type i = 0; i < other.size_; i++) {
			buf_[i] = other.buf_[i];
		}
		return *this;
	}

	CompactSet& operator = (CompactSet && other)
	{
		buf_ = other.buf_;
		capacity_ = other.capacity_;
		size_ = other.size_;
		other.buf_ = nullptr;
		other.capacity_ = 0;
		other.size_ = 0;
		return *this;
	}

	ContiguousIterator<T> begin()
	{
		return ContiguousIterator<T>(buf_);
	}

	ContiguousConstIterator<T> begin() const
	{
		return ContiguousConstIterator<T>(buf_);
	}

	ContiguousIterator<T> end()
	{
		return ContiguousIterator<T>(buf_ + size_);
	}

	ContiguousConstIterator<T> end() const
	{
		return ContiguousConstIterator<T>(buf_ + size_);
	}

	ContiguousConstIterator<T> cbegin() const
	{
		return ContiguousConstIterator<T>(buf_);
	}

	ContiguousConstIterator<T> cend() const
	{
		return ContiguousConstIterator<T>(buf_ + size_);
	}

	size_type size() const
	{
		return size_;
	}

	bool empty() const
	{
		return size_ == 0;
	}

	inline T const& operator [] (size_type index) const
	{
		return buf_[index];
	}

	inline T& operator [] (size_type index)
	{
		return buf_[index];
	}

	void reallocate(size_type newCapacity)
	{
		auto oldBuf = buf_;
		auto oldCapacity = capacity_;
		RawReallocate(newCapacity);

		size_ = std::min(size_, newCapacity);
		for (size_type i = 0; i < size_; i++) {
			buf_[i] = oldBuf[i];
		}

		FreeBuffer(oldBuf, oldCapacity);
	}

	void resize(size_type newSize)
	{
		if (capacity_ < newSize) {
			reallocate(newSize);
		}

		size_ = newSize;
	}

	void ordered_remove_at(size_type index)
	{
		assert(index < size_);
		buf_[index] = T();

		for (auto i = index; i < size_ - 1; i++) {
			buf_[i] = buf_[i + 1];
		}

		size_--;
	}

	void remove_at(size_type index)
	{
		assert(index < size_);

		if (index < size_ - 1) {
			buf_[index] = buf_[size_ - 1];
			buf_[size_ - 1] = T();
		} else {
			buf_[index] = T();
		}

		size_--;
	}

	bool remove(T const& value)
	{
		for (size_type i = 0; i < size_; i++) {
			if (buf_[i] == value) {
				remove_at(i);
				return true;
			}
		}

		return false;
	}

	void pop_back()
	{
		assert(size_ > 0);
		buf_[--size_] = T();
	}
	
	void clear()
	{
		size_ = 0;
	}

protected:
	T* buf_{ nullptr };
	size_type capacity_{ 0 };
	size_type size_{ 0 };

	void FreeBuffer(T* buf, size_t oldCapacity)
	{
		if (buf != nullptr) {
			for (size_t i = 0; i < oldCapacity; i++) {
				buf[i].~T();
			}

			if (StoreSize) {
				Allocator::Free((void*)((std::ptrdiff_t)buf - 8));
			} else {
				Allocator::Free(buf);
			}
		}
	}

	void RawReallocate(size_type newCapacity)
	{
		if (newCapacity > 0) {
			if (StoreSize) {
				auto newBuf = Allocator::Alloc(newCapacity * sizeof(T) + 8);
				*(uint64_t*)newBuf = newCapacity;

				buf_ = (T*)((std::ptrdiff_t)newBuf + 8);
			} else {
				buf_ = (T*)Allocator::Alloc(newCapacity * sizeof(T));
			}

			for (size_type i = 0; i < newCapacity; i++) {
				new (buf_ + i) T();
			}
		} else {
			buf_ = nullptr;
		}

		capacity_ = newCapacity;
	}
};

template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
class Set : public CompactSet<T, Allocator, StoreSize>
{
public:
	CompactSet<T, Allocator, StoreSize>::size_type capacity_increment() const
	{
		if (capacityIncrementSize_ != 0) {
			return this->capacity_ + (typename CompactSet<T, Allocator, StoreSize>::size_type)capacityIncrementSize_;
		} else if (this->capacity_ > 0) {
			return 2 * this->capacity_;
		} else {
			return 1;
		}
	}

	void push_back(T const& value)
	{
		if (this->capacity_ <= this->size_) {
			this->reallocate(capacity_increment());
		}

		this->buf_[this->size_++] = value;
	}

	void push_back(T && value)
	{
		if (this->capacity_ <= this->size_) {
			this->reallocate(capacity_increment());
		}

		this->buf_[this->size_++] = std::move(value);
	}

	void insert(CompactSet<T, Allocator, StoreSize>::size_type index, T const& value)
	{
		if (this->capacity_ <= this->size_) {
			this->reallocate(capacity_increment());
		}

		for (auto i = this->size_; i > index; i--) {
			this->buf_[i] = this->buf_[i - 1];
		}

		this->buf_[index] = value;
		this->size_++;
	}

private:
	uint64_t capacityIncrementSize_{ 0 };
};

template <class T, class Allocator = GameMemoryAllocator>
class PrimitiveSmallSet : public CompactSet<T, Allocator, false>
{
public:
	virtual ~PrimitiveSmallSet() {}

	CompactSet<T, Allocator, false>::size_type capacity_increment() const
	{
		if (this->capacity_ > 0) {
			return 2 * this->capacity_;
		} else {
			return 1;
		}
	}

	void push_back(T const& value)
	{
		if (this->capacity_ <= this->size_) {
			this->reallocate(capacity_increment());
		}

		this->buf_[this->size_++] = value;
	}

	void push_back(T && value)
	{
		if (this->capacity_ <= this->size_) {
			this->reallocate(capacity_increment());
		}

		this->buf_[this->size_++] = std::move(value);
	}
};

template <class T, class Allocator = GameMemoryAllocator, bool StoreSize = false>
class ObjectSet : public Set<T, Allocator, StoreSize>
{
public:
	virtual ~ObjectSet() {}
};

template <class T, class Allocator = GameMemoryAllocator>
class CompactObjectSet : public ObjectSet<T, Allocator, true>
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
class Array
{
public:
	using value_type = T;
	using reference = T&;
	using const_reference = T const&;
	using iterator = ContiguousIterator<T>;
	using const_iterator = ContiguousConstIterator<T>;
	using difference_type = int32_t;
	using size_type = uint32_t;

	inline Array() {}

	Array(Array const& o)
	{
		copy_from(o);
	}

	Array(Array && o)
		: buf_(o.buf_),
		capacity_(o.capacity_),
		size_(o.size_),
		numUsed_(o.numUsed_),
		growSize_(o.growSize_)
	{
		o.buf_ = nullptr;
		o.capacity_ = 0;
		o.size_ = 0;
		o.numUsed_ = 0;
	}

	virtual ~Array()
	{
		if (buf_) {
			GameDeleteArray<T>(buf_, capacity_);
		}
	}

	Array& operator =(Array const& o)
	{
		copy_from(o);
		return *this;
	}

	Array& operator =(Array && o)
	{
		buf_ = o.buf_;
		capacity_ = o.capacity_;
		size_ = o.size_;
		numUsed_ = o.numUsed_;
		growSize_ = o.growSize_;

		o.buf_ = nullptr;
		o.capacity_ = 0;
		o.size_ = 0;
		o.numUsed_ = 0;
		return *this;
	}

	ContiguousIterator<T> begin()
	{
		return ContiguousIterator<T>(buf_);
	}

	ContiguousConstIterator<T> begin() const
	{
		return ContiguousConstIterator<T>(buf_);
	}

	ContiguousIterator<T> end()
	{
		return ContiguousIterator<T>(buf_ + size_);
	}

	ContiguousConstIterator<T> end() const
	{
		return ContiguousConstIterator<T>(buf_ + size_);
	}

	ContiguousConstIterator<T> cbegin() const
	{
		return ContiguousConstIterator<T>(buf_);
	}

	ContiguousConstIterator<T> cend() const
	{
		return ContiguousConstIterator<T>(buf_ + size_);
	}

	size_type size() const
	{
		return size_;
	}

	bool empty() const
	{
		return size_ == 0;
	}

	size_type capacity() const
	{
		return capacity_;
	}

	size_type grow_size() const
	{
		return growSize_;
	}

	void set_grow_size(uint32_t size)
	{
		growSize_ = size;
	}

	inline T const & operator [] (size_type index) const
	{
		return buf_[index];
	}

	inline T & operator [] (size_type index)
	{
		return buf_[index];
	}

	void reallocate(size_type newCapacity)
	{
		auto newBuf = GameAllocArray<T>(newCapacity);
		size_ = std::min(size_, capacity_);
		for (size_type i = 0; i < std::min(size_, newCapacity); i++) {
			newBuf[i] = buf_[i];
		}

		if (buf_ != nullptr) {
			GameDeleteArray<T>(buf_, capacity_);
		}

		buf_ = newBuf;
		capacity_ = newCapacity;
	}

	void resize(size_type newSize)
	{
		if (capacity_ < newSize) {
			reallocate(newSize);
		}

		size_ = newSize;
	}

	void remove(uint32_t index)
	{
		assert(index < size_);
		buf_[index] = T();

		for (auto i = index; i < size_ - 1; i++) {
			buf_[i] = buf_[i + 1];
		}

		size_--;
	}

	void clear()
	{
		size_ = 0;
	}

	void push_back(T const& value)
	{
		if (capacity_ <= size_) {
			reallocate(capacity_increment());
		}

		buf_[size_++] = value;
	}

	void push_back(T&& value)
	{
		if (capacity_ <= size_) {
			reallocate(capacity_increment());
		}

		buf_[size_++] = std::move(value);
	}

	void insert(size_type index, T const& value)
	{
		if (capacity_ <= size_) {
			reallocate(capacity_increment());
		}

		for (auto i = size_; i > index; i--) {
			buf_[i] = buf_[i - 1];
		}

		buf_[index] = value;
		size_++;
	}

private:
	T* buf_{ nullptr };
	uint32_t capacity_{ 0 };
	uint32_t size_{ 0 };
	uint32_t numUsed_{ 0 };
	uint32_t growSize_{ 1 };

	void copy_from(Array const& a)
	{
		numUsed_ = a.numUsed_;
		growSize_ = a.growSize_;

		reallocate(a.size_);
		size_ = a.size_;
		for (uint32_t i = 0; i < size_; i++) {
			buf_[i] = a.buf_[i];
		}
	}

	size_type capacity_increment() const
	{
		if (capacity_ > 0) {
			return 2 * capacity_;
		} else {
			return 1;
		}
	}
};

template <class T>
class Queue
{
public:
	using value_type = T;
	using reference = T&;
	using const_reference = T const&;
	using difference_type = int32_t;
	using size_type = int32_t;

	inline Queue() {}

	Queue(Queue const& o)
	{
		copy_from(o);
	}

	Queue(Queue&& o)
		: buf_(o.buf_),
		capacity_(o.capacity_),
		size_(o.size_),
		readIndex_(o.readIndex_),
		writeIndex_(o.writeIndex_)
	{
		o.buf_ = nullptr;
		o.capacity_ = 0;
		o.readIndex_ = -1;
		o.writeIndex_ = 0;
	}

	virtual ~Queue()
	{
		if (buf_) {
			GameDeleteArray<T>(buf_, capacity_);
		}
	}

	Queue& operator =(Queue const& o)
	{
		copy_from(o);
		return *this;
	}

	Queue& operator =(Queue&& o)
	{
		buf_ = o.buf_;
		capacity_ = o.capacity_;
		size_ = o.size_;
		readIndex_ = o.readIndex_;
		writeIndex_ = o.writeIndex_;

		o.buf_ = nullptr;
		o.capacity_ = 0;
		o.size_ = 0;
		o.readIndex_ = -1;
		o.writeIndex_ = 0;
		return *this;
	}

	size_type size() const
	{
		return size_;
	}

	bool empty() const
	{
		return size_ == 0;
	}

	size_type capacity() const
	{
		return capacity_;
	}

	void push(T const& value)
	{
		if (size_ == 0 || readIndex_ == writeIndex_) {
			resize(capacity_increment());
		}

		if (size_ == 0) {
			buf_[0] = value;
			size_ = 1;
			writeIndex_ = 1;
			readIndex_ = 0;
		} else {
			buf_[writeIndex_] = value;
			size_++;
			if (++writeIndex_ == capacity_) {
				writeIndex_ = 0;
			}
		}
	}

	T pop()
	{
		assert(size_ > 0 && readIndex_ != writeIndex_);

		T val = buf_[readIndex_];
		size_--;
		if (++readIndex_ == capacity_) {
			readIndex_ = 0;
		}

		if (readIndex_ == writeIndex_) {
			readIndex_ = -1;
			writeIndex_ = 0;
		}

		return val;
	}

	void resize(size_type newCapacity)
	{
		auto newBuf = GameAllocArray<T>(newCapacity);

		if (readIndex_ < writeIndex_) {
			auto wr = 0;
			for (size_type i = readIndex_ + 1; i < writeIndex_; i++, wr++) {
				newBuf[wr] = buf_[i];
			}
		} else {
			auto wr = 0;
			for (size_type i = readIndex_ + 1; i < capacity_; i++, wr++) {
				newBuf[wr] = buf_[i];
			}

			for (size_type i = 0; i < writeIndex_; i++, wr++) {
				newBuf[wr] = buf_[i];
			}
		}

		size_ = std::min(size_, capacity_);
		for (size_type i = 0; i < std::min(size_, newCapacity); i++) {
			newBuf[i] = buf_[i];
		}

		if (buf_ != nullptr) {
			GameDeleteArray<T>(buf_, capacity_);
		}

		buf_ = newBuf;
		capacity_ = newCapacity;
		readIndex_ = 0;
		writeIndex_ = size_;
	}

	void clear()
	{
		size_ = 0; 
		readIndex_ = 0;
		writeIndex_ = 0;
	}

private:
	T* buf_{ nullptr };
	int32_t capacity_{ 0 };
	int32_t size_{ 0 };
	int32_t readIndex_{ -1 };
	int32_t writeIndex_{ 0 };

	void copy_from(Queue const& a)
	{
		readIndex_ = a.readIndex_;
		writeIndex_ = a.writeIndex_;

		reallocate(a.capacity_);
		size_ = a.size_;
		for (size_type i = 0; i < capacity_; i++) {
			buf_[i] = a.buf_[i];
		}
	}

	size_type capacity_increment() const
	{
		if (capacity_ > 0) {
			return 2 * capacity_;
		} else {
			return 1;
		}
	}
};

END_SE()