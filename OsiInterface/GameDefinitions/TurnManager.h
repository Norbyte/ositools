#pragma once

#include "BaseTypes.h"
#include "EntitySystem.h"

namespace osidbg
{

#pragma pack(push, 1)
	namespace eoc
	{
		struct alignas(4) CombatTeamId
		{
			uint32_t TeamId : 24;
			uint32_t CombatId : 8;
		};

		enum CombatComponentFlags : uint32_t
		{
			CCF_CombatDisabled_M = 0x1,
			CCF_CanGuard = 0x2,
			CCF_Guarded = 0x4,
			CCF_CanFight = 0x10,
			CCF_TurnEnded = 0x40,
			CCF_CanForceEndTurn = 0x100,
			CCF_HasNoCombatGroup = 0x200,
			CCF_EnteredCombat = 0x1000,
			CCF_TookExtraTurn = 0x2000,
			CCF_ShouldTakeExtraTurn1 = 0x4000,
			CCF_ShouldTakeExtraTurn2 = 0x8000,
			CCF_DelayDeathCount = 0x20000,
		};

		struct CombatComponent
		{
			void * VMT;
			BaseComponent Base;
			FixedString Alignment1;
			FixedString Alignment2;
			FixedString CombatGroupId;
			CombatTeamId CombatAndTeamIndex;
			uint16_t Initiative;
			bool HasAttackOfOpportunity;
			uint8_t _Pad;
			CombatComponentFlags Flags;
		};
	}

	namespace esv
	{
		struct TurnBasedProtocol
		{
			void * VMT;
			void * field_8;
			void * NetEventManagerVMT;
			void * field_18;
			void * GameEventManagerVMT;
			ObjectSet<uint8_t> UpdatedCombatIds;
			ObjectSet<uint8_t> PrimSet_uchar2;
			eocnet::Message *TurnCombatSetTeamMessage;
			eocnet::Message *FleeCombatResultMessage;
			eocnet::Message *CombatTurnOrderMessage;
			eocnet::Message *CombatSummonsMessage;
		};

		struct TurnManager
		{
			struct EntityWrapper
			{
				ObjectHandle EntityHandle;
				eoc::CombatComponent *CombatComponentPtr;
				Character * Character;
				struct Item * Item;
				uint8_t Flags;
				uint8_t _Pad[7];
			};

			struct CombatTeam
			{
				eoc::CombatTeamId TeamId;
				uint16_t Initiative;
				uint16_t CombatTeamRound;
				bool StillInCombat;
				bool AddedToTeamIdSet2;
				uint8_t _Pad[6];
				struct CombatGroup *CombatGroup;
				EntityWrapper EntityWrapper;
				ComponentHandle ComponentHandle;
			};

			struct CombatGroup
			{
				ObjectSet<eoc::CombatTeamId> CombatTeamsOrdered;
				uint16_t Initiative;
				char Party;
				uint8_t _Pad[5];
				uint64_t LastAddedTeamIndex;
			};

			struct Combat
			{
				ObjectSet<CombatTeam *> CurrentRoundTeams;
				ObjectSet<CombatTeam *> CombatTeams;
				ObjectSet<CombatGroup *> CombatGroups;
				ObjectSet<eoc::CombatTeamId> ValidEntityTeamIdSet;
				ObjectSet<eoc::CombatTeamId> TeamIdSet2;
				ObjectSet<void *> SummonDataSet;
				FixedStringRefMap<uint32_t, CombatTeam> CombatTeamRefMap;
				uint64_t NextTeamId;
				ObjectSet<ObjectHandle> WaitingForCharComponents;
				FixedStringRefMap<FixedString, void *> ObjHKRefMap;
				ObjectHandle InitialPlayerHandle;
				ObjectHandle InitialEnemyHandle;
				FixedString LevelName;
				float TimeSpentInTurn;
				float TimeLeft_M;
				float TimeSpentTryingToEndTurn;
				float TurnTimer_M;
				uint8_t HasParticipantSurfacesNumTicks;
				char CombatRound;
				bool IsActive;
				bool HasParticipantSurfaces;
				bool CombatStartEventSent;
				uint8_t NewRound;
				bool IsFightBetweenPlayers;
			};

			struct TimeoutOverride
			{
				ComponentHandle Handle;
				float Timeout;
			};

			void * VMT;
			BaseComponentProcessingSystem Base;
			ObjectSet<ComponentHandle> ComponentHandleSet;
			ObjectSet<ObjectHandle> EntityHandleSet;
			ObjectSet<ObjectHandle> EntityHandleSet2;
			ObjectSet<EntityWrapper> EntityWrapperSet;
			ObjectSet<ObjectHandle> EntitesLeftCombatHandleSet;
			int TeamMode;
			uint8_t _Pad1[4];
			FixedStringRefMap<uint8_t, Combat> Combats;
			ObjectSet<uint8_t> FreeIdSet;
			uint8_t NextCombatId;
			uint8_t _Pad2[7];
			FixedStringRefMap<uint8_t, TimeoutOverride> TimeoutOverrides;
			FixedStringMapBase<CombatGroup> CombatGroupInfos;
			uint8_t _Pad3[4];
			ObjectSet<void *> WaypointDistSet;
			ObjectSet<esv::Character *> CharacterPtrSet;
		};

	}
#pragma pack(pop)

}
