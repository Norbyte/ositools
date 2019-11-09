#pragma once

#include "DivInterface.h"
#include "Wrappers.h"
#include <optional>

namespace osidbg {


	struct Pattern
	{
		void FromString(std::string const & s);
		void FromRaw(const char * s);
		void Scan(uint8_t const * start, size_t length, std::function<void(uint8_t const *)> callback, bool multiple = true);

	private:
		struct PatternByte
		{
			uint8_t pattern;
			uint8_t mask;
		};

		std::vector<PatternByte> pattern_;

		bool MatchPattern(uint8_t const * start);
		void ScanPrefix1(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback, bool multiple);
		void ScanPrefix2(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback, bool multiple);
		void ScanPrefix4(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback, bool multiple);
	};

	uint8_t const * AsmCallToAbsoluteAddress(uint8_t const * call);
	uint8_t const * AsmLeaToAbsoluteAddress(uint8_t const * lea);

	class LibraryManager
	{
	public:
		bool FindLibraries();
		void PostStartupFindLibraries();
		void Cleanup();

		inline CRPGStatsManager * GetStats() const
		{
			if (eocGlobals_[5]) {
				return *(CRPGStatsManager **)eocGlobals_[5];
			} else {
				return nullptr;
			}
		}

		inline CharacterFactory * GetCharacterFactory() const
		{
			if (EsvCharacterFactory) {
				return *EsvCharacterFactory;
			}
			else {
				return nullptr;
			}
		}

		inline ItemFactory * GetItemFactory() const
		{
			if (EsvItemFactory) {
				return *EsvItemFactory;
			}
			else {
				return nullptr;
			}
		}

		inline GlobalStringTable const * GetGlobalStringTable() const
		{
			if (GlobalStrings) {
				return *GlobalStrings;
			}
			else {
				return nullptr;
			}
		}

		inline esv::GameActionManager * GetGameActionManager() const
		{
			if (LevelManager == nullptr || *LevelManager == nullptr) {
				return nullptr;
			}

			auto levelMgr = *LevelManager;
			if (coreLibStart_ == nullptr) {
				auto l1 = ((uint64_t *)levelMgr)[16];
				auto l2 = *(uint64_t *)(l1 + 208);
				return (esv::GameActionManager *)l2;
			}
			else {
				auto l1 = ((uint64_t *)levelMgr)[1];
				auto l2 = *(uint64_t *)(l1 + 216);
				return (esv::GameActionManager *)l2;
			}
		}

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
		esv::CustomStatsProtocol__ProcessMsg EsvCustomStatsProtocolProcessMsg { nullptr };

		uint8_t const * UICharacterSheetHook{ nullptr };
		uint8_t const * ActivateClientSystemsHook{ nullptr };
		uint8_t const * ActivateServerSystemsHook{ nullptr };
		uint8_t const * CustomStatUIRollHook{ nullptr };
		
		enum class StatusHealEnterTag {};
		HookableFunction<StatusHealEnterTag, bool (esv::Status *)> StatusHealEnter;

		enum class StatusHitEnterTag {};
		HookableFunction<StatusHitEnterTag, bool (esv::Status *)> StatusHitEnter;

		enum class CharacterHitTag {};
		WrappableFunction<CharacterHitTag, void (esv::Character * , CDivinityStats_Character *, CDivinityStats_Item *, DamagePairList *,
			HitType, bool, HitDamageInfo *, int, void *, HighGroundBonus, bool, CriticalRoll)> CharacterHitHook;

		enum class ApplyStatusTag {};
		WrappableFunction<ApplyStatusTag, void (esv::StatusMachine *, esv::Status *)> ApplyStatusHook;

	private:

#if defined(OSI_EOCAPP)
		bool FindEoCApp(uint8_t const * & start, size_t & size);
		void FindMemoryManagerEoCApp();
		void FindLibrariesEoCApp();
		void FindServerGlobalsEoCApp();
		void FindEoCGlobalsEoCApp();
		void FindGlobalStringTableEoCApp();
		void FindGameActionManagerEoCApp();
		void FindGameActionsEoCApp();
		void FindStatusMachineEoCApp();
		void FindStatusTypesEoCApp();
		void FindHitFuncsEoCApp();
		void FindItemFuncsEoCApp();
		void FindCustomStatsEoCApp();
#else
		bool FindEoCPlugin(uint8_t const * & start, size_t & size);
		void FindMemoryManagerEoCPlugin();
		void FindLibrariesEoCPlugin();
		void FindServerGlobalsEoCPlugin();
		void FindEoCGlobalsEoCPlugin();
		void FindGlobalStringTableCoreLib();
		void FindGameActionManagerEoCPlugin();
		void FindGameActionsEoCPlugin();
		void FindStatusMachineEoCPlugin();
		void FindStatusTypesEoCPlugin();
		void FindHitFuncsEoCPlugin();
		void FindItemFuncsEoCPlugin();
		void FindCustomStatsEoCPlugin();
#endif

		void EnableCustomStats();

		bool IsFixedStringRef(uint8_t const * ref, char const * str) const;

		struct EoCLibraryInfo
		{
			uint8_t const * initFunc;
			uint8_t const * freeFunc;
			uint32_t refs;
		};

		uint8_t const * moduleStart_;
		size_t moduleSize_;

		uint8_t const * coreLibStart_;
		size_t coreLibSize_;

		bool PostLoaded{ false };

		GlobalStringTable const ** GlobalStrings;

		void ** LevelManager;
		CharacterFactory ** EsvCharacterFactory;
		ItemFactory ** EsvItemFactory;

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
			// 0, 1
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

		std::map<uint8_t const *, EoCLibraryInfo> libraries_;
		uint8_t const * serverRegisterFuncs_[50]{ nullptr };
		uint8_t const ** serverGlobals_[50]{ nullptr };
		uint8_t const * eocRegisterFuncs_[6]{ nullptr };
		uint8_t const ** eocGlobals_[6]{ nullptr };
	};
}
