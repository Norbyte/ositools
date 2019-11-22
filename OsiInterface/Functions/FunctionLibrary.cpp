#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include <fstream>

namespace osidbg
{
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

		void ShowErrorMessage(OsiArgumentDesc const & args)
		{
			auto message = args[0].String;

			auto wmsg = FromUTF8(message);
			gOsirisProxy->GetLibraryManager().ShowStartupError(wmsg, false, false);
		}

		bool IsModLoaded(OsiArgumentDesc & args)
		{
			auto modUuid = ToFixedString(args[0].String);
			auto & loaded = args[1].Int32;

			auto modManager = GetModManager();
			if (modManager == nullptr) {
				OsiError("Mod manager not available");
				return false;
			}

			if (!modUuid) {
				loaded = 0;
				return true;
			}

			loaded = 0;
			auto & mods = modManager->BaseModule.LoadOrderedModules.Set;
			for (uint32_t i = 0; i < mods.Size; i++) {
				auto const & mod = mods.Buf[i];
				if (mod.Info.ModuleUUID == modUuid) {
					loaded = 1;
				}
			}

			return true;
		}

		void OsiLuaReset(OsiArgumentDesc const & args)
		{
			ExtensionState::Get().LuaReset();
			ExtensionState::Get().LuaStartup();
		}

		void OsiLuaLoad(OsiArgumentDesc const & args)
		{
			auto path = args[0].String;
			ExtensionState::Get().LuaLoadGameFile(path);
		}

		void OsiLuaCall(OsiArgumentDesc const & args)
		{
			auto func = args[0].String;
			auto arg = args[1].String;
			ExtensionState::Get().LuaCall(func, arg);
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

		time_t tm;
		OsiRng.seed(time(&tm));
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
		RegisterCustomStatFunctions();

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

		auto showError = std::make_unique<CustomCall>(
			"NRD_ShowErrorMessage",
			std::vector<CustomFunctionParam>{
				{ "Message", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::ShowErrorMessage
		);
		functionMgr.Register(std::move(showError));

		auto isModLoaded = std::make_unique<CustomQuery>(
			"NRD_IsModLoaded",
			std::vector<CustomFunctionParam>{
				{ "ModUuid", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "IsLoaded", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::IsModLoaded
		);
		functionMgr.Register(std::move(isModLoaded));

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

	void CustomFunctionLibrary::OnBaseModuleLoaded()
	{
		Debug("CustomFunctionLibrary::OnBaseModuleLoaded(): Re-initializing module state.");
		// FIXME - move extension state here?
		gCharacterStatsGetters.ResetExtension();
		ExtensionState::Get().LuaReset();
		ExtensionState::Get().LuaStartup();
	}

	ExtensionState & ExtensionState::Get()
	{
		return gOsirisProxy->GetExtensionState();
	}
}
