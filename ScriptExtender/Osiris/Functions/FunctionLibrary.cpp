#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/TurnManager.h>
#include <Osiris/Functions/FunctionLibrary.h>
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
			gExtender->GetLibraryManager().ShowStartupError(wmsg, false, false);
		}

		bool IsModLoaded(OsiArgumentDesc & args)
		{
			FixedString modUuid(args[0].String);
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

			auto loaded = script::LoadExternalFile(path, PathRootType::GameStorage);
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

			script::SaveExternalFile(path, PathRootType::GameStorage, contents);
		}

		void BreakOnCharacter(OsiArgumentDesc const & args)
		{
			auto character = GetEntityWorld()->GetComponent<Character>(args[0].String);
			OsiWarn("GotIt");
		}

		void BreakOnItem(OsiArgumentDesc const & args)
		{
			auto item = GetEntityWorld()->GetComponent<Item>(args[0].String);
			OsiWarn("GotIt");
		}

		void DoExperiment(OsiArgumentDesc const & args)
		{
			OsiErrorS("Nothing to see here");
		}
	}

	CustomFunctionLibrary::CustomFunctionLibrary(OsirisExtender& osiris)
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

	void CustomFunctionLibrary::Startup()
	{
		if (loaded_) {
			return;
		}

		using namespace std::placeholders;
		auto& lib = gExtender->GetEngineHooks();
		// CreateUIObject always enabled for main menu version display
		lib.UIObjectManager__CreateUIObject.SetPostHook(
			std::bind(&CustomFunctionLibrary::OnCreateUIObject, this, _1, _2, _3, _4, _5, _6, _7, _8)
		);

		lib.esv__StatusMachine__Update.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusMachineUpdate, this, _1, _2)
		);
		lib.esv__StatusMachine__DeleteStatusByHandle.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusMachineDelete, this, _1, _2)
		);
		lib.esv__Status__GetEnterChanceHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnStatusGetEnterChance, this, _1, _2, _3)
		);
		lib.esv__StatusHeal__Enter.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusHealEnter, this, _1)
		);
		lib.esv__ActionMachine__SetState.AddPreHook(
			std::bind(&CustomFunctionLibrary::OnBeforeActionMachineSetState, this, _1, _2, _3, _4, _5, _6)
		);
		lib.esv__ActionMachine__SetState.AddPostHook(
			std::bind(&CustomFunctionLibrary::OnActionMachineSetState, this, _1, _2, _3, _4, _5, _6, _7)
		);
		lib.esv__ActionMachine__ResetState.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnActionMachineResetState, this, _1, _2)
		);
		lib.esv__ProjectileHelpers__ShootProjectile.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnShootProjectile, _1, _2)
		);
		lib.esv__Projectile__Explode.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnProjectileExplode, this, _1)
		);

		lib.RPGStats__ParseProperties.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnParseSkillProperties, this, _1, _2, _3)
		);
		lib.SkillPrototype__FormatDescriptionParam.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnSkillFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
		);
#if defined(OSI_EOCAPP)
		lib.SkillPrototype__GetAttackAPCost.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnGetSkillAPCost, this, _1, _2, _3, _4, _5, _6, _7)
		);
#else
		lib.SkillPrototype__GetAttackAPCost.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnGetSkillAPCost, this, _1, _2, _3, _4, _5, _6, _7, _8)
		);
#endif
		lib.StatusPrototype__FormatDescriptionParam.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnStatusFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
		);
		lib.esv__TurnManager__UpdateTurnOrder.SetPostHook(
			std::bind(&CustomFunctionLibrary::OnUpdateTurnOrder, this, _1, _2)
		);
		lib.esv__ItemHelpers__GenerateTreasureItem.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnGenerateTreasureItem, this, _1, _2, _3)
		);
		lib.esv__CombineManager__ExecuteCombination.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnCraftingExecuteCombination, this, _1, _2, _3, _4, _5, _6, _7, _8)
		);
		lib.esv__ExecutePropertyDataOnGroundHit.SetPostHook(
			std::bind(&CustomFunctionLibrary::OnExecutePropertyDataOnGroundHit, this, _1, _2, _3, _4, _5)
		);
		lib.esv__ExecutePropertyDataOnPositionOnly.SetPostHook(
			std::bind(&CustomFunctionLibrary::OnExecutePropertyDataOnPositionOnly, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
		);
		lib.esv__ExecuteCharacterSetExtraProperties.SetPostHook(
			std::bind(&CustomFunctionLibrary::OnExecuteCharacterSetExtraProperties, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)
		);
		lib.esv__AiHelpers__SortActions.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnSortAiActions, this, _1, _2)
		);
		lib.esv__AiHelpers__PeekAction.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnPeekAiAction, this, _1, _2, _3, _4)
		);
		GetStaticSymbols().CharStatsGetters.WrapperHitChance.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnGetHitChance, this, _1, _2, _3)
		);
		lib.esv__LoadProtocol__HandleModuleLoaded.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnPeerModuleLoaded, this, _1, _2, _3, _4)
		);
		lib.App__OnInputEvent.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnAppInputEvent, this, _1, _2, _3)
		);
		lib.ecl__InventoryProtocol__PostUpdate.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnInventoryProtocolPostUpdate, this, _1, _2)
		);
		lib.ls__InputManager__InjectInput.SetPreHook(
			std::bind(&CustomFunctionLibrary::OnInjectInput, this, _1, _2)
		);

		loaded_ = true;
	}
}
