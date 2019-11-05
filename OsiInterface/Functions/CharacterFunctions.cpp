#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"
#include <locale>
#include <codecvt>

namespace osidbg
{
	namespace func
	{
		CharacterDynamicStat * GetCharacterDynamicStat(esv::Character * character, uint32_t index)
		{
			if (character->Stats == nullptr) {
				OsiError("Character has no stats!");
				return nullptr;
			}

			if (character->Stats->DynamicStats == nullptr) {
				OsiError("Character has no dynamic stats!");
				return nullptr;
			}

			auto numStats = character->Stats->DynamicStatsEnd - character->Stats->DynamicStats;
			if (numStats <= index) {
				OsiError("Tried to get dynamic stat " << index << ", character only has " << numStats);
				return nullptr;
			}

			return character->Stats->DynamicStats[index];
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

			disabled = permanentBoosts->IsTalentRemoved(*talentId) ? 1 : 0;
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

			permanentBoosts->RemoveTalent(*talentId, disabled != 0);
		}

		esv::CustomStatDefinitionComponent * FindCustomStatDefinition(char const * name)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wstrName = converter.from_bytes(name);

			auto entityWorld = GetEntityWorld();
			auto sys = entityWorld->SystemTypes.Buf[(uint32_t)SystemType::CustomStat].System;
			auto statSystem = (esv::CustomStatSystem *)((uint8_t *)sys - 0x18);

			for (uint32_t i = 0; i < statSystem->CustomStatDefinitionHandles.Set.Size; i++) {
				auto handle = statSystem->CustomStatDefinitionHandles.Set.Buf[i].Handle;
				auto component = FindComponentByHandle(ComponentType::CustomStatDefinition, handle);
				if (component != nullptr) {
					auto statDefn = (esv::CustomStatDefinitionComponent *)((uint8_t *)component - 80);
					if (wcscmp(statDefn->Name.GetPtr(), wstrName.c_str()) == 0) {
						return statDefn;
					}
				}
			}

			OsiError("Could not find custom stat definition '" << name << "'");
			return nullptr;
		}

		bool CharacterGetCustomStat(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			auto statName = args[1].String;
			auto & statValue = args[2].Int32;

			auto statDefn = FindCustomStatDefinition(statName);
			if (statDefn == nullptr) return false;

			auto entityWorld = GetEntityWorld();
			auto statsComponent = (eoc::CustomStatsComponent *)entityWorld->GetComponent(
				character->Base.EntityObjectHandle, ComponentType::CustomStats);
			if (statsComponent == nullptr) {
				OsiError("Character has no CustomStatsComponent");
				return false;
			}

			auto value = statsComponent->StatValues.Find(statDefn->Id.Str);
			if (value == nullptr) {
				OsiWarn("Character has no custom stat named '" << statName << "'");
				return false;
			}

			statValue = *value;
			return true;
		}
	}

	void CustomFunctionLibrary::RegisterCharacterFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

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


		auto characterGetCustomStat = std::make_unique<CustomQuery>(
			"NRD_CharacterGetCustomStat",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetCustomStat
		);
		functionMgr.Register(std::move(characterGetCustomStat));
	}

}
