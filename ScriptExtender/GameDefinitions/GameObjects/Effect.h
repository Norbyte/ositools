#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Visuals.h>

BEGIN_SE()

struct EffectResource : public DeferredLoadableResource
{
	FixedString EffectName;
	ObjectSet<aspk::Component*> Components;
	ObjectSet<FixedString> Dependencies;
	void* EffectConstructor;
	void* field_108;
};

struct EffectFactory : public ProtectedGameObject<EffectFactory>
{
	virtual ~EffectFactory() = 0;
	// ...

#if !defined(OSI_EOCAPP)
	bool Initialized;
#endif
};

struct Effect : public Visual
{
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Effect;

	virtual void PreUpdate(GameTime const&) = 0;
	virtual void Update(GameTime const&) = 0;
	virtual void PostUpdate(GameTime const&) = 0;
	virtual void SetForwardShading2() = 0;
	virtual EntityHandle* GetSoundObject(EntityHandle&) = 0;
	virtual void CreateSoundObject(float) = 0;
	virtual void DestroySoundObject() = 0;
	virtual void ActivateSound(EntityHandle&) = 0;
	virtual void DeactivateSound(EntityHandle&) = 0;
	virtual void ReleaseResources(void*) = 0;
	virtual void InitResources() = 0;
	virtual void GetRenderableObjects(void*) = 0;
	virtual void CollectVisuals(ObjectSet<Visual*>&) = 0;
	virtual bool IsStopped() = 0;
	virtual bool IsPlaying() = 0;
	virtual void ReloadFx() = 0;
	virtual void UpdateEffectParametersToEffectResource() = 0;
	virtual void UpdateEffectParametersFromEffectResource() = 0;
	virtual void ReloadParameters() = 0;
	virtual void Construct(void* EffectConstructor) = 0;
	virtual void SetPlayToEnd(bool) = 0;
	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Stop() = 0;
	virtual void StopImmediate() = 0;
	virtual void SetForgetEffect(bool) = 0;
	virtual void SetInput(uint32_t entry, ComponentHandle const&) = 0;
	virtual void SetInput(uint32_t entry, FixedString const&) = 0;
	virtual void SetInput(uint32_t entry, char const*) = 0;
	virtual void SetInput(uint32_t entry, glm::vec4 const&) = 0;
	virtual void SetInput(uint32_t entry, glm::vec3 const&) = 0;
	virtual void SetInput(uint32_t entry, float) = 0;
	virtual void SetInput(uint32_t entry, int) = 0;
	virtual void GetInput(uint32_t entry, ComponentHandle&) = 0;
	virtual void ResetInput(uint32_t entry) = 0;

	void* VMT;
	__int64 field_258;
	EffectResource* Resource;
	EffectFlags Flags;
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

	void LuaPlay();
	void LuaPause();
	void LuaStop();
	void LuaStopImmediate();
	void LuaReloadParameters();
	void LuaUpdateEffectParametersFromEffectResource();
};

END_SE()

BEGIN_NS(lsfx)

struct Component;

struct Parameter
{
	STDString Name;
	float Value;
};

struct Property : public ProtectedGameObject<Property>
{
	virtual ~Property() = 0;
	virtual void Clone(Property* other) = 0;
	virtual bool Visit(ObjectVisitor* visitor) = 0;
	virtual aspk::PropertyType GetType() = 0;

	FixedString FullName;
	FixedString AttributeName;
	Array<Parameter*> Parameters;
};

struct Module : public ProtectedGameObject<Module>
{
	STDString Name;
	ObjectSet<FixedString> PropertyNames;
	ObjectSet<Property*> BoundProperties;
	Component* Owner;
};

struct Component : public ProtectedGameObject<Component>
{
	virtual ~Component() = 0;
	virtual void Clone(Component&) = 0;
	virtual bool Visit(ObjectVisitor&) = 0;
	virtual float GetParameterValue(char const*, float) = 0;
	virtual void CloneProperties(Component&) = 0;
	virtual bool VisitProperties(ObjectVisitor&) = 0;
	virtual bool PURECALL(ObjectVisitor&) = 0;
	virtual bool OnPropertyChanged(char const*) = 0;
	virtual void Update(float) = 0;
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void Reset() = 0;
	virtual void Reload() = 0;
	virtual void OnPause() = 0;

#if defined(OSI_EOCAPP)
	uint32_t Unknown1;
	uint32_t Unknown2;
#endif
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


struct Property : public lsfx::Property
{
	Input* InputNode;
	Component* Component;
	FixedString Input;
};

template <class T>
struct TypedProperty : public Property
{
	T Value;
};

struct ColorARGBKeyFrame
{
	float Time;
#if defined(OSI_EOCAPP)
	uint8_t _Pad[12];
#endif
	glm::vec4 Color;
};

struct ColorARGBKeyFrameData
{
	Array<ColorARGBKeyFrame> Keyframes;
};

struct FloatKeyFrameData : public ProtectedGameObject<FloatKeyFrameData>
{
	virtual ~FloatKeyFrameData() = 0;
	virtual uint32_t GetType() = 0;
	virtual float GetValueAt(float time) = 0;
	virtual void SetValue(void*) = 0;
	virtual bool Visit(ObjectVisitor&) = 0;
};

struct LinearFloatKeyFrame : public ProtectedGameObject<LinearFloatKeyFrame>
{
	virtual ~LinearFloatKeyFrame() = 0;
	virtual void Set(LinearFloatKeyFrame const&) = 0;
	virtual bool Visit(ObjectVisitor&) = 0;

	float Time;
	uint32_t _Pad;
	float Value;
};

struct LinearFloatKeyFrameData : public FloatKeyFrameData
{
	Array<LinearFloatKeyFrame> Keyframes;
};

struct CubicFloatKeyFrame : public ProtectedGameObject<CubicFloatKeyFrame>
{
	virtual ~CubicFloatKeyFrame() = 0;
	virtual void Set(CubicFloatKeyFrame const&) = 0;
	virtual bool Visit(ObjectVisitor&) = 0;

	float Time;
	uint32_t _Pad;
	glm::vec4 Value;
};

struct CubicFloatKeyFrameData : public FloatKeyFrameData
{
	Array<CubicFloatKeyFrame> Keyframes;
};

struct Input : public ProtectedGameObject<Input>
{
	virtual ~Input() = 0;
	virtual void Clear() = 0;
	virtual uint32_t GetType() = 0;

	bool HasValue;
	FixedString Name;
};

template <class T>
struct TypedInput : public Input
{
	T Value;
};


struct Phase
{
	float Duration;
	int field_4;
	int PlayCount;
};


struct EffectHeader
{
	float Duration;
	Array<Phase> Phases;
};

struct Component : public lsfx::Component
{
	Effect* Effect;
	Map<FixedString, Property*> NamedProperties;
	int TrackGroupIndex;
	float StartTime;
	float EndTime;
	bool IsActive;
	bool StayAlive;
};

struct Effect : public dse::Effect
{
	virtual void ReleaseFx() = 0;

	bool PlayToEnd;
	float Speed;
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

BEGIN_NS(fx)

struct Effect : public aspk::Effect
{
	bool HasBound;
	bool IsInitialized;
};


struct EffectStatesManager : public ProtectedGameObject<EffectStatesManager>
{
	ObjectSet<void*> States;
	CRITICAL_SECTION CriticalSection;
};

struct FxEffectFactory : public EffectFactory
{
	struct CacheInfo
	{
		uint16_t StartIndex;
		uint8_t Size;
		uint8_t Capacity;
	};

	void* VMT2;
	CRITICAL_SECTION CacheCriticalSection;
	Map<STDString, FixedString> UnknownPaths;
#if defined(OSI_EOCAPP)
	uint32_t UnknownLock;
#else
	CRITICAL_SECTION* ResourceLock;
#endif
	Map<FixedString, FixedString> EffectNameToResourceName;
	ObjectSet<Path> SourcePaths;
#if !defined(OSI_EOCAPP)
	ObjectSet<Path> OS_Path2;
	EffectStatesManager* EffectStates;
#endif
	RefMap<FixedString, CacheInfo> EffectCaches;
	ObjectSet<Effect*> EffectPool;
	ComponentFactory<Effect, ObjectFactoryRWLocker> EffectFactory;
	CRITICAL_SECTION EffectCriticalSection;
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
