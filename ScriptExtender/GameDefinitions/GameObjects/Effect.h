#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Visuals.h>

BEGIN_NS(ecl)

struct Effect : public Visual
{
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Effect;

	void* VMT;
	__int64 field_258;
	void* EffectResource; // EffectResource*
	uint8_t EffectFlags;
	ComponentHandle SoundObjectHandle;
	uint8_t SoundObjectRefCount;
	uint32_t _Pad;
	Transform CachedWorldTransform;
	FixedString EffectName;
	uint8_t field_300;
	Effect* ParentEffect;
	float CachedParentFadeOpacity;
	uint8_t FreezeFadeOpacity;
	void* OldJob;
	uint16_t RefCount;
};

END_NS()

BEGIN_NS(lsfx)

struct Component;

struct Parameter
{
	STDString Name;
	float Value;
};

struct Property
{
	void* VMT;
	FixedString EffectPropertyName;
	FixedString AttributeName;
	PrimitiveSmallSet<Parameter*> Parameters;
	uint32_t field_30;
	uint32_t field_34;
};

struct Module
{
	STDString Name;
	ObjectSet<FixedString> EffectPropertyName;
	ObjectSet<Property*> BoundProperties;
	Component* Owner;
};

struct Component
{
	void* VMT;
	ObjectSet<Property*> Properties;
	Guid UUID;
	STDString Name;
	ObjectSet<Module*> Modules;
};


END_NS()

BEGIN_NS(aspk)

struct Effect;
struct Property;
struct Component;
struct Input;

struct EffectHeader
{
	float Duration;
	Array<void*> Phases;
};

struct Component : public lsfx::Component
{
	Effect* Effect;
	Map<FixedString, Property*> Properties;
	int TrackGroupIndex;
	float StartTime;
	float EndTime;
	bool IsActive;
	bool StayAlive;
};

struct Effect : public dse::ecl::Effect
{
	bool field_328;
	int field_32C;
	ObjectSet<Component*> Components;
	bool Paused;
	bool RequestAdvancePhase;
	bool NeedsRewind;
	int NumPhaseLoops;
	float PhaseTime;
	float RemainingTime;
	float RemainingPhaseTime;
	float CurrentTime;
	uint8_t CurrentPhase;
	EffectHeader EffectHeader;
	ObjectSet<Input*> Inputs;
};

END_NS()


BEGIN_NS(ecl)

struct MultiEffectHandler : public Noncopyable<MultiEffectHandler>
{
	using InitProc = void (MultiEffectHandler* self, char const* effect, glm::vec3 const& position, IEoCClientObject* target, void* skillState, IEoCClientObject* listenForTextKeys, FixedString const& weaponBones);
	using UpdateProc = void (MultiEffectHandler* self);
	using DeleteProc = void (MultiEffectHandler* self, bool immediate);

	struct WeaponAttachmentInfo
	{
		FixedString EffectName;
		FixedString BoneNames;
		FixedString VisualId;
	}; 
	
	struct MultiEffectVisual : public ProtectedGameObject<MultiEffectVisual>
	{
		void* VMT{ nullptr };
		EntityHandle VisualEntityHandle;
		MultiEffectHandler* MultiEffectHandler;
		ObjectSet<FixedString> OS_FS;
	};

	struct EffectInfo
	{
		FixedString Effect;
		ObjectSet<FixedString> BoneNames;
		MultiEffectHandlerFlags Flags;
	};

	void* VMT{ nullptr };
	glm::vec3 Position;
	ComponentHandle TargetObjectHandle;
	ComponentHandle ListenForTextKeysHandle;
	FixedString WeaponBones;
	RefMap<FixedString, ObjectSet<EffectInfo*>> TextKeyEffects;
	void* SkillState{ nullptr };
	ObjectSet<ComponentHandle> Effects;
	ObjectSet<ComponentHandle> AttachedVisualComponents;
	ObjectSet<MultiEffectVisual*> Visuals;
	ObjectSet<WeaponAttachmentInfo> WeaponAttachments;
	bool ListeningOnTextKeys;
};

struct BeamEffectHandler : public Noncopyable<BeamEffectHandler>
{
	void* VMT;
	glm::vec3 SourcePosition;
	ComponentHandle SourceHandle;
	ComponentHandle TargetHandle;
	ComponentHandle SourceHandle2;
	RefMap<FixedString, ObjectSet<MultiEffectHandler::EffectInfo*>> TextKeyEffectDescs;
	RefMap<FixedString, ObjectSet<ComponentHandle>> TextKeyEffects;
	ObjectSet<ComponentHandle> Effects;
	bool ListeningOnTextKeys;
};

END_NS()

BEGIN_NS(esv)

struct Effect : public BaseComponent
{
	static constexpr auto ComponentPoolIndex = EntityComponentIndex::Effect;
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;

	FixedString GUID; // Unused
	NetId NetID;
	ObjectSet<int16_t> PeerIDs;
	bool Loop;
	bool ForgetEffect;
	bool IsForgotten;
	bool IsDeleted;
	float Duration;
	FixedString EffectName;
	ComponentHandle Target;
	ComponentHandle BeamTarget;
	glm::vec3 Position;
	glm::mat3 Rotation;
	float Scale;
	FixedString Bone;
	FixedString BeamTargetBone;
	glm::vec3 BeamTargetPos;
	bool DetachBeam;

	void Delete();
};


struct EffectFactory : public NetworkComponentFactory<Effect>
{
	void* VMT2;
	ObjectSet<EntityHandle> EffectEntities;
	EntityWorld* World;
};


struct EffectManager
{
	using CreateEffectProc = Effect* (EffectManager* self, FixedString const& effectName, __int64 sourceHandle, FixedString const& castBone);
	using DestroyEffectProc = void (EffectManager* self, uint64_t effectHandle);

	void* VMT;
	BaseComponentProcessingSystem<EntityWorld> ComponentProcessingSystem;
	EffectFactory* EffectFactory;
	ObjectSet<Effect*> Effects;
	ObjectSet<Effect*> DeletedEffects;
	ObjectSet<Effect*> ForgottenEffects;
};

END_NS()
