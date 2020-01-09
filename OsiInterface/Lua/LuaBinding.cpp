#include <stdafx.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include "LuaBinding.h"
#include "resource.h"
#include <fstream>

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



	char const * const LuaObjectProxy<esv::Status>::MetatableName = "LuaStatus";

	int LuaObjectProxy<esv::Status>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Status object no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto & propertyMap = StatusToPropertyMap(obj_);
		auto fetched = LuaPropertyMapGet(L, propertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaObjectProxy<esv::Status>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	int LuaStatGetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName);

	char const * const LuaObjectProxy<CRPGStats_Object>::MetatableName = "LuaCRPGStatsObjectProxy";

	int LuaObjectProxy<CRPGStats_Object>::LuaIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Name") == 0) {
			lua_pushstring(L, obj_->Name);
			return 1;
		}

		return LuaStatGetAttribute(L, obj_, attributeName);
	}

	int LuaObjectProxy<CRPGStats_Object>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	std::optional<int32_t> GetCharacterDynamicStat(CDivinityStats_Character * stats, char const * statName)
	{
		bool isBaseStat = strncmp(statName, "Base", 4) == 0;
		return stats->GetStat(isBaseStat ? (statName + 4) : statName, isBaseStat);
	}

	char const * const LuaObjectProxy<CDivinityStats_Character>::MetatableName = "LuaCharacterStats";

	int CharacterFetchStat(lua_State * L, CDivinityStats_Character * stats, char const * prop)
	{
		auto dynamicStat = GetCharacterDynamicStat(stats, prop);
		if (dynamicStat) {
			lua_pushinteger(L, *dynamicStat);
			return 1;
		}

		if (strncmp(prop, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(prop + 7);
			if (talentId) {
				bool hasTalent = stats->HasTalent(*talentId, false);
				lua_pushboolean(L, hasTalent);
				return 1;
			} else {
				return 0;
			}
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = stats->GetAbility(*abilityId, false);
			lua_pushinteger(L, abilityLevel);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterStatsPropertyMap, stats, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaObjectProxy<CDivinityStats_Character>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Character stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchStat(L, obj_, prop);
	}

	int LuaObjectProxy<CDivinityStats_Character>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}



	char const * const LuaStatusHandleProxy::MetatableName = "LuaStatusHandleProxy";

	int LuaStatusHandleProxy::LuaIndex(lua_State * L)
	{
		auto character = FindCharacterByHandle(character_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");
		
		auto status = character->GetStatusByHandle(status_, true);
		if (status == nullptr) return luaL_error(L, "Status handle invalid");

		auto prop = luaL_checkstring(L, 2);
		auto & propertyMap = StatusToPropertyMap(status);
		auto fetched = LuaPropertyMapGet(L, propertyMap, status, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaStatusHandleProxy::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaHandleProxy<CDivinityStats_Character>::MetatableName = "LuaCharacterStatsHandleProxy";

	int LuaHandleProxy<CDivinityStats_Character>::LuaIndex(lua_State * L)
	{
		auto character = FindCharacterByHandle(handle_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");
		if (character->Stats == nullptr) return luaL_error(L, "Character has no stats!");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchStat(L, character->Stats, prop);
	}

	int LuaHandleProxy<CDivinityStats_Character>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}



	char const * const LuaHandleProxy<esv::PlayerCustomData>::MetatableName = "LuaPlayerCustomDataHandleProxy";

	int LuaHandleProxy<esv::PlayerCustomData>::LuaIndex(lua_State * L)
	{
		auto character = FindCharacterByHandle(handle_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");

		if (character->PlayerData == nullptr
			|| !character->PlayerData->CustomData.Initialized) {
			OsiError("Character has no player data, or custom data was not initialized.");
			return 0;
		}

		auto prop = luaL_checkstring(L, 2);

		auto fetched = LuaPropertyMapGet(L, gPlayerCustomDataPropertyMap, &character->PlayerData->CustomData, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaHandleProxy<esv::PlayerCustomData>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaHandleProxy<esv::Character>::MetatableName = "LuaCharacterHandleProxy";

	int LuaHandleProxy<esv::Character>::LuaIndex(lua_State * L)
	{
		auto character = FindCharacterByHandle(handle_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "PlayerCustomData") == 0) {
			if (character->PlayerData != nullptr
				&& character->PlayerData->CustomData.Initialized) {
				LuaHandleProxy<esv::PlayerCustomData>::New(L, handle_);
				return 1;
			} else {
				OsiError("Character has no player data, or custom data was not initialized.");
				return 0;
			}
		}

		if (strcmp(prop, "Stats") == 0) {
			if (character->Stats != nullptr) {
				LuaHandleProxy<CDivinityStats_Character>::New(L, handle_);
				return 1;
			} else {
				OsiError("Character has no stats.");
				return 0;
			}
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterPropertyMap, character, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaHandleProxy<esv::Character>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaHandleProxy<esv::Item>::MetatableName = "LuaItemHandleProxy";

	int LuaHandleProxy<esv::Item>::LuaIndex(lua_State * L)
	{
		auto item = FindItemByHandle(handle_);
		if (item == nullptr) return luaL_error(L, "Item handle invalid");

		auto prop = luaL_checkstring(L, 2);
		bool fetched = false;
		if (item->StatsDynamic != nullptr) {
			fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item->StatsDynamic, prop, false);
		}

		if (!fetched) {
			fetched = LuaPropertyMapGet(L, gItemPropertyMap, item, prop, true);
		}

		return fetched ? 1 : 0;
	}

	int LuaHandleProxy<esv::Item>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}



	char const * const LuaStatsExtraDataProxy::MetatableName = "LuaStatsExtraDataProxy";

	int LuaStatsExtraDataProxy::LuaIndex(lua_State * L)
	{
		auto stats = gStaticSymbols.GetStats();
		if (stats == nullptr || stats->ExtraData == nullptr) return luaL_error(L, "Stats not available");

		auto key = luaL_checkstring(L, 2);
		auto extraData = stats->ExtraData->Properties.Find(key);
		if (extraData != nullptr) {
			lua_pushnumber(L, *extraData);
			return 1;
		} else {
			return 0;
		}
	}


	LuaExtensionLibrary::LuaExtensionLibrary()
	{
	}

	void LuaExtensionLibrary::Register(lua_State * L)
	{
		RegisterLib(L);
		LuaOsiFunctionNameProxy::RegisterMetatable(L);
		LuaObjectProxy<esv::Status>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Character>::RegisterMetatable(L);
		LuaObjectProxy<CRPGStats_Object>::RegisterMetatable(L);
		LuaHandleProxy<esv::Character>::RegisterMetatable(L);
		LuaHandleProxy<esv::PlayerCustomData>::RegisterMetatable(L);
		LuaHandleProxy<esv::Item>::RegisterMetatable(L);
		LuaStatusHandleProxy::RegisterMetatable(L);
		LuaHandleProxy<CDivinityStats_Character>::RegisterMetatable(L);
		LuaStatsExtraDataProxy::RegisterMetatable(L);
		RegisterNameResolverMetatable(L);
		CreateNameResolver(L);
	}

	int GetExtensionVersion(lua_State * L);
	int EnableStatOverride(lua_State * L);
	int OsiPrint(lua_State* L);
	int OsiPrintError(lua_State* L);
	int JsonParse(lua_State * L);
	int JsonStringify(lua_State * L);
	int IsModLoaded(lua_State* L);
	int GetModLoadOrder(lua_State* L);
	int GetModInfo(lua_State* L);
	int GetStatEntries(lua_State * L);
	int StatGetAttribute(lua_State * L);
	int StatSetAttribute(lua_State * L);
	int GetCharacter(lua_State * L);
	int GetItem(lua_State * L);
	int GetStatus(lua_State * L);
	int OsirisIsCallable(lua_State * L);
	int LuaRandom(lua_State * L);

	void LuaExtensionLibrary::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Version", GetExtensionVersion},
			{"Require", Require},
			{"NewCall", NewCall},
			{"NewQuery", NewQuery},
			{"NewEvent", NewEvent},
			{"EnableStatOverride", EnableStatOverride},
			{"Print", OsiPrint},
			{"PrintError", OsiPrintError},

			{"JsonParse", JsonParse},
			{"JsonStringify", JsonStringify},

			{"IsModLoaded", IsModLoaded},
			{"GetModLoadOrder", GetModLoadOrder},
			{"GetModInfo", GetModInfo},

			{"GetStatEntries", GetStatEntries},
			{"StatGetAttribute", StatGetAttribute},
			{"StatSetAttribute", StatSetAttribute},
			{"GetCharacter", GetCharacter},
			{"GetItem", GetItem},
			{"GetStatus", GetStatus},
			{"OsirisIsCallable", OsirisIsCallable},
			{"Random", LuaRandom},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}

	int LuaExtensionLibrary::Require(lua_State * L)
	{
		auto modGuid = luaL_checkstring(L, 1);
		auto fileName = luaL_checkstring(L, 2);
		ExtensionState::Get().LuaLoadModScript(modGuid, fileName);
		return 0;
	}


	// Unsafe libs (IO, OS, package, debug) removed
	static const luaL_Reg loadedlibs[] = {
	  {"_G", luaopen_base},
	  {LUA_TABLIBNAME, luaopen_table},
	  {LUA_STRLIBNAME, luaopen_string},
	  {LUA_MATHLIBNAME, luaopen_math},
	  // debug table is stripped in the sandbox startup script
	  {LUA_DBLIBNAME, luaopen_debug},
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

		auto sandbox = GetBuiltinLibrary();
		LoadScript(sandbox, "sandbox");

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_pushstring(L, "ExtraData"); // stack: Ext, "ExtraData"
		LuaStatsExtraDataProxy::New(L); // stack: Ext, "ExtraData", ExtraDataProxy
		lua_settable(L, -3); // stack: Ext
		lua_pop(L, 1); // stack: -
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
		status = CallWithTraceback(0, LUA_MULTRET);
		if (status != LUA_OK) {
			OsiError("Failed to execute script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1); // pop error message from the stack
			return false;
		}

		return true;
	}

	int LuaState::CallWithTraceback(int narg, int nres)
	{
		auto L = state_;
		int base = lua_gettop(L) - narg;  /* function index */
		lua_pushcfunction(L, &LuaState::TracebackHandler);  /* push message handler */
		lua_insert(L, base);  /* put it under function and args */
		int status = lua_pcall(L, narg, nres, base);
		lua_remove(L, base);  /* remove message handler from the stack */
		return status;
	}

	int LuaState::TracebackHandler(lua_State *L)
	{
		const char *msg = lua_tostring(L, 1);
		if (msg == NULL) {  /* is error object not a string? */
			if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
				lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
				return 1;  /* that is the message */
			else
				msg = lua_pushfstring(L, "(error object is a %s value)",
					luaL_typename(L, 1));
		}
		luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
		return 1;  /* return the traceback */
	}

	std::optional<int32_t> LuaState::StatusGetEnterChance(esv::Status * status, bool useCharacterStats)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAll & ~RestrictHandleConversion);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "StatusGetEnterChance"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto luaStatus = LuaObjectProxy<esv::Status>::New(L, status); // stack: fn, status
		LuaGameObjectPin<esv::Status> _(luaStatus);
		lua_pushboolean(L, useCharacterStats); // stack: fn, status, useCS

		if (CallWithTraceback(2, 1) != 0) { // stack: retval
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
		LuaRestriction restriction(*this, RestrictAll);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "GetHitChance"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto luaAttacker = LuaObjectProxy<CDivinityStats_Character>::New(L, attacker); // stack: fn, attacker
		LuaGameObjectPin<CDivinityStats_Character> _(luaAttacker);
		auto luaTarget = LuaObjectProxy<CDivinityStats_Character>::New(L, target); // stack: fn, attacker, target
		LuaGameObjectPin<CDivinityStats_Character> __(luaTarget);

		if (CallWithTraceback(2, 1) != 0) { // stack: retval
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


	bool LuaState::GetDescriptionParam(SkillPrototype * prototype, CDivinityStats_Character * character,
		ObjectSet<STDString> const & paramTexts, std::wstring & replacement)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAll);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_SkillGetDescriptionParam"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto stats = gStaticSymbols.GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return false;
		}

		auto skill = stats->objects.Find(prototype->RPGStatsObjectIndex);
		if (skill == nullptr) {
			OsiError("Invalid RPGStats index in SkillPrototype?");
			return false;
		}

		auto luaSkill = LuaObjectProxy<CRPGStats_Object>::New(L, skill); // stack: fn, skill
		LuaGameObjectPin<CRPGStats_Object> _(luaSkill);
		auto luaCharacter = LuaObjectProxy<CDivinityStats_Character>::New(L, character); // stack: fn, skill, character
		LuaGameObjectPin<CDivinityStats_Character> _2(luaCharacter);

		for (uint32_t i = 0; i < paramTexts.Set.Size; i++) {
			lua_pushstring(L, paramTexts.Set.Buf[i].GetPtr()); // stack: fn, skill, character, params...
		}

		if (CallWithTraceback(2 + paramTexts.Set.Size, 1) != 0) { // stack: retval
			OsiError("GetDescriptionParam handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok;
		if (isnil) {
			ok = false;
		} else {
			auto str = lua_tostring(L, -1);
			if (str != nullptr) {
				replacement = FromUTF8(str);
				ok = true;
			} else {
				OsiErrorS("GetDescriptionParam returned non-string value");
				ok = false;
			}
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}

	void LuaState::OnGameSessionLoading()
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAll | ScopeSessionLoad);

		identityAdapters_.UpdateAdapters();

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_OnGameSessionLoading"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn

		if (CallWithTraceback(0, 0) != 0) { // stack: -
			OsiError("Ext.OnGameSessionLoading failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	void LuaState::OnModuleLoading()
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAll | ScopeModuleLoad);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_OnModuleLoading"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn

		if (CallWithTraceback(0, 0) != 0) { // stack: -
			OsiError("Ext.OnModuleLoading failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	std::string LuaState::GetBuiltinLibrary()
	{
		auto hResource = FindResource(gThisModule, MAKEINTRESOURCE(IDR_LUA_BUILTIN_LIBRARY),
			L"LUA_SCRIPT");

		if (hResource) {
			auto hGlobal = LoadResource(gThisModule, hResource);
			if (hGlobal) {
				auto resourceData = LockResource(hGlobal);
				if (resourceData) {
					DWORD resourceSize = SizeofResource(gThisModule, hResource);
					std::string script;
					script.resize(resourceSize);
					memcpy(script.data(), resourceData, resourceSize);
					return script;
				}
			}
		}

		OsiErrorS("Could not find bootstrap resource!");
		return std::string();
	}

	void ExtensionState::OnGameSessionLoading()
	{
		LuaStatePin lua(ExtensionState::Get());
		if (lua) {
			lua->OnGameSessionLoading();
		}
	}

	void ExtensionState::OnModuleLoading()
	{
		LuaStatePin lua(ExtensionState::Get());
		if (lua) {
			lua->OnModuleLoading();
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
		if (!HasFeatureFlag("Lua")) {
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

		LuaRestriction restriction(*lua, LuaState::RestrictAll);
		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const & mod = mods.Buf[i];
			auto dir = ToUTF8(mod.Info.Directory.GetPtr());
			auto bootstrapFile = "Mods/" + dir + "/Story/RawFiles/Lua/Bootstrap.lua";
			auto reader = gStaticSymbols.MakeFileReader(bootstrapFile);
			if (reader.IsLoaded()) {
				std::string bootstrapScriptName;
				if (dir.length() > 37) {
					// Strip GUID from end of dir
					bootstrapScriptName = dir.substr(0, dir.length() - 37) + "/Bootstrap.lua";
				} else {
					bootstrapScriptName = dir + "/Bootstrap.lua";
				}

				LuaLoadGameFile(reader, bootstrapScriptName);
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

	void ExtensionState::LuaLoadGameFile(FileReaderPin & reader, std::string const & scriptName)
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

		lua->LoadScript(reader.ToString(), scriptName);
	}

	void ExtensionState::LuaLoadGameFile(std::string const & path, std::string const & scriptName)
	{
		auto reader = gStaticSymbols.MakeFileReader(path);
		if (!reader.IsLoaded()) {
			OsiError("Script file could not be opened: " << path);
			return;
		}

		LuaLoadGameFile(reader, scriptName.empty() ? path : scriptName);
		OsiMsg("Loaded game script: " << path);
	}

	void ExtensionState::LuaLoadModScript(std::string const & modNameGuid, std::string const & fileName)
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

		std::string scriptName = ToUTF8(mod->Info.Directory.GetPtr());
		if (scriptName.length() > 37) {
			// Strip GUID from end of dir
			scriptName = scriptName.substr(0, scriptName.length() - 37);
		}
		scriptName += "/" + fileName;

		LuaLoadGameFile(path, scriptName);
	}
}
