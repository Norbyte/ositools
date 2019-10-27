#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

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

		template <OsiPropertyMapType Type>
		void CharacterSetStat(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			if (character == nullptr || character->Stats == nullptr) return;

			OsirisPropertyMapSet(gCharacterStatsPropertyMap, character->Stats, args, 1, Type);
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
			&func::CharacterSetStat<OsiPropertyMapType::Integer>
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
	}

}
