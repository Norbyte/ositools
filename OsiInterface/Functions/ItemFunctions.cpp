#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

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
			}
			else {
				args.Get(1).String = item->StatsId.Str;
				return true;
			}
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

#define STATDEF(name) if (strcmp(stat, #name) == 0) { return &attributes.name; }
#define STATOBJ(obj, name) if (strcmp(stat, #name) == 0) { return &obj.name; }

		int32_t * StatAttributeToPtr(CDivinityStats_Equipment_Attributes & attributes, char const * stat)
		{
			STATDEF(Durability);
			STATDEF(DurabilityDegradeSpeed);
			STATDEF(StrengthBoost);
			STATDEF(FinesseBoost);
			STATDEF(IntelligenceBoost);
			STATDEF(ConstitutionBoost);
			STATDEF(Memory);
			STATDEF(WitsBoost);
			STATDEF(SightBoost);
			STATDEF(HearingBoost);
			STATDEF(VitalityBoost);
			STATDEF(SourcePointsBoost);
			STATDEF(MaxAP);
			STATDEF(StartAP);
			STATDEF(APRecovery);
			STATDEF(AccuracyBoost);
			STATDEF(DodgeBoost);
			STATDEF(LifeSteal);
			STATDEF(CriticalChance);
			STATDEF(ChanceToHitBoost);
			STATDEF(MovementSpeedBoost);
			STATDEF(RuneSlots);
			STATDEF(RuneSlots_V1);
			STATDEF(FireResistance);
			STATDEF(AirResistance);
			STATDEF(WaterResistance);
			STATDEF(EarthResistance);
			STATDEF(PoisonResistance);
			STATDEF(TenebriumResistance);
			STATDEF(PiercingResistance);
			STATDEF(CorrosiveResistance);
			STATDEF(PhysicalResistance);
			STATDEF(MagicResistance);
			STATDEF(CustomResistance);
			STATDEF(Movement);
			STATDEF(Initiative);
			STATDEF(Willpower);
			STATDEF(Bodybuilding);
			STATDEF(MaxSummons);
			STATDEF(Value);
			STATDEF(Weight);

			// TODO - ObjectInstanceName?, Skills, AbilityModifiers, Talents

			switch (attributes.EquipmentStatsType) {
				case CDivinityStats_Equipment_Attributes::ST_Weapon:
				{
					auto & weapon = static_cast<CDivinityStats_Equipment_Attributes_Weapon &>(attributes);
					STATOBJ(weapon, DamageType);
					STATOBJ(weapon, MinDamage);
					STATOBJ(weapon, MaxDamage);
					STATOBJ(weapon, DamageBoost);
					STATOBJ(weapon, DamageFromBase);
					STATOBJ(weapon, CriticalDamage);
					// FIXME - WeaponRange is a float
					// STATOBJ(weapon, WeaponRange);
					STATOBJ(weapon, CleaveAngle);
					// FIXME - CleavePercentage is a float
					// STATOBJ(weapon, CleavePercentage);
					STATOBJ(weapon, AttackAPCost);
					break;
				}

				case CDivinityStats_Equipment_Attributes::ST_Armor:
				{
					auto & armor = static_cast<CDivinityStats_Equipment_Attributes_Armor &>(attributes);
					STATOBJ(armor, ArmorValue);
					STATOBJ(armor, ArmorBoost);
					STATOBJ(armor, MagicArmorValue);
					STATOBJ(armor, MagicArmorBoost);
					break;
				}

				case CDivinityStats_Equipment_Attributes::ST_Shield:
				{
					auto & shield = static_cast<CDivinityStats_Equipment_Attributes_Shield &>(attributes);
					STATOBJ(shield, ArmorValue);
					STATOBJ(shield, ArmorBoost);
					STATOBJ(shield, MagicArmorValue);
					STATOBJ(shield, MagicArmorBoost);
					STATOBJ(shield, Blocking);
					break;
				}

				default:
					OsiError("Unknown equipment stats type: " << attributes.EquipmentStatsType);
					break;
			}

			OsiError("CDivinityStats_Equipment_Attributes has no property named '" << stat << '"');
			return nullptr;
		}

		bool ItemGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			if (item == nullptr) return false;

			if (item->StatsDynamic == nullptr
				|| item->StatsDynamic->DynamicAttributes_End - item->StatsDynamic->DynamicAttributes_Start < 2) {
				OsiError("Item has no stats!");
				return false;
			}

			auto & permanentBoosts = *item->StatsDynamic->DynamicAttributes_Start[1];
			// auto stats0 = item->StatsDynamic->DynamicAttributes_Start[0];

			auto statName = args.Get(1).String;
			auto value = StatAttributeToPtr(permanentBoosts, statName);
			if (value == nullptr) return false;

			args.Get(2).Int32 = *value;
			return true;
		}

		void ItemSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			if (item == nullptr) return;

			if (item->StatsDynamic == nullptr
				|| item->StatsDynamic->DynamicAttributes_End - item->StatsDynamic->DynamicAttributes_Start < 2) {
				OsiError("Item has no stats!");
				return;
			}

			auto & stats = *item->StatsDynamic->DynamicAttributes_Start[0];
			auto & permanentBoosts = *item->StatsDynamic->DynamicAttributes_Start[1];

			auto statName = args.Get(1).String;
			auto value = StatAttributeToPtr(permanentBoosts, statName);
			if (value == nullptr) return;

			auto statValue = StatAttributeToPtr(stats, statName);

			statValue -= *value;
			statValue += args.Get(2).Int32;
			*value = args.Get(2).Int32;
		}

		void ItemAddPermanentBoost(OsiArgumentDesc const & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			if (item == nullptr) return;

			if (item->StatsDynamic == nullptr
				|| item->StatsDynamic->DynamicAttributes_End - item->StatsDynamic->DynamicAttributes_Start < 2) {
				OsiError("Item has no stats!");
				return;
			}

			auto & stats = *item->StatsDynamic->DynamicAttributes_Start[0];
			auto & permanentBoosts = *item->StatsDynamic->DynamicAttributes_Start[1];

			auto statName = args.Get(1).String;
			auto value = StatAttributeToPtr(permanentBoosts, statName);
			if (value == nullptr) return;

			auto statValue = StatAttributeToPtr(stats, statName);

			statValue += args.Get(2).Int32;
			*value += args.Get(2).Int32;
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


		auto itemSetIdentified = std::make_unique<CustomCall>(
			"NRD_ItemSetIdentified",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "IsIdentified", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::ItemSetIdentified
		);
		functionMgr.Register(std::move(itemSetIdentified));

		auto itemGetPermanentBoost = std::make_unique<CustomQuery>(
			"NRD_ItemGetPermanentBoost",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ItemGetPermanentBoost
		);
		functionMgr.Register(std::move(itemGetPermanentBoost));


		auto itemSetPermanentBoost = std::make_unique<CustomCall>(
			"NRD_ItemSetPermanentBoost",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ItemSetPermanentBoost
		);
		functionMgr.Register(std::move(itemSetPermanentBoost));


		auto itemAddPermanentBoost = std::make_unique<CustomCall>(
			"NRD_ItemAddPermanentBoost",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Delta", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ItemAddPermanentBoost
		);
		functionMgr.Register(std::move(itemAddPermanentBoost));
	}

}
