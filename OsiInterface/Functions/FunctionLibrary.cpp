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
				OsiError("ItemGetStatsId(): Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->StatsId.Str) {
				OsiError("ItemGetStatsId(): Item '" << itemGuid << "' has no stats ID!");
				return false;
			}
			else {
				args.Get(1).String = const_cast<char *>(item->StatsId.Str);
				return true;
			}
		}

		void ItemSetIdentified(OsiArgumentDesc const & args)
		{
			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("ItemSetIdentified(): Item '" << itemGuid << "' does not exist!");
				return;
			}

			item->StatsDynamic->IsIdentified = args.Get(1).Int32 ? 1 : 0;
		}

		void SkillSetCooldown(OsiArgumentDesc const & args)
		{
			auto characterGuid = args.Get(0).String;
			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				OsiError("SkillSetCooldown(): Character '" << characterGuid << "' does not exist!");
				return;
			}

			if (character->SkillManager == nullptr) {
				OsiError("SkillSetCooldown(): Character '" << characterGuid << "' has no SkillManager!");
				return;
			}

			auto skillId = args.Get(1).String;
			auto skill = character->SkillManager->Skills.Find(skillId);
			if (skill == nullptr) {
				OsiError("SkillSetCooldown(): Character '" << characterGuid << "' doesn't have skill '" << skillId << "'!");
				return;
			}

			if ((*skill)->OncePerCombat) {
				OsiError("SkillSetCooldown(): Skill '" << skillId << " doesn't support cooldown!");
				return;
			}

			auto cooldown = args.Get(2).Float;
			if (cooldown < 0.0f) {
				OsiError("SkillSetCooldown(): Cooldown cannot be negative!");
				return;
			}

			(*skill)->ActiveCooldown = cooldown;
		}

		void BreakOnCharacter(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			OsiWarn("GotIt");
		}

		void BreakOnItem(OsiArgumentDesc const & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			OsiWarn("GotIt");
		}

		void DoExperiment(OsiArgumentDesc const & args)
		{
			OsiError("Nothing to see here");
		}
	}

	CustomFunctionLibrary::CustomFunctionLibrary(class OsirisProxy & osiris)
		: osiris_(osiris)
	{}

	void CustomFunctionLibrary::Register()
	{
		RegisterHelperFunctions();
		RegisterStatFunctions();
		RegisterStatusFunctions();
		RegisterGameActionFunctions();
		RegisterProjectileFunctions();

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


		auto skillSetCooldown = std::make_unique<CustomCall>(
			"NRD_SkillSetCooldown",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "Cooldown", ValueType::Real, FunctionArgumentDirection::In }
			},
			&func::SkillSetCooldown
		);
		functionMgr.Register(std::move(skillSetCooldown));


		auto breakOnCharacter = std::make_unique<CustomCall>(
			"NRD_BreakOnCharacter",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::BreakOnCharacter
		);
		functionMgr.Register(std::move(breakOnCharacter));


		auto breakOnItem = std::make_unique<CustomCall>(
			"NRD_BreakOnItem",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::BreakOnItem
		);
		functionMgr.Register(std::move(breakOnItem));


		auto experiment = std::make_unique<CustomCall>(
			"NRD_Experiment",
			std::vector<CustomFunctionParam>{
				{ "Experiment", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::DoExperiment
		);
		functionMgr.Register(std::move(experiment));
	}

}
