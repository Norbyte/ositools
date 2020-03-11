#include <stdafx.h>
#include <LuaBindingClient.h>
#include <OsirisProxy.h>
#include <ExtensionStateClient.h>
#include "resource.h"

namespace osidbg
{
	LuaExtensionLibraryClient::LuaExtensionLibraryClient()
	{
	}

	void LuaExtensionLibraryClient::Register(lua_State * L)
	{
		RegisterLib(L);
		LuaObjectProxy<esv::Status>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Character>::RegisterMetatable(L);
		LuaObjectProxy<CharacterDynamicStat>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Item>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Equipment_Attributes>::RegisterMetatable(L);
		LuaHandleProxy<esv::Character>::RegisterMetatable(L);
		LuaHandleProxy<esv::PlayerCustomData>::RegisterMetatable(L);
		LuaHandleProxy<esv::Item>::RegisterMetatable(L);
		LuaHandleProxy<CDivinityStats_Character>::RegisterMetatable(L);
		LuaHandleProxy<CDivinityStats_Item>::RegisterMetatable(L);
		LuaStatsExtraDataProxy::RegisterMetatable(L);
		LuaStatsProxy::RegisterMetatable(L);
		LuaSkillPrototypeProxy::RegisterMetatable(L);
		LuaDamageList::RegisterMetatable(L);
	}

	int GetExtensionVersion(lua_State * L);
	int OsiPrint(lua_State* L);
	int OsiPrintError(lua_State* L);
	int JsonParse(lua_State * L);
	int JsonStringify(lua_State * L);
	int IsModLoaded(lua_State* L);
	int GetModLoadOrder(lua_State* L);
	int GetModInfo(lua_State* L);
	int GetStatEntries(lua_State * L);
	int GetSkillSet(lua_State * L);
	int GetEquipmentSet(lua_State * L);
	int StatGetAttribute(lua_State * L);
	int StatSetAttribute(lua_State * L);
	int StatAddCustomDescription(lua_State * L);
	int StatSetLevelScaling(lua_State * L);
	int GetStat(lua_State * L);
	int NewDamageList(lua_State * L);
	int OsirisIsCallableClient(lua_State * L);
	int LuaRandom(lua_State * L);
	int LuaRound(lua_State * L);



	void LuaExtensionLibraryClient::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Version", GetExtensionVersion},
			{"Require", Require},
			{"Print", OsiPrint},
			{"PrintError", OsiPrintError},

			{"JsonParse", JsonParse},
			{"JsonStringify", JsonStringify},

			{"IsModLoaded", IsModLoaded},
			{"GetModLoadOrder", GetModLoadOrder},
			{"GetModInfo", GetModInfo},

			{"GetStatEntries", GetStatEntries},
			{"GetSkillSet", GetSkillSet},
			{"GetEquipmentSet", GetEquipmentSet},
			{"StatGetAttribute", StatGetAttribute},
			{"StatSetAttribute", StatSetAttribute},
			{"StatAddCustomDescription", StatAddCustomDescription},
			{"StatSetLevelScaling", StatSetLevelScaling},
			{"GetStat", GetStat},

			{"NewDamageList", NewDamageList},
			{"OsirisIsCallable", OsirisIsCallableClient},
			{"Random", LuaRandom},
			{"Round", LuaRound},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}

	int LuaExtensionLibraryClient::Require(lua_State * L)
	{
		auto modGuid = luaL_checkstring(L, 1);
		auto fileName = luaL_checkstring(L, 2);
		ExtensionStateClient::Get().LuaLoadModScript(modGuid, fileName);
		return 0;
	}


	LuaStateClient::LuaStateClient()
	{
		library_.Register(state_);

		auto baseLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY);
		LoadScript(baseLib, "BuiltinLibrary.lua");
		auto clientLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY_CLIENT);
		LoadScript(clientLib, "BuiltinLibraryClient.lua");

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_pushstring(L, "ExtraData"); // stack: Ext, "ExtraData"
		LuaStatsExtraDataProxy::New(L); // stack: Ext, "ExtraData", ExtraDataProxy
		lua_settable(L, -3); // stack: Ext
		lua_pop(L, 1); // stack: -

		// Ext is not writeable after loading SandboxStartup!
		auto sandbox = GetBuiltinLibrary(IDR_LUA_SANDBOX_STARTUP);
		LoadScript(sandbox, "SandboxStartup.lua");
	}

	bool LuaStateClient::SkillGetDescriptionParam(SkillPrototype * prototype, CDivinityStats_Character * character,
		ObjectSet<STDString> const & paramTexts, std::wstring & replacement)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_SkillGetDescriptionParam"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return false;
		}

		auto skill = stats->objects.Find(prototype->RPGStatsObjectIndex);
		if (skill == nullptr) {
			OsiError("Invalid RPGStats index in SkillPrototype?");
			return false;
		}

		auto luaSkill = LuaStatsProxy::New(L, skill, std::optional<int32_t>()); // stack: fn, skill
		LuaStatsPin _(luaSkill);
		auto luaCharacter = LuaObjectProxy<CDivinityStats_Character>::New(L, character); // stack: fn, skill, character
		LuaGameObjectPin<CDivinityStats_Character> _2(luaCharacter);

		for (uint32_t i = 0; i < paramTexts.Set.Size; i++) {
			lua_pushstring(L, paramTexts[i].GetPtr()); // stack: fn, skill, character, params...
		}

		if (CallWithTraceback(2 + paramTexts.Set.Size, 1) != 0) { // stack: retval
			OsiError("SkillGetDescriptionParam handler failed: " << lua_tostring(L, -1));
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
				OsiErrorS("SkillGetDescriptionParam returned non-string value");
				ok = false;
			}
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}


	bool LuaStateClient::StatusGetDescriptionParam(StatusPrototype * prototype, CDivinityStats_Character * statusSource,
		CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, std::wstring & replacement)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_StatusGetDescriptionParam"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return false;
		}

		auto status = stats->objects.Find(prototype->RPGStatsObjectIndex);
		if (status == nullptr) {
			OsiError("Invalid RPGStats index in StatusPrototype?");
			return false;
		}

		if (character == nullptr) {
			character = statusSource;
		}

		auto luaStatus = LuaStatsProxy::New(L, status, std::optional<int32_t>()); // stack: fn, status
		LuaStatsPin _(luaStatus);

		auto luaSrcCharacter = LuaObjectProxy<CDivinityStats_Character>::New(L, statusSource); // stack: fn, status, srcCharacter, character
		LuaGameObjectPin<CDivinityStats_Character> _2(luaSrcCharacter);

		auto luaCharacter = LuaObjectProxy<CDivinityStats_Character>::New(L, character); // stack: fn, status, srcCharacter, character
		LuaGameObjectPin<CDivinityStats_Character> _3(luaCharacter);

		for (uint32_t i = 0; i < paramTexts.Set.Size; i++) {
			lua_pushstring(L, paramTexts[i].GetPtr()); // stack: fn, status, srcCharacter, character, params...
		}

		if (CallWithTraceback(3 + paramTexts.Set.Size, 1) != 0) { // stack: retval
			OsiError("StatusGetDescriptionParam handler failed: " << lua_tostring(L, -1));
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
				OsiErrorS("StatusGetDescriptionParam returned non-string value");
				ok = false;
			}
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}


	ExtensionStateClient & ExtensionStateClient::Get()
	{
		return gOsirisProxy->GetClientExtensionState();
	}


	LuaState * ExtensionStateClient::GetLua()
	{
		if (Lua) {
			return Lua.get();
		} else {
			return nullptr;
		}
	}

	ModManager * ExtensionStateClient::GetModManager()
	{
		return GetModManagerClient();
	}

	void ExtensionStateClient::DoLuaReset()
	{
		Lua.reset();
		Lua = std::make_unique<LuaStateClient>();
	}

	void ExtensionStateClient::LuaStartup()
	{
		ExtensionState::LuaStartup();

		LuaClientPin lua(*this);
		auto gameState = GetStaticSymbols().GetClientState();
		if (gameState
			&& (*gameState == ClientGameState::LoadLevel
				|| (*gameState == ClientGameState::LoadModule && WasStatLoadTriggered())
				|| *gameState == ClientGameState::LoadSession
				|| *gameState == ClientGameState::LoadGMCampaign
				|| *gameState == ClientGameState::Paused
				|| *gameState == ClientGameState::PrepareRunning
				|| *gameState == ClientGameState::Running
				|| *gameState == ClientGameState::GameMasterPause)) {
			lua->OnModuleResume();
			OsiWarn("Client resume -- state " << (unsigned)*gameState);
		} else {
			OsiWarn("Client NO resume -- state " << (unsigned)*gameState);
		}
	}

}
