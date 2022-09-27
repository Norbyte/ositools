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

			inline uint32_t GetTeamId()
			{
				return TeamId;
			}

			inline uint8_t GetCombatId()
			{
				return (uint8_t)CombatId;
			}
		};

		struct CombatComponent : public ProtectedGameObject<CombatComponent>
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
		struct CombatComponent : public eoc::CombatComponent
		{
			static constexpr auto ComponentPoolIndex = esv::EntityComponentIndex::Combat;
			static constexpr auto ObjectTypeIndex = ObjectHandleType::CombatComponent;
		};

		struct TurnBasedProtocol : public ProtectedGameObject<TurnBasedProtocol>
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

		struct TurnManager : public ProtectedGameObject<TurnManager>
		{
			using UpdateTurnOrderProc = void (esv::TurnManager * self, uint8_t combatId);

			struct EntityWrapper
			{
				EntityHandle Handle;
				eoc::CombatComponent *CombatComponentPtr;
				Character * Character;
				struct Item * Item;
				uint8_t Flags;

				inline esv::Character * GetCharacter() const
				{
					return GetEntityWorld()->GetComponent<esv::Character>(Handle);
				}

				inline esv::Item * GetItem() const
				{
					return GetEntityWorld()->GetComponent<esv::Item>(Handle);
				}

				inline eoc::CombatComponent * GetCombatComponent() const
				{
					return GetEntityWorld()->GetComponent<esv::CombatComponent>(Handle);
				}
			};

			struct CombatGroup
			{
				ObjectSet<eoc::CombatTeamId> CombatTeamsOrdered;
				uint16_t Initiative;
				uint8_t Party;
				uint64_t LastAddedTeamIndex;
			};

			struct CombatTeam
			{
				eoc::CombatTeamId Id;
				uint16_t Initiative;
				uint16_t CombatTeamRound;
				bool StillInCombat;
				bool AddedNextTurnNotification;
				CombatGroup *CombatGroup;
				EntityWrapper EntityWrapper;
				ComponentHandleWithType ComponentHandle;

				// For compatibility with extender v55
				inline uint32_t GetTeamId()
				{
					return Id.GetTeamId();
				}

				inline uint8_t GetCombatId()
				{
					return Id.GetCombatId();
				}

				inline Character* GetCharacter()
				{
					return EntityWrapper.GetCharacter();
				}

				inline Item* GetItem()
				{
					return EntityWrapper.GetItem();
				}
			};

			struct Combat : public ProtectedGameObject<Combat>
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
				uint8_t CombatRound;
				bool IsActive;
				bool HasParticipantSurfaces;
				bool CombatStartEventSent;
				uint8_t NewRound;
				bool IsFightBetweenPlayers;

				// For compatibility with extender v55
				ObjectSet<CombatTeam*, MSVCMemoryAllocator>* LuaGetCurrentTurnOrder()
				{
					return &CurrentRoundTeams;
				}

				ObjectSet<CombatTeam*, MSVCMemoryAllocator>* LuaGetNextTurnOrder()
				{
					return &NextRoundTeams;
				}

				void LuaUpdateCurrentTurnOrder(lua_State* L);
				void LuaUpdateNextTurnOrder(lua_State* L);
				void LuaUpdateTurnOrder(lua_State* L, int index, ObjectSet<esv::TurnManager::CombatTeam*, MSVCMemoryAllocator>& combatTeams,
					ObjectSet<eoc::CombatTeamId>& combatNotifies);

				RefMap<uint32_t, CombatTeam*>* LuaGetAllTeams()
				{
					return &Teams;
				}
			};

			struct TimeoutOverride
			{
				ComponentHandleWithType Handle;
				float Timeout;
			};

			void * VMT;
			BaseComponentProcessingSystem<EntityWorld> Base;
			ObjectSet<ComponentHandleWithType> AttachedCombatComponents;
			ObjectSet<EntityHandle> CombatEntities;
			ObjectSet<EntityHandle> CombatEntities2;
			ObjectSet<EntityWrapper> EntityWrappers;
			ObjectSet<ComponentHandle> EntitiesLeftCombat;
			int TeamMode;
			RefMap<uint8_t, Combat> Combats;
			ObjectSet<uint8_t> FreeCombatIds;
			uint8_t NextCombatId;
			RefMap<uint8_t, TimeoutOverride> TimeoutOverrides;
			Map<FixedString, CombatGroup> CombatGroups;
			ObjectSet<void *> WaypointDistSet;
			ObjectSet<esv::Character *> CombatParticipants;
		};

	}
}

BEGIN_NS(eoc)

struct CombatGroupInfo
{
	FixedString Id;
	TranslatedString Name;
};

END_NS()


BEGIN_NS(ecl)

struct GameEventListener
{
	virtual ~GameEventListener() = 0;
	virtual bool OnLevelLoad(Event const& e) = 0;
	virtual bool OnLevelActivate(Event const& e) = 0;
	virtual bool OnLevelUnload(Event const& e) = 0;
	virtual bool OnLevelDeactivate(Event const& e) = 0;
	virtual bool OnModuleLoad(Event const& e) = 0;
	virtual bool OnModuleUnload(Event const& e) = 0;
	virtual bool OnSessionLoad(Event const& e) = 0;
	virtual bool OnSessionUnload(Event const& e) = 0;
	virtual bool OnDifficultyChanged(Event const& e) = 0;
	virtual bool OnCharacterAddedToParty(Event const& e) = 0;
	virtual bool OnPartySwitched(Event const& e) = 0;
	virtual bool OnCharacterOwnerChanged(Event const& e) = 0;
	virtual bool OnCharacterTransformedEvent(Event const& e) = 0;
	virtual bool OnSplitscreenChanged(Event const& e) = 0;
	virtual bool OnCharacterAssigned(Event const& e) = 0;
};

struct TurnManager : public ProtectedGameObject<TurnManager>, public GameEventListener
{
	struct CombatOrderChange
	{
		uint32_t TeamId;
		bool field_4;
		int OrderIndex;
		int field_C;
	};

	struct Combat : public ProtectedGameObject<Combat>
	{
		ObjectSet<ComponentHandleWithType> CurrentRoundTeams;
		ObjectSet<ComponentHandleWithType> NextRoundTeams;
		ObjectSet<ComponentHandleWithType> CombatGroups;
		// TeamId -> Combat component handle
		RefMap<uint32_t, ComponentHandleWithType> Teams;
		eoc::CombatTeamId InitialPlayerTeamId;
		eoc::CombatTeamId InitialEnemyTeamId;
		FixedString LevelName;
		float TurnTimer;
		ObjectSet<CombatOrderChange> OrderChanges;
	};

	BaseComponentProcessingSystem<EntityWorld> Base;
	ObjectSet<EntityHandle> EntityHandles;
	RefMap<uint8_t, Combat> Combats;
	RefMap<PlayerId, NetId> PlayerIdToCombatantNetId;
	Map<FixedString, eoc::CombatGroupInfo> CombatGroups;
	__int64 field_68;
	bool CameraControl;
};

struct CombatComponent : public eoc::CombatComponent
{
	static constexpr auto ComponentPoolIndex = ecl::EntityComponentIndex::Combat;
	static constexpr auto ObjectTypeIndex = ObjectHandleType::CombatComponent;
};

END_NS()

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
