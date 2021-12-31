#pragma once

#define DEBUG_LIFETIMES
#undef TRACE_LIFETIMES

BEGIN_NS(lua)

template <class T, unsigned Size>
class HierarchicalPoolAllocator
{
public:
	static constexpr unsigned PageBits = 64;
	static constexpr unsigned PageShift = 6;

	HierarchicalPoolAllocator()
	{
		pool_ = new T[Size];
		memset(pool_, 0, sizeof(T) * Size);
		memset(l1_, 0xff, sizeof(l1_));
		memset(l2_, 0xff, sizeof(l2_));
		memset(l3_, 0xff, sizeof(l3_));

		auto l1buckets = Size / (PageBits * PageBits);
		if (l1buckets % PageBits) {
			l1_[std::size(l1_) - 1] = 0xffffffffffffffffull << (PageBits - (l1buckets & (PageBits - 1)));
		}
	}

	~HierarchicalPoolAllocator()
	{
		delete [] pool_;
	}

	T* Allocate()
	{
		for (auto i = 0; i < std::size(l1_); i++) {
			unsigned long l1;
			if (_BitScanForward64(&l1, l1_[i])) {
				auto l2off = (i << PageShift) + l1;
				unsigned long l2;
				_BitScanForward64(&l2, l2_[l2off]);
				assert(l2 < PageBits);
				auto l3off = (i << (2 * PageShift)) + (l1 << PageShift) + l2;
				unsigned long l3;
				_BitScanForward64(&l3, l3_[l3off]);
				assert(l3 < PageBits);
				l3_[l3off] &= ~(1ull << l3);
				if (l3_[l3off] == 0) {
					l2_[l2off] &= ~(1ull << l2);
					if (l2_[l2off] == 0) {
						l1_[i] &= ~(1ull << l1);
					}
				}

				auto off = (i << (3 * PageShift)) + (l1 << (2 * PageShift)) + (l2 << PageShift) + l3;
#if defined(TRACE_LIFETIMES)
				INFO("ACQ: off=%d, root=%d, l1=%d, l2=%d, l3=%d, l2off=%d, l3off=%d", off, i, l1, l2, l3, l2off, l3off);
#endif

				auto lifetime = pool_ + off;
#if defined(DEBUG_LIFETIMES)
				if (lifetime->References() > 0) {
					ERR("Attempted to construct a lifetime on a slot that still has references! This is very, very bad!");
				}
#endif

				lifetime->Acquire();
				return lifetime;
			}
		}

		OsiErrorS("Couldn't allocate Lua lifetime - pool is full! This is very, very bad.");
		return nullptr;
	}

	void Free(T* ptr)
	{
		assert(ptr >= pool_ && ptr < pool_ + Size);
		auto index = ptr - pool_;
		auto off = index;
		auto l3 = index & (PageBits - 1);
		index >>= PageShift;
		auto l3off = index;
		auto l2 = index & (PageBits - 1);
		index >>= PageShift;
		auto l2off = index;
		auto l1 = index & (PageBits - 1);
		index >>= PageShift;
		auto l1off = index;

		bool l3set = (l3_[l3off] == 0);
		l3_[l3off] |= 1ull << l3;
		if (l3set) {
			bool l2set = (l2_[l2off] == 0);
			l2_[l2off] |= 1ull << l2;
			if (l2set) {
				l1_[l1off] |= 1ull << l1;
			}
		}

		ptr->Release();
	}

public:
	static_assert((Size % 4096) == 0, "Size must be a multiple of 4096");

	uint64_t l1_[Size / 262144 + ((Size % 262144) ? 1 : 0)];
	uint64_t l2_[Size / 4096];
	uint64_t l3_[Size / 64];
	T* pool_;
};

class LifetimePool;

class Lifetime : public Noncopyable<Lifetime>
{
public:
	Lifetime()
		: isDeleted_(true), isAlive_(false), isInfinite_(false), references_(0)
	{}

	~Lifetime()
	{
		assert(references_ == 0 && !isAlive_);
	}

	void Acquire()
	{
		isDeleted_ = false;
		isAlive_ = true;
		isInfinite_ = false;
		references_ = 0;
#if defined(TRACE_LIFETIMES)
		INFO("[%p] ACQUIRE", this);
#endif
	}

	void Release()
	{
#if defined(DEBUG_LIFETIMES)
		if (isInfinite_) {
			ERR("[%p] Attempted to release a persistent reference. This is very, very bad!", this);
		}

		if (isAlive_) {
			ERR("[%p] Attempted to release an alive lifetime.", this);
		}

		if (isDeleted_) {
			ERR("[%p] Attempted to release a deleted reference. This is very, very bad!", this);
		}
#endif

		assert(references_ == 0 && !isDeleted_);
		isDeleted_ = true;
		isAlive_ = false;
#if defined(TRACE_LIFETIMES)
		INFO("[%p] RELEASE", this);
#endif
	}

	inline bool IsAlive() const
	{
		return isAlive_;
	}

	inline void SetInfinite()
	{
#if defined(TRACE_LIFETIMES)
		INFO("[%p] MAKE GLOBAL", this);
#endif
		assert(!isInfinite_);
		isInfinite_ = true;
	}

	inline void ClearInfinite()
	{
#if defined(TRACE_LIFETIMES)
		INFO("[%p] MAKE GLOBAL", this);
#endif
		assert(isInfinite_);
		isInfinite_ = false;
	}

	inline unsigned References() const
	{
		return references_;
	}

	inline void Kill()
	{
#if defined(DEBUG_LIFETIMES)
		if (isInfinite_) {
			ERR("[%p] Attempted to kill a persistent reference. This is very, very bad!", this);
			return;
		}

		if (!isAlive_) {
			ERR("[%p] Attempted to kill a reference that is not alive.", this);
			return;
		}

		if (isDeleted_ || references_ == 0) {
			ERR("[%p] Attempted to kill a deleted lifetime!", this);
			return;
		}
#endif

#if defined(TRACE_LIFETIMES)
		INFO("[%p] Kill", this);
#endif

		assert(isAlive_);
		isAlive_ = false;
	}

protected:
	friend class LifetimeReference;
	friend class LifetimePin;
	friend class StaticLifetimePin;

	void AddRef()
	{
#if defined(DEBUG_LIFETIMES)
		if (!isAlive_ || isDeleted_) {
			ERR("[%p] IncRef on deleted lifetime?", this);
			return;
		}
#endif

		references_++;
	}

	unsigned DecRef()
	{
		assert(references_ > 0);
		return --references_;
	}

private:
	std::atomic<unsigned> references_;
	bool isDeleted_;
	bool isAlive_;
	bool isInfinite_;
};

class LifetimePool : Noncopyable<LifetimePool>
{
public:
	inline Lifetime* Allocate()
	{
		return pool_.Allocate();
	}

	inline void Release(Lifetime* lifetime)
	{
		assert(lifetime->References() == 0);
		pool_.Free(lifetime);
	}

	inline auto const& GetAllocator() const
	{
		return pool_;
	}

private:
	HierarchicalPoolAllocator<Lifetime, 262144> pool_;
};

class LifetimeHolder
{
public:
	inline LifetimeHolder(LifetimePool& pool, Lifetime* lifetime)
		: lifetime(lifetime), pool(pool)
	{}

	LifetimeHolder(LifetimeHolder&& o) noexcept
		: lifetime(o.lifetime), pool(o.pool)
	{}

	LifetimeHolder(LifetimeHolder const& o)
		: lifetime(o.lifetime), pool(o.pool)
	{}

	Lifetime* lifetime;
	LifetimePool& pool;
};

class LifetimeReference
{
public:
	inline LifetimeReference(LifetimePool& pool, Lifetime* lifetime)
		: lifetime_(lifetime), pool_(pool)
	{
		if (lifetime_) {
			lifetime_->AddRef();
		}
	}

	inline ~LifetimeReference()
	{
		if (lifetime_ && lifetime_->DecRef() == 0) {
			pool_.Release(lifetime_);
		}
	}

	LifetimeReference(LifetimeReference&& o) noexcept
		: lifetime_(o.lifetime_), pool_(o.pool_)
	{
		o.lifetime_ = nullptr;
	}

	LifetimeReference(LifetimeReference const& o)
		: lifetime_(o.lifetime_), pool_(o.pool_)
	{
		if (lifetime_) {
			lifetime_->AddRef();
		}
	}

	LifetimeReference(LifetimeHolder&& o) noexcept
		: lifetime_(o.lifetime), pool_(o.pool)
	{}

	LifetimeReference(LifetimeHolder const& o)
		: lifetime_(o.lifetime), pool_(o.pool)
	{
		if (lifetime_) {
			lifetime_->AddRef();
		}
	}

	LifetimeReference& operator = (LifetimeReference const&) = delete;
	LifetimeReference& operator = (LifetimeReference &&) = delete;

	inline bool IsAlive() const
	{
		return lifetime_ && lifetime_->IsAlive();
	}

	inline LifetimeHolder Get() const
	{
		return LifetimeHolder(pool_, lifetime_);
	}

	inline Lifetime* GetLifetime() const
	{
		return lifetime_;
	}

private:
	Lifetime* lifetime_;
	LifetimePool& pool_;
};

class LifetimeStack
{
public:
	LifetimeStack(LifetimePool& pool)
		: pool_(pool)
	{}

	LifetimePool& Pool()
	{
		return pool_;
	}

	bool IsEmpty() const
	{
		return stack_.empty();
	}

	Lifetime* Push()
	{
		assert(stack_.size() < 0x1000);
		auto lifetime = pool_.Allocate();
		stack_.push_back(LifetimeReference(pool_, lifetime));
		return lifetime;
	}

	void Push(Lifetime* lifetime)
	{
		assert(stack_.size() < 0x1000);
		stack_.push_back(LifetimeReference(pool_, lifetime));
	}

	void PopAndKill()
	{
		assert(!stack_.empty());
		auto lifetime = stack_.rbegin()->GetLifetime();
		if (lifetime != nullptr) {
			lifetime->Kill();
		}
		stack_.pop_back();
	}

	void PopAndKill(Lifetime* lifetime)
	{
		assert(!stack_.empty());
		assert(stack_.rbegin()->GetLifetime() == lifetime);
		if (lifetime != nullptr) {
			lifetime->Kill();
		}
		stack_.pop_back();
	}

	void Pop(Lifetime* lifetime)
	{
		assert(!stack_.empty());
		assert(stack_.rbegin()->GetLifetime() == lifetime);
		stack_.pop_back();
	}

	inline LifetimeHolder GetCurrent() const
	{
		assert(!stack_.empty());
		return stack_.rbegin()->Get();
	}

private:
	LifetimePool& pool_;
	std::vector<LifetimeReference> stack_;
};

class LifetimePin : Noncopyable<LifetimePin>
{
public:
	LifetimePin(LifetimeStack& stack)
		: stack_(stack)
	{
		lifetime_ = stack_.Push();
		if (lifetime_) {
			lifetime_->AddRef();
		}
	}

	~LifetimePin()
	{
		stack_.PopAndKill(lifetime_);
		if (lifetime_ && lifetime_->DecRef() == 0) {
			stack_.Pool().Release(lifetime_);
		}
	}

private:
	LifetimeStack& stack_;
	Lifetime* lifetime_;
};

class StaticLifetimePin : Noncopyable<StaticLifetimePin>
{
public:
	StaticLifetimePin(LifetimeStack& stack, Lifetime* lifetime)
		: stack_(stack), lifetime_(lifetime)
	{
		 stack_.Push(lifetime_);
		 if (lifetime_) {
			 lifetime_->AddRef();
		 }
	}

	~StaticLifetimePin()
	{
		stack_.Pop(lifetime_);
		if (lifetime_ && lifetime_->DecRef() == 0) {
			stack_.Pool().Release(lifetime_);
		}
	}

private:
	LifetimeStack& stack_;
	Lifetime* lifetime_;
};

template <class T>
class LifetimeTrackedObject : public Noncopyable<LifetimeTrackedObject<T>>
{
public:
	inline LifetimeTrackedObject(T* obj, LifetimePool& lifetimePool, Lifetime* lifetime)
		: object_(obj), reference_(lifetimePool, lifetime)
	{}
	
	inline LifetimeTrackedObject(T* obj, LifetimeReference const& lifetime)
		: object_(obj), reference_(lifetime)
	{}
	
	inline LifetimeTrackedObject(T* obj, LifetimeHolder const& lifetime)
		: object_(obj), reference_(lifetime)
	{}

	inline T* Get() const
	{
		if (reference_.IsAlive()) {
			return object_;
		} else {
			return nullptr;
		}
	}

	inline LifetimeHolder GetLifetime() const
	{
		return reference_.Get();
	}

private:
	T* object_;
	LifetimeReference reference_;
};

END_NS()
