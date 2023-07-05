#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_NS(esv)

struct IScriptCheckObject;
struct Plan;
struct PlanReaction;

struct VariableHandleType
{
	ComponentHandle ObjectHandle;
	int RemapIndex;
};

struct Script : public ProtectedGameObject<Script>
{
	void* VMT;
	ObjectSet<FixedString> OS_FS;
};


struct ScriptParamBuffer : public ProtectedGameObject<ScriptParamBuffer>
{
	void* Buffer;
	uint16_t Capacity;
	uint16_t Size;
	uint16_t WritePos;
	uint16_t ReadPos;
	uint16_t References;
};


struct ScriptParam : public ProtectedGameObject<ScriptParam>
{
	void* VMT;
	Path Path1;
	Path Path2;
	ScriptParamBuffer* ScriptParamBuffer;
	ComponentHandle ObjectHandle;
	FixedString FixedString;
	uint16_t References;
	uint16_t Flags;
	bool IsList;
	uint8_t TypeId;
};


struct ScriptTemplate : public ProtectedGameObject<ScriptTemplate>
{
	void* VMT;
	Path Path1;
	ObjectSet<Path> Paths;
};


struct PlanTemplate : public ScriptTemplate
{
	PrimitiveSmallSet<ScriptParam*, uint8_t> Params;
	PrimitiveSmallSet<PlanReaction*, uint16_t> Reactions;
	PrimitiveSmallSet<Plan*, uint8_t>* Plans;
	PrimitiveSmallSet<PlanReaction*, uint16_t> Reactions2;
	PrimitiveSmallSet<IScriptCheckObject*, uint16_t> Checks;
};


struct PlanScript : public Script
{
	Map<FixedString, ScriptParam*> Params;
	PrimitiveSmallSet<PlanReaction*, uint16_t> Reactions;
	PrimitiveSmallSet<Plan*, uint8_t>* Plans;
	PrimitiveSmallSet<PlanReaction*, uint16_t> Reactions2;
	PrimitiveSmallSet<PlanTemplate*, uint16_t> Templates;
	PrimitiveSmallSet<VariableHandleType, uint16_t> VariableHandles;
};


struct PlanCondition : public ProtectedGameObject<PlanCondition>
{
	void* VMT;
	int ConditionId;
	uint8_t field_C;
};


struct PlanEvent : public ProtectedGameObject<PlanEvent>
{
	void* VMT;
	uint16_t EventId;
	STDString Name;
	std::array<ScriptParam*, 9> Params;
};


struct PlanCheck : public ProtectedGameObject<PlanCheck>
{
	void* VMT;
	PrimitiveSmallSet<PlanCondition*, uint8_t> Conditions;
	STDString Check;
	__int64 field_40;
	int8_t Active_M;
};


struct PlanReaction : public PlanCheck
{
	PrimitiveSmallSet<Plan*, uint8_t> Plans;
	FixedString Name;
	Plan* Plan;
	Path Path;
	int Priority;
	uint8_t CharacterCombatStateFlags;
	uint8_t field_A5;
};


struct PlanAction : public ProtectedGameObject<PlanAction>
{
	void* VMT;
	int32_t ActionId;
};


struct PlanActionWithParams : public PlanAction
{
	PrimitiveSmallSet<ScriptParam*, uint8_t> Params;
};


struct PlanInterrupt : public ProtectedGameObject<PlanInterrupt>
{
	void* VMT;
	PrimitiveSmallSet<ScriptParam*, uint8_t> Params;
	STDString Name;
	int16_t InterruptId;
};


struct Plan : public ProtectedGameObject<Plan>
{
	void* VMT;
	PrimitiveSmallSet<PlanAction*, uint16_t> Actions;
	PrimitiveSmallSet<void*, uint16_t> JumpData;
	PrimitiveSmallSet<PlanEvent*, uint8_t> Events;
	PrimitiveSmallSet<PlanInterrupt*, uint8_t> Interrupts;
	PrimitiveSmallSet<ScriptParam*, uint8_t> Params;
	Path Path1;
	Path ScriptSource;
	FixedString Name;
	uint32_t InstructionCounter;
	bool IsExecutingComplexAction;
	bool  field_DD;
};


struct PlanManager : public ProtectedGameObject<PlanManager>
{
	void* VMT;
	void* IScriptDebugDataCollector_VMT;
	ComponentHandle OwnerHandle;
	PlanReaction* Reaction;
	PlanReaction* ScriptFrame;
	PlanScript* Script;
	void* Timer;
	bool Active;
	bool Initialized;
	uint16_t InterruptType;
	ScratchBuffer ScratchBuf;
	Array<FixedString> Arr_FS;
	Map<FixedString, float> ReactionDelays;
	PrimitiveSmallSet<Plan*, uint8_t> Plans;
	__int64 field_B0;
};


struct PlanEventManager : public ProtectedGameObject<PlanEventManager>
{
	PlanManager* GamePlanManager;
	ObjectSet<PlanManager*> Events;
};


END_NS()
