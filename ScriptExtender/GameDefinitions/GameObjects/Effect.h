#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Render.h>

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
	Transform CachedWorldTransform;
	FixedString EffectName;
	uint8_t field_300;
	Effect* ParentEffect;
	float CachedParentFadeOpacity;
	uint8_t FreezeFadeOpacity;
	void* OldJob;
	uint16_t RefCount;
};

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

END_NS()

BEGIN_NS(esv)

struct Effect : public BaseComponent
{
	static constexpr auto ComponentPoolIndex = EntityComponentIndex::Effect;
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Effect;

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
	ObjectSet<Effect*> ActiveEffects;
};

END_NS()
