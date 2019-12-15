#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Stats.h"
#include "Status.h"

namespace osidbg
{

#pragma pack(push, 1)
	namespace eoc
	{
		struct PlayerUpgrade
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
	}

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

		struct Skill
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
			uint32_t NumCharges;
			uint8_t _Pad4[4];
			uint64_t Unknown3;
		};

		struct SkillManager
		{
			void * FreeSkillState;
			ObjectHandle OwnerHandle;
			FixedStringMapBase<Skill *> Skills;
			uint8_t _Pad[4];
			FixedStringRefMap<uint32_t> TimeItemAddedToSkillManager;
			bool IsLoading;
			uint8_t _Pad2[3];
			uint32_t SomeCount;
		};

		struct SkillBarItem
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

		struct PlayerCustomData
		{
			void * VMT;
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
			void * CustomIconImg;
		};

		struct PlayerData
		{
			ObjectHandle SomeObjectHandle;
			ObjectSet<SkillBarItem> SkillBar;
			ObjectSet<uint32_t> LockedAbility;
			FixedStringMapBase<void *> ShapeShiftVariableManagers;
			uint8_t _Pad1[4];
			FixedStringMapBase<void *> ShapeShiftAttitudeMaps;
			uint8_t _Pad2[4];
			bool LevelUpMarker;
			uint8_t SelectedSkillSetIndex;
			uint8_t _Pad3[6];
			FixedString QuestSelected;
			PlayerCustomData CustomData;
			uint64_t Unkn1[3];
			void * PreviousPickpocketTargets;
			ObjectHandle SomeObjectHandle2;
			ObjectSet<glm::vec3> PreviousPositions;
			uint32_t PreviousPositionId;
			bool HelmetOption;
			bool Dirty;
			uint8_t _Pad4[2];
			uint32_t Renown;
			uint8_t CachedTension;
			uint8_t _Pad5[3];
			ObjectHandle SomeObjectHandle3;
			uint8_t HomesteadKeyState;
			uint8_t _Pad6[7];
			ObjectHandle RecruiterHandle;
			FixedString OriginalTemplate;
			FixedString Region;
		};


		struct Character : public EoCServerObject
		{
			Status * GetStatusByHandle(ObjectHandle handle, bool returnPending) const;

			glm::mat3 WorldRot;
			float Scale;
			ObjectSet<void *> PeerIDClassNames;
			void * CurrentTemplate;
			void * OriginalTemplate;
			void * TemplateUsedForSkills;
			EsvCharacterFlags2 Flags2;
			EsvCharacterFlags3 Flags3;
			uint8_t FlagsEx;
			uint8_t Team;
			uint8_t Color;
			uint8_t _Pad4[3];
			uint64_t U4[3];
			float U41[3];
			float U42[3];
			void * AI;
			FixedString AnimationOverride;
			uint32_t WalkSpeedOverride;
			uint32_t RunSpeedOverride;
			ObjectSet<FixedString> VoiceSet;
			uint8_t NeedsUpdateCount;
			uint8_t ScriptForceUpdateCount;
			uint8_t ForceSynchCount;
			bool U5;
			uint8_t _Pad5[4];
			CDivinityStats_Character * Stats;
			ObjectHandle InventoryHandle;
			void * MovementMachine;
			void * ActionMachine;
			void * SteeringMachine;
			void * BehaviourMachine;
			uint64_t U7[2];
			void * OsirisController;
			void * RequestController;
			void * StatusController;
#if !defined(OSI_EOCAPP)
			uint64_t U8[2];
#endif
			void * ScriptController;
			void * TaskController;
			StatusMachine * StatusMachine;
			SkillManager * SkillManager;
			void * VariableManager;
			void * RaceVariableManager_M;
			FixedStringMapBase<void *> Attitudes; // Element type unknown
			uint8_t _Pad61[4];
			FixedString SkillBeingPrepared;
			uint64_t U9[1];
			uint32_t Dialog;
			bool IsDialogAiControlled;
			uint8_t U10[3];
			float LifeTime;
			float TurnTimer;
			float TriggerTrapsTimer;
			uint32_t U11;
			uint64_t U12;
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
			uint32_t PartialAP;
			uint8_t AnimType;
			uint8_t DelayDeathCount;
			uint16_t _Pad6;
			void * StatusManagerDirty_M;
			ObjectSet<ObjectHandle> ObjectHandleSet3;
			ObjectSet<FixedString> RegisteredTriggers;
			PlayerData * PlayerData;
			eoc::PlayerUpgrade PlayerUpgrade;
			uint8_t _Pad62[7];
			uint64_t U13;
			float U131;
			uint8_t _Pad63[4];
			void * CustomDisplayName;
			void * StoryDisplayName;
			void * OriginalTransformDisplayName;
			uint64_t U14[20];
			int32_t MaxVitalityPatchCheck;
			int32_t MaxArmorPatchCheck;
			int32_t MaxMagicArmorPatchCheck;
			float _Pad7;
			FixedString AnimationSetOverride;
			ObjectHandle PartyHandle;
			ObjectSet<FixedString> CreatedTemplateItems;
			ObjectSet<FixedString> Treasures;
			FixedString CustomTradeTreasure;
			ObjectSet<FixedString> Target;
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
			ObjectHandle U17;
			FixedString Archetype;
			FixedString EquipmentColor;
			FixedString ProjectileTemplate;
			uint32_t TimeElapsed;
			uint8_t _Pad81[4];
			ObjectSet<FixedString> PreferredAiTarget;
			uint64_t U18;
			FixedStringRefMap<void *> U19;
			ObjectSet<FixedString> U20;
			void * VisualSetIndices;
			bool ReadyCheckBlocked;
			bool CorpseLootable;
			uint8_t NumConsumables;
			uint8_t _Pad9[5];
			FixedString PreviousLevel;
		};

		typedef void(*Character__Hit)(esv::Character * self, CDivinityStats_Character * attackerStats, CDivinityStats_Item * itemStats, DamagePairList * damageList,
			HitType hitType, bool rollForDamage, HitDamageInfo * damageInfo, int forceReduceDurability, void * skillProperties, HighGroundBonus highGroundFlag, bool procWindWalker, CriticalRoll criticalRoll);
	}
#pragma pack(pop)
}
