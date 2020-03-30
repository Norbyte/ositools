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

	inline void push(lua_State * L, StringView v)
	{
		lua_pushlstring(L, v.data(), v.size());
	}

	inline void push(lua_State * L, WStringView v)
	{
		push(L, ToUTF8(v));
	}

	// Dummy pin class for values that need no special pinning/unpinning behavior
	struct NullPin {};

	template <class T>
	inline typename std::enable_if_t<std::is_integral_v<T>, NullPin> push_pin(lua_State * L, T v)
	{
		push(L, v);
		return {};
	}

	template <class T, typename std::enable_if_t<std::is_invocable_v<T, lua_State *>, int> * = nullptr>
	inline auto push_pin(lua_State * L, T v)
	{
		return v(L);
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

		inline UnbindablePin(UnbindablePin const &) = delete;

		inline UnbindablePin(UnbindablePin && other)
			: object_(other.object_)
		{
			other.object_ = nullptr;
		}

		UnbindablePin & operator = (UnbindablePin const &) = delete;


	private:
		T * object_;
	};

#define NULL_PIN(type) inline NullPin push_pin(lua_State * L, type v) \
	{ \
		push(L, v); \
		return {}; \
	}

	NULL_PIN(nullptr_t)
	NULL_PIN(double)
	NULL_PIN(char const *)
	NULL_PIN(FixedString)
	NULL_PIN(StringView)
	NULL_PIN(WStringView)


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



	template <class T, typename std::enable_if_t<std::is_enum_v<T>, int> * = nullptr>
	std::optional<T> safe_get(lua_State * L, int index = -1)
	{
		switch (lua_type(L, index)) {
		case LUA_TSTRING:
		{
			auto val = lua_tostring(L, index);
			auto index = EnumInfo<T>::Find(val);
			if (index) {
				return (T)*index;
			} else {
				ERR("'%s' is not a valid enumeration label in '%s'", val, EnumInfo<T>::Name);
				return {};
			}
			break;
		}

		case LUA_TNUMBER:
		{
			auto val = lua_tointeger(L, index);
			auto index = EnumInfo<T>::Find((T)val);
			if (index) {
				return (T)val;
			} else {
				ERR("'%d' is not a valid enumeration index in '%s'", val, EnumInfo<T>::Name);
				return {};
			}
			break;
		}

		default:
			return {};
		}
	}

	template <class T, typename std::enable_if_t<std::is_same_v<T, bool>, int> * = nullptr>
	inline std::optional<T> safe_get(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TBOOLEAN) {
			return lua_toboolean(L, index) == 1;
		} else {
			return {};
		}
	}

	template <class T, typename std::enable_if_t<std::is_integral_v<T>, int> * = nullptr>
	inline std::optional<T> safe_get(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TNUMBER) {
			return (T)lua_tointeger(L, index);
		} else {
			return {};
		}
	}

	template <class T, typename std::enable_if_t<std::is_floating_point_v<T>, int> * = nullptr>
	inline std::optional<T> safe_get(lua_State * L, int index)
	{
		if (lua_type(L, index) == LUA_TNUMBER) {
			return (T)lua_tonumber(L, index);
		} else {
			return {};
		}
	}

	template <class T, typename std::enable_if_t<std::is_same_v<T, char const *>, int> * = nullptr>
	inline std::optional<char const *> safe_get(lua_State * L, int index)
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
		return safe_get<TEnum>(L, index);
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


	template <class T, class... Args>
	inline auto Push(Args... args)
	{
		return[args...](lua_State * L) {
			auto obj = T::New(L, args...);
			if constexpr (std::is_base_of_v<Pushable<PushPolicy::Unbind>, T>) {
				return UnbindablePin{ obj };
			} else {
				return NullPin{};
			}
		};
	}

	// Pushes all arguments to the Lua stack and returns a pin that should
	// be destroyed after the call
	template <class ...Args>
	inline auto PushArguments(lua_State * L, std::tuple<Args...> args)
	{
		return std::apply([=](const auto &... elem)
		{
			return std::tuple{ push_pin(L, elem)... };
		}, args);
	}

	// Helper for indicating return type of a Lua function
	template <class... Args>
	struct ReturnType {};

	// Helper struct to allow function overloading without (real) template-dependent parameters
	template <class>
	struct Overload {};

	// Fetches a required return value (i.e. succeeded = false if arg doesn't exist or is nil)
	template <class T>
	T CheckedGetReturnValue(lua_State * L, int & index, bool & succeeded, Overload<T>)
	{
		auto i = index--;
		if (lua_isnil(L, i)) {
			ERR("Return value %d must not be missing or nil", -i);
			succeeded = false;
			return {};
		} else {
			auto val = safe_get<T>(L, i);
			if ((bool)val) {
				return *val;
			} else {
				ERR("Failed to fetch return value %d, incorrect type?", -i);
				succeeded = false;
				return {};
			}
		}
	}

	// Fetches an optional return value (i.e. succeeded = true if arg doesn't exist or is nil)
	template <class T>
	std::optional<T> CheckedGetReturnValue(lua_State * L, int & index, bool & succeeded, Overload<std::optional<T>>)
	{
		auto i = index--;
		if (lua_isnil(L, i)) {
			return {};
		} else {
			auto val = safe_get<T>(L, i);
			if ((bool)val) {
				return val;
			} else {
				ERR("Failed to fetch return value %d, incorrect type?", -i);
				succeeded = false;
				return {};
			}
		}
	}

	// Fetch Lua return values into a tuple
	// Sets succeeded=false if validation of any return value failed.
	// Tuple size *must* match lua_call nres, otherwise it'll corrupt the Lua heap!
	template <class... Args>
	auto CheckedGetReturnValues(lua_State * L, bool & succeeded)
	{
		int index{ -1 };
		return std::tuple{CheckedGetReturnValue(L, index, succeeded, Overload<Args>{})...};
	}

	// Fetch Lua return values into a tuple
	// Returns {} if validation of any return value failed, rval tuple otherwise.
	// Tuple size *must* match lua_call nres, otherwise it'll corrupt the Lua heap!
	template <class... Args>
	auto CheckedPopReturnValues(lua_State * L)
	{
		bool succeeded{ true };
		auto rval = CheckedGetReturnValues<Args...>(L, succeeded);
		lua_pop(L, (int)sizeof...(Args));
		if (succeeded) {
			return std::optional(rval);
		} else {
			return std::optional<decltype(rval)>();
		}
	}

	// Calls Lua function and fetches Lua return values into a tuple.
	// Function and arguments must be already pushed to the Lua stack.
	// Returns {} if call or return value fetch failed, rval tuple otherwise.
	// Function name only needed for error reporting purposes
	template <class... Args>
	auto CheckedCall(lua_State * L, int numArgs, char const * functionName)
	{
		if (CallWithTraceback(L, numArgs, sizeof...(Args)) != 0) { // stack: errmsg
			ERR("%s Lua call failed: %s", functionName, lua_tostring(L, -1));
			lua_pop(L, 1);
			return decltype(CheckedPopReturnValues<Args...>(L))();
		}

		auto result = CheckedPopReturnValues<Args...>(L);
		if (!result) {
			ERR("Got incorrect return values from %s", functionName);
		}

		return result;
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

	int CallWithTraceback(lua_State * L, int narg, int nres);

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

	template <class T>
	inline std::optional<T *> safe_get_userdata(lua_State * L, int index)
	{
		if (lua_isnil(L, index)) {
			return {};
		} else {
			auto val = T::AsUserData(L, index);
			if (val) {
				return val;
			} else {
				ERR("Expected userdata of type '%s'", T::MetatableName);
				return {};
			}
		}
	}

	template <class T, typename std::enable_if_t<std::is_base_of_v<Userdata<std::remove_pointer_t<T>>, std::remove_pointer_t<T>>, int> * = nullptr>
	inline std::optional<T> safe_get(lua_State * L, int index)
	{
		return safe_get_userdata<std::remove_pointer_t<T>>(L, index);
	}
}
