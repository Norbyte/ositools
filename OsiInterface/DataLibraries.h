#pragma once

#include "DivInterface.h"
#include "Wrappers.h"
#include <optional>

namespace osidbg {

	typedef void (* ProjectileHelpers_ShootProjectile)(void * ShootProjectileHelper, float Unknown);
	typedef void * (* GameActionManager__CreateAction)(EsvGameActionManager * GameActionManager, GameActionType actionId, uint64_t SomeHandle);
	typedef void (* GameActionManager__AddAction)(EsvGameActionManager * GameActionManager, void * Action);
	typedef void (* TornadoAction__Setup)(void * TornadoAction);
	typedef void (* GameObjectMoveAction__Setup)(void * Action, ObjectHandle & ObjectToMove, glm::vec3 * TargetPosition);
	typedef void (* SummonHelpers__Summon)(SummonHelperResults * Results, SummonHelperSummonArgs * Args);
	typedef EsvStatus * (* StatusMachine__CreateStatus)(void * StatusMachine, FixedString & StatusId, uint64_t ObjectHandle);
	typedef void (* StatusMachine__ApplyStatus)(void * StatusMachine, EsvStatus * Status);
	typedef void (* Character__Hit)(EsvCharacter * self, void * attackerStats, CDivinityStats_Item * itemStats, Array<TDamagePair> * damageList, 
		uint32_t hitType, bool rollForDamage, HitDamageInfo * damageInfo, int aiTest_M, void * skillProperties, HighGroundBonus highGroundFlag, bool procWindWalker, CriticalRoll criticalRoll);
	typedef bool (* Status__Enter)(EsvStatus * Status);

	

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

		inline EsvGameActionManager * GetGameActionManager() const
		{
			if (LevelManager == nullptr || *LevelManager == nullptr) {
				return nullptr;
			}

			auto levelMgr = *LevelManager;
			if (coreLibStart_ == nullptr) {
				auto l1 = ((uint64_t *)levelMgr)[16];
				auto l2 = *(uint64_t *)(l1 + 208);
				return (EsvGameActionManager *)l2;
			}
			else {
				auto l1 = ((uint64_t *)levelMgr)[1];
				auto l2 = *(uint64_t *)(l1 + 216);
				return (EsvGameActionManager *)l2;
			}
		}

		ProjectileHelpers_ShootProjectile ShootProjectile{ nullptr };
		GameActionManager__CreateAction CreateGameAction{ nullptr };
		GameActionManager__AddAction AddGameAction{ nullptr };
		TornadoAction__Setup TornadoActionSetup{ nullptr };
		TornadoAction__Setup WallActionCreateWall{ nullptr };
		SummonHelpers__Summon SummonHelpersSummon{ nullptr };
		GameObjectMoveAction__Setup GameObjectMoveActionSetup{ nullptr };
		StatusMachine__CreateStatus StatusMachineCreateStatus{ nullptr };
		StatusMachine__ApplyStatus StatusMachineApplyStatus{ nullptr };
		Character__Hit CharacterHit{ nullptr };
		EsvStatusVMT const * StatusHealVMT{ nullptr };
		EsvStatusVMT const * StatusHitVMT{ nullptr };
		
		enum class StatusHealEnterTag {};
		HookableFunction<StatusHealEnterTag, bool (EsvStatus *)> StatusHealEnter;

		enum class StatusHitEnterTag {};
		HookableFunction<StatusHitEnterTag, bool (EsvStatus *)> StatusHitEnter;

	private:
		bool FindEoCPlugin(uint8_t const * & start, size_t & size);
		bool FindEoCApp(uint8_t const * & start, size_t & size);

		void FindLibrariesEoCPlugin();
		void FindLibrariesEoCApp();

		void FindServerGlobalsEoCPlugin();
		void FindServerGlobalsEoCApp();

		void FindEoCGlobalsEoCPlugin();
		void FindEoCGlobalsEoCApp();

		void FindGlobalStringTableCoreLib();
		void FindGlobalStringTableEoCApp();

		void FindGameActionManagerEoCPlugin();
		void FindGameActionManagerEoCApp();

		void FindGameActionsEoCPlugin();
		void FindGameActionsEoCApp();

		void FindStatusMachineEoCPlugin();
		void FindStatusMachineEoCApp();

		void FindStatusTypesEoCPlugin();
		void FindStatusTypesEoCApp();

		void FindHitFuncsEoCPlugin();
		void FindHitFuncsEoCApp();

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
