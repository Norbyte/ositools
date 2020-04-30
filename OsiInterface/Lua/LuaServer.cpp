#include <stdafx.h>
#include <GameDefinitions/Projectile.h>
#include <Lua/LuaBindingServer.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include "resource.h"

namespace dse::lua
{
	char const* const ObjectProxy<esv::Status>::MetatableName = "esv::Status";

	int ObjectProxy<esv::Status>::Index(lua_State* L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Status object no longer available");

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "StatusType") == 0) {
			auto statusType = obj_->GetStatusId();
			auto typeName = EnumInfo<StatusType>::Find(statusType);
			push(L, typeName);
			return 1;
		}

		auto& propertyMap = StatusToPropertyMap(obj_);
		auto fetched = LuaPropertyMapGet(L, propertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<esv::Status>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const* const ObjectProxy<esv::PlayerCustomData>::MetatableName = "esv::PlayerCustomData";

	esv::PlayerCustomData* ObjectProxy<esv::PlayerCustomData>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto character = esv::GetEntityWorld()->GetCharacter(handle_);
		if (character == nullptr) luaL_error(L, "Character handle invalid");

		if (character->PlayerData == nullptr
			|| !character->PlayerData->CustomData.Initialized) {
			OsiError("Character has no player data, or custom data was not initialized.");
			return nullptr;
		}

		return &character->PlayerData->CustomData;
	}

	int ObjectProxy<esv::PlayerCustomData>::Index(lua_State* L)
	{
		auto customData = Get(L);
		if (!customData) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gPlayerCustomDataPropertyMap, customData, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<esv::PlayerCustomData>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const* const ObjectProxy<esv::Character>::MetatableName = "esv::Character";

	int ServerCharacterFetchProperty(lua_State* L, esv::Character* character, char const* prop)
	{
		if (strcmp(prop, "PlayerCustomData") == 0) {
			if (character->PlayerData != nullptr
				&& character->PlayerData->CustomData.Initialized) {
				ObjectHandle handle;
				character->GetObjectHandle(handle);
				ObjectProxy<esv::PlayerCustomData>::New(L, handle);
				return 1;
			}
			else {
				OsiError("Character has no player data, or custom data was not initialized.");
				return 0;
			}
		}

		if (strcmp(prop, "Stats") == 0) {
			if (character->Stats != nullptr) {
				ObjectHandle handle;
				character->GetObjectHandle(handle);
				ObjectProxy<CDivinityStats_Character>::New(L, handle);
				return 1;
			}
			else {
				OsiError("Character has no stats.");
				return 0;
			}
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterPropertyMap, character, prop, true);
		return fetched ? 1 : 0;
	}

	esv::Character* ObjectProxy<esv::Character>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto character = esv::GetEntityWorld()->GetCharacter(handle_);
		if (character == nullptr) luaL_error(L, "Character handle invalid");
		return character;
	}

	int CharacterGetInventoryItems(lua_State* L)
	{
		auto self = checked_get<ObjectProxy<esv::Character>*>(L, 1);

		lua_newtable(L);

		auto inventory = esv::FindInventoryByHandle(self->Get(L)->InventoryHandle);
		if (inventory != nullptr) {
			int32_t index = 1;
			for (auto itemHandle : inventory->ItemsBySlot) {
				if (itemHandle) {
					auto item = esv::GetEntityWorld()->GetItem(itemHandle);
					if (item != nullptr) {
						settable(L, index++, item->MyGuid);
					}
				}
			}
		}

		return 1;
	}

#include <Lua/LuaShared.inl>

	int ObjectProxy<esv::Character>::Index(lua_State* L)
	{
		auto character = Get(L);
		if (!character) return 0;

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "GetInventoryItems") == 0) {
			lua_pushcfunction(L, &CharacterGetInventoryItems);
			return 1;
		}

		if (strcmp(prop, "HasTag") == 0) {
			lua_pushcfunction(L, &GameObjectHasTag<esv::Character>);
			return 1;
		}

		if (strcmp(prop, "GetTags") == 0) {
			lua_pushcfunction(L, &GameObjectGetTags<esv::Character>);
			return 1;
		}

		if (strcmp(prop, "GetStatus") == 0) {
			lua_pushcfunction(L, (&GameObjectGetStatus<esv::Character, esv::Status>));
			return 1;
		}

		if (strcmp(prop, "GetStatusByType") == 0) {
			lua_pushcfunction(L, (&GameObjectGetStatusByType<esv::Character, esv::Status>));
			return 1;
		}

		if (strcmp(prop, "GetStatuses") == 0) {
			lua_pushcfunction(L, (&GameObjectGetStatuses<esv::Character>));
			return 1;
		}

		return ServerCharacterFetchProperty(L, character, prop);
	}

	int ObjectProxy<esv::Character>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const* const ObjectProxy<esv::Item>::MetatableName = "esv::Item";

	esv::Item* ObjectProxy<esv::Item>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto item = esv::GetEntityWorld()->GetItem(handle_);
		if (item == nullptr) luaL_error(L, "Item handle invalid");
		return item;
	}

	int ObjectProxy<esv::Item>::Index(lua_State* L)
	{
		auto item = Get(L);
		if (!item) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto propFS = ToFixedString(prop);

		if (propFS == GFS.strHasTag) {
			lua_pushcfunction(L, &GameObjectHasTag<esv::Item>);
			return 1;
		}

		if (propFS == GFS.strGetTags) {
			lua_pushcfunction(L, &GameObjectGetTags<esv::Item>);
			return 1;
		}

		if (propFS == GFS.strGetStatus) {
			lua_pushcfunction(L, (&GameObjectGetStatus<esv::Item, esv::Status>));
			return 1;
		}

		if (propFS == GFS.strGetStatusByType) {
			lua_pushcfunction(L, (&GameObjectGetStatusByType<esv::Item, esv::Status>));
			return 1;
		}

		if (propFS == GFS.strGetStatuses) {
			lua_pushcfunction(L, (&GameObjectGetStatuses<esv::Item>));
			return 1;
		}

		if (propFS == GFS.strStats) {
			if (item->StatsDynamic != nullptr) {
				ObjectProxy<CDivinityStats_Item>::New(L, handle_);
				return 1;
			}
			else {
				OsiError("Item has no stats.");
				return 0;
			}
		}

		bool fetched = false;
		if (item->StatsDynamic != nullptr) {
			fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item->StatsDynamic, propFS, false);
		}

		if (!fetched) {
			fetched = LuaPropertyMapGet(L, gItemPropertyMap, item, propFS, true);
		}

		return fetched ? 1 : 0;
	}

	int ObjectProxy<esv::Item>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const* const ObjectProxy<esv::Projectile>::MetatableName = "esv::Projectile";

	esv::Projectile* ObjectProxy<esv::Projectile>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto projectile = esv::GetEntityWorld()->GetProjectile(handle_);
		if (projectile == nullptr) luaL_error(L, "Projectile handle invalid");
		return projectile;
	}

	int ObjectProxy<esv::Projectile>::Index(lua_State* L)
	{
		auto projectile = Get(L);
		if (!projectile) return 0;

		auto prop = luaL_checkstring(L, 2);
		bool fetched = LuaPropertyMapGet(L, gProjectilePropertyMap, projectile, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<esv::Projectile>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}
}

namespace dse::esv::lua
{
	using namespace dse::lua;


	char const* const StatusHandleProxy::MetatableName = "esv::HStatus";

	int StatusHandleProxy::Index(lua_State* L)
	{
		auto character = GetEntityWorld()->GetCharacter(character_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");

		esv::Status* status;
		if (statusHandle_) {
			status = character->GetStatus(statusHandle_, true);
		} else {
			status = character->GetStatus(statusNetId_);
		}

		if (status == nullptr) return luaL_error(L, "Status handle invalid");

		auto prop = luaL_checkstring(L, 2);
		auto& propertyMap = StatusToPropertyMap(status);
		auto fetched = LuaPropertyMapGet(L, propertyMap, status, prop, true);
		return fetched ? 1 : 0;
	}

	int StatusHandleProxy::NewIndex(lua_State* L)
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

		for (auto team : combatTeams) {
			notifies.insert(team->TeamId);
		}

		for (auto notify : combatNotifies) {
			notifies.insert(notify);
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
				character->GetObjectHandle(handle);
				ObjectProxy<esv::Character>::New(L, handle);
			} else {
				return 0;
			}
		} else if (strcmp(prop, "Item") == 0) {
			auto item = team->EntityWrapper.GetItem();
			if (item != nullptr) {
				ObjectHandle handle;
				item->GetObjectHandle(handle);
				ObjectProxy<esv::Item>::New(L, handle);
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
		ObjectProxy<esv::Character>::RegisterMetatable(L);
		ObjectProxy<esv::PlayerCustomData>::RegisterMetatable(L);
		ObjectProxy<esv::Item>::RegisterMetatable(L);
		ObjectProxy<esv::Projectile>::RegisterMetatable(L);

		OsiFunctionNameProxy::RegisterMetatable(L);
		StatusHandleProxy::RegisterMetatable(L);
		TurnManagerCombatProxy::RegisterMetatable(L);
		TurnManagerTeamProxy::RegisterMetatable(L);
		RegisterNameResolverMetatable(L);
		CreateNameResolver(L);
	}


	esv::Character* GetCharacter(lua_State* L, int index)
	{
		esv::Character* character = nullptr;
		switch (lua_type(L, index)) {
		case LUA_TNUMBER:
		{
			auto value = lua_tointeger(L, index);
			if (value > 0xffffffff) {
				ObjectHandle handle{ value };
				if (handle.GetType() == (uint32_t)ObjectType::ClientCharacter) {
					OsiError("Attempted to resolve client ObjectHandle on the server");
				} else {
					character = GetEntityWorld()->GetCharacter(handle);
				}
			} else {
				NetId netId{ (uint32_t)value };
				character = GetEntityWorld()->GetCharacter(netId);
			}
			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, index);
			character = GetEntityWorld()->GetCharacter(guid);
			break;
		}

		default:
			OsiError("Expected character UUID, Handle or NetId; got " << lua_typename(L, lua_type(L, 1)));
			break;
		}

		return character;
	}

	int GetCharacter(lua_State* L)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve character handle in restricted context");
		}

		esv::Character* character = GetCharacter(L, 1);

		if (character != nullptr) {
			ObjectHandle handle;
			character->GetObjectHandle(handle);
			ObjectProxy<esv::Character>::New(L, handle);
			return 1;
		}
		else {
			return 0;
		}
	}

	int GetItem(lua_State* L)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve item handle in restricted context");
		}

		esv::Item* item = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TNUMBER:
		{
			auto handle = ObjectHandle(lua_tointeger(L, 1));
			item = GetEntityWorld()->GetItem(handle);
			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, 1);
			item = GetEntityWorld()->GetItem(guid);
			break;
		}

		default:
			OsiError("Expected item GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
			return 0;
		}

		if (item != nullptr) {
			ObjectHandle handle;
			item->GetObjectHandle(handle);
			ObjectProxy<esv::Item>::New(L, handle);
			return 1;
		}
		else {
			return 0;
		}
	}

	int GetGameObject(lua_State* L)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve game object handle in restricted context");
		}

		esv::Item* item = nullptr;
		esv::Character* character = nullptr;
		esv::Projectile* projectile = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TNUMBER:
		{
			auto handle = ObjectHandle(lua_tointeger(L, 1));
			if (handle) {
				switch ((ObjectType)handle.GetType()) {
				case ObjectType::ServerCharacter:
					character = GetEntityWorld()->GetCharacter(handle);
					break;

				case ObjectType::ServerItem:
					item = GetEntityWorld()->GetItem(handle);
					break;

				case ObjectType::ServerProjectile:
					projectile = GetEntityWorld()->GetProjectile(handle);
					break;

				default:
					OsiError("Cannot resolve unsupported server handle type: " << handle.GetType());
					break;
				}
			}

			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, 1);
			character = GetEntityWorld()->GetCharacter(guid, false);
			item = GetEntityWorld()->GetItem(guid, false);
			break;
		}

		default:
			OsiError("Expected object GUID or handle, got ");
			return 0;
		}

		if (item != nullptr) {
			ObjectHandle handle;
			item->GetObjectHandle(handle);
			ObjectProxy<esv::Item>::New(L, handle);
			return 1;
		} else if (character != nullptr) {
			ObjectHandle handle;
			character->GetObjectHandle(handle);
			ObjectProxy<esv::Character>::New(L, handle);
			return 1;
		} else if (projectile != nullptr) {
			ObjectHandle handle;
			projectile->GetObjectHandle(handle);
			ObjectProxy<esv::Projectile>::New(L, handle);
			return 1;
		} else {
			return 0;
		}
	}

	int GetStatus(lua_State* L)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve status handle in restricted context");
		}

		esv::Character* character = GetCharacter(L, 1);
		if (character == nullptr) return 0;

		auto index = lua_tointeger(L, 2);
		esv::Status* status;
		if (index > 0xffffffff) {
			ObjectHandle statusHandle{ index };
			status = character->GetStatus(statusHandle, true);
			if (status != nullptr) {
				ObjectHandle characterHandle;
				character->GetObjectHandle(characterHandle);
				StatusHandleProxy::New(L, characterHandle, statusHandle);
				return 1;
			}
		} else {
			NetId statusNetId{ (uint32_t)index };
			status = character->GetStatus(statusNetId);
			if (status != nullptr) {
				ObjectHandle characterHandle;
				character->GetObjectHandle(characterHandle);
				StatusHandleProxy::New(L, characterHandle, statusNetId);
				return 1;
			}
		}

		OsiError("Character has no status with ObjectHandle/NetId 0x" << std::hex << index);
		return 0;
	}

	int GetCombat(lua_State* L)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve combat ID in restricted context");
		}

		auto turnMgr = GetEntityWorld()->GetTurnManager();
		if (turnMgr == nullptr) {
			OsiErrorS("esv::TurnManager not available");
			return 0;
		}

		auto combatId = (uint8_t)luaL_checkinteger(L, 1);
		auto combat = turnMgr->Combats.Find(combatId);
		if (combat == nullptr) {
			OsiError("No combat found with ID " << (unsigned)combatId);
			return 0;
		}

		TurnManagerCombatProxy::New(L, combatId);
		return 1;
	}

	int OsirisIsCallable(lua_State* L)
	{
		LuaServerPin lua(ExtensionState::Get());
		bool allowed = (lua->RestrictionFlags & State::RestrictOsiris) != 0;
		push(L, allowed);
		return 1;
	}

	int BroadcastMessage(lua_State * L)
	{
		auto channel = luaL_checkstring(L, 1);
		auto payload = luaL_checkstring(L, 2);

		esv::Character * excludeCharacter = nullptr;
		if (lua_gettop(L) > 2 && !lua_isnil(L, 3)) {
			auto excludeCharacterGuid = luaL_checkstring(L, 3);
			excludeCharacter = GetEntityWorld()->GetCharacter(excludeCharacterGuid);
			if (excludeCharacter == nullptr) return 0;
		}

		auto & networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeServerMessage(-1);
		if (msg != nullptr) {
			auto postMsg = msg->GetMessage().mutable_post_lua();
			postMsg->set_channel_name(channel);
			postMsg->set_payload(payload);
			if (excludeCharacter != nullptr) {
				networkMgr.ServerBroadcast(msg, excludeCharacter->UserID);
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

		auto character = GetEntityWorld()->GetCharacter(characterGuid);
		if (character == nullptr) return 0;

		auto & networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeServerMessage(character->UserID);
		if (msg != nullptr) {
			auto postMsg = msg->GetMessage().mutable_post_lua();
			postMsg->set_channel_name(channel);
			postMsg->set_payload(payload);
			networkMgr.ServerSend(msg, character->UserID);
		} else {
			OsiErrorS("Could not get free message!");
		}

		return 0;
	}

	int PlayerHasExtender(lua_State* L)
	{
		auto characterGuid = luaL_checkstring(L, 1);

		auto character = GetEntityWorld()->GetCharacter(characterGuid);
		if (character == nullptr) return 0;

		auto& networkMgr = gOsirisProxy->GetNetworkManager();
		bool hasExtender = networkMgr.ServerCanSendExtenderMessages(character->UserID.GetPeerId());
		push(L, hasExtender);
		return 1;
	}

	void ExtensionLibraryServer::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Version", GetExtensionVersion},
			{"MonotonicTime", MonotonicTime},
			{"Include", Include},
			{"NewCall", NewCall},
			{"NewQuery", NewQuery},
			{"NewEvent", NewEvent},
			{"Print", OsiPrint},
			{"PrintError", OsiPrintError},

			{"SaveFile", SaveFile},
			{"LoadFile", LoadFile},

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
			{"CreateStat", CreateStat},
			{"SyncStat", SyncStat},
			{"GetDeltaMod", GetDeltaMod},
			{"UpdateDeltaMod", UpdateDeltaMod},

			{"GetCharacter", GetCharacter},
			{"GetItem", GetItem},
			{"GetGameObject", GetGameObject},
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
			{"GetTranslatedString", GetTranslatedString},
			{"GetTranslatedStringFromKey", GetTranslatedStringFromKey},

			{"BroadcastMessage", BroadcastMessage},
			{"PostMessageToClient", PostMessageToClient},
			{"PlayerHasExtender", PlayerHasExtender},
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
		push(L, "ExtraData"); // stack: Ext, "ExtraData"
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


	std::optional<int32_t> ServerState::StatusGetEnterChance(esv::Status * status, bool isEnterCheck)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictOsiris);

		PushExtFunction(L, "_StatusGetEnterChance"); // stack: fn
		auto _{ PushArguments(L,
			std::tuple{Push<ObjectProxy<esv::Status>>(status)}) };
		push(L, isEnterCheck);

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
		Restriction restriction(*this, RestrictOsiris);

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
		for (auto const& dmg : *damageList) {
			luaDamageList->Get().SafeAdd(dmg);
		}

		push(L, EnumInfo<HitType>::Find(hitType));
		push(L, noHitRoll);
		push(L, forceReduceDurability);

		lua_newtable(L);
		settable(L, "EffectFlags", (int64_t)hit->EffectFlags);
		settable(L, "TotalDamageDone", hit->TotalDamage);
		settable(L, "ArmorAbsorption", hit->ArmorAbsorption);
		settable(L, "LifeSteal", hit->LifeSteal);
		settable(L, "DamageType", EnumInfo<DamageType>::Find(hit->DamageType));

		auto alwaysBackstab = skillProperties != nullptr
			&& skillProperties->Properties.Find(ToFixedString("AlwaysBackstab")) != nullptr;
		push(L, alwaysBackstab);

		push(L, EnumInfo<HighGroundBonus>::Find(highGroundFlag));
		push(L, EnumInfo<CriticalRoll>::Find(criticalRoll));

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
				hit->EffectFlags = (HitFlag)effectFlags;
				hit->TotalDamage = (int32_t)totalDamageDone;
				hit->ArmorAbsorption = (int32_t)armorAbsorption;
				hit->LifeSteal = (int32_t)lifeSteal;
				hit->DamageList.Clear();
				for (auto const& dmg : damageList->Get()) {
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
		Restriction restriction(*this, RestrictOsiris);

		auto turnMgr = GetEntityWorld()->GetTurnManager();
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


	std::optional<STDString> ServerState::GetModPersistentVars(STDString const& modTable)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_GetModPersistentVars");
		push(L, modTable);

		auto ret = CheckedCall<std::optional<char const*>>(L, 1, "Ext.GetModPersistentVars");
		if (ret) {
			return std::get<0>(*ret);
		} else {
			return {};
		}
	}


	void ServerState::RestoreModPersistentVars(STDString const& modTable, STDString const& vars)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_RestoreModPersistentVars");
		push(L, modTable);
		push(L, vars);

		CheckedCall<>(L, 2, "Ext.RestoreModPersistentVars");
	}


	void ServerState::OnGameSessionLoading()
	{
		identityAdapters_.UpdateAdapters();

		State::OnGameSessionLoading();
	}
}

namespace dse::esv
{

	ExtensionState & ExtensionState::Get()
	{
		return gOsirisProxy->GetServerExtensionState();
	}

	lua::State * ExtensionState::GetLua()
	{
		if (Lua) {
			return Lua.get();
		} else {
			return nullptr;
		}
	}

	ModManager * ExtensionState::GetModManager()
	{
		return GetModManagerServer();
	}

	void ExtensionState::OnGameSessionLoading()
	{
		runtimeModifiedStats_.clear();
		dse::ExtensionStateBase::OnGameSessionLoading();
	}

	void ExtensionState::Reset()
	{
		ExtensionStateBase::Reset();
		DamageHelpers.Clear();
	}

	void ExtensionState::DoLuaReset()
	{
		Lua.reset();
		Lua = std::make_unique<lua::ServerState>();
		Lua->StoryFunctionMappingsUpdated();
	}

	void ExtensionState::LuaStartup()
	{
		ExtensionStateBase::LuaStartup();

		LuaServerPin lua(*this);
		auto gameState = GetStaticSymbols().GetServerState();
		if (gameState
			&& (*gameState == esv::GameState::LoadLevel
				|| (*gameState == esv::GameState::LoadModule && WasStatLoadTriggered())
				|| *gameState == esv::GameState::LoadGMCampaign
				|| *gameState == esv::GameState::LoadSession
				|| *gameState == esv::GameState::Sync
				|| *gameState == esv::GameState::Paused
				|| *gameState == esv::GameState::Running
				|| *gameState == esv::GameState::GameMasterPause)) {
			lua->OnModuleResume();
			OsiWarn("Server resume -- state " << (unsigned)*gameState);
		} else {
			OsiWarn("Server NO resume -- state " << (unsigned)*gameState);
		}
	}

	void ExtensionState::MarkRuntimeModifiedStat(FixedString const& statId)
	{
		runtimeModifiedStats_.insert(statId);
	}

	void ExtensionState::StoryLoaded()
	{
		DEBUG("ExtensionStateServer::StoryLoaded()");
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
}
