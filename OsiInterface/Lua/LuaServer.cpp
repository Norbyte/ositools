#include <stdafx.h>
#include <Lua/LuaBindingServer.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include "resource.h"

namespace dse::lua
{
	char const * const ObjectProxy<esv::Status>::MetatableName = "esv::Status";

	int ObjectProxy<esv::Status>::Index(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Status object no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto & propertyMap = StatusToPropertyMap(obj_);
		auto fetched = LuaPropertyMapGet(L, propertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<esv::Status>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const StatusHandleProxy::MetatableName = "esv::HStatus";

	int StatusHandleProxy::Index(lua_State * L)
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

	int StatusHandleProxy::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const HandleProxy<esv::PlayerCustomData>::MetatableName = "esv::HPlayerCustomData";

	int HandleProxy<esv::PlayerCustomData>::Index(lua_State * L)
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

	int HandleProxy<esv::PlayerCustomData>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const HandleProxy<esv::Character>::MetatableName = "esv::HCharacter";

	int CharacterFetchProperty(lua_State * L, esv::Character * character, char const * prop);

	int HandleProxy<esv::Character>::Index(lua_State * L)
	{
		auto character = FindCharacterByHandle(handle_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchProperty(L, character, prop);
	}

	int HandleProxy<esv::Character>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const HandleProxy<esv::Item>::MetatableName = "esv::HItem";

	int HandleProxy<esv::Item>::Index(lua_State * L)
	{
		auto item = FindItemByHandle(handle_);
		if (item == nullptr) return luaL_error(L, "Item handle invalid");

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "Stats") == 0) {
			if (item->StatsDynamic != nullptr) {
				HandleProxy<CDivinityStats_Item>::New(L, handle_);
				return 1;
			} else {
				OsiError("Item has no stats.");
				return 0;
			}
		}

		bool fetched = false;
		if (item->StatsDynamic != nullptr) {
			fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item->StatsDynamic, prop, false);
		}

		if (!fetched) {
			fetched = LuaPropertyMapGet(L, gItemPropertyMap, item, prop, true);
		}

		return fetched ? 1 : 0;
	}

	int HandleProxy<esv::Item>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const TurnManagerCombatProxy::MetatableName = "esv::TurnManager::Combat";

	void TurnManagerCombatProxy::PopulateMetatable(lua_State * L)
	{
		lua_newtable(L);

		lua_pushcfunction(L, &GetCurrentTurnOrder);
		lua_setfield(L, -2, "GetCurrentTurnOrder");

		lua_pushcfunction(L, &GetNextTurnOrder);
		lua_setfield(L, -2, "GetNextTurnOrder");

		lua_pushcfunction(L, &UpdateCurrentTurnOrder);
		lua_setfield(L, -2, "UpdateCurrentTurnOrder");

		lua_pushcfunction(L, &UpdateNextTurnOrder);
		lua_setfield(L, -2, "UpdateNextTurnOrder");

		lua_pushcfunction(L, &GetAllTeams);
		lua_setfield(L, -2, "GetAllTeams");

		lua_setfield(L, -2, "__index");
	}

	void CombatTeamListToLua(lua_State * L, CompactSet<esv::TurnManager::CombatTeam *, MSVCMemoryAllocator> const & teams)
	{
		lua_newtable(L);

		for (uint32_t i = 0; i < teams.Size; i++) {
			push(L, i + 1);
			TurnManagerTeamProxy::New(L, teams[i]->TeamId);
			lua_settable(L, -3);
		}
	}

	int TurnManagerCombatProxy::Index(lua_State * L)
	{
		auto combat = Get();
		if (combat == nullptr) return luaL_error(L, "Combat no longer available");

		auto prop = luaL_checkstring(L, 2);
		if (strcmp(prop, "CombatId") == 0) {
			push(L, combatId_);
		} else if (strcmp(prop, "LevelName") == 0) {
			push(L, combat->LevelName.Str);
		} else if (strcmp(prop, "IsActive") == 0) {
			push(L, combat->IsActive);
		} else {
			OsiError("Combat has no attribute named " << prop);
			return 0;
		}

		return 1;
	}

	int TurnManagerCombatProxy::GetCurrentTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		CombatTeamListToLua(L, combat->CurrentRoundTeams.Set);
		return 1;
	}

	int TurnManagerCombatProxy::GetNextTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		CombatTeamListToLua(L, combat->NextRoundTeams.Set);
		return 1;
	}

	void UpdateTurnOrder(lua_State * L, uint8_t combatId, int index,
		ObjectSet<esv::TurnManager::CombatTeam *, MSVCMemoryAllocator> & combatTeams,
		ObjectSet<eoc::CombatTeamId> & combatNotifies)
	{
		if (lua_type(L, index) != LUA_TTABLE) luaL_error(L, "UpdateTurnOrder() expects a table of teams as the first argument");

		std::vector<esv::TurnManager::CombatTeam *> teams;
		std::unordered_set<eoc::CombatTeamId> notifies;

		lua_pushnil(L);
		if (index < 0) index--;

		while (lua_next(L, index) != 0) {
			auto luaTeam = TurnManagerTeamProxy::CheckUserData(L, -1);
			if (luaTeam->TeamId().CombatId != combatId) {
				luaL_error(L, "Tried to add team from another combat");
			}

			auto team = luaTeam->Get();
			if (team == nullptr) {
				luaL_error(L, "Tried to add nonexistent team");
			}

			teams.push_back(team);
			notifies.insert(team->TeamId);
			lua_pop(L, 1);
		}

		for (uint32_t i = 0; i < combatTeams.Set.Size; i++) {
			notifies.insert(combatTeams[i]->TeamId);
		}

		for (uint32_t i = 0; i < combatNotifies.Set.Size; i++) {
			notifies.insert(combatNotifies[i]);
		}

		combatTeams.Set.Size = 0;
		for (auto const & team : teams) {
			combatTeams.Set.Add(team);
		}

		combatNotifies.Set.Size = 0;
		for (auto const & teamId : notifies) {
			combatNotifies.Set.Add(teamId);
		}

		auto protocol = GetTurnBasedProtocol();
		if (protocol != nullptr) {
			// FIXME - filter duplicates
			protocol->UpdatedTurnOrderCombatIds.Set.Add(combatId);
		}
	}

	int TurnManagerCombatProxy::UpdateCurrentTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		UpdateTurnOrder(L, self->combatId_, 2, combat->CurrentRoundTeams, combat->CurrentTurnChangeNotificationTeamIds);
		return 0;
	}

	int TurnManagerCombatProxy::UpdateNextTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		UpdateTurnOrder(L, self->combatId_, 2, combat->NextRoundTeams, combat->NextTurnChangeNotificationTeamIds);
		return 0;
	}

	int TurnManagerCombatProxy::GetAllTeams(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		lua_newtable(L);

		uint32_t i = 1;
		combat->Teams.Iterate([L, &i](uint32_t teamId, esv::TurnManager::CombatTeam * team) {
			push(L, i++);
			TurnManagerTeamProxy::New(L, eoc::CombatTeamId(teamId));
			lua_settable(L, -3);
		});

		return 1;
	}


	char const * const TurnManagerTeamProxy::MetatableName = "esv::TurnManager::Team";

	int TurnManagerTeamProxy::Index(lua_State * L)
	{
		auto team = Get();
		if (team == nullptr) return luaL_error(L, "Team no longer available");

		auto prop = luaL_checkstring(L, 2);
		if (strcmp(prop, "TeamId") == 0) {
			push(L, (uint32_t)team->TeamId);
		} else if (strcmp(prop, "CombatId") == 0) {
			push(L, team->TeamId.CombatId);
		} else if (strcmp(prop, "Initiative") == 0) {
			push(L, (uint32_t)team->Initiative);
		} else if (strcmp(prop, "StillInCombat") == 0) {
			push(L, team->StillInCombat);
			// TODO - fetching CombatGroup?
		} else if (strcmp(prop, "Character") == 0) {
			auto character = team->EntityWrapper.GetCharacter();
			if (character != nullptr) {
				ObjectHandle handle;
				character->GetObjectHandle(&handle);
				HandleProxy<esv::Character>::New(L, handle);
			} else {
				return 0;
			}
		} else if (strcmp(prop, "Item") == 0) {
			auto item = team->EntityWrapper.GetItem();
			if (item != nullptr) {
				ObjectHandle handle;
				item->GetObjectHandle(&handle);
				HandleProxy<esv::Item>::New(L, handle);
			} else {
				return 0;
			}
		} else {
			OsiError("Combat team has no attribute named " << prop);
			return 0;
		}

		return 1;
	}


	void ExtensionLibraryServer::Register(lua_State * L)
	{
		ExtensionLibrary::Register(L);

		ObjectProxy<esv::Status>::RegisterMetatable(L);
		HandleProxy<esv::Character>::RegisterMetatable(L);
		HandleProxy<esv::PlayerCustomData>::RegisterMetatable(L);
		HandleProxy<esv::Item>::RegisterMetatable(L);

		OsiFunctionNameProxy::RegisterMetatable(L);
		StatusHandleProxy::RegisterMetatable(L);
		TurnManagerCombatProxy::RegisterMetatable(L);
		TurnManagerTeamProxy::RegisterMetatable(L);
		RegisterNameResolverMetatable(L);
		CreateNameResolver(L);
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
	int GetStat(lua_State * L);
	int GetCharacter(lua_State * L);
	int GetItem(lua_State * L);
	int GetStatus(lua_State * L);
	int GetCombat(lua_State * L);
	int NewDamageList(lua_State * L);
	int OsirisIsCallable(lua_State * L);
	int IsDeveloperMode(lua_State * L);
	int AddPathOverride(lua_State * L);
	int LuaRandom(lua_State * L);
	int LuaRound(lua_State * L);
	int GenerateIdeHelpers(lua_State * L);
	int AddVoiceMetaData(lua_State * L);


	int BroadcastMessage(lua_State * L)
	{
		auto channel = luaL_checkstring(L, 1);
		auto payload = luaL_checkstring(L, 2);

		esv::Character * excludeCharacter = nullptr;
		if (!lua_isnil(L, 3)) {
			auto excludeCharacterGuid = luaL_checkstring(L, 3);
			excludeCharacter = FindCharacterByNameGuid(excludeCharacterGuid);
			if (excludeCharacter == nullptr) return 0;
		}

		auto & networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeServerMessage();
		if (msg != nullptr) {
			auto postMsg = msg->GetMessage().mutable_post_lua();
			postMsg->set_channel_name(channel);
			postMsg->set_payload(payload);
			if (excludeCharacter != nullptr) {
				networkMgr.ServerBroadcast(msg, excludeCharacter->PeerId);
			} else {
				networkMgr.ServerBroadcast(msg, -1);
			}
		} else {
			OsiErrorS("Could not get free message!");
		}

		return 0;
	}

	int PostMessageToClient(lua_State * L)
	{
		auto characterGuid = luaL_checkstring(L, 1);
		auto channel = luaL_checkstring(L, 2);
		auto payload = luaL_checkstring(L, 3);

		auto character = FindCharacterByNameGuid(characterGuid);
		if (character == nullptr) return 0;

		auto & networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeServerMessage();
		if (msg != nullptr) {
			auto postMsg = msg->GetMessage().mutable_post_lua();
			postMsg->set_channel_name(channel);
			postMsg->set_payload(payload);
			networkMgr.ServerSend(msg, character->PeerId);
		} else {
			OsiErrorS("Could not get free message!");
		}

		return 0;
	}

	void ExtensionLibraryServer::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Version", GetExtensionVersion},
			{"Require", Require},
			{"NewCall", NewCall},
			{"NewQuery", NewQuery},
			{"NewEvent", NewEvent},
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
			{"GetStat", GetStat},

			{"GetCharacter", GetCharacter},
			{"GetItem", GetItem},
			{"GetStatus", GetStatus},
			{"GetCombat", GetCombat},
			{"NewDamageList", NewDamageList},
			{"OsirisIsCallable", OsirisIsCallable},
			{"IsDeveloperMode", IsDeveloperMode},
			{"Random", LuaRandom},
			{"Round", LuaRound},
			{"GenerateIdeHelpers", GenerateIdeHelpers},

			{"AddPathOverride", AddPathOverride},
			{"AddVoiceMetaData", AddVoiceMetaData},

			{"BroadcastMessage", BroadcastMessage},
			{"PostMessageToClient", PostMessageToClient},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}


	ServerState::ServerState()
		: identityAdapters_(gOsirisProxy->GetGlobals())
	{
		identityAdapters_.UpdateAdapters();

		library_.Register(L);

		auto baseLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY);
		LoadScript(baseLib, "BuiltinLibrary.lua");
		auto serverLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY_SERVER);
		LoadScript(serverLib, "BuiltinLibraryServer.lua");
		auto gameMathLib = GetBuiltinLibrary(IDR_LUA_GAME_MATH);
		LoadScript(gameMathLib, "Game.Math.lua");

		lua_getglobal(L, "Ext"); // stack: Ext
		lua_pushstring(L, "ExtraData"); // stack: Ext, "ExtraData"
		StatsExtraDataProxy::New(L); // stack: Ext, "ExtraData", ExtraDataProxy
		lua_settable(L, -3); // stack: Ext
		lua_pop(L, 1); // stack: -

		// Ext is not writeable after loading SandboxStartup!
		auto sandbox = GetBuiltinLibrary(IDR_LUA_SANDBOX_STARTUP);
		LoadScript(sandbox, "SandboxStartup.lua");
	}

	ServerState::~ServerState()
	{
		if (gOsirisProxy) {
			gOsirisProxy->GetCustomFunctionManager().ClearDynamicEntries();
		}
	}


	std::optional<int32_t> ServerState::StatusGetEnterChance(esv::Status * status, bool useCharacterStats)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_StatusGetEnterChance"); // stack: fn
		auto _{ PushArguments(L,
			std::tuple{Push<ObjectProxy<esv::Status>>(status)}) };
		push(L, useCharacterStats);

		auto result = CheckedCall<std::optional<int32_t>>(L, 2, "Ext.StatusGetEnterChance");
		if (result) {
			return std::get<0>(*result);
		} else {
			return {};
		}
	}


	bool ServerState::ComputeCharacterHit(CDivinityStats_Character * target,
		CDivinityStats_Character *attacker, CDivinityStats_Item *weapon, DamagePairList *damageList,
		HitType hitType, bool noHitRoll, bool forceReduceDurability, HitDamageInfo *hit,
		CRPGStats_Object_Property_List *skillProperties, HighGroundBonus highGroundFlag, CriticalRoll criticalRoll)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_ComputeCharacterHit"); // stack: fn

		auto luaTarget = ObjectProxy<CDivinityStats_Character>::New(L, target);
		UnbindablePin _(luaTarget);
		ItemOrCharacterPushPin luaAttacker(L, attacker);

		ObjectProxy<CDivinityStats_Item> * luaWeapon = nullptr;
		if (weapon != nullptr) {
			luaWeapon = ObjectProxy<CDivinityStats_Item>::New(L, weapon);
		} else {
			lua_pushnil(L);
		}
		UnbindablePin _2(luaWeapon);

		auto luaDamageList = DamageList::New(L);
		for (uint32_t i = 0; i < damageList->Size; i++) {
			luaDamageList->Get().SafeAdd((*damageList)[i]);
		}

		auto hitTypeLabel = EnumInfo<HitType>::Find(hitType);
		if (hitTypeLabel) {
			push(L, *hitTypeLabel);
		} else {
			lua_pushnil(L);
		}

		push(L, noHitRoll);
		push(L, forceReduceDurability);

		lua_newtable(L);
		settable(L, "EffectFlags", hit->EffectFlags);
		settable(L, "TotalDamageDone", hit->TotalDamage);
		settable(L, "ArmorAbsorption", hit->ArmorAbsorption);
		settable(L, "LifeSteal", hit->LifeSteal);

		auto damageTypeLabel = EnumInfo<DamageType>::Find(hit->DamageType);
		if (damageTypeLabel) {
			settable(L, "DamageType", *damageTypeLabel);
		}

		auto alwaysBackstab = skillProperties != nullptr
			&& skillProperties->Properties.Find(ToFixedString("AlwaysBackstab")) != nullptr;
		push(L, alwaysBackstab);

		auto highGroundLabel = EnumInfo<HighGroundBonus>::Find(highGroundFlag);
		if (highGroundLabel) {
			push(L, *highGroundLabel);
		} else {
			lua_pushnil(L);
		}

		auto criticalRollLabel = EnumInfo<CriticalRoll>::Find(criticalRoll);
		if (criticalRollLabel) {
			push(L, *criticalRollLabel);
		} else {
			lua_pushnil(L);
		}

		if (CallWithTraceback(L, 11, 1) != 0) { // stack: succeeded
			OsiError("ComputeCharacterHit handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok;
		if (isnil) {
			ok = false;
		} else if (lua_type(L, -1) == LUA_TTABLE) {
			lua_getfield(L, -1, "EffectFlags");
			auto effectFlags = lua_tointeger(L, -1);
			lua_pop(L, 1);
			lua_getfield(L, -1, "TotalDamageDone");
			auto totalDamageDone = lua_tointeger(L, -1);
			lua_pop(L, 1);
			lua_getfield(L, -1, "ArmorAbsorption");
			auto armorAbsorption = lua_tointeger(L, -1);
			lua_pop(L, 1);
			lua_getfield(L, -1, "LifeSteal");
			auto lifeSteal = lua_tointeger(L, -1);
			lua_pop(L, 1);
			lua_getfield(L, -1, "DamageList");
			auto damageList = DamageList::AsUserData(L, -1);
			lua_pop(L, 1);

			if (damageList == nullptr) {
				OsiErrorS("Missing 'DamageList' in table returned from ComputeCharacterHit");
				ok = false;
			} else {
				hit->EffectFlags = (uint32_t)effectFlags;
				hit->TotalDamage = (int32_t)totalDamageDone;
				hit->ArmorAbsorption = (int32_t)armorAbsorption;
				hit->LifeSteal = (int32_t)lifeSteal;
				hit->DamageList.Clear();
				for (uint32_t i = 0; i < damageList->Get().Size; i++) {
					auto const & dmg = damageList->Get()[i];
					hit->DamageList.AddDamage(dmg.DamageType, dmg.Amount);
				}
				ok = true;
			}
		} else {
			OsiErrorS("ComputeCharacterHit must return a table");
			ok = false;
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}


	bool ServerState::OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		auto turnMgr = GetTurnManager();
		if (!turnMgr) {
			OsiErrorS("Couldn't fetch turn manager");
			return false;
		}

		auto combat = turnMgr->Combats.Find(combatId);
		if (combat == nullptr) {
			OsiError("No combat found with ID " << (unsigned)combatId);
			return false;
		}

		PushExtFunction(L, "_CalculateTurnOrder"); // stack: fn

		TurnManagerCombatProxy::New(L, combatId); // stack: fn, combat
		CombatTeamListToLua(L, combat->NextRoundTeams.Set);

		if (CallWithTraceback(L, 2, 1) != 0) { // stack: retval
			OsiError("OnUpdateTurnOrder handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok = false;
		if (!isnil) {
			try {
				UpdateTurnOrder(L, combatId, -1, combat->NextRoundTeams, combat->NextTurnChangeNotificationTeamIds);
				ok = true;
			} catch (Exception &) {
				OsiError("UpdateTurnOrder failed");
			}
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}

	void ServerState::OnGameSessionLoading()
	{
		identityAdapters_.UpdateAdapters();

		State::OnGameSessionLoading();
	}
}

namespace dse
{

	ExtensionStateServer & ExtensionStateServer::Get()
	{
		return gOsirisProxy->GetServerExtensionState();
	}

	lua::State * ExtensionStateServer::GetLua()
	{
		if (Lua) {
			return Lua.get();
		} else {
			return nullptr;
		}
	}

	ModManager * ExtensionStateServer::GetModManager()
	{
		return GetModManagerServer();
	}

	void ExtensionStateServer::Reset()
	{
		ExtensionState::Reset();
		DamageHelpers.Clear();
	}

	void ExtensionStateServer::DoLuaReset()
	{
		Lua.reset();
		Lua = std::make_unique<lua::ServerState>();
		Lua->StoryFunctionMappingsUpdated();
	}

	void ExtensionStateServer::LuaStartup()
	{
		ExtensionState::LuaStartup();

		LuaServerPin lua(*this);
		auto gameState = GetStaticSymbols().GetServerState();
		if (gameState
			&& (*gameState == ServerGameState::LoadLevel
				|| (*gameState == ServerGameState::LoadModule && WasStatLoadTriggered())
				|| *gameState == ServerGameState::LoadGMCampaign
				|| *gameState == ServerGameState::LoadSession
				|| *gameState == ServerGameState::Sync
				|| *gameState == ServerGameState::Paused
				|| *gameState == ServerGameState::Running
				|| *gameState == ServerGameState::GameMasterPause)) {
			lua->OnModuleResume();
			OsiWarn("Server resume -- state " << (unsigned)*gameState);
		} else {
			OsiWarn("Server NO resume -- state " << (unsigned)*gameState);
		}
	}

	void ExtensionStateServer::StoryLoaded()
	{
		DEBUG("ExtensionStateServer::StoryLoaded()");
		if (Lua) {
			Lua->StoryLoaded();
		}
	}

	void ExtensionStateServer::StoryFunctionMappingsUpdated()
	{
		if (Lua) {
			Lua->StoryFunctionMappingsUpdated();
		}
	}
}
