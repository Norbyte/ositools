#pragma once

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <optional>

namespace dse::lua
{
	inline void push(lua_State * L, nullptr_t v)
	{
		lua_pushnil(L);
	}

	inline void push(lua_State * L, bool v)
	{
		lua_pushboolean(L, v ? 1 : 0);
	}

	inline void push(lua_State * L, int32_t v)
	{
		lua_pushinteger(L, (lua_Integer)v);
	}

	inline void push(lua_State * L, uint32_t v)
	{
		lua_pushinteger(L, (lua_Integer)v);
	}

	inline void push(lua_State * L, int64_t v)
	{
		lua_pushinteger(L, v);
	}

	inline void push(lua_State * L, uint64_t v)
	{
		lua_pushinteger(L, (lua_Integer)v);
	}

	inline void push(lua_State * L, double v)
	{
		lua_pushnumber(L, v);
	}

	inline void push(lua_State * L, char const * v)
	{
		lua_pushstring(L, v);
	}

	inline void push(lua_State * L, FixedString v)
	{
		lua_pushstring(L, v.Str);
	}

	inline void push(lua_State * L, std::string const & v)
	{
		lua_pushstring(L, v.c_str());
	}

	inline void push(lua_State * L, std::wstring const & v)
	{
		lua_pushstring(L, ToUTF8(v).c_str());
	}

	inline void push(lua_State * L, STDString const & v)
	{
		lua_pushstring(L, v.GetPtr());
	}

	inline void push(lua_State * L, STDWString const & v)
	{
		lua_pushstring(L, ToUTF8(v.GetPtr()).c_str());
	}

	// Dummy pin class for values that need no special pinning/unpinning behavior
	struct NullPin {};

	template <class T>
	inline typename std::enable_if_t<std::is_integral_v<T>, NullPin> push_pin(lua_State * L, T v)
	{
		push(L, v);
		return {};
	}

	// Specifies additional actions taken when the object is pushed to a Lua call
	enum class PushPolicy
	{
		None,
		// Indicates that the we should call Unbind() on the specified object after
		// it goes out of scope (in Lua)
		Unbind
	};

	template <PushPolicy TPolicy>
	struct Pushable
	{};

	template <class T>
	class UnbindablePin
	{
	public:
		inline UnbindablePin(T * object)
			: object_(object)
		{}

		inline ~UnbindablePin()
		{
			if (object_) object_->Unbind();
		}

	private:
		T * object_;
	};

	template <class T>
	inline typename std::enable_if_t<std::is_base_of_v<Pushable<PushPolicy::Unbind>, T>, UnbindablePin<T>> push_pin(lua_State * L, T * v)
	{
		return UnbindablePin(v);
	}

	template <class T>
	inline typename std::enable_if_t<std::is_base_of_v<Pushable<PushPolicy::None>, T>, NullPin> push_pin(lua_State * L, T * v)
	{
		return {};
	}

#define NULL_PIN(type) inline NullPin push_pin(lua_State * L, type v) \
	{ \
		push(L, v); \
		return {}; \
	}

	NULL_PIN(nullptr_t)
	NULL_PIN(double)
	NULL_PIN(char const *)
	NULL_PIN(FixedString)
	NULL_PIN(std::string const &)
	NULL_PIN(std::wstring const &)
	NULL_PIN(STDString const &)
	NULL_PIN(STDWString const &)


	template <class TKey, class TValue>
	inline void settable(lua_State * L, TKey const & k, TValue const & v, int index = -3)
	{
		push(L, k);
		push(L, v);
		lua_settable(L, index);
	}

	template <class TValue>
	TValue get(lua_State * L, int index = -1);

	template <>
	inline bool get<bool>(lua_State * L, int index)
	{
		return lua_toboolean(L, index) == 1;
	}

	template <>
	inline int32_t get<int32_t>(lua_State * L, int index)
	{
		return (int32_t)lua_tointeger(L, index);
	}

	template <>
	inline int64_t get<int64_t>(lua_State * L, int index)
	{
		return lua_tointeger(L, index);
	}

	template <>
	inline double get<double>(lua_State * L, int index)
	{
		return lua_tonumber(L, index);
	}

	template <>
	inline char const * get<char const *>(lua_State * L, int index)
	{
		return lua_tostring(L, index);
	}


	template <class TValue>
	std::optional<TValue> checked_get(lua_State * L, int index = -1);

	template <>
	inline std::optional<bool> checked_get<bool>(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TBOOLEAN) {
			return lua_toboolean(L, index) == 1;
		} else {
			return {};
		}
	}

	template <>
	inline std::optional<int32_t> checked_get<int32_t>(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TNUMBER) {
			return (int32_t)lua_tointeger(L, index);
		} else {
			return {};
		}
	}

	template <>
	inline std::optional<int64_t> checked_get<int64_t>(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TNUMBER) {
			return lua_tointeger(L, index);
		} else {
			return {};
		}
	}

	template <>
	inline std::optional<double> checked_get<double>(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TNUMBER) {
			return lua_tonumber(L, index);
		} else {
			return {};
		}
	}

	template <>
	inline std::optional<char const *> checked_get<char const *>(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TSTRING) {
			return lua_tostring(L, index);
		} else {
			return {};
		}
	}


	template <class TKey, class TValue>
	TValue gettable(lua_State * L, TKey const & k, int index = -2)
	{
		push(L, k);
		lua_gettable(L, index);
		TValue val = get<TValue>(L, -1);
		lua_pop(L, 1);
		return val;
	}

	template <class TEnum>
	std::optional<TEnum> toenum(lua_State * L, int index)
	{
		switch (lua_type(L, index)) {
		case LUA_TSTRING:
		{
			auto val = lua_tostring(L, index);
			auto index = EnumInfo<TEnum>::Find(val);
			if (index) {
				return (TEnum)*index;
			} else {
				return {};
			}
			break;
		}

		case LUA_TNUMBER:
		{
			auto val = lua_tointeger(L, index);
			auto index = EnumInfo<TEnum>::Find((TEnum)val);
			if (index) {
				return (TEnum)val;
			} else {
				return {};
			}
			break;
		}

		default:
			return {};
		}
	}

	template <class TEnum>
	TEnum checkenum(lua_State * L, int index)
	{
		switch (lua_type(L, index)) {
		case LUA_TSTRING:
		{
			auto val = lua_tostring(L, index);
			auto index = EnumInfo<TEnum>::Find(val);
			if (index) {
				return (TEnum)*index;
			} else {
				luaL_error(L, "Param %d is not a valid enum label", index);
			}
			break;
		}

		case LUA_TNUMBER:
		{
			auto val = lua_tointeger(L, index);
			auto index = EnumInfo<TEnum>::Find((TEnum)val);
			if (index) {
				return (TEnum)val;
			} else {
				luaL_error(L, "Param %d is not a valid enum label", index);
			}
			break;
		}

		default:
			luaL_error(L, "Param %d must be an integer or string enumeration label", index);
		}

		return (TEnum)0;
	}

	class RegistryEntry
	{
	public:
		RegistryEntry();
		RegistryEntry(lua_State * L, int index);
		~RegistryEntry();

		RegistryEntry(RegistryEntry const &) = delete;
		RegistryEntry(RegistryEntry &&);

		RegistryEntry & operator = (RegistryEntry const &) = delete;
		RegistryEntry & operator = (RegistryEntry &&);

		void Push();

	private:
		lua_State * L_;
		int ref_;
	};

	class Callable {};
	class Indexable {};
	class NewIndexable {};

	template <class T>
	class Userdata
	{
	public:
		static T * AsUserData(lua_State * L, int index)
		{
			if (lua_type(L, index) == LUA_TUSERDATA) {
				auto obj = luaL_testudata(L, index, T::MetatableName);
				return reinterpret_cast<T *>(obj);
			} else {
				return nullptr;
			}
		}

		static T * CheckUserData(lua_State * L, int index)
		{
			return reinterpret_cast<T *>(luaL_checkudata(L, index, T::MetatableName));
		}

		template <class... Args>
		static T * New(lua_State * L, Args... args)
		{
			auto obj = reinterpret_cast<T *>(lua_newuserdata(L, sizeof(T)));
			new (obj) T(std::forward<Args>(args)...);
			luaL_setmetatable(L, T::MetatableName);
			return obj;
		}

		static int CallProxy(lua_State * L)
		{
			if constexpr (std::is_base_of_v<Callable, T>) {
				auto self = CheckUserData(L, 1);
				return self->LuaCall(L);
			} else {
				return luaL_error(L, "Not callable!");
			}
		}

		static int IndexProxy(lua_State * L)
		{
			if constexpr (std::is_base_of_v<Indexable, T>) {
				auto self = CheckUserData(L, 1);
				return self->Index(L);
			} else {
				return luaL_error(L, "Not indexable!");
			}
		}

		static int NewIndexProxy(lua_State * L)
		{
			if constexpr (std::is_base_of_v<NewIndexable, T>) {
				auto self = CheckUserData(L, 1);
				return self->NewIndex(L);
			} else {
				return luaL_error(L, "Not newindexable!");
			}
		}

		static void PopulateMetatable(lua_State * L)
		{
			// Add custom metatable items by overriding this in subclasses
		}

		static void RegisterMetatable(lua_State * L)
		{
			lua_register(L, T::MetatableName, nullptr);
			luaL_newmetatable(L, T::MetatableName); // stack: mt

			if constexpr (std::is_base_of_v<Callable, T>) {
				lua_pushcfunction(L, &CallProxy); // stack: mt, &LuaCall
				lua_setfield(L, -2, "__call"); // mt.__call = &LuaCall; stack: mt
			}

			if constexpr (std::is_base_of_v<Indexable, T>) {
				lua_pushcfunction(L, &IndexProxy); // stack: mt, &Index
				lua_setfield(L, -2, "__index"); // mt.__index = &Index; stack: mt
			}

			if constexpr (std::is_base_of_v<NewIndexable, T>) {
				lua_pushcfunction(L, &NewIndexProxy); // stack: mt, &NewIndex
				lua_setfield(L, -2, "__newindex"); // mt.__index = &NewIndex; stack: mt
			}

			T::PopulateMetatable(L);

			lua_pop(L, 1); // stack: -
		}
	};
}
