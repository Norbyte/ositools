#pragma once

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
		EsvInventoryFactory = 18
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
		EsvInventoryFactory = 22
	};

	struct StaticSymbols
	{
		struct EoCLibraryInfo
		{
			uint8_t const * initFunc;
			uint8_t const * freeFunc;
			uint32_t refs;
		};

		ls__FixedString__Create CreateFixedString{ nullptr };

		esv::ProjectileHelpers_ShootProjectile ShootProjectile{ nullptr };
		esv::GameActionManager__CreateAction CreateGameAction{ nullptr };
		esv::GameActionManager__AddAction AddGameAction{ nullptr };
		esv::TornadoAction__Setup TornadoActionSetup{ nullptr };
		esv::TornadoAction__Setup WallActionCreateWall{ nullptr };
		esv::SummonHelpers__Summon SummonHelpersSummon{ nullptr };
		esv::GameObjectMoveAction__Setup GameObjectMoveActionSetup{ nullptr };
		esv::StatusMachine__CreateStatus StatusMachineCreateStatus{ nullptr };
		esv::StatusMachine__ApplyStatus StatusMachineApplyStatus{ nullptr };
		esv::Character__Hit CharacterHit{ nullptr };
		CDivinityStats_Character::HitInternalProc CharacterHitInternal{ nullptr };
		esv::StatusVMT const * StatusHealVMT{ nullptr };
		esv::StatusVMT const * StatusHitVMT{ nullptr };
		esv::ParseItem ParseItem{ nullptr };
		esv::CreateItemFromParsed CreateItemFromParsed{ nullptr };
		esv::CustomStatsProtocol__ProcessMsg EsvCustomStatsProtocolProcessMsg{ nullptr };
		esv::ActionMachine::SetStateProc EclActionMachine__SetState{ nullptr };
		esv::ActionMachine::SetStateProc EsvActionMachine__SetState{ nullptr };

		ecl::EoCClient ** EoCClient{ nullptr };
		ecl::EoCClient__HandleError EoCClientHandleError{ nullptr };

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
		SkillPrototype::FormatDescriptionParam SkillPrototypeFormatDescriptionParam{ nullptr };
		SkillPrototype::GetSkillDamage SkillPrototypeGetSkillDamage{ nullptr };
		StatusPrototype::FormatDescriptionParam StatusPrototypeFormatDescriptionParam{ nullptr };
		esv::TurnManager::UpdateTurnOrder TurnManagerUpdateTurnOrder{ nullptr };
		esv::Inventory::Equip InventoryEquip{ nullptr };

		GlobalStringTable const ** GlobalStrings{ nullptr };
		ls__Path__GetPrefixForRoot GetPrefixForRoot{ nullptr };
		ls__FileReader__FileReader FileReaderCtor{ nullptr };
		ls__FileReader__Dtor FileReaderDtor{ nullptr };
		STDString ** PathRoots{ nullptr };

		UIObjectManager::RegisterUIObjectCreator UIObjectManager__RegisterUIObjectCreator{ nullptr };
		UIObjectManager::CreateUIObject UIObjectManager__CreateUIObject{ nullptr };
		UIObjectManager::DestroyUIObject UIObjectManager__DestroyUIObject{ nullptr };
		UIObjectManager::GetInstance UIObjectManager__GetInstance{ nullptr };
		UIObjectManager ** UIObjectManager__Instance{ nullptr };
		ecl::EoCUI::ctor EoCUI__ctor{ nullptr };
		UIObject::VMT * EoCUI__vftable{ nullptr };

		TranslatedStringRepository ** TranslatedStringRepository__Instance{ nullptr };
		TranslatedStringRepository::GetInstance TranslatedStringRepository__GetInstance{ nullptr };
		TranslatedStringRepository::Get TranslatedStringRepository__Get{ nullptr };

		eoc::SpeakerManager ** eoc__SpeakerManager{ nullptr };

		esv::LevelManager ** LevelManager{ nullptr };
		InventoryFactory ** EsvInventoryFactory{ nullptr };
		CharacterFactory ** EsvCharacterFactory{ nullptr };
		ItemFactory ** EsvItemFactory{ nullptr };
		esv::EoCServer ** EoCServer{ nullptr };
#if defined(OSI_EOCAPP)
		GlobalSwitches ** pGlobalSwitches{ nullptr };
#else
		GlobalSwitches *** pGlobalSwitches{ nullptr };
#endif
		void * ItemFoldDynamicAttributes{ nullptr };
		void * ModuleSettingsHasCustomMods{ nullptr };

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


		inline InventoryFactory * GetInventoryFactory() const
		{
			if (EsvInventoryFactory) {
				return *EsvInventoryFactory;
			} else {
				return nullptr;
			}
		}

		inline CharacterFactory * GetCharacterFactory() const
		{
			if (EsvCharacterFactory) {
				return *EsvCharacterFactory;
			} else {
				return nullptr;
			}
		}

		inline ItemFactory * GetItemFactory() const
		{
			if (EsvItemFactory) {
				return *EsvItemFactory;
			} else {
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

		inline esv::GameActionManager * GetGameActionManager() const
		{
			if (LevelManager == nullptr 
				|| *LevelManager == nullptr
				|| (*LevelManager)->CurrentLevel == nullptr) {
				return nullptr;
			}

			return (*LevelManager)->CurrentLevel->GameActionManager;
		}

		inline CRPGStatsManager * GetStats() const
		{
			if (EocGlobals[5]) {
				return *(CRPGStatsManager **)EocGlobals[5];
			} else {
				return nullptr;
			}
		}

		inline std::optional<ClientGameState> GetClientState() const
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

		inline std::optional<ServerGameState> GetServerState() const
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
