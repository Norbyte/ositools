#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Stats.h"
#include "Status.h"
#include "ActionMachine.h"

namespace dse
{
	struct CharacterTemplate;

#pragma pack(push, 1)
	namespace eoc
	{
		struct PlayerUpgrade : public ProtectedGameObject<PlayerUpgrade>
		{
			CDivinityStats_Character * Stats;
			uint32_t AttributePoints;
			uint32_t CombatAbilityPoints;
			uint32_t CivilAbilityPoints;
			uint32_t TalentPoints;
			uint32_t Unknown;
			uint8_t _Pad[4];
			ObjectSet<int> Attributes;
			ObjectSet<int> Abilities;
			BitArray<4> Talents;
			ObjectSet<uint16_t> Traits;
			bool IsCustom;
		};

		struct PlayerCustomData : public ProtectedGameObject<PlayerCustomData>
		{
			void* VMT;
			bool Initialized;
			bool CustomLookEnabled;
			uint8_t _Pad[6];
			STDWString Name;
			TranslatedString NameTranslated;
			FixedString ClassType;
			uint32_t SkinColor;
			uint32_t HairColor;
			uint32_t ClothColor1;
			uint32_t ClothColor2;
			uint32_t ClothColor3;
			bool IsMale;
			uint8_t _Pad2[3];
			FixedString Race;
			FixedString OriginName;
			FixedString Icon;
			FixedString MusicInstrument;
			FixedString OwnerProfileID;
			FixedString ReservedProfileID;
			FixedString AiPersonality;
			FixedString Speaker;
			ScratchBuffer CustomIconImg;
			uint8_t _Pad3[4];
		};
	}

	using PlayerId = int16_t;
	using InputPlayerIndex = uint32_t;

	struct PlayerManager
	{
		struct PlayerInfo
		{
			uint32_t PlayerId;
			uint8_t _Pad[4];
			void* RenderView;
		};

		void * VMT;
		uint16_t NextPlayerId;
		uint8_t _Pad0[6];
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
			uint8_t _Pad[6];
			ObjectSet<FixedString> Tags;
		};

		struct SkillInfo
		{
			float ScoreModifier;
			int32_t StartRound;
			int32_t MinimumImpact;
			bool OnlyCastOnSelf;
			uint8_t AIFlags; // Enum
			uint8_t _Pad[2];
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
			uint8_t _Pad[4];
			SkillInfo Info;
			ObjectSet<ObjectHandle> CauseList;
			ObjectHandle UnknownHandle;
			uint32_t Unknown1;
			uint8_t _Pad2[4];
			ObjectHandle OwnerHandle;
			FixedString SkillId;
			float ActiveCooldown;
			bool IsActivated;
			bool IsLearned;
			bool ZeroMemory;
			bool OncePerCombat;
			bool Unknown2;
			uint8_t _Pad3[3];
			int32_t NumCharges;
			uint8_t _Pad4[4];
			uint64_t Unknown3;
		};

		struct SkillManager : public ProtectedGameObject<SkillManager>
		{
			void * FreeSkillState;
			ObjectHandle OwnerHandle;
			Map<FixedString, Skill *> Skills;
			uint8_t _Pad[4];
			RefMap<FixedString, uint32_t> TimeItemAddedToSkillManager;
			bool IsLoading;
			uint8_t _Pad2[3];
			uint32_t FreeMemorySlots;
		};

		struct SkillBarItem : public ProtectedGameObject<SkillBarItem>
		{
			enum ItemType : uint32_t
			{
				kNone = 0,
				kSkill = 1,
				kItem = 2
			};

			ItemType Type;
			uint32_t _Pad;
			FixedString SkillOrStatId;
			ObjectHandle ItemHandle;
		};

		struct PlayerCustomData : public eoc::PlayerCustomData {};

		struct PlayerData : public ProtectedGameObject<PlayerData>
		{
			ObjectHandle SomeObjectHandle;
			ObjectSet<SkillBarItem> SkillBar;
			ObjectSet<uint32_t> LockedAbility;
			Map<FixedString, void *> ShapeShiftVariableManagers;
			uint8_t _Pad1[4];
			Map<FixedString, void *> ShapeShiftAttitudeMaps;
			uint8_t _Pad2[4];
			bool LevelUpMarker;
			uint8_t SelectedSkillSetIndex;
			uint8_t _Pad3[6];
			FixedString QuestSelected;
			PlayerCustomData CustomData;
			void * PreviousPickpocketTargets;
			ObjectHandle SomeObjectHandle2;
			ObjectSet<glm::vec3> PreviousPositions;
			uint32_t PreviousPositionId;
			bool HelmetOption;
			bool ShouldReevaluateSkillBar;
			uint8_t _Pad4[2];
			uint32_t Renown;
			uint32_t CachedTension;
			ObjectHandle SomeObjectHandle3;
			uint8_t HomesteadKeyState;
			uint8_t _Pad6[7];
			ObjectHandle RecruiterHandle;
			FixedString OriginalTemplate;
			FixedString Region;
		};


		struct Character : public IEoCServerObject
		{

			typedef void(*HitProc)(esv::Character* self, CDivinityStats_Character* attackerStats, CDivinityStats_Item* itemStats, DamagePairList* damageList,
				HitType hitType, bool noHitRoll, HitDamageInfo* damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List* skillProperties, HighGroundBonus highGroundFlag, bool procWindWalker, CriticalRoll criticalRoll);
			typedef void(*ApplyDamageProc)(esv::Character* self, HitDamageInfo& hit, uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection);

			Status* GetStatus(ObjectHandle handle, bool returnPending) const;
			Status* GetStatus(NetId handle) const;

			glm::vec3 WorldPos; // Saved
			uint32_t _Pad2;
			CharacterFlags Flags; // Saved
			uint32_t U2;
			uint32_t _Pad3;
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
			uint8_t _Pad4[3];
			uint64_t U4[3];
			float U41[3];
			float U42[3];
			eoc::Ai* AI;
			FixedString AnimationOverride;
			int32_t WalkSpeedOverride;
			int32_t RunSpeedOverride;
			ObjectSet<FixedString> VoiceSet;
			uint8_t NeedsUpdateCount;
			uint8_t ScriptForceUpdateCount;
			uint8_t ForceSynchCount;
			bool U5;
			uint8_t _Pad5[4];
			CDivinityStats_Character * Stats;
			ObjectHandle InventoryHandle;
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
			uint8_t _Pad61[4];
			FixedString SkillBeingPrepared;
			CharacterTemplate* CurrentTemplate2;
			uint32_t Dialog;
			bool IsDialogAiControlled;
			uint8_t U10[3];
			float LifeTime;
			float TurnTimer;
			float TriggerTrapsTimer;
			UserId UserID;
			uint32_t UserID2;
			uint32_t U12;
			ObjectHandle OwnerHandle;
			ObjectHandle FollowCharacterHandle;
			ObjectHandle EnemyCharacterHandle;
			ObjectHandle SpiritCharacterHandle;
			ObjectHandle CorpseCharacterHandle;
			ObjectHandle ObjectHandle6;
			ObjectSet<ObjectHandle> EnemyHandles;
			ObjectSet<void *> SurfacePathInfluenceSet; // Set<eoc::SurfacePathInfluence>
			ObjectSet<ObjectHandle> SummonHandles;
			void * PlanManager;
			float PartialAP;
			uint8_t AnimType;
			uint8_t DelayDeathCount;
			uint16_t _Pad6;
			Status * DelayedDyingStatus;
			ObjectSet<ObjectHandle> RegisteredTriggerHandles;
			ObjectSet<FixedString> RegisteredTriggers;
			PlayerData * PlayerData;
			eoc::PlayerUpgrade PlayerUpgrade;
			uint8_t _Pad62[7];
			uint32_t ServerControlRefCount;
			uint32_t U13;
			float U131;
			uint8_t _Pad63[4];
			STDWString * CustomDisplayName;
			void * StoryDisplayName;
			TranslatedString OriginalTransformDisplayName;
			int32_t MaxVitalityPatchCheck;
			int32_t MaxArmorPatchCheck;
			int32_t MaxMagicArmorPatchCheck;
			float _Pad7;
			FixedString AnimationSetOverride;
			ObjectHandle PartyHandle;
			ObjectSet<FixedString> CreatedTemplateItems;
			ObjectSet<FixedString> Treasures;
			FixedString CustomTradeTreasure;
			ObjectSet<FixedString> Tags;
			ObjectHandle CrimeHandle;
			ObjectHandle PreviousCrimeHandle;
			uint32_t CrimeState;
			uint32_t PreviousCrimeState;
			bool IsAlarmed;
			bool CrimeWarningsEnabled;
			bool CrimeInterrogationEnabled;
			bool _Pad8;
			uint32_t InvestigationTimer;
			ObjectSet<FixedString> DisabledCrime;
			uint64_t DamageCounter;
			uint64_t HealCounter;
			uint64_t KillCounter;
			ObjectHandle MovingCasterHandle;
			FixedString Archetype;
			FixedString EquipmentColor;
			FixedString ProjectileTemplate;
			uint32_t TimeElapsed;
			uint8_t _Pad81[4];
			ObjectSet<FixedString> PreferredAiTarget;
			void* CharacterBody;
			RefMap<FixedString, void *> U19;
			ObjectSet<FixedString> TagsFromItems;
			void * VisualSetIndices;
			bool ReadyCheckBlocked;
			bool CorpseLootable;
			uint8_t NumConsumables;
			uint8_t _Pad9;
			SurfaceType CustomBloodSurface;
			FixedString PreviousLevel;
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
			ObjectSet<uint32_t> Abilities_EAbility;
			Map<FixedString, void*> AttitudeOverrideMap; // ObjectHandleMap<int>*
			uint8_t _Pad0[4];
			char field_58;
			uint8_t _Pad1[7];
			__int64 field_60;
			char field_68;
			uint8_t _Pad2[7];
			PlayerCustomData CustomData;
			char field_1C8;
			uint8_t _Pad3[7];
			__int64 field_1D0;
			int NetID1;
			int NetID2;
			int field_1E0;
			int field_1E4;
			FixedString field_1E8;
			ObjectSet<FixedString> ObjSet_FS;
			char field_210;
			char field_211;
			int NetID3;
			uint8_t _Pad4[2];
			FixedString field_218;
			FixedString field_220;
		};



		struct Character : public IEocClientObject
		{
			Status* GetStatus(ObjectHandle statusHandle) const;
			Status* GetStatus(NetId handle) const;

			glm::vec3 WorldPos; // Saved
			uint32_t _Pad2;
			uint64_t Flags; // Saved
			uint32_t U2;
			uint32_t _Pad3;
			FixedString CurrentLevel; // Saved
			glm::mat3 WorldRot;
			float Scale;
			glm::vec3 Velocity;
			int field_34;
			int field_38;
			uint8_t gap3C[4];
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
			uint8_t gap84[4];
			CharacterTemplate* Template;
			CharacterTemplate* OriginalTemplate;
			CDivinityStats_Character* Stats;
			void* InventoryHandle;
			void* MovementMachine;
			void* ActionStateMachine;
			void* SteeringMachine;
			void* BehaviourMachine;
			__int64 field_C8;
			__int64 field_D0;
			__int64 field_D8;
			__int64 field_E0;
			__int64 field_E8;
			StatusMachine* StatusMachine;
			void* SkillManager;
			int field_100;
			uint8_t gap104[4];
			__int64 field_108;
			__int64 field_110;
			__int64 OwnerCharacterHandle;
			__int64 OH3;
			__int64 CorpseCharacterHandle;
			__int64 OH5;
			int field_138;
			uint8_t gap13C[4];
			__int64 OH6;
			__int64 OH7;
			__int64 ViewConeEffectHandle;
			__int64 field_158;
			__int64 field_160;
			ecl::PlayerData* PlayerData;
			__int64 field_170;
			int AttributeGrowthBonus;
			int CombatAbilityBonus;
			int CivilAbilityBonus;
			int TalentGrowthBonus;
			__int64 field_188;
			__int64 field_190;
			__int64 field_198;
			__int64 field_1A0;
			__int64 field_1A8;
			__int64 field_1B0;
			__int64 field_1B8;
			__int64 field_1C0;
			__int64 field_1C8;
			__int64 field_1D0;
			__int64 field_1D8;
			__int64 field_1E0;
			__int64 field_1E8;
			__int64 field_1F0;
			__int64 field_1F8;
			char field_200;
			uint8_t gap201[7];
			ObjectSet<void*> SurfacePathInfluences;
			ObjectSet<FixedString> Tags;
			__int64 field_248;
			float field_250;
			uint8_t gap254[4];
			ObjectHandle SoundObjectHandles[3];
			ObjectHandle OH9;
			ObjectHandle OH10;
			char PickpocketNLootingFlags;
			char Flags2;
			uint8_t gap282[2];
			int Flags3;
			char field_288;
			char field_289;
			uint8_t gap28A[6];
			TranslatedString* DisplayNameOverride;
			TranslatedString DisplayName1;
			TranslatedString DisplayName2;
			ObjectHandle OH11;
			float field_3F0;
			int SoundBoneIndex;
			int field_3F8;
			int field_3FC;
			ObjectSet<FixedString> FixedStrings2;
			FixedString AnimationSetOverride;
			__int64 field_428;
			__int64 field_430;
			__int64 OH12;
			__int64 field_440;
			void* ResourceTemplate2;
			char Cloth;
			uint8_t gap451[7];
			void* ResourceTemplate1;
			int field_460;
			uint8_t gap464[4];
			__int64 field_468;
			__int64 field_470;
			__int64 field_478;
			ObjectHandle OH13;
			FixedString FS_Base;
			FixedString FS3;
			int field_498;
			char field_49C;
			uint8_t gap49D[3];
			ObjectSet< ObjectHandle> ObjectHandles;
			Map<FixedString, void*> field_4C0;
			uint8_t gap4D4[4];
			__int64 field_4D8;
			__int64 field_4E0;
			ObjectSet<FixedString> ItemTags;
			void* VisualSetIndices;
			char field_510;
		};

	}
#pragma pack(pop)
}
