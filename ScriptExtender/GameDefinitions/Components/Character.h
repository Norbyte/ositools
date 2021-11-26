#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/GameObjects/ActionMachine.h>
#include <GameDefinitions/Sound.h>

namespace dse
{
	namespace eoc
	{
		struct PlayerUpgrade : public ProtectedGameObject<PlayerUpgrade>
		{
			stats::Character * Stats;
			uint32_t AttributePoints;
			uint32_t CombatAbilityPoints;
			uint32_t CivilAbilityPoints;
			uint32_t TalentPoints;
			uint32_t Unknown;
			ObjectSet<int> Attributes;
			ObjectSet<int> Abilities;
			stats::TalentArray Talents;
			ObjectSet<uint16_t> Traits;
			bool IsCustom;
		};

		struct PlayerCustomData : public ProtectedGameObject<PlayerCustomData>
		{
			void* VMT;
			bool Initialized;
			bool CustomLookEnabled;
			STDWString Name;
			TranslatedString NameTranslated;
			FixedString ClassType;
			uint32_t SkinColor;
			uint32_t HairColor;
			uint32_t ClothColor1;
			uint32_t ClothColor2;
			uint32_t ClothColor3;
			bool IsMale;
			FixedString Race;
			FixedString OriginName;
			FixedString Icon;
			FixedString MusicInstrument;
			FixedString OwnerProfileID;
			FixedString ReservedProfileID;
			FixedString AiPersonality;
			FixedString Speaker;
			ScratchBuffer CustomIconImg;
		};
	}

	using PlayerId = int16_t;
	using InputPlayerIndex = uint32_t;

	struct PlayerManager
	{
		struct PlayerInfo
		{
			uint32_t PlayerId;
			void* RenderView;
		};

		void * VMT;
		uint16_t NextPlayerId;
		ObjectSet<PlayerId> FreePlayerIds;
		ObjectSet<PlayerId> PlayerList;
		RefMap<PlayerId, PlayerInfo> PlayerIds;
		RefMap<InputPlayerIndex, PlayerId> InputPlayerIndices;
		ObjectSet<void *> RenderViews; // rf::RenderView*
	};


	namespace esv
	{

		struct SkillConditions
		{
			int32_t MinimumHealthPercentage;
			int32_t MaximumHealthPercentage;
			bool HasNoPhysicalArmor;
			bool HasNoMagicalArmor;
			ObjectSet<FixedString> Tags;
		};

		struct SkillInfo
		{
			float ScoreModifier;
			int32_t StartRound;
			int32_t MinimumImpact;
			bool OnlyCastOnSelf;
			uint8_t AIFlags; // Enum
			SkillConditions SourceConditions;
			SkillConditions TargetConditions;
			bool CasualExplorer;
			bool Classic;
			bool TacticianHardcore;
			bool HonorHardcore;
			uint32_t Unknown;
		};

		struct Skill : public ProtectedGameObject<Skill>
		{
			void * VMT;
			FixedString UnknownFS;
			NetId NetID;
			SkillInfo Info;
			ObjectSet<ComponentHandle> CauseList;
			ComponentHandle UnknownHandle;
			uint32_t Unknown1;
			ComponentHandle OwnerHandle;
			FixedString SkillId;
			float ActiveCooldown;
			bool IsActivated;
			bool IsLearned;
			bool ZeroMemory;
			bool OncePerCombat;
			bool Unknown2;
			int32_t NumCharges;
			uint64_t Unknown3;
		};

		struct SkillManager : public ProtectedGameObject<SkillManager>
		{
			void * FreeSkillState;
			ComponentHandle OwnerHandle;
			Map<FixedString, Skill *> Skills;
			RefMap<FixedString, uint32_t> TimeItemAddedToSkillManager;
			bool IsLoading;
			uint32_t FreeMemorySlots;
		};

		struct SkillBarItem : public ProtectedGameObject<SkillBarItem>
		{
			SkillBarItemType Type;
			FixedString SkillOrStatId;
			ComponentHandle ItemHandle;
		};

		struct PlayerCustomData : public eoc::PlayerCustomData {};

		struct PlayerData : public ProtectedGameObject<PlayerData>
		{
			struct PickpocketData
			{
				int64_t Weight;
				int64_t Value;
			};

			ComponentHandle SomeObjectHandle;
			ObjectSet<SkillBarItem> SkillBar;
			ObjectSet<uint32_t> LockedAbility;
			Map<FixedString, void *> ShapeShiftVariableManagers;
			Map<FixedString, void *> ShapeShiftAttitudeMaps;
			bool LevelUpMarker;
			uint8_t SelectedSkillSetIndex;
			FixedString QuestSelected;
			PlayerCustomData CustomData;
			RefMap<ComponentHandle, PickpocketData> * PreviousPickpocketTargets;
			ComponentHandle PickpocketTarget;
			ObjectSet<glm::vec3> PreviousPositions;
			uint32_t PreviousPositionId;
			bool HelmetOption;
			bool ShouldReevaluateSkillBar;
			uint32_t Renown;
			uint32_t CachedTension;
			ComponentHandle SomeObjectHandle3;
			uint8_t HomesteadKeyState;
			ComponentHandle RecruiterHandle;
			FixedString OriginalTemplate;
			FixedString Region;
		};


		struct Character : public IEoCServerObject
		{

			using HitProc = void (esv::Character* self, stats::Character* attackerStats, stats::Item* itemStats, stats::DamagePairList* damageList,
				stats::HitType hitType, bool noHitRoll, stats::HitDamageInfo* damageInfo, int forceReduceDurability, stats::PropertyList* skillProperties, stats::HighGroundBonus highGroundFlag, bool procWindWalker, stats::CriticalRoll criticalRoll);
			using ApplyDamageProc = void (esv::Character* self, stats::HitDamageInfo& hit, uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection);

			Status* GetStatus(ComponentHandle handle, bool returnPending, bool returnUnapplied = false) const;
			Status* GetStatus(NetId handle) const;

			glm::vec3 WorldPos; // Saved
			CharacterFlags Flags; // Saved
			uint32_t U2;
			FixedString CurrentLevel; // Saved
			glm::mat3 WorldRot;
			float Scale;
			ObjectSet<void *> PeerIDClassNames;
			CharacterTemplate* CurrentTemplate;
			CharacterTemplate* OriginalTemplate;
			CharacterTemplate* TemplateUsedForSkills;
			CharacterFlags2 Flags2;
			CharacterFlags3 Flags3;
			uint8_t FlagsEx;
			uint8_t Team;
			uint8_t Color;
			uint64_t U4[3];
			float U41[3];
			float U42[3];
			eoc::Ai* AI;
			FixedString AnimationOverride;
			float WalkSpeedOverride;
			float RunSpeedOverride;
			ObjectSet<FixedString> VoiceSet;
			uint8_t NeedsUpdateCount;
			uint8_t ScriptForceUpdateCount;
			uint8_t ForceSynchCount;
			bool U5;
			stats::Character * Stats;
			ComponentHandle InventoryHandle;
			void * MovementMachine;
			esv::ActionMachine * ActionMachine;
			void * SteeringMachine;
			void * BehaviourMachine;
			void * CharacterSupervisor;
			void * NetworkController;
			void * OsirisController;
			void * RequestController;
			void * StatusController;
#if !defined(OSI_EOCAPP)
			uint64_t U8[2];
#endif
			void * ScriptController;
			void * DialogController;
			StatusMachine * StatusMachine;
			SkillManager * SkillManager;
			void * VariableManager;
			void * ShapeShiftVariableManager;
			Map<FixedString, void *> Attitudes; // Element type unknown
			FixedString SkillBeingPrepared;
			void* CurrentTemplate2; // Not a RootTemplate!
			uint32_t Dialog;
			bool IsDialogAiControlled;
			uint8_t U10[3];
			float LifeTime;
			float TurnTimer;
			float TriggerTrapsTimer;
			UserId UserID;
			UserId ReservedUserID;
			uint32_t U12;
			ComponentHandle OwnerHandle;
			ComponentHandle FollowCharacterHandle;
			ComponentHandle EnemyCharacterHandle;
			ComponentHandle SpiritCharacterHandle;
			ComponentHandle CorpseCharacterHandle;
			ComponentHandle ObjectHandle6;
			ObjectSet<ComponentHandle> EnemyHandles;
			ObjectSet<void *> SurfacePathInfluenceSet; // Set<eoc::SurfacePathInfluence>
			ObjectSet<ComponentHandle> SummonHandles;
			void * PlanManager;
			float PartialAP;
			uint8_t AnimType;
			uint8_t DelayDeathCount;
			Status * DelayedDyingStatus;
			ObjectSet<ComponentHandle> RegisteredTriggerHandles;
			ObjectSet<FixedString> RegisteredTriggers;
			PlayerData * PlayerData;
			eoc::PlayerUpgrade PlayerUpgrade;
			uint32_t ServerControlRefCount;
			float NoiseTimer;
			float U131;
			STDWString * CustomDisplayName;
			void * StoryDisplayName;
			TranslatedString OriginalTransformDisplayName;
			int32_t MaxVitalityPatchCheck;
			int32_t MaxArmorPatchCheck;
			int32_t MaxMagicArmorPatchCheck;
			FixedString AnimationSetOverride;
			ComponentHandle PartyHandle;
			ObjectSet<FixedString> CreatedTemplateItems;
			ObjectSet<FixedString> Treasures;
			FixedString CustomTradeTreasure;
			ObjectSet<FixedString> Tags;
			ComponentHandle CrimeHandle;
			ComponentHandle PreviousCrimeHandle;
			uint32_t CrimeState;
			uint32_t PreviousCrimeState;
			bool IsAlarmed;
			bool CrimeWarningsEnabled;
			bool CrimeInterrogationEnabled;
			uint32_t InvestigationTimer;
			ObjectSet<FixedString> DisabledCrime;
			uint64_t DamageCounter;
			uint64_t HealCounter;
			uint64_t KillCounter;
			ComponentHandle MovingCasterHandle;
			FixedString Archetype;
			FixedString EquipmentColor;
			FixedString ProjectileTemplate;
			uint32_t TimeElapsed;
			ObjectSet<FixedString> PreferredAiTarget;
			void* CharacterBody;
			RefMap<ComponentHandle, ObjectSet<stats::PropertyStatus*>> StatusesFromItems;
			ObjectSet<FixedString> TagsFromItems;
			void * VisualSetIndices;
			bool ReadyCheckBlocked;
			bool CorpseLootable;
			uint8_t NumConsumables;
			SurfaceType CustomBloodSurface;
			FixedString PreviousLevel;

			// For compatibility with v55
			ObjectSet<FixedString> GetInventoryItemGuids();
			ObjectSet<Item*> GetInventoryItems();
			ObjectSet<FixedString> GetNearbyCharacters(float distance);
			ObjectSet<FixedString> GetSummonGuids();
			ObjectSet<Character*> GetSummons();
			ObjectSet<FixedString> GetSkillIds();
			Skill* GetSkillInfo(FixedString const& skill);
			std::optional<int> GetCustomStatValue(FixedString const& statId);
			bool SetCustomStatValue(FixedString const& statId, int value);
		};
	}

	namespace ecl 
	{
		struct PlayerManager : public dse::PlayerManager
		{
			void* VMT_IProfileSelector;
			void* VMT_EventListener;
			void* VMT_PlayerManager2;
			RefMap<PlayerId, FixedString> PlayerIdToProfileGuid;
			RefMap<PlayerId, NetId> PlayerIdToNetIdMap;
			ObjectSet<InputPlayerIndex> InputPlayerIndices;
			uint64_t Unknown[3];
			ObjectSet<void*> field_F0;
			uint64_t Unknown2[6];
		};

		struct PlayerCustomData : public eoc::PlayerCustomData {};

		struct PlayerData : public ProtectedGameObject<PlayerData>
		{
			ObjectSet<void*> SkillBarItems; // ecl::SkillBarItem
			ObjectSet<uint32_t> LockedAbilities;
			Map<FixedString, void*> AttitudeOverrideMap; // ObjectHandleMap<int>*
			uint8_t SelectedSkillSet;
			__int64 field_60;
			char field_68;
			PlayerCustomData CustomData;
			char field_1C8;
			__int64 field_1D0;
			NetId PickpocketTargetNetID;
			NetId CorpseLootTargetNetID;
			bool HelmetOptionState;
			bool ArmorOptionState;
			int CachedTension;
			FixedString QuestSelected;
			ObjectSet<FixedString> MemorisedSkills;
			char field_210;
			char field_211;
			int NetID3;
			FixedString OriginalTemplate;
			FixedString Region;
		};



		struct Character : public IEocClientObject
		{
			Status* GetStatus(ComponentHandle statusHandle) const;
			Status* GetStatus(NetId handle) const;

			glm::vec3 WorldPos; // Saved
			uint32_t _Pad2;
			uint64_t Flags; // Saved
			uint32_t U2;
			FixedString CurrentLevel; // Saved
			glm::mat3 WorldRot;
			float Scale;
			glm::vec3 Velocity;
			int field_34;
			int field_38;
			uint64_t field_40;
			__int64 field_48;
			__int64 field_50;
			void* PhysicsObject;
			void* Light;
			void* AiObject;
			int field_70;
			UserId UserID;
			int32_t UserId2;
			NetId NetID2;
			NetId NetID3;
			CharacterTemplate* Template;
			CharacterTemplate* OriginalTemplate;
			Character* Stats;
			ComponentHandle InventoryHandle;
			void* MovementMachine;
			void* ActionStateMachine;
			void* SteeringMachine;
			void* BehaviourMachine;
			void* InputController;
			void* NetworkController;
			void* StatusController;
			void* DialogController;
			void* CharacterSupervisir;
			StatusMachine* StatusMachine;
			void* SkillManager;
			int field_100;
			__int64 field_108;
			void* CharacterBody;
			ComponentHandle OwnerCharacterHandle;
			ComponentHandle OH3;
			ComponentHandle CorpseCharacterHandle;
			ComponentHandle OH5;
			int field_138;
			ComponentHandle HighlightCircleEffect;
			ComponentHandle OH7;
			ComponentHandle ViewConeEffectHandle;
			__int64 field_158;
			__int64 field_160;
			ecl::PlayerData* PlayerData;
			eoc::PlayerUpgrade PlayerUpgrade;
			ObjectSet<void*> SurfacePathInfluences;
			ObjectSet<FixedString> Tags;
			__int64 field_248;
			float field_250;
			SoundObjectId SoundObjectHandles[3];
			ComponentHandle OH9;
			ComponentHandle FollowCharacterHandle;
			char PickpocketNLootingFlags;
			char Flags2;
			int Flags3;
			char field_288;
			char field_289;
			TranslatedString* DisplayNameOverride;
			TranslatedString StoryDisplayName;
			TranslatedString OriginalDisplayName;
			ComponentHandle TalkingIconEffect;
			float field_3F0;
			int SoundBoneIndex;
			int field_3F8;
			int field_3FC;
			ObjectSet<FixedString> FixedStrings2;
			FixedString AnimationSetOverride;
			float WalkSpeedOverride;
			float RunSpeedOverride;
			__int64 field_430;
			__int64 OH12;
			__int64 field_440;
			void* ResourceTemplate2;
			char Cloth;
			void* ResourceTemplate1;
			int field_460;
			__int64 field_468;
			__int64 field_470;
			__int64 field_478;
			ComponentHandle OH13;
			FixedString Archetype;
			FixedString FS3;
			int field_498;
			char field_49C;
			ObjectSet<ComponentHandle> ObjectHandles;
			Map<FixedString, void*> field_4C0;
			__int64 field_4D8;
			__int64 field_4E0;
			ObjectSet<FixedString> ItemTags;
			void* VisualSetIndices;
			bool CorpseLootable;
		};

	}
}
