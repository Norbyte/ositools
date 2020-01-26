#pragma once

#include <GameDefinitions/Character.h>
#include <GameDefinitions/CustomStats.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/Osiris.h>
#include <GameDefinitions/Status.h>
#include <GameDefinitions/ActionMachine.h>

namespace osidbg
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
		ecl::GameStateEventManager__ExecuteGameStateChangedEvent GameStateChangedEvent{ nullptr };
		eoc__SkillPrototypeManager__Init SkillPrototypeManagerInit{ nullptr };
		SkillPrototype::FormatDescriptionParam SkillPrototypeFormatDescriptionParam{ nullptr };
		SkillPrototype::GetSkillDamage SkillPrototypeGetSkillDamage{ nullptr };
		StatusPrototype::FormatDescriptionParam StatusPrototypeFormatDescriptionParam{ nullptr };

		GlobalStringTable const ** GlobalStrings{ nullptr };
		ls__Path__GetPrefixForRoot GetPrefixForRoot{ nullptr };
		ls__FileReader__FileReader FileReaderCtor{ nullptr };
		ls__FileReader__Dtor FileReaderDtor{ nullptr };
		STDString ** PathRoots{ nullptr };

		void ** LevelManager{ nullptr };
		CharacterFactory ** EsvCharacterFactory{ nullptr };
		ItemFactory ** EsvItemFactory{ nullptr };
		esv::EoCServer ** EoCServer{ nullptr };

		std::map<uint8_t const *, EoCLibraryInfo> Libraries;

		uint8_t const * ServerRegisterFuncs[50]{ nullptr };
		uint8_t const ** ServerGlobals[50]{ nullptr };
		uint8_t const * EocRegisterFuncs[6]{ nullptr };
		uint8_t const ** EocGlobals[6]{ nullptr };

		EoCAllocFunc EoCAlloc{ nullptr };
		EoCFreeFunc EoCFree{ nullptr };
		CrtAllocFunc CrtAlloc{ nullptr };
		CrtFreeFunc CrtFree{ nullptr };


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
			if (LevelManager == nullptr || *LevelManager == nullptr) {
				return nullptr;
			}

			auto levelMgr = *LevelManager;
#if !defined(OSI_EOCAPP)
			auto l1 = ((uint64_t *)levelMgr)[16];
			auto l2 = *(uint64_t *)(l1 + 208);
			return (esv::GameActionManager *)l2;
#else
			auto l1 = ((uint64_t *)levelMgr)[1];
			auto l2 = *(uint64_t *)(l1 + 216);
			return (esv::GameActionManager *)l2;
#endif
		}

		inline CRPGStatsManager * GetStats() const
		{
			if (EocGlobals[5]) {
				return *(CRPGStatsManager **)EocGlobals[5];
			} else {
				return nullptr;
			}
		}

		std::string ToPath(std::string const & path, PathRootType root) const;
		FileReaderPin MakeFileReader(std::string const & path, PathRootType root = PathRootType::Data) const;
		void DestroyFileReader(FileReader * reader);
	};

	extern StaticSymbols gStaticSymbols;
}
