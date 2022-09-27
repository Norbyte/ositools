#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_NS(lua)

StatsEntryProxyRefImpl::StatsEntryProxyRefImpl(LifetimeHandle const& lifetime, stats::Object* obj, std::optional<int> level, bool legacy)
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
	auto modifierList = Get()->GetModifierList();
	if (modifierList) {
		auto const& attrs = modifierList->Attributes;
		if (!key) {
			if (attrs.Primitives.size() > 0) {
				auto const& prop = attrs.Primitives[0]->Name;
				push(L, prop);
				Get()->LuaGetAttribute(L, prop, level_);
				return 2;
			}
		} else {
			auto curKey = attrs.FindIndex(key);
			if (curKey && attrs.Primitives.size() > (unsigned)*curKey + 1) {
				auto const& prop = attrs.Primitives[*curKey + 1]->Name;
				push(L, prop);
				Get()->LuaGetAttribute(L, prop, level_);
				return 2;
			}
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
	auto extraData = stats->ExtraData->Properties.find(FixedString(key));
	if (extraData) {
		push(L, extraData.Value());
	} else {
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
	auto extraData = stats->ExtraData->Properties.find(FixedString(key));
	if (extraData) {
		extraData.Value() = value;
	} else {
		LuaError("Cannot set nonexistent ExtraData value '" << key << "'");
	}

	return 0;
}

struct CustomLevelMap : public LevelMap
{
	RegistryEntry ClientFunction;
	RegistryEntry ServerFunction;
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
		auto state = gExtender->GetCurrentExtensionState();
		if (!state || !state->GetLua()) return {};

		LuaVirtualPin lua(state);
		auto L = lua->GetState();
		StackCheck _(L);

		if (lua->IsClient()) {
			if (!ClientFunction) return {};
			ClientFunction.Push();
		} else {
			if (!ServerFunction) return {};
			ServerFunction.Push();
		}

		push(L, attributeValue);
		push(L, level);

		Restriction restriction(*lua, State::RestrictAll);
		if (CallWithTraceback(L, 2, 1) != 0) { // stack: retval
			OsiError("Level scaled value fetch failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return {};
		}

		if (lua_type(L, -1) != LUA_TNUMBER) {
			OsiErrorS("Level scaled fetcher returned non-numeric value");
			// lua_pop(L, 1); // stack: -
			return {};
		}

		auto value = lua_tointeger(L, -1);
		lua_pop(L, 1); // stack: -
		return value;
	}
};

CustomLevelMap gDummyCustomLevelMap;

void RestoreLevelMaps(bool isClient)
{
	auto & levelMaps = GetStaticSymbols().GetStats()->LevelMaps.Primitives;
	for (auto& levelMap : levelMaps) {
		if (*(void**)levelMap == *(void**)&gDummyCustomLevelMap) {
			auto customMap = static_cast<CustomLevelMap*>(levelMap);
			if (isClient) {
				customMap->ClientFunction = RegistryEntry{};
			} else {
				customMap->ServerFunction = RegistryEntry{};
			}

			if (!customMap->ClientFunction && !customMap->ServerFunction) {
				levelMap = customMap->OriginalLevelMap;
			}
		}
	}
}

END_NS()

/// <lua_module>Stats</lua_module>
BEGIN_NS(lua::stats)

using namespace dse::stats;

character_creation::CharacterCreationManager* GetCharacterCreation()
{
	return *GetStaticSymbols().eoc__CharacterCreationManager;
}

ObjectSet<FixedString> FetchSkillSetEntries(RPGStats * stats)
{
	ObjectSet<FixedString> names;
	for (auto skillSet : stats->SkillSetManager->Primitives) {
		names.push_back(skillSet->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchItemComboEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto itemCombo : stats->ItemCombinationManager->Primitives) {
		names.push_back(itemCombo->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchItemComboPropertyEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& combo : stats->ItemCombinationManager->ComboProperties) {
		names.push_back(combo.Key);
	}

	return names;
}

ObjectSet<FixedString> FetchItemComboPreviewDataEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& preview : stats->ItemCombinationManager->PreviewData) {
		names.push_back(preview.Key);
	}

	return names;
}

ObjectSet<FixedString> FetchItemGroupEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& group : stats->ItemProgressionManager->ItemGroups) {
		names.push_back(group.Key);
	}

	return names;
}

ObjectSet<FixedString> FetchItemNameGroupEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto const& group : stats->ItemProgressionManager->NameGroups) {
		names.push_back(group.Key);
	}

	return names;
}

void FetchDeltaModEntries(lua_State* L, RPGStats* stats)
{
	int32_t index = 1;
	lua_newtable(L);
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
		names.push_back(equipmentSet->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchTreasureTableEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto treasureTable : stats->TreasureTables.Primitives) {
		names.push_back(treasureTable->Name);
	}

	return names;
}

ObjectSet<FixedString> FetchTreasureCategoryEntries(RPGStats* stats)
{
	ObjectSet<FixedString> names;
	for (auto treasureCategory : stats->TreasureCategories.Primitives) {
		names.push_back(treasureCategory->Category);
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

		names.push_back(object->Name);
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

		names.push_back(object->Name);
	}

	return names;
}

/// <summary>
/// Returns a table with the names of all stat entries. 
/// When the optional parameter `statType` is specified, it'll only return stats with the specified type. (The type of a stat entry is specified in the stat .txt file itself (eg. `type "StatusData"`).
/// The following types are supported:  `StatusData`, `SkillData`, `Armor`, `Shield`, `Weapon`, `Potion`, `Character`, `Object`, `SkillSet`, `EquipmentSet`, `TreasureTable`, `ItemCombination`, `ItemComboProperty`, `CraftingPreviewData`, `ItemGroup`, `NameGroup`, `DeltaMod`
/// </summary>
/// <lua_export>GetStats</lua_export>
/// <lua_legacy>Ext.GetStatEntries</lua_legacy>
/// <param name="statType">Type of stat to fetch</param>
/// <returns></returns>
UserReturn GetStats(lua_State * L, std::optional<FixedString> statType)
{
	auto stats = GetStaticSymbols().GetStats();
	if (stats == nullptr) {
		OsiError("RPGStats not available");
		push(L, nullptr);
		return 1;
	}

	if (statType && *statType == GFS.strDeltaMod) {
		FetchDeltaModEntries(L, stats);
	} else {
		ObjectSet<FixedString> names;
		if (!statType) {
			names = FetchStatEntries(stats, FixedString{});
		} else if (*statType == GFS.strSkillSet) {
			names = FetchSkillSetEntries(stats);
		} else if (*statType == GFS.strEquipmentSet) {
			names = FetchEquipmentSetEntries(stats);
		} else if (*statType == GFS.strTreasureTable) {
			names = FetchTreasureTableEntries(stats);
		} else if (*statType == GFS.strTreasureCategory) {
			names = FetchTreasureCategoryEntries(stats);
		} else if (*statType == GFS.strItemCombination) {
			names = FetchItemComboEntries(stats);
		} else if (*statType == GFS.strItemComboProperty) {
			names = FetchItemComboPropertyEntries(stats);
		} else if (*statType == GFS.strCraftingPreviewData) {
			names = FetchItemComboPreviewDataEntries(stats);
		} else if (*statType == GFS.strItemGroup) {
			names = FetchItemGroupEntries(stats);
		} else if (*statType == GFS.strNameGroup) {
			names = FetchItemNameGroupEntries(stats);
		} else {
			names = FetchStatEntries(stats, *statType);
		}

		LuaWrite(L, names);
	}

	return 1;
}

/// <summary>
/// Returns a table with the names of all stat entries that were loaded before the specified mod.
/// This function is useful for retrieving stats that can be overridden by a mod according to the module load order.
/// When the optional parameter `statType` is specified, it'll only return stats with the specified type. (The type of a stat entry is specified in the stat .txt file itself (eg. `type "StatusData"`).
/// </summary>
/// <lua_export>GetStatsLoadedBefore</lua_export>
/// <lua_legacy>Ext.GetStatEntriesLoadedBefore</lua_legacy>
/// <param name="modUuid">Return stats entries declared before this module was loaded</param>
/// <param name="statType">Type of stat to fetch</param>
/// <returns></returns>
ObjectSet<FixedString> GetStatsLoadedBefore(FixedString const& modUuid, std::optional<FixedString> statType)
{
	auto stats = GetStaticSymbols().GetStats();
	if (stats == nullptr) {
		OsiError("RPGStats not available");
		return {};
	}

	return FetchStatEntriesBefore(stats, modUuid, statType);
}

ByValReturn<SkillSet> GetSkillSet(char const* skillSetName)
{
	auto stats = GetStaticSymbols().GetStats();
	return stats->SkillSetManager->Find(skillSetName);
}

void UpdateSkillSet(lua_State* L)
{
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

UserReturn GetEquipmentSet(lua_State * L, FixedString const& equipmentSetName)
{
	auto stats = GetStaticSymbols().GetStats();
	auto equipmentSet = stats->EquipmentSetManager->Find(equipmentSetName);
	return LuaWrite(L, equipmentSet);
}

void UpdateEquipmentSet(lua_State* L)
{
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

UserReturn GetTreasureTableLegacy(lua_State* L, FixedString const& tableName)
{
	auto stats = GetStaticSymbols().GetStats();
	auto table = stats->TreasureTables.Find(tableName);
	return LuaWrite(L, table);
}

stats::TreasureTable* GetTreasureTable(FixedString const& tableName)
{
	auto stats = GetStaticSymbols().GetStats();
	return stats->TreasureTables.Find(tableName);
}

void UpdateTreasureTable(lua_State* L)
{
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

UserReturn GetTreasureCategory(lua_State* L, FixedString const& categoryName)
{
	auto const* stats = GetStaticSymbols().GetStats();
	auto category = stats->TreasureCategories.Find(categoryName);
	return LuaWrite(L, category);
}

void UpdateTreasureCategory(lua_State* L, FixedString const& name)
{
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

UserReturn GetItemCombo(lua_State* L, FixedString const& comboName)
{
	auto combo = GetStaticSymbols().GetStats()->ItemCombinationManager->Find(comboName);
	return LuaWrite(L, combo);
}

void UpdateItemCombo(lua_State* L)
{
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

UserReturn GetItemComboPreviewData(lua_State* L, FixedString const& comboName)
{
	auto preview = GetStaticSymbols().GetStats()->ItemCombinationManager->PreviewData.try_get(comboName);
	return LuaWrite(L, preview);
}

void UpdateItemComboPreviewData(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	auto name = checked_getfield<FixedString>(L, "Name", 1);

	auto stats = GetStaticSymbols().GetStats();
	auto existing = stats->ItemCombinationManager->PreviewData.try_get(name);
	ItemCombinationPreviewData* previewData = existing ? existing : nullptr;
	bool isNew = (previewData == nullptr);

	lua_pushvalue(L, 1);
	LuaRead(L, previewData);
	lua_pop(L, 1);

	if (isNew) {
		stats->ItemCombinationManager->PreviewData.insert(name, previewData);
	}
}

UserReturn GetItemComboProperty(lua_State* L, FixedString const& propertyName)
{
	auto prop = GetStaticSymbols().GetStats()->ItemCombinationManager->ComboProperties.try_get(propertyName);
	return LuaWrite(L, prop);
}

void UpdateItemComboProperty(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	auto name = checked_getfield<FixedString>(L, "Name", 1);

	auto stats = GetStaticSymbols().GetStats();
	auto existing = stats->ItemCombinationManager->ComboProperties.try_get(name);
	ItemCombinationProperty* comboProperty = existing ? existing : nullptr;
	bool isNew = (comboProperty == nullptr);

	lua_pushvalue(L, 1);
	LuaRead(L, comboProperty);
	lua_pop(L, 1);

	if (isNew) {
		stats->ItemCombinationManager->ComboProperties.insert(name, comboProperty);
	}
}

UserReturn GetItemGroup(lua_State* L, FixedString const& name)
{
	auto group = GetStaticSymbols().GetStats()->ItemProgressionManager->ItemGroups.try_get(name);
	return LuaWrite(L, group);
}


UserReturn GetNameGroup(lua_State* L, FixedString const& name)
{
	auto nameGroup = GetStaticSymbols().GetStats()->ItemProgressionManager->NameGroups.try_get(name);
	return LuaWrite(L, nameGroup);
}

UserReturn GetItemSet(lua_State* L, FixedString const& itemSetName)
{
	auto itemSet = GetStaticSymbols().GetStats()->ItemSetsManager->Find(itemSetName);
	return LuaWrite(L, itemSet);
}

void UpdateItemSet(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	auto name = checked_getfield<FixedString>(L, "Name", 1);

	auto stats = GetStaticSymbols().GetStats();
	auto itemSet = stats->ItemSetsManager->Find(name);
	bool isNew = (itemSet == nullptr);

	lua_pushvalue(L, 1);
	LuaRead(L, itemSet);
	lua_pop(L, 1);

	if (isNew) {
		stats->ItemSetsManager->Add(name, itemSet);
	}
}

RefMap<FixedString, ItemColorDefinition>* GetAllItemColors()
{
	return &GetStaticSymbols().GetStats()->Colors;
}

ItemColorDefinition* GetItemColor(lua_State* L, FixedString const& colorName)
{
	return GetStaticSymbols().GetStats()->Colors.try_get_ptr(colorName);
}

void UpdateItemColor(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	auto name = checked_getfield<FixedString>(L, "Name", 1);

	auto stats = GetStaticSymbols().GetStats();
	auto itemColor = stats->Colors.try_get_ptr(name);
	bool isNew = (itemColor == nullptr);

	lua_pushvalue(L, 1);
	LuaRead(L, itemColor);
	lua_pop(L, 1);

	if (isNew) {
		stats->Colors.insert(name, *itemColor);
	}
}

/// <summary>
/// Returns the specified `attribute` of the stat entry.
/// If the stat entry does not exist, the stat entry doesn't have an attribute named `attribute`, or the attribute is not supported, the function returns `nil`.
/// 
/// ** Notes: **
/// - For enumerations, the function will return the enumeration label(eg. `Corrosive`). See `ModifierLists.txt` or `Enumerations.xml` for a list of enumerationsand enumeration labels.
/// - The following fields are not supported: `AoEConditions`, `TargetConditions`, `ForkingConditions`, `CycleConditions`
/// `Requirements` and `MemorizationRequirements` are returned in the following format :
/// ```js
/// [
///		{
/// 		"Not" : false, // Negated condition?	
///			"Param" : 1, // Parameter; number for ability/attribute level, string for Tag
///			"Requirement" : "FireSpecialist" // Requirement name
///		},
///		{
/// 		"Not" : false,
/// 		"Param" : 1,
/// 		"Requirement" : "Necromancy"
///		}
/// ]
/// ```
/// </summary>
/// <lua_export>GetAttribute</lua_export>
/// <lua_legacy>Ext.StatGetAttribute</lua_legacy>
/// <lua_deprecated>StatGetAttribute() is deprecated; read properties directly from the stats object instead!</lua_deprecated>
/// <param name="statName"></param>
/// <param name="attributeName"></param>
/// <returns></returns>
UserReturn GetAttribute(lua_State * L, char const* statName, FixedString const& attributeName)
{
	WarnDeprecated56("StatGetAttribute() is deprecated; read properties directly from the stats object instead!");

	auto object = StatFindObject(statName);
	if (!object) {
		push(L, nullptr);
		return 1;
	}

	return object->LuaGetAttributeLegacy(L, attributeName, {});
}

/// <summary>
/// Updates the specified `attribute` of the stat entry. 
/// This essentially allows on-the-fly patching of stat .txt files from script without having to override the while stat entry.
/// If the function is called while the module is loading (i.e. from a `ModuleLoading`/`StatsLoaded` listener) no additional calls are needed.
/// If the function is called after module load, the stats entry must be synchronized with the client via the `Sync` call.
/// ** Notes: **
/// - For enumerations, the function accepts both the enumeration label (a string value, eg. `Corrosive`) 
///   and the enumeration index (an integer value, eg, `7`). 
///   See `ModifierLists.txt` or `Enumerations.xml` for a list of enumerations and enumeration labels.
/// - Be aware that a lot of number-like attributes are in fact enums; eg. the `Strength`, `Finesse`, `Intelligence`, etc. attributes of `Potion` are enumerations 
///   and setting them by passing an integer value to this function will yield unexpected results.
///   For example, calling `SetAttribute("SomePotion", "Strength", 5)` will set the `Strength` value to `-9.6`! 
///   The proper way to set these values is by passing the enumeration label as string, 
///   eg. `SetAttribute("SomePotion", "Strength", "5")`
/// 
/// Example:
/// ```lua
/// -- Swap DamageType from Poison to Air on all skills
/// for i,name in pairs(Ext.Stats.GetEntries("SkillData")) do
///     local damageType = Ext.Stats.GetAttribute(name, "DamageType")
///     if damageType == "Poison" then
///         Ext.Stats.SetAttribute(name, "DamageType", "Air")
///     end
/// end
/// ```
/// 
/// When modifying stat attributes that contain tables (i.e. `Requirements`, `TargetConditions`, `SkillProperties` etc.) it is not sufficient to just modify the table, the modified table must be reassigned to the property:
/// ```lua
/// local requirements = Ext.Stats.GetAttribute(name, "MemorizationRequirements")
/// table.insert(requirements, {Name = "Intelligence", Param = 10, Not = false})
/// Ext.Stats.SetAttribute(name, "Requirements", requirements)
/// ```
/// 
/// Stat entries that are modified on the fly (i.e. after `ModuleLoading`/`StatsLoaded`) must be synchronized via `SyncStat()`. Neglecting to do this will cause the stat entry to be different on the client and the server.
/// ```lua
/// local stat = Ext.Stats.Get(name)
/// stat.DamageType = "Air"
/// stat.Damage = 10
/// Ext.Stats.Sync(name)
/// ```
/// </summary>
/// <lua_export>SetAttribute</lua_export>
/// <lua_legacy>Ext.StatSetAttribute</lua_legacy>
/// <lua_deprecated>StatSetAttribute() is deprecated; set properties directly on the stats object instead!</lua_deprecated>
/// <param name="statName"></param>
/// <param name="attributeName"></param>
/// <param name="value"></param>
bool SetAttribute(lua_State * L, char const* statName, FixedString const& attributeName)
{
	WarnDeprecated56("StatSetAttribute() is deprecated; set properties directly on the stats object instead!");

	auto object = StatFindObject(statName);
	if (!object) return 0;

	return object->LuaSetAttribute(L, attributeName, 3);
}

/// <summary>
/// Adds a custom property description to the specified stat entry. (The blue text in the skill description tooltip).
/// This function can only be called from a `ModuleLoading` listener.
/// 
/// Example:
/// ```lua
/// Ext.Stats.AddCustomDescription("Dome_CircleOfProtection", "SkillProperties", "Custom desc one")
/// ```
/// </summary>
/// <lua_export>AddCustomDescription</lua_export>
/// <lua_legacy>Ext.Stats.AddCustomDescription</lua_legacy>
/// <param name="statName">Stats name to fetch</param>
/// <param name="attributeName">Property list to expand (SkillProperties or ExtraProperties)</param>
/// <param name="description">Description text</param>
void AddCustomDescription(lua_State * L, const char* statName, const char* attributeName, const char* description)
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

	auto props = object->PropertyLists.try_get(FixedString(attributeName));
	if (props == nullptr) {
		OsiError("Stat object '" << object->Name << "' has no property list named '" << attributeName << "'");
		return;
	}

	// FIXME - assign name + add to map properly
	auto customProp = GameAlloc<PropertyCustomDescription>();
	customProp->Context = (PropertyContext)0;
	customProp->TypeId = PropertyType::CustomDescription;
	customProp->Conditions = nullptr;
	customProp->TextLine1 = FromUTF8(description);
	props->Properties.Primitives.push_back(customProp);
}

/// <summary>
/// Replaces level scaling formula for the specified stat.
/// This function can only be called from a `ModuleLoading` listener.
/// 
/// `func` must satisfy the following requirements :
///  - Must be a Lua function that receives two arguments `(attributeValue, level)`and returns the integer level scaled value.
///  - Must have no side effects(i.e.can't set external variables, call external functions, etc)
///  - Must always returns the same result when given the same argument values
///  - Since the function is called very frequently (up to 50, 000 calls during a level load), it should execute as quickly as possible
/// </summary>
/// <lua_export>SetLevelScaling</lua_export>
/// <lua_legacy>Ext.Stats.SetLevelScaling</lua_legacy>
/// <param name="modifierListName">Type of stat to override (`Character`, `SkillData`, `Potion`, etc.)</param>
/// <param name="modifierListName">Stat attribute to override (`Strength`, `Constitution`, ...)</param>
/// <param name="func">Level scaling function</param>
void SetLevelScaling(lua_State * L, FixedString const& modifierListName, FixedString const& modifierName)
{
	StackCheck _(L);
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

	CustomLevelMap* levelMap;
	auto currentLevelMap = stats->LevelMaps.Find(modifier->LevelMapIndex);

	if (*(void**)currentLevelMap == *(void**)&gDummyCustomLevelMap) {
		levelMap = static_cast<CustomLevelMap*>(currentLevelMap);
	} else {
		auto originalLevelMap = currentLevelMap;

		levelMap = GameAlloc<CustomLevelMap>();
		levelMap->ModifierListIndex = originalLevelMap->ModifierListIndex;
		levelMap->ModifierIndex = originalLevelMap->ModifierIndex;
		levelMap->ValueListIndex = originalLevelMap->ValueListIndex;
		levelMap->Name = originalLevelMap->Name;
		levelMap->OriginalLevelMap = originalLevelMap;
		stats->LevelMaps.Primitives[modifier->LevelMapIndex] = levelMap;
	}

	if (lua->IsClient()) {
		levelMap->ClientFunction = RegistryEntry(L, 3);
	} else {
		levelMap->ServerFunction = RegistryEntry(L, 3);
	}
}

/// <summary>
/// Returns the specified stats entry as an object for easier manipulation.
/// If the `level` argument is not specified or is `nil`, the table will contain stat values as specified in the stat entry.
/// If the `level` argument is not `nil`, the table will contain level - scaled values for the specified level.
/// A `level` value of `-1` will use the level specified in the stat entry.
/// 
/// The behavior of getting a table entry is identical to that of `StatGetAttribute` and setting a table entry is identical to `StatSetAttribute`.
/// 
/// The `StatSetAttribute` example rewritten using `GetStat`:
/// ```lua
/// -- Swap DamageType from Poison to Air on all skills
/// for i, name in pairs(Ext.Stats.GetEntries("SkillData")) do
///     local stat = Ext.Stats.Get(name)
///     if stat.DamageType == "Poison" then
///         stat.DamageType = "Air"
///     end
/// end
/// ```
/// </summary>
/// <lua_export>Get</lua_export>
/// <lua_legacy>Ext.GetGet</lua_legacy>
/// <param name="statName">Stats name to fetch</param>
/// <param name="level">Specify `nil` to return raw (unscaled) stat values, `-1` to return values scaled to the stats level, or a specific level value to scale returned stats to that level</param>
/// <param name="warnOnError">Log a warning in the console if the stats object could not be found?</param>
/// <param name="byRef">Specifies whether the returned object should use by-value or by-ref properties (default: by-value)</param>
/// <returns></returns>
UserReturn Get(lua_State * L, char const* statName, std::optional<int> level, std::optional<bool> warnOnError, std::optional<bool> byRef)
{
	auto object = StatFindObject(statName, warnOnError.value_or(true));
	if (object != nullptr) {
		ObjectProxy2::MakeImpl<StatsEntryProxyRefImpl, Object>(L, object, State::FromLua(L)->GetCurrentLifetime(), level, !byRef.value_or(false));
		return 1;
	} else {
		push(L, nullptr);
		return 1;
	}
}

bool CopyStats(Object* obj, FixedString const& copyFrom)
{
	auto stats = GetStaticSymbols().GetStats();
	auto copyFromObject = stats->Objects.Find(copyFrom);
	if (copyFromObject == nullptr) {
		OsiError("Cannot copy stats from nonexistent object: " << copyFrom);
		return false;
	}

	if (obj->ModifierListIndex != copyFromObject->ModifierListIndex) {
		auto objModifier = stats->ModifierLists.Find(obj->ModifierListIndex);
		auto copyModifier = stats->ModifierLists.Find(obj->ModifierListIndex);
		OsiError("Cannot copy stats from object '" << copyFrom << "' (a " << copyModifier->Name 
			<< ") to an object of type " << objModifier->Name);
		return false;
	}

	obj->Level = copyFromObject->Level;
	obj->AIFlags = copyFromObject->AIFlags;

	for (size_t i = 0; i < obj->IndexedProperties.size(); i++) {
		obj->IndexedProperties[i] = copyFromObject->IndexedProperties[i];
	}

	for (auto const& prop : copyFromObject->PropertyLists) {
		// TODO - is reusing property list objects allowed?
		obj->PropertyLists.insert(prop.Key, prop.Value);
	}

	for (auto const& cond : copyFromObject->Conditions) {
		// TODO - is reusing condition objects allowed?
		obj->Conditions.insert(cond.Key, cond.Value);
	}

	obj->Requirements = copyFromObject->Requirements;
	obj->MemorizationRequirements = copyFromObject->MemorizationRequirements;
	obj->ComboCategories = copyFromObject->ComboCategories;

	return true;
}

/// <summary>
/// Creates a new stats entry. 
/// If a stat object with the same name already exists, the specified modifier type is invalid or the specified template doesn't exist, the function returns `nil`.
/// After all stat properties were initialized, the stats entry must be synchronized by calling `SyncStat()`.
/// 
/// - If the entry was created on the server, `SyncStat()` will replicate the stats entry to all clients.If the entry was created on the client, `SyncStat()` will only update it locally.
/// Example:
/// ```lua
/// local stat = Ext.Stats.Create("NRD_Dynamic_Skill", "SkillData", "Rain_Water")
/// stat.RainEffect = "RS3_FX_Environment_Rain_Fire_01"
/// stat.SurfaceType = "Fire"
/// Ext.Stats.Sync("NRD_Dynamic_Skill")
/// ```
/// </summary>
/// <lua_export>Create</lua_export>
/// <lua_legacy>Ext.CreateStat</lua_legacy>
/// <param name="statName">Name of stats entry to create; it should be globally unique</param>
/// <param name="modifierList">Stats entry type (eg. `SkillData`, `StatusData`, `Weapon`, etc.)</param>
/// <param name="copyFromTemplate">If this parameter is not `nil`, stats properties are copied from the specified stats entry to the newly created entry</param>
/// <param name="byRef">Specifies whether the created object should use by-value or by-ref properties (default: by-value)</param>
/// <returns>stats::Object</returns>
UserReturn Create(lua_State * L, FixedString const& statName, FixedString const& modifierList, std::optional<FixedString> copyFromTemplate, std::optional<bool> byRef)
{
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
		}
	}

	auto stats = GetStaticSymbols().GetStats();
	auto object = stats->CreateObject(statName, modifierList);
	if (!object) {
		push(L, nullptr);
		return 1;
	}

	if (copyFromTemplate) {
		if (!CopyStats(*object, *copyFromTemplate)) {
			push(L, nullptr);
			return 1;
		}
	}

	ObjectProxy2::MakeImpl<StatsEntryProxyRefImpl, Object>(L, *object, State::FromLua(L)->GetCurrentLifetime(), -1, !byRef.value_or(false));
	return 1;
}

/// <summary>
/// Synchronizes the changes made to the specified stats entry to each client.
/// `Sync` must be called each time a stats entry is modified dynamically (after `ModuleLoading`/`StatsLoaded`) to ensure that the hostand all clients see the same properties.
/// </summary>
/// <lua_export>Sync</lua_export>
/// <lua_legacy>Ext.SyncStat</lua_legacy>
/// <param name="statName">Name of stats entry to sync</param>
/// <param name="persist">Is the stats entry persistent, i.e. if it will be written to savegames. (default `true`)</param>
void Sync(FixedString const& statName, std::optional<bool> persist)
{
	auto stats = GetStaticSymbols().GetStats();
	auto object = stats->Objects.Find(statName);
	if (!object) {
		OsiError("Cannot sync nonexistent stat: " << statName);
		return;
	}

	stats->SyncWithPrototypeManager(object);

	if (gExtender->GetServer().IsInServerThread()) {
		object->BroadcastSyncMessage(false);

		gExtender->GetServer().GetExtensionState().MarkDynamicStat(statName);
		if (persist && *persist) {
			gExtender->GetServer().GetExtensionState().MarkPersistentStat(statName);
		}
	}
}

/// <summary>
/// Toggles whether the specified stats entry should be persisted to savegames.
/// Changes made to non - persistent stats will be lost the next time a game is reloaded.
/// If a dynamically created stats entry is marked as non - persistent, the entry will be deleted completely after the next reload.Make sure that you don't delete entries that are still in use as it could break the game in various ways.
/// </summary>
/// <lua_export>SetPersistence</lua_export>
/// <lua_legacy>Ext.StatSetPersistence</lua_legacy>
/// <param name="statName">Name of stats entry to update</param>
/// <param name="persist">Is the stats entry persistent, i.e. if it will be written to savegames</param>
void SetPersistence(FixedString const& statName, bool persist)
{
	if (!gExtender->GetServer().IsInServerThread()) {
		OsiError("Can only set persistence in server context");
		return;
	}

	auto stats = GetStaticSymbols().GetStats();
	auto object = stats->Objects.Find(statName);
	if (!object) {
		OsiError("Cannot set persistence for nonexistent stat: " << statName);
		return;
	}

	if (persist) {
		gExtender->GetServer().GetExtensionState().MarkPersistentStat(statName);
	} else {
		gExtender->GetServer().GetExtensionState().UnmarkPersistentStat(statName);
	}
}


UserReturn GetDeltaMod(lua_State* L, FixedString const& name, FixedString const& modifierType)
{
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
int TypedEnumLabelToIndex(lua_State* L, FixedString const& label)
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
if (strcmp(enumName.GetString(), #T) == 0) { \
	return TypedEnumIndexToLabel<NS::T>(L, (NS::T)index); \
}
#define BEGIN_ENUM(T, type) \
if (strcmp(enumName.GetString(), #T) == 0) { \
	return TypedEnumIndexToLabel<T>(L, (T)index); \
}

UserReturn EnumIndexToLabel(lua_State* L, FixedString const& enumName, int index)
{
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
if (strcmp(enumName.GetString(), #T) == 0) { \
	return TypedEnumLabelToIndex<NS::T>(L, label); \
}
#define BEGIN_ENUM(T, type) \
if (strcmp(enumName.GetString(), #T) == 0) { \
	return TypedEnumLabelToIndex<T>(L, label); \
}


UserReturn EnumLabelToIndex(lua_State* L, FixedString const& enumName, FixedString const& label)
{
#include <GameDefinitions/Enumerations.inl>

	auto valueList = GetStaticSymbols().GetStats()->ModifierValueLists.Find(enumName);
	if (valueList) {
		auto value = valueList->Values.find(FixedString(label));

		if (value) {
			push(L, value.Value());
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

UserReturn NewDamageList(lua_State* L)
{
	MakeObjectContainer<stats::DamagePairList>(L);
	return 1;
}

void AddVoiceMetaData(FixedString const& speakerGuid, FixedString const& translatedStringKey, char const* source,
	float length, std::optional<int> priority)
{
	auto speakerMgr = GetStaticSymbols().eoc__SpeakerManager;
	if (speakerMgr == nullptr || *speakerMgr == nullptr || (*speakerMgr)->SpeakerMetaDataHashMap == nullptr) {
		OsiError("Speaker manager not initialized!");
		return;
	}

	auto speaker = (*speakerMgr)->SpeakerMetaDataHashMap->get_or_insert(speakerGuid);
	auto voiceMeta = speaker->get_or_insert(translatedStringKey);
	voiceMeta->CodecID = 4;
	voiceMeta->IsRecorded = true;
	voiceMeta->Length = (float)length;
	voiceMeta->Priority = priority.value_or(0);

	auto path = GetStaticSymbols().ToPath(source, PathRootType::Data);
	voiceMeta->Source.Name = path;
}

void RegisterStatsLib()
{
	DECLARE_MODULE(Stats, Both)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetCharacterCreation)
	MODULE_FUNCTION(GetStats)
	MODULE_FUNCTION(GetStatsLoadedBefore)
	MODULE_FUNCTION(Get)
	MODULE_FUNCTION(Create)
	// TODO - move to stats object method
	MODULE_FUNCTION(Sync)
	MODULE_FUNCTION(SetPersistence)
	MODULE_FUNCTION(GetAttribute)
	MODULE_FUNCTION(SetAttribute)
	MODULE_FUNCTION(AddCustomDescription)
	MODULE_FUNCTION(SetLevelScaling)
	MODULE_FUNCTION(EnumIndexToLabel)
	MODULE_FUNCTION(EnumLabelToIndex)
	MODULE_FUNCTION(NewDamageList)
	MODULE_FUNCTION(AddVoiceMetaData)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, DeltaMod, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetDeltaMod)
	MODULE_NAMED_FUNCTION("Update", UpdateDeltaMod)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, SkillSet, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetSkillSet)
	MODULE_NAMED_FUNCTION("Update", UpdateSkillSet)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, EquipmentSet, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetEquipmentSet)
	MODULE_NAMED_FUNCTION("Update", UpdateEquipmentSet)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, TreasureTable, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("Get", GetTreasureTable)
	MODULE_NAMED_FUNCTION("GetLegacy", GetTreasureTableLegacy)
	MODULE_NAMED_FUNCTION("Update", UpdateTreasureTable)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, TreasureCategory, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetTreasureCategory)
	MODULE_NAMED_FUNCTION("Update", UpdateTreasureCategory)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, ItemCombo, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetItemCombo)
	MODULE_NAMED_FUNCTION("Update", UpdateItemCombo)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, ItemComboPreview, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetItemComboPreviewData)
	MODULE_NAMED_FUNCTION("Update", UpdateItemComboPreviewData)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, ItemComboProperty, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetItemComboProperty)
	MODULE_NAMED_FUNCTION("Update", UpdateItemComboProperty)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, ItemGroup, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetItemGroup)
	END_MODULE()
		
	DECLARE_SUBMODULE(Stats, NameGroup, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetNameGroup)
	END_MODULE()

	DECLARE_SUBMODULE(Stats, ItemSet, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("GetLegacy", GetItemSet)
	MODULE_NAMED_FUNCTION("Update", UpdateItemSet)
	END_MODULE()

	DECLARE_SUBMODULE(Stats, ItemColor, Both)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("Get", GetItemColor)
	MODULE_NAMED_FUNCTION("Update", UpdateItemColor)
	MODULE_NAMED_FUNCTION("GetAll", GetAllItemColors)
	END_MODULE()
}

END_NS()
