#include <stdafx.h>
#include <GameHooks/DataLibraries.h>
#include <Extender/Shared/ExtensionState.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Symbols.h>
#include <string>
#include <functional>
#include <psapi.h>
#include <DbgHelp.h>

namespace dse
{
	LibraryManager::LibraryManager()
		: symbolMapper_(mappings_)
	{}

	bool LibraryManager::FindLibraries(uint32_t gameRevision)
	{
		RegisterSymbols();

		SymbolMappingLoader loader(mappings_);
		PreRegisterLibraries(loader);
		if (!loader.LoadBuiltinMappings()) {
			ERR("Failed to load symbol mapping table");
			CriticalInitFailed = true;
		}

#if 0
		if (!CriticalInitFailed) {
			// Debug check for symbol xml -> engine bindings
			for (auto const& sym : mappings_.StaticSymbols) {
				if (!sym.second.Bound) {
					WARN("No mapping exists for engine symbol: %s", sym.first.c_str());
				}
			}
		}
#endif

		memset(&GetStaticSymbols().CharStatsGetters, 0, sizeof(GetStaticSymbols().CharStatsGetters));

		if (!BindApp()) {
#if defined(OSI_EOCAPP)
			ERR("LibraryManager::FindLibraries(): Unable to bind EoCApp module.");
#else
			ERR("LibraryManager::FindLibraries(): Unable to bind EoCPlugin module.");
#endif
			return false;
		}

		RegisterLibraries(symbolMapper_);
		symbolMapper_.MapAllSymbols(false);

		CriticalInitFailed = CriticalInitFailed || symbolMapper_.HasFailedCriticalMappings();
		InitFailed = InitFailed || symbolMapper_.HasFailedMappings();

#if defined(OSI_EOCAPP)
		FindServerGlobalsEoCApp();
		FindEoCGlobalsEoCApp();
		FindGlobalStringTableEoCApp();
#else
		FindServerGlobalsEoCPlugin();
		FindEoCGlobalsEoCPlugin();
		FindGlobalStringTableCoreLib();
#endif

		return !CriticalInitFailed;
	}

#define SYM_OFF(name) mappings_.StaticSymbols.insert(std::make_pair(#name, SymbolMappings::StaticSymbol{ (int)offsetof(StaticSymbols, name) }))
#define CHAR_GETTER_SYM_OFF(name) mappings_.StaticSymbols.insert(std::make_pair("CharacterStatGetters__" #name, SymbolMappings::StaticSymbol{ (int)offsetof(StaticSymbols, CharStatsGetters) + (int)offsetof(stats::CharacterStatsGetters, name) }))

	void LibraryManager::RegisterSymbols()
	{
		SYM_OFF(EoCAlloc);
		SYM_OFF(EoCFree);
		SYM_OFF(CrtAlloc);
		SYM_OFF(CrtFree);

		SYM_OFF(ls__FixedString__Create);
		SYM_OFF(ls__GlobalStrings);

		SYM_OFF(ls__Path__GetPrefixForRoot);
		SYM_OFF(ls__FileReader__ctor);
		SYM_OFF(ls__FileReader__dtor);
		SYM_OFF(ls__PathRoots);

		SYM_OFF(AppInstance);
		SYM_OFF(App__OnInputEvent);

		SYM_OFF(ecl__EoCClient);
		SYM_OFF(ecl__EoCClient__HandleError);
		SYM_OFF(esv__EoCServer);

		SYM_OFF(esv__ProjectileHelpers__ShootProjectile);
		SYM_OFF(esv__Projectile__Explode);
		SYM_OFF(esv__GameActionManager__CreateAction);
		SYM_OFF(esv__GameActionManager__AddGameAction);
		SYM_OFF(esv__TornadoAction__Setup);
		SYM_OFF(esv__WallAction__CreateWall);
		SYM_OFF(esv__SummonHelpers__Summon);
		SYM_OFF(esv__GameObjectMoveAction__Setup);
		SYM_OFF(esv__Projectile__Explode);
		SYM_OFF(esv__Projectile__Explode);

		SYM_OFF(esv__StatusMachine__CreateStatus);
		SYM_OFF(esv__StatusMachine__ApplyStatus);
		SYM_OFF(esv__StatusMachine__EnterStatus);
		SYM_OFF(esv__StatusMachine__Update);
		SYM_OFF(esv__StatusMachine__DeleteStatusByHandle);
		SYM_OFF(esv__StatusMachine__DestroyStatus);
		SYM_OFF(esv__StatusMachine__ExitStatus);

		SYM_OFF(ecl__StatusMachine__ExitStatus);

		SYM_OFF(esv__StatusHeal__VMT);
		SYM_OFF(esv__StatusHit__VMT);
		SYM_OFF(esv__StatusHeal__Enter);
		SYM_OFF(esv__StatusHit__Enter);
		SYM_OFF(esv__StatusHit__Setup);
		SYM_OFF(esv__Status__GetEnterChance);
		SYM_OFF(esv__Status__ShouldDelete);

		SYM_OFF(esv__StatusConsume__Enter);
		SYM_OFF(esv__StatusConsumeBase__Enter);
		SYM_OFF(esv__StatusConsume__Exit);
		SYM_OFF(esv__StatusConsumeBase__Exit);

		SYM_OFF(esv__ParseItem);
		SYM_OFF(esv__CreateItemFromParsed);
		SYM_OFF(esv__Inventory__Equip);
		SYM_OFF(esv__ItemHelpers__GenerateTreasureItem);
		SYM_OFF(esv__CombineManager__ExecuteCombination);

		SYM_OFF(esv__CustomStatsProtocol__ProcessMsg);

		SYM_OFF(ecl__ActionMachine__SetState);
		SYM_OFF(esv__ActionMachine__SetState);
		SYM_OFF(ecl__ActionMachine__ResetState);
		SYM_OFF(esv__ActionMachine__ResetState);

		SYM_OFF(RPGStats__Load);
		SYM_OFF(RPGStats__ParseStructureFolder);
		SYM_OFF(RPGStats__ParseDataFolder);
		SYM_OFF(RPGStats__ParseProperties);
		SYM_OFF(eoc__SkillPrototypeManager);
		SYM_OFF(eoc__StatusPrototypeManager);

		SYM_OFF(eoc__GetDamageType);
		SYM_OFF(eoc__GetDamageTypeString);
		SYM_OFF(eoc__DamageTypeToTranslateString);
		SYM_OFF(eoc__DamageDescriptionToTranslateString);
		SYM_OFF(eoc__DamageTypeToTranslateStringExtended);
		SYM_OFF(fmt__GetColorCodeDmg);
		SYM_OFF(fmt__ColorCodeAndTypeDmg);

		SYM_OFF(esv__ExecutePropertyDataOnGroundHit);
		SYM_OFF(esv__ExecutePropertyDataOnPositionOnly);
		SYM_OFF(esv__ExecuteCharacterSetExtraProperties);

		SYM_OFF(esv__AiHelpers__PeekAction);
		SYM_OFF(esv__AiHelpers__SortActions);

		SYM_OFF(esv__Character__Hit);
		SYM_OFF(esv__Character__ApplyDamage);
		SYM_OFF(CDivinityStats_Character__HitInternal);

		SYM_OFF(esv__SurfaceActionFactory);
		SYM_OFF(esv__SurfaceActionFactory__CreateAction);
		SYM_OFF(esv__TransformSurfaceAction__Init);
		SYM_OFF(eoc__SurfaceTransformActionsFromType);
		SYM_OFF(eoc__SurfaceTemplates);

		SYM_OFF(ActivateClientSystemsHook);
		SYM_OFF(ActivateServerSystemsHook);
		SYM_OFF(NetworkFixedStrings);
		SYM_OFF(InitNetworkFixedStrings);

		SYM_OFF(ecl__GameStateEventManager__ExecuteGameStateChangedEvent);
		SYM_OFF(esv__GameStateEventManager__ExecuteGameStateChangedEvent);
		SYM_OFF(ecl__GameStateThreaded__GameStateWorker__DoWork);
		SYM_OFF(esv__GameStateThreaded__GameStateWorker__DoWork);
		SYM_OFF(ecl__GameStateMachine__Update);
		SYM_OFF(esv__GameStateMachine__Update);
		SYM_OFF(ecl__GameStateLoad__IncLocalProgress);

		SYM_OFF(net__Host__AddProtocol);
		SYM_OFF(net__MessageFactory__RegisterMessage);
		SYM_OFF(net__MessageFactory__GetFreeMessage);

		SYM_OFF(SkillPrototypeManager__Init);
		SYM_OFF(SkillPrototype__FormatDescriptionParam);
		SYM_OFF(SkillPrototype__GetSkillDamage);
		SYM_OFF(SkillPrototype__GetAttackAPCost);
		SYM_OFF(StatusPrototype__FormatDescriptionParam);

		SYM_OFF(esv__TurnManager__UpdateTurnOrder);
		SYM_OFF(eoc__AiGrid__SearchForCell);

		SYM_OFF(UIObjectManager__RegisterUIObjectCreator);
		SYM_OFF(UIObjectManager__CreateUIObject);
		SYM_OFF(UIObjectManager__DestroyUIObject);
		SYM_OFF(UIObjectManager__GetInstance);
		SYM_OFF(UIObjectManager__Instance);
		SYM_OFF(ecl__EoCUI__ctor);
		SYM_OFF(ecl__EoCUI__vftable);
		SYM_OFF(ls__UIObject__GetUIScaleMultiplier);
		SYM_OFF(ls__UIObject__RaiseFlag);
		SYM_OFF(ls__UIObject__ClearFlag);
		SYM_OFF(ecl__CursorControl);

		SYM_OFF(ls__CustomDrawStruct__VMT);
		SYM_OFF(ls__UIHelper__UIClearIcon);
		SYM_OFF(ls__UIHelper__UICreateIconMesh);
		SYM_OFF(ls__UIHelper__CustomDrawObject);

		SYM_OFF(ls__DragDropManager);
		SYM_OFF(ls__DragDropManager__GetInstance);
		SYM_OFF(ls__DragDropManager__StartDragString);
		SYM_OFF(ls__DragDropManager__StartDragHandle);
		SYM_OFF(ls__DragDropManager__StopDrag);
		SYM_OFF(ecl__DragDropManager);

		SYM_OFF(TranslatedStringRepository__Instance);
		SYM_OFF(TranslatedStringRepository__GetInstance);
		SYM_OFF(TranslatedStringRepository__Get);
		SYM_OFF(TranslatedStringRepository__UnloadOverrides);

		SYM_OFF(TranslatedStringKeyManager__Instance);
		SYM_OFF(TranslatedStringKeyManager__GetInstance);
		SYM_OFF(TranslatedStringKeyManager__GetTranlatedStringFromKey);

		SYM_OFF(eoc__SpeakerManager);
		SYM_OFF(ls__PlayerManager__Instance);
		SYM_OFF(esv__OsirisVariableHelper__SavegameVisit);
		SYM_OFF(ModManager__CollectAvailableMods);
		SYM_OFF(ls__ScriptCheckBlock__Build);

		SYM_OFF(eoc__CharacterCreationManager);

		SYM_OFF(ecl__LevelManager);
		SYM_OFF(ecl__InventoryFactory);
		SYM_OFF(ecl__InventoryViewFactory);

		SYM_OFF(ecl__InventoryProtocol__PostUpdate);

		SYM_OFF(esv__LevelManager);
		SYM_OFF(esv__InventoryFactory);
		SYM_OFF(esv__InventoryViewFactory);

		SYM_OFF(esv__OsirisTaskFactory);
		SYM_OFF(esv__OsirisTaskFactory__CreateTask);
		SYM_OFF(esv__OsirisTaskFactory__ReleaseTask);

		SYM_OFF(esv__gAiModifiers);
		SYM_OFF(esv__gAiHelpers);

		SYM_OFF(ls__GlobalTemplateManager);
		SYM_OFF(ls__GlobalTemplateManager__GetInstance);
		SYM_OFF(esv__CacheTemplateManager);
		SYM_OFF(esv__LocalTemplateManager);
		SYM_OFF(ecl__CacheTemplateManager);

		SYM_OFF(ls__CacheTemplateManagerBase__CacheTemplate);

		SYM_OFF(ecl__MultiEffectHandler__Init);
		SYM_OFF(ecl__MultiEffectHandler__Update);
		SYM_OFF(ecl__MultiEffectHandler__Delete);

		SYM_OFF(ecl__MaterialManager);
		SYM_OFF(ecl__MaterialManager__RemoveMaterial);
		SYM_OFF(ecl__MaterialManager__RemoveFadingMaterial);
		SYM_OFF(ecl__MaterialManager__AddOverlayMaterial);
		SYM_OFF(ecl__MaterialManager__AddFadingOverlayMaterial);
		SYM_OFF(ecl__MaterialManager__AddReplacementMaterial);

		SYM_OFF(ls__Visual__AddAttachment);

		SYM_OFF(osi__ShowNotification);

		SYM_OFF(ls__gEffectsManager);
		SYM_OFF(ls__EffectsManager__GetInstance);
		SYM_OFF(ls__EffectsManager__DestroyEffect);

		SYM_OFF(esv__EffectManager__CreateEffect);
		SYM_OFF(esv__EffectManager__DestroyEffect);

		SYM_OFF(ResourceManager__GetInstance);
		SYM_OFF(ResourceManager__InstantiateVisual);
		SYM_OFF(ResourceManager__Instance);
		SYM_OFF(ls__VisualFactory__DestroyVisual);
		SYM_OFF(ecl__EquipmentVisualsSystem__CreateVisuals);
		SYM_OFF(ls__GlobalSwitches);
		SYM_OFF(ls__GraphicSettings);
		SYM_OFF(ls__CoreGlobals);

		SYM_OFF(CDivinityStats_Item__FoldDynamicAttributes);
		SYM_OFF(ls__ModuleSettings__HasCustomMods);
		SYM_OFF(ls__ModuleSettings__HasCustomModsGB5);

		SYM_OFF(ModuleSettings__Validate);
		SYM_OFF(Module__Hash);
		SYM_OFF(esv__LoadProtocol__HandleModuleLoaded);

		SYM_OFF(ls__InputManager);
		SYM_OFF(ls__InputManager__GetInstance);
		SYM_OFF(ls__InputManager__InjectInput);

		SYM_OFF(rf__CameraManager);
		SYM_OFF(rf__CameraManager__GetInstance);
		SYM_OFF(ecl__GameCamera__VMT);

		SYM_OFF(esv__AlignmentContainer);
		SYM_OFF(esv__AlignmentContainer__Load__Hook);

		SYM_OFF(IgValuePathMakeNameRef);
		SYM_OFF(IgValuePathPathMakeArrayRef);
		SYM_OFF(IgValuePathSetArrayIndex);
		SYM_OFF(IgValueGetType);
		SYM_OFF(IgValueGetArrayLength);
		SYM_OFF(IgValueGetBoolean);
		SYM_OFF(IgValueGetF64);
		SYM_OFF(IgValueGetStringUTF8);
		SYM_OFF(IgValueSetBoolean);
		SYM_OFF(IgValueSetF64);
		SYM_OFF(IgValueSetStringUTF8);
		SYM_OFF(IgPlayerCreateFastNameUTF8);
		SYM_OFF(IgPlayerCallMethod);
		SYM_OFF(IgSetWarningCallback);
		SYM_OFF(IgSetTraceCallbackUTF8);

		CHAR_GETTER_SYM_OFF(GetChanceToHitBoost);
		CHAR_GETTER_SYM_OFF(GetHitChance);
		CHAR_GETTER_SYM_OFF(GetTalent);
		CHAR_GETTER_SYM_OFF(GetAbility);
	}

	bool LibraryManager::PostStartupFindLibraries()
	{
		if (PostLoaded) {
			return !CriticalInitFailed;
		}

		auto initStart = std::chrono::high_resolution_clock::now();

		symbolMapper_.MapAllSymbols(true);

		if (!CriticalInitFailed) {

			for (auto const& sym : mappings_.StaticSymbols) {
				if (sym.second.Bound && sym.second.Offset != -1) {
					auto ptr = (void**)((uint8_t*)&GetStaticSymbols() + sym.second.Offset);
					if (!*ptr) {
						ERR("Engine symbol not bound after mapping phase: %s", sym.first.c_str());
					}
				}
			}

			GFS.Initialize();
			InitializeEnumerations();
			InitPropertyMaps();
			lua::RegisterLibraries();
			TypeInformationRepository::GetInstance().Initialize();

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			auto& sym = GetStaticSymbols();

			if (sym.AppInstance != nullptr && *sym.AppInstance != nullptr) {
				sym.App__OnInputEvent = (*sym.AppInstance)->__vftable->OnInputEvent;
			}

			if (sym.esv__StatusHeal__VMT != nullptr) {
				sym.esv__Status__GetEnterChance = sym.esv__StatusHeal__VMT->GetEnterChance;
				sym.esv__StatusHeal__Enter = sym.esv__StatusHeal__VMT->Enter;
			}

			if (sym.esv__StatusHit__VMT != nullptr) {
				sym.esv__StatusHit__Enter = sym.esv__StatusHit__VMT->Enter;
			}



			sym.CharStatsGetters.WrapAll();

			DetourTransactionCommit();
		}

		auto initEnd = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(initEnd - initStart).count();
		DEBUG("LibraryManager::PostStartupFindLibraries() took %d ms", ms);

		PostLoaded = true;
		return !CriticalInitFailed;
	}

	bool LibraryManager::GetGameVersion(GameVersionInfo & version)
	{
#if defined(OSI_EOCAPP)
		HMODULE hGameModule = GetModuleHandleW(L"EoCApp.exe");
#else
		HMODULE hGameModule = GetModuleHandleW(L"DivinityEngine2.exe");
#endif
		if (hGameModule == NULL) {
			return false;
		}

		auto hResource = FindResource(hGameModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
		if (hResource == NULL) return false;
		auto dwSize = SizeofResource(hGameModule, hResource);
		auto hData = LoadResource(hGameModule, hResource);
		if (hData == NULL) return false;
		auto pRes = LockResource(hData);
		if (pRes == NULL) return false;

		auto pResCopy = LocalAlloc(LMEM_FIXED, dwSize);
		CopyMemory(pResCopy, pRes, dwSize);

		UINT verLength;
		VS_FIXEDFILEINFO * fixedFileInfo;
		if (VerQueryValue(pResCopy, L"\\", (LPVOID*)&fixedFileInfo, &verLength) != TRUE) return false;

		version.Major = HIWORD(fixedFileInfo->dwFileVersionMS);
		version.Minor = LOWORD(fixedFileInfo->dwFileVersionMS);
		version.Revision = HIWORD(fixedFileInfo->dwFileVersionLS);
		version.Build = LOWORD(fixedFileInfo->dwFileVersionLS);

		LocalFree(pResCopy);
		FreeResource(hData);
		return true;
	}

	void LibraryManager::ShowStartupError(STDWString const & msg, bool wait, bool exitGame)
	{
		ERR(L"STARTUP ERROR: %s", msg.c_str());

		if (GetStaticSymbols().ecl__EoCClient == nullptr
			|| GetStaticSymbols().ecl__EoCClient__HandleError == nullptr
			|| GetStaticSymbols().EoCAlloc == nullptr) {
			return;
		}

		if (wait) {
			std::thread messageThread([this, msg, exitGame]() {
				unsigned retries{ 0 };
				while (!CanShowError() && retries < 1200) {
					Sleep(100);
					retries++;
				}

				if (retries < 1200) {
					ShowStartupError(msg, exitGame);
				} else {
					MessageBoxW(NULL, msg.c_str(), L"Script Extender Error", 
						MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
				}
			});
			messageThread.detach();
		} else {
			if (CanShowError()) {
				ShowStartupError(msg, exitGame);
			} else {
				ShowStartupMessage(msg, exitGame);
			}
		}
	}

	void LibraryManager::ShowStartupError(STDWString const & msg, bool exitGame)
	{
		if (!CanShowMessages()) return;

		GetStaticSymbols().ecl__EoCClient__HandleError(*GetStaticSymbols().ecl__EoCClient, &msg, exitGame, &msg);
	}

	void LibraryManager::ShowStartupMessage(STDWString const & msg, bool exitGame)
	{
		// Don't show progress if we're already in a loaded state, as it'll show a message box instead
		if (CanShowError()) return;

		ShowStartupError(msg, exitGame);
	}

	bool LibraryManager::CanShowMessages()
	{
		return GetStaticSymbols().GetClientState()
			&& GetStaticSymbols().ecl__EoCClient__HandleError != nullptr
			&& GetStaticSymbols().EoCAlloc != nullptr;
	}

	bool LibraryManager::CanShowError()
	{
		if (!CanShowMessages()) return false;

		auto state = GetStaticSymbols().GetClientState();
		return state == ecl::GameState::Running
			|| state == ecl::GameState::Paused
			|| state == ecl::GameState::GameMasterPause
			|| state == ecl::GameState::Menu
			|| state == ecl::GameState::Lobby;
	}


	void LibraryManager::ApplyCodePatches()
	{
		EnableCustomStats();
		DisableItemFolding();
		EnableAchievements();
		ApplyCodePatch("ecl::ShroudManager::InitObjects");
	}


	bool LibraryManager::ApplyCodePatch(std::string const& mapping)
	{
		auto it = mappings_.Mappings.find(mapping);
		if (it == mappings_.Mappings.end()) {
			ERR("Cannot apply patch - no such mapping: '%s'", mapping.c_str());
			return false;
		}

		if (it->second.Patches.empty()) {
			ERR("Cannot apply patch - mapping '%s' contains no patch entries", mapping.c_str());
			return false;
		}

		bool hasChanges = false;
		for (auto& patch : it->second.Patches) {
			hasChanges = ApplyPatch(patch) || hasChanges;
		}

		return hasChanges;
	}


	bool LibraryManager::UndoCodePatch(std::string const& mapping)
	{
		auto it = mappings_.Mappings.find(mapping);
		if (it == mappings_.Mappings.end()) {
			ERR("Cannot undo patch - no such mapping: '%s'", mapping.c_str());
			return false;
		}

		bool hasChanges = false;
		for (auto& patch : it->second.Patches) {
			hasChanges = UndoPatch(patch) || hasChanges;
		}

		return hasChanges;
	}


	bool LibraryManager::ApplyPatch(SymbolMappings::Patch& patch)
	{
		if (patch.WasApplied || patch.ResolvedRef == nullptr) {
			return false;
		}

		WriteAnchor code(patch.ResolvedRef, patch.Bytes.size());
		patch.OriginalBytes.resize(patch.Bytes.size());
		memcpy(patch.OriginalBytes.data(), patch.ResolvedRef, patch.OriginalBytes.size());
		memcpy(const_cast<uint8_t*>(patch.ResolvedRef), patch.Bytes.data(), patch.Bytes.size());
		patch.WasApplied = true;
		return true;
	}


	bool LibraryManager::UndoPatch(SymbolMappings::Patch& patch)
	{
		if (!patch.WasApplied) return false;

		WriteAnchor code(patch.ResolvedRef, patch.OriginalBytes.size());
		memcpy(const_cast<uint8_t*>(patch.ResolvedRef), patch.OriginalBytes.data(), patch.OriginalBytes.size());
		patch.WasApplied = false;
		return true;
	}


	void LibraryManager::EnableCustomStats()
	{
		auto const& sym = GetStaticSymbols();
		if (sym.ActivateClientSystemsHook == nullptr
			|| sym.ActivateServerSystemsHook == nullptr) {
			ERR("LibraryManager::EnableCustomStats(): Hooks not available");
			return;
		}

		if (gExtender->HasFeatureFlag("CustomStats") && !EnabledCustomStats) {
			{
				uint8_t const replacement[] = { 0x90, 0x90 };
				WriteAnchor code(sym.ActivateClientSystemsHook, sizeof(replacement));
				memcpy(code.ptr(), replacement, sizeof(replacement));
			}

			{
				uint8_t const replacement[] = { 0x90, 0x90 };
				WriteAnchor code(sym.ActivateServerSystemsHook, sizeof(replacement));
				memcpy(code.ptr(), replacement, sizeof(replacement));
			}

			ApplyCodePatch("CustomStatUIRollHook");

			EnabledCustomStats = true;
		}

		if (gExtender->HasFeatureFlag("CustomStats")
			&& gExtender->HasFeatureFlag("CustomStatsPane")) {
			ApplyCodePatch("UICharacterSheetHook");
		}

		ApplyCodePatch("esv::AlignmentContainer::Load Hook");
	}

	void LibraryManager::DisableItemFolding()
	{
#if defined(OSI_EOCAPP)
		if (gExtender->HasFeatureFlag("DisableFolding")) {
			if (ApplyCodePatch("Item::FoldDynamicAttributes")) {
				DEBUG("Dynamic item stat folding disabled.");
			}
		}
#endif
	}


	void LibraryManager::EnableAchievements()
	{
#if defined(OSI_EOCAPP)
		if (gExtender->GetConfig().EnableAchievements) {
			if (ApplyCodePatch("ls::ModuleSettings::HasCustomMods")
				&& ApplyCodePatch("ls::ModuleSettings::HasCustomModsGB5")) {
				DEBUG("Modded achievements enabled.");
			}
		}
#endif
	}
}
