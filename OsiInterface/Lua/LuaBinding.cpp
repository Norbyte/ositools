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



	char const * const LuaObjectProxy<esv::Status>::MetatableName = "esv::Status";

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


	int LuaStatGetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, std::optional<int> level);
	int LuaStatSetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, int valueIdx);

	char const * const LuaStatsProxy::MetatableName = "CRPGStats_Object";

	int LuaStatsProxy::LuaIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Level") == 0) {
			lua_push(L, obj_->Level);
			return 1;
		} else if (strcmp(attributeName, "Name") == 0) {
			lua_pushstring(L, obj_->Name);
			return 1;
		}

		return LuaStatGetAttribute(L, obj_, attributeName, level_);
	}

	int LuaStatsProxy::LuaNewIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);
		return LuaStatSetAttribute(L, obj_, attributeName, 3);
	}


	char const * const LuaSkillPrototypeProxy::MetatableName = "eoc::SkillPrototype";

	LuaSkillPrototypeProxy::LuaSkillPrototypeProxy(SkillPrototype * obj, std::optional<int> level)
		: obj_(obj), level_(level)
	{
		stats_ = StatFindObject(obj->RPGStatsObjectIndex);
	}

	int LuaSkillPrototypeProxy::LuaIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Level") == 0) {
			lua_push(L, obj_->Level);
			return 1;
		}

		return LuaStatGetAttribute(L, stats_, attributeName, level_);
	}

	std::optional<int32_t> GetCharacterDynamicStat(CDivinityStats_Character * stats, char const * statName)
	{
		bool isBaseStat = strncmp(statName, "Base", 4) == 0;
		return stats->GetStat(isBaseStat ? (statName + 4) : statName, isBaseStat);
	}

	char const * const LuaObjectProxy<CDivinityStats_Character>::MetatableName = "CDivinityStats_Character";

	int CharacterFetchStat(lua_State * L, CDivinityStats_Character * stats, char const * prop)
	{
		auto dynamicStat = GetCharacterDynamicStat(stats, prop);
		if (dynamicStat) {
			lua_pushinteger(L, *dynamicStat);
			return 1;
		}

		if (strcmp(prop, "DynamicStats") == 0) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto statPtr = stats->DynamicStats; statPtr != stats->DynamicStatsEnd; statPtr++) {
				lua_push(L, statIdx++);
				LuaObjectProxy<CharacterDynamicStat>::New(L, *statPtr);
				lua_settable(L, -3);
			}

			return 1;
		}

		if (strcmp(prop, "DamageBoost") == 0) {
			lua_pushinteger(L, stats->GetDamageBoost());
			return 1;
		}

		if (strcmp(prop, "MainWeapon") == 0) {
			auto weapon = stats->GetMainWeapon();
			if (weapon != nullptr) {
				LuaObjectProxy<CDivinityStats_Item>::New(L, weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (strcmp(prop, "OffHandWeapon") == 0) {
			auto weapon = stats->GetOffHandWeapon();
			if (weapon != nullptr) {
				LuaObjectProxy<CDivinityStats_Item>::New(L, weapon);
				return 1;
			} else {
				return 0;
			}
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

	int ItemFetchStat(lua_State * L, CDivinityStats_Item * item, char const * prop)
	{
		if (strcmp(prop, "DynamicStats") == 0) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto statPtr = item->DynamicAttributes_Start; statPtr != item->DynamicAttributes_End; statPtr++) {
				lua_push(L, statIdx++);
				LuaObjectProxy<CDivinityStats_Equipment_Attributes>::New(L, *statPtr);
				lua_settable(L, -3);
			}

			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item, prop, false);
		if (fetched) {
			return 1;
		}

		return LuaStatGetAttribute(L, item, prop, {});
	}

	int CharacterFetchProperty(lua_State * L, esv::Character * character, char const * prop)
	{
		if (strcmp(prop, "PlayerCustomData") == 0) {
			if (character->PlayerData != nullptr
				&& character->PlayerData->CustomData.Initialized) {
				ObjectHandle handle;
				character->GetObjectHandle(&handle);
				LuaHandleProxy<esv::PlayerCustomData>::New(L, handle);
				return 1;
			} else {
				OsiError("Character has no player data, or custom data was not initialized.");
				return 0;
			}
		}

		if (strcmp(prop, "Stats") == 0) {
			if (character->Stats != nullptr) {
				ObjectHandle handle;
				character->GetObjectHandle(&handle);
				LuaHandleProxy<CDivinityStats_Character>::New(L, handle);
				return 1;
			} else {
				OsiError("Character has no stats.");
				return 0;
			}
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterPropertyMap, character, prop, true);
		return fetched ? 1 : 0;
	}


	char const * const LuaObjectProxy<CDivinityStats_Item>::MetatableName = "CDivinityStats_Item";

	int LuaObjectProxy<CDivinityStats_Item>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Item stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, obj_, prop);
	}

	int LuaObjectProxy<CDivinityStats_Item>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaObjectProxy<CDivinityStats_Equipment_Attributes>::MetatableName = "CDivinityStats_Equipment_Attributes";

	int LuaObjectProxy<CDivinityStats_Equipment_Attributes>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Equipment stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto & propMap = obj_->GetPropertyMap();
		auto fetched = LuaPropertyMapGet(L, propMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaObjectProxy<CDivinityStats_Equipment_Attributes>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported!");
	}


	char const * const LuaObjectProxy<CharacterDynamicStat>::MetatableName = "CharacterDynamicStat";

	int LuaObjectProxy<CharacterDynamicStat>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Character stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gCharacterDynamicStatPropertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaObjectProxy<CharacterDynamicStat>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported!");
	}


	char const * const LuaStatusHandleProxy::MetatableName = "esv::HStatus";

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


	char const * const LuaHandleProxy<CDivinityStats_Character>::MetatableName = "HCDivinityStats_Character";

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


	char const * const LuaHandleProxy<CDivinityStats_Item>::MetatableName = "HCDivinityStats_Item";

	int LuaHandleProxy<CDivinityStats_Item>::LuaIndex(lua_State * L)
	{
		auto item = FindItemByHandle(handle_);
		if (item == nullptr) return luaL_error(L, "Item handle invalid");
		if (item->StatsDynamic == nullptr) return luaL_error(L, "Item has no stats!");

		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, item->StatsDynamic, prop);
	}

	int LuaHandleProxy<CDivinityStats_Item>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}



	char const * const LuaHandleProxy<esv::PlayerCustomData>::MetatableName = "esv::HPlayerCustomData";

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


	char const * const LuaHandleProxy<esv::Character>::MetatableName = "esv::HCharacter";

	int LuaHandleProxy<esv::Character>::LuaIndex(lua_State * L)
	{
		auto character = FindCharacterByHandle(handle_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchProperty(L, character, prop);
	}

	int LuaHandleProxy<esv::Character>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaHandleProxy<esv::Item>::MetatableName = "esv::HItem";

	int LuaHandleProxy<esv::Item>::LuaIndex(lua_State * L)
	{
		auto item = FindItemByHandle(handle_);
		if (item == nullptr) return luaL_error(L, "Item handle invalid");

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "Stats") == 0) {
			if (item->StatsDynamic != nullptr) {
				LuaHandleProxy<CDivinityStats_Item>::New(L, handle_);
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

	int LuaHandleProxy<esv::Item>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}



	char const * const LuaStatsExtraDataProxy::MetatableName = "CRPGStats_ExtraData";

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



	char const * const LuaDamageList::MetatableName = "CDamageList";

	void LuaDamageList::PopulateMetatable(lua_State * L)
	{
		lua_newtable(L);

		lua_pushcfunction(L, &Add);
		lua_setfield(L, -2, "Add");

		lua_pushcfunction(L, &Clear);
		lua_setfield(L, -2, "Clear");

		lua_pushcfunction(L, &Multiply);
		lua_setfield(L, -2, "Multiply");

		lua_pushcfunction(L, &Merge);
		lua_setfield(L, -2, "Merge");

		lua_pushcfunction(L, &ConvertDamageType);
		lua_setfield(L, -2, "ConvertDamageType");

		lua_pushcfunction(L, &AggregateSameTypeDamages);
		lua_setfield(L, -2, "AggregateSameTypeDamages");

		lua_pushcfunction(L, &ToTable);
		lua_setfield(L, -2, "ToTable");

		lua_setfield(L, -2, "__index");
	}

	int LuaDamageList::Add(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto damageType = lua_checkenum<DamageType>(L, 2);
		auto amount = (int32_t)luaL_checkinteger(L, 3);

		self->damages_.AddDamage(damageType, amount);

		return 0;
	}

	int LuaDamageList::Clear(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		if (lua_gettop(L) >= 2) {
			auto damageType = lua_checkenum<DamageType>(L, 2);
			self->damages_.ClearDamage(damageType);
		} else {
			self->damages_.Clear();
		}

		return 0;
	}

	int LuaDamageList::Multiply(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto multiplier = luaL_checknumber(L, 2);

		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			auto & item = self->damages_[i];
			item.Amount = (int32_t)round(item.Amount * multiplier);
		}

		return 0;
	}

	int LuaDamageList::Merge(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto other = LuaDamageList::CheckUserData(L, 2);

		for (uint32_t i = 0; i < other->damages_.Size; i++) {
			auto & item = other->damages_[i];
			self->damages_.AddDamage(item.DamageType, item.Amount);
		}

		return 0;
	}

	int LuaDamageList::ConvertDamageType(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto damageType = lua_checkenum<DamageType>(L, 2);

		int32_t totalDamage = 0;
		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			totalDamage += self->damages_[i].Amount;
		}

		self->damages_.Clear();
		self->damages_.AddDamage(damageType, totalDamage);

		return 0;
	}

	int LuaDamageList::AggregateSameTypeDamages(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);

		for (uint32_t i = self->damages_.Size; i > 0; i--) {
			auto & src = self->damages_[i - 1];
			for (uint32_t j = i - 1; j > 0; j--) {
				auto & dest = self->damages_[j - 1];
				if (src.DamageType == dest.DamageType) {
					dest.Amount += src.Amount;
					self->damages_.Remove(i - 1);
					break;
				}
			}
		}

		return 0;
	}

	int LuaDamageList::ToTable(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);

		lua_newtable(L); // Stack: tab

		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			auto const & item = self->damages_[i];

			lua_push(L, i + 1); // Stack: tab, index
			lua_newtable(L); // Stack: tab, index, dmgTab
			auto dmgTypeName = EnumInfo<DamageType>::Find(item.DamageType);
			luaL_settable(L, "DamageType", *dmgTypeName);
			luaL_settable(L, "Amount", item.Amount);

			lua_settable(L, -3); // Stack: tab
		}

		return 1;
	}



	char const * const LuaTurnManagerCombatProxy::MetatableName = "esv::TurnManager::Combat";

	void LuaTurnManagerCombatProxy::PopulateMetatable(lua_State * L)
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
			lua_push(L, i + 1);
			LuaTurnManagerTeamProxy::New(L, teams[i]->TeamId);
			lua_settable(L, -3);
		}
	}

	int LuaTurnManagerCombatProxy::LuaIndex(lua_State * L)
	{
		auto combat = Get();
		if (combat == nullptr) return luaL_error(L, "Combat no longer available");

		auto prop = luaL_checkstring(L, 2);
		if (strcmp(prop, "CombatId") == 0) {
			lua_push(L, combatId_);
		} else if (strcmp(prop, "LevelName") == 0) {
			lua_push(L, combat->LevelName.Str);
		} else if (strcmp(prop, "IsActive") == 0) {
			lua_push(L, combat->IsActive);
		} else {
			OsiError("Combat has no attribute named " << prop);
			return 0;
		}

		return 1;
	}

	int LuaTurnManagerCombatProxy::GetCurrentTurnOrder(lua_State * L)
	{
		auto self = LuaTurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		CombatTeamListToLua(L, combat->CurrentRoundTeams.Set);
		return 1;
	}

	int LuaTurnManagerCombatProxy::GetNextTurnOrder(lua_State * L)
	{
		auto self = LuaTurnManagerCombatProxy::CheckUserData(L, 1);
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
			auto luaTeam = LuaTurnManagerTeamProxy::CheckUserData(L, -1);
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

	int LuaTurnManagerCombatProxy::UpdateCurrentTurnOrder(lua_State * L)
	{
		auto self = LuaTurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		UpdateTurnOrder(L, self->combatId_, 2, combat->CurrentRoundTeams, combat->CurrentTurnChangeNotificationTeamIds);
		return 0;
	}

	int LuaTurnManagerCombatProxy::UpdateNextTurnOrder(lua_State * L)
	{
		auto self = LuaTurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		UpdateTurnOrder(L, self->combatId_, 2, combat->NextRoundTeams, combat->NextTurnChangeNotificationTeamIds);
		return 0;
	}

	int LuaTurnManagerCombatProxy::GetAllTeams(lua_State * L)
	{
		auto self = LuaTurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		lua_newtable(L);

		uint32_t i = 1;
		combat->Teams.Iterate([L, &i](uint32_t teamId, esv::TurnManager::CombatTeam * team) {
			lua_push(L, i++);
			LuaTurnManagerTeamProxy::New(L, eoc::CombatTeamId(teamId));
			lua_settable(L, -3);
		});

		return 1;
	}


	char const * const LuaTurnManagerTeamProxy::MetatableName = "esv::TurnManager::Team";

	/*void LuaTurnManagerTeamProxy::PopulateMetatable(lua_State * L)
	{
		lua_newtable(L);

		lua_pushcfunction(L, &GetCurrentTurnOrder);
		lua_setfield(L, -2, "GetCurrentTurnOrder");

		lua_setfield(L, -2, "__index");
	}*/

	int LuaTurnManagerTeamProxy::LuaIndex(lua_State * L)
	{
		auto team = Get();
		if (team == nullptr) return luaL_error(L, "Team no longer available");

		auto prop = luaL_checkstring(L, 2);
		if (strcmp(prop, "TeamId") == 0) {
			lua_push(L, (uint32_t)team->TeamId);
		} else if (strcmp(prop, "CombatId") == 0) {
			lua_push(L, team->TeamId.CombatId);
		} else if (strcmp(prop, "Initiative") == 0) {
			lua_push(L, (uint32_t)team->Initiative);
		} else if (strcmp(prop, "StillInCombat") == 0) {
			lua_push(L, team->StillInCombat);
			// TODO - fetching CombatGroup?
		} else if (strcmp(prop, "Character") == 0) {
			auto character = team->EntityWrapper.GetCharacter();
			if (character != nullptr) {
				ObjectHandle handle;
				character->GetObjectHandle(&handle);
				LuaHandleProxy<esv::Character>::New(L, handle);
			} else {
				return 0;
			}
		} else if (strcmp(prop, "Item") == 0) {
			auto item = team->EntityWrapper.GetItem();
			if (item != nullptr) {
				ObjectHandle handle;
				item->GetObjectHandle(&handle);
				LuaHandleProxy<esv::Item>::New(L, handle);
			} else {
				return 0;
			}
		} else {
			OsiError("Combat team has no attribute named " << prop);
			return 0;
		}

		return 1;
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
		LuaObjectProxy<CharacterDynamicStat>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Item>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Equipment_Attributes>::RegisterMetatable(L);
		LuaHandleProxy<esv::Character>::RegisterMetatable(L);
		LuaHandleProxy<esv::PlayerCustomData>::RegisterMetatable(L);
		LuaHandleProxy<esv::Item>::RegisterMetatable(L);
		LuaStatusHandleProxy::RegisterMetatable(L);
		LuaHandleProxy<CDivinityStats_Character>::RegisterMetatable(L);
		LuaHandleProxy<CDivinityStats_Item>::RegisterMetatable(L);
		LuaStatsExtraDataProxy::RegisterMetatable(L);
		LuaStatsProxy::RegisterMetatable(L);
		LuaSkillPrototypeProxy::RegisterMetatable(L);
		LuaDamageList::RegisterMetatable(L);
		LuaTurnManagerCombatProxy::RegisterMetatable(L);
		LuaTurnManagerTeamProxy::RegisterMetatable(L);
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
	int StatAddCustomDescription(lua_State * L);
	int GetStat(lua_State * L);
	int GetCharacter(lua_State * L);
	int GetItem(lua_State * L);
	int GetStatus(lua_State * L);
	int GetCombat(lua_State * L);
	int NewDamageList(lua_State * L);
	int OsirisIsCallable(lua_State * L);
	int LuaRandom(lua_State * L);
	int LuaRound(lua_State * L);
	int GenerateIdeHelpers(lua_State * L);

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
			{"StatAddCustomDescription", StatAddCustomDescription},
			{"GetStat", GetStat},
			{"GetCharacter", GetCharacter},
			{"GetItem", GetItem},
			{"GetStatus", GetStatus},
			{"GetCombat", GetCombat},
			{"NewDamageList", NewDamageList},
			{"OsirisIsCallable", OsirisIsCallable},
			{"Random", LuaRandom},
			{"Round", LuaRound},
			{"GenerateIdeHelpers", GenerateIdeHelpers},
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


	bool LuaState::SkillGetDescriptionParam(SkillPrototype * prototype, CDivinityStats_Character * character,
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

	bool LuaState::GetSkillDamage(SkillPrototype * skill, DamagePairList * damageList,
		CRPGStats_ObjectInstance *attacker, bool isFromItem, bool stealthed, float * attackerPosition,
		float * targetPosition, DeathType * pDeathType, int level, bool noRandomization)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAll);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_GetSkillDamage"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto luaSkill = LuaSkillPrototypeProxy::New(L, skill, -1); // stack: fn, skill
		LuaSkillPrototypePin _(luaSkill);
		LuaObjectProxy<CDivinityStats_Character> * luaAttackerChar{ nullptr };
		LuaObjectProxy<CDivinityStats_Item> * luaAttackerItem{ nullptr };
		LuaStatsProxy * luaAttackerObject{ nullptr };

		if (attacker == nullptr) {
			lua_pushnil(L);
		} else if (attacker->ModifierListIndex == gStaticSymbols.GetStats()->modifierList.FindIndex(ToFixedString("Character"))) {
			auto ch = reinterpret_cast<CDivinityStats_Character *>(attacker);
			luaAttackerChar = LuaObjectProxy<CDivinityStats_Character>::New(L, ch);
		} else if (attacker->ModifierListIndex == gStaticSymbols.GetStats()->modifierList.FindIndex(ToFixedString("Item"))) {
			auto it = reinterpret_cast<CDivinityStats_Item *>(attacker);
			luaAttackerItem = LuaObjectProxy<CDivinityStats_Item>::New(L, it);
		} else {
			luaAttackerObject = LuaStatsProxy::New(L, attacker, -1);
			OsiWarnS("Could not determine stats type of attacker");
		}

		lua_push(L, isFromItem);
		lua_push(L, stealthed);
		
		// Push attacker position
		lua_newtable(L);
		luaL_settable(L, 1, attackerPosition[0]);
		luaL_settable(L, 2, attackerPosition[1]);
		luaL_settable(L, 3, attackerPosition[2]);

		// Push target position
		lua_newtable(L);
		luaL_settable(L, 1, targetPosition[0]);
		luaL_settable(L, 2, targetPosition[1]);
		luaL_settable(L, 3, targetPosition[2]);

		lua_push(L, level);
		lua_push(L, noRandomization);

		bool succeeded = (CallWithTraceback(8, 2) != 0);

		if (luaAttackerChar != nullptr) {
			luaAttackerChar->Unbind();
		}
		if (luaAttackerItem != nullptr) {
			luaAttackerItem->Unbind();
		}
		if (luaAttackerObject != nullptr) {
			luaAttackerObject->Unbind();
		}

		if (succeeded) { // stack: damageList, deathType
			OsiError("GetSkillDamage handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok;
		if (isnil) {
			ok = false;
		} else {
			ok = true;

			auto deathType = lua_toenum<DeathType>(L, -1);
			if (deathType) {
				if (pDeathType) {
					*pDeathType = *deathType;
				}
			} else {
				OsiErrorS("GetSkillDamage returned invalid death type");
				ok = false;
			}

			if (ok) {
				auto damages = LuaDamageList::AsUserData(L, -2);
				if (damages) {
					auto const & list = damages->Get();
					for (uint32_t i = 0; i < list.Size; i++) {
						auto const & item = list[i];
						damageList->AddDamage(item.DamageType, item.Amount);
					}
				} else {
					OsiErrorS("GetSkillDamage returned invalid damage list object");
					ok = false;
				}
			}
		}

		lua_pop(L, 2); // stack: -
		return ok;
	}

	bool LuaState::StatusGetDescriptionParam(StatusPrototype * prototype, CDivinityStats_Character * statusSource,
		CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, std::wstring & replacement)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAll);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_StatusGetDescriptionParam"); // stack: Ext, fn
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

	bool LuaState::OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAll);

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

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_CalculateTurnOrder"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		LuaTurnManagerCombatProxy::New(L, combatId); // stack: fn, combat
		CombatTeamListToLua(L, combat->NextRoundTeams.Set);

		if (CallWithTraceback(2, 1) != 0) { // stack: retval
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
			} catch (LuaException &) {
				OsiError("UpdateTurnOrder failed");
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
			auto const & mod = mods[i];
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
