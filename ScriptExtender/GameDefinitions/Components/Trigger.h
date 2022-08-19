#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

namespace dse
{
	struct ITriggerData : public ProtectedGameObject<ITriggerData>
	{
		virtual ~ITriggerData() = 0;
		virtual int GetSyncBehavior() = 0;
		virtual ITriggerData* Clone() = 0;
		virtual bool Visit(ObjectVisitor* visitor) = 0;
	};

	struct SoundVolumeTriggerData : public ITriggerData
	{
		FixedString AmbientSound;
		float Occlusion;
		uint8_t AuxBus1;
		uint8_t AuxBus2;
		uint8_t AuxBus3;
		uint8_t AuxBus4;
	};

	struct DefaultSyncedTriggerData : public ITriggerData
	{
	};

	struct DefaultServerOnlyTriggerData : public ITriggerData
	{
	};

	struct StartTriggerData : public ITriggerData
	{
		uint8_t Player;
		uint8_t Team;
		float Angle;
	};

	struct TeleportTriggerData : public ITriggerData
	{
		float Angle;
		bool Zoom;
	};

	struct EventTriggerData : public ITriggerData
	{
		FixedString EnterEvent;
		FixedString LeaveEvent;
	};

	struct CrimeAreaTriggerData : public ITriggerData
	{
		int16_t CrimeArea;
	};

	struct CrimeRegionTriggerData : public ITriggerData
	{
		FixedString TriggerRegion;
	};

	struct MusicVolumeTriggerData : public ITriggerData
	{
		struct MusicEvent
		{
			bool TriggerOnEnter;
			bool TriggerOnlyOnce;
			bool IsStinger;
			FixedString MusicEvent;
			FixedString Bansuri;
			FixedString Cello;
			FixedString Tambura;
			FixedString Ud;
			FixedString OriginTheme;
			FixedString OriginOnly;
			bool OriginThemeAddInstrument;
		};

		ObjectSet<MusicEvent> MusicEvents;
	};


	struct AiHintAreaTriggerData : public ITriggerData
	{
		bool IsAnchor;
	};

	struct SecretRegionTriggerData : public ITriggerData
	{
		bool SecretRegionUnlocked;
	};

	struct StatsAreaTriggerData : public ITriggerData
	{
		int LevelOverride;
		int TreasureLevelOverride;
		Guid ParentGuid;
	};

	struct RegionTriggerData : public ITriggerData
	{
		bool RegionCameraLock;
		bool RegionCameraLockPos;
		bool RegionShroudGen;
		bool RegionShroudVisibleInWorld;
	};

	struct ExplorationTriggerData : public ITriggerData
	{
		int ExplorationReward;
	};

	struct AtmosphereTriggerData : public ITriggerData
	{
		ObjectSet<FixedString, GameMemoryAllocator, true> Atmospheres;
		float FadeTime;
	};



	struct Trigger : public IGameObject
	{
		virtual void SetTemplateWithID(FixedString const& tmpl, TemplateType templateType) = 0;
		virtual void CacheTemplatesIfNeeded(TemplateType templateType, LevelTemplate const* tmpl) = 0;
		virtual void SetVisual(Visual* visual) = 0;
		virtual void LoadVisual(FixedString const& visualResourceId) = 0;
		virtual void ReloadVisual(FixedString const& visualResourceId, bool) = 0;
		virtual void LoadSound() = 0;
		virtual void UnloadSound() = 0;
		virtual void Initialize() = 0;
		virtual void Terminate() = 0;
		virtual TriggerTypeId GetTriggerTypeID() = 0;
		virtual void Destroy() = 0;
		virtual void SavegameVisit(ObjectVisitor* visitor) = 0;
		virtual void Visit(void* visitor, bool) = 0;
		virtual void LoadStaticData() = 0;
		virtual void LoadFromTemplate() = 0;

		ComponentHandle Handle; // Defunct, use GetObjectHandle() instead
		FixedString UUID; // Defunct, use GetGuid() instead
		TriggerTemplate* Template;
		uint16_t field_20;
		uint16_t Flags;
		uint16_t SyncFlags;
		uint16_t field_26;
		glm::vec3 Translate;
		FixedString TriggerType;
		bool IsGlobal;
		glm::mat3 Rotate;
		glm::mat3 RotateTransposed;
		float Scale;
		void* Visual;
		FixedString Level;
		void* SyncDirtyListener;
	};

	struct AreaTrigger;

	struct AreaTriggerPhysics : public ProtectedGameObject<AreaTriggerPhysics>
	{
		void* VMT;
		AreaTrigger* Trigger;
		int AreaTypeId;
	};

	struct AreaTrigger : public Trigger
	{
		AreaTriggerPhysics* Physics;
		ObjectSet<ComponentHandle> ObjectsInTrigger;
	};

	struct AtmosphereTrigger : public AreaTrigger
	{
		ObjectSet<FixedString> AtmosphereResourceIds;
		FixedString CurrentAtmosphere;
		float FadeTime;
	}; 

	struct SoundStateSync
	{
		FixedString Switch;
		FixedString State;
		bool SyncNeeded;
	};

	struct SoundRTPCSync
	{
		FixedString RTPC;
		float Value;
		bool SyncNeeded;
	};

	struct SoundVolumeTrigger : public AreaTrigger
	{
		uint64_t field_D0;
		ObjectSet<SoundStateSync> SoundStateSyncs;
		uint64_t field_F8;
		ObjectSet<SoundRTPCSync> SoundRTPCSyncs;
	};
}

namespace dse::esv
{
	struct AreaTriggerBase : public AreaTrigger
	{
		BaseComponent Base;
		FixedString MyGuid;
		NetId NetID;
	};

	struct EocAreaTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::EoCAreaTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerEocAreaTrigger;
	};

	struct EventTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::EventTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerEventTrigger;

		FixedString EnterEvent;
		FixedString LeaveEvent;
	};

	struct CrimeAreaTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::CrimeAreaTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerCrimeAreaTrigger;

		int CrimeArea;
		FixedString field_108;
		__int64 field_110;
	};

	struct CrimeRegionTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::CrimeRegionTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerCrimeRegionTrigger;

		FixedString TriggerRegion;
	};

	struct MusicVolumeTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::MusicVolumeTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerMusicVolumeTrigger;

		struct Triggered
		{
			int16_t Slot;
			int16_t Index;
		};

		ObjectSet<MusicVolumeTriggerData::MusicEvent> MusicEvents;
		ObjectSet<Triggered> TriggeredBy;
	};

	struct SecretRegionTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::SecretRegionTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerSecretRegionTrigger;

		bool SecretRegionUnlocked;
	};

	struct RegionTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::RegionTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerRegionTrigger;

		bool field_100;
	};

	struct ExplorationTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::ExplorationTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerExplorationTrigger;

		ObjectSet<FixedString> OS_FS;
		int ExplorationReward;
		__int64 field_128;
	};

	struct PointTriggerBase : public dse::Trigger
	{
		BaseComponent Base;
		FixedString MyGuid;
		NetId NetID;
	};

	struct TeleportTrigger : public PointTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::TeleportTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerTeleportTrigger;

		float Angle;
		bool Zoom;
	};

	struct StartTrigger : public PointTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::StartTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerStartTrigger;

		uint8_t Player;
		uint8_t Team;
		float Angle;
	};

	struct EocPointTrigger : public PointTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::EoCPointTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerEocPointTrigger;
	};


	struct AIHintAreaTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::AIHintAreaTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerAIHintAreaTrigger;

		bool IsAnchor;
	};

	struct StatsAreaTrigger : public AreaTriggerBase
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::StatsAreaTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerStatsAreaTrigger;

		int LevelOverride;
		int TreasureLevelOverride;
		Guid ParentGuid;
	};

	struct AtmosphereTrigger : public dse::AtmosphereTrigger
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::AtmosphereTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerAtmosphereTrigger;

		BaseComponent Base;
		FixedString MyGuid;
		NetId NetID;
	};

	struct SoundVolumeTrigger : public dse::SoundVolumeTrigger
	{
		static constexpr auto ComponentPoolIndex = EntityComponentIndex::SoundVolumeTrigger;
		static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerSoundVolumeTrigger;

		BaseComponent Base;
		FixedString MyGuid;
		NetId NetID;
	};
}
