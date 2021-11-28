#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

namespace dse::lua
{
	StatsEntryProxyRefImpl::StatsEntryProxyRefImpl(LifetimeHolder const& lifetime, stats::Object* obj, std::optional<int> level, bool legacy)
		: ObjectProxyRefImpl(lifetime, obj),
		level_(level), legacy_(legacy)
	{}

	FixedString const& StatsEntryProxyRefImpl::GetTypeName() const
	{
		return GFS.strStatsEntry;
	}
		
	bool StatsEntryProxyRefImpl::GetProperty(lua_State* L, FixedString const& prop)
	{
		if (legacy_) {
			return Get()->LuaGetAttributeLegacy(L, prop, level_) == 1;
		} else {
			return Get()->LuaGetAttribute(L, prop, level_) == 1;
		}
	}

	bool StatsEntryProxyRefImpl::SetProperty(lua_State* L, FixedString const& prop, int index)
	{
		return Get()->LuaSetAttribute(L, prop, index);
	}

	int StatsEntryProxyRefImpl::Next(lua_State* L, FixedString const& key)
	{
		auto const& attrs = Get()->GetModifierList()->Attributes;
		if (!key) {
			if (attrs.Primitives.Size > 0) {
				auto const& prop = attrs.Primitives[0]->Name;
				push(L, prop);
				Get()->LuaGetAttribute(L, prop, level_);
				return 2;
			}
		} else {
			auto curKey = attrs.FindIndex(key);
			if (curKey && attrs.Primitives.Size > (unsigned)*curKey + 1) {
				auto const& prop = attrs.Primitives[*curKey + 1]->Name;
				push(L, prop);
				Get()->LuaGetAttribute(L, prop, level_);
				return 2;
			}
		}
			
		return 0;
	}


	char const* const StatsExtraDataProxy::MetatableName = "ExtraData";

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
		auto value = get<float>(L, 3);
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
		StatsExtraDataProxy::RegisterMetatable(L);
	}
	
	
	void FetchSkillSetEntries(lua_State * L, stats::RPGStats * stats)
	{
		int32_t index = 1;
		for (auto skillSet : stats->SkillSetManager->Primitives) {
			settable(L, index++, skillSet->Name);
		}
	}

	void FetchItemComboEntries(lua_State* L, stats::RPGStats* stats)
	{
		int32_t index = 1;
		for (auto itemCombo : stats->ItemCombinationManager->Primitives) {
			settable(L, index++, itemCombo->Name);
		}
	}

	void FetchItemComboPropertyEntries(lua_State* L, stats::RPGStats* stats)
	{
		int32_t index = 1;
		for (auto const& combo : stats->ItemCombinationManager->ComboProperties) {
			settable(L, index++, combo.Key);
		}
	}

	void FetchItemComboPreviewDataEntries(lua_State* L, stats::RPGStats* stats)
	{
		int32_t index = 1;
		for (auto const& preview : stats->ItemCombinationManager->PreviewData) {
			settable(L, index++, preview.Key);
		}
	}

	void FetchItemGroupEntries(lua_State* L, stats::RPGStats* stats)
	{
		int32_t index = 1;
		for (auto const& group : stats->ItemProgressionManager->ItemGroups) {
			settable(L, index++, group.Key);
		}
	}

	void FetchItemNameGroupEntries(lua_State* L, stats::RPGStats* stats)
	{
		int32_t index = 1;
		for (auto const& group : stats->ItemProgressionManager->NameGroups) {
			settable(L, index++, group.Key);
		}
	}

	void FetchDeltaModEntries(lua_State* L, stats::RPGStats* stats)
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

	void FetchEquipmentSetEntries(lua_State * L, stats::RPGStats * stats)
	{
		int32_t index = 1;
		for (auto equipmentSet : stats->EquipmentSetManager->Primitives) {
			settable(L, index++, equipmentSet->Name);
		}
	}

	void FetchTreasureTableEntries(lua_State* L, stats::RPGStats* stats)
	{
		int32_t index = 1;
		for (auto treasureTable : stats->TreasureTables.Primitives) {
			settable(L, index++, treasureTable->Name);
		}
	}

	void FetchTreasureCategoryEntries(lua_State* L, stats::RPGStats* stats)
	{
		int32_t index = 1;
		for (auto treasureCategory : stats->TreasureCategories.Primitives) {
			settable(L, index++, treasureCategory->Category);
		}
	}

	void FetchStatEntries(lua_State * L, stats::RPGStats * stats, FixedString const& statType)
	{
		stats::ModifierList* modifierList{ nullptr };
		if (statType) {
			modifierList = stats->ModifierLists.Find(statType);
			if (modifierList == nullptr) {
				OsiError("Unknown stats entry type: " << statType);
				return;
			}
		}

		int32_t index = 1;
		for (auto object : stats->Objects.Primitives) {
			if (statType) {
				auto type = stats->GetTypeInfo(object);
				if (modifierList != nullptr && type != modifierList) {
					continue;
				}
			}

			settable(L, index++, object->Name);
		}
	}

	void FetchStatEntriesBefore(lua_State* L, stats::RPGStats* stats, FixedString const& modId, FixedString const& statType)
	{
		stats::ModifierList* modifierList{ nullptr };
		if (statType) {
			modifierList = stats->ModifierLists.Find(statType);
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
			statType = get<FixedString>(L, 1);
		}
		
		lua_newtable(L);
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("RPGStats not available");
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
		auto modId = get<FixedString>(L, 1);
		FixedString statType;
		if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
			statType = get<FixedString>(L, 2);
		}

		lua_newtable(L);
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("RPGStats not available");
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
		auto name = get<FixedString>(L, 1);
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
		stats::ItemCombinationPreviewData* previewData = existing ? *existing : nullptr;
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
		stats::ItemCombinationProperty* comboProperty = existing ? *existing : nullptr;
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

	int StatGetAttribute(lua_State * L)
	{
		auto statName = get<char const*>(L, 1);
		auto attributeName = get<FixedString>(L, 2);

		auto object = stats::StatFindObject(statName);
		if (!object) {
			push(L, nullptr);
			return 1;
		}

		return object->LuaGetAttributeLegacy(L, attributeName, {});
	}

	int StatGetAttributeScaled(lua_State * L)
	{
		auto statName = get<char const*>(L, 1);
		auto attributeName = get<FixedString>(L, 2);

		auto object = stats::StatFindObject(statName);
		if (!object) {
			push(L, nullptr);
			return 1;
		}

		return object->LuaGetAttributeLegacy(L, attributeName, object->Level);
	}

	int StatSetAttribute(lua_State * L)
	{
		auto statName = get<char const*>(L, 1);
		auto attributeName = get<FixedString>(L, 2);

		auto object = stats::StatFindObject(statName);
		if (!object) return 0;

		push(L, object->LuaSetAttribute(L, attributeName, 3));
		return 1;
	}

	void StatAddCustomDescription(lua_State * L, const char* statName, const char* attributeName, const char* description)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::ScopeModulePreLoad) {
			luaL_error(L, "Stat functions unavailable during module preload");
		}

		if (!(lua->RestrictionFlags & State::ScopeModuleLoad)) {
			luaL_error(L, "StatAddCustomDescription() can only be called during module load");
		}

		auto object = stats::StatFindObject(statName);
		if (!object) return;

		auto props = object->PropertyLists.Find(FixedString(attributeName));
		if (props == nullptr || *props == nullptr) {
			OsiError("Stat object '" << object->Name << "' has no property list named '" << attributeName << "'");
			return;
		}

		// FIXME - assign name + add to map properly
		auto customProp = GameAlloc<stats::PropertyCustomDescription>();
		customProp->Context = (stats::PropertyContext)0;
		customProp->TypeId = stats::PropertyType::CustomDescription;
		customProp->Conditions = nullptr;
		customProp->TextLine1 = FromUTF8(description);
		(*props)->Properties.Primitives.Add(customProp);
	}

	WrapLuaFunction(StatAddCustomDescription)


	struct CustomLevelMap : public stats::LevelMap
	{
		RegistryEntry Function;
		stats::LevelMap * OriginalLevelMap{ nullptr };

		CustomLevelMap() {}
		~CustomLevelMap() override {}
		
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
			auto levelMap = static_cast<CustomLevelMap *>(levelMaps.Buf[levelMapIndex]);
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

		auto lua = State::FromLua(L);
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

		stats::LevelMap * originalLevelMap;
		auto currentLevelMap = stats->LevelMaps.Find(modifier->LevelMapIndex);
		
		auto it = lua->OverriddenLevelMaps.find(modifier->LevelMapIndex);
		if (it != lua->OverriddenLevelMaps.end()) {
			auto overridden = static_cast<CustomLevelMap *>(currentLevelMap);
			originalLevelMap = overridden->OriginalLevelMap;
		} else {
			originalLevelMap = currentLevelMap;
		}

		auto levelMap = GameAlloc<CustomLevelMap>();
		levelMap->ModifierListIndex = originalLevelMap->ModifierListIndex;
		levelMap->ModifierIndex = originalLevelMap->ModifierIndex;
		levelMap->ValueListIndex = originalLevelMap->ValueListIndex;
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
		bool warnOnError{ false };
		if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
			level = (int32_t)luaL_checkinteger(L, 2);
		}
		if (lua_gettop(L) >= 3) {
			warnOnError = get<bool>(L, 3);
		}
		
		auto object = stats::StatFindObject(statName, warnOnError);
		if (object != nullptr) {
			ObjectProxy2::MakeImpl<StatsEntryProxyRefImpl, stats::Object>(L, object, GetCurrentLifetime(), level, true);
			return 1;
		} else {
			push(L, nullptr);
			return 1;
		}
	}

	bool CopyStats(stats::Object* obj, char const* copyFrom)
	{
		auto stats = GetStaticSymbols().GetStats();
		auto copyFromObject = stats->Objects.Find(FixedString(copyFrom));
		if (copyFromObject == nullptr) {
			OsiError("Cannot copy stats from nonexistent object: " << copyFrom);
			return false;
		}

		if (obj->ModifierListIndex != copyFromObject->ModifierListIndex) {
			auto objModifier = stats->ModifierLists.Find(obj->ModifierListIndex);
			auto copyModifier = stats->ModifierLists.Find(obj->ModifierListIndex);
			OsiError("Cannot copy stats from object '" << copyFrom << "' (a " << copyModifier->Name.Str 
				<< ") to an object of type " << objModifier->Name.Str);
			return false;
		}

		obj->Level = copyFromObject->Level;
		obj->AIFlags = copyFromObject->AIFlags;

		for (size_t i = 0; i < obj->IndexedProperties.size(); i++) {
			obj->IndexedProperties[i] = copyFromObject->IndexedProperties[i];
		}

		for (auto const& prop : copyFromObject->PropertyLists) {
			// TODO - is reusing property list objects allowed?
			obj->PropertyLists.Insert(prop.Key, prop.Value);
		}

		for (auto const& cond : copyFromObject->Conditions) {
			// TODO - is reusing condition objects allowed?
			obj->Conditions.Insert(cond.Key, cond.Value);
		}

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

		auto lua = State::FromLua(L);
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

		ObjectProxy2::MakeImpl<StatsEntryProxyRefImpl, stats::Object>(L, *object, GetCurrentLifetime(), -1, true);
		return 1;
	}

	void SyncStat(lua_State* L, char const* statName, std::optional<bool> persist)
	{
		auto stats = GetStaticSymbols().GetStats();
		auto object = stats->Objects.Find(FixedString(statName));
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
		auto object = stats->Objects.Find(FixedString(statName));
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
		auto name = get<char const *>(L, 1);
		auto modifierType = get<char const*>(L, 2);

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("RPGStats not available");
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
			OsiError("RPGStats not available");
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
		auto enumName = get<FixedString>(L, 1);
		auto index = get<int>(L, 2);

#include <GameDefinitions/Enumerations.inl>

		auto valueList = GetStaticSymbols().GetStats()->ModifierValueLists.Find(enumName);
		if (valueList) {
			std::optional<FixedString> value;
			for (auto const& val : valueList->Values) {
				if (val.Value == index) {
					value = val.Key;
				}
			}

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
		auto enumName = get<FixedString>(L, 1);
		auto label = get<char const*>(L, 2);

#include <GameDefinitions/Enumerations.inl>

		auto valueList = GetStaticSymbols().GetStats()->ModifierValueLists.Find(enumName);
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
