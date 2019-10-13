#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace osidbg
{
	namespace func
	{
		bool ItemGetStatsId(OsiArgumentDesc & args)
		{
			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->StatsId.Str) {
				OsiError("Item '" << itemGuid << "' has no stats ID!");
				return false;
			} else {
				args.Get(1).String = item->StatsId.Str;
				return true;
			}
		}

		bool ItemGetGenerationParams(OsiArgumentDesc & args)
		{
			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->Generation) {
				OsiError("Item '" << itemGuid << "' has no generation data!");
				return false;
			} else {
				args.Get(1).String = item->Generation->Base.Str ? item->Generation->Base.Str : "";
				args.Get(2).String = item->Generation->ItemType.Str ? item->Generation->ItemType.Str : "";
				args.Get(3).Int32 = item->Generation->Level;
				return true;
			}
		}

		bool ItemHasDeltaModifier(OsiArgumentDesc & args)
		{
			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			auto deltaMod = args.Get(1).String;

			int32_t count = 0;
			if (item->StatsDynamic != nullptr) {
				auto const & boosts = item->StatsDynamic->BoostNameSet;
				for (uint32_t i = 0; i < boosts.Set.Size; i++) {
					if (strcmp(boosts.Set.Buf[i].Str, deltaMod) == 0) {
						count++;
					}
				}
			}

			if (item->Generation != nullptr) {
				auto const & boosts = item->Generation->Boosts;
				for (uint32_t i = 0; i < boosts.Size; i++) {
					if (strcmp(boosts.Buf[i].Str, deltaMod) == 0) {
						count++;
					}
				}
			}

			args.Get(2).Int32 = count;
			return count > 0;
		}

		void ItemSetIdentified(OsiArgumentDesc const & args)
		{
			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return;
			}

			item->StatsDynamic->IsIdentified = args.Get(1).Int32 ? 1 : 0;
		}

		template <OsiPropertyMapType Type>
		bool ItemGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			if (item == nullptr) return false;

			if (item->StatsDynamic == nullptr
				|| item->StatsDynamic->DynamicAttributes_End - item->StatsDynamic->DynamicAttributes_Start < 2) {
				OsiError("Item has no stats!");
				return false;
			}

			auto permanentBoosts = item->StatsDynamic->DynamicAttributes_Start[1];

			switch (permanentBoosts->StatsType) {
			case EquipmentStatsType::Weapon:
			{
				auto * weapon = static_cast<CDivinityStats_Equipment_Attributes_Weapon *>(permanentBoosts);
				return OsirisPropertyMapGet(gEquipmentAttributesWeaponPropertyMap, weapon, args, 1, Type);
			}

			case EquipmentStatsType::Armor:
			{
				auto * armor = static_cast<CDivinityStats_Equipment_Attributes_Armor *>(permanentBoosts);
				return OsirisPropertyMapGet(gEquipmentAttributesArmorPropertyMap, armor, args, 1, Type);
			}

			case EquipmentStatsType::Shield:
			{
				auto * shield = static_cast<CDivinityStats_Equipment_Attributes_Shield *>(permanentBoosts);
				return OsirisPropertyMapGet(gEquipmentAttributesShieldPropertyMap, shield, args, 1, Type);
			}

			default:
				OsiError("Unknown equipment stats type: " << (unsigned)permanentBoosts->StatsType);
				return false;
			}
		}

		template <OsiPropertyMapType Type>
		void ItemSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			if (item == nullptr) return;

			if (item->StatsDynamic == nullptr
				|| item->StatsDynamic->DynamicAttributes_End - item->StatsDynamic->DynamicAttributes_Start < 2) {
				OsiError("Item has no stats!");
				return;
			}

			auto permanentBoosts = item->StatsDynamic->DynamicAttributes_Start[1];

			switch (permanentBoosts->StatsType) {
			case EquipmentStatsType::Weapon:
			{
				auto * weapon = static_cast<CDivinityStats_Equipment_Attributes_Weapon *>(permanentBoosts);
				OsirisPropertyMapSet(gEquipmentAttributesWeaponPropertyMap, weapon, args, 1, Type);
				break;
			}

			case EquipmentStatsType::Armor:
			{
				auto * armor = static_cast<CDivinityStats_Equipment_Attributes_Armor *>(permanentBoosts);
				OsirisPropertyMapSet(gEquipmentAttributesArmorPropertyMap, armor, args, 1, Type);
				break;
			}

			case EquipmentStatsType::Shield:
			{
				auto * shield = static_cast<CDivinityStats_Equipment_Attributes_Shield *>(permanentBoosts);
				OsirisPropertyMapSet(gEquipmentAttributesShieldPropertyMap, shield, args, 1, Type);
				break;
			}

			default:
				OsiError("Unknown equipment stats type: " << (unsigned)permanentBoosts->StatsType);
			}
		}


		std::unique_ptr<ObjectSet<EoCItemDefinition>> gPendingItemClone;


		void ItemCloneBegin(OsiArgumentDesc const & args)
		{
			if (gPendingItemClone) {
				OsiWarn("ItemCloneBegin() called when a clone is already in progress. Previous clone state will be discarded.");
			}

			gPendingItemClone.reset();

			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) return;

			gPendingItemClone = std::make_unique<ObjectSet<EoCItemDefinition>>();
			gOsirisProxy->GetLibraryManager().ParseItem(item, gPendingItemClone.get(), false, false);

			if (gPendingItemClone->Set.Size != 1) {
				OsiError("Something went wrong during item parsing. Item set size: " << gPendingItemClone->Set.Size);
				gPendingItemClone.reset();
				return;
			}
		}


		bool ItemClone(OsiArgumentDesc & args)
		{
			if (!gPendingItemClone) {
				OsiError("No item clone is in progress");
				return false;
			}

			auto item = gOsirisProxy->GetLibraryManager().CreateItemFromParsed(gPendingItemClone.get(), 0);
			gPendingItemClone.reset();

			if (item == nullptr) {
				OsiError("Failed to clone item.");
				return false;
			}

			args.Get(0).String = item->GetGuid()->Str;
			return true;
		}


		template <OsiPropertyMapType Type>
		void ItemCloneSet(OsiArgumentDesc const & args)
		{
			if (!gPendingItemClone) {
				OsiError("No item clone is currently in progress!");
				return;
			}

			OsirisPropertyMapSet(gEoCItemDefinitionPropertyMap, &gPendingItemClone->Set.Buf[0], args, 0, Type);
		}


	}

	void CustomFunctionLibrary::RegisterItemFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto itemGetStatsId = std::make_unique<CustomQuery>(
			"NRD_ItemGetStatsId",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatsId", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::ItemGetStatsId
		);
		functionMgr.Register(std::move(itemGetStatsId));

		auto itemGetGenerationParams = std::make_unique<CustomQuery>(
			"NRD_ItemGetGenerationParams",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Base", ValueType::String, FunctionArgumentDirection::Out },
				{ "ItemType", ValueType::String, FunctionArgumentDirection::Out },
				{ "Level", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::ItemGetGenerationParams
		);
		functionMgr.Register(std::move(itemGetGenerationParams));

		auto itemHasDeltaMod = std::make_unique<CustomQuery>(
			"NRD_ItemHasDeltaModifier",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "DeltaMod", ValueType::String, FunctionArgumentDirection::Out },
				{ "Count", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::ItemHasDeltaModifier
		);
		functionMgr.Register(std::move(itemHasDeltaMod));

		auto itemSetIdentified = std::make_unique<CustomCall>(
			"NRD_ItemSetIdentified",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "IsIdentified", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::ItemSetIdentified
		);
		functionMgr.Register(std::move(itemSetIdentified));


		auto itemGetPermanentBoostInt = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(itemGetPermanentBoostInt));


		auto itemGetPermanentBoostReal = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoostReal",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoost<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(itemGetPermanentBoostReal));


		auto itemSetPermanentBoostInt = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(itemSetPermanentBoostInt));


		auto itemSetPermanentBoostReal = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoostReal",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoost<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(itemSetPermanentBoostReal));


		auto itemCloneBegin = std::make_unique<CustomCall>(
			"NRD_ItemCloneBegin",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::ItemCloneBegin
		);
		functionMgr.Register(std::move(itemCloneBegin));

		auto itemClone = std::make_unique<CustomQuery>(
			"NRD_ItemClone",
			std::vector<CustomFunctionParam>{
				{ "NewItem", ValueType::GuidString, FunctionArgumentDirection::Out }
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
	}

}
