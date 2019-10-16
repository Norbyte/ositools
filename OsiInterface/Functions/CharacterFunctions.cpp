#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace osidbg
{
	namespace func
	{
		template <OsiPropertyMapType Type>
		bool CharacterGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) return false;

			if (character->Stats == nullptr
				|| character->Stats->DynamicStats == nullptr
				|| character->Stats->DynamicStatsEnd - character->Stats->DynamicStats < 2) {
				OsiError("Character has no permanent boost stats!");
				return false;
			}

			auto permanentBoosts = character->Stats->DynamicStats[1];
			return OsirisPropertyMapGet(gCharacterDynamicStatPropertyMap, permanentBoosts, args, 1, Type);
		}

		template <OsiPropertyMapType Type>
		void CharacterSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) return;

			if (character->Stats == nullptr
				|| character->Stats->DynamicStats == nullptr
				|| character->Stats->DynamicStatsEnd - character->Stats->DynamicStats < 2) {
				OsiError("Character has no permanent boost stats!");
				return;
			}

			auto permanentBoosts = character->Stats->DynamicStats[1];
			OsirisPropertyMapSet(gCharacterDynamicStatPropertyMap, permanentBoosts, args, 1, Type);
		}
	}

	void CustomFunctionLibrary::RegisterCharacterFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

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
	}

}
