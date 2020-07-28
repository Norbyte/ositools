#include <stdafx.h>
#include <GameDefinitions/Symbols.h>
#include <Lua/LuaBinding.h>
#include <Lua/LuaSerializers.h>
#include <ExtensionHelpers.h>

#define P(name) s.VisitProperty(#name, v.name)

namespace dse::lua
{
	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Treasure_Table& v)
	{
		s.BeginObject();
		P(Name);
		P(MinLevel);
		P(MaxLevel);
		P(IgnoreLevelDiff);
		P(UseTreasureGroupContainers);
		P(CanMerge);
		P(SubTables);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Treasure_SubTable_Description& v)
	{
		s.BeginObject();
		P(TotalFrequency);
		P(TotalCount);
		P(StartLevel);
		P(EndLevel);
		P(Categories);
		P(DropCounts);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Treasure_SubTable_Description::Category& v)
	{
		auto stats = GetStaticSymbols().GetStats();

		s.BeginObject();
		P(Frequency);

		if (v.IsTreasureTable) {
			auto refTable = stats->TreasureTables.Find(v.Index);
			if (refTable) {
				s.VisitProperty("TreasureTable", refTable->Name);
			}
		} else {
			auto refCategory = stats->TreasureCategories.Find(v.Index);
			if (refCategory) {
				s.VisitProperty("TreasureCategory", refCategory->Category);
			}
		}

		for (int i = 0; i < 7; i++) {
			s.VisitProperty(stats->TreasureItemTypes[i].Str, v.Frequencies[i]);
		}

		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Treasure_SubTable_Description::DropCount& v)
	{
		s.BeginObject();
		P(Chance);
		P(Amount);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Treasure_Category& v)
	{
		s.BeginObject();
		P(Category);
		P(Items);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Treasure_Category::Item& v)
	{
		s.BeginObject();
		P(Name);
		P(Priority);
		P(MinAmount);
		P(MaxAmount);
		P(ActPart);
		P(Unique);
		P(MinLevel);
		P(MaxLevel);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemCombination& v)
	{
		s.BeginObject();
		P(Name);
		P(RecipeCategory);
		P(CraftingStation);
		P(AutoLevel);
		P(Ingredients);
		P(Results);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemCombinationIngredient& v)
	{
		s.BeginObject();
		P(Object);
		P(IngredientType);
		P(Transform);
		P(ItemRarity);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemCombinationResult& v)
	{
		s.BeginObject();
		P(Requirement);
		P(ReqLevel);
		P(PreviewStatsId);
		P(PreviewIcon);
		P(PreviewTooltip);
		P(Name);
		P(Results);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemCombinationResult::CombinationResult& v)
	{
		s.BeginObject();
		P(Result);
		P(Boost);
		P(ResultAmount);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemCombinationPreviewData& v)
	{
		s.BeginObject();
		P(Name);
		P(Type);
		P(StatsId);
		P(Tooltip);
		P(Icon);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemCombinationProperty& v)
	{
		s.BeginObject();
		P(Name);
		P(PreviewIcon);
		P(PreviewTooltip);
		P(Entries);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemCombinationProperty::ItemCombinationPropertyEntry& v)
	{
		s.BeginObject();
		P(ObjectId);
		P(IngredientType);
		P(Result);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CItemGroup& v)
	{
		s.BeginObject();
		P(Name);
		P(LevelGroups);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CLevelGroup& v)
	{
		s.BeginObject();
		P(MinLevel);
		P(MaxLevel);
		P(Name);
		P(RootGroups);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CRootGroup& v)
	{
		s.BeginObject();
		P(MinLevel);
		P(MaxLevel);
		P(RootGroup);
		P(NameGroupLinks);
		s.VisitProperty("Unknown", v.field_10);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CNameGroupLink& v)
	{
		s.BeginObject();
		P(NameGroup);
		P(NoneCoolSuffix);
		P(ItemName);
		s.VisitProperty("Unknown", v.field_0);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CNameGroup& v)
	{
		s.BeginObject();
		P(Name);
		P(Names);
		P(NamesCool);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CNameGroupName& v)
	{
		s.BeginObject();
		s.VisitProperty("Name", v.Name.Str1.WStr);
		s.VisitProperty("Name2", v.Name2.Str1.WStr);
		s.EndObject();
		return s;
	}

	void LuaSerializeStatsEnum(LuaSerializer& s, char const* key, FixedString const& enumName, int& v)
	{
		auto stats = GetStaticSymbols().GetStats();

		if (s.IsWriting) {
			push(s.L, stats->EnumIndexToLabel(enumName, v));
			lua_setfield(s.L, -2, key);
		} else {
			lua_getfield(s.L, -1, key);
			auto str = luaL_checkstring(s.L, -1);
			auto idx = stats->EnumLabelToIndex(enumName, str);
			if (idx) {
				v = *idx;
			} else {
				luaL_error(s.L, "'%s' is not a valid value for enumeration '%s'", enumName.Str, str);
			}
			lua_pop(s.L, 1);
		}
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Requirement& v)
	{
		s.BeginObject();
		s.VisitProperty("Requirement", v.RequirementId);
		s.VisitProperty("Not", v.Negate);

		if (s.IsWriting) {
			if (v.RequirementId == RequirementType::Tag) {
				s.VisitProperty("Param", v.StringParam);
			} else {
				s.VisitProperty("Param", v.IntParam);
			}
		} else {
			if (v.RequirementId == RequirementType::Tag) {
				auto param = getfield<char const*>(s.L, "Param");
				s.VisitProperty("Param", v.StringParam);
				v.IntParam = -1;
			} else {
				s.VisitProperty("Param", v.IntParam);
				v.StringParam = FixedString{};
			}
		}

		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_Data& v)
	{
		static long gIndex{ 0 };
		auto stats = GetStaticSymbols().GetStats();

		P(Context);
		s.VisitProperty("Type", v.TypeId);
		
		if (s.IsWriting) {
			if (v.Conditions) {
				STDString name(v.Name.Str);
				if (name[name.length() - 1] == ')') {
					auto ifPos = name.find("_IF(");
					if (ifPos != std::string::npos) {
						auto condition = name.substr(ifPos + 4, name.length() - ifPos - 5);
						setfield(s.L, "Condition", condition);
					}
				}
			}
		} else {
			STDString name = std::to_string(gIndex++).c_str();

			auto conditions = getfield<char const*>(s.L, "Condition");
			if (conditions && *conditions) {
				name += "_IF(";
				name += conditions;
				name += ")";

				auto scriptCheckBlock = stats->BuildScriptCheckBlockFromProperties(conditions);
				if (scriptCheckBlock) {
					auto statConditions = GameAlloc<CDivinityStats_Condition>();
					statConditions->ScriptCheckBlock = scriptCheckBlock;
					statConditions->Name = MakeFixedString(name.c_str());
					v.Conditions = statConditions;
				}
				else {
					OsiWarn("Failed to parse conditions: " << conditions);
				}
			}

			v.Name = MakeFixedString(name.c_str());
		}

		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_Custom& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		s.VisitProperty("Action", v.Name);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_Status& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		s.VisitProperty("Action", v.Status);
		P(StatusChance);
		P(Duration);
		P(StatsId);
		P(Arg4);
		P(Arg5);
		P(SurfaceBoost);
		P(SurfaceBoosts);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_SurfaceChange& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		LuaSerializeStatsEnum(s, "Action", GFS.strSurfaceChange, v.SurfaceChange);
		P(SurfaceChance);
		P(Lifetime);
		P(StatusChance);
		P(Radius);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_GameAction& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		LuaSerializeStatsEnum(s, "Action", GFS.strGameAction, v.GameAction);
		P(Arg1);
		P(Arg2);
		P(Arg3);
		P(Arg4);
		P(Arg5);
		LuaSerializeStatsEnum(s, "StatusHealType", GFS.strStatusHealType, v.StatusHealType);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_OsirisTask& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		LuaSerializeStatsEnum(s, "Action", GFS.strOsirisTask, v.OsirisTask);
		P(Chance);
		P(VitalityOnRevive);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_Sabotage& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		P(Amount);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_Summon& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		P(Template);
		P(Duration);
		P(IsTotem);
		P(Skill);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CDivinityStats_Object_Property_Force& v)
	{
		s << static_cast<CDivinityStats_Object_Property_Data&>(v);
		P(Distance);
		return s;
	}

	void SerializeObjectProperty(LuaSerializer& s, CDivinityStats_Object_Property_Data*& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			if (!v) {
				lua_pushnil(s.L);
			} else {
				#define V(type, cls) case CRPGStats_Object_Property_Type::type: \
					s << *static_cast<cls*>(v); \
					break;

				switch (v->TypeId) {
					V(Custom, CDivinityStats_Object_Property_Custom)
					V(Status, CDivinityStats_Object_Property_Status)
					V(SurfaceChange, CDivinityStats_Object_Property_SurfaceChange)
					V(GameAction, CDivinityStats_Object_Property_GameAction)
					V(OsirisTask, CDivinityStats_Object_Property_OsirisTask)
					V(Sabotage, CDivinityStats_Object_Property_Sabotage)
					V(Summon, CDivinityStats_Object_Property_Summon)
					V(Force, CDivinityStats_Object_Property_Force)

					default:
						ERR("Unable to serialize unknown object property type %d to Lua!", v->TypeId);
				}
				#undef V
			}
		} else {
			if (v == nullptr) {
				CRPGStats_Object_Property_Type type;
				s.VisitProperty("Type", type);
				v = GetStaticSymbols().GetStats()->ConstructProperty(type);
			}

			if (v) {
				#define V(type, cls) case CRPGStats_Object_Property_Type::type: \
					s << *static_cast<cls*>(v); \
					break;

				switch (v->TypeId) {
					V(Custom, CDivinityStats_Object_Property_Custom)
					V(Status, CDivinityStats_Object_Property_Status)
					V(SurfaceChange, CDivinityStats_Object_Property_SurfaceChange)
					V(GameAction, CDivinityStats_Object_Property_GameAction)
					V(OsirisTask, CDivinityStats_Object_Property_OsirisTask)
					V(Sabotage, CDivinityStats_Object_Property_Sabotage)
					V(Summon, CDivinityStats_Object_Property_Summon)
					V(Force, CDivinityStats_Object_Property_Force)

					default:
						ERR("Unable to serialize unknown object property type %d to Lua!", v->TypeId);
				}
				#undef V
			}
		}
		s.EndObject();
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_Object_Property_List& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			int index{ 1 };
			v.Properties.NameHashMap.Iterate([&s, &v, &index](auto const& k, auto& idx) {
				push(s.L, index++);
				SerializeObjectProperty(s, v.Properties.Primitives[idx]);
				lua_settable(s.L, -3);
			});
		} else {
			for (auto idx : iterate(s.L, -1)) {
				CDivinityStats_Object_Property_Data* prop{ nullptr };
				SerializeObjectProperty(s, prop);
				if (prop) {
					v.Properties.Add(prop->Name, prop);
					v.AllPropertyContexts = prop->Context;
				}
			}
		}
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, CRPGStats_DeltaModifier& v)
	{
		auto stats = GetStaticSymbols().GetStats();

		P(ModifierType);
		P(SlotType);
		P(WeaponType);
		P(ArmorType);
		P(Handedness);
		P(Name);
		P(BoostType);
		P(MinLevel);
		P(MinLevel);
		P(Frequency);

		if (s.IsWriting) {
			lua_newtable(s.L);
			int index = 1;
			for (uint32_t i = 0; i < v.BoostIndices.Set.Size; i++) {
				auto boost = stats->objects.Find(v.BoostIndices[i]);
				if (boost != nullptr) {
					push(s.L, index++);
					lua_newtable(s.L);
					s.VisitProperty("Boost", boost->Name);
					s.VisitProperty("Count", v.BoostCounts[i]);
					lua_settable(s.L, -3);
				}
			}
			lua_setfield(s.L, -2, "Boosts");
		} else {
			v.BoostCounts.Set.Clear();
			v.BoostIndices.Set.Clear();
			lua_getfield(s.L, 1, "Boosts");

			luaL_checktype(s.L, -1, LUA_TTABLE);
			for (auto valueIndex : iterate(s.L, -1)) {
				FixedString boost;
				int count;
				s.VisitProperty("Boost", boost);
				s.VisitProperty("Count", count);

				auto object = stats->objects.FindIndex(boost);
				if (object != -1) {
					v.BoostIndices.Set.Add(object);
					v.BoostCounts.Set.Add(count);
				} else {
					OsiError("DeltaMod references nonexistent boost '" << boost << "'");
				}
			}
		}

		return s;
	}
}

#undef P
