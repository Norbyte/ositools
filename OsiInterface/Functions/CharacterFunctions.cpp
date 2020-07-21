#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace dse::esv
{
	FunctionHandle SkillIteratorEventHandle;

	namespace func
	{
		CharacterDynamicStat * GetCharacterDynamicStat(esv::Character * character, uint32_t index)
		{
			if (character->Stats == nullptr) {
				OsiErrorS("Character has no stats!");
				return nullptr;
			}

			if (character->Stats->DynamicStats.empty()) {
				OsiErrorS("Character has no dynamic stats!");
				return nullptr;
			}

			auto numStats = character->Stats->DynamicStats.size();
			if ((uint32_t)numStats <= index) {
				OsiError("Tried to get dynamic stat " << index << ", character only has " << numStats);
				return nullptr;
			}

			return character->Stats->DynamicStats[index];
		}

		bool CharacterGetComputedStat(OsiArgumentDesc & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			auto statName = args[1].String;
			auto baseStats = args[2].Int32 == 1;
			auto & statValue = args[3];
			if (character == nullptr || character->Stats == nullptr) return false;

			auto value = character->Stats->GetStat(ToFixedString(statName), baseStats);
			if (value) {
				statValue.Set(*value);
				return true;
			} else {
				return false;
			}
		}

		bool CharacterGetHitChance(OsiArgumentDesc & args)
		{
			auto attacker = GetEntityWorld()->GetCharacter(args[0].String);
			auto target = GetEntityWorld()->GetCharacter(args[1].String);
			auto & hitChance = args[2];
			if (attacker == nullptr
				|| target == nullptr
				|| attacker->Stats == nullptr
				|| target->Stats == nullptr) {
				return false;
			}

			auto chance = attacker->Stats->GetHitChance(target->Stats);
			if (chance) {
				hitChance.Set(*chance);
				return true;
			} else {
				return false;
			}
		}

		template <OsiPropertyMapType Type>
		bool CharacterGetStat(OsiArgumentDesc & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			if (character == nullptr || character->Stats == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterStatsPropertyMap, character->Stats, args, 1, Type);
		}

		void CharacterSetStatInt(OsiArgumentDesc const & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			auto stat = ToFixedString(args[1].String);
			auto value = args[2].Int32;

			if (character == nullptr || character->Stats == nullptr) return;

			auto clamped = value;
			if (stat == GFS.strCurrentVitality) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxVitality);
			} else if (stat == GFS.strCurrentArmor) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxArmor);
			} else if (stat == GFS.strCurrentMagicArmor) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxMagicArmor);
			}

			gCharacterStatsPropertyMap.setInt(character->Stats, stat, clamped, false, true);
		}

		template <OsiPropertyMapType Type>
		bool CharacterGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			if (character == nullptr) return false;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterDynamicStatPropertyMap, permanentBoosts, args, 1, Type);
		}

		template <OsiPropertyMapType Type>
		void CharacterSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			if (character == nullptr) return;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return;

			OsirisPropertyMapSet(gCharacterDynamicStatPropertyMap, permanentBoosts, args, 1, Type);
		}

		void CharacterSetPermanentBoostTalent(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto talent = args[1].String;
			auto enabled = args[2].Int32;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) return;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return;
			}

			permanentBoosts->Talents.Toggle(*talentId, enabled != 0);
		}

		bool CharacterIsTalentDisabled(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto talent = args[1].String;
			auto & disabled = args[2];

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) return false;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return false;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return false;
			}

			disabled.Set(permanentBoosts->RemovedTalents.HasTalent(*talentId) ? 1 : 0);
			return true;
		}

		void CharacterDisableTalent(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto talent = args[1].String;
			auto disabled = args[2].Int32;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) return;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return;
			}

			permanentBoosts->RemovedTalents.Toggle(*talentId, disabled != 0);
			character->Stats->DisabledTalents.Toggle(*talentId, disabled != 0);
		}

		void CharacterSetGlobal(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto global = args[1].Int32 == 1;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) return;

			character->SetGlobal(global);
		}

		template <OsiPropertyMapType Type>
		bool CharacterGet(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterPropertyMap, character, args, 1, Type);
		}

		void CharacterIterateSkills(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto eventName = args[1].String;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr || character->SkillManager == nullptr) return;

			std::vector<std::tuple<char const*, bool, bool>> skillEvents;

			auto & skills = character->SkillManager->Skills;
			skills.Iterate([&characterGuid, &eventName, &skillEvents](FixedString const & skillId, esv::Skill * skill) {
				skillEvents.push_back(std::tuple(skill->SkillId.Str, skill->IsLearned, skill->IsActivated));
			});

			for (auto const& skill : skillEvents) {
				auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
				eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, characterGuid });
				eventArgs->Add(OsiArgumentValue{ ValueType::String, std::get<0>(skill) });
				eventArgs->Add(OsiArgumentValue{ (int32_t)std::get<1>(skill) });
				eventArgs->Add(OsiArgumentValue{ (int32_t)std::get<2>(skill) });

				gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(SkillIteratorEventHandle, eventArgs);

				delete eventArgs;
			}
		}

		void CharacterEquipItem(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto itemGuid = args[1].String;
			auto slotName = args[2].String;
			auto consumeAP = args[3].Int32 > 0;
			auto checkRequirements = args[4].Int32 > 0;
			auto updateVitality = args[5].Int32 > 0;
			auto useWeaponAnimType = args[6].Int32 > 0;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr || !character->InventoryHandle) return;

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) return;

			int16_t slotIndex = -1;
			if (*slotName) {
				auto slot = EnumInfo<ItemSlot>::Find(slotName);
				if (!slot) {
					OsiError("Cannot equip item to invalid slot: " << slotName);
					return;
				}

				slotIndex = (int16_t)*slot;
			}

			auto inventory = FindInventoryByHandle(character->InventoryHandle);
			if (inventory == nullptr) return;

			ObjectHandle itemHandle;
			item->GetObjectHandle(itemHandle);

			auto equipProc = GetStaticSymbols().InventoryEquip;
			equipProc(inventory, itemHandle.Handle, consumeAP, slotIndex, true, checkRequirements,
				updateVitality, useWeaponAnimType);
		}

		bool ObjectGetInternalFlag(OsiArgumentDesc & args)
		{
			auto guid = args[0].String;
			auto flag = args[1].Int32;
			auto & value = args[2];

			if (flag < 0 || flag >= 88) {
				OsiErrorS("Only flag values between 0..87 are supported.");
				return false;
			}

			auto character = GetEntityWorld()->GetCharacter(guid, false);
			if (character != nullptr) {
				if (flag < 64) {
					value.Set(character->HasFlag(1ull << flag));
				} else if (flag < 72) {
					value.Set(((uint64_t)character->Flags2 & (1ull << (flag - 64))) != 0);
				} else if (flag < 80) {
					value.Set(((uint64_t)character->Flags3 & (1ull << (flag - 70))) != 0);
				} else if (flag < 88) {
					value.Set((character->FlagsEx & (1 << (flag - 78))) != 0);
				}

				return true;
			}

			auto item = GetEntityWorld()->GetItem(guid);
			if (item != nullptr) {
				if (flag < 64) {
					value.Set(item->HasFlag(1ull << flag));
				} else if (flag < 72) {
					value.Set(((uint8_t)item->Flags2 & (1 << (flag - 64))) != 0);
				}

				return true;
			} else {
				return false;
			}
		}

		void ObjectSetInternalFlag(OsiArgumentDesc const & args)
		{
			auto guid = args[0].String;
			auto flag = args[1].Int32;
			auto value = args[2].Int32 > 0;

			if (flag < 0 || flag >= 88) {
				OsiErrorS("Only flag values between 0..87 are supported.");
				return;
			}

			auto character = GetEntityWorld()->GetCharacter(guid, false);
			if (character != nullptr) {
				if (flag < 64) {
					if (value) {
						character->SetFlags(1ull << flag);
					} else {
						character->ClearFlags(1ull << flag);
					}
				} else if (flag < 72) {
					if (value) {
						character->Flags2 |= (esv::CharacterFlags2)(1 << (flag - 64));
					} else {
						character->Flags2 &= (esv::CharacterFlags2)~(1 << (flag - 64));
					}
				} else if (flag < 80) {
					if (value) {
						character->Flags3 |= (esv::CharacterFlags3)(1 << (flag - 72));
					} else {
						character->Flags3 &= (esv::CharacterFlags3)~(1 << (flag - 72));
					}
				} else if (flag < 88) {
					if (value) {
						character->FlagsEx |= (1 << (flag - 80));
					} else {
						character->FlagsEx &= ~(1 << (flag - 80));
					}
				}

				return;
			}

			auto item = GetEntityWorld()->GetItem(guid);
			if (item != nullptr) {
				if (flag < 64) {
					if (value) {
						item->SetFlags(1ull << flag);
					} else {
						item->ClearFlags(1ull << flag);
					}
				} else if (flag < 72) {
					if (value) {
						item->Flags2 |= (esv::ItemFlags2)(1 << (flag - 64));
					} else {
						item->Flags2 &= (esv::ItemFlags2)~(1 << (flag - 64));
					}
				}

				return;
			}
		}

		template <OsiPropertyMapType Type>
		bool RootTemplateGet(OsiArgumentDesc& args)
		{
			auto guid = args[0].String;
			auto property = args[1].Int32;
			auto& value = args[2];

			auto character = GetEntityWorld()->GetCharacter(guid, false);
			if (character != nullptr) {
				return OsirisPropertyMapGet(gCharacterTemplatePropertyMap, character->CurrentTemplate, args, 1, Type);
			}

			auto item = GetEntityWorld()->GetItem(guid);
			if (item != nullptr) {
				return OsirisPropertyMapGet(gItemTemplatePropertyMap, item->CurrentTemplate, args, 1, Type);
			} else {
				return false;
			}
		}
	}

	void CustomFunctionLibrary::RegisterCharacterFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto characterGetComputedStat = std::make_unique<CustomQuery>(
			"NRD_CharacterGetComputedStat",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "ExcludeBoosts", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetComputedStat
		);
		functionMgr.Register(std::move(characterGetComputedStat));

		auto characterGetHitChance = std::make_unique<CustomQuery>(
			"NRD_CharacterGetHitChance",
			std::vector<CustomFunctionParam>{
				{ "Attacker", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Target", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "HitChance", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetHitChance
		);
		functionMgr.Register(std::move(characterGetHitChance));

		auto characterGetStatInt = std::make_unique<CustomQuery>(
			"NRD_CharacterGetStatInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetStat<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(characterGetStatInt));

		auto characterGetStatString = std::make_unique<CustomQuery>(
			"NRD_CharacterGetStatString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetStat<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(characterGetStatString));

		auto characterSetStatInt = std::make_unique<CustomCall>(
			"NRD_CharacterSetStatInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterSetStatInt
		);
		functionMgr.Register(std::move(characterSetStatInt));


		auto characterGetPermanentBoostInt = std::make_unique<CustomQuery>(
			"NRD_CharacterGetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(characterGetPermanentBoostInt));


		auto characterSetPermanentBoostInt = std::make_unique<CustomCall>(
			"NRD_CharacterSetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterSetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(characterSetPermanentBoostInt));


		auto characterSetTalent = std::make_unique<CustomCall>(
			"NRD_CharacterSetPermanentBoostTalent",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "HasTalent", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterSetPermanentBoostTalent
		);
		functionMgr.Register(std::move(characterSetTalent));


		auto characterIsTalentDisabled = std::make_unique<CustomQuery>(
			"NRD_CharacterIsTalentDisabled",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "IsDisabled", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterIsTalentDisabled
		);
		functionMgr.Register(std::move(characterIsTalentDisabled));


		auto characterDisableTalent = std::make_unique<CustomCall>(
			"NRD_CharacterDisableTalent",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "IsDisabled", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterDisableTalent
		);
		functionMgr.Register(std::move(characterDisableTalent));


		auto characterSetGlobal = std::make_unique<CustomCall>(
			"NRD_CharacterSetGlobal",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "IsGlobal", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterSetGlobal
		);
		functionMgr.Register(std::move(characterSetGlobal));

		auto characterGetInt = std::make_unique<CustomQuery>(
			"NRD_CharacterGetInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(characterGetInt));

		auto characterGetReal = std::make_unique<CustomQuery>(
			"NRD_CharacterGetReal",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::CharacterGet<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(characterGetReal));
		
		auto characterGetString = std::make_unique<CustomQuery>(
			"NRD_CharacterGetString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::CharacterGet<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(characterGetString));
		
		auto iterateCharacterSkills = std::make_unique<CustomCall>(
			"NRD_CharacterIterateSkills",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Event", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::CharacterIterateSkills
		);
		functionMgr.Register(std::move(iterateCharacterSkills));

		auto skillIteratorEvent = std::make_unique<CustomEvent>(
			"NRD_SkillIteratorEvent",
			std::vector<CustomFunctionParam>{
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "IsLearned", ValueType::Integer, FunctionArgumentDirection::In },
				{ "IsActivated", ValueType::Integer, FunctionArgumentDirection::In },
			}
		);
		SkillIteratorEventHandle = functionMgr.Register(std::move(skillIteratorEvent));

		auto characterEquipItem = std::make_unique<CustomCall>(
			"NRD_CharacterEquipItem",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Slot", ValueType::String, FunctionArgumentDirection::In },
				{ "ConsumeAP", ValueType::Integer, FunctionArgumentDirection::In },
				{ "CheckRequirements", ValueType::Integer, FunctionArgumentDirection::In },
				{ "UpdateVitality", ValueType::Integer, FunctionArgumentDirection::In },
				{ "UseWeaponAnimType", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::CharacterEquipItem
		);
		functionMgr.Register(std::move(characterEquipItem));

		auto objectGetInternalFlag = std::make_unique<CustomQuery>(
			"NRD_ObjectGetInternalFlag",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Flag", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ObjectGetInternalFlag
		);
		functionMgr.Register(std::move(objectGetInternalFlag));

		auto objectSetInternalFlag = std::make_unique<CustomCall>(
			"NRD_ObjectSetInternalFlag",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Flag", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ObjectSetInternalFlag
		);
		functionMgr.Register(std::move(objectSetInternalFlag));

		auto rootTemplateGetInt = std::make_unique<CustomQuery>(
			"NRD_RootTemplateGetInt",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::RootTemplateGet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(rootTemplateGetInt));

		auto rootTemplateGetReal = std::make_unique<CustomQuery>(
			"NRD_RootTemplateGetReal",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::RootTemplateGet<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(rootTemplateGetReal));

		auto rootTemplateGetString = std::make_unique<CustomQuery>(
			"NRD_RootTemplateGetString",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::RootTemplateGet<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(rootTemplateGetString));
	}

}
