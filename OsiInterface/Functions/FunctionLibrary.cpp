#include <stdafx.h>
#include <OsirisProxy.h>
#include <GameDefinitions/TurnManager.h>
#include "FunctionLibrary.h"
#include <Version.h>
#include <fstream>
#include "json/json.h"

namespace osidbg
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

			auto modManager = GetModManager();
			if (modManager == nullptr) {
				OsiErrorS("Mod manager not available");
				return false;
			}

			if (!modUuid) {
				loaded.Set(0);
				return true;
			}

			loaded.Set(0);
			auto & mods = modManager->BaseModule.LoadOrderedModules.Set;
			for (uint32_t i = 0; i < mods.Size; i++) {
				auto const & mod = mods[i];
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

#define SAFE_PATH_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_."

		std::optional<std::string> GetPathForScriptIo(char const * scriptPath)
		{
			std::string path = scriptPath;

			if (path.find_first_not_of(SAFE_PATH_CHARS) != std::string::npos
				|| path.find("..") != std::string::npos) {
				OsiError("Illegal file name for external file access: '" << path << "'");
				return {};
			}

			auto storageRoot = GetStaticSymbols().ToPath("", PathRootType::GameStorage);
			if (storageRoot.empty()) {
				OsiErrorS("Could not fetch game storage path");
				return {};
			}

			storageRoot += "/Osiris Data";

			auto storageRootWstr = FromUTF8(storageRoot);
			BOOL created = CreateDirectory(storageRootWstr.c_str(), NULL);
			if (created == FALSE) {
				DWORD lastError = GetLastError();
				if (lastError != ERROR_ALREADY_EXISTS) {
					OsiError("Could not create storage root directory: " << storageRoot);
					return {};
				}
			}

			return storageRoot + "/" + path;
		}

		bool LoadFile(OsiArgumentDesc & args)
		{
			auto path = args[0].String;
			auto & contents = args[1];

			auto absolutePath = GetPathForScriptIo(path);
			if (!absolutePath) return false;

			std::ifstream f(absolutePath->c_str(), std::ios::in | std::ios::binary);
			if (!f.good()) {
				OsiError("Could not open file: '" << path << "'");
				return false;
			}

			std::string body;
			f.seekg(0, std::ios::end);
			body.resize(f.tellg());
			f.seekg(0, std::ios::beg);
			f.read(body.data(), body.size());

			contents.Set(gTempStrings.Make(body));

			return true;
		}

		void SaveFile(OsiArgumentDesc const & args)
		{
			auto path = args[0].String;
			auto contents = args[1].String;

			auto absolutePath = GetPathForScriptIo(path);
			if (!absolutePath) return;

			std::ofstream f(absolutePath->c_str(), std::ios::out | std::ios::binary);
			if (!f.good()) {
				OsiError("Could not open file: '" << path << "'");
				return;
			}

			f.write(contents, strlen(contents));
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
				OsiErrorS("DoExperiment(): Applied to character");
			}

			auto item = FindItemByNameGuid(args[1].String);
			if (item) {
				auto stats = item->StatsDynamic->DynamicAttributes_Start[1];
				stats->FireResistance += 50;

				auto stats0 = item->StatsDynamic->DynamicAttributes_Start[0];
				stats0->FireResistance += 50;
				OsiErrorS("DoExperiment(): Applied to item");
			}

			OsiErrorS("Nothing to see here");
		}
	}

	CustomFunctionLibrary::CustomFunctionLibrary(class OsirisProxy & osiris)
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
		if (!ExtensionState::Get().HasFeatureFlag("OsirisExtensions")) {
			return;
		}

		if (PostLoaded) {
			return;
		}

		using namespace std::placeholders;

		osiris_.GetLibraryManager().StatusGetEnterChance.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnStatusGetEnterChance, this, _1, _2, _3)
		);
		osiris_.GetLibraryManager().StatusHitEnter.AddPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusHitEnter, this, _1)
		);
		osiris_.GetLibraryManager().StatusHealEnter.AddPreHook(
			std::bind(&CustomFunctionLibrary::OnStatusHealEnter, this, _1)
		);
		osiris_.GetLibraryManager().CharacterHitHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnCharacterHit, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)
		);
		osiris_.GetLibraryManager().CharacterHitInternalHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnCharacterHitInternal, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)
		);
		osiris_.GetLibraryManager().ApplyStatusHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnApplyStatus, this, _1, _2, _3)
		);
		osiris_.GetLibraryManager().ActionMachineSetStateHook.AddPostHook(
			std::bind(&CustomFunctionLibrary::OnActionMachineSetState, this, _1, _2, _3, _4, _5, _6, _7)
		);
		osiris_.GetLibraryManager().SkillPrototypeFormatDescriptionParamHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnSkillFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
		);
		osiris_.GetLibraryManager().SkillPrototypeGetSkillDamageHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnGetSkillDamage, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11)
		);
		osiris_.GetLibraryManager().StatusPrototypeFormatDescriptionParamHook.SetWrapper(
			std::bind(&CustomFunctionLibrary::OnStatusFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
		);
		osiris_.GetLibraryManager().TurnManagerUpdateTurnOrderHook.AddPostHook(
			std::bind(&CustomFunctionLibrary::OnUpdateTurnOrder, this, _1, _2)
		);

		PostLoaded = true;
	}

	void CustomFunctionLibrary::EnableStatOverride(char const * stat)
	{
		using namespace std::placeholders;

		if (!ExtensionState::Get().HasFeatureFlag("FormulaOverrides")) {
			OsiErrorS("Formula overrides not enabled in extension config");
			return;
		}

		if (strcmp(stat, "HitChance") == 0) {
			auto & hitChance = GetStaticSymbols().CharStatsGetters.WrapperHitChance;
			if (!hitChance.IsHooked()) {
				hitChance.SetWrapper(
					std::bind(&CustomFunctionLibrary::OnGetHitChance, this, _1, _2, _3)
				);
			}
		} else {
			OsiError("Override not supported for this stat: " << stat);
		}
	}

	void CustomFunctionLibrary::OnBaseModuleLoaded()
	{
		DEBUG("CustomFunctionLibrary::OnBaseModuleLoaded(): Re-initializing module state.");
		auto & functionMgr = osiris_.GetCustomFunctionManager();
		functionMgr.ClearDynamicEntries();

		// FIXME - move extension state here?
		GetStaticSymbols().CharStatsGetters.ResetExtension();

		ExtensionState::Get().LuaReset(true);
	}
}
