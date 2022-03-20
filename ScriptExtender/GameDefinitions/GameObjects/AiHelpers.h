#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_NS(esv)

struct AiHelpersModifiers : ProtectedGameObject<AiHelpersModifiers>
{
	Array<float> Modifiers;
};

struct AiModifiers
{
	void* VMT;
	AiHelpersModifiers* BaseModifiers;
	RefMap<FixedString, AiHelpersModifiers*> Archetypes;
	ObjectSet<FixedString> ArchetypeNames;
};


struct AiScoreImpl
{
	struct DamageAmount
	{
		float DamageAmount;
		float HealAmount;
		float DoTAmount;
		float HoTAmount;
		float ControlAmount;
		float BoostAmount;
		float ArmorDamage;
		float MagicArmorDamage;
		float PositionScore;
	};

	AiScoreReasonFlags Reason;
	uint16_t Flags1;
	int field_4;
	Map<ComponentHandle, DamageAmount> DamageAmounts;
	float FutureScore;
	float FailScore;
	uint32_t SoftReasonFlags2;
#if !defined(OSI_EOCAPP)
	int field_24;
	ScratchBuffer ScoreLog;
#endif
	int RefCount;
};


struct AiScore
{
	AiScoreImpl* Score;
};


struct AiAction : ProtectedGameObject<AiAction>
{
	AiActionType ActionType;
	FixedString SkillId;
	ComponentHandle SkillItemHandle;
	SkillAIParams* AIParams;
	FixedString MovementSkillId;
	Item* MovementSkillItem;
	bool IsFinished;
	bool SavingActionPoints;
	bool IgnoreMovementScore;
	ComponentHandle TargetHandle;
	glm::vec3 TargetPosition2;
	ComponentHandle Target2Handle;
	glm::vec3 LookAtPosition;
	glm::vec3 MovementSkillTargetPosition;
	glm::vec3 EndPosition;
	glm::vec3 EndPosition2;
	float CloseEnough;
	float CloseEnough2;
	int MovementType;
	int field_94;
	ComponentHandle AiHandle;
	float FinalScore;
	esv::AiScore Score1;
	esv::AiScore ScoreWithoutMovement;
	ComponentHandle TauntedSourceHandle;
	float ActionFinalScore;
	float PositionFinalScore;
	float MovementFinalScore;
	int APCost;
	int APCost2;
	int MagicCost;
	bool field_D8;
	bool IsPositionCalculated;
	bool field_DA;
	bool UseMovementSkill;
	float Distance;
	float FreeActionMultiplier;
	float CostModifier;
};

struct AiSkillData
{
	FixedString SkillId;
	ComponentHandle ItemHandle;
	SkillAIParams* AIParams;
};

struct AiItemData
{
	ComponentHandle ItemHandle;
	SkillAIParams* AIParams;
};


struct AiRequest : ProtectedGameObject<AiRequest>
{
	bool IsCalculating;
	ObjectSet<AiAction> AiActions;
	uint32_t AiActionToExecute;
	uint64_t ActionCount;
	AiActionStep LastStep;
	bool CalculationFinished;
	bool CanMove;
	bool UseBehaviorVariables;
	bool HasAiOnPositionSkillData;
	uint32_t CurrentAiSkillIndex;
	uint32_t CurrentAiItemIndex;
	uint32_t CurrentAiActionIndex;
	uint32_t CurrentAiSkill2Index;
	float HighestActionScore;
	void* MovementState;
	ObjectSet<AiSkillData> Skills;
	ObjectSet<AiSkillData> OnPositionSkills;
	ObjectSet<AiItemData> Items;
	ObjectSet<ComponentHandle> UnknownHandles;
};

struct AiCombos
{
	struct Combo
	{
		SurfaceType SurfaceType1;
		SurfaceType SurfaceType2;
		SurfaceTransformActionType TransformType;
		uint8_t field_C;
	};

	ObjectSet<Combo> Combos;
	RefMap<SurfaceType, ObjectSet<uint64_t>> SurfaceComboIndices;
	RefMap<uint8_t, RefMap<SurfaceType, void*>> CombatSurfaceCombos;
	ObjectSet<stats::DamageType> DamageTypes;
	ObjectSet<FixedString> SkillPrototypes;
};


struct AiKnownStateSet
{
	struct StatusInteraction
	{
		ComponentHandle TargetHandle;
		FixedString StatusId;
		bool HasLifeTime;
	};
	
	struct SurfaceOnObjectAction
	{
		ComponentHandle TargetHandle;
		SurfaceType SurfaceTypeId;
		bool HasLifeTime;
	};
	
	struct SurfaceInteraction
	{
		ComponentHandle SurfaceHandle;
		SurfaceTransformActionType SurfaceTransform;
		std::array<SurfaceType, 2> SurfaceTypeByLayer;
		int field_14;
	};

	ObjectSet<StatusInteraction> StatusInteractions;
	ObjectSet<SurfaceOnObjectAction> SurfaceOnObjectActions;
	ObjectSet<SurfaceInteraction> SurfaceInteractions;
	ObjectSet<void*> ItemActions;
	ObjectSet<void*> ItemActions2;
};


struct AiKnownStateImpl
{
	esv::AiKnownStateSet States;
	esv::AiKnownStateSet States2;
	float field_140[3];
	int RefCount;
};


struct AiKnownState
{
	AiKnownStateImpl* State;
};


struct AiTranslateOverride
{
	glm::vec3 OriginalTranslate;
	glm::vec3 NewTranslate;
};


struct AiHelpers : ProtectedGameObject<AiHelpers>
{
	using PeekActionProc = AiAction* (AiHelpers* self, Character* character, AiActionType actionType, bool isFinished);
	using SortActionsProc = void (Set<AiAction>* actions);

	void* VMT;
	char field_8;
	char field_9;
	Map<ComponentHandle, AiRequest*> CharacterAiRequests;
	ObjectSet<ComponentHandle> PendingCharacters;
	int AiPrepareCount;
	Character* PreparingCharacter;
	eoc::AiGrid* PreparingAiGrid;
	Level* PreparingLevel;
	AiHelpersModifiers* Modifiers;
	ObjectSet<uint8_t> AiFlagsStack;
	uint8_t Flags;
	AiCombos* Combos;
	Item* CurrentItem;
	ObjectSet<AiKnownStateSet*> KnownStateStack;
	AiKnownStateSet* KnownState;
	ObjectSet<FixedString> OS_FixedString;
	ObjectSet<Surface*> Surfaces;
	ObjectSet<Character*> EnemyCharacters;
	ObjectSet<Character*> AllyCharacters2;
	ObjectSet<Character*> NeutralCharacters;
	ObjectSet<Character*> SameCombatCharacters;
	ObjectSet<Character*> SameCombatEnemyCharacters;
	ObjectSet<Character*> AllyCharacters;
	ObjectSet<Character*> SameCombatNeutralCharacters;
	ObjectSet<Item*> Items;
	ObjectSet<ComponentHandle> CurrentAiRequestObjects;
	int LowestCharacterHP;
	int HighestCharacterHP;
	int field_238;
	glm::vec3 OverrideTranslate;
	RefMap<ComponentHandle, void*> Map_ObjHandle_Unkn;
	RefMap<ComponentHandle, AiTranslateOverride> AiTranslateOverrides;
	RefMap<ComponentHandle, void*> AiScoreCacheDamageCharacter;
	RefMap<ComponentHandle, void*> AiScoreCacheDamageItem;
	RefMap<ComponentHandle, void*> Map_ObjHandle_Unkn5;
	RefMap<ComponentHandle, void*> AiScoreCacheDestroyItem;
	RefMap<ComponentHandle, void*> AiScoreCacheDamageItemActions;
	RefMap<ComponentHandle, void*> AiScoreCacheProperties;
	RefMap<ComponentHandle, void*> AiScoreCacheStatus;
	RefMap<ComponentHandle, void*> AiScoreCacheSurfaceInteractions;
	RefMap<ComponentHandle, void*> Map_ObjHandle_Unkn11;
	RefMap<ComponentHandle, void*> AiAverageDamageCache;
	RefMap<ComponentHandle, void*> field_308;
	int field_318;
	int field_31C;
	int AiScoreCacheDamageItemCount;
	int field_324;
	int AiScoreCacheHealCharacterCount;
	int field_32C;
	int AiScoreCacheDestroyItemCount;
	int field_334;
	int AiScoreCacheDamageItemActionsCount;
	int field_33C;
	int AiScoreCachePropertiesCount;
	int field_344;
	int AiScoreCacheStatusCount;
	int field_34C;
	int AiScoreCacheSurfaceInteractionsCount;
	int field_354;
	int field_358;
	int field_35C;
	__int64 field_360;
	Map<uint8_t, void*> Map_u8_Unk;
};

END_NS()
