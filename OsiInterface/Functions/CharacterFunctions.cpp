#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace osidbg
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

			if (character->Stats->DynamicStats == nullptr) {
				OsiErrorS("Character has no dynamic stats!");
				return nullptr;
			}

			auto numStats = character->Stats->DynamicStatsEnd - character->Stats->DynamicStats;
			if (numStats <= index) {
				OsiError("Tried to get dynamic stat " << index << ", character only has " << numStats);
				return nullptr;
			}

			return character->Stats->DynamicStats[index];
		}

		bool CharacterGetComputedStat(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			auto statName = args[1].String;
			auto baseStats = args[2].Int32 == 1;
			auto & statValue = args[3].Int32;
			if (character == nullptr || character->Stats == nullptr) return false;

			auto value = character->Stats->GetStat(statName, baseStats);
			if (value) {
				statValue = *value;
				return true;
			} else {
				return false;
			}
		}

		bool CharacterGetHitChance(OsiArgumentDesc & args)
		{
			auto attacker = FindCharacterByNameGuid(args[0].String);
			auto target = FindCharacterByNameGuid(args[1].String);
			auto & hitChance = args[2].Int32;
			if (attacker == nullptr
				|| target == nullptr
				|| attacker->Stats == nullptr
				|| target->Stats == nullptr) {
				return false;
			}

			auto chance = attacker->Stats->GetHitChance(target->Stats);
			if (chance) {
				hitChance = *chance;
				return true;
			} else {
				return false;
			}
		}

		template <OsiPropertyMapType Type>
		bool CharacterGetStat(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			if (character == nullptr || character->Stats == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterStatsPropertyMap, character->Stats, args, 1, Type);
		}

		void CharacterSetStatInt(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			auto stat = args[1].String;
			auto value = args[2].Int32;

			if (character == nullptr || character->Stats == nullptr) return;

			auto clamped = value;
			if (strcmp(stat, "CurrentVitality") == 0) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxVitality);
			} else if (strcmp(stat, "CurrentArmor") == 0) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxArmor);
			} else if (strcmp(stat, "CurrentMagicArmor") == 0) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxMagicArmor);
			}

			gCharacterStatsPropertyMap.setInt(character->Stats, stat, clamped, false, true);
		}

		template <OsiPropertyMapType Type>
		bool CharacterGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			if (character == nullptr) return false;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterDynamicStatPropertyMap, permanentBoosts, args, 1, Type);
		}

		template <OsiPropertyMapType Type>
		void CharacterSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
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

			auto character = FindCharacterByNameGuid(characterGuid);
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
			auto & disabled = args[2].Int32;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) return false;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return false;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return false;
			}

			disabled = permanentBoosts->RemovedTalents.HasTalent(*talentId) ? 1 : 0;
			return true;
		}

		void CharacterDisableTalent(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto talent = args[1].String;
			auto disabled = args[2].Int32;

			auto character = FindCharacterByNameGuid(characterGuid);
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

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) return;

			character->SetGlobal(global);
		}

		template <OsiPropertyMapType Type>
		bool CharacterGet(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterPropertyMap, character, args, 1, Type);
		}

		void CharacterIterateSkills(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto eventName = args[1].String;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr || character->SkillManager == nullptr) return;

			auto & skills = character->SkillManager->Skills;
			skills.Iterate([&characterGuid, &eventName](FixedString const & skillId, esv::Skill * skill) {
				auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
				eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, characterGuid });
				eventArgs->Add(OsiArgumentValue{ ValueType::String, skill->SkillId.Str });
				eventArgs->Add(OsiArgumentValue{ (int32_t)skill->IsLearned });
				eventArgs->Add(OsiArgumentValue{ (int32_t)skill->IsActivated });

				gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(SkillIteratorEventHandle, eventArgs);

				delete eventArgs;
			});
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
				{ "IsBaseStat", ValueType::Integer, FunctionArgumentDirection::In },
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
	}

}
