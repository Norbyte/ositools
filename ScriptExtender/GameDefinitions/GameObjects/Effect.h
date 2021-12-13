#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Render.h>

BEGIN_SE()

struct Effect : public Visual
{
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

END_SE()

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
	ObjectSet<ComponentHandle> AttachedVisuals;
	ObjectSet<MultiEffectVisual*> Visuals;
	ObjectSet<WeaponAttachmentInfo> WeaponAttachments;
	bool ListeningOnTextKeys;
};

END_NS()
