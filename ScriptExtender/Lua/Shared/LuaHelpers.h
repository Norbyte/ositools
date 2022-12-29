#pragma once

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <optional>
#include <GameDefinitions/Base/Base.h>
#include <Lua/Shared/LuaReference.h>

BEGIN_NS(lua)

enum class MetatableTag : uint8_t
{
	ObjectProxyByRef = 0,
	ArrayProxy = 1,
	MapProxy = 2,
	EnumValue = 3,
	BitfieldValue = 4,
	Max = BitfieldValue
};

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

// LuaEnumValue forward declarations
void push_enum_value(lua_State* L, EnumUnderlyingType value, EnumInfoStore<EnumUnderlyingType> const& store);
EnumUnderlyingType get_enum_value(lua_State* L, int index, EnumInfoStore<EnumUnderlyingType> const& store);
std::optional<EnumUnderlyingType> try_get_enum_value(lua_State* L, int index, EnumInfoStore<EnumUnderlyingType> const& store);

EnumUnderlyingType get_bitfield_value(lua_State* L, int index, BitmaskInfoStore<EnumUnderlyingType> const& store, bool maskInvalidBits);
std::optional<EnumUnderlyingType> try_get_bitfield_value(lua_State* L, int index, BitmaskInfoStore<EnumUnderlyingType> const& store, bool maskInvalidBits);
void push_bitfield_value(lua_State* L, EnumUnderlyingType value, BitmaskInfoStore<EnumUnderlyingType> const& store);

// Generation ID forward declarations
uint32_t get_generation_id(lua_State* L);

struct MathParam
{
	union {
		float f;
		glm::vec3 vec3;
		glm::vec4 vec4;
		glm::quat quat;
		glm::mat3 mat3;
		glm::mat4 mat4;
	};
	uint32_t Arity;
};

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

inline void push(lua_State * L, int16_t v)
{
	lua_pushinteger(L, (lua_Integer)v);
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

void push(lua_State* L, FixedString const& v);

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

inline void push(lua_State* L, TemplateHandle const& h)
{
	push(L, h.Value);
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
void push(lua_State* L, glm::quat const& v);
void push(lua_State* L, glm::mat3 const& m);
void push(lua_State* L, glm::mat4 const& m);
void push(lua_State* L, ig::InvokeDataValue const& v);
void push(lua_State* L, Ref const& v);
void push(lua_State* L, PersistentRef const& v);
void push(lua_State* L, RegistryEntry const& v);
void push(lua_State* L, PersistentRegistryEntry const& v);

void assign(lua_State* L, int idx, glm::vec2 const& v);
void assign(lua_State* L, int idx, glm::vec3 const& v);
void assign(lua_State* L, int idx, glm::vec4 const& v);
void assign(lua_State* L, int idx, glm::quat const& v);
void assign(lua_State* L, int idx, glm::mat3 const& m);
void assign(lua_State* L, int idx, glm::mat4 const& m);

template <class T>
inline typename std::enable_if_t<std::is_enum_v<T>, void> push(lua_State* L, T v)
{
	auto ei = EnumInfo<T>::Store;
	push_enum_value(L, static_cast<EnumUnderlyingType>(v), *ei);
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
inline void push_bitfield(lua_State* L, T value)
{
	static_assert(std::is_base_of_v<BitmaskInfoBase<T>, EnumInfo<T>>, "Can only push bitmask fields!");
	push_bitfield_value(L, (typename EnumInfo<T>::UnderlyingType)value, *EnumInfo<T>::Store);
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

FixedString do_get(lua_State* L, int index, Overload<FixedString>);

inline STDString do_get(lua_State* L, int index, Overload<STDString>)
{
	size_t len;
	auto str = luaL_checklstring(L, index, &len);
	return STDString(str, len);
}

inline STDWString do_get(lua_State* L, int index, Overload<STDWString>)
{
	size_t len;
	auto str = luaL_checklstring(L, index, &len);
	return FromUTF8(StringView(str, len));
}

inline Guid do_get(lua_State* L, int index, Overload<Guid>)
{
	size_t len;
	auto str = luaL_checklstring(L, index, &len);
	auto guid = Guid::ParseGuidString(StringView(str, len));
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

inline TemplateHandle do_get(lua_State* L, int index, Overload<TemplateHandle>)
{
	return TemplateHandle((uint32_t)luaL_checknumber(L, index));
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
	if constexpr (std::is_base_of_v<BitmaskInfoBase<T>, EnumInfo<T>>) {
		return (T)get_bitfield_value(L, index, *EnumInfo<T>::Store);
	} else {
		return (T)get_enum_value(L, index, *EnumInfo<T>::Store);
	}
}

ig::InvokeDataValue do_get(lua_State* L, int index, Overload<ig::InvokeDataValue>);

Ref do_get(lua_State* L, int index, Overload<Ref>);
RegistryEntry do_get(lua_State* L, int index, Overload<RegistryEntry>);
PersistentRef do_get(lua_State* L, int index, Overload<PersistentRef>);
PersistentRegistryEntry do_get(lua_State* L, int index, Overload<PersistentRegistryEntry>);

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

inline glm::i16vec2 do_get(lua_State* L, int index, Overload<glm::i16vec2>)
{	
	auto i = lua_absindex(L, index);
	glm::i16vec2 val;
	luaL_checktype(L, index, LUA_TTABLE);
	val.x = gettable<int16_t>(L, 1, i);
	val.y = gettable<int16_t>(L, 2, i);
	return val;	
}

glm::vec2 do_get(lua_State* L, int index, Overload<glm::vec2>);
glm::vec3 do_get(lua_State* L, int index, Overload<glm::vec3>);
glm::vec4 do_get(lua_State* L, int index, Overload<glm::vec4>);
glm::quat do_get(lua_State* L, int index, Overload<glm::quat>);
glm::mat3 do_get(lua_State* L, int index, Overload<glm::mat3>);
glm::mat4 do_get(lua_State* L, int index, Overload<glm::mat4>);
MathParam do_get(lua_State* L, int index, Overload<MathParam>);

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
	void InstantiateModule(lua_State* L, char const* prefix, ModuleDefinition const& module);
	void InstantiateNamedModule(lua_State* L, char const* name, ModuleDefinition const& module);
	void InstantiateNamedModule(lua_State* L, char const* name, char const* subTableName, ModuleDefinition const& module);
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
			luaL_error(L, "Label '%s' is not valid for enumeration '%s'", label.GetString(), EnumInfo<T>::Name);
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
	lua_createtable(L, 2, 0);
	settable(L, 1, v.x);
	settable(L, 2, v.y);
}

inline void push(lua_State* L, glm::i16vec2 const& v)
{
	lua_createtable(L, 2, 0);
	settable(L, 1, v.x);
	settable(L, 2, v.y);
}

inline void push(lua_State* L, Version const& v)
{
	lua_createtable(L, 4, 0);
	settable(L, 2, v.Major());
	settable(L, 1, v.Minor());
	settable(L, 3, v.Revision());
	settable(L, 4, v.Build());
}

template <class T>
inline T get(lua_State* L, int index)
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

struct LuaInternalState;
LuaInternalState* lua_new_internal_state();
void lua_release_internal_state(LuaInternalState* state);

// Object API for storing pointer-like data in a Lua TValue.
void lua_push_cppobject(lua_State* L, MetatableTag metatableTag, int propertyMapIndex, void* object, LifetimeHandle const& lifetime);
void lua_get_cppobject(lua_State* L, int idx, MetatableTag expectedMetatableTag, CppObjectMetadata& obj);
void lua_get_cppobject(lua_State* L, int idx, CppObjectMetadata& obj);
bool lua_try_get_cppobject(lua_State* L, int idx, CppObjectMetadata& obj);
bool lua_try_get_cppobject(lua_State* L, int idx, MetatableTag expectedMetatableTag, CppObjectMetadata& obj);

// Value API for storing raw 64-bit data in a Lua TValue.
void lua_push_cppvalue(lua_State* L, MetatableTag metatableTag, int propertyMapIndex, uint64_t object);
void lua_get_cppvalue(lua_State* L, int idx, MetatableTag expectedMetatableTag, CppValueMetadata& obj);
void lua_get_cppvalue(lua_State* L, int idx, CppValueMetadata& obj);
bool lua_try_get_cppvalue(lua_State* L, int idx, CppValueMetadata& obj);
bool lua_try_get_cppvalue(lua_State* L, int idx, MetatableTag expectedMetatableTag, CppValueMetadata& obj);

void* LuaCppAlloc(lua_State* L, size_t size);
void LuaCppFree(lua_State* L, void* block, size_t size);
CMetatable* LuaCppGetLightMetatable(lua_State* L, unsigned long long val, unsigned long long extra);
CMetatable* LuaCppGetMetatable(lua_State* L, void* val, unsigned long long extra);
void* LuaCppCanonicalize(lua_State* L, void* val);
class GenericPropertyMap& LuaGetPropertyMap(int propertyMapIndex);

void LuaCacheString(lua_State* L, TString* s);
void LuaReleaseString(lua_State* L, TString* s);

END_NS()
