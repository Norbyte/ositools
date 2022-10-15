#pragma once

#include <GameDefinitions/GameObjects/Ai.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/CustomStats.h>
#include <GameDefinitions/GameObjects/GameAction.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/Components/Inventory.h>
#include <GameDefinitions/GameObjects/Level.h>
#include <GameDefinitions/Input.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/Osiris.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/GameObjects/ActionMachine.h>
#include <GameDefinitions/TurnManager.h>
#include <GameDefinitions/UI.h>
#include <GameDefinitions/GameObjects/Surface.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>
#include <GameDefinitions/Sound.h>
#include <GameDefinitions/Resource.h>
#include <GameDefinitions/GameObjects/Effect.h>
#include <GameDefinitions/GameObjects/CharacterCreation.h>
#include <GameDefinitions/Alignment.h>
#include <GameDefinitions/GameObjects/AiHelpers.h>
#include <GameDefinitions/GameObjects/Controllers.h>
#include <GameDefinitions/GameObjects/Camera.h>
#include <GameDefinitions/GameObjects/Render.h>

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

		EoCAllocFunc EoCAlloc{ nullptr };
		EoCFreeFunc EoCFree{ nullptr };
		CrtAllocFunc CrtAlloc{ nullptr };
		CrtFreeFunc CrtFree{ nullptr };

		FixedString::CreateProc* ls__FixedString__Create{ nullptr };
		GlobalStringTable const** ls__GlobalStrings{ nullptr };

		Path::GetPrefixForRootProc* ls__Path__GetPrefixForRoot{ nullptr };
		FileReader::CtorProc* ls__FileReader__ctor{ nullptr };
		FileReader::DtorProc* ls__FileReader__dtor{ nullptr };
		STDString** ls__PathRoots{ nullptr };

		App** AppInstance{ nullptr };
		App::VMT::OnInputEventProc* App__OnInputEvent{ nullptr };

		ecl::EoCClient** ecl__EoCClient{ nullptr };
		ecl::EoCClient::HandleErrorProc* ecl__EoCClient__HandleError{ nullptr };
		esv::EoCServer** esv__EoCServer{ nullptr };

		esv::ProjectileHelpers__ShootProjectile* esv__ProjectileHelpers__ShootProjectile{ nullptr };
		esv::Projectile::ExplodeProc* esv__Projectile__Explode{ nullptr };
		esv::GameActionManager__CreateAction esv__GameActionManager__CreateAction{ nullptr };
		esv::GameActionManager__AddAction esv__GameActionManager__AddGameAction{ nullptr };
		esv::TornadoAction__Setup esv__TornadoAction__Setup{ nullptr };
		esv::WallAction__CreateWall esv__WallAction__CreateWall{ nullptr };
		esv::SummonHelpers__Summon esv__SummonHelpers__Summon{ nullptr };
		esv::GameObjectMoveAction__Setup esv__GameObjectMoveAction__Setup{ nullptr };

		esv::StatusVMT const * esv__StatusHeal__VMT{ nullptr };
		esv::StatusVMT const * esv__StatusHit__VMT{ nullptr };
		esv::StatusHeal::EnterProc* esv__StatusHeal__Enter{ nullptr };
		esv::StatusHit::EnterProc* esv__StatusHit__Enter{ nullptr };
		esv::StatusHit::SetupProc* esv__StatusHit__Setup{ nullptr };
		esv::Status::GetEnterChanceProc* esv__Status__GetEnterChance{ nullptr };
		esv::Status::ShouldDeleteProc* esv__Status__ShouldDelete{ nullptr };

		esv::ParseItem esv__ParseItem{ nullptr };
		esv::CreateItemFromParsed esv__CreateItemFromParsed{ nullptr };
		esv::Inventory::Equip esv__Inventory__Equip{ nullptr };
		esv::ItemHelpers__GenerateTreasureItem* esv__ItemHelpers__GenerateTreasureItem{ nullptr };
		esv::CombineManager::ExecuteCombinationProc* esv__CombineManager__ExecuteCombination{ nullptr };

		esv::CustomStatsProtocol__ProcessMsg esv__CustomStatsProtocol__ProcessMsg{ nullptr };

		esv::ActionMachine::SetStateProc* ecl__ActionMachine__SetState{ nullptr };
		esv::ActionMachine::SetStateProc* esv__ActionMachine__SetState{ nullptr };
		esv::ActionMachine::ResetStateProc* ecl__ActionMachine__ResetState{ nullptr };
		esv::ActionMachine::ResetStateProc* esv__ActionMachine__ResetState{ nullptr };

		stats::RPGStats::LoadProc RPGStats__Load{ nullptr };
		stats::RPGStats::ParsePropertiesProc* RPGStats__ParseProperties{ nullptr };
		stats::SkillPrototypeManager** eoc__SkillPrototypeManager{ nullptr };
		stats::StatusPrototypeManager** eoc__StatusPrototypeManager{ nullptr };

		esv::ExecutePropertyDataOnGroundHitProc* esv__ExecutePropertyDataOnGroundHit{ nullptr };
		esv::ExecutePropertyDataOnPositionOnlyProc* esv__ExecutePropertyDataOnPositionOnly{ nullptr };
		esv::ExecuteCharacterSetExtraPropertiesProc* esv__ExecuteCharacterSetExtraProperties{ nullptr };

		esv::Character::HitProc* esv__Character__Hit{ nullptr };
		esv::Character::ApplyDamageProc* esv__Character__ApplyDamage{ nullptr };
		stats::Character::HitInternalProc* CDivinityStats_Character__HitInternal{ nullptr };

		esv::StatusMachine::CreateStatusProc* esv__StatusMachine__CreateStatus{ nullptr };
		esv::StatusMachine::ApplyStatusProc* esv__StatusMachine__ApplyStatus{ nullptr };
		esv::StatusMachine::UpdateProc* esv__StatusMachine__Update{ nullptr };
		esv::StatusMachine::DeleteStatusByHandleProc* esv__StatusMachine__DeleteStatusByHandle{ nullptr };

		esv::SurfaceActionFactory** esv__SurfaceActionFactory{ nullptr };
		esv::SurfaceActionFactory::CreateActionProc* esv__SurfaceActionFactory__CreateAction{ nullptr };
		esv::TransformSurfaceAction::InitProc* esv__TransformSurfaceAction__Init{ nullptr };
		eoc::SurfaceTransformInteractions* eoc__SurfaceTransformActionsFromType{ nullptr };
		SurfaceTemplate** eoc__SurfaceTemplates{ nullptr };

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
		ecl::GameStateMachine__Update ecl__GameStateMachine__Update{ nullptr };
		esv::GameStateMachine__Update esv__GameStateMachine__Update{ nullptr };
		ecl::GameStateLoad__IncLocalProgress ecl__GameStateLoad__IncLocalProgress{ nullptr };

		net::Host::AddProtocol net__Host__AddProtocol{ nullptr };
		net::MessageFactory::RegisterMessage net__MessageFactory__RegisterMessage{ nullptr };
		net::MessageFactory::GetFreeMessage net__MessageFactory__GetFreeMessage{ nullptr };

		stats::eoc__SkillPrototypeManager__Init SkillPrototypeManager__Init{ nullptr };
		stats::SkillPrototype::FormatDescriptionParamProc* SkillPrototype__FormatDescriptionParam{ nullptr };
		stats::SkillPrototype::GetSkillDamageProc* SkillPrototype__GetSkillDamage{ nullptr };
		stats::SkillPrototype::GetAttackAPCostProc* SkillPrototype__GetAttackAPCost{ nullptr };
		stats::StatusPrototype::FormatDescriptionParamProc* StatusPrototype__FormatDescriptionParam{ nullptr };

		esv::TurnManager::UpdateTurnOrderProc* esv__TurnManager__UpdateTurnOrder{ nullptr };
		eoc::AiGrid::SearchForCellProc* eoc__AiGrid__SearchForCell{ nullptr };

		UIObjectManager::RegisterUIObjectCreatorProc* UIObjectManager__RegisterUIObjectCreator{ nullptr };
		UIObjectManager::CreateUIObjectProc* UIObjectManager__CreateUIObject{ nullptr };
		UIObjectManager::DestroyUIObjectProc* UIObjectManager__DestroyUIObject{ nullptr };
		UIObjectManager::GetInstanceProc* UIObjectManager__GetInstance{ nullptr };
		UIObjectManager ** UIObjectManager__Instance{ nullptr };
		ecl::EoCUI::ctor ecl__EoCUI__ctor{ nullptr };
		UIObject::VMT * ecl__EoCUI__vftable{ nullptr };
		UIObject::GetUIScaleMultiplierProc* ls__UIObject__GetUIScaleMultiplier{ nullptr };
		UIObject::RaiseFlagProc* ls__UIObject__RaiseFlag{ nullptr };
		UIObject::RaiseFlagProc* ls__UIObject__ClearFlag{ nullptr };

		void* ls__CustomDrawStruct__VMT{ nullptr };
		CustomDrawStruct::UIClearIcon* ls__UIHelper__UIClearIcon{ nullptr };
		CustomDrawStruct::UICreateIconMesh* ls__UIHelper__UICreateIconMesh{ nullptr };
		ecl::FlashCustomDrawCallback::CustomDrawObject* ls__UIHelper__CustomDrawObject{ nullptr };

		DragDropManager** ls__DragDropManager{ nullptr };
		DragDropManager::GetInstanceProc* ls__DragDropManager__GetInstance{ nullptr };
		DragDropManager::StartDragStringProc* ls__DragDropManager__StartDragString{ nullptr };
		DragDropManager::StartDragHandleProc* ls__DragDropManager__StartDragHandle{ nullptr };
		DragDropManager::StopDragProc* ls__DragDropManager__StopDrag{ nullptr };
		ecl::DragDropManager** ecl__DragDropManager{ nullptr };

		TranslatedStringRepository ** TranslatedStringRepository__Instance{ nullptr };
		TranslatedStringRepository::GetInstance TranslatedStringRepository__GetInstance{ nullptr };
		TranslatedStringRepository::Get TranslatedStringRepository__Get{ nullptr };
		TranslatedStringRepository::UnloadOverrides TranslatedStringRepository__UnloadOverrides{ nullptr };

		TranslatedStringKeyManager** TranslatedStringKeyManager__Instance{ nullptr };
		TranslatedStringKeyManager::GetInstance TranslatedStringKeyManager__GetInstance{ nullptr };
		TranslatedStringKeyManager::GetTranlatedStringFromKey TranslatedStringKeyManager__GetTranlatedStringFromKey{ nullptr };

		eoc::SpeakerManager ** eoc__SpeakerManager{ nullptr };
		PlayerManager** ls__PlayerManager__Instance{ nullptr };
		void* esv__OsirisVariableHelper__SavegameVisit{ nullptr };
		ModManager::CollectAvailableMods ModManager__CollectAvailableMods{ nullptr };
		stats::ScriptCheckBlock__Build ls__ScriptCheckBlock__Build{ nullptr };

		character_creation::CharacterCreationManager** eoc__CharacterCreationManager{ nullptr };

		ecl::LevelManager ** ecl__LevelManager{ nullptr };
		ecl::InventoryFactory ** ecl__InventoryFactory{ nullptr };
		ecl::InventoryViewFactory** ecl__InventoryViewFactory{ nullptr };

		ecl::InventoryProtocol::PostUpdateProc* ecl__InventoryProtocol__PostUpdate{ nullptr };

		esv::LevelManager ** esv__LevelManager{ nullptr };
		esv::InventoryFactory ** esv__InventoryFactory{ nullptr };
		esv::InventoryViewFactory ** esv__InventoryViewFactory{ nullptr };

		esv::OsirisTaskFactory** esv__OsirisTaskFactory{ nullptr };
		esv::OsirisTaskFactory::CreateTaskProc* esv__OsirisTaskFactory__CreateTask{ nullptr };
		esv::OsirisTaskFactory::ReleaseTaskProc* esv__OsirisTaskFactory__ReleaseTask{ nullptr };

		esv::AiModifiers ** esv__gAiModifiers{ nullptr };
		esv::AiHelpers ** esv__gAiHelpers{ nullptr };

		esv::AiHelpers::PeekActionProc* esv__AiHelpers__PeekAction{ nullptr };
		esv::AiHelpers::SortActionsProc* esv__AiHelpers__SortActions{ nullptr };

		GlobalTemplateManager** ls__GlobalTemplateManager{ nullptr };
		GlobalTemplateManager::GetInstanceProc* ls__GlobalTemplateManager__GetInstance{ nullptr };
		GlobalCacheTemplateManager** esv__CacheTemplateManager{ nullptr };
		LocalTemplateManager** esv__LocalTemplateManager{ nullptr };
		GlobalCacheTemplateManager** ecl__CacheTemplateManager{ nullptr };

		CacheTemplateManagerBase::CacheTemplateProc* ls__CacheTemplateManagerBase__CacheTemplate{ nullptr };

		ecl::MultiEffectHandler::InitProc* ecl__MultiEffectHandler__Init{ nullptr };
		ecl::MultiEffectHandler::UpdateProc* ecl__MultiEffectHandler__Update{ nullptr };
		ecl::MultiEffectHandler::DeleteProc* ecl__MultiEffectHandler__Delete{ nullptr };

		Visual::AddAttachmentProc* ls__Visual__AddAttachment{ nullptr };

		void** ls__gEffectsManager{ nullptr };
		EffectsManager__GetInstanceProc* ls__EffectsManager__GetInstance{ nullptr };
		EffectsManager__DestroyEffect* ls__EffectsManager__DestroyEffect{ nullptr };

		esv::EffectManager::CreateEffectProc* esv__EffectManager__CreateEffect{ nullptr };
		esv::EffectManager::DestroyEffectProc* esv__EffectManager__DestroyEffect{ nullptr };

		ResourceManager::GetInstanceProc * ResourceManager__GetInstance{ nullptr };
		ResourceManager::InstantiateVisualProc* ResourceManager__InstantiateVisual{ nullptr };
		ResourceManager ** ResourceManager__Instance{ nullptr };
		VisualFactory::DestroyVisualProc* ls__VisualFactory__DestroyVisual{ nullptr };
		ecl::EquipmentVisualsSystem::CreateVisualsProc* ecl__EquipmentVisualsSystem__CreateVisuals{ nullptr };

#if defined(OSI_EOCAPP)
		GlobalSwitches ** ls__GlobalSwitches{ nullptr };
#else
		GlobalSwitches *** ls__GlobalSwitches{ nullptr };
#endif
		GraphicSettings** ls__GraphicSettings{ nullptr };
		void * CDivinityStats_Item__FoldDynamicAttributes{ nullptr };
		void * ls__ModuleSettings__HasCustomMods{ nullptr };
		void * ls__ModuleSettings__HasCustomModsGB5{ nullptr };
		ModuleSettings::ValidateProc* ModuleSettings__Validate{ nullptr };
		Module::HashProc* Module__Hash{ nullptr };
		esv::LoadProtocol::HandleModuleLoadedProc* esv__LoadProtocol__HandleModuleLoaded{ nullptr };

		InputManager** ls__InputManager{ nullptr };
		InputManager::GetInstanceProc* ls__InputManager__GetInstance{ nullptr };
		InputManager::InjectInputProc* ls__InputManager__InjectInput{ nullptr };

		rf::CameraManager** rf__CameraManager{ nullptr };
		rf::CameraManager::GetInstanceProc* rf__CameraManager__GetInstance{ nullptr };
		void* ecl__GameCamera__VMT{ nullptr };

		esv::AlignmentContainer** esv__AlignmentContainer{ nullptr };
		void* esv__AlignmentContainer__Load__Hook{ nullptr };

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

		ig::SetWarningCallbackProc IgSetWarningCallback{ nullptr };
		ig::SetTraceCallbackUTF8Proc IgSetTraceCallbackUTF8{ nullptr };

		void * DUMMY1{ nullptr };
		std::map<uint8_t const *, EoCLibraryInfo> Libraries;

		uint8_t const * ServerRegisterFuncs[50]{ nullptr };
		uint8_t const ** ServerGlobals[50]{ nullptr };
		uint8_t const * EocRegisterFuncs[6]{ nullptr };
		uint8_t const ** EocGlobals[6]{ nullptr };

		stats::CharacterStatsGetters CharStatsGetters;

		inline StaticSymbols() {}
		StaticSymbols(StaticSymbols const &) = delete;
		StaticSymbols & operator = (StaticSymbols const &) = delete;

		inline GlobalSwitches * GetGlobalSwitches() const
		{
#if defined(OSI_EOCAPP)
			if (ls__GlobalSwitches && *ls__GlobalSwitches) {
				return *ls__GlobalSwitches;
			} else {
				return nullptr;
			}
#else
			if (ls__GlobalSwitches && *ls__GlobalSwitches && **ls__GlobalSwitches) {
				return **ls__GlobalSwitches;
			} else {
				return nullptr;
			}
#endif
		}

		inline esv::InventoryFactory * GetServerInventoryFactory() const
		{
			if (esv__InventoryFactory) {
				return *esv__InventoryFactory;
			} else {
				return nullptr;
			}
		}

		inline esv::InventoryViewFactory * GetServerInventoryViewFactory() const
		{
			if (esv__InventoryViewFactory) {
				return *esv__InventoryViewFactory;
			} else {
				return nullptr;
			}
		}

		inline ecl::InventoryFactory* GetClientInventoryFactory() const
		{
			if (ecl__InventoryFactory) {
				return *ecl__InventoryFactory;
			} else {
				return nullptr;
			}
		}

		inline ecl::InventoryViewFactory * GetClientInventoryViewFactory() const
		{
			if (ecl__InventoryViewFactory) {
				return *ecl__InventoryViewFactory;
			} else {
				return nullptr;
			}
		}

		inline GlobalStringTable const * GetGlobalStringTable() const
		{
			if (ls__GlobalStrings) {
				return *ls__GlobalStrings;
			} else {
				return nullptr;
			}
		}

		inline esv::LevelManager* GetServerLevelManager() const
		{
			if (esv__LevelManager == nullptr
				|| *esv__LevelManager == nullptr) {
				return nullptr;
			}

			return *esv__LevelManager;
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
			if (ecl__LevelManager == nullptr
				|| *ecl__LevelManager == nullptr) {
				return nullptr;
			}

			return *ecl__LevelManager;
		}

		inline ecl::Level* GetCurrentClientLevel() const
		{
			auto levelMgr = GetClientLevelManager();
			if (levelMgr == nullptr) {
				return nullptr;
			}

			return levelMgr->CurrentLevel;
		}

		inline stats::RPGStats * GetStats() const
		{
			if (EocGlobals[5]) {
				return *(stats::RPGStats **)EocGlobals[5];
			} else {
				return nullptr;
			}
		}

		inline std::optional<ecl::GameState> GetClientState() const
		{
			if (ecl__EoCClient != nullptr
				&& *ecl__EoCClient != nullptr
				&& (*ecl__EoCClient)->GameStateMachine != nullptr
				&& *(*ecl__EoCClient)->GameStateMachine != nullptr) {
				return (*(*ecl__EoCClient)->GameStateMachine)->State;
			} else {
				return {};
			}
		}

		inline std::optional<esv::GameState> GetServerState() const
		{
			if (esv__EoCServer != nullptr
				&& *esv__EoCServer != nullptr
				&& (*esv__EoCServer)->StateMachine != nullptr) {
				return (*esv__EoCServer)->StateMachine->State;
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

		inline void* GetEffectsManager() const
		{
			if (ls__gEffectsManager) {
				return *ls__gEffectsManager;
			} else if (ls__EffectsManager__GetInstance) {
				return ls__EffectsManager__GetInstance();
			} else {
				return nullptr;
			}
		}

		inline GlobalTemplateManager* GetGlobalTemplateManager() const
		{
			if (ls__GlobalTemplateManager) {
				return *ls__GlobalTemplateManager;
			} else if (ls__GlobalTemplateManager__GetInstance) {
				return ls__GlobalTemplateManager__GetInstance();
			} else {
				return nullptr;
			}
		}

		inline DragDropManager* GetDragDropManager() const
		{
			if (ls__DragDropManager) {
				return *ls__DragDropManager;
			} else if (ls__DragDropManager__GetInstance) {
				return ls__DragDropManager__GetInstance();
			} else {
				return nullptr;
			}
		}

		inline InputManager* GetInputManager() const
		{
			if (ls__InputManager) {
				return *ls__InputManager;
			} else if (ls__InputManager__GetInstance) {
				return ls__InputManager__GetInstance();
			} else {
				return nullptr;
			}
		}

		inline rf::CameraManager* GetCameraManager() const
		{
			if (rf__CameraManager) {
				return *rf__CameraManager;
			} else if (rf__CameraManager__GetInstance) {
				return rf__CameraManager__GetInstance();
			} else {
				return nullptr;
			}
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
			if (esv__GameActionManager__AddGameAction != nullptr) {
#if defined(OSI_EOCAPP)
				esv__GameActionManager__AddGameAction(&mgr->GameActions, &action);
#else
				esv__GameActionManager__AddGameAction(mgr, action);
#endif
			}
		}

		void CanonicalizePath(STDString & path) const;
		STDString ToPath(StringView path, PathRootType root, bool canonicalize = true) const;
		FileReaderPin MakeFileReader(StringView path, PathRootType root = PathRootType::Data, bool canonicalize = true) const;
		void DestroyFileReader(FileReader * reader);
		bool FileExists(StringView path, PathRootType root = PathRootType::Data, bool canonicalize = true) const;
		GlobalTemplateBank* GetGlobalTemplateBank() const;
		ResourceBank* GetResourceBank() const;
	};

	extern StaticSymbols* gStaticSymbols;

	void InitStaticSymbols();

	inline StaticSymbols& GetStaticSymbols()
	{
		return *gStaticSymbols;
	}
}
