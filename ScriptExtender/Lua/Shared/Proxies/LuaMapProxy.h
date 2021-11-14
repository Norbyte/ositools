#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

LifetimeHolder GetCurrentLifetime();

class MapProxyImplBase
{
public:
	inline virtual ~MapProxyImplBase() {};
	virtual char const* GetKeyTypeName() const = 0;
	virtual char const* GetValueTypeName() const = 0;
	virtual void* GetRaw() = 0;
	virtual bool GetValue(lua_State* L, int luaKeyIndex) = 0;
	virtual bool SetValue(lua_State* L, int luaKeyIndex, int luaValueIndex) = 0;
	virtual int Next(lua_State* L, int luaKeyIndex) = 0;
	virtual unsigned Length() = 0;
};

	

template <class TKey, class TValue>
class RefMapByRefProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "RefMap key should be a by-value type!");

	RefMapByRefProxyImpl(LifetimeHolder const& lifetime, RefMap<TKey, TValue> * obj)
		: object_(obj), lifetime_(lifetime)
	{}
		
	~RefMapByRefProxyImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	char const* GetKeyTypeName() const override
	{
		return GetTypeInfo<TKey>();
	}

	char const* GetValueTypeName() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto value = object_->Find(key);
		if (value) {
			MakeObjectRef(L, lifetime_, value);
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, int luaKeyIndex, int luaValueIndex) override
	{
		return false;
	}

	unsigned Length() override
	{
		return object_->Count();
	}

	int Next(lua_State* L, int luaKeyIndex) override
	{
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = object_->begin();
			if (it != object_->end()) {
				LuaWrite(L, it.Key());
				MakeObjectRef(L, lifetime_, &it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = object_->FindIterator(key);
			if (it != object_->end()) {
				it++;
				if (it != object_->end()) {
					LuaWrite(L, it.Key());
					MakeObjectRef(L, lifetime_, &it.Value());
					return 2;
				}
			}
		}

		return 0;
	}

private:
	RefMap<TKey, TValue>* object_;
	LifetimeHolder lifetime_;
};

	
template <class TKey, class TValue>
class RefMapByValProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "RefMap key should be a by-value type!");

	RefMapByValProxyImpl(LifetimeHolder const& lifetime, RefMap<TKey, TValue> * obj)
		: object_(obj), lifetime_(lifetime)
	{}
		
	~RefMapByValProxyImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	char const* GetKeyTypeName() const override
	{
		return GetTypeInfo<TKey>();
	}

	char const* GetValueTypeName() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto value = object_->Find(key);
		if (value) {
			LuaWrite(L, *value);
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, int luaKeyIndex, int luaValueIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		if (lua_type(L, luaValueIndex) == LUA_TNIL) {
			// FIXME - add support for remove() in refmaps!
			// object_->Remove(key);
		} else {
			TValue value;
			lua_pushvalue(L, luaValueIndex);
			LuaRead(L, value);
			lua_pop(L, 1);

			*object_->Insert(key) = value;
		}

		return true;
	}

	unsigned Length() override
	{
		return object_->Count();
	}

	int Next(lua_State* L, int luaKeyIndex) override
	{
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = object_->begin();
			if (it != object_->end()) {
				LuaWrite(L, it.Key());
				LuaWrite(L, it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = object_->FindIterator(key);
			if (it != object_->end()) {
				it++;
				if (it != object_->end()) {
					LuaWrite(L, it.Key());
					LuaWrite(L, it.Value());
					return 2;
				}
			}
		}

		return 0;
	}

private:
	RefMap<TKey, TValue>* object_;
	LifetimeHolder lifetime_;
};
	

template <class TKey, class TValue>
class MapByRefProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "Map key should be a by-value type!");

	MapByRefProxyImpl(LifetimeHolder const& lifetime, Map<TKey, TValue> * obj)
		: object_(obj), lifetime_(lifetime)
	{}
		
	~MapByRefProxyImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	char const* GetKeyTypeName() const override
	{
		return GetTypeInfo<TKey>();
	}

	char const* GetValueTypeName() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto value = object_->Find(key);
		if (value) {
			MakeObjectRef(L, lifetime_, value);
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, int luaKeyIndex, int luaValueIndex) override
	{
		return false;
	}

	unsigned Length() override
	{
		return object_->Count();
	}

	int Next(lua_State* L, int luaKeyIndex) override
	{
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = object_->begin();
			if (it != object_->end()) {
				LuaWrite(L, it.Key());
				MakeObjectRef(L, lifetime_, &it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = object_->FindIterator(key);
			if (it != object_->end()) {
				it++;
				if (it != object_->end()) {
					LuaWrite(L, it.Key());
					MakeObjectRef(L, lifetime_, &it.Value());
					return 2;
				}
			}
		}

		return 0;
	}

private:
	Map<TKey, TValue>* object_;
	LifetimeHolder lifetime_;
};

	
template <class TKey, class TValue>
class MapByValProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "Map key should be a by-value type!");

	MapByValProxyImpl(LifetimeHolder const& lifetime, Map<TKey, TValue> * obj)
		: object_(obj), lifetime_(lifetime)
	{}
		
	~MapByValProxyImpl() override
	{}

	void* GetRaw() override
	{
		return object_;
	}

	char const* GetKeyTypeName() const override
	{
		return GetTypeInfo<TKey>();
	}

	char const* GetValueTypeName() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto value = object_->Find(key);
		if (value) {
			LuaWrite(L, *value);
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, int luaKeyIndex, int luaValueIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		if (lua_type(L, luaValueIndex) == LUA_TNIL) {
			// FIXME - add support for remove() in maps!
			// object_->Remove(key);
		} else {
			TValue value;
			lua_pushvalue(L, luaValueIndex);
			LuaRead(L, value);
			lua_pop(L, 1);

			*object_->Insert(key) = value;
		}

		return true;
	}

	unsigned Length() override
	{
		return object_->Count();
	}

	int Next(lua_State* L, int luaKeyIndex) override
	{
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = object_->begin();
			if (it != object_->end()) {
				LuaWrite(L, it.Key());
				LuaWrite(L, it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = object_->FindIterator(key);
			if (it != object_->end()) {
				it++;
				if (it != object_->end()) {
					LuaWrite(L, it.Key());
					LuaWrite(L, it.Value());
					return 2;
				}
			}
		}

		return 0;
	}

private:
	Map<TKey, TValue>* object_;
	LifetimeHolder lifetime_;
};


class MapProxy : private Userdata<MapProxy>, public Indexable, public NewIndexable,
	public Lengthable, public Iterable, public Stringifiable, public GarbageCollected
{
public:
	static char const * const MetatableName;


	template <class TKey, class TValue>
	inline static RefMapByRefProxyImpl<TKey, TValue>* MakeByRef(lua_State* L, RefMap<TKey, TValue>* object, LifetimeHolder const& lifetime)
	{
		auto self = NewWithExtraData(L, sizeof(RefMapByRefProxyImpl<TKey, TValue>), lifetime);
		return new (self->GetImpl()) RefMapByRefProxyImpl<TKey, TValue>(lifetime, object);
	}

	template <class TKey, class TValue>
	inline static RefMapByValProxyImpl<TKey, TValue>* MakeByVal(lua_State* L, RefMap<TKey, TValue>* object, LifetimeHolder const& lifetime)
	{
		auto self = NewWithExtraData(L, sizeof(RefMapByValProxyImpl<TKey, TValue>), lifetime);
		return new (self->GetImpl()) RefMapByValProxyImpl<TKey, TValue>(lifetime, object);
	}

	template <class TKey, class TValue>
	inline static MapByRefProxyImpl<TKey, TValue>* MakeByRef(lua_State* L, Map<TKey, TValue>* object, LifetimeHolder const& lifetime)
	{
		auto self = NewWithExtraData(L, sizeof(MapByRefProxyImpl<TKey, TValue>), lifetime);
		return new (self->GetImpl()) MapByRefProxyImpl<TKey, TValue>(lifetime, object);
	}

	template <class TKey, class TValue>
	inline static MapByValProxyImpl<TKey, TValue>* MakeByVal(lua_State* L, Map<TKey, TValue>* object, LifetimeHolder const& lifetime)
	{
		auto self = NewWithExtraData(L, sizeof(MapByValProxyImpl<TKey, TValue>), lifetime);
		return new (self->GetImpl()) MapByValProxyImpl<TKey, TValue>(lifetime, object);
	}

	inline MapProxyImplBase* GetImpl()
	{
		return reinterpret_cast<MapProxyImplBase*>(this + 1);
	}

	inline bool IsAlive() const
	{
		return lifetime_.IsAlive();
	}

	template <class TKey, class TValue>
	RefMapByRefProxyImpl<TKey, TValue>* GetByRefRefMap()
	{
		if (!lifetime_.IsAlive()) {
			return nullptr;
		}
			
		return dynamic_cast<RefMapByRefProxyImpl<TKey, TValue>*>(GetImpl());
	}

	template <class TKey, class TValue>
	RefMapByValProxyImpl<TKey, TValue>* GetByValRefMap()
	{
		if (!lifetime_.IsAlive()) {
			return nullptr;
		}
			
		return dynamic_cast<RefMapByValProxyImpl<TKey, TValue>*>(GetImpl());
	}

private:
	LifetimeReference lifetime_;

	MapProxy(LifetimeHolder const& lifetime)
		: lifetime_(lifetime)
	{}

	~MapProxy()
	{
		GetImpl()->~MapProxyImplBase();
	}

protected:
	friend Userdata<MapProxy>;

	int Index(lua_State* L);
	int NewIndex(lua_State* L);
	int Length(lua_State* L);
	int Next(lua_State* L);
	int ToString(lua_State* L);
	int GC(lua_State* L);
};

template <class T>
struct IsMapLike { static constexpr bool Value = false; };

template <class TK, class TV>
struct IsMapLike<Map<TK, TV>>
{ 
	static constexpr bool Value = true; 
	using TKey = TK;
	using TValue = TV;
};

template <class TK, class TV>
struct IsMapLike<RefMap<TK, TV>>
{ 
	static constexpr bool Value = true;
	using TKey = TK;
	using TValue = TV;
};

template <class T>
inline void push_map_proxy_by_ref(lua_State* L, LifetimeHolder const& lifetime, T* v)
{
	MapProxy::MakeByRef<T>(L, v, lifetime);
}

END_NS()
