#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include <fstream>

namespace osidbg
{

	void LuaReset();
	void LuaLoad(std::string const & path);
	void LuaCall(char const * func, char const * arg);

	namespace func
	{

		char const * ItemGetStatsIdProxy(char const * itemGuid)
		{
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return nullptr;
			}

			if (!item->StatsId.Str) {
				OsiError("Item '" << itemGuid << "' has no stats ID!");
				return nullptr;
			} else {
				return item->StatsId.Str;
			}
		}

		void OsiLuaReset(OsiArgumentDesc const & args)
		{
			LuaReset();
		}

		void OsiLuaLoad(OsiArgumentDesc const & args)
		{
			auto path = args[0].String;
			LuaLoad(path);
		}

		void OsiLuaCall(OsiArgumentDesc const & args)
		{
			auto func = args[0].String;
			auto arg = args[1].String;
			LuaCall(func, arg);
		}

		void BreakOnCharacter(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			OsiWarn("GotIt");
		}

		void BreakOnItem(OsiArgumentDesc const & args)
		{
			auto item = FindItemByNameGuid(args[0].String);
			OsiWarn("GotIt");
		}

		void DoExperiment(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			if (character) {
				auto stats = character->Stats->DynamicStats[1];
				stats->PoisonResistance += 50;
				stats->APStart += 3;
				stats->APRecovery += 3;

				auto stats0 = character->Stats->DynamicStats[0];
				stats0->PoisonResistance += 50;
				OsiError("DoExperiment(): Applied to character");
			}

			auto item = FindItemByNameGuid(args[1].String);
			if (item) {
				auto stats = item->StatsDynamic->DynamicAttributes_Start[1];
				stats->FireResistance += 50;

				auto stats0 = item->StatsDynamic->DynamicAttributes_Start[0];
				stats0->FireResistance += 50;
				OsiError("DoExperiment(): Applied to item");
			}

			OsiError("Nothing to see here");
		}
	}

	void ExtensionState::Reset()
	{
		DamageHelpers.Clear();
	}

	CustomFunctionLibrary::CustomFunctionLibrary(class OsirisProxy & osiris)
		: osiris_(osiris)
	{}

	void CustomFunctionLibrary::Register()
	{
		RegisterHelperFunctions();
		RegisterMathFunctions();
		RegisterStatFunctions();
		RegisterStatusFunctions();
		RegisterGameActionFunctions();
		RegisterProjectileFunctions();
		RegisterHitFunctions();
		RegisterPlayerFunctions();
		RegisterItemFunctions();
		RegisterCharacterFunctions();

		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto breakOnCharacter = std::make_unique<CustomCall>(
			"NRD_BreakOnCharacter",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In }
			},
			&func::BreakOnCharacter
		);
		functionMgr.Register(std::move(breakOnCharacter));

		auto breakOnItem = std::make_unique<CustomCall>(
			"NRD_BreakOnItem",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In }
			},
			&func::BreakOnItem
		);
		functionMgr.Register(std::move(breakOnItem));

		auto experiment = std::make_unique<CustomCall>(
			"NRD_Experiment",
			std::vector<CustomFunctionParam>{
				{ "Arg1", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg3", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::DoExperiment
		);
		functionMgr.Register(std::move(experiment));

		auto luaReset = std::make_unique<CustomCall>(
			"NRD_LuaReset",
			std::vector<CustomFunctionParam>{},
			&func::OsiLuaReset
		);
		functionMgr.Register(std::move(luaReset));

		auto luaLoad = std::make_unique<CustomCall>(
			"NRD_LuaLoad",
			std::vector<CustomFunctionParam>{
				{ "Path", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::OsiLuaLoad
		);
		functionMgr.Register(std::move(luaLoad));

		auto luaCall = std::make_unique<CustomCall>(
			"NRD_LuaCall",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::OsiLuaCall
		);
		functionMgr.Register(std::move(luaCall));
	}

	void CustomFunctionLibrary::PostStartup()
	{
		if (PostLoaded) {
			return;
		}

		using namespace std::placeholders;

		osiris_.GetLibraryManager().StatusHitEnter.AddPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusHitEnter, this, _1)
		);
		osiris_.GetLibraryManager().StatusHealEnter.AddPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusHealEnter, this, _1)
		);
		osiris_.GetLibraryManager().CharacterHitHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnCharacterHit, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)
		);
		osiris_.GetLibraryManager().ApplyStatusHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnApplyStatus, this, _1, _2, _3)
		);

		PostLoaded = true;
	}
}
