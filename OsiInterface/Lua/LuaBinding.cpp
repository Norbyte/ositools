#include <stdafx.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include "LuaBinding.h"
#include <fstream>
#include <json/json.h>

namespace osidbg
{
	LuaRegistryEntry::LuaRegistryEntry(lua_State * L, int index)
		: L_(L)
	{
		lua_pushvalue(L, index);
		ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	LuaRegistryEntry::~LuaRegistryEntry()
	{
		if (ref_ != -1) {
			luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
		}
	}

	LuaRegistryEntry::LuaRegistryEntry(LuaRegistryEntry && other)
		: L_(other.L_), ref_(other.ref_)
	{
		other.ref_ = -1;
	}

	LuaRegistryEntry & LuaRegistryEntry::operator = (LuaRegistryEntry && other)
	{
		L_ = other.L_;
		ref_ = other.ref_;
		other.ref_ = -1;
		return *this;
	}

	void LuaRegistryEntry::Push()
	{
		assert(ref_ != -1);
		lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);
	}



	char const * const LuaGameObjectProxy<esv::Status>::MetatableName = "LuaStatus";

	int LuaGameObjectProxy<esv::Status>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Status object no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto & propertyMap = StatusToPropertyMap(obj_);
		auto fetched = LuaPropertyMapGet(L, propertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaGameObjectProxy<esv::Status>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	std::optional<int32_t> GetCharacterDynamicStat(CDivinityStats_Character * stats, char const * statName)
	{
		bool isBaseStat = strncmp(statName, "Base", 4) == 0;
		return stats->GetStat(isBaseStat ? (statName + 4) : statName, isBaseStat);
	}

	char const * const LuaGameObjectProxy<CDivinityStats_Character>::MetatableName = "LuaCharacterStats";

	int LuaGameObjectProxy<CDivinityStats_Character>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Character stats no longer available");

		auto prop = luaL_checkstring(L, 2);

		auto dynamicStat = GetCharacterDynamicStat(obj_, prop);
		if (dynamicStat) {
			lua_pushinteger(L, *dynamicStat);
			return 1;
		}

		if (strncmp(prop, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(prop + 7);
			if (talentId) {
				bool hasTalent = obj_->HasTalent(*talentId, false);
				lua_pushboolean(L, hasTalent);
				return 1;
			} else {
				return 0;
			}
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = obj_->GetAbility(*abilityId, false);
			lua_pushinteger(L, abilityLevel);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterStatsPropertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaGameObjectProxy<CDivinityStats_Character>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	LuaExtensionLibrary::LuaExtensionLibrary()
	{
	}

	void LuaExtensionLibrary::Register(lua_State * L)
	{
		RegisterLib(L);
		LuaOsiFunctionNameProxy::RegisterMetatable(L);
		LuaGameObjectProxy<esv::Status>::RegisterMetatable(L);
		LuaGameObjectProxy<CDivinityStats_Character>::RegisterMetatable(L);
		RegisterNameResolverMetatable(L);
		CreateNameResolver(L);
	}

	int EnableStatOverride(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & LuaState::RestrictExt) {
			return luaL_error(L, "Attempted to toggle stat overrides in restricted context");
		}

		auto stat = luaL_checkstring(L, 1);
		gOsirisProxy->GetFunctionLibrary().EnableStatOverride(stat);
		return 0;
	}

	int OsiPrint(lua_State* L)
	{
		int nargs = lua_gettop(L);  /* number of arguments */

		std::stringstream ss;
		lua_getglobal(L, "tostring");
		for (int i = 1; i <= nargs; i++) {
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */
			lua_call(L, 1, 1);
			const char * str = lua_tostring(L, -1);  /* get result */
			if (str == nullptr)
				return luaL_error(L, "'tostring' must return a string to 'print'");
			if (i > 1) ss << "\t";
			ss << str;
			lua_pop(L, 1);  /* pop result */
		}

		gOsirisProxy->LogOsirisMsg(ss.str());
		return 0;
	}

	void JsonParse(lua_State * L, Json::Value & val);

	void JsonParseArray(lua_State * L, Json::Value & val)
	{
		lua_newtable(L);
		int idx = 1;
		for (auto it = val.begin(), end = val.end(); it != end; ++it) {
			lua_pushinteger(L, idx++);
			JsonParse(L, *it);
			lua_settable(L, -3);
		}
	}

	void JsonParseObject(lua_State * L, Json::Value & val)
	{
		lua_newtable(L);
		for (auto it = val.begin(), end = val.end(); it != end; ++it) {
			JsonParse(L, it.key());
			JsonParse(L, *it);
			lua_settable(L, -3);
		}
	}

	void JsonParse(lua_State * L, Json::Value & val)
	{
		switch (val.type()) {
		case Json::nullValue:
			lua_pushnil(L);
			break;

		case Json::intValue:
			lua_pushinteger(L, val.asInt64());
			break;

		case Json::uintValue:
			lua_pushinteger(L, (int64_t)val.asUInt64());
			break;

		case Json::realValue:
			lua_pushnumber(L, val.asDouble());
			break;

		case Json::stringValue:
			lua_pushstring(L, val.asCString());
			break;

		case Json::booleanValue:
			lua_pushboolean(L, val.asBool() ? 1 : 0);
			break;

		case Json::arrayValue:
			JsonParseArray(L, val);
			break;

		case Json::objectValue:
			JsonParseObject(L, val);
			break;

		default:
			luaL_error(L, "Attempted to parse unknown Json value");
		}
	}
	

	int JsonParse(lua_State * L)
	{
		size_t length;
		auto json = luaL_checklstring(L, 1, &length);

		Json::CharReaderBuilder factory;
		std::unique_ptr<Json::CharReader> reader(factory.newCharReader());

		Json::Value root;
		std::string errs;
		if (!reader->parse(json, json + length, &root, &errs)) {
			return luaL_error(L, "Unable to parse JSON: %s", errs.c_str());
		}

		JsonParse(L, root);
		return 1;
	}

	Json::Value JsonStringify(lua_State * L, int index);

	Json::Value JsonStringifyObject(lua_State * L, int index)
	{
		Json::Value arr(Json::objectValue);
		lua_pushnil(L);

		if (index < 0) index--;

		while (lua_next(L, index) != 0) {
			Json::Value val(JsonStringify(L, -1));

			if (lua_type(L, -2) == LUA_TSTRING) {
				auto key = lua_tostring(L, -2);
				arr[key] = val;
			} else if (lua_type(L, -2) == LUA_TNUMBER) {
				lua_pushvalue(L, -2);
				auto key = lua_tostring(L, -1);
				arr[key] = val;
				lua_pop(L, 1);
			} else {
				throw std::runtime_error("Can only stringify string or number table keys");
			}

			lua_pop(L, 1);
		}

		return arr;
	}

	Json::Value JsonStringifyArray(lua_State * L, int index)
	{
		Json::Value arr(Json::arrayValue);
		lua_pushnil(L);

		if (index < 0) index--;

		while (lua_next(L, index) != 0) {
			arr.append(JsonStringify(L, -1));
			lua_pop(L, 1);
		}

		return arr;
	}

	bool JsonCanStringifyAsArray(lua_State * L, int index)
	{
		Json::Value arr(Json::objectValue);
		lua_pushnil(L);

		if (index < 0) index--;

		int next = 1;
		bool isArray = true;
		while (lua_next(L, index) != 0) {
			if (lua_isinteger(L, -2)) {
				auto key = lua_tointeger(L, -2);
				if (key != next++) {
					isArray = false;
				}
			} else {
				isArray = false;
			}

			lua_pop(L, 1);
		}

		return isArray;
	}

	Json::Value JsonStringifyTable(lua_State * L, int index)
	{
		if (JsonCanStringifyAsArray(L, index)) {
			return JsonStringifyArray(L, index);
		} else {
			return JsonStringifyObject(L, index);
		}
	}


	Json::Value JsonStringify(lua_State * L, int index)
	{
		switch (lua_type(L, index)) {
			case LUA_TNIL:
				return Json::Value(Json::nullValue);

			case LUA_TBOOLEAN:
				return Json::Value(lua_toboolean(L, index) == 1);

			case LUA_TNUMBER:
				if (lua_isinteger(L, index)) {
					return Json::Value(lua_tointeger(L, index));
				} else {
					return Json::Value(lua_tonumber(L, index));
				}

			case LUA_TSTRING:
				return Json::Value(lua_tostring(L, index));

			case LUA_TTABLE:
				return JsonStringifyTable(L, index);

			case LUA_TLIGHTUSERDATA:
			case LUA_TFUNCTION:
			case LUA_TUSERDATA:
			case LUA_TTHREAD:
			default:
				throw std::runtime_error("Attempted to stringify a lightuserdata, userdata, function or thread value");
		}
	}


	int JsonStringify(lua_State * L)
	{
		int nargs = lua_gettop(L);
		if (nargs != 1) {
			return luaL_error(L, "JsonStringify expects exactly one parameter.");
		}

		Json::Value root;
		try {
			root = JsonStringify(L, 1);
		} catch (std::runtime_error & e) {
			return luaL_error(L, "%s", e.what());
		}

		Json::StreamWriterBuilder builder;
		builder["indentation"] = "\t";
		std::stringstream ss;
		std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(root, &ss);

		int nargs2 = lua_gettop(L);
		lua_pushstring(L, ss.str().c_str());
		return 1;
	}


	void LuaExtensionLibrary::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Require", Require},
			{"NewCall", NewCall},
			{"NewQuery", NewQuery},
			{"NewEvent", NewEvent},
			{"EnableStatOverride", EnableStatOverride},
			{"Print", OsiPrint},
			{"JsonParse", JsonParse},
			{"JsonStringify", JsonStringify},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}

	int LuaExtensionLibrary::Require(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & LuaState::RestrictExt) {
			return luaL_error(L, "Attempted to load Lua code in restricted context");
		}

		auto modGuid = luaL_checkstring(L, 1);
		auto fileName = luaL_checkstring(L, 2);
		ExtensionState::Get().LuaLoadGameFile(modGuid, fileName);
		return 0;
	}


	// Unsafe libs (IO, OS, package, debug) removed
	static const luaL_Reg loadedlibs[] = {
	  {"_G", luaopen_base},
	  {LUA_TABLIBNAME, luaopen_table},
	  {LUA_STRLIBNAME, luaopen_string},
	  {LUA_MATHLIBNAME, luaopen_math},
	  {NULL, NULL}
	};

	int LuaPanic(lua_State * L)
	{
		throw LuaException();
	}

	LuaState::LuaState()
		: identityAdapters_(gOsirisProxy->GetGlobals())
	{
		state_ = luaL_newstate();
		lua_atpanic(state_, &LuaPanic);
		OpenLibs();

		identityAdapters_.UpdateAdapters();

		proxy_.Register(state_);

		auto sandbox = R"(
dofile = function ()
	error("dofile() has been disabled for security reasons")
end

loadfile = function ()
	error("loadfile() has been disabled for security reasons")
end

load = function ()
	error("load() has been disabled for security reasons")
end

loadstring = function ()
	error("loadstring() has been disabled for security reasons")
end

rawget = function ()
	error("rawget() has been disabled for security reasons")
end

rawset = function ()
	error("rawset() has been disabled for security reasons")
end

rawequal = function ()
	error("rawequal() has been disabled for security reasons")
end

debug = nil
)";
		LoadScript(sandbox, "sandbox");
	}

	LuaState::~LuaState()
	{
		if (gOsirisProxy) {
			gOsirisProxy->GetCustomFunctionManager().ClearDynamicEntries();
		}

		lua_close(state_);
	}

	void LuaState::FinishStartup()
	{
		assert(!startupDone_);
		startupDone_ = true;
	}
		
	void LuaState::OpenLibs()
	{
		const luaL_Reg *lib;
		/* "require" functions from 'loadedlibs' and set results to global table */
		for (lib = loadedlibs; lib->func; lib++) {
			luaL_requiref(state_, lib->name, lib->func, 1);
			lua_pop(state_, 1);  /* remove lib */
		}
	}

	bool LuaState::LoadScript(std::string const & script, std::string const & name)
	{
		std::lock_guard lock(mutex_);

		/* Load the file containing the script we are going to run */
		int status = luaL_loadbufferx(state_, script.c_str(), script.size(), name.c_str(), "text");
		if (status != LUA_OK) {
			OsiError("Failed to parse script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1);  /* pop error message from the stack */
			return false;
		}

		/* Ask Lua to run our little script */
		status = lua_pcall(state_, 0, LUA_MULTRET, 0);
		if (status != LUA_OK) {
			OsiError("Failed to execute script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1); // pop error message from the stack
			return false;
		}

		return true;
	}

	std::optional<int32_t> LuaState::StatusGetEnterChance(esv::Status * status, bool useCharacterStats, float chanceMultiplier)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, LuaState::RestrictOsiris);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "StatusGetEnterChance"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto luaStatus = LuaGameObjectProxy<esv::Status>::New(L, status); // stack: fn, status
		LuaGameObjectPin<esv::Status> _(luaStatus);
		lua_pushboolean(L, useCharacterStats);
		lua_pushnumber(L, chanceMultiplier); // stack: fn, status, useCS, chanceMul

		if (lua_pcall(L, 3, 1, 0) != 0) { // stack: retval
			OsiError("StatusGetEnterChance handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return {};
		}

		int isnum;
		int isnil = lua_isnil(L, -1);
		auto retval = lua_tointegerx(L, -1, &isnum);
		lua_pop(L, 1); // stack: -

		if (isnum) {
			return std::clamp((int32_t)retval, 0, 100);
		} else {
			if (!isnil) {
				OsiError("StatusGetEnterChance returned non-integer value");
			}

			return {};
		}
	}

	std::optional<int32_t> LuaState::GetHitChance(CDivinityStats_Character * attacker, CDivinityStats_Character * target)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, LuaState::RestrictOsiris | LuaState::RestrictExt);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "GetHitChance"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto luaAttacker = LuaGameObjectProxy<CDivinityStats_Character>::New(L, attacker); // stack: fn, attacker
		LuaGameObjectPin<CDivinityStats_Character> _(luaAttacker);
		auto luaTarget = LuaGameObjectProxy<CDivinityStats_Character>::New(L, target); // stack: fn, attacker, target
		LuaGameObjectPin<CDivinityStats_Character> __(luaTarget);

		if (lua_pcall(L, 2, 1, 0) != 0) { // stack: retval
			OsiError("GetHitChance handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return {};
		}

		int isnum;
		int isnil = lua_isnil(L, -1);
		auto retval = lua_tointegerx(L, -1, &isnum);
		lua_pop(L, 1); // stack: -

		if (isnum) {
			return std::clamp((int32_t)retval, 0, 100);
		} else {
			if (!isnil) {
				OsiErrorS("GetHitChance returned non-integer value");
			}

			return {};
		}
	}


	void ExtensionState::StoryLoaded()
	{
		if (Lua) {
			Lua->StoryLoaded();
		}
	}

	void ExtensionState::StoryFunctionMappingsUpdated()
	{
		if (Lua) {
			Lua->StoryFunctionMappingsUpdated();
		}
	}


	void ExtensionState::IncLuaRefs()
	{
		assert(Lua);
		LuaRefs++;
	}

	void ExtensionState::DecLuaRefs()
	{
		assert(LuaRefs > 0);
		LuaRefs--;
		if (LuaRefs == 0 && LuaPendingDelete) {
			LuaResetInternal();
		}
	}

	void ExtensionState::LuaReset(bool startup)
	{
		if (!EnableLua) {
			OsiWarn("Lua extensions not enabled; not initializing Lua VM");
			return;
		}

		if (LuaPendingDelete) {
			OsiWarn("State delete is already pending!");
		}

		LuaPendingDelete = true;
		if (startup) {
			LuaPendingStartup = true;
		}

		if (LuaRefs == 0) {
			LuaResetInternal();
		} else {
			OsiWarn("Lua state deletion deferred (" << LuaRefs << " references still alive)");
		}
	}

	void ExtensionState::LuaResetInternal()
	{
		assert(LuaPendingDelete);
		assert(LuaRefs == 0);

		LuaPendingDelete = false;

		// Destroy previous instance first to make sure that no dangling
		// references are made to the old state while constructing the new
		Lua.reset();
		Lua = std::make_unique<LuaState>();
		Lua->StoryFunctionMappingsUpdated();
		OsiWarn("LUA VM reset.");

		if (LuaPendingStartup) {
			LuaPendingStartup = false;
			LuaStartup();
		}
	}

	void ExtensionState::LuaStartup()
	{
		LuaStatePin lua(*this);
		if (!lua) {
			OsiErrorS("Called when the Lua VM has not been initialized!");
			return;
		}

		auto modManager = GetModManager();
		if (modManager == nullptr) {
			OsiErrorS("Could not bootstrap Lua modules - mod manager not available");
			return;
		}

		auto & mods = modManager->BaseModule.LoadOrderedModules.Set;

		LuaRestriction restriction(*lua, LuaState::RestrictOsiris);
		OsiWarnS("Bootstrapping Lua modules ...");
		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const & mod = mods.Buf[i];
			auto dir = ToUTF8(mod.Info.Directory.GetPtr());
			auto bootstrapFile = "Mods/" + dir + "/Story/RawFiles/Lua/Bootstrap.lua";
			auto reader = gOsirisProxy->GetLibraryManager().MakeFileReader(bootstrapFile);
			if (reader.IsLoaded()) {
				OsiWarn("Found bootstrap file: " << bootstrapFile);
				LuaLoadGameFile(reader, bootstrapFile);
			}
		}
		
		lua->FinishStartup();
	}

	void ExtensionState::LuaLoadExternalFile(std::string const & path)
	{
		std::ifstream f(path, std::ios::in | std::ios::binary);
		if (!f.good()) {
			OsiError("File does not exist: " << path);
			return;
		}

		f.seekg(0, std::ios::end);
		auto length = f.tellg();
		f.seekg(0, std::ios::beg);
		std::string s(length, '\0');
		f.read(const_cast<char *>(s.data()), length);
		f.close();

		LuaStatePin lua(*this);
		if (!lua) {
			OsiErrorS("Called when the Lua VM has not been initialized!");
			return;
		}

		lua->LoadScript(s, path);
		OsiWarn("Loaded external script: " << path);
	}

	void ExtensionState::LuaLoadGameFile(FileReaderPin & reader, std::string const & path)
	{
		if (!reader.IsLoaded()) {
			OsiErrorS("Attempted to load script from invalid file reader");
			return;
		}

		LuaStatePin lua(*this);
		if (!lua) {
			OsiErrorS("Called when the Lua VM has not been initialized!");
			return;
		}

		lua->LoadScript(reader.ToString(), path);
	}

	void ExtensionState::LuaLoadGameFile(std::string const & path)
	{
		auto reader = gOsirisProxy->GetLibraryManager().MakeFileReader(path);
		if (!reader.IsLoaded()) {
			OsiError("Script file could not be opened: " << path);
			return;
		}

		LuaLoadGameFile(reader, path);
		OsiWarn("Loaded game script: " << path);
	}

	void ExtensionState::LuaLoadGameFile(std::string const & modNameGuid, std::string const & fileName)
	{
		auto mod = GetModManager()->FindModByNameGuid(modNameGuid.c_str());
		if (mod == nullptr) {
			OsiError("Mod does not exist or is not loaded: " << modNameGuid);
			return;
		}

		std::string path("Mods/");
		path += ToUTF8(mod->Info.Directory.GetPtr());
		path += "/Story/RawFiles/Lua/";
		path += fileName;

		LuaLoadGameFile(path);
	}
}
