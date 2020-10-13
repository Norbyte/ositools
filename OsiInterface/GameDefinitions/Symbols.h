#pragma once

#include <GameDefinitions/Ai.h>
#include <GameDefinitions/Character.h>
#include <GameDefinitions/CustomStats.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/Level.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/Osiris.h>
#include <GameDefinitions/Status.h>
#include <GameDefinitions/ActionMachine.h>
#include <GameDefinitions/TurnManager.h>
#include <GameDefinitions/UI.h>
#include <GameDefinitions/Surface.h>
#include <GameDefinitions/RootTemplates.h>
#include <GameDefinitions/Sound.h>

namespace dse
{
	enum class EsvGlobalEoCApp {
		EsvLSDialogEventManager = 0,
		EsvStoryDialogEventManager = 1,
		EsvStoryItemEventManager = 2,
		EsvStoryCharacterEventManager = 3,
		// 4
		// 5-6 event visitor
		// 7-8 game event visitor
		ServerLevelManager = 9,
		PartyManager = 10,
		EsvCharacterFactory = 11,
		// 12
		EsvProjectileFactory = 13,
		EsvEoCTriggerFactory = 14,
		EsvItemFactory = 15,
		EsvSkillFactory = 16,
		EsvSkillStatePool = 17,
		EsvInventoryFactory = 18,
		EsvInventoryViewFactory = 19,
		StoryImplementation = 20,
		AlignmentFactory = 21,
		AlignmentContainer = 22,
		OsirisVariableHelper = 23,
		// 24
		OverheadTextFactory = 25,
		SurfaceActionFactory = 26,
		CustomBookManager = 27,
		// 28
		GameControl = 29,
		// 30
		CharacterCreationManager = 31,
		MarkerManager = 32,
		OsirisTaskFactory = 33,
		EnvironmentalInfluencesPool = 34,
		CombatFormationManager = 35,
		DivinityStats = 36,
		PlanManager = 37,
		// 38
		ActivationManager = 39
	};

	enum class EsvGlobalEoCPlugin {
		EsvSavegameManager = 0,
		EsvEoCServer = 1,
		EsvCacheTemplateManager = 2,
		EsvAiFactory = 3,
		EsvGameObjectFactory = 4,
		EsvLSDialogEventManager = 5,
		EsvStoryDialogEventManager = 6,
		EsvStoryItemEventManager = 7,
		EsvStoryCharacterEventManager = 8,
		// 9, 10-11, 12-13 event visitors
		ServerLevelManager = 14,
		PartyManager = 15,
		EsvCharacterFactory = 16,
		EsvProjectileFactory = 17,
		EsvEoCTriggerFactory = 18,
		EsvItemFactory = 19,
		EsvSkillFactory = 20,
		EsvSkillStatePool = 21,
		EsvInventoryFactory = 22,
		EsvInventoryViewFactory = 23,
		StoryImplementation = 24,
		AlignmentFactory = 25,
		OsirisVariableHelper = 26,
		// 27
		OverheadTextFactory = 28,
		SurfaceActionFactory = 29,
		CustomBookManager = 30,
		// 31
		GameControl = 32,
		// 33
		CharacterCreationManager = 34,
		MarkerManager = 35,
		OsirisTaskFactory = 36,
		EnvironmentalInfluencesPool = 37,
		CombatFormationManager = 38,
		DivinityStats = 39,
		PlanManager = 40,
		ActivationManager = 41
	};

	struct StaticSymbols : Noncopyable<StaticSymbols>
	{
		struct EoCLibraryInfo
		{
			uint8_t const * initFunc;
			uint8_t const * freeFunc;
			uint32_t refs;
		};

		ls__FixedString__Create CreateFixedString{ nullptr };

		esv::ProjectileHelpers__ShootProjectile* esv__ProjectileHelpers__ShootProjectile{ nullptr };
		esv::Projectile::ExplodeProc* esv__Projectile__Explode{ nullptr };
		esv::GameActionManager__CreateAction CreateGameAction{ nullptr };
		esv::GameActionManager__AddAction AddGameAction{ nullptr };
		esv::TornadoAction__Setup TornadoActionSetup{ nullptr };
		esv::TornadoAction__Setup WallActionCreateWall{ nullptr };
		esv::SummonHelpers__Summon SummonHelpersSummon{ nullptr };
		esv::GameObjectMoveAction__Setup GameObjectMoveActionSetup{ nullptr };
		esv::StatusMachine::CreateStatusProc* esv__StatusMachine__CreateStatus{ nullptr };
		esv::StatusMachine::ApplyStatusProc* esv__StatusMachine__ApplyStatus{ nullptr };
		esv::Character::HitProc* esv__Character__Hit{ nullptr };
		CDivinityStats_Character::HitInternalProc* CDivinityStats_Character__HitInternal{ nullptr };
		esv::Character::ApplyDamageProc* esv__Character__ApplyDamage{ nullptr };
		esv::StatusVMT const * StatusHealVMT{ nullptr };
		esv::StatusVMT const * StatusHitVMT{ nullptr };
		esv::StatusHeal::EnterProc* esv__StatusHeal__Enter{ nullptr };
		esv::StatusHit::EnterProc* esv__StatusHit__Enter{ nullptr };
		esv::StatusHit::SetupProc* esv__StatusHit__Setup{ nullptr };
		esv::ParseItem ParseItem{ nullptr };
		esv::CreateItemFromParsed CreateItemFromParsed{ nullptr };
		esv::CustomStatsProtocol__ProcessMsg EsvCustomStatsProtocolProcessMsg{ nullptr };
		esv::ActionMachine::SetStateProc* ecl__ActionMachine__SetState{ nullptr };
		esv::ActionMachine::SetStateProc* esv__ActionMachine__SetState{ nullptr };
		esv::ActionMachine::ResetStateProc* ecl__ActionMachine__ResetState{ nullptr };
		esv::ActionMachine::ResetStateProc* esv__ActionMachine__ResetState{ nullptr };
		esv::ExecutePropertyDataOnGroundHitProc* esv__ExecutePropertyDataOnGroundHit{ nullptr };
		esv::ExecutePropertyDataOnPositionOnlyProc* esv__ExecutePropertyDataOnPositionOnly{ nullptr };
		esv::ExecuteCharacterSetExtraPropertiesProc* esv__ExecuteCharacterSetExtraProperties{ nullptr };

		esv::SurfaceActionFactory** esv__SurfaceActionFactory{ nullptr };
		esv::SurfaceActionFactory::CreateActionProc* esv__SurfaceActionFactory__CreateAction{ nullptr };
		esv::TransformSurfaceAction::InitProc* esv__TransformSurfaceAction__Init{ nullptr };

		ecl::EoCClient ** EoCClient{ nullptr };
		ecl::EoCClient__HandleError EoCClientHandleError{ nullptr };
		App** AppInstance{ nullptr };
		App::VMT::OnInputEventProc* App__OnInputEvent{ nullptr };

		uint8_t const * UICharacterSheetHook{ nullptr };
		uint8_t const * ActivateClientSystemsHook{ nullptr };
		uint8_t const * ActivateServerSystemsHook{ nullptr };
		uint8_t const * CustomStatUIRollHook{ nullptr };
		eoc::NetworkFixedStrings ** NetworkFixedStrings{ nullptr };
		void * InitNetworkFixedStrings{ nullptr };
		ecl::GameStateEventManager__ExecuteGameStateChangedEvent ecl__GameStateEventManager__ExecuteGameStateChangedEvent{ nullptr };
		esv::GameStateEventManager__ExecuteGameStateChangedEvent esv__GameStateEventManager__ExecuteGameStateChangedEvent{ nullptr };
		ecl::GameStateThreaded__GameStateWorker__DoWork ecl__GameStateThreaded__GameStateWorker__DoWork{ nullptr };
		esv::GameStateThreaded__GameStateWorker__DoWork esv__GameStateThreaded__GameStateWorker__DoWork{ nullptr };
		net::Host::AddProtocol net__Host__AddProtocol{ nullptr };
		net::MessageFactory::RegisterMessage net__MessageFactory__RegisterMessage{ nullptr };
		net::MessageFactory::GetFreeMessage net__MessageFactory__GetFreeMessage{ nullptr };
		eoc__SkillPrototypeManager__Init SkillPrototypeManagerInit{ nullptr };
		SkillPrototype::FormatDescriptionParamProc* SkillPrototype__FormatDescriptionParam{ nullptr };
		SkillPrototype::GetSkillDamageProc* SkillPrototype__GetSkillDamage{ nullptr };
		SkillPrototype::GetAttackAPCostProc* SkillPrototype__GetAttackAPCost{ nullptr };
		StatusPrototype::FormatDescriptionParamProc* StatusPrototype__FormatDescriptionParam{ nullptr };
		esv::TurnManager::UpdateTurnOrderProc* esv__TurnManager__UpdateTurnOrder{ nullptr };
		esv::Inventory::Equip InventoryEquip{ nullptr };
		esv::Status::GetEnterChanceProc* esv__Status__GetEnterChance{ nullptr };
		esv::ItemHelpers__GenerateTreasureItem* esv__ItemHelpers__GenerateTreasureItem{ nullptr };
		esv::CombineManager::ExecuteCombinationProc* esv__CombineManager__ExecuteCombination{ nullptr };
		eoc::SurfaceTransformInteractions* eoc__SurfaceTransformActionsFromType{ nullptr };
		SurfaceTemplate** eoc__SurfaceTemplates{ nullptr };
		eoc::AiGrid::SearchForCellProc* eoc__AiGrid__SearchForCell{ nullptr };

		GlobalStringTable const ** GlobalStrings{ nullptr };
		ls__Path__GetPrefixForRoot GetPrefixForRoot{ nullptr };
		ls__FileReader__FileReader FileReaderCtor{ nullptr };
		ls__FileReader__Dtor FileReaderDtor{ nullptr };
		STDString ** PathRoots{ nullptr };

		UIObjectManager::RegisterUIObjectCreatorProc* UIObjectManager__RegisterUIObjectCreator{ nullptr };
		UIObjectManager::CreateUIObjectProc* UIObjectManager__CreateUIObject{ nullptr };
		UIObjectManager::DestroyUIObjectProc* UIObjectManager__DestroyUIObject{ nullptr };
		UIObjectManager::GetInstanceProc* UIObjectManager__GetInstance{ nullptr };
		UIObjectManager ** UIObjectManager__Instance{ nullptr };
		ecl::EoCUI::ctor EoCUI__ctor{ nullptr };
		UIObject::VMT * EoCUI__vftable{ nullptr };

		TranslatedStringRepository ** TranslatedStringRepository__Instance{ nullptr };
		TranslatedStringRepository::GetInstance TranslatedStringRepository__GetInstance{ nullptr };
		TranslatedStringRepository::Get TranslatedStringRepository__Get{ nullptr };
		TranslatedStringRepository::UnloadOverrides TranslatedStringRepository__UnloadOverrides{ nullptr };

		TranslatedStringKeyManager** TranslatedStringKeyManager__Instance{ nullptr };
		TranslatedStringKeyManager::GetInstance TranslatedStringKeyManager__GetInstance{ nullptr };
		TranslatedStringKeyManager::GetTranlatedStringFromKey TranslatedStringKeyManager__GetTranlatedStringFromKey{ nullptr };

		CRPGStatsManager::LoadProc RPGStats__Load{ nullptr };
		CRPGStatsManager::ParsePropertiesProc* RPGStats__ParseProperties{ nullptr };
		eoc::SpeakerManager ** eoc__SpeakerManager{ nullptr };
		SkillPrototypeManager** eoc__SkillPrototypeManager{ nullptr };
		StatusPrototypeManager** eoc__StatusPrototypeManager{ nullptr };
		void* esv__OsirisVariableHelper__SavegameVisit{ nullptr };
		ModManager::CollectAvailableMods ModManager__CollectAvailableMods{ nullptr };
		ScriptCheckBlock__Build ScriptCheckBlock__Build{ nullptr };

		ecl::LevelManager ** EclLevelManager{ nullptr };
		ecl::InventoryFactory ** EclInventoryFactory{ nullptr };

		esv::LevelManager ** EsvLevelManager{ nullptr };
		esv::InventoryFactory ** EsvInventoryFactory{ nullptr };
		esv::SurfaceActionFactory** EsvSurfaceActionFactory{ nullptr };
		esv::EoCServer ** EoCServer{ nullptr };
		ResourceManager::GetInstanceProc * ResourceManager__GetInstance{ nullptr };
		ResourceManager ** ResourceManager__Instance{ nullptr };
#if defined(OSI_EOCAPP)
		GlobalSwitches ** pGlobalSwitches{ nullptr };
#else
		GlobalSwitches *** pGlobalSwitches{ nullptr };
#endif
		void * ItemFoldDynamicAttributes{ nullptr };
		void * ModuleSettingsHasCustomMods{ nullptr };
		void * ModuleSettingsHasCustomModsGB5{ nullptr };
		ModuleSettings::ValidateProc* ModuleSettings__Validate{ nullptr };
		Module::HashProc* Module__Hash{ nullptr };
		esv::LoadProtocol::HandleModuleLoadedProc* esv__LoadProtocol__HandleModuleLoaded{ nullptr };

		ig::ValuePathMakeNameRefProc IgValuePathMakeNameRef{ nullptr };
		ig::ValuePathMakeArrayRefProc IgValuePathPathMakeArrayRef{ nullptr };
		ig::ValuePathSetArrayIndexProc IgValuePathSetArrayIndex{ nullptr };

		ig::ValueGetTypeProc IgValueGetType{ nullptr };
		ig::ValueGetArrayLengthProc IgValueGetArrayLength{ nullptr };

		ig::ValueGetBooleanProc IgValueGetBoolean{ nullptr };
		ig::ValueGetF64Proc IgValueGetF64{ nullptr };
		ig::ValueGetStringUTF8Proc IgValueGetStringUTF8{ nullptr };

		ig::ValueSetBooleanProc IgValueSetBoolean{ nullptr };
		ig::ValueSetF64Proc IgValueSetF64{ nullptr };
		ig::ValueSetStringUTF8Proc IgValueSetStringUTF8{ nullptr };

		ig::PlayerCreateFastNameUTF8 IgPlayerCreateFastNameUTF8{ nullptr };
		ig::PlayerCallMethod IgPlayerCallMethod{ nullptr };

		void * DUMMY1{ nullptr };
		std::map<uint8_t const *, EoCLibraryInfo> Libraries;

		uint8_t const * ServerRegisterFuncs[50]{ nullptr };
		uint8_t const ** ServerGlobals[50]{ nullptr };
		uint8_t const * EocRegisterFuncs[6]{ nullptr };
		uint8_t const ** EocGlobals[6]{ nullptr };

		EoCAllocFunc EoCAlloc{ nullptr };
		EoCFreeFunc EoCFree{ nullptr };
		CrtAllocFunc CrtAlloc{ nullptr };
		CrtFreeFunc CrtFree{ nullptr };

		CharacterStatsGetters CharStatsGetters;

		inline StaticSymbols() {}
		StaticSymbols(StaticSymbols const &) = delete;
		StaticSymbols & operator = (StaticSymbols const &) = delete;

		inline GlobalSwitches * GetGlobalSwitches() const
		{
#if defined(OSI_EOCAPP)
			if (pGlobalSwitches && *pGlobalSwitches) {
				return *pGlobalSwitches;
			} else {
				return nullptr;
			}
#else
			if (pGlobalSwitches && *pGlobalSwitches && **pGlobalSwitches) {
				return **pGlobalSwitches;
			} else {
				return nullptr;
			}
#endif
		}


		inline esv::InventoryFactory * GetServerInventoryFactory() const
		{
			if (EsvInventoryFactory) {
				return *EsvInventoryFactory;
			} else {
				return nullptr;
			}
		}

		inline ecl::InventoryFactory* GetClientInventoryFactory() const
		{
			if (EclInventoryFactory) {
				return *EclInventoryFactory;
			}
			else {
				return nullptr;
			}
		}

		inline GlobalStringTable const * GetGlobalStringTable() const
		{
			if (GlobalStrings) {
				return *GlobalStrings;
			} else {
				return nullptr;
			}
		}

		inline esv::LevelManager* GetServerLevelManager() const
		{
			if (EsvLevelManager == nullptr
				|| *EsvLevelManager == nullptr) {
				return nullptr;
			}

			return *EsvLevelManager;
		}

		inline esv::Level* GetCurrentServerLevel() const
		{
			auto levelMgr = GetServerLevelManager();
			if (levelMgr == nullptr) {
				return nullptr;
			}

			return levelMgr->CurrentLevel;
		}

		inline esv::GameActionManager * GetGameActionManager() const
		{
			auto level = GetCurrentServerLevel();
			if (level == nullptr) {
				return nullptr;
			}

			return level->GameActionManager;
		}

		inline ecl::LevelManager* GetClientLevelManager() const
		{
			if (EclLevelManager == nullptr
				|| *EclLevelManager == nullptr) {
				return nullptr;
			}

			return *EclLevelManager;
		}

		inline ecl::Level* GetCurrentClientLevel() const
		{
			auto levelMgr = GetClientLevelManager();
			if (levelMgr == nullptr) {
				return nullptr;
			}

			return levelMgr->CurrentLevel;
		}

		inline CRPGStatsManager * GetStats() const
		{
			if (EocGlobals[5]) {
				return *(CRPGStatsManager **)EocGlobals[5];
			} else {
				return nullptr;
			}
		}

		inline std::optional<ecl::GameState> GetClientState() const
		{
			if (EoCClient != nullptr
				&& *EoCClient != nullptr
				&& (*EoCClient)->GameStateMachine != nullptr
				&& *(*EoCClient)->GameStateMachine != nullptr) {
				return (*(*EoCClient)->GameStateMachine)->State;
			} else {
				return {};
			}
		}

		inline std::optional<esv::GameState> GetServerState() const
		{
			if (EoCServer != nullptr
				&& *EoCServer != nullptr
				&& (*EoCServer)->StateMachine != nullptr) {
				return (*EoCServer)->StateMachine->State;
			} else {
				return {};
			}
		}

		inline UIObjectManager * GetUIObjectManager() const
		{
			if (UIObjectManager__Instance != nullptr) {
				return *UIObjectManager__Instance;
			} else if (UIObjectManager__GetInstance != nullptr) {
				return UIObjectManager__GetInstance();
			} else {
				return nullptr;
			}
		}

		inline ResourceManager * GetResourceManager() const
		{
			if (ResourceManager__Instance != nullptr) {
				return *ResourceManager__Instance;
			} else if (ResourceManager__GetInstance != nullptr) {
				return ResourceManager__GetInstance();
			} else {
				return nullptr;
			}
		}

		inline TranslatedStringKeyManager* GetTranslatedStringKeyManager() const
		{
			auto getKeyMgrInstance = TranslatedStringKeyManager__GetInstance;
			auto keyMgrInstance = TranslatedStringKeyManager__Instance;

			TranslatedStringKeyManager* keyMgr{ nullptr };
			if (getKeyMgrInstance) {
				keyMgr = getKeyMgrInstance();
			} else if (keyMgrInstance) {
				keyMgr = *keyMgrInstance;
			}

			return keyMgr;
		}

		inline TranslatedStringRepository* GetTranslatedStringRepository() const
		{
			auto getRepoInstance = TranslatedStringRepository__GetInstance;
			auto repoInstance = TranslatedStringRepository__Instance;

			TranslatedStringRepository* repo{ nullptr };
			if (getRepoInstance) {
				repo = getRepoInstance();
			} else if (repoInstance) {
				repo = *repoInstance;
			}

			return repo;
		}

		inline SurfaceTemplate* GetSurfaceTemplate(SurfaceType type) const
		{
			if (eoc__SurfaceTemplates != nullptr && type <= SurfaceType::Sentinel) {
				return eoc__SurfaceTemplates[(unsigned)type];
			} else {
				return nullptr;
			}
		}


		inline void RegisterUIObjectCreator(UIObjectManager * self, unsigned int index, UIObjectFunctor * creator)
		{
#if defined(OSI_EOCAPP)
			auto hashMap = (UIObjectManager *)((intptr_t)self + offsetof(UIObjectManager, UIObjectCreators));
			UIObjectManager__RegisterUIObjectCreator(hashMap, index, creator);
#else
			UIObjectManager__RegisterUIObjectCreator(self, index, creator);
#endif
		}

		inline void AddGameActionWrapper(esv::GameActionManager * mgr, esv::GameAction * action) const
		{
			if (AddGameAction != nullptr) {
#if defined(OSI_EOCAPP)
				AddGameAction(&mgr->GameActions, &action);
#else
				AddGameAction(mgr, action);
#endif
			}
		}

		void CanonicalizePath(STDString & path) const;
		STDString ToPath(StringView path, PathRootType root, bool canonicalize = true) const;
		FileReaderPin MakeFileReader(StringView path, PathRootType root = PathRootType::Data, bool canonicalize = true) const;
		void DestroyFileReader(FileReader * reader);
		bool FileExists(StringView path, PathRootType root = PathRootType::Data, bool canonicalize = true) const;
	};

	StaticSymbols & GetStaticSymbols();
}
