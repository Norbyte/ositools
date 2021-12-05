#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_NS(lua)

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

struct CustomLevelMap : public LevelMap
{
	RegistryEntry Function;
	LevelMap * OriginalLevelMap{ nullptr };

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

END_NS()

BEGIN_NS(lua::stats)

using namespace dse::stats;

ObjectSet<FixedString> FetchSkillSetEntries(RPGStats * stats)
{
	ObjectSet<FixedString> names;
	for (auto skillSet : stats->SkillSetManager->Primitives) {
		names.Add(skillSet->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchItemComboEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto itemCombo : stats->ItemCombinationManager->Primitives) {
		names.Add(itemCombo->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchItemComboPropertyEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& combo : stats->ItemCombinationManager->ComboProperties) {
		names.Add(combo.Key);
	}

	return names;
}

ObjectSet<FixedString> FetchItemComboPreviewDataEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& preview : stats->ItemCombinationManager->PreviewData) {
		names.Add(preview.Key);
	}

	return names;
}

ObjectSet<FixedString> FetchItemGroupEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& group : stats->ItemProgressionManager->ItemGroups) {
		names.Add(group.Key);
	}

	return names;
}

ObjectSet<FixedString> FetchItemNameGroupEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& group : stats->ItemProgressionManager->NameGroups) {
		names.Add(group.Key);
	}

	return names;
}

void FetchDeltaModEntries(lua_State* L, RPGStats* stats)
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

ObjectSet<FixedString> FetchEquipmentSetEntries(RPGStats * stats)
{
	ObjectSet<FixedString> names;
	for (auto equipmentSet : stats->EquipmentSetManager->Primitives) {
		names.Add(equipmentSet->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchTreasureTableEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto treasureTable : stats->TreasureTables.Primitives) {
		names.Add(treasureTable->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchTreasureCategoryEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto treasureCategory : stats->TreasureCategories.Primitives) {
		names.Add(treasureCategory->Category);
	}

	return names;
}

ObjectSet<FixedString> FetchStatEntries(RPGStats * stats, FixedString const& statType)
{
	ModifierList* modifierList{ nullptr };
	if (statType) {
		modifierList = stats->ModifierLists.Find(statType);
		if (modifierList == nullptr) {
			OsiError("Unknown stats entry type: " << statType);
			return {};
		}
	}

	ObjectSet<FixedString> names;
	for (auto object : stats->Objects.Primitives) {
		if (statType) {
			auto type = stats->GetTypeInfo(object);
			if (modifierList != nullptr && type != modifierList) {
				continue;
			}
		}

		names.Add(object->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchStatEntriesBefore(RPGStats* stats, FixedString const& modId, std::optional<FixedString> statType)
{
	ModifierList* modifierList{ nullptr };
	if (statType) {
		modifierList = stats->ModifierLists.Find(*statType);
		if (modifierList == nullptr) {
			OsiError("Unknown stats entry type: " << *statType);
			return {};
		}
	}

	auto entries = gExtender->GetStatLoadOrderHelper().GetStatsLoadedBefore(modId);

	ObjectSet<FixedString> names;
	for (auto object : entries) {
		if (statType) {
			auto type = stats->GetTypeInfo(object);
			if (modifierList != nullptr && type != modifierList) {
				continue;
			}
		}

		names.Add(object->Name);
	}

	return names;
}

UserReturn GetStats(lua_State * L)
{
	StackCheck _(L, 1);
	FixedString statType;
	if (lua_gettop(L) >= 1 && !lua_isnil(L, 1)) {
		statType = get<FixedString>(L, 1);
	}
		
	auto stats = GetStaticSymbols().GetStats();
	if (stats == nullptr) {
		OsiError("RPGStats not available");
		push(L, nullptr);
		return 1;
	}

	if (statType == GFS.strDeltaMod) {
		FetchDeltaModEntries(L, stats);
	} else {
		ObjectSet<FixedString> names;
		if (statType == GFS.strSkillSet) {
			names = FetchSkillSetEntries(stats);
		} else if (statType == GFS.strEquipmentSet) {
			names = FetchEquipmentSetEntries(stats);
		} else if (statType == GFS.strTreasureTable) {
			names = FetchTreasureTableEntries(stats);
		} else if (statType == GFS.strTreasureCategory) {
			names = FetchTreasureCategoryEntries(stats);
		} else if (statType == GFS.strItemCombination) {
			names = FetchItemComboEntries(stats);
		} else if (statType == GFS.strItemComboProperty) {
			names = FetchItemComboPropertyEntries(stats);
		} else if (statType == GFS.strCraftingPreviewData) {
			names = FetchItemComboPreviewDataEntries(stats);
		} else if (statType == GFS.strItemGroup) {
			names = FetchItemGroupEntries(stats);
		} else if (statType == GFS.strNameGroup) {
			names = FetchItemNameGroupEntries(stats);
		} else {
			names = FetchStatEntries(stats, statType);
		}

		LuaWrite(L, names);
	}

	return 1;
}

ObjectSet<FixedString> GetStatsLoadedBefore(FixedString const& modId, std::optional<FixedString> statType)
{
	auto stats = GetStaticSymbols().GetStats();
	if (stats == nullptr) {
		OsiError("RPGStats not available");
		return {};
	}

	return FetchStatEntriesBefore(stats, modId, statType);
}

ByValReturn<SkillSet> GetSkillSet(char const* skillSetName)
{
	auto stats = GetStaticSymbols().GetStats();
	return stats->SkillSetManager->Find(skillSetName);
}

void UpdateSkillSet(lua_State* L)
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
}

UserReturn GetEquipmentSet(lua_State * L)
{
	auto equipmentSetName = luaL_checkstring(L, 1);
	auto stats = GetStaticSymbols().GetStats();
	auto equipmentSet = stats->EquipmentSetManager->Find(equipmentSetName);
	return LuaWrite(L, equipmentSet);
}

void UpdateEquipmentSet(lua_State* L)
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
}

UserReturn GetTreasureTable(lua_State* L)
{
	auto tableName = luaL_checkstring(L, 1);
	auto stats = GetStaticSymbols().GetStats();
	auto table = stats->TreasureTables.Find(tableName);
	return LuaWrite(L, table);
}

void UpdateTreasureTable(lua_State* L)
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
}

UserReturn GetTreasureCategory(lua_State* L)
{
	auto categoryName = luaL_checkstring(L, 1);
	auto const* stats = GetStaticSymbols().GetStats();
	auto category = stats->TreasureCategories.Find(categoryName);
	return LuaWrite(L, category);
}

void UpdateTreasureCategory(lua_State* L)
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
}

UserReturn GetItemCombo(lua_State* L)
{
	auto comboName = luaL_checkstring(L, 1);
	auto combo = GetStaticSymbols().GetStats()->ItemCombinationManager->Find(comboName);
	return LuaWrite(L, combo);
}

void UpdateItemCombo(lua_State* L)
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
}

UserReturn GetItemComboPreviewData(lua_State* L)
{
	FixedString comboName(luaL_checkstring(L, 1));
	auto preview = GetStaticSymbols().GetStats()->ItemCombinationManager->PreviewData.Find(comboName);
	return LuaWrite(L, preview);
}

void UpdateItemComboPreviewData(lua_State* L)
{
	StackCheck _(L);
	luaL_checktype(L, 1, LUA_TTABLE);
	auto name = checked_getfield<FixedString>(L, "Name", 1);

	auto stats = GetStaticSymbols().GetStats();
	auto existing = stats->ItemCombinationManager->PreviewData.Find(name);
	ItemCombinationPreviewData* previewData = existing ? *existing : nullptr;
	bool isNew = (previewData == nullptr);

	lua_pushvalue(L, 1);
	LuaRead(L, previewData);
	lua_pop(L, 1);

	if (isNew) {
		stats->ItemCombinationManager->PreviewData.Insert(name, previewData);
	}
}

UserReturn GetItemComboProperty(lua_State* L)
{
	FixedString propertyName(luaL_checkstring(L, 1));
	auto prop = GetStaticSymbols().GetStats()->ItemCombinationManager->ComboProperties.Find(propertyName);
	return LuaWrite(L, prop);
}

void UpdateItemComboProperty(lua_State* L)
{
	StackCheck _(L);
	luaL_checktype(L, 1, LUA_TTABLE);
	auto name = checked_getfield<FixedString>(L, "Name", 1);

	auto stats = GetStaticSymbols().GetStats();
	auto existing = stats->ItemCombinationManager->ComboProperties.Find(name);
	ItemCombinationProperty* comboProperty = existing ? *existing : nullptr;
	bool isNew = (comboProperty == nullptr);

	lua_pushvalue(L, 1);
	LuaRead(L, comboProperty);
	lua_pop(L, 1);

	if (isNew) {
		stats->ItemCombinationManager->ComboProperties.Insert(name, comboProperty);
	}
}

UserReturn GetItemGroup(lua_State* L)
{
	FixedString name(luaL_checkstring(L, 1));
	auto group = GetStaticSymbols().GetStats()->ItemProgressionManager->ItemGroups.Find(name);
	return LuaWrite(L, group);
}


UserReturn GetNameGroup(lua_State* L)
{
	FixedString name(luaL_checkstring(L, 1));
	auto nameGroup = GetStaticSymbols().GetStats()->ItemProgressionManager->NameGroups.Find(name);
	return LuaWrite(L, nameGroup);
}

UserReturn StatGetAttribute(lua_State * L)
{
	WarnDeprecated56("StatGetAttribute() is deprecated; set properties directly on the stats object instead!");

	auto statName = get<char const*>(L, 1);
	auto attributeName = get<FixedString>(L, 2);

	auto object = StatFindObject(statName);
	if (!object) {
		push(L, nullptr);
		return 1;
	}

	return object->LuaGetAttributeLegacy(L, attributeName, {});
}

bool StatSetAttribute(lua_State * L)
{
	WarnDeprecated56("StatSetAttribute() is deprecated; set properties directly on the stats object instead!");

	auto statName = get<char const*>(L, 1);
	auto attributeName = get<FixedString>(L, 2);

	auto object = StatFindObject(statName);
	if (!object) return 0;

	return object->LuaSetAttribute(L, attributeName, 3);
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

	auto object = StatFindObject(statName);
	if (!object) return;

	auto props = object->PropertyLists.Find(FixedString(attributeName));
	if (props == nullptr || *props == nullptr) {
		OsiError("Stat object '" << object->Name << "' has no property list named '" << attributeName << "'");
		return;
	}

	// FIXME - assign name + add to map properly
	auto customProp = GameAlloc<PropertyCustomDescription>();
	customProp->Context = (PropertyContext)0;
	customProp->TypeId = PropertyType::CustomDescription;
	customProp->Conditions = nullptr;
	customProp->TextLine1 = FromUTF8(description);
	(*props)->Properties.Primitives.Add(customProp);
}

void StatSetLevelScaling(lua_State * L)
{
	StackCheck _(L);
	auto modifierListName = luaL_checkstring(L, 1);
	auto modifierName = luaL_checkstring(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);

	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::ScopeModulePreLoad) {
		luaL_error(L, "Stat functions unavailable during module preload");
	}

	if (!(lua->RestrictionFlags & (State::ScopeModuleLoad | State::ScopeModuleResume))) {
		luaL_error(L, "StatSetLevelScaling() can only be called during module load/resume");
	}

	auto stats = GetStaticSymbols().GetStats();
	auto modifier = stats->GetModifierInfo(FixedString(modifierListName), FixedString(modifierName));
	if (modifier == nullptr) {
		OsiError("Modifier list '" << modifierListName << "' or modifier '" << modifierName << "' does not exist!");
		return;
	}

	if (modifier->LevelMapIndex == -1) {
		OsiError("Modifier list '" << modifierListName << "', modifier '" << modifierName << "' is not level scaled!");
		return;
	}

	LevelMap * originalLevelMap;
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
}

UserReturn GetStat(lua_State * L)
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
		
	auto object = StatFindObject(statName, warnOnError);
	if (object != nullptr) {
		ObjectProxy2::MakeImpl<StatsEntryProxyRefImpl, Object>(L, object, GetCurrentLifetime(), level, true);
		return 1;
	} else {
		push(L, nullptr);
		return 1;
	}
}

bool CopyStats(Object* obj, char const* copyFrom)
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

UserReturn CreateStat(lua_State * L)
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

	ObjectProxy2::MakeImpl<StatsEntryProxyRefImpl, Object>(L, *object, GetCurrentLifetime(), -1, true);
	return 1;
}

void SyncStat(char const* statName, std::optional<bool> persist)
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


void StatSetPersistence(char const* statName, bool persist)
{
	if (!gExtender->GetServer().IsInServerThread()) {
		OsiError("Can only set persistence in server context");
		return;
	}

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


UserReturn GetDeltaMod(lua_State* L)
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

void UpdateDeltaMod(lua_State* L)
{
	StackCheck _(L, 0);
	luaL_checktype(L, 1, LUA_TTABLE);
	auto name = checked_getfield<char const*>(L, "Name", 1);
	auto modifierType = checked_getfield<char const*>(L, "ModifierType", 1);

	auto stats = GetStaticSymbols().GetStats();
	if (stats == nullptr) {
		OsiError("RPGStats not available");
		return;
	}

	auto deltaModType = stats->DeltaMods.Find(modifierType);
	if (deltaModType == nullptr) {
		OsiError("Unknown DeltaMod ModifierType: " << modifierType);
		return;
	}

	auto deltaMod = deltaModType->Find(name);
	bool isNew = (deltaMod == nullptr);

	lua_pushvalue(L, 1);
	LuaRead(L, deltaMod);
	lua_pop(L, 1);

	if (isNew) {
		deltaModType->Add(FixedString(name), deltaMod);
	}
}

template <class T>
int TypedEnumIndexToLabel(lua_State* L, T index)
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
int TypedEnumLabelToIndex(lua_State* L, char const* label)
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
	return TypedEnumIndexToLabel<NS::T>(L, (NS::T)index); \
}
#define BEGIN_ENUM(T, type) \
if (strcmp(enumName.Str, #T) == 0) { \
	return TypedEnumIndexToLabel<T>(L, (T)index); \
}

UserReturn EnumIndexToLabel(lua_State* L)
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
	return TypedEnumLabelToIndex<NS::T>(L, label); \
}
#define BEGIN_ENUM(T, type) \
if (strcmp(enumName.Str, #T) == 0) { \
	return TypedEnumLabelToIndex<T>(L, label); \
}


UserReturn EnumLabelToIndex(lua_State* L)
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

void RegisterStatsLib(lua_State* L)
{
	StatsExtraDataProxy::RegisterMetatable(L);

	static const luaL_Reg lib[] = {
		{"GetStats", LuaWrapFunction(&GetStats)},
		{"GetStatsLoadedBefore", LuaWrapFunction(&GetStatsLoadedBefore)},
		{"Get", LuaWrapFunction(&GetStat)},
		{"Create", LuaWrapFunction(&CreateStat)},
		{"Sync", LuaWrapFunction(&SyncStat)},
		{"SetPersistence", LuaWrapFunction(&StatSetPersistence)},
		{"GetAttribute", LuaWrapFunction(&StatGetAttribute)},
		{"SetAttribute", LuaWrapFunction(&StatSetAttribute)},
		{"AddCustomDescriptionWrapper", LuaWrapFunction(&StatAddCustomDescription)},
		{"SetLevelScaling", LuaWrapFunction(&StatSetLevelScaling)},
		{"EnumIndexToLabel", LuaWrapFunction(&EnumIndexToLabel)},
		{"EnumLabelToIndex", LuaWrapFunction(&EnumLabelToIndex)},
		{0,0}
	};

	RegisterLib(L, "Stats", lib);

	static const luaL_Reg deltaModLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetDeltaMod)},
		{"Update", LuaWrapFunction(&UpdateDeltaMod)},
		{0,0}
	};

	RegisterLib(L, "Stats", "DeltaMod", deltaModLib);

	static const luaL_Reg skillSetLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetSkillSet)},
		{"Update", LuaWrapFunction(&UpdateSkillSet)},
		{0,0}
	};

	RegisterLib(L, "Stats", "SkillSet", skillSetLib);

	static const luaL_Reg equipmentSetLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetEquipmentSet)},
		{"Update", LuaWrapFunction(&UpdateEquipmentSet)},
		{0,0}
	};

	RegisterLib(L, "Stats", "EquipmentSet", equipmentSetLib);

	static const luaL_Reg treasureTableLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetTreasureTable)},
		{"Update", LuaWrapFunction(&UpdateTreasureTable)},
		{0,0}
	};

	RegisterLib(L, "Stats", "TreasureTable", treasureTableLib);

	static const luaL_Reg treasureCategoryLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetTreasureCategory)},
		{"Update", LuaWrapFunction(&UpdateTreasureCategory)},
		{0,0}
	};

	RegisterLib(L, "Stats", "TreasureCategory", treasureCategoryLib);

	static const luaL_Reg itemComboLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetItemCombo)},
		{"Update", LuaWrapFunction(&UpdateItemCombo)},
		{0,0}
	};

	RegisterLib(L, "Stats", "ItemCombo", itemComboLib);

	static const luaL_Reg itemComboPreviewLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetItemComboPreviewData)},
		{"Update", LuaWrapFunction(&UpdateItemComboPreviewData)},
		{0,0}
	};

	RegisterLib(L, "Stats", "ItemComboPreview", itemComboPreviewLib);

	static const luaL_Reg itemComboPropertyLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetItemComboProperty)},
		{"Update", LuaWrapFunction(&UpdateItemComboProperty)},
		{0,0}
	};

	RegisterLib(L, "Stats", "ItemComboProperty", itemComboPropertyLib);

	static const luaL_Reg itemGroupLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetItemGroup)},
		{0,0}
	};

	RegisterLib(L, "Stats", "ItemGroup", itemGroupLib);

	static const luaL_Reg nameGroupLib[] = {
		{"GetLegacy", LuaWrapFunction(&GetNameGroup)},
		{0,0}
	};

	RegisterLib(L, "Stats", "NameGroup", nameGroupLib);
}

END_NS()
