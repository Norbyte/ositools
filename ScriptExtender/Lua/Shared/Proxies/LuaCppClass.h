#pragma once

#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

struct CppObjectMetadata
{
	void* Ptr;
	uint8_t MetatableTag;
	uint16_t PropertyMapTag;
	LifetimeHandle Lifetime;
};

class CppPropertyMapManager
{
public:
	int RegisterPropertyMap(GenericPropertyMap* mt);
	GenericPropertyMap* GetPropertyMap(int index);

private:
	ObjectSet<GenericPropertyMap*> propertyMaps_;
};

class CppMetatableManager
{
public:
	int RegisterMetatable(CMetatable* mt);
	CMetatable* GetMetatable(int index);

	static CppMetatableManager& FromLua(lua_State* L);

private:
	ObjectSet<CMetatable *> metatables_;
};

// NOTE: Must match TMS enumeration in Lua
enum class MetamethodName : int
{
	Index,
	NewIndex,
	GC,
	Mode,
	Len,
	Eq,
	Add,
	Sub,
	Mul,
	Mod,
	Pow,
	Div,
	IDiv,
	BAnd,
	BOr,
	BXor,
	Shl,
	Shr,
	Unm,
	BNot,
	Lt,
	Le,
	Concat,
	Call,
	Pairs,
	ToString,
	Name
};

template <class T>
class LightCppObjectMetatable
{
public:
	template <class T>
	inline static void Make(lua_State* L, T* object, LifetimeHandle const& lifetime)
	{
		auto const& pm = StaticLuaPropertyMap<T>::PropertyMap;
		lua_push_cppobject(L, RegistryIndex(), pm.RegistryIndex, object, lifetime);
	}

	static std::optional<CppObjectMetadata> TryGet(lua_State* L, int index)
	{
		CppObjectMetadata meta;
		if (lua_try_get_cppobject(L, index, RegistryIndex(), meta)) {
			return meta;
		} else {
			return {};
		}
	}

	static void* Get(lua_State* L, int index, int propertyMapIndex)
	{
		CppObjectMetadata meta;
		if (lua_try_get_cppobject(L, index, meta)) {
			auto& pm = LuaGetPropertyMap(meta.PropertyMapTag);
			if (pm.IsA(propertyMapIndex)) {
				return meta.Ptr;
			} else {
				luaL_error(L, "Argument %d: Expected object of type '%s', got '%s'", index,
					LuaGetPropertyMap(propertyMapIndex).Name.GetString(),
					pm.Name.GetString());
				return nullptr;
			}
		} else {
			luaL_error(L, "Argument %d: Expected object of type '%s', got '%s'", index,
				LuaGetPropertyMap(propertyMapIndex).Name.GetString(),
				lua_typename(L, lua_type(L, index)));
			return nullptr;
		}
	}

	template <class T>
	static T* GetTyped(lua_State* L, int index)
	{
		auto ptr = Get(L, index, StaticLuaPropertyMap<T>::PropertyMap.RegistryIndex);
		return reinterpret_cast<T*>(ptr);
	}

	static int CallProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<Callable, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::Call(L, self);
		} else {
			return luaL_error(L, "Not callable!");
		}
	}

	static int IndexProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<Indexable, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::Index(L, self);
		} else {
			return luaL_error(L, "Not indexable!");
		}
	}

	static int NewIndexProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<NewIndexable, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::NewIndex(L, self);
		} else {
			return luaL_error(L, "Not newindexable!");
		}
	}

	static int LengthProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<Lengthable, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::Length(L, self);
		} else {
			return luaL_error(L, "Not lengthable!");
		}
	}

	static int PairsProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<Iterable, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::Pairs(L, self);
		} else {
			return luaL_error(L, "Not iterable!");
		}
	}

	static int ToStringProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<Stringifiable, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::ToString(L, self);
		} else {
			return luaL_error(L, "Not stringifiable!");
		}
	}

	static int EqualProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<EqualityComparable, T>) {
			CppObjectMetadata self, other;
			lua_get_cppobject(L, 1, 0, self);

			bool equal;
			if (lua_try_get_cppobject(L, 2, 0, other)) {
				equal = T::IsEqual(L, self, other);
			} else {
				equal = false;
			}

			push(L, equal);
			return 1;
		} else {
			return luaL_error(L, "Not comparable!");
		}
	}

	// Default __pairs implementation
	static int Pairs(lua_State* L, CppObjectMetadata const& self)
	{
		StackCheck _(L, 3);
		lua_pushcfunction(L, &NextProxy);
		lua_pushvalue(L, 1);
		push(L, nullptr);

		return 3;
	}

	static int NextProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<Iterable, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::Next(L, self);
		} else {
			return luaL_error(L, "Not iterable!");
		}
	}

	static int NameProxy(lua_State* L)
	{
		if constexpr (std::is_base_of_v<Named, T>) {
			CppObjectMetadata self;
			lua_get_cppobject(L, 1, 0, self);
			return T::Name(L, self);
		} else {
			return luaL_error(L, "Not named!");
		}
	}

	static void PopulateMetatable(lua_State* L, CMetatable* mt)
	{
		// Add custom metatable items by overriding this in subclasses
	}

	static void RegisterMetatable(lua_State* L)
	{
		auto mt = lua_alloc_cmetatable(L);

		if constexpr (std::is_base_of_v<Callable, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::Call, &CallProxy);
		}

		if constexpr (std::is_base_of_v<Indexable, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::Index, &IndexProxy);
		}

		if constexpr (std::is_base_of_v<NewIndexable, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::NewIndex, &NewIndexProxy);
		}

		if constexpr (std::is_base_of_v<Lengthable, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::Len, &LengthProxy);
		}

		if constexpr (std::is_base_of_v<Iterable, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::Pairs, &PairsProxy);
		}

		if constexpr (std::is_base_of_v<Stringifiable, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::ToString, &ToStringProxy);
		}

		// Light cppobjects are not garbage collected (they're passed by value)

		if constexpr (std::is_base_of_v<EqualityComparable, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::Eq, &EqualProxy);
		}

		if constexpr (std::is_base_of_v<Named, T>) {
			lua_cmetatable_set(L, mt, (int)MetamethodName::Name, &NameProxy);
		}

		T::PopulateMetatable(L, mt);
		registryIndex_ = CppMetatableManager::FromLua(L).RegisterMetatable(mt);
	}

	inline static int RegistryIndex()
	{
		return registryIndex_;
	}

private:
	static int registryIndex_;
};

END_NS()
