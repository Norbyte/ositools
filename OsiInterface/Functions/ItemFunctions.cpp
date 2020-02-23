#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include <GameDefinitions/Symbols.h>
#include "PropertyMaps.h"

namespace osidbg
{
	FunctionHandle ItemDeltaModIteratorEventHandle;

	namespace func
	{
		bool ItemGetStatsId(OsiArgumentDesc & args)
		{
			auto itemGuid = args[0].String;
			auto item = FindItemByNameGuid(itemGuid);
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
			auto item = FindItemByNameGuid(itemGuid);
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
			auto item = FindItemByNameGuid(itemGuid);
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
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			auto deltaMod = args[1].String;

			int32_t count = 0;
			if (item->StatsDynamic != nullptr) {
				auto const & boosts = item->StatsDynamic->BoostNameSet;
				for (uint32_t i = 0; i < boosts.Set.Size; i++) {
					if (strcmp(boosts[i].Str, deltaMod) == 0) {
						count++;
					}
				}
			}

			if (item->Generation != nullptr) {
				auto const & boosts = item->Generation->Boosts;
				for (uint32_t i = 0; i < boosts.Set.Size; i++) {
					if (strcmp(boosts[i].Str, deltaMod) == 0) {
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

			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) return;

			if (item->Generation != nullptr) {
				auto const & boosts = item->Generation->Boosts;
				for (uint32_t i = 0; i < boosts.Set.Size; i++) {
					auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
					eventArgs->Add(OsiArgumentValue{ ValueType::ItemGuid, itemGuid });
					eventArgs->Add(OsiArgumentValue{ ValueType::String, boosts[i].Str });
					eventArgs->Add(OsiArgumentValue{ 1 });
					gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(ItemDeltaModIteratorEventHandle, eventArgs);
					delete eventArgs;
				}
			}

			if (item->StatsDynamic != nullptr) {
				auto const & boosts = item->StatsDynamic->BoostNameSet;
				for (uint32_t i = 0; i < boosts.Set.Size; i++) {
					auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
					eventArgs->Add(OsiArgumentValue{ ValueType::ItemGuid, itemGuid });
					eventArgs->Add(OsiArgumentValue{ ValueType::String, boosts[i].Str });
					eventArgs->Add(OsiArgumentValue{ 0 });
					gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(ItemDeltaModIteratorEventHandle, eventArgs);
					delete eventArgs;
				}
			}
		}

		void ItemSetIdentified(OsiArgumentDesc const & args)
		{
			auto itemGuid = args[0].String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return;
			}

			if (item->StatsDynamic == nullptr) {
				OsiError("Item '" << itemGuid << "' has no dynamic stats!");
				return;
			}

			item->StatsDynamic->IsIdentified = args[1].Int32 ? 1 : 0;
		}

		CDivinityStats_Equipment_Attributes * GetItemDynamicStat(esv::Item * item, uint32_t index)
		{
			if (item->StatsDynamic == nullptr) {
				OsiErrorS("Item has no dynamic stats!");
				return nullptr;
			}

			auto numStats = item->StatsDynamic->DynamicAttributes_End - item->StatsDynamic->DynamicAttributes_Start;
			if (numStats <= 1) {
				OsiError("Tried to get dynamic stat " << index << ", item only has " << numStats);
				return nullptr;
			}

			return item->StatsDynamic->DynamicAttributes_Start[index];
		}

		template <OsiPropertyMapType Type>
		bool ItemGet(OsiArgumentDesc & args)
		{
			auto item = FindItemByNameGuid(args[0].String);
			if (item == nullptr) return false;

			bool fetched = false;
			if (item->StatsDynamic != nullptr) {
				fetched = OsirisPropertyMapGet(gItemStatsPropertyMap, item->StatsDynamic, args, 1, Type, false);
			}

			if (!fetched) {
				fetched = OsirisPropertyMapGet(gItemPropertyMap, item, args, 1, Type, true);
			}

			return fetched;
		}

		template <OsiPropertyMapType Type>
		bool ItemGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto item = FindItemByNameGuid(args[0].String);
			if (item == nullptr) return false;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return false;

			auto & propertyMap = permanentBoosts->GetPropertyMap();
			return OsirisPropertyMapGetRaw(propertyMap, permanentBoosts, args, 1, Type);
		}

		template <OsiPropertyMapType Type>
		void ItemSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto item = FindItemByNameGuid(args[0].String);
			if (item == nullptr) return;

			auto permanentBoosts = GetItemDynamicStat(item, 1);
			if (permanentBoosts == nullptr) return;

			auto & propertyMap = permanentBoosts->GetPropertyMap();
			OsirisPropertyMapSetRaw(propertyMap, permanentBoosts, args, 1, Type);
		}


		void ItemCloneBegin(OsiArgumentDesc const & args)
		{
			if (ExtensionState::Get().PendingItemClone) {
				OsiWarn("ItemCloneBegin() called when a clone is already in progress. Previous clone state will be discarded.");
			}

			auto parseItem = GetStaticSymbols().ParseItem;
			auto createItemFromParsed = GetStaticSymbols().CreateItemFromParsed;
			if (parseItem == nullptr || createItemFromParsed == nullptr) {
				OsiErrorS("esv::ParseItem not found!");
				return;
			}
			
			ExtensionState::Get().PendingItemClone.reset();

			auto itemGuid = args[0].String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) return;

			auto & clone = ExtensionState::Get().PendingItemClone;
			clone = std::make_unique<ObjectSet<eoc::ItemDefinition>>();
			parseItem(item, clone.get(), false, false);

			if (clone->Set.Size != 1) {
				OsiError("Something went wrong during item parsing. Item set size: " << clone->Set.Size);
				clone.reset();
				return;
			}
		}


		bool ItemClone(OsiArgumentDesc & args)
		{
			if (!ExtensionState::Get().PendingItemClone) {
				OsiErrorS("No item clone is in progress");
				return false;
			}

			auto & clone = ExtensionState::Get().PendingItemClone;
			auto item = GetStaticSymbols().CreateItemFromParsed(clone.get(), 0);
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


		void ItemCloneAddBoost(OsiArgumentDesc const & args)
		{
			auto & clone = ExtensionState::Get().PendingItemClone;
			if (!clone) {
				OsiErrorS("No item clone is currently in progress!");
				return;
			}

			auto boostType = args[0].String;
			auto boostName = ToFixedString(args[1].String);

			if (!boostName) {
				OsiError("Not a valid boost name: " << args[1].String);
				return;
			}

			auto & defn = (*clone)[0];
			if (strcmp(boostType, "Generation") == 0) {
				defn.GenerationBoosts.Set.Add(boostName);
			} else if (strcmp(boostType, "DeltaMod") == 0) {
				defn.DeltaMods.Set.Add(boostName);
			} else if (strcmp(boostType, "Rune") == 0) {
				for (uint32_t i = 0; i < defn.RuneBoosts.Set.Size; i++) {
					if (!*defn.RuneBoosts[i].Str) {
						defn.RuneBoosts[i] = boostName;
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
