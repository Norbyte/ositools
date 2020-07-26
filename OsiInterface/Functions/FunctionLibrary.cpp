#include <stdafx.h>
#include <OsirisProxy.h>
#include <GameDefinitions/TurnManager.h>
#include <Functions/FunctionLibrary.h>
#include <Version.h>
#include <ScriptHelpers.h>
#include <fstream>
#include "json/json.h"

namespace dse::esv
{
	namespace func
	{
		void ShowErrorMessage(OsiArgumentDesc const & args)
		{
			auto message = args[0].String;
			auto wmsg = FromUTF8(message);
			gOsirisProxy->GetLibraryManager().ShowStartupError(wmsg, false, false);
		}

		bool IsModLoaded(OsiArgumentDesc & args)
		{
			auto modUuid = ToFixedString(args[0].String);
			auto & loaded = args[1];

			auto modManager = GetModManagerServer();
			if (modManager == nullptr) {
				OsiErrorS("Mod manager not available");
				return false;
			}

			if (!modUuid) {
				loaded.Set(0);
				return true;
			}

			loaded.Set(0);
			for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
				if (mod.Info.ModuleUUID == modUuid) {
					loaded.Set(1);
				}
			}

			return true;
		}

		bool GetVersion(OsiArgumentDesc & args)
		{
			args[0].Set((int32_t)CurrentVersion);
			return true;
		}

		bool LoadFile(OsiArgumentDesc & args)
		{
			auto path = args[0].String;
			auto & contents = args[1];

			auto loaded = script::LoadExternalFile(path);
			if (loaded) {
				contents.Set(gTempStrings.Make(*loaded));
				return true;
			} else {
				return false;
			}
		}

		void SaveFile(OsiArgumentDesc const & args)
		{
			auto path = args[0].String;
			auto contents = args[1].String;

			script::SaveExternalFile(path, contents);
		}

		void BreakOnCharacter(OsiArgumentDesc const & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			OsiWarn("GotIt");
		}

		void BreakOnItem(OsiArgumentDesc const & args)
		{
			auto item = GetEntityWorld()->GetItem(args[0].String);
			OsiWarn("GotIt");
		}

		void DoExperiment(OsiArgumentDesc const & args)
		{
			OsiErrorS("Nothing to see here");
		}
	}

	CustomFunctionLibrary::CustomFunctionLibrary(OsirisProxy & osiris)
		: osiris_(osiris)
	{}

	void CustomFunctionLibrary::Register()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();
		functionMgr.BeginStaticRegistrationPhase();

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
		RegisterLuaFunctions();

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

		auto getVersion = std::make_unique<CustomQuery>(
			"NRD_GetVersion",
			std::vector<CustomFunctionParam>{
				{ "Version", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::GetVersion
		);
		functionMgr.Register(std::move(getVersion));

		auto loadFile = std::make_unique<CustomQuery>(
			"NRD_LoadFile",
			std::vector<CustomFunctionParam>{
				{ "Path", ValueType::String, FunctionArgumentDirection::In },
				{ "Contents", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::LoadFile
		);
		functionMgr.Register(std::move(loadFile));

		auto saveFile = std::make_unique<CustomCall>(
			"NRD_SaveFile",
			std::vector<CustomFunctionParam>{
				{ "Path", ValueType::String, FunctionArgumentDirection::In },
				{ "Contents", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::SaveFile
		);
		functionMgr.Register(std::move(saveFile));

		functionMgr.EndStaticRegistrationPhase();
	}

	void CustomFunctionLibrary::PostStartup()
	{
		if (!gOsirisProxy->HasFeatureFlag("OsirisExtensions")) {
			return;
		}

		if (PostLoaded) {
			return;
		}

		using namespace std::placeholders;

		osiris_.GetLibraryManager().esv__Status__GetEnterChanceHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnStatusGetEnterChance, this, _1, _2, _3)
		);
		osiris_.GetLibraryManager().esv__StatusHeal__Enter.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusHealEnter, this, _1)
		);
		osiris_.GetLibraryManager().esv__ActionMachine__SetState.AddPreHook(
			std::bind(&CustomFunctionLibrary::OnBeforeActionMachineSetState, this, _1, _2, _3, _4, _5, _6)
		);
		osiris_.GetLibraryManager().esv__ActionMachine__SetState.AddPostHook(
			std::bind(&CustomFunctionLibrary::OnActionMachineSetState, this, _1, _2, _3, _4, _5, _6, _7)
		);
		osiris_.GetLibraryManager().esv__ActionMachine__ResetState.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnActionMachineResetState, this, _1, _2)
		);
		osiris_.GetLibraryManager().SkillPrototype__FormatDescriptionParam.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnSkillFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
		);
		osiris_.GetLibraryManager().StatusPrototype__FormatDescriptionParam.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnStatusFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
		);
		osiris_.GetLibraryManager().esv__TurnManager__UpdateTurnOrder.SetPostHook(
			std::bind(&CustomFunctionLibrary::OnUpdateTurnOrder, this, _1, _2)
		);
		osiris_.GetLibraryManager().UIObjectManager__CreateUIObject.SetPostHook(
			std::bind(&CustomFunctionLibrary::OnCreateUIObject, this, _1, _2, _3, _4, _5, _6, _7, _8)
		);
		osiris_.GetLibraryManager().esv__ItemHelpers__GenerateTreasureItem.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnGenerateTreasureItem, this, _1, _2, _3)
		);
		osiris_.GetLibraryManager().esv__CombineManager__ExecuteCombination.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnCraftingExecuteCombination, this, _1, _2, _3, _4, _5, _6, _7, _8)
		);
		GetStaticSymbols().CharStatsGetters.WrapperHitChance.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnGetHitChance, this, _1, _2, _3)
		);
		osiris_.GetLibraryManager().esv__Character_HasMadnessHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::CharacterHasMadnessFix, this, _1, _2)
		);
		osiris_.GetLibraryManager().ecl__Character_HasMadnessHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::EclCharacterHasMadnessFix, this, _1, _2)
		);

		PostLoaded = true;
	}

	void CustomFunctionLibrary::OnBaseModuleLoadedServer()
	{
		DEBUG("CustomFunctionLibrary::OnBaseModuleLoadedServer(): Re-initializing module state.");
		auto & functionMgr = osiris_.GetCustomFunctionManager();
		functionMgr.ClearDynamicEntries();
		esv::ExtensionState::Get().LuaReset(true);
	}

	void CustomFunctionLibrary::OnBaseModuleLoadedClient()
	{
		DEBUG("CustomFunctionLibrary::OnBaseModuleLoadedClient(): Re-initializing module state.");
		ecl::ExtensionState::Get().LuaReset(true);
	}
}
