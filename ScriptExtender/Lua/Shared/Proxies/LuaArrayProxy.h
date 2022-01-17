#pragma once

#include <span>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_SE()

BY_VAL(uint8_t);
BY_VAL(int16_t);
BY_VAL(uint16_t);
BY_VAL(int32_t);
BY_VAL(uint32_t);
BY_VAL(int64_t);
BY_VAL(uint64_t);
BY_VAL(float);
BY_VAL(double);
BY_VAL(bool);
BY_VAL(ComponentHandle);
BY_VAL(EntityHandle);
BY_VAL(FixedString);
BY_VAL(STDString);
BY_VAL(STDWString);
BY_VAL(Path);
BY_VAL(NetId);
BY_VAL(UserId);
BY_VAL(glm::ivec2);
BY_VAL(glm::vec2);
BY_VAL(glm::vec3);
BY_VAL(glm::vec4);
BY_VAL(glm::mat3);
BY_VAL(TypeInformationRef);

END_SE()

BEGIN_NS(lua)

LifetimeHolder GetCurrentLifetime();

class ArrayProxyImplBase
{
public:
	inline virtual ~ArrayProxyImplBase() {};
	virtual TypeInformation const& GetType() const = 0;
	virtual void* GetRaw() = 0;
	virtual bool GetElement(lua_State* L, unsigned arrayIndex) = 0;
	virtual bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) = 0;
	virtual int Next(lua_State* L, int key) = 0;
	virtual unsigned Length() = 0;
};


template <class TArray, class TElement>
class ArrayProxyByRefImpl : public ArrayProxyImplBase
{
public:
	ArrayProxyByRefImpl(LifetimeHolder const& lifetime, TArray* obj)
		: object_(obj), lifetime_(lifetime)
	{
		assert(obj != nullptr);
	}
		
	~ArrayProxyByRefImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	TypeInformation const& GetType() const override
	{
		return GetTypeInfo<TElement>();
	}

	bool GetElement(lua_State* L, unsigned arrayIndex) override
	{
		if (arrayIndex > 0 && arrayIndex <= (int)object_->size()) {
			MakeObjectRef(L, lifetime_, &(*object_)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) override
	{
		// Appending/swapping elements to by-ref arrays not supported for now
		return false;
	}

	unsigned Length() override
	{
		return (unsigned)object_->size();
	}

	int Next(lua_State* L, int key) override
	{
		if (key >= 0 && key < (int)object_->size()) {
			push(L, ++key);
			MakeObjectRef(L, lifetime_, &(*object_)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}

private:
	TArray* object_;
	LifetimeHolder lifetime_;
};


template <class TArray, class TElement>
class ArrayProxyByValImpl : public ArrayProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<TElement>, "ArrayProxyByValImpl template parameter should not be a pointer type!");

	ArrayProxyByValImpl(LifetimeHolder const& lifetime, TArray* obj)
		: object_(obj), lifetime_(lifetime)
	{
		assert(obj != nullptr);
	}
		
	~ArrayProxyByValImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	TypeInformation const& GetType() const override
	{
		return GetTypeInfo<TElement>();
	}

	bool GetElement(lua_State* L, unsigned arrayIndex) override
	{
		if (arrayIndex > 0 && arrayIndex <= object_->size()) {
			LuaWrite(L, (*object_)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) override
	{
		if (arrayIndex > 0 && arrayIndex <= object_->size()) {
			lua_pushvalue(L, luaIndex);
			LuaRead(L, (*object_)[arrayIndex - 1]);
			lua_pop(L, 1);
			return true;
		} else if (arrayIndex == object_->size() + 1) {
			TElement val;
			lua_pushvalue(L, luaIndex);
			LuaRead(L, val);
			lua_pop(L, 1);

			object_->push_back(val);
			return true;
		} else {
			return false;
		}
	}

	unsigned Length() override
	{
		return (unsigned)object_->size();
	}

	int Next(lua_State* L, int key) override
	{
		if (key >= 0 && key < (int)object_->size()) {
			push(L, ++key);
			LuaWrite(L, (*object_)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}

private:
	TArray* object_;
	LifetimeHolder lifetime_;
};


template <class TArray, class TElement>
class FixedSizeArrayProxyByRefImpl : public ArrayProxyImplBase
{
public:
	FixedSizeArrayProxyByRefImpl(LifetimeHolder const& lifetime, TArray* obj)
		: object_(obj), lifetime_(lifetime)
	{
		assert(obj != nullptr);
	}
		
	~FixedSizeArrayProxyByRefImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	TypeInformation const& GetType() const override
	{
		return GetTypeInfo<TElement>();
	}

	bool GetElement(lua_State* L, unsigned arrayIndex) override
	{
		if (arrayIndex > 0 && arrayIndex <= (int)object_->size()) {
			MakeObjectRef(L, lifetime_, &(*object_)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) override
	{
		// Appending/swapping elements to by-ref arrays not supported for now
		return false;
	}

	unsigned Length() override
	{
		return (unsigned)object_->size();
	}

	int Next(lua_State* L, int key) override
	{
		if (key >= 0 && key < (int)object_->size()) {
			push(L, ++key);
			MakeObjectRef(L, lifetime_, &(*object_)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}

private:
	TArray* object_;
	LifetimeHolder lifetime_;
};


template <class TArray, class TElement>
class FixedSizeArrayProxyByValImpl : public ArrayProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<TElement>, "FixedSizeArrayProxyByValImpl template parameter should not be a pointer type!");

	FixedSizeArrayProxyByValImpl(LifetimeHolder const& lifetime, TArray* obj)
		: object_(obj), lifetime_(lifetime)
	{
		assert(obj != nullptr);
	}
		
	~FixedSizeArrayProxyByValImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	TypeInformation const& GetType() const override
	{
		return GetTypeInfo<TElement>();
	}

	bool GetElement(lua_State* L, unsigned arrayIndex) override
	{
		if (arrayIndex > 0 && arrayIndex <= object_->size()) {
			LuaWrite(L, (*object_)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, unsigned arrayIndex, int luaIndex) override
	{
		if (arrayIndex > 0 && arrayIndex <= object_->size()) {
			lua_pushvalue(L, luaIndex);
			LuaRead(L, (*object_)[arrayIndex - 1]);
			lua_pop(L, 1);
			return true;
		} else {
			// Cannot append elements to a gsl::span
			return false;
		}
	}

	unsigned Length() override
	{
		return (unsigned)object_->size();
	}

	int Next(lua_State* L, int key) override
	{
		if (key >= 0 && key < (int)object_->size()) {
			push(L, ++key);
			LuaWrite(L, (*object_)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}

private:
	TArray* object_;
	LifetimeHolder lifetime_;
};


class ArrayProxy : private Userdata<ArrayProxy>, public Indexable, public NewIndexable,
	public Lengthable, public Iterable, public Stringifiable, public GarbageCollected, public EqualityComparable
{
public:
	static char const * const MetatableName;

	template <class TImpl, class... Args>
	inline static TImpl* MakeImplByRef(lua_State* L, LifetimeHolder const& lifetime, Args... args)
	{
		auto self = NewWithExtraData(L, sizeof(TImpl), lifetime);
		return new (self->GetImpl()) TImpl(lifetime, args...);
	}

	template <class T>
	inline static auto MakeByRef(lua_State* L, Array<T>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<ArrayProxyByRefImpl<Array<T>, T>>(L, lifetime, object);
	}

	template <class T>
	inline static auto MakeByRef(lua_State* L, Vector<T>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<ArrayProxyByRefImpl<Vector<T>, T>>(L, lifetime, object);
	}

	template <class T>
	inline static auto MakeByRef(lua_State* L, std::span<T>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<FixedSizeArrayProxyByRefImpl<std::span<T>, T>>(L, lifetime, object);
	}

	template <class T, class Allocator, bool StoreSize>
	inline static auto MakeByRef(lua_State* L, ObjectSet<T, Allocator, StoreSize>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<ArrayProxyByRefImpl<ObjectSet<T, Allocator, StoreSize>, T>>(L, lifetime, object);
	}

	template <class T, int Size>
	inline static auto MakeByRef(lua_State* L, std::array<T, Size>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<FixedSizeArrayProxyByRefImpl<std::array<T, Size>, T>>(L, lifetime, object);
	}

	template <class T>
	inline static auto MakeByVal(lua_State* L, Array<T>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<ArrayProxyByValImpl<Array<T>, T>>(L, lifetime, object);
	}

	template <class T>
	inline static auto MakeByVal(lua_State* L, Vector<T>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<ArrayProxyByValImpl<Vector<T>, T>>(L, lifetime, object);
	}

	template <class T>
	inline static auto MakeByVal(lua_State* L, std::span<T>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<FixedSizeArrayProxyByValImpl<std::span<T>, T>>(L, lifetime, object);
	}

	template <class T, class Allocator, bool StoreSize>
	inline static auto MakeByVal(lua_State* L, ObjectSet<T, Allocator, StoreSize>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<ArrayProxyByValImpl<ObjectSet<T, Allocator, StoreSize>, T>>(L, lifetime, object);
	}

	template <class T, int Size>
	inline static auto MakeByVal(lua_State* L, std::array<T, Size>* object, LifetimeHolder const& lifetime)
	{
		return MakeImplByRef<FixedSizeArrayProxyByValImpl<std::array<T, Size>, T>>(L, lifetime, object);
	}

	inline ArrayProxyImplBase* GetImpl()
	{
		return reinterpret_cast<ArrayProxyImplBase*>(this + 1);
	}

	inline bool IsAlive() const
	{
		return lifetime_.IsAlive();
	}

	template <class T>
	T* Get()
	{
		if (!lifetime_.IsAlive()) {
			return nullptr;
		}

		if (GetTypeInfo<T>().TypeName == GetImpl()->GetType().TypeName) {
			return reinterpret_cast<T*>(GetImpl()->GetRaw());
		} else {
			return nullptr;
		}
	}

private:
	LifetimeReference lifetime_;

	ArrayProxy(LifetimeHolder const& lifetime)
		: lifetime_(lifetime)
	{}

	~ArrayProxy()
	{
		GetImpl()->~ArrayProxyImplBase();
	}

protected:
	friend Userdata<ArrayProxy>;

	int Index(lua_State* L);
	int NewIndex(lua_State* L);
	int Length(lua_State* L);
	int Next(lua_State* L);
	int ToString(lua_State* L);
	int GC(lua_State* L);
	bool IsEqual(lua_State* L, ArrayProxy* other);
};

template <class T>
struct IsArrayLike { static constexpr bool Value = false; };

template <class T>
struct IsArrayLike<Array<T>> { static constexpr bool Value = true; using TElement = T; };

template <class T>
struct IsArrayLike<Vector<T>> { static constexpr bool Value = true; using TElement = T; };

template <class T>
struct IsArrayLike<std::span<T>> { static constexpr bool Value = true; using TElement = T; };

template <class T, class Allocator, bool StoreSize>
struct IsArrayLike<ObjectSet<T, Allocator, StoreSize>> { static constexpr bool Value = true; using TElement = T; };

template <class T, size_t Size>
struct IsArrayLike<std::array<T, Size>> { static constexpr bool Value = true; using TElement = T; };

template <class T>
inline void push_array_ref_proxy(lua_State* L, LifetimeHolder const& lifetime, T* v)
{
	ArrayProxy::MakeByRef<T>(L, v, lifetime);
}

template <class T>
inline T* checked_get_array_proxy(lua_State* L, int index)
{
	auto proxy = Userdata<ArrayProxy>::CheckUserData(L, index);
	auto const& typeName = GetTypeInfo<T>().TypeName;
	if (proxy->GetImpl()->GetType().TypeName == typeName) {
		auto obj = proxy->Get<T>();
		if (obj == nullptr) {
			luaL_error(L, "Argument %d: got Array<%s> whose lifetime has expired", index, typeName.GetString());
			return nullptr;
		} else {
			return obj;
		}
	} else {
		luaL_error(L, "Argument %d: expected Array<%s>, got Array<%s>", index, typeName.GetString(), proxy->GetImpl()->GetType().TypeName.GetString());
		return nullptr;
	}
}

END_NS()
