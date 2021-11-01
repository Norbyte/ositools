#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>

namespace dse::lua
{
	int LuaStatGetAttribute(lua_State* L, CRPGStats_Object* object, char const* attributeName, std::optional<int> level);
	int LuaStatSetAttribute(lua_State* L, CRPGStats_Object* object, char const* attributeName, int valueIdx);

	char const* const StatsProxy::MetatableName = "CRPGStats_Object";

	int StatsProxy::Index(lua_State* L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Attempted to read property of null CRPGStats_Object object");
		auto attributeName = luaL_checkstring(L, 2);

		return LuaStatGetAttribute(L, obj_, attributeName, level_);
	}

	int StatsProxy::NewIndex(lua_State* L)
	{
		auto attributeName = luaL_checkstring(L, 2);
		return LuaStatSetAttribute(L, obj_, attributeName, 3);
	}


	char const* const SkillPrototypeProxy::MetatableName = "eoc::SkillPrototype";

	SkillPrototypeProxy::SkillPrototypeProxy(SkillPrototype* obj, std::optional<int> level)
		: obj_(obj), level_(level)
	{
		stats_ = obj->GetStats();
	}

	int SkillPrototypeProxy::Index(lua_State* L)
	{
		if (stats_ == nullptr) return luaL_error(L, "Attempted to read property of null SkillPrototype object");
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Level") == 0) {
			push(L, obj_->Level);
			return 1;
		}

		return LuaStatGetAttribute(L, stats_, attributeName, level_);
	}

	char const* const ObjectProxy<CDivinityStats_Character>::MetatableName = "CDivinityStats_Character";

	bool CharacterFetchStat(lua_State* L, CDivinityStats_Character* stats, FixedString const& prop, bool excludeBoosts)
	{
		if (!prop) return false;

		if (prop == GFS.strSight) {
			if (excludeBoosts) {
				push(L, stats->BaseSight);
			} else {
				push(L, stats->Sight);
			}
			return true;
		}

		auto dynamicStat = stats->GetStat(prop, excludeBoosts);
		if (dynamicStat) {
			push(L, *dynamicStat);
			return true;
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = stats->GetAbility(*abilityId, excludeBoosts);
			push(L, abilityLevel);
			return true;
		}

		if (strncmp(prop.Str, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(prop.Str + 7);
			if (talentId) {
				bool hasTalent = stats->HasTalent(*talentId, false);
				push(L, hasTalent);
				return true;
			}
		}

		return false;
	}

	int CharacterFetchStat(lua_State* L, CDivinityStats_Character* stats, char const* propStr, FixedString const& prop)
	{
		if (prop == GFS.strDynamicStats) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto dynamicStat : stats->DynamicStats) {
				push(L, statIdx++);
				ObjectProxy<CharacterDynamicStat>::New(L, GetCurrentLifetime(), dynamicStat);
				lua_settable(L, -3);
			}

			return 1;
		}

		if (prop == GFS.strMainWeapon) {
			auto weapon = stats->GetMainWeapon();
			if (weapon != nullptr) {
				ObjectProxy<CDivinityStats_Item>::New(L, GetCurrentLifetime(), weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (prop == GFS.strOffHandWeapon) {
			auto weapon = stats->GetOffHandWeapon();
			if (weapon != nullptr) {
				ObjectProxy<CDivinityStats_Item>::New(L, GetCurrentLifetime(), weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (prop == GFS.strModId) {
			push(L, gExtender->GetStatLoadOrderHelper().GetStatsEntryMod(stats->Name));
			return 1;
		}

		if (prop == GFS.strNotSneaking) {
			push(L, (bool)(stats->Flags & StatCharacterFlags::IsSneaking));
			return 1;
		}

		if (stats->Character != nullptr) {
			if (prop == GFS.strCharacter) {
				ComponentHandle handle;
				stats->Character->GetObjectHandle(handle);
				if (handle.GetType() == (uint32_t)ObjectType::ClientCharacter) {
					ObjectProxy<ecl::Character>::New(L, handle);
				} else if (handle.GetType() == (uint32_t)ObjectType::ServerCharacter) {
					ObjectProxy<esv::Character>::New(L, handle);
				} else {
					ERR("Unknown character handle type: %d", handle.GetType());
					push(L, nullptr);
				}
				return 1;
			}

			if (prop == GFS.strRotation) {
				push(L, *stats->Character->GetRotation());
				return 1;
			}

			if (prop == GFS.strPosition) {
				push(L, *stats->Character->GetTranslate());
				return 1;
			}

			if (prop == GFS.strMyGuid) {
				push(L, *stats->Character->GetGuid());
				return 1;
			}

			if (prop == GFS.strNetID) {
				push(L, stats->Character->NetID);
				return 1;
			}
		}

		bool fetched;
		if (!prop && strncmp(propStr, "Base", 4) == 0) {
			FixedString baseStatName(propStr + 4);
			fetched = CharacterFetchStat(L, stats, baseStatName, true);
		} else {
			fetched = CharacterFetchStat(L, stats, prop, false);
		}

		if (!fetched) {
			fetched = LuaPropertyMapGet(L, gCharacterStatsPropertyMap, stats, prop, false);
		}

		if (fetched) {
			return 1;
		}

		OsiError("Unknown character stats property: " << propStr);
		return 0;
	}

	CDivinityStats_Character* ObjectProxy<CDivinityStats_Character>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;
		auto character = esv::GetEntityWorld()->GetCharacter(handle_);
		if (character == nullptr) luaL_error(L, "Character handle invalid");
		if (character->Stats == nullptr) luaL_error(L, "Character has no stats!");
		return character->Stats;
	}

	int CharacterGetItemBySlot(lua_State* L)
	{
		auto self = checked_get<ObjectProxy<CDivinityStats_Character>*>(L, 1);
		auto slot = checked_get<ItemSlot>(L, 2);

		auto item = self->Get(L)->GetItemBySlot(slot, true);
		if (item != nullptr) {
			ObjectProxy<CDivinityStats_Item>::New(L, GetCurrentLifetime(), item);
			return 1;
		}
		else {
			return 0;
		}
	}

	int ObjectProxy<CDivinityStats_Character>::Index(lua_State* L)
	{
		auto stats = Get(L);
		if (!stats) return 0;

		auto prop = luaL_checkstring(L, 2);
		FixedString fs(prop);

		if (fs == GFS.strGetItemBySlot) {
			lua_pushcfunction(L, &CharacterGetItemBySlot);
			return 1;
		}

		return CharacterFetchStat(L, stats, prop, fs);
	}

	int ObjectProxy<CDivinityStats_Character>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gCharacterStatsPropertyMap);
	}

	int ItemFetchStat(lua_State* L, CDivinityStats_Item* item, char const* prop)
	{
		if (strcmp(prop, "DynamicStats") == 0) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto dynamicStat : item->DynamicAttributes) {
				push(L, statIdx++);
				ObjectProxy<CDivinityStats_Equipment_Attributes>::New(L, GetCurrentLifetime(), dynamicStat);
				lua_settable(L, -3);
			}

			return 1;
		}

		if (strncmp(prop, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(prop + 7);
			if (talentId) {
				bool hasTalent = item->HasTalent(*talentId);
				push(L, hasTalent);
				return 1;
			}
			else {
				return 0;
			}
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = item->GetAbility(*abilityId);
			push(L, abilityLevel);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item, prop, false);
		if (fetched) {
			return 1;
		}

		return LuaStatGetAttribute(L, item, prop, {});
	}


	char const* const ObjectProxy<CDivinityStats_Item>::MetatableName = "CDivinityStats_Item";

	CDivinityStats_Item* ObjectProxy<CDivinityStats_Item>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;
		auto item = esv::GetEntityWorld()->GetItem(handle_);
		if (item == nullptr) luaL_error(L, "Item handle invalid");
		if (item->Stats == nullptr) luaL_error(L, "Item has no stats!");
		return item->Stats;
	}

	int ObjectProxy<CDivinityStats_Item>::Index(lua_State* L)
	{
		auto stats = Get(L);
		if (!stats) return 0;


		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, stats, prop);
	}

	int ObjectProxy<CDivinityStats_Item>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gItemStatsPropertyMap);
	}


	char const* const ObjectProxy<CDivinityStats_Equipment_Attributes>::MetatableName = "CDivinityStats_Equipment_Attributes";

	CDivinityStats_Equipment_Attributes* ObjectProxy<CDivinityStats_Equipment_Attributes>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "Equipment stats no longer available");
		return nullptr;
	}

	int ObjectProxy<CDivinityStats_Equipment_Attributes>::Index(lua_State* L)
	{
		auto stats = Get(L);
		if (!stats) return 0;

		return GenericGetter(L, stats->GetPropertyMap());
	}

	int ObjectProxy<CDivinityStats_Equipment_Attributes>::NewIndex(lua_State* L)
	{
		auto stats = Get(L);
		if (!stats) return 0;

		return GenericSetter(L, stats->GetPropertyMap());
	}


	char const* const ObjectProxy<CharacterDynamicStat>::MetatableName = "CharacterDynamicStat";

	CharacterDynamicStat* ObjectProxy<CharacterDynamicStat>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "Character stats no longer available");
		return nullptr;
	}

	int ObjectProxy<CharacterDynamicStat>::Index(lua_State* L)
	{
		return GenericGetter(L, gCharacterDynamicStatPropertyMap);
	}

	int ObjectProxy<CharacterDynamicStat>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gCharacterDynamicStatPropertyMap);
	}


	ItemOrCharacterPushPin::ItemOrCharacterPushPin(lua_State* L, CRPGStats_Object* obj)
	{
		if (obj == nullptr) {
			lua_pushnil(L);
		}
		else if (obj->ModifierListIndex == GetStaticSymbols().GetStats()->modifierList.FindIndex(FixedString("Character"))) {
			auto ch = reinterpret_cast<CDivinityStats_Character*>(obj);
			character_ = ObjectProxy<CDivinityStats_Character>::New(L, GetCurrentLifetime(), ch);
		}
		else if (obj->ModifierListIndex == GetStaticSymbols().GetStats()->modifierList.FindIndex(FixedString("Item"))) {
			auto item = reinterpret_cast<CDivinityStats_Item*>(obj);
			item_ = ObjectProxy<CDivinityStats_Item>::New(L, GetCurrentLifetime(), item);
		}
		else {
			object_ = StatsProxy::New(L, obj, -1);
			OsiWarnS("Could not determine stats type of object");
		}
	}

	ItemOrCharacterPushPin::~ItemOrCharacterPushPin()
	{
		if (character_) character_->Unbind();
		if (item_) item_->Unbind();
		if (object_) object_->Unbind();
	}


	char const* const StatsExtraDataProxy::MetatableName = "CRPGStats_ExtraData";

	int StatsExtraDataProxy::Index(lua_State* L)
	{
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr || stats->ExtraData == nullptr) return luaL_error(L, "Stats not available");

		auto key = luaL_checkstring(L, 2);
		auto extraData = stats->ExtraData->Properties.Find(FixedString(key));
		if (extraData != nullptr) {
			push(L, *extraData);
		}
		else {
			push(L, nullptr);
		}

		return 1;
	}

	int StatsExtraDataProxy::NewIndex(lua_State* L)
	{
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr || stats->ExtraData == nullptr) return luaL_error(L, "Stats not available");

		auto key = luaL_checkstring(L, 2);
		auto value = checked_get<float>(L, 3);
		auto extraData = stats->ExtraData->Properties.Find(FixedString(key));
		if (extraData != nullptr) {
			*extraData = value;
		}
		else {
			LuaError("Cannot set nonexistent ExtraData value '" << key << "'");
		}

		return 0;
	}


	void RegisterStatsObjects(lua_State* L)
	{
		ObjectProxy<CDivinityStats_Character>::RegisterMetatable(L);
		ObjectProxy<CharacterDynamicStat>::RegisterMetatable(L);
		ObjectProxy<CDivinityStats_Item>::RegisterMetatable(L);
		ObjectProxy<CDivinityStats_Equipment_Attributes>::RegisterMetatable(L);
		StatsExtraDataProxy::RegisterMetatable(L);
		StatsProxy::RegisterMetatable(L);
		SkillPrototypeProxy::RegisterMetatable(L);
	}
	
	
	void FetchSkillSetEntries(lua_State * L, CRPGStatsManager * stats)
	{
		int32_t index = 1;
		for (auto skillSet : stats->SkillSetManager->Primitives) {
			settable(L, index++, skillSet->Name);
		}
	}

	void FetchItemComboEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		for (auto itemCombo : stats->ItemCombinationManager->Primitives) {
			settable(L, index++, itemCombo->Name);
		}
	}

	void FetchItemComboPropertyEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		stats->ItemCombinationManager->ComboProperties.Iterate([L, &index](FixedString const& key, CItemCombinationProperty const*) {
			settable(L, index++, key);
		});
	}

	void FetchItemComboPreviewDataEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		stats->ItemCombinationManager->PreviewData.Iterate([L, &index](FixedString const& key, CItemCombinationPreviewData const*) {
			settable(L, index++, key);
		});
	}

	void FetchItemGroupEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		stats->ItemProgressionManager->ItemGroups.Iterate([L, &index](FixedString const& key, CItemGroup*) {
			settable(L, index++, key);
		});
	}

	void FetchItemNameGroupEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		stats->ItemProgressionManager->NameGroups.Iterate([L, &index](FixedString const& key, CNameGroup*) {
			settable(L, index++, key);
		});
	}

	void FetchDeltaModEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		for (auto deltaModList : stats->DeltaMods.Primitives) {
			for (auto deltaMod : deltaModList->Primitives) {
				push(L, index++);
				lua_newtable(L);
				setfield(L, "Name", deltaMod->Name);
				setfield(L, "ModifierType", deltaMod->ModifierType);
				lua_settable(L, -3);
			}
		}
	}

	void FetchEquipmentSetEntries(lua_State * L, CRPGStatsManager * stats)
	{
		int32_t index = 1;
		for (auto equipmentSet : stats->EquipmentSetManager->Primitives) {
			settable(L, index++, equipmentSet->Name);
		}
	}

	void FetchTreasureTableEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		for (auto treasureTable : stats->TreasureTables.Primitives) {
			settable(L, index++, treasureTable->Name);
		}
	}

	void FetchTreasureCategoryEntries(lua_State* L, CRPGStatsManager* stats)
	{
		int32_t index = 1;
		for (auto treasureCategory : stats->TreasureCategories.Primitives) {
			settable(L, index++, treasureCategory->Category);
		}
	}

	void FetchStatEntries(lua_State * L, CRPGStatsManager * stats, FixedString const& statType)
	{
		ModifierList* modifierList{ nullptr };
		if (statType) {
			modifierList = stats->modifierList.Find(statType);
			if (modifierList == nullptr) {
				OsiError("Unknown stats entry type: " << statType);
				return;
			}
		}

		int32_t index = 1;
		for (auto object : stats->objects.Primitives) {
			if (statType) {
				auto type = stats->GetTypeInfo(object);
				if (modifierList != nullptr && type != modifierList) {
					continue;
				}
			}

			settable(L, index++, object->Name);
		}
	}

	void FetchStatEntriesBefore(lua_State* L, CRPGStatsManager* stats, FixedString const& modId, FixedString const& statType)
	{
		ModifierList* modifierList{ nullptr };
		if (statType) {
			modifierList = stats->modifierList.Find(statType);
			if (modifierList == nullptr) {
				OsiError("Unknown stats entry type: " << statType);
				return;
			}
		}

		auto entries = gExtender->GetStatLoadOrderHelper().GetStatsLoadedBefore(modId);

		int32_t index = 1;
		for (auto object : entries) {
			if (statType) {
				auto type = stats->GetTypeInfo(object);
				if (modifierList != nullptr && type != modifierList) {
					continue;
				}
			}

			settable(L, index++, object->Name);
		}
	}

	int GetStatEntries(lua_State * L)
	{
		StackCheck _(L, 1);
		FixedString statType;
		if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
			statType = checked_get<FixedString>(L, 1);
		}
		
		lua_newtable(L);
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 1;
		}

		if (statType == GFS.strSkillSet) {
			FetchSkillSetEntries(L, stats);
		} else if (statType == GFS.strEquipmentSet) {
			FetchEquipmentSetEntries(L, stats);
		} else if (statType == GFS.strTreasureTable) {
			FetchTreasureTableEntries(L, stats);
		} else if (statType == GFS.strTreasureCategory) {
			FetchTreasureCategoryEntries(L, stats);
		} else if (statType == GFS.strItemCombination) {
			FetchItemComboEntries(L, stats);
		} else if (statType == GFS.strItemComboProperty) {
			FetchItemComboPropertyEntries(L, stats);
		} else if (statType == GFS.strCraftingPreviewData) {
			FetchItemComboPreviewDataEntries(L, stats);
		} else if (statType == GFS.strItemGroup) {
			FetchItemGroupEntries(L, stats);
		} else if (statType == GFS.strNameGroup) {
			FetchItemNameGroupEntries(L, stats);
		} else if (statType == GFS.strDeltaMod) {
			FetchDeltaModEntries(L, stats);
		} else {
			FetchStatEntries(L, stats, statType);
		}

		return 1;
	}

	int GetStatEntriesLoadedBefore(lua_State* L)
	{
		StackCheck _(L, 1);
		auto modId = checked_get<FixedString>(L, 1);
		FixedString statType;
		if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
			statType = checked_get<FixedString>(L, 2);
		}

		lua_newtable(L);
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 1;
		}

		FetchStatEntriesBefore(L, stats, modId, statType);

		return 1;
	}

	int GetSkillSet(lua_State * L)
	{
		auto skillSetName = luaL_checkstring(L, 1);
		auto stats = GetStaticSymbols().GetStats();
		auto skillSet = stats->SkillSetManager->Find(skillSetName);
		return LuaWrite(L, skillSet);
	}

	int UpdateSkillSet(lua_State* L)
	{
		StackCheck _(L);
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_getfield<FixedString>(L, "Name", 1);

		auto stats = GetStaticSymbols().GetStats();
		auto skillSet = stats->SkillSetManager->Find(name);
		bool isNew = (skillSet == nullptr);

		lua_pushvalue(L, 1);
		LuaRead(L, skillSet);
		lua_pop(L, 1);

		if (isNew) {
			stats->SkillSetManager->Add(name, skillSet);
		}

		return 0;
	}

	int GetEquipmentSet(lua_State * L)
	{
		auto equipmentSetName = luaL_checkstring(L, 1);
		auto stats = GetStaticSymbols().GetStats();
		auto equipmentSet = stats->EquipmentSetManager->Find(equipmentSetName);
		return LuaWrite(L, equipmentSet);
	}

	int UpdateEquipmentSet(lua_State* L)
	{
		StackCheck _(L);
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_getfield<FixedString>(L, "Name", 1);

		auto stats = GetStaticSymbols().GetStats();
		auto equipmentSet = stats->EquipmentSetManager->Find(name);
		bool isNew = (equipmentSet == nullptr);

		lua_pushvalue(L, 1);
		LuaRead(L, equipmentSet);
		lua_pop(L, 1);

		if (isNew) {
			stats->EquipmentSetManager->Add(name, equipmentSet);
		}

		return 0;
	}

	int GetTreasureTable(lua_State* L)
	{
		auto tableName = luaL_checkstring(L, 1);
		auto stats = GetStaticSymbols().GetStats();
		auto table = stats->TreasureTables.Find(tableName);
		return LuaWrite(L, table);
	}

	int UpdateTreasureTable(lua_State* L)
	{
		StackCheck _(L);
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_getfield<FixedString>(L, "Name", 1);

		auto stats = GetStaticSymbols().GetStats();
		auto table = stats->TreasureTables.Find(name);
		bool isNew = (table == nullptr);

		lua_pushvalue(L, 1);
		LuaRead(L, table);
		lua_pop(L, 1);

		if (isNew) {
			stats->TreasureTables.Add(name, table);
		}

		return 0;
	}

	int GetTreasureCategory(lua_State* L)
	{
		auto categoryName = luaL_checkstring(L, 1);
		auto const* stats = GetStaticSymbols().GetStats();
		auto category = stats->TreasureCategories.Find(categoryName);
		return LuaWrite(L, category);
	}

	int UpdateTreasureCategory(lua_State* L)
	{
		StackCheck _(L);
		auto name = checked_get<FixedString>(L, 1);
		luaL_checktype(L, 2, LUA_TTABLE);

		auto stats = GetStaticSymbols().GetStats();
		auto category = stats->TreasureCategories.Find(name);
		bool isNew = (category == nullptr);

		lua_pushvalue(L, 2);
		LuaRead(L, category);
		lua_pop(L, 1);

		if (isNew) {
			stats->TreasureCategories.Add(name, category);
		}

		return 0;
	}

	int GetItemCombo(lua_State* L)
	{
		auto comboName = luaL_checkstring(L, 1);
		auto combo = GetStaticSymbols().GetStats()->ItemCombinationManager->Find(comboName);
		return LuaWrite(L, combo);
	}

	int UpdateItemCombo(lua_State* L)
	{
		StackCheck _(L);
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_getfield<FixedString>(L, "Name", 1);

		auto stats = GetStaticSymbols().GetStats();
		auto combo = stats->ItemCombinationManager->Find(name);
		bool isNew = (combo == nullptr);

		lua_pushvalue(L, 1);
		LuaRead(L, combo);
		lua_pop(L, 1);

		if (isNew) {
			stats->ItemCombinationManager->Add(name, combo);
		}

		return 0;
	}

	int GetItemComboPreviewData(lua_State* L)
	{
		FixedString comboName(luaL_checkstring(L, 1));
		auto preview = GetStaticSymbols().GetStats()->ItemCombinationManager->PreviewData.Find(comboName);
		return LuaWrite(L, preview);
	}

	int UpdateItemComboPreviewData(lua_State* L)
	{
		StackCheck _(L);
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_getfield<FixedString>(L, "Name", 1);

		auto stats = GetStaticSymbols().GetStats();
		auto existing = stats->ItemCombinationManager->PreviewData.Find(name);
		CItemCombinationPreviewData* previewData = existing ? *existing : nullptr;
		bool isNew = (previewData == nullptr);

		lua_pushvalue(L, 1);
		LuaRead(L, previewData);
		lua_pop(L, 1);

		if (isNew) {
			stats->ItemCombinationManager->PreviewData.Insert(name, previewData);
		}

		return 0;
	}

	int GetItemComboProperty(lua_State* L)
	{
		FixedString propertyName(luaL_checkstring(L, 1));
		auto prop = GetStaticSymbols().GetStats()->ItemCombinationManager->ComboProperties.Find(propertyName);
		return LuaWrite(L, prop);
	}

	int UpdateItemComboProperty(lua_State* L)
	{
		StackCheck _(L);
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_getfield<FixedString>(L, "Name", 1);

		auto stats = GetStaticSymbols().GetStats();
		auto existing = stats->ItemCombinationManager->ComboProperties.Find(name);
		CItemCombinationProperty* comboProperty = existing ? *existing : nullptr;
		bool isNew = (comboProperty == nullptr);

		lua_pushvalue(L, 1);
		LuaRead(L, comboProperty);
		lua_pop(L, 1);

		if (isNew) {
			stats->ItemCombinationManager->ComboProperties.Insert(name, comboProperty);
		}

		return 0;
	}


	int GetItemGroup(lua_State* L)
	{
		FixedString name(luaL_checkstring(L, 1));
		auto group = GetStaticSymbols().GetStats()->ItemProgressionManager->ItemGroups.Find(name);
		return LuaWrite(L, group);
	}


	int GetNameGroup(lua_State* L)
	{
		FixedString name(luaL_checkstring(L, 1));
		auto nameGroup = GetStaticSymbols().GetStats()->ItemProgressionManager->NameGroups.Find(name);
		return LuaWrite(L, nameGroup);
	}


	CRPGStats_Object_Property_List* LuaToObjectPropertyList(lua_State* L, FixedString const& propertyName)
	{
		auto properties = GetStaticSymbols().GetStats()->ConstructPropertyList(propertyName);
		LuaSerializer s(L, false);
		s << properties;
		return properties;
	}

	int LuaStatGetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, std::optional<int> level)
	{
		StackCheck _(L, 1);
		auto stats = GetStaticSymbols().GetStats();

		FixedString attributeFS(attributeName);

		if (attributeFS == GFS.strLevel) {
			push(L, object->Level);
			return 1;
		} else if (attributeFS == GFS.strName) {
			push(L, object->Name);
			return 1;
		} else if (attributeFS == GFS.strModId) {
			push(L, gExtender->GetStatLoadOrderHelper().GetStatsEntryMod(object->Name));
			return 1;
		} else if (attributeFS == GFS.strUsing) {
			if (object->Using) {
				auto parent = stats->objects.Find(object->Using);
				if (parent != nullptr) {
					push(L, parent->Name);
					return 1;
				}
			}

			push(L, nullptr);
			return 1;
		} else if (attributeFS == GFS.strRequirements) {
			return LuaWrite(L, object->Requirements);
		} else if (attributeFS == GFS.strMemorizationRequirements) {
			return LuaWrite(L, object->MemorizationRequirements);
		} else if (attributeFS == GFS.strAIFlags) {
			push(L, object->AIFlags);
			return 1;
		} else if (attributeFS == GFS.strComboCategory) {
			return LuaWrite(L, object->ComboCategories);
		} else if (attributeFS == GFS.strSkillProperties || attributeFS == GFS.strExtraProperties) {
			auto propertyList = object->PropertyList.Find(attributeFS);
			if (propertyList) {
				return LuaWrite(L, *propertyList);
			} else {
				push(L, nullptr);
				return 1;
			}
		}

		int index;
		auto attrInfo = stats->GetAttributeInfo(object, attributeFS, index);
		if (attrInfo && attrInfo->Name == GFS.strConditions) {
			auto conditions = object->ConditionList.Find(attributeFS);
			if (conditions) {
				OsiError("Conditions property '" << attributeFS << "' is not readable");
			}

			push(L, nullptr);
			return 1;
		}

		auto value = stats->GetAttributeString(object, attributeFS);
		if (!value) {
			std::optional<int> intval;
			if (level) {
				if (*level == -1) {
					*level = object->Level;
				}

				intval = stats->GetAttributeIntScaled(object, attributeFS, *level);
			} else {
				intval = stats->GetAttributeInt(object, attributeFS);
			}

			if (!intval) {
				OsiError("Stat object '" << object->Name << "' has no attribute named '" << attributeFS << "'");
				push(L, nullptr);
			} else {
				push(L, *intval);
			}
		} else {
			push(L, *value);
		}

		return 1;
	}

	int StatGetAttribute(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);

		auto object = StatFindObject(statName);
		if (!object) {
			push(L, nullptr);
			return 1;
		}

		return LuaStatGetAttribute(L, object, attributeName, {});
	}

	int StatGetAttributeScaled(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);

		auto object = StatFindObject(statName);
		if (!object) {
			push(L, nullptr);
			return 1;
		}

		return LuaStatGetAttribute(L, object, attributeName, {});
	}

	int LuaStatSetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, int valueIdx)
	{
		StackCheck _(L);
		LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
		if (lua->RestrictionFlags & State::ScopeModulePreLoad) {
			return luaL_error(L, "Stat functions unavailable during module preload");
		}

		if (!(lua->RestrictionFlags & State::ScopeModuleLoad)) {
			static bool syncWarningShown{ false };
			if (!syncWarningShown) {
				OsiWarn("Stats edited after ModuleLoad must be synced manually; make sure that you call SyncStat() on it when you're finished!");
				syncWarningShown = true;
			}
		}

		FixedString attributeFS(attributeName);

		if (attributeFS == GFS.strLevel) {
			object->Level = (int32_t)luaL_checkinteger(L, valueIdx);
			return 0;
		} else if (attributeFS == GFS.strRequirements) {
			LuaRead(L, object->Requirements);
			return 0;
		} else if (attributeFS == GFS.strMemorizationRequirements) {
			LuaRead(L, object->MemorizationRequirements);
			return 0;
		} else if (attributeFS == GFS.strAIFlags) {
			object->AIFlags = FixedString(lua_tostring(L, valueIdx));
			return 0;
		} else if (attributeFS == GFS.strComboCategory) {
			object->ComboCategories.Clear();
			if (lua_type(L, valueIdx) != LUA_TTABLE) {
				OsiError("Must pass a table when setting ComboCategory");
				return 0;
			}

			for (auto category : iterate(L, valueIdx)) {
				auto categoryName = checked_get<char const*>(L, category);
				object->ComboCategories.Add(FixedString(categoryName));
			}

			return 0;
		} else if (attributeFS == GFS.strSkillProperties || attributeFS == GFS.strExtraProperties) {
			STDString name = object->Name.Str;
			name += "_";
			name += attributeName;
			FixedString statsPropertyKey(name.c_str());

			auto newList = LuaToObjectPropertyList(L, statsPropertyKey);
			if (newList) {
				auto propertyList = object->PropertyList.Find(FixedString(attributeName));
				if (propertyList) {
					// FIXME - add Remove() support!
					object->PropertyList.Clear();
					// FIXME - need to remove from stats.PropertyLists too!
					// GameFree(*propertyList);
				}

				object->PropertyList.Insert(FixedString(attributeName), newList);
			}

			return 0;
		}

		int index;
		auto stats = GetStaticSymbols().GetStats();
		auto attrInfo = stats->GetAttributeInfo(object, attributeFS, index);
		if (attrInfo && attrInfo->Name == GFS.strConditions) {
			auto conditions = object->ConditionList.Find(attributeFS);
			if (conditions) {
				auto value = luaL_checkstring(L, valueIdx);
				auto scriptCheckBlock = stats->BuildScriptCheckBlockFromProperties(value);
				if (scriptCheckBlock) {
					auto statConditions = GameAlloc<CDivinityStats_Condition>();
					statConditions->ScriptCheckBlock = scriptCheckBlock;
					STDString name = object->Name.Str;
					name += "_";
					name += attributeName;
					statConditions->Name = FixedString(name.c_str());
					*conditions = statConditions;
				} else {
					OsiWarn("Failed to parse conditions: " << value);
				}
			} else {
				// FIXME - not implemented yet!
				OsiWarnS("Adding new Conditions entries not implemented yet!");
			}

			return 0;
		}

		switch (lua_type(L, valueIdx)) {
		case LUA_TSTRING:
		{
			auto value = luaL_checkstring(L, valueIdx);
			stats->SetAttributeString(object, attributeFS, value);
			break;
		}

		case LUA_TNUMBER:
		{
			auto value = (int32_t)luaL_checkinteger(L, valueIdx);
			stats->SetAttributeInt(object, attributeFS, value);
			break;
		}

		default:
			return luaL_error(L, "Expected a string or integer attribute value.");
		}

		return 0;
	}

	int StatSetAttribute(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);

		auto object = StatFindObject(statName);
		if (!object) return 0;

		return LuaStatSetAttribute(L, object, attributeName, 3);
	}

	void StatAddCustomDescription(lua_State * L, const char* statName, const char* attributeName, const char* description)
	{
		LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
		if (lua->RestrictionFlags & State::ScopeModulePreLoad) {
			luaL_error(L, "Stat functions unavailable during module preload");
		}

		if (!(lua->RestrictionFlags & State::ScopeModuleLoad)) {
			luaL_error(L, "StatAddCustomDescription() can only be called during module load");
		}

		auto object = StatFindObject(statName);
		if (!object) return;

		auto props = object->PropertyList.Find(FixedString(attributeName));
		if (props == nullptr || *props == nullptr) {
			OsiError("Stat object '" << object->Name << "' has no property list named '" << attributeName << "'");
			return;
		}

		// FIXME - assign name + add to map properly
		auto customProp = GameAlloc<CRPGStats_Object_Property_CustomDescription>();
		customProp->Context = (CRPGStats_Object_PropertyContext)0;
		customProp->TypeId = CRPGStats_Object_Property_Type::CustomDescription;
		customProp->Conditions = nullptr;
		customProp->TextLine1 = FromUTF8(description);
		(*props)->Properties.Primitives.Add(customProp);
	}

	WrapLuaFunction(StatAddCustomDescription)


	struct CRPGStats_CustomLevelMap : public CRPGStats_LevelMap
	{
		RegistryEntry Function;
		CRPGStats_LevelMap * OriginalLevelMap{ nullptr };

		CRPGStats_CustomLevelMap() {}
		~CRPGStats_CustomLevelMap() override {}
		
		void SetModifierList(int modifierListIndex, int modifierIndex) override
		{
			OsiError("Not supported!");
		}

		void SetModifierList(FixedString const& modifierListName, FixedString const& modifierName) override
		{
			OsiError("Not supported!");
		}

		int64_t GetScaledValue(int attributeValue, int level) override
		{
			auto value = LuaGetScaledValue(attributeValue, level);
			if (value) {
				return *value;
			} else {
				return OriginalLevelMap->GetScaledValue(attributeValue, level);
			}
		}

		std::optional<int64_t> LuaGetScaledValue(int attributeValue, int level)
		{
			// GetScaledValue must always use the client pin, as the override function is
			// reigstered from the client state
			ecl::LuaClientPin pin(ecl::ExtensionState::Get());
			if (!pin) return {};

			Restriction restriction(*pin, State::RestrictAll);

			auto L = pin->GetState();
			Function.Push();

			push(L, attributeValue);
			push(L, level);

			if (lua_pcall(L, 2, 1, 0) != 0) { // stack: retval
				OsiError("Level scaled value fetch failed: " << lua_tostring(L, -1));
				lua_pop(L, 1);
				return {};
			}

			if (lua_type(L, -1) != LUA_TNUMBER) {
				OsiErrorS("Level scaled fetcher returned non-numeric value");
				return {};
			}

			auto value = lua_tointeger(L, -1);
			lua_pop(L, 1); // stack: -
			return value;
		}
	};

	void RestoreLevelMaps(std::unordered_set<int32_t> const & levelMapIds)
	{
		auto & levelMaps = GetStaticSymbols().GetStats()->LevelMaps.Primitives;
		for (auto levelMapIndex : levelMapIds) {
			auto levelMap = static_cast<CRPGStats_CustomLevelMap *>(levelMaps.Buf[levelMapIndex]);
			levelMaps.Buf[levelMapIndex] = levelMap->OriginalLevelMap;
		}

		if (!levelMapIds.empty()) {
			OsiWarn("Restored " << levelMapIds.size() << " level map overrides (Lua VM deleted)");
		}
	}

	int StatSetLevelScaling(lua_State * L)
	{
		StackCheck _(L);
		auto modifierListName = luaL_checkstring(L, 1);
		auto modifierName = luaL_checkstring(L, 2);
		luaL_checktype(L, 3, LUA_TFUNCTION);

		LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
		if (lua->RestrictionFlags & State::ScopeModulePreLoad) {
			return luaL_error(L, "Stat functions unavailable during module preload");
		}

		if (!(lua->RestrictionFlags & (State::ScopeModuleLoad | State::ScopeModuleResume))) {
			return luaL_error(L, "StatSetLevelScaling() can only be called during module load/resume");
		}

		auto stats = GetStaticSymbols().GetStats();
		auto modifier = stats->GetModifierInfo(FixedString(modifierListName), FixedString(modifierName));
		if (modifier == nullptr) {
			OsiError("Modifier list '" << modifierListName << "' or modifier '" << modifierName << "' does not exist!");
			return 0;
		}

		if (modifier->LevelMapIndex == -1) {
			OsiError("Modifier list '" << modifierListName << "', modifier '" << modifierName << "' is not level scaled!");
			return 0;
		}

		CRPGStats_LevelMap * originalLevelMap;
		auto currentLevelMap = stats->LevelMaps.Find(modifier->LevelMapIndex);
		
		auto it = lua->OverriddenLevelMaps.find(modifier->LevelMapIndex);
		if (it != lua->OverriddenLevelMaps.end()) {
			auto overridden = static_cast<CRPGStats_CustomLevelMap *>(currentLevelMap);
			originalLevelMap = overridden->OriginalLevelMap;
		} else {
			originalLevelMap = currentLevelMap;
		}

		auto levelMap = GameAlloc<CRPGStats_CustomLevelMap>();
		levelMap->ModifierListIndex = originalLevelMap->ModifierListIndex;
		levelMap->ModifierIndex = originalLevelMap->ModifierIndex;
		levelMap->RPGEnumerationIndex = originalLevelMap->RPGEnumerationIndex;
		levelMap->Name = originalLevelMap->Name;
		levelMap->Function = RegistryEntry(L, 3);
		levelMap->OriginalLevelMap = originalLevelMap;

		stats->LevelMaps.Primitives.Buf[modifier->LevelMapIndex] = levelMap;
		lua->OverriddenLevelMaps.insert(modifier->LevelMapIndex);

		return 0;
	}

	int GetStat(lua_State * L)
	{
		StackCheck _(L, 1);
		auto statName = luaL_checkstring(L, 1);
		std::optional<int> level;
		bool warnOnError{ true };
		if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
			level = (int32_t)luaL_checkinteger(L, 2);
		}
		if (lua_gettop(L) >= 3) {
			warnOnError = checked_get<bool>(L, 3);
		}
		
		auto object = StatFindObject(statName, warnOnError);
		if (object != nullptr) {
			StatsProxy::New(L, object, level);
			return 1;
		} else {
			push(L, nullptr);
			return 1;
		}
	}

	bool CopyStats(CRPGStats_Object* obj, char const* copyFrom)
	{
		auto stats = GetStaticSymbols().GetStats();
		auto copyFromObject = stats->objects.Find(FixedString(copyFrom));
		if (copyFromObject == nullptr) {
			OsiError("Cannot copy stats from nonexistent object: " << copyFrom);
			return false;
		}

		if (obj->ModifierListIndex != copyFromObject->ModifierListIndex) {
			auto objModifier = stats->modifierList.Find(obj->ModifierListIndex);
			auto copyModifier = stats->modifierList.Find(obj->ModifierListIndex);
			OsiError("Cannot copy stats from object '" << copyFrom << "' (a " << copyModifier->Name.Str 
				<< ") to an object of type " << objModifier->Name.Str);
			return false;
		}

		obj->Level = copyFromObject->Level;
		obj->AIFlags = copyFromObject->AIFlags;

		for (size_t i = 0; i < obj->IndexedProperties.size(); i++) {
			obj->IndexedProperties[i] = copyFromObject->IndexedProperties[i];
		}

		copyFromObject->PropertyList.Iterate([obj](auto const& key, auto propertyList) {
			// TODO - is reusing property list objects allowed?
			obj->PropertyList.Insert(key, propertyList);
		});

		copyFromObject->ConditionList.Iterate([obj](auto const& key, auto propertyList) {
			// TODO - is reusing condition objects allowed?
			obj->ConditionList.Insert(key, propertyList);
		});

		obj->Requirements = copyFromObject->Requirements;
		obj->MemorizationRequirements = copyFromObject->MemorizationRequirements;
		obj->ComboCategories = copyFromObject->ComboCategories;

		return true;
	}

	int CreateStat(lua_State * L)
	{
		StackCheck _(L, 1);
		auto statName = luaL_checkstring(L, 1);
		auto modifierName = luaL_checkstring(L, 2);
		char const* copyFrom{ nullptr };
		if (lua_gettop(L) >= 3) {
			copyFrom = luaL_checkstring(L, 3);
		}

		LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
		if (lua->RestrictionFlags & State::ScopeModulePreLoad) {
			return luaL_error(L, "Stat functions unavailable during module preload");
		}

		if (!(lua->RestrictionFlags & State::ScopeModuleLoad)) {
			if (gExtender->GetServer().IsInServerThread()) {
				static bool syncWarningShown{ false };
				if (!syncWarningShown) {
					OsiWarn("Stats entres created after ModuleLoad must be synced manually; make sure that you call SyncStat() on it when you're finished!");
					syncWarningShown = true;
				}
			} else {
				OsiError("Cannot call CreateStat() on client after module load!");
				push(L, nullptr);
				return 1;
			}
		}

		auto stats = GetStaticSymbols().GetStats();
		auto object = stats->CreateObject(FixedString(statName), FixedString(modifierName));
		if (!object) {
			push(L, nullptr);
			return 1;
		}

		if (copyFrom != nullptr) {
			if (!CopyStats(*object, copyFrom)) {
				push(L, nullptr);
				return 1;
			}
		}

		StatsProxy::New(L, *object, -1);
		return 1;
	}

	void SyncStat(lua_State* L, char const* statName, std::optional<bool> persist)
	{
		auto stats = GetStaticSymbols().GetStats();
		auto object = stats->objects.Find(FixedString(statName));
		if (!object) {
			OsiError("Cannot sync nonexistent stat: " << statName);
			return;
		}

		stats->SyncWithPrototypeManager(object);

		if (gExtender->GetServer().IsInServerThread()) {
			object->BroadcastSyncMessage(false);

			gExtender->GetServer().GetExtensionState().MarkDynamicStat(FixedString(statName));
			if (persist && *persist) {
				gExtender->GetServer().GetExtensionState().MarkPersistentStat(FixedString(statName));
			}
		}
	}

	WrapLuaFunction(SyncStat)

	void StatSetPersistence(lua_State* L, char const* statName, bool persist)
	{
		auto stats = GetStaticSymbols().GetStats();
		auto object = stats->objects.Find(FixedString(statName));
		if (!object) {
			OsiError("Cannot set persistence for nonexistent stat: " << statName);
			return;
		}

		if (persist) {
			gExtender->GetServer().GetExtensionState().MarkPersistentStat(FixedString(statName));
		} else {
			gExtender->GetServer().GetExtensionState().UnmarkPersistentStat(FixedString(statName));
		}
	}

	WrapLuaFunction(StatSetPersistence)

	int GetDeltaMod(lua_State* L)
	{
		StackCheck _(L, 1);
		auto name = checked_get<char const *>(L, 1);
		auto modifierType = checked_get<char const*>(L, 2);

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			push(L, nullptr);
			return 1;
		}

		auto deltaModType = stats->DeltaMods.Find(modifierType);
		if (deltaModType == nullptr) {
			OsiError("Unknown DeltaMod ModifierType: " << modifierType);
			push(L, nullptr);
			return 1;
		}

		auto deltaMod = deltaModType->Find(name);
		return LuaWrite(L, deltaMod);
	}

	int UpdateDeltaMod(lua_State* L)
	{
		StackCheck _(L, 0);
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_getfield<char const*>(L, "Name", 1);
		auto modifierType = checked_getfield<char const*>(L, "ModifierType", 1);

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 0;
		}

		auto deltaModType = stats->DeltaMods.Find(modifierType);
		if (deltaModType == nullptr) {
			OsiError("Unknown DeltaMod ModifierType: " << modifierType);
			return 0;
		}

		auto deltaMod = deltaModType->Find(name);
		bool isNew = (deltaMod == nullptr);

		lua_pushvalue(L, 1);
		LuaRead(L, deltaMod);
		lua_pop(L, 1);

		if (isNew) {
			deltaModType->Add(FixedString(name), deltaMod);
		}

		return 0;
	}

	template <class T>
	int EnumIndexToLabel(lua_State* L, T index)
	{
		auto label = EnumInfo<T>::Find(index);
		if (label) {
			push(L, label);
		} else {
			push(L, nullptr);
		}

		return 1;
	}

	template <class T>
	int EnumLabelToIndex(lua_State* L, char const* label)
	{
		auto index = EnumInfo<T>::Find(label);
		if (index) {
			push(L, *index);
		} else {
			push(L, nullptr);
		}

		return 1;
	}

#define BEGIN_BITMASK_NS(NS, T, type)
#define BEGIN_BITMASK(T, type)
#define E(label)
#define EV(label, value)
#define END_ENUM_NS()
#define END_ENUM()

// TODO - this solution has subpar performance
#define BEGIN_ENUM_NS(NS, T, type) \
	if (strcmp(enumName.Str, #T) == 0) { \
		return EnumIndexToLabel<NS::T>(L, (NS::T)index); \
	}
#define BEGIN_ENUM(T, type) \
	if (strcmp(enumName.Str, #T) == 0) { \
		return EnumIndexToLabel<T>(L, (T)index); \
	}

	int EnumIndexToLabel(lua_State* L)
	{
		auto enumName = checked_get<FixedString>(L, 1);
		auto index = checked_get<int>(L, 2);

#include <GameDefinitions/Enumerations.inl>

		auto valueList = GetStaticSymbols().GetStats()->modifierValueList.Find(enumName);
		if (valueList) {
			std::optional<FixedString> value;
			valueList->Values.Iterate([&value, index](auto const& k, auto const& idx) {
				if (idx == index) {
					value = k;
				}
			});

			if (value) {
				push(L, *value);
				return 1;
			} else {
				OsiError("Enumeration '" << enumName << "' has no label with index " << index);
				push(L, nullptr);
				return 1;
			}
		}

		OsiError("No such enumeration: " << enumName);
		push(L, nullptr);
		return 1;
	}

#undef BEGIN_ENUM_NS
#undef BEGIN_ENUM
	
// TODO - this solution has subpar performance
#define BEGIN_ENUM_NS(NS, T, type) \
	if (strcmp(enumName.Str, #T) == 0) { \
		return EnumLabelToIndex<NS::T>(L, label); \
	}
#define BEGIN_ENUM(T, type) \
	if (strcmp(enumName.Str, #T) == 0) { \
		return EnumLabelToIndex<T>(L, label); \
	}


	int EnumLabelToIndex(lua_State* L)
	{
		auto enumName = checked_get<FixedString>(L, 1);
		auto label = checked_get<char const*>(L, 2);

#include <GameDefinitions/Enumerations.inl>

		auto valueList = GetStaticSymbols().GetStats()->modifierValueList.Find(enumName);
		if (valueList) {
			auto value = valueList->Values.Find(FixedString(label));

			if (value) {
				push(L, *value);
				return 1;
			} else {
				OsiError("Enumeration '" << enumName << "' has no label named '" << label << "'");
				push(L, nullptr);
				return 1;
			}
		}

		OsiError("No such enumeration: " << enumName);
		push(L, nullptr);
		return 1;
	}

#undef BEGIN_BITMASK_NS
#undef BEGIN_ENUM_NS
#undef BEGIN_BITMASK
#undef BEGIN_ENUM
#undef E
#undef EV
#undef END_ENUM_NS
#undef END_ENUM

}
