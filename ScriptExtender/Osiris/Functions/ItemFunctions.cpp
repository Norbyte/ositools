#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <ScriptHelpers.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse::esv
{
	FunctionHandle ItemDeltaModIteratorEventHandle;

	namespace func
	{
		bool ItemGetStatsId(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->StatsId.Str) {
				OsiError("Item '" << itemGuid << "' has no stats ID!");
				return false;
			} else {
				args[1].Set(item->StatsId.Str);
				return true;
			}
		}

		bool ItemGetGenerationParams(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->Generation) {
				OsiError("Item '" << itemGuid << "' has no generation data!");
				return false;
			} else {
				OsiWarn("NRD_ItemGetGenerationParams() with 4 arguments is deprecated. Use the 5-argument version instead!");

				args[1].Set(item->Generation->Base ? item->Generation->Base.Str : "");
				args[2].Set(item->Generation->ItemType ? item->Generation->ItemType.Str : "");
				args[3].Set((int32_t)item->Generation->Level);
				return true;
			}
		}

		bool ItemGetGenerationParams2(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->Generation) {
				OsiError("Item '" << itemGuid << "' has no generation data!");
				return false;
			} else {
				args[1].String = item->Generation->Base ? item->Generation->Base.Str : "";
				args[2].String = item->Generation->ItemType ? item->Generation->ItemType.Str : "";
				args[3].Int32 = item->Generation->Level;
				args[4].Int32 = item->Generation->Random;
				return true;
			}
		}

		bool ItemHasDeltaModifier(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			FixedString deltaMod(args[1].String);

			int32_t count = 0;
			if (item->Stats != nullptr) {
				for (auto const& boost : item->Stats->BoostNameSet) {
					if (boost == deltaMod) {
						count++;
					}
				}
			}

			if (item->Generation != nullptr) {
				for (auto const& boost : item->Generation->Boosts) {
					if (boost == deltaMod) {
						count++;
					}
				}
			}

			args[2].Set(count);
			return count > 0;
		}


		void ItemIterateDeltaModifiers(OsiArgumentDesc const & args)
		{
			auto itemGuid = args[0].String;
			auto eventName = args[1].String;

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) return;

			if (item->Generation != nullptr) {
				for (auto const& boost : item->Generation->Boosts) {
					auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
					eventArgs->Add(OsiArgumentValue{ ValueType::ItemGuid, itemGuid });
					eventArgs->Add(OsiArgumentValue{ ValueType::String, boost.Str });
					eventArgs->Add(OsiArgumentValue{ 1 });
					gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(ItemDeltaModIteratorEventHandle, eventArgs);
					delete eventArgs;
				}
			}

			if (item->Stats != nullptr) {
				for (auto const& boost : item->Stats->BoostNameSet) {
					auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
					eventArgs->Add(OsiArgumentValue{ ValueType::ItemGuid, itemGuid });
					eventArgs->Add(OsiArgumentValue{ ValueType::String, boost.Str });
					eventArgs->Add(OsiArgumentValue{ 0 });
					gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(ItemDeltaModIteratorEventHandle, eventArgs);
					delete eventArgs;
				}
			}
		}

		void ItemSetIdentified(OsiArgumentDesc const & args)
		{
			auto itemGuid = args[0].String;
			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return;
			}

			if (item->Stats == nullptr) {
				OsiError("Item '" << itemGuid << "' has no dynamic stats!");
				return;
			}

			item->Stats->IsIdentified = args[1].Int32 ? 1 : 0;
		}

		bool ItemGetParent(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto & parentGuid = args[1];

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->ParentInventoryHandle) {
				return false;
			}

			auto inventory = FindInventoryByHandle(item->ParentInventoryHandle);
			if (inventory == nullptr) {
				return false;
			}

			if (!inventory->ParentHandle) {
				return false;
			}

			auto parent = GetEntityWorld()->GetGameObject(inventory->ParentHandle);
			if (parent != nullptr) {
				parentGuid.Set(parent->MyGuid.Str);
				return true;
			} else {
				return false;
			}
		}

		CDivinityStats_Equipment_Attributes * GetItemDynamicStat(esv::Item * item, uint32_t index)
		{
			if (item->Stats == nullptr) {
				OsiErrorS("Item has no dynamic stats!");
				return nullptr;
			}

			auto numStats = item->Stats->DynamicStats.size();
			if (numStats <= 1) {
				OsiError("Tried to get dynamic stat " << index << ", item only has " << numStats);
				return nullptr;
			}

			return item->Stats->DynamicStats[index];
		}

		template <OsiPropertyMapType Type>
		bool ItemGet(OsiArgumentDesc & args)
		{
			auto item = GetEntityWorld()->GetItem(args[0].String);
			if (item == nullptr) return false;

			bool fetched = false;
			if (item->Stats != nullptr) {
				fetched = OsirisPropertyMapGet(gItemStatsPropertyMap, item->Stats, args, 1, Type, false);
			}

			if (!fetched) {
				fetched = OsirisPropertyMapGet(gItemPropertyMap, item, args, 1, Type, true);
			}

			return fetched;
		}

		template <OsiPropertyMapType Type>
		bool ItemGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto item = GetEntityWorld()->GetItem(args[0].String);
			if (item == nullptr) return false;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return false;

			auto & propertyMap = permanentBoosts->GetPropertyMap();
			return OsirisPropertyMapGetRaw(propertyMap, permanentBoosts, args, 1, Type);
		}

		template <OsiPropertyMapType Type>
		void ItemSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto item = GetEntityWorld()->GetItem(args[0].String);
			if (item == nullptr) return;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return;

			auto & propertyMap = permanentBoosts->GetPropertyMap();
			OsirisPropertyMapSetRaw(propertyMap, permanentBoosts, args, 1, Type);
		}

		bool ItemGetPermanentBoostAbility(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto ability = args[1].String;
			auto & level = args[2];

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) return false;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return false;

			auto abilityId = EnumInfo<AbilityType>::Find(ability);
			if (!abilityId) {
				OsiError("Ability name is invalid: " << ability);
				return false;
			}

			level.Set(permanentBoosts->AbilityModifiers[(unsigned)*abilityId]);
			return true;
		}

		bool ItemGetPermanentBoostTalent(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto talent = args[1].String;
			auto & enabled = args[2];

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) return false;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return false;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return false;
			}

			enabled.Set(permanentBoosts->Talents.HasTalent(*talentId) ? 1 : 0);
			return true;
		}

		void ItemSetPermanentBoostAbility(OsiArgumentDesc const & args)
		{
			auto itemGuid = args[0].String;
			auto ability = args[1].String;
			auto level = args[2].Int32;

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) return;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return;

			auto abilityId = EnumInfo<AbilityType>::Find(ability);
			if (!abilityId) {
				OsiError("Ability name is invalid: " << ability);
				return;
			}

			permanentBoosts->AbilityModifiers[(unsigned)*abilityId] = level;
		}

		void ItemSetPermanentBoostTalent(OsiArgumentDesc const & args)
		{
			auto itemGuid = args[0].String;
			auto talent = args[1].String;
			auto enabled = args[2].Int32;

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) return;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return;
			}

			permanentBoosts->Talents.Toggle(*talentId, enabled != 0);
		}


		void ItemConstructBegin(OsiArgumentDesc const & args)
		{
			auto templateGuid = args[0].String;

			if (ExtensionState::Get().PendingItemClone) {
				OsiWarn("ItemConstructBegin() called when a clone is already in progress. Previous clone state will be discarded.");
			}

			ExtensionState::Get().PendingItemClone.reset();

			auto & clone = ExtensionState::Get().PendingItemClone;
			clone = std::make_unique<ObjectSet<eoc::ItemDefinition>>();
			script::CreateItemDefinition(templateGuid, *clone);
		}


		void ItemCloneBegin(OsiArgumentDesc const & args)
		{
			if (ExtensionState::Get().PendingItemClone) {
				OsiWarn("ItemCloneBegin() called when a clone is already in progress. Previous clone state will be discarded.");
			}
			

			auto itemGuid = args[0].String;
			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) return;

			auto & clone = ExtensionState::Get().PendingItemClone;
			clone = std::make_unique<ObjectSet<eoc::ItemDefinition>>();
			script::ParseItem(item, *clone, false);
		}


		bool ItemClone(OsiArgumentDesc & args)
		{
			if (!ExtensionState::Get().PendingItemClone) {
				OsiErrorS("No item clone is in progress");
				return false;
			}

			auto & clone = ExtensionState::Get().PendingItemClone;
			auto item = GetStaticSymbols().esv__CreateItemFromParsed(clone.get(), 0);
			clone.reset();

			if (item == nullptr) {
				OsiErrorS("Failed to clone item.");
				return false;
			}

			args[0].Set(item->GetGuid()->Str);
			return true;
		}


		template <OsiPropertyMapType Type>
		void ItemCloneSet(OsiArgumentDesc const & args)
		{
			auto & clone = ExtensionState::Get().PendingItemClone;
			if (!clone) {
				OsiErrorS("No item clone is currently in progress!");
				return;
			}

			OsirisPropertyMapSet(gEoCItemDefinitionPropertyMap, 
				&(*clone)[0], args, 0, Type);
		}


		void ItemCloneResetProgression(OsiArgumentDesc const & args)
		{
			auto & clone = ExtensionState::Get().PendingItemClone;
			if (!clone) {
				OsiErrorS("No item clone is currently in progress!");
				return;
			}

			auto & item = (*clone)[0];
			item.LevelGroupIndex = -1;
			item.RootGroupIndex = -1;
			item.NameIndex = -1;
			item.NameCool = 0;
		}


		void ItemCloneAddBoost(OsiArgumentDesc const & args)
		{
			auto & clone = ExtensionState::Get().PendingItemClone;
			if (!clone) {
				OsiErrorS("No item clone is currently in progress!");
				return;
			}

			auto boostType = args[0].String;
			FixedString boostName(args[1].String);

			if (!boostName) {
				OsiError("Not a valid boost name: " << args[1].String);
				return;
			}

			auto & defn = (*clone)[0];
			if (strcmp(boostType, "Generation") == 0) {
				defn.GenerationBoosts.Add(boostName);
			} else if (strcmp(boostType, "DeltaMod") == 0) {
				defn.DeltaMods.Add(boostName);
			} else if (strcmp(boostType, "Rune") == 0) {
				for (auto& boost : defn.RuneBoosts) {
					if (!boost) {
						boost = boostName;
						return;
					}
				}

				OsiError("Item has no free rune slot");
			} else {
				OsiError("Unknown boost type: " << boostType);
			}
		}
	}

	void CustomFunctionLibrary::RegisterItemFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto itemGetStatsId = std::make_unique<CustomQuery>(
			"NRD_ItemGetStatsId",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "StatsId", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::ItemGetStatsId
		);
		functionMgr.Register(std::move(itemGetStatsId));

		auto itemGetGenerationParams = std::make_unique<CustomQuery>(
			"NRD_ItemGetGenerationParams",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Base", ValueType::String, FunctionArgumentDirection::Out },
				{ "ItemType", ValueType::String, FunctionArgumentDirection::Out },
				{ "Level", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::ItemGetGenerationParams
		);
		functionMgr.Register(std::move(itemGetGenerationParams));

		auto itemGetGenerationParams2 = std::make_unique<CustomQuery>(
			"NRD_ItemGetGenerationParams",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Base", ValueType::String, FunctionArgumentDirection::Out },
				{ "ItemType", ValueType::String, FunctionArgumentDirection::Out },
				{ "Level", ValueType::Integer, FunctionArgumentDirection::Out },
				{ "Random", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::ItemGetGenerationParams2
		);
		functionMgr.Register(std::move(itemGetGenerationParams2));

		auto itemHasDeltaMod = std::make_unique<CustomQuery>(
			"NRD_ItemHasDeltaModifier",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "DeltaMod", ValueType::String, FunctionArgumentDirection::Out },
				{ "Count", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::ItemHasDeltaModifier
		);
		functionMgr.Register(std::move(itemHasDeltaMod));

		auto itemIterateDeltaMods = std::make_unique<CustomCall>(
			"NRD_ItemIterateDeltaModifiers",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "EventName", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::ItemIterateDeltaModifiers
		);
		functionMgr.Register(std::move(itemIterateDeltaMods));

		auto itemSetIdentified = std::make_unique<CustomCall>(
			"NRD_ItemSetIdentified",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "IsIdentified", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::ItemSetIdentified
		);
		functionMgr.Register(std::move(itemSetIdentified));


		auto itemGetParent = std::make_unique<CustomQuery>(
			"NRD_ItemGetParent",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Parent", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::ItemGetParent
		);
		functionMgr.Register(std::move(itemGetParent));


		auto itemGetInt = std::make_unique<CustomQuery>(
			"NRD_ItemGetInt",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ItemGet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(itemGetInt));

		auto itemGetString = std::make_unique<CustomQuery>(
			"NRD_ItemGetString",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::ItemGet<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(itemGetString));

		auto itemGetGuidString = std::make_unique<CustomQuery>(
			"NRD_ItemGetGuidString",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::ItemGet<OsiPropertyMapType::GuidStringHandle>
		);
		functionMgr.Register(std::move(itemGetGuidString));


		auto itemGetPermanentBoostInt = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(itemGetPermanentBoostInt));


		auto itemGetPermanentBoostReal = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoostReal",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoost<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(itemGetPermanentBoostReal));


		auto itemGetPermanentBoostString = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoostString",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoost<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(itemGetPermanentBoostString));


		auto itemGetAbility = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoostAbility",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Ability", ValueType::String, FunctionArgumentDirection::In },
				{ "Points", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoostAbility
		);
		functionMgr.Register(std::move(itemGetAbility));


		auto itemGetTalent = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoostTalent",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "HasTalent", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoostTalent
		);
		functionMgr.Register(std::move(itemGetTalent));


		auto itemSetPermanentBoostInt = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(itemSetPermanentBoostInt));


		auto itemSetPermanentBoostReal = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoostReal",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoost<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(itemSetPermanentBoostReal));


		auto itemSetPermanentBoostString = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoostString",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoost<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(itemSetPermanentBoostString));


		auto itemSetAbility = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoostAbility",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Ability", ValueType::String, FunctionArgumentDirection::In },
				{ "Points", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoostAbility
		);
		functionMgr.Register(std::move(itemSetAbility));


		auto itemSetTalent = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoostTalent",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "HasTalent", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoostTalent
		);
		functionMgr.Register(std::move(itemSetTalent));
		

		auto itemConstructBegin = std::make_unique<CustomCall>(
			"NRD_ItemConstructBegin",
			std::vector<CustomFunctionParam>{
				{ "TemplateGuid", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::ItemConstructBegin
		);
		functionMgr.Register(std::move(itemConstructBegin));

		auto itemCloneBegin = std::make_unique<CustomCall>(
			"NRD_ItemCloneBegin",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In }
			},
			&func::ItemCloneBegin
		);
		functionMgr.Register(std::move(itemCloneBegin));

		auto itemClone = std::make_unique<CustomQuery>(
			"NRD_ItemClone",
			std::vector<CustomFunctionParam>{
				{ "NewItem", ValueType::ItemGuid, FunctionArgumentDirection::Out }
			},
			&func::ItemClone
		);
		functionMgr.Register(std::move(itemClone));

		auto itemCloneSetInt = std::make_unique<CustomCall>(
			"NRD_ItemCloneSetInt",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ItemCloneSet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(itemCloneSetInt));

		auto itemCloneSetString = std::make_unique<CustomCall>(
			"NRD_ItemCloneSetString",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::ItemCloneSet<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(itemCloneSetString));

		auto itemCloneResetProgression = std::make_unique<CustomCall>(
			"NRD_ItemCloneResetProgression",
			std::vector<CustomFunctionParam>{},
			&func::ItemCloneResetProgression
		);
		functionMgr.Register(std::move(itemCloneResetProgression));

		auto itemCloneAddBoost = std::make_unique<CustomCall>(
			"NRD_ItemCloneAddBoost",
			std::vector<CustomFunctionParam>{
				{ "BoostType", ValueType::String, FunctionArgumentDirection::In },
				{ "Boost", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::ItemCloneAddBoost
		);
		functionMgr.Register(std::move(itemCloneAddBoost));

		auto itemDeltaModIteratorEvent = std::make_unique<CustomEvent>(
			"NRD_ItemDeltaModIteratorEvent",
			std::vector<CustomFunctionParam>{
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "DeltaMod", ValueType::String, FunctionArgumentDirection::In },
				{ "IsGenerated", ValueType::Integer, FunctionArgumentDirection::In },
			}
		);
		ItemDeltaModIteratorEventHandle = functionMgr.Register(std::move(itemDeltaModIteratorEvent));
	}

}
