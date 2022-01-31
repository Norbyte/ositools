#include <stdafx.h>
#include <GameDefinitions/Symbols.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/Shared/ExtensionHelpers.h>

#define P(name) s.VisitProperty(#name, v.name)
#define PO(name, default) s.VisitOptionalProperty(#name, v.name, default)

namespace dse::lua
{
	void LuaToInvokeDataValue(lua_State* L, int index, ig::InvokeDataValue& val);
	void InvokeDataValueToLua(lua_State* L, ig::InvokeDataValue const& val);

	LuaSerializer& operator << (LuaSerializer& s, TranslatedString& v)
	{
		// TODO - is this enough?
		s << v.Handle.Handle;
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, char const*& v)
	{
		if (s.IsWriting) {
			push(s.L, v);
		} else {
			// Cannot unserialize string pointers, as we don't know how their memory should be managed
		}
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, ig::InvokeDataValue& v)
	{
		if (s.IsWriting) {
			InvokeDataValueToLua(s.L, v);
		} else {
			LuaToInvokeDataValue(s.L, 1, v);
		}

		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, TypeInformationRef& v)
	{
		if (s.IsWriting) {
			if (v) {
				MakeObjectRef(s.L, v.GetStatic()->Type);
			} else {
				push(s.L, nullptr);
			}
		}

		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::EquipmentSet& v)
	{
		s.BeginObject();
		P(Name);
		P(Groups);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::EquipmentGroup& v)
	{
		s.BeginObject();
		// Name property is unused
		// P(Name);
		P(Equipment);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::SkillSet& v)
	{
		s.BeginObject();
		P(Name);
		P(Skills);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureTable& v)
	{
		s.BeginObject();
		P(Name);
		PO(MinLevel, 0);
		PO(MaxLevel, 0);
		PO(IgnoreLevelDiff, false);
		PO(UseTreasureGroupContainers, false);
		PO(CanMerge, false);
		P(SubTables);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureSubTableDescription& v)
	{
		s.BeginObject();
		P(TotalCount);
		PO(StartLevel, 0);
		PO(EndLevel, 0);
		P(Categories);
		P(DropCounts);

		if (!s.IsWriting) {
			v.TotalFrequency = 0;
			v.CategoryFrequencies.reallocate(v.Categories.size());
			for (uint32_t i = 0; i < v.Categories.size(); i++) {
				v.CategoryFrequencies[i] = v.Categories[i]->Frequency;
				v.TotalFrequency += v.Categories[i]->Frequency;
			}

			// Allow negative TotalCount if there are no DropCounts, since a negative value indicates a "guaranteed" drop
			if (v.DropCounts.size() > 0 || v.TotalCount > 0) {
				v.TotalCount = 0;
				for (uint32_t i = 0; i < v.DropCounts.size(); i++) {
					v.TotalCount += v.DropCounts[i].Amount;
				}
			}

			v.Amounts.reallocate(v.DropCounts.size());
			for (uint32_t i = 0; i < v.DropCounts.size(); i++) {
				v.Amounts[i] = v.DropCounts[i].Amount;
			}
		}

		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureSubTableDescription::Category& v)
	{
		auto stats = GetStaticSymbols().GetStats();

		s.BeginObject();
		PO(Frequency, 1);

		if (s.IsWriting) {
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
		} else {
			FixedString treasureTable, treasureCategory;
			s.VisitOptionalProperty("TreasureTable", treasureTable, GFS.strEmpty);
			s.VisitOptionalProperty("TreasureCategory", treasureCategory, GFS.strEmpty);

			if (treasureTable && treasureTable != GFS.strEmpty) {
				auto idx = stats->TreasureTables.FindIndex(treasureTable);
				if (idx) {
					v.Index = *idx;
				} else {
					luaL_error(s.L, "Treasure table '%s' does not exist!", treasureTable.Str);
				}

				v.IsTreasureTable = true;
				v.IsTreasureTable2 = true;
			} else {
				auto idx = stats->TreasureCategories.FindIndex(treasureCategory);
				if (idx) {
					v.Index = *idx;
				} else {
					luaL_error(s.L, "Treasure category '%s' does not exist!", treasureCategory.Str);
				}

				v.IsTreasureTable = false;
				v.IsTreasureTable2 = false;
			}
		}

		for (int i = 0; i < 7; i++) {
			s.VisitOptionalProperty(stats->TreasureItemTypes[i].Str, v.Frequencies[i], (uint16_t)0);
		}

		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureSubTableDescription::DropCount& v)
	{
		s.BeginObject();
		P(Chance);
		P(Amount);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureCategory& v)
	{
		s.BeginObject();
		P(Category);
		P(Items);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::TreasureCategory::Item& v)
	{
		s.BeginObject();
		P(Name);
		PO(Priority, 1);
		PO(MinAmount, 1);
		PO(MaxAmount, 1);
		P(ActPart);
		PO(Unique, 0);
		PO(MinLevel, 0);
		PO(MaxLevel, 0);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombination& v)
	{
		s.BeginObject();
		P(Name);
		P(RecipeCategory);
		PO(CraftingStation, CraftingStationType::None);
		PO(AutoLevel, false);
		P(Ingredients);
		P(Results);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationIngredient& v)
	{
		s.BeginObject();
		P(Object);
		P(IngredientType);
		PO(Transform, IngredientTransformType::None);
		PO(ItemRarity, ItemDataRarity::Sentinel);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationResult& v)
	{
		s.BeginObject();
		PO(Requirement, stats::AbilityType::Sentinel);
		PO(ReqLevel, 0);
		P(PreviewStatsId);
		PO(PreviewIcon, GFS.strEmpty);
		PO(PreviewTooltip, GFS.strEmpty);
		P(Name);
		P(Results);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationResult::CombinationResult& v)
	{
		s.BeginObject();
		P(Result);
		PO(Boost, GFS.strEmpty);
		PO(ResultAmount, 1);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationPreviewData& v)
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

	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationProperty& v)
	{
		s.BeginObject();
		P(Name);
		P(PreviewIcon);
		P(PreviewTooltip);
		P(Entries);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::ItemCombinationProperty::Entry& v)
	{
		s.BeginObject();
		P(ObjectId);
		P(IngredientType);
		P(Result);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::CItemGroup& v)
	{
		s.BeginObject();
		P(Name);
		P(LevelGroups);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::CLevelGroup& v)
	{
		s.BeginObject();
		P(MinLevel);
		P(MaxLevel);
		P(Name);
		P(RootGroups);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::CRootGroup& v)
	{
		s.BeginObject();
		P(MinLevel);
		P(MaxLevel);
		P(RootGroup);
		PO(Color, GFS.strEmpty);
		P(NameGroupLinks);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::CNameGroupLink& v)
	{
		s.BeginObject();
		P(NameGroup);
		P(NoneCoolSuffix);
		P(ItemName);
		s.VisitOptionalProperty("Unknown", v.field_0, 0);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::NameGroup& v)
	{
		s.BeginObject();
		P(Name);
		P(Names);
		P(NamesCool);
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::NameGroupName& v)
	{
		s.BeginObject();
		s.VisitProperty("Name", v.Name.Handle.ReferenceString);
		s.VisitProperty("Name2", v.Name2.Handle.ReferenceString);
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

	LuaSerializer& operator << (LuaSerializer& s, stats::Requirement& v)
	{
		s.BeginObject();
		s.VisitProperty("Requirement", v.RequirementId);
		s.VisitProperty("Not", v.Not);

		if (s.IsWriting) {
			if (v.RequirementId == stats::RequirementType::Tag) {
				s.VisitProperty("Param", v.Tag);
			} else {
				s.VisitProperty("Param", v.Param);
			}
		} else {
			if (v.RequirementId == stats::RequirementType::Tag) {
				auto param = checked_getfield<char const*>(s.L, "Param");
				s.VisitProperty("Param", v.Tag);
				v.Param = -1;
			} else {
				s.VisitProperty("Param", v.Param);
				v.Tag = FixedString{};
			}
		}

		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyData& v)
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

			auto conditions = checked_getfield<std::optional<char const*>>(s.L, "Condition");
			if (conditions && *conditions) {
				name += "_IF(";
				name += *conditions;
				name += ")";

				auto scriptCheckBlock = stats->BuildScriptCheckBlockFromProperties(*conditions);
				if (scriptCheckBlock) {
					auto statConditions = GameAlloc<stats::Condition>();
					statConditions->ScriptCheckBlock = scriptCheckBlock;
					statConditions->Name = FixedString(name.c_str());
					v.Conditions = statConditions;
				}
				else {
					OsiWarn("Failed to parse conditions: " << *conditions);
				}
			}

			v.Name = FixedString(name.c_str());
		}

		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyCustom& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		s.VisitProperty("Action", v.Name);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyStatus& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		s.VisitProperty("Action", v.Status);
		PO(StatusChance, 100.0f);
		PO(Duration, 1.0f);
		auto statsId = checked_getfield<std::optional<FixedString>>(s.L, "Arg3");
		if (statsId) {
			v.StatsId = *statsId;
		} else {
			PO(StatsId, GFS.strEmpty);
		}
		PO(Arg4, -1);
		PO(Arg5, -1);
		PO(SurfaceBoost, false);
		PO(SurfaceBoosts, ObjectSet<SurfaceType>{});
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertySurfaceChange& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		LuaSerializeStatsEnum(s, "Action", GFS.strSurfaceChange, v.SurfaceChange);
		PO(SurfaceChance, 1.0f);
		PO(Lifetime, 0.0f);
		PO(StatusChance, 0.0f);
		PO(Radius, -1.0f);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyGameAction& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		LuaSerializeStatsEnum(s, "Action", GFS.strGameAction, v.GameAction);
		PO(Arg1, -1.0f);
		PO(Arg2, -1.0f);
		PO(Arg3, GFS.strEmpty);
		PO(Arg4, 1.0f);
		PO(Arg5, 0.0f);
		LuaSerializeStatsEnum(s, "StatusHealType", GFS.strStatusHealType, v.StatusHealType);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyOsirisTask& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		LuaSerializeStatsEnum(s, "Action", GFS.strOsirisTask, v.OsirisTask);
		PO(Chance, 1.0f);
		PO(VitalityOnRevive, -1);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertySabotage& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		PO(Amount, 1);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertySummon& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		P(Template);
		PO(Duration, 6.0f);
		PO(IsTotem, false);
		PO(Skill, GFS.strEmpty);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyForce& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		P(Distance);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyCustomDescription& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		s.VisitProperty("TextLine1", v.TextLine1);
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyExtender& v)
	{
		s << static_cast<stats::PropertyData&>(v);
		s.VisitProperty("Action", v.PropertyName);
		PO(Arg1, 100.0f);
		PO(Arg2, 1.0f);
		auto arg3 = checked_getfield<std::optional<FixedString>>(s.L, "Arg3");
		if (arg3) {
			v.Arg3 = *arg3;
		} else {
			PO(Arg3, GFS.strEmpty);
		}
		PO(Arg4, -1);
		PO(Arg5, -1);
		return s;
	}

	void SerializeObjectProperty(LuaSerializer& s, stats::PropertyData*& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			if (!v) {
				lua_pushnil(s.L);
			} else {
				#define V(type, cls) case stats::PropertyType::type: \
					s << *static_cast<cls*>(v); \
					break;

				switch (v->TypeId) {
					V(Custom, stats::PropertyCustom)
					V(Status, stats::PropertyStatus)
					V(SurfaceChange, stats::PropertySurfaceChange)
					V(GameAction, stats::PropertyGameAction)
					V(OsirisTask, stats::PropertyOsirisTask)
					V(Sabotage, stats::PropertySabotage)
					V(Summon, stats::PropertySummon)
					V(Force, stats::PropertyForce)
					V(CustomDescription, stats::PropertyCustomDescription)
					V(Extender, stats::PropertyExtender)

					default:
						ERR("Unable to serialize unknown object property type %d to Lua!", v->TypeId);
				}
				#undef V
			}
		} else {
			if (v == nullptr) {
				stats::PropertyType type;
				s.VisitProperty("Type", type);
				v = GetStaticSymbols().GetStats()->ConstructProperty(type);
			}

			if (v) {
				#define V(type, cls) case stats::PropertyType::type: \
					s << *static_cast<cls*>(v); \
					break;

				switch (v->TypeId) {
					V(Custom, stats::PropertyCustom)
					V(Status, stats::PropertyStatus)
					V(SurfaceChange, stats::PropertySurfaceChange)
					V(GameAction, stats::PropertyGameAction)
					V(OsirisTask, stats::PropertyOsirisTask)
					V(Sabotage, stats::PropertySabotage)
					V(Summon, stats::PropertySummon)
					V(Force, stats::PropertyForce)
					V(CustomDescription, stats::PropertyCustomDescription)
					V(Extender, stats::PropertyExtender)

					default:
						ERR("Unable to serialize unknown object property type %d to Lua!", v->TypeId);
				}
				#undef V
			}
		}
		s.EndObject();
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::PropertyList& v)
	{
		s.BeginObject();
		if (s.IsWriting) {
			int index{ 1 };
			for (auto const& val : v.Properties.NameHashMap) {
				push(s.L, index++);
				SerializeObjectProperty(s, v.Properties.Primitives[val.Value]);
				lua_settable(s.L, -3);
			}
		} else {
			for (auto idx : iterate(s.L, -1)) {
				stats::PropertyData* prop{ nullptr };
				SerializeObjectProperty(s, prop);
				if (prop) {
					v.Properties.Add(prop->Name, prop);
					v.AllPropertyContexts |= prop->Context;
				}
			}
		}
		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, stats::DeltaModifier& v)
	{
		auto stats = GetStaticSymbols().GetStats();

		s.BeginObject();

		P(ModifierType);
		PO(SlotType, stats::ItemSlot32::Sentinel);
		PO(WeaponType, stats::WeaponType::Sentinel);
		PO(ArmorType, stats::ArmorType::Sentinel);
		PO(Handedness, stats::HandednessType::Any);
		P(Name);
		P(BoostType);
		PO(MinLevel, -1);
		PO(MaxLevel, -1);
		PO(Frequency, 1);

		if (s.IsWriting) {
			lua_newtable(s.L);
			int index = 1;
			for (uint32_t i = 0; i < v.BoostIndices.size(); i++) {
				auto boost = stats->Objects.Find(v.BoostIndices[i]);
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
			v.BoostCounts.clear();
			v.BoostIndices.clear();
			lua_getfield(s.L, 1, "Boosts");

			luaL_checktype(s.L, -1, LUA_TTABLE);
			for (auto valueIndex : iterate(s.L, -1)) {
				FixedString boost;
				int count;
				s.VisitProperty("Boost", boost);
				s.VisitOptionalProperty("Count", count, 1);

				auto object = stats->Objects.FindIndex(boost);
				if (object) {
					v.BoostIndices.push_back(*object);
					v.BoostCounts.push_back(count);
				} else {
					OsiError("DeltaMod references nonexistent boost '" << boost << "'");
				}
			}

			lua_pop(s.L, 1);
		}

		s.EndObject();
		return s;
	}

	LuaSerializer& operator << (LuaSerializer& s, SurfaceTemplate::StatusData& v)
	{
		s.BeginObject();
		P(StatusId);
		PO(Chance, 1.0f);
		P(Duration);
		PO(RemoveStatus, false);
		PO(OnlyWhileMoving, false);
		PO(ApplyToCharacters, true);
		PO(ApplyToItems, true);
		PO(KeepAlive, true);
		PO(VanishOnReapply, false);
		PO(ForceStatus, false);
		s.EndObject();
		return s;
	}
}

#undef P
