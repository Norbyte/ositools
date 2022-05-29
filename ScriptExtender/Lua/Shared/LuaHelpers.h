#pragma once

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <optional>
#include <GameDefinitions/Base/Base.h>

namespace dse::lua
{
#if !defined(NDEBUG)
	// Ensures that the size of the Lua stack matches the expected value.
	// Since there are no built-in checks in Lua function boundaries, an incorrect push
	// may keep populating the stack with unused items and cause a stack overflow after some time.
	struct StackCheck
	{
		inline StackCheck(lua_State* state, int delta = 0)
			: L(state)
		{
			expectedTop = lua_gettop(L) + delta;
		}

		~StackCheck()
		{
			// During stack unwinding the topmost frame will clean up the stack,
			// so frames where we didn't reach the catch{} handler yet may see incorrect results
			if (std::uncaught_exceptions() > 0) return;

			int newTop = lua_gettop(L);
			if (newTop != expectedTop) {
				// DebugBreak() crashes without a debugger
				if (IsDebuggerPresent()) {
					DebugBreak();
				} else {
					luaL_error(L, "Stack check failed! Top is %d, expected %d", newTop, expectedTop);
				}
			}
		}

		lua_State* L;
		int expectedTop;
	};
#else
	struct StackCheck
	{
		inline StackCheck(lua_State* state, int delta = 0)
		{}
	};
#endif

	struct TableIterationHelper
	{
		struct EndIterator {};

		struct Iterator
		{
			lua_State* L;
			int Index;
			bool End;

			inline Iterator(lua_State* _L, int index)
				: L(_L), Index(index), End(false)
			{}

			inline Iterator operator ++ ()
			{
				lua_pop(L, 1);
				Iterator it(L, Index);
				it.End = lua_next(L, Index) == 0;
				End = it.End;
				return it;
			}

			inline Iterator& operator ++ (int)
			{
				lua_pop(L, 1);
				End = lua_next(L, Index) == 0;
				return *this;
			}

			inline bool operator == (EndIterator const&)
			{
				return End;
			}

			inline bool operator != (EndIterator const&)
			{
				return !End;
			}

			inline int operator * ()
			{
				return -1;
			}
		};

		lua_State* L;
		int Index;

		inline Iterator begin()
		{
			lua_pushnil(L);

			Iterator it(L, Index);
			it.End = lua_next(L, Index) == 0;
			return it;
		}

		inline EndIterator end()
		{
			return EndIterator{};
		}
	};

	inline TableIterationHelper iterate(lua_State* L, int index)
	{
		TableIterationHelper helper;
		helper.L = L;
		helper.Index = lua_absindex(L, index);
		return helper;
	}

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
		if (v) {
			lua_pushstring(L, v);
		} else {
			lua_pushnil(L);
		}
	}

	inline void push(lua_State * L, FixedString const& v)
	{
		lua_pushstring(L, v.Str);
	}

	inline void push(lua_State* L, STDString const& s)
	{
		lua_pushlstring(L, s.data(), s.size());
	}

	inline void push(lua_State* L, STDWString const& s)
	{
		push(L, ToUTF8(s));
	}

	inline void push(lua_State* L, Guid const& s)
	{
		push(L, s.Print());
	}

	inline void push(lua_State * L, StringView const& v)
	{
		lua_pushlstring(L, v.data(), v.size());
	}

	inline void push(lua_State * L, WStringView const& v)
	{
		push(L, ToUTF8(v));
	}

	void push(lua_State* L, EntityHandle const& h);

	inline void push(lua_State* L, ComponentHandle const& h)
	{
		lua_pushlightuserdata(L, (void*)h.Handle);
	}

	inline void push(lua_State* L, lua_CFunction v)
	{
		lua_pushcfunction(L, v);
	}

	inline void push(lua_State* L, Path const& p)
	{
		push(L, p.Name);
	}

	inline void push(lua_State* L, NetId const& v)
	{
		push(L, v.Id);
	}

	inline void push(lua_State* L, UserId const& v)
	{
		push(L, v.Id);
	}

	void push(lua_State* L, glm::ivec2 const& v);
	void push(lua_State* L, glm::vec2 const& v);
	void push(lua_State* L, glm::vec3 const& v);
	void push(lua_State* L, glm::vec4 const& v);
	void push(lua_State* L, glm::mat3 const& m);
	void push(lua_State* L, glm::mat4 const& m);
	void push(lua_State* L, ig::InvokeDataValue const& v);

	template <class T>
	inline typename std::enable_if_t<std::is_enum_v<T>, void> push(lua_State* L, T v)
	{
		auto label = EnumInfo<T>::Find(v);
		if (label) {
			push(L, label);
		} else {
			lua_pushnil(L);
		}
	}

	template <class T>
	inline typename std::enable_if_t<std::is_pointer_v<T>,
		std::enable_if_t<!std::is_same_v<T, char const*>,
		std::enable_if_t<!std::is_same_v<T, char*>,
		std::enable_if_t<!std::is_same_v<T, lua_CFunction>, void>>>> push(lua_State* L, T v)
	{
		static_assert(false, "Can't push pointers to Lua");
	}

	template <class T>
	inline void push(lua_State* L, std::optional<T> const& v)
	{
		if (v) {
			push(L, *v);
		} else {
			lua_pushnil(L);
		}
	}

	template <class T>
	inline void push_flags(lua_State* L, T value)
	{
		static_assert(std::is_base_of_v<BitmaskInfoBase<T>, EnumInfo<T>>, "Can only push bitmask fields!");

		lua_newtable(L);
		int i = 1;
		for (auto const& val : EnumInfo<T>::Values) {
			if ((value & val.Value) == val.Value) {
				push(L, i++);
				push(L, val.Key);
				lua_settable(L, -3);
			}
		}
	}


	template <class T>
	T get(lua_State* L, int index);

	inline bool do_get(lua_State * L, int index, Overload<bool>)
	{
		luaL_checktype(L, index, LUA_TBOOLEAN);
		return lua_toboolean(L, index) == 1;
	}

	inline NetId do_get(lua_State* L, int index, Overload<NetId>)
	{
		auto v = luaL_checkinteger(L, index);
		return NetId((uint32_t)v);
	}

	inline UserId do_get(lua_State* L, int index, Overload<UserId>)
	{
		auto v = luaL_checkinteger(L, index);
		return UserId((int32_t)v);
	}

	inline char const* do_get(lua_State* L, int index, Overload<char const*>)
	{
		return luaL_checkstring(L, index);
	}

	inline FixedString do_get(lua_State* L, int index, Overload<FixedString>)
	{
		auto str = luaL_checkstring(L, index);
		return FixedString(str);
	}

	inline STDString do_get(lua_State* L, int index, Overload<STDString>)
	{
		return STDString(luaL_checkstring(L, index));
	}

	inline STDWString do_get(lua_State* L, int index, Overload<STDWString>)
	{
		auto str = luaL_checkstring(L, index);
		return FromUTF8(str);
	}

	inline Guid do_get(lua_State* L, int index, Overload<Guid>)
	{
		auto str = luaL_checkstring(L, index);
		auto guid = Guid::ParseGuidString(str);
		if (!guid) {
			luaL_error(L, "Param %d: not a valid GUID value: '%s'", str);
		}

		return *guid;
	}

	inline ComponentHandle do_get(lua_State* L, int index, Overload<ComponentHandle>)
	{
		luaL_checktype(L, index, LUA_TLIGHTUSERDATA);
		return ComponentHandle{ (uint64_t)lua_touserdata(L, index) };
	}

	inline Path do_get(lua_State* L, int index, Overload<Path>)
	{
		return Path(luaL_checkstring(L, index));
	}

	template <class T>
	inline typename std::enable_if_t<std::is_integral_v<T>, T> do_get(lua_State * L, int index, Overload<T>)
	{
		return (T)luaL_checkinteger(L, index);
	}

	template <class T>
	inline typename std::enable_if_t<std::is_floating_point_v<T>, T> do_get(lua_State* L, int index, Overload<T>)
	{
		return (T)luaL_checknumber(L, index);
	}

	template <class T>
	typename std::enable_if_t<std::is_enum_v<T>, T> do_get(lua_State * L, int index, Overload<T>)
	{
		switch (lua_type(L, index)) {
		case LUA_TSTRING:
		{
			auto val = lua_tostring(L, index);
			auto index = EnumInfo<T>::Find(val);
			if (index) {
				return (T)*index;
			} else {
				luaL_error(L, "Param %d: not a valid '%s' enum label: %s", index, EnumInfo<T>::Name, val);
			}
			break;
		}

		case LUA_TNUMBER:
		{
			if constexpr (std::is_base_of_v<BitmaskInfoBase<T>, EnumInfo<T>>) {
				auto val = lua_tointeger(L, index);
				auto index = EnumInfo<T>::Find((T)val);
				if (((T)val & ~EnumInfo<T>::AllowedFlags) == (T)0) {
					return (T)val;
				} else {
					luaL_error(L, "Param %d: not a valid '%s' flag bitmask: %08x", index, EnumInfo<T>::Name, val);
				}
			} else {
				auto val = lua_tointeger(L, index);
				auto index = EnumInfo<T>::Find((T)val);
				if (index) {
					return (T)val;
				} else {
					luaL_error(L, "Param %d: not a valid '%s' enum index: %d", index, EnumInfo<T>::Name, val);
				}
			}
			break;
		}

		case LUA_TTABLE:
		{
			if constexpr (std::is_base_of_v<BitmaskInfoBase<T>, EnumInfo<T>>) {
				T val{ 0 };
				for (auto valueIdx : iterate(L, index)) {
					auto label = do_get(L, valueIdx, Overload<FixedString>{});
					auto index = EnumInfo<T>::Find(label);
					if (index) {
						val |= (T)*index;
					} else {
						luaL_error(L, "Param %d: not a valid '%s' enum label: %s", 
							index, EnumInfo<T>::Name, label.GetString());
					}
				}

				return val;
			} else {
				luaL_error(L, "Param %d: table values not allowed as parameter when passing a '%s' enumeration (only for bitfields)", 
					index, EnumInfo<T>::Name);
			}
			break;
		}

		default:
			luaL_error(L, "Param %d: expected integer or string '%s' enumeration value, got %s", index,
				EnumInfo<T>::Name, lua_typename(L, lua_type(L, index)));
			break;
		}

		return (T)0;
	}

	ig::InvokeDataValue do_get(lua_State* L, int index, Overload<ig::InvokeDataValue>);


	template <class TValue>
	TValue checked_getfield(lua_State* L, char const* k, int index = -1)
	{
		lua_getfield(L, index, k);
		TValue val = get<TValue>(L, -1);
		lua_pop(L, 1);
		return val;
	}

	template <class TValue>
	TValue gettable(lua_State* L, lua_Integer k, int index = -2)
	{
		lua_rawgeti(L, index, k);
		TValue val = do_get(L, -1, Overload<TValue>{});
		lua_pop(L, 1);
		return val;
	}

	template <class TValue>
	TValue gettable(lua_State* L, char const* k, int index = -2)
	{
		push(L, k);
		lua_rawget(L, index);
		TValue val = do_get(L, -1, Overload<TValue>{});
		lua_pop(L, 1);
		return val;
	}

	inline glm::ivec2 do_get(lua_State* L, int index, Overload<glm::ivec2>)
	{	
		auto i = lua_absindex(L, index);
		glm::ivec2 val;	
		luaL_checktype(L, index, LUA_TTABLE);
		val.x = gettable<int32_t>(L, 1, i);
		val.y = gettable<int32_t>(L, 2, i);
		return val;	
	}

	inline glm::vec2 do_get(lua_State* L, int index, Overload<glm::vec2>)
	{
		auto i = lua_absindex(L, index);
		glm::vec2 val;
		luaL_checktype(L, index, LUA_TTABLE);
		val.x = gettable<float>(L, 1, i);
		val.y = gettable<float>(L, 2, i);
		return val;
	}

	inline glm::vec3 do_get(lua_State* L, int index, Overload<glm::vec3>)
	{
		auto i = lua_absindex(L, index);
		glm::vec3 val;
		luaL_checktype(L, index, LUA_TTABLE);
		val.x = gettable<float>(L, 1, i);
		val.y = gettable<float>(L, 2, i);
		val.z = gettable<float>(L, 3, i);
		return val;
	}

	inline glm::vec4 do_get(lua_State* L, int index, Overload<glm::vec4>)
	{
		auto i = lua_absindex(L, index);
		glm::vec4 val;	
		luaL_checktype(L, index, LUA_TTABLE);
		val.x = gettable<float>(L, 1, i);
		val.y = gettable<float>(L, 2, i);
		val.z = gettable<float>(L, 3, i);
		val.w = gettable<float>(L, 4, i);
		return val;	
	}

	inline glm::mat3 do_get(lua_State* L, int index, Overload<glm::mat3>)
	{
		auto idx = lua_absindex(L, index);
		glm::mat3 val;
		luaL_checktype(L, index, LUA_TTABLE);
		for (int i = 0; i < 9; i++) {
			val[i / 3][i % 3] = gettable<float>(L, i + 1, idx);
		}
		return val;
	}
	
	inline glm::mat4 do_get(lua_State* L, int index, Overload<glm::mat4>)
	{
		auto idx = lua_absindex(L, index);
		glm::mat4 val;
		luaL_checktype(L, index, LUA_TTABLE);
		for (int i = 0; i < 16; i++) {
			val[i / 4][i % 4] = gettable<float>(L, i + 1, idx);
		}
		return val;
	}

	inline Version do_get(lua_State* L, int index, Overload<Version>)
	{
		auto i = lua_absindex(L, index);
		luaL_checktype(L, i, LUA_TTABLE);
		uint32_t minor = gettable<uint32_t>(L, 1, i),
			major = gettable<uint32_t>(L, 2, i),
			revision = gettable<uint32_t>(L, 3, i),
			build = gettable<uint32_t>(L, 4, i);
		return Version(minor, major, revision, build);	
	}

	template <class T>
	inline typename std::optional<T> do_get(lua_State* L, int index, Overload<std::optional<T>>)
	{
		if (lua_type(L, index) == LUA_TNIL) {
			return {};
		} else {
			return do_get(L, index, Overload<T>{});
		}
	}

	// Overload helper for fetching a parameter for a Lua -> C++ function call
	template <class T>
	inline T checked_get_param(lua_State* L, int i, Overload<T>)
	{
		return get<T>(L, i);
	}

	template <class T>
	inline std::optional<T> checked_get_param(lua_State* L, int i, Overload<std::optional<T>>)
	{
		if (lua_gettop(L) < i || lua_isnil(L, i)) {
			return {};
		} else {
			return get<T>(L, i);
		}
	}


	int CallWithTraceback(lua_State* L, int narg, int nres);

	// Calls Lua function.
	// Function and arguments must be already pushed to the Lua stack.
	// Returns false if call failed, true tuple otherwise.
	// Function name only needed for error reporting purposes
	inline bool CheckedCall(lua_State* L, int numArgs, char const* functionName)
	{
		if (CallWithTraceback(L, numArgs, 0) != 0) { // stack: errmsg
			ERR("%s Lua call failed: %s", functionName, lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		return true;
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

		void Push() const;

	private:
		lua_State * L_;
		int ref_;
	};

	struct ModuleFunction
	{
		FixedString Name;
		lua_CFunction Func;
		TypeInformation Signature;
	};

	enum class ModuleRole
	{
		Client,
		Server,
		Both
	};

	struct ModuleDefinition
	{
		ModuleRole Role;
		FixedString Table;
		FixedString SubTable;
		std::vector<ModuleFunction> Functions;
	};

	class ModuleRegistry
	{
	public:
		void RegisterModule(ModuleDefinition const& module);
		void ConstructState(lua_State* L, ModuleRole role);
		void RegisterTypeInformation();

		inline std::vector<ModuleDefinition> const& GetModules() const
		{
			return modules_;
		}

	private:
		std::vector<ModuleDefinition> modules_;

		void MakeLuaFunctionTable(ModuleDefinition const& module, std::vector<luaL_Reg>& lib);
		void InstantiateModule(lua_State* L, ModuleDefinition const& module);
		void InstantiateModule(lua_State* L, char const* name, ModuleDefinition const& module);
		void InstantiateModule(lua_State* L, char const* name, char const* subTableName, ModuleDefinition const& module);
		void RegisterModuleTypeInformation(ModuleDefinition const& module);
	};

	extern ModuleRegistry gModuleRegistry;

	template <class T>
	inline T checked_get_flags(lua_State* L, int index)
	{
		static_assert(std::is_base_of_v<BitmaskInfoBase<T>, EnumInfo<T>>, "Can only fetch bitmask fields!");

		luaL_checktype(L, index, LUA_TTABLE);
		T flags = (T)0;
		for (auto idx : iterate(L, index)) {
			auto label = get<FixedString>(L, idx);
			auto val = EnumInfo<T>::Find(label);
			if (val) {
				flags |= *val;
			} else {
				luaL_error(L, "Label '%s' is not valid for enumeration '%s'", label.Str, EnumInfo<T>::Name);
			}
		}

		return flags;
	}
	

	template <class TValue>
	inline void setfield(lua_State* L, char const* k, TValue const& v, int index = -2)
	{
		push(L, v);
		lua_setfield(L, index, k);
	}

	template <class TKey, class TValue>
	inline void settable(lua_State* L, TKey const& k, TValue const& v, int index = -3)
	{
		push(L, k);
		push(L, v);
		lua_settable(L, index);
	}

	inline void push(lua_State* L, glm::ivec2 const& v)
	{
		lua_newtable(L);
		settable(L, 1, v.x);
		settable(L, 2, v.y);
	}

	inline void push(lua_State* L, glm::vec2 const& v)
	{
		lua_newtable(L);
		settable(L, 1, v.x);
		settable(L, 2, v.y);
	}

	inline void push(lua_State* L, glm::vec3 const& v)
	{
		lua_newtable(L);
		settable(L, 1, v.x);
		settable(L, 2, v.y);
		settable(L, 3, v.z);
	}

	inline void push(lua_State* L, glm::vec4 const& v)
	{
		lua_newtable(L);
		settable(L, 1, v.x);
		settable(L, 2, v.y);
		settable(L, 3, v.z);
		settable(L, 4, v.w);
	}

	inline void push(lua_State* L, Version const& v)
	{
		lua_newtable(L);
		settable(L, 1, v.Minor());
		settable(L, 2, v.Major());
		settable(L, 3, v.Revision());
		settable(L, 4, v.Build());
	}

	inline void push(lua_State* L, glm::mat3 const& m)
	{
		lua_newtable(L);
		for (auto i = 0; i < 9; i++) {
			settable(L, i + 1, m[i / 3][i % 3]);
		}
	}

	inline void push(lua_State* L, glm::mat4 const& m)
	{
		lua_newtable(L);
		for (auto i = 0; i < 16; i++) {
			settable(L, i + 1, m[i / 4][i % 4]);
		}
	}

	template <class T>
	inline typename T get(lua_State* L, int index)
	{
		return do_get(L, index, Overload<T>{});
	}

	template <class T>
	T try_gettable(lua_State* L, char const* k, int index, T defaultValue)
	{
		push(L, k);
		lua_rawget(L, index);
		auto val = get<std::optional<T>>(L, -1);
		lua_pop(L, 1);
		return val ? *val : defaultValue;
	}

	template <class T>
	std::optional<T> try_gettable(lua_State* L, char const* k, int index)
	{
		push(L, k);
		lua_rawget(L, index);
		auto val = get<std::optional<T>>(L, -1);
		lua_pop(L, 1);
		return val;
	}
}
