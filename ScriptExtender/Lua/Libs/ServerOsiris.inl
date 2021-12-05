#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>
#include <regex>

BEGIN_NS(esv::lua::osiris)

void RegisterListener(lua_State* L, char const* name, int arity, char const* typeName)
{
	luaL_checktype(L, 4, LUA_TFUNCTION);

	OsirisHookSignature::HookType type;
	if (strcmp(typeName, "before") == 0) {
		type = OsirisHookSignature::BeforeTrigger;
	} else if (strcmp(typeName, "after") == 0) {
		type = OsirisHookSignature::AfterTrigger;
	} else if (strcmp(typeName, "beforeDelete") == 0) {
		type = OsirisHookSignature::BeforeDeleteTrigger;
	} else if (strcmp(typeName, "afterDelete") == 0) {
		type = OsirisHookSignature::AfterDeleteTrigger;
	} else {
		luaL_error(L, "Hook type must be 'before', 'beforeDelete', 'after' or 'afterDelete'");
	}

	LuaServerPin lua(ExtensionState::Get());
	RegistryEntry handler(L, 4);
	lua->Osiris().GetOsirisCallbacks().Subscribe(name, arity, type, std::move(handler));
}

bool IsCallable(lua_State* L)
{
	StackCheck _(L, 1);
	auto lua = State::FromLua(L);
	return gExtender->GetServer().Osiris().IsStoryLoaded()
		&& ((lua->RestrictionFlags & State::RestrictOsiris) == 0);
}

ValueType StringToValueType(std::string_view s)
{
	if (s == "INTEGER") {
		return ValueType::Integer;
	} else if (s == "INTEGER") {
		return ValueType::Integer;
	} else if (s == "INTEGER64") {
		return ValueType::Integer64;
	} else if (s == "REAL") {
		return ValueType::Real;
	} else if (s == "STRING") {
		return ValueType::String;
	} else if (s == "GUIDSTRING") {
		return ValueType::GuidString;
	} else if (s == "CHARACTERGUID") {
		return ValueType::CharacterGuid;
	} else if (s == "ITEMGUID") {
		return ValueType::ItemGuid;
	} else if (s == "TRIGGERGUID") {
		return ValueType::TriggerGuid;
	} else if (s == "SPLINEGUID") {
		return ValueType::SplineGuid;
	} else if (s == "LEVELTEMPLATEGUID") {
		return ValueType::LevelTemplateGuid;
	} else {
		OsiError("Unknown Osiris value type: " << s.data());
		return ValueType::None;
	}
}

const std::regex inOutParamRe("^\\s*(\\[(in|out)\\])?\\(([A-Z0-9]+)\\)(_[a-zA-Z0-9]+)\\s*$");
const std::regex inParamRe("^\\s*\\(([A-Z0-9]+)\\)(_[a-zA-Z0-9]+)\\s*$");

CustomFunctionParam ParseCustomFunctionParam(lua_State * L, STDString const & param, bool isQuery)
{
	CustomFunctionParam parsed;

	std::smatch paramMatch;
	if (!std::regex_match(param, paramMatch, isQuery ? inOutParamRe : inParamRe)) {
		luaL_error(L, "Parameter string malformed: %s", param.c_str());
	}

	if (isQuery && paramMatch[2].matched) {
		auto dir = paramMatch[2].str();
		if (dir == "in") {
			parsed.Dir = FunctionArgumentDirection::In;
		} else if (dir == "out") {
			parsed.Dir = FunctionArgumentDirection::Out;
		} else {
			luaL_error(L, "Invalid parameter direction: %s", dir.c_str());
		}
	} else {
		parsed.Dir = FunctionArgumentDirection::In;
	}

	auto type = paramMatch[isQuery ? 3 : 1].str();
	parsed.Type = StringToValueType(type);
	if (parsed.Type == ValueType::None) {
		luaL_error(L, "Unsupported parameter type: %s", type.c_str());
	}

	parsed.Name = paramMatch[isQuery ? 4 : 2].str().substr(1);
	return parsed;
}

void ParseCustomFunctionParams(lua_State * L, char const * s, 
	std::vector<CustomFunctionParam> & params, bool isQuery)
{
	STDString param;
	std::istringstream paramStream(s);

	while (std::getline(paramStream, param, ',')) {
		auto parsedParam = ParseCustomFunctionParam(L, param, isQuery);
		params.push_back(parsedParam);
	}
}

void NewCall(lua_State * L)
{
	auto lua = State::FromLua(L);
	if (lua->StartupDone()) luaL_error(L, "Attempted to register call after Lua startup phase");

	luaL_checktype(L, 1, LUA_TFUNCTION);
	auto funcName = luaL_checkstring(L, 2);
	auto args = luaL_checkstring(L, 3);

	std::vector<CustomFunctionParam> argList;
	ParseCustomFunctionParams(L, args, argList, false);

	RegistryEntry func(L, 1);
	auto call = std::make_unique<CustomLuaCall>(funcName, argList, std::move(func));

	auto & functionMgr = gExtender->GetServer().Osiris().GetCustomFunctionManager();
	functionMgr.RegisterDynamic(std::move(call));
}

void NewQuery(lua_State * L)
{
	auto lua = State::FromLua(L);
	if (lua->StartupDone()) luaL_error(L, "Attempted to register query after Lua startup phase");

	luaL_checktype(L, 1, LUA_TFUNCTION);
	auto funcName = luaL_checkstring(L, 2);
	auto args = luaL_checkstring(L, 3);

	std::vector<CustomFunctionParam> argList;
	ParseCustomFunctionParams(L, args, argList, true);

	RegistryEntry func(L, 1);
	auto query = std::make_unique<CustomLuaQuery>(funcName, argList, std::move(func));

	auto & functionMgr = gExtender->GetServer().Osiris().GetCustomFunctionManager();
	functionMgr.RegisterDynamic(std::move(query));
}

void NewEvent(lua_State * L)
{
	auto lua = State::FromLua(L);
	if (lua->StartupDone()) luaL_error(L, "Attempted to register event after Lua startup phase");

	auto funcName = luaL_checkstring(L, 1);
	auto args = luaL_checkstring(L, 2);

	std::vector<CustomFunctionParam> argList;
	ParseCustomFunctionParams(L, args, argList, false);

	auto customEvt = std::make_unique<CustomEvent>(funcName, argList);

	auto & functionMgr = gExtender->GetServer().Osiris().GetCustomFunctionManager();
	functionMgr.RegisterDynamic(std::move(customEvt));
}

void RegisterOsirisLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"RegisterListener", LuaWrapFunction(&RegisterListener)},
		{"OsirisIsCallable", LuaWrapFunction(&IsCallable)},
		{"NewCall", LuaWrapFunction(&NewCall)},
		{"NewQuery", LuaWrapFunction(&NewQuery)},
		{"NewEvent", LuaWrapFunction(&NewEvent)},
		{0,0}
	};

	RegisterLib(L, "Osiris", lib);
}

END_NS()
