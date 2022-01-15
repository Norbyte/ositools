#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/EntitySystem.h>

namespace dse
{
	namespace eoc
	{
		struct CombatTeamId
		{
			union {
				struct {
					uint32_t TeamId : 24;
					uint32_t CombatId : 8;
				};
				uint32_t CombinedId;
			};

			inline CombatTeamId() : CombinedId(0)
			{}

			inline CombatTeamId(uint32_t id) : CombinedId(id)
			{}

			inline operator uint32_t () const
			{
				return CombinedId;
			}

			inline bool operator == (CombatTeamId const & o) const
			{
				return CombinedId == o.CombinedId;
			}

			inline bool operator != (CombatTeamId const & o) const
			{
				return CombinedId != o.CombinedId;
			}

			inline bool operator < (CombatTeamId const & o) const
			{
				return CombinedId < o.CombinedId;
			}
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
			CCF_CounterAttacked = 0x40000
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
			CombatComponentFlags Flags;
		};
	}

	namespace esv
	{
		struct TurnBasedProtocol
		{
			void * VMT;
			net::GameServer * GameServer;
			void * NetEventManagerVMT;
			void * field_18;
			void * GameEventManagerVMT;
			ObjectSet<uint8_t> UpdatedTurnOrderCombatIds;
			ObjectSet<uint8_t> UpdatedSummonCombatIds;
			net::Message * TurnCombatSetTeamMessage;
			net::Message * FleeCombatResultMessage;
			net::Message * CombatTurnOrderMessage;
			net::Message * CombatSummonsMessage;
		};

		struct TurnManager
		{
			using UpdateTurnOrderProc = void (esv::TurnManager * self, uint8_t combatId);

			struct EntityWrapper
			{
				EntityHandle Handle;
				eoc::CombatComponent *CombatComponentPtr;
				Character * Character;
				struct Item * Item;
				uint8_t Flags;

				esv::Character * GetCharacter() const
				{
					return GetEntityWorld()->GetCharacterComponentByEntityHandle(Handle);
				}

				esv::Item * GetItem() const
				{
					return GetEntityWorld()->GetItemComponentByEntityHandle(Handle);
				}

				eoc::CombatComponent * GetCombatComponent() const
				{
					return GetEntityWorld()->GetCombatComponentByEntityHandle(Handle);
				}
			};

			struct CombatGroup
			{
				ObjectSet<eoc::CombatTeamId> CombatTeamsOrdered;
				uint16_t Initiative;
				char Party;
				uint64_t LastAddedTeamIndex;
			};

			struct CombatTeam
			{
				eoc::CombatTeamId TeamId;
				uint16_t Initiative;
				uint16_t CombatTeamRound;
				bool StillInCombat;
				bool AddedNextTurnNotification;
				CombatGroup *CombatGroup;
				EntityWrapper EntityWrapper;
				ComponentHandleWithType ComponentHandle;
			};

			struct Combat
			{
				ObjectSet<CombatTeam *, MSVCMemoryAllocator> CurrentRoundTeams;
				ObjectSet<CombatTeam *, MSVCMemoryAllocator> NextRoundTeams;
				ObjectSet<CombatGroup *, MSVCMemoryAllocator> CombatGroups;
				ObjectSet<eoc::CombatTeamId> CurrentTurnChangeNotificationTeamIds;
				ObjectSet<eoc::CombatTeamId> NextTurnChangeNotificationTeamIds;
				ObjectSet<void *> SummonDataSet;
				RefMap<uint32_t, CombatTeam *> Teams;
				uint32_t NextTeamId;
				ObjectSet<ComponentHandle> WaitingForCharComponents;
				RefMap<FixedString, void *> ObjHKRefMap;
				ComponentHandle InitialPlayerHandle;
				ComponentHandle InitialEnemyHandle;
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
				ComponentHandleWithType Handle;
				float Timeout;
			};

			void * VMT;
			BaseComponentProcessingSystem Base;
			ObjectSet<ComponentHandleWithType> ComponentHandleSet;
			ObjectSet<ComponentHandle> EntityHandleSet;
			ObjectSet<ComponentHandle> EntityHandleSet2;
			ObjectSet<EntityWrapper> EntityWrapperSet;
			ObjectSet<ComponentHandle> EntitesLeftCombatHandleSet;
			int TeamMode;
			RefMap<uint8_t, Combat> Combats;
			ObjectSet<uint8_t> FreeIdSet;
			uint8_t NextCombatId;
			RefMap<uint8_t, TimeoutOverride> TimeoutOverrides;
			Map<FixedString, CombatGroup> CombatGroupInfos;
			ObjectSet<void *> WaypointDistSet;
			ObjectSet<esv::Character *> CharacterPtrSet;
		};

	}
}

namespace std
{
	template<> struct hash<dse::eoc::CombatTeamId>
	{
		typedef dse::eoc::CombatTeamId argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<uint32_t>{}(fn.CombinedId);
		}
	};
}
