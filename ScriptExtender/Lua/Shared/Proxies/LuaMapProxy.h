#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

class MapProxyImplBase
{
public:
	MapProxyImplBase();
	inline virtual ~MapProxyImplBase() {};
	void Register();
	virtual int GetRegistryIndex() const;
	virtual TypeInformation const& GetArrayType() const = 0;
	virtual TypeInformation const& GetKeyType() const = 0;
	virtual TypeInformation const& GetValueType() const = 0;
	virtual bool GetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) = 0;
	virtual bool SetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex, int luaValueIndex) = 0;
	virtual int Next(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) = 0;
	virtual unsigned Length(CppObjectMetadata& self) = 0;

private:
	int registryIndex_{ -1 };
};

	

template <class TKey, class TValue>
class RefMapByRefProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "RefMap key should be a by-value type!");

	using TMap = RefMap<TKey, TValue>;

	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TMap>();
	}

	TypeInformation const& GetKeyType() const override
	{
		return GetTypeInfo<TKey>();
	}

	TypeInformation const& GetValueType() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		auto value = obj->find(key);
		if (value) {
			MakeObjectRef(L, self.Lifetime, &value.Value());
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex, int luaValueIndex) override
	{
		return false;
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		return obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = obj->begin();
			if (it) {
				LuaWrite(L, it.Key());
				MakeObjectRef(L, self.Lifetime, &it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = obj->find(key);
			if (it) {
				it++;
				if (it) {
					LuaWrite(L, it.Key());
					MakeObjectRef(L, self.Lifetime, &it.Value());
					return 2;
				}
			}
		}

		return 0;
	}
};

	
template <class TKey, class TValue>
class RefMapByValProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "RefMap key should be a by-value type!");

	using TMap = RefMap<TKey, TValue>;

	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TMap>();
	}

	TypeInformation const& GetKeyType() const override
	{
		return GetTypeInfo<TKey>();
	}

	TypeInformation const& GetValueType() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		auto value = obj->find(key);
		if (value) {
			LuaWrite(L, value.Value());
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex, int luaValueIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		if (lua_type(L, luaValueIndex) == LUA_TNIL) {
			auto it = obj->find(key);
			if (it) {
				obj->erase(it);
			}
		} else {
			TValue value;
			lua_pushvalue(L, luaValueIndex);
			LuaRead(L, value);
			lua_pop(L, 1);

			obj->insert(key, value);
		}

		return true;
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		return obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = obj->begin();
			if (it) {
				LuaWrite(L, it.Key());
				LuaWrite(L, it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = obj->find(key);
			if (it) {
				it++;
				if (it) {
					LuaWrite(L, it.Key());
					LuaWrite(L, it.Value());
					return 2;
				}
			}
		}

		return 0;
	}
};
	

template <class TKey, class TValue>
class MapByRefProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "Map key should be a by-value type!");

	using TMap = Map<TKey, TValue>;

	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TMap>();
	}

	TypeInformation const& GetKeyType() const override
	{
		return GetTypeInfo<TKey>();
	}

	TypeInformation const& GetValueType() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		auto value = obj->find(key);
		if (value) {
			MakeObjectRef(L, self.Lifetime, &value.Value());
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex, int luaValueIndex) override
	{
		return false;
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		return obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = obj->begin();
			if (it) {
				LuaWrite(L, it.Key());
				MakeObjectRef(L, self.Lifetime, &it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = obj->find(key);
			if (it) {
				it++;
				if (it) {
					LuaWrite(L, it.Key());
					MakeObjectRef(L, self.Lifetime, &it.Value());
					return 2;
				}
			}
		}

		return 0;
	}
};

	
template <class TKey, class TValue>
class MapByValProxyImpl : public MapProxyImplBase
{
public:
	static_assert(ByVal<TKey>::Value, "Map key should be a by-value type!");

	using TMap = Map<TKey, TValue>;

	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TMap>();
	}

	TypeInformation const& GetKeyType() const override
	{
		return GetTypeInfo<TKey>();
	}

	TypeInformation const& GetValueType() const override
	{
		return GetTypeInfo<TValue>();
	}

	bool GetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		auto it = obj->find(key);
		if (it) {
			LuaWrite(L, it.Value());
			return true;
		} else {
			return false;
		}
	}

	bool SetValue(lua_State* L, CppObjectMetadata& self, int luaKeyIndex, int luaValueIndex) override
	{
		TKey key;
		lua_pushvalue(L, luaKeyIndex);
		LuaRead(L, key);
		lua_pop(L, 1);

		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		if (lua_type(L, luaValueIndex) == LUA_TNIL) {
			auto it = obj->find(key);
			if (it) {
				obj->erase(it);
			}
		} else {
			TValue value;
			lua_pushvalue(L, luaValueIndex);
			LuaRead(L, value);
			lua_pop(L, 1);

			obj->insert(key, value);
		}

		return true;
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		return obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int luaKeyIndex) override
	{
		auto obj = reinterpret_cast<TMap*>(self.Ptr);
		if (lua_type(L, luaKeyIndex) == LUA_TNIL) {
			auto it = obj->begin();
			if (it) {
				LuaWrite(L, it.Key());
				LuaWrite(L, it.Value());
				return 2;
			}
		} else {
			TKey key;
			lua_pushvalue(L, luaKeyIndex);
			LuaRead(L, key);
			lua_pop(L, 1);

			auto it = obj->find(key);
			if (it) {
				it++;
				if (it) {
					LuaWrite(L, it.Key());
					LuaWrite(L, it.Value());
					return 2;
				}
			}
		}

		return 0;
	}
};


class MapProxyMetatable : public LightCppObjectMetatable<MapProxyMetatable>, public Indexable, public NewIndexable,
	public Lengthable, public Iterable, public Stringifiable, public EqualityComparable
{
public:
	static constexpr MetatableTag MetaTag = MetatableTag::MapProxy;

	template <class TImpl>
	static MapProxyImplBase* GetImplementation()
	{
		static MapProxyImplBase* impl = new TImpl();
		return impl;
	}

	inline static void MakeImpl(lua_State* L, void* object, LifetimeHandle const& lifetime, MapProxyImplBase* impl)
	{
		lua_push_cppobject(L, MetatableTag::MapProxy, impl->GetRegistryIndex(), object, lifetime);
	}

	template <class TKey, class TValue>
	inline static void MakeByRef(lua_State* L, RefMap<TKey, TValue>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<RefMapByRefProxyImpl<TKey, TValue>>());
	}

	template <class TKey, class TValue>
	inline static void MakeByVal(lua_State* L, RefMap<TKey, TValue>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<RefMapByValProxyImpl<TKey, TValue>>());
	}

	template <class TKey, class TValue>
	inline static void MakeByRef(lua_State* L, Map<TKey, TValue>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<MapByRefProxyImpl<TKey, TValue>>());
	}

	template <class TKey, class TValue>
	inline static void MakeByVal(lua_State* L, Map<TKey, TValue>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<MapByValProxyImpl<TKey, TValue>>());
	}

	static int Index(lua_State* L, CppObjectMetadata& self);
	static int NewIndex(lua_State* L, CppObjectMetadata& self);
	static int Length(lua_State* L, CppObjectMetadata& self);
	static int Next(lua_State* L, CppObjectMetadata& self);
	static int ToString(lua_State* L, CppObjectMetadata& self);
	static bool IsEqual(lua_State* L, CppObjectMetadata& self, CppObjectMetadata& other);
	static char const* GetTypeName(lua_State* L, CppObjectMetadata& self);
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
inline void push_map_proxy_by_ref(lua_State* L, LifetimeHandle const& lifetime, T* v)
{
	MapProxyMetatable::MakeByRef<T>(L, v, lifetime);
}

END_NS()
