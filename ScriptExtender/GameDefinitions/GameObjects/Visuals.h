#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Resource.h>
#include <GameDefinitions/GameObjects/Physics.h>
#include <GameDefinitions/GameObjects/VisualsShared.h>

BEGIN_SE()

struct Skeleton : public ProtectedGameObject<Skeleton>
{
	virtual ~Skeleton() = 0;
	virtual void Unkn08() = 0;
	virtual void Unkn10() = 0;
	virtual void Unkn18() = 0;
	virtual void Unkn20() = 0;
	virtual void Unkn28() = 0;
	virtual void Unkn30() = 0;
	virtual bool GetBoneIndexFromName(char const* boneName, int* boneIndex) = 0;
	virtual char const* GetBoneNameFromIndex(int boneIndex) = 0;
	virtual int GetBoneParentIndex(int boneIndex) = 0;
	virtual void Unkn50() = 0;

	bool NeedsUpdate;
	int NumBones;
	void* Bones;
	PhysicsRagdoll* LinkedRagdoll;
};

struct AnimatableObject;
struct Visual;

struct Pose : public ProtectedGameObject<Pose>
{
	glm::mat4* CompositeMatrices;
	int NumCompositeMatrices;
};

struct MeshBinding : public ProtectedGameObject<MeshBinding>
{
	virtual ~MeshBinding() = 0;
	virtual void RemapToSkeleton(Skeleton* skeleton) = 0;
	virtual void Unk10() = 0;
	virtual void UpdatePose() = 0;
	virtual void Unk20() = 0;
	virtual void Unk28() = 0;
	virtual void Unk30() = 0;

	void* Unknown1;
	Pose Pose;
	AnimatableObject* Link;
	void* Unknown2;
	glm::mat4 Transform;
	Bound Bound;
};

struct CallbackThreadsafetyHelper
{
	__int64 field_0;
	__int64 field_8;
};

struct Actor
{
	virtual ~Actor() = 0;
	virtual void OnWTCompletion() = 0;
	virtual void OnWTCanceled() = 0;
	virtual void ExecuteWTKernel() = 0;
	virtual void GetHeuristic() = 0;
	virtual void DestroyCallbacks() = 0;
	virtual void AddAnimation() = 0;
	virtual void RemoveAnimation() = 0;
	virtual void PlayAnimation() = 0;
	virtual void SetAnimationSpeed() = 0;
	virtual void StopAnimations() = 0;
	virtual void GetAnimationSpeed() = 0;
	virtual void IsAnimationLooping() = 0;
	virtual void IsAnimationEnabled() = 0;
	virtual void IsAnimationPlaying() = 0;
	virtual void SetTextKeyEventHandler(void* handler) = 0;
	virtual void* GetTextKeyEventHandler() = 0;
	virtual void CalculateLOD() = 0;
	virtual void BuildTransitionData() = 0;
	virtual void CheckCompleteTransitions() = 0;
	virtual void UpdateTime() = 0;
	virtual void UpdateTransitions() = 0;
	virtual void UpdateActor() = 0;
	virtual void UpdateRagdoll() = 0;
	virtual void UpdateIK() = 0;

	void AddMeshBinding(MeshBinding* binding);

	__int64 field_8;
	Skeleton* Skeleton;
	void* AnimationLayers;
	ObjectSet<MeshBinding*> MeshBindings;
	__int64 field_40;
#if defined(OSI_EOCAPP)
	__int64 field_48;
#endif
	PhysicsRagdoll PhysicsRagdoll;
	void* Animation;
	void* TextKeyEventHandler;
	__int64 field_118;
	int field_120;
	char field_124;
	int field_128;
	__int64 field_130;
	int field_138;
	char field_13C;
	int field_140;
	CallbackThreadsafetyHelper field_148;
	CallbackThreadsafetyHelper field_158;
	bool WorkerThreadPending;
	uint8_t TextKeyPrepareFlags;
	Visual* Visual;
	GameTime Time;
	int AnimationUpdateTicket;
};


struct MoveableObject : public ProtectedGameObject<MoveableObject>
{
	virtual uint32_t GetRTTI() = 0;
	virtual ~MoveableObject() = 0;

	virtual void SetWorldTranslate(glm::vec3 const& translate) = 0;
	virtual void SetWorldRotate(glm::mat3 const& rotate) = 0;
	virtual void SetWorldScale(glm::vec3 const& scale) = 0;
	virtual void SetWorldTransform(glm::mat4 const& mat) = 0;
	virtual void SetWorldTransform(glm::vec3 const& translate, glm::mat3 const& rotate, glm::vec3 const& scale) = 0;

	virtual void SetLocalTranslate(glm::vec3 const& translate) = 0;
	virtual void SetLocalRotate(glm::mat3 const& rotate) = 0;
	virtual void SetLocalScale(glm::vec3 const& scale) = 0;
	virtual void SetLocalTransform(glm::mat4 const& mat) = 0;
	virtual void SetLocalTransform(glm::vec3 const& translate, glm::mat3 const& rotate, glm::vec3 const& scale) = 0;

	virtual uint8_t CalculateTextureLOD(glm::vec3 const& translate) = 0;
	virtual void SetTextureLOD(/* ??? */) = 0;
	virtual uint64_t GetCullFlags() = 0;
	virtual void CameraSort(void* cameraController) = 0;
	virtual bool AttachToScene(Scene* scene) = 0;
	virtual bool DetachFromScene() = 0;
	virtual bool IsAttachedToScene() = 0;
	virtual char const* GetDebugName() = 0;
	virtual void GetDebugInfo(ScratchBuffer* debugInfo) = 0;
	virtual void NotifyBoundChange() = 0;
	virtual void UpdateBound() = 0;
	virtual void UpdateBoundChange() = 0;

#if defined(OSI_EOCAPP)
	uint64_t Unknown1;
#endif
	Transform LocalTransform;

#if defined(OSI_EOCAPP)
	float Unknown2[2];
#endif
	Transform WorldTransform;
	Scene *ContainingScene;
	void* CullFunction;
	void* CameraSortCallback;
	uint8_t ObjectFlags;
	void* Node;
	uint8_t DirtyFlags;
	Bound WorldBound;
	Bound LocalBound;
	Bound BaseBound;
};


struct AnimationSet
{
	struct AnimationDescriptor
	{
		FixedString ID;
		FixedString Name;
	};

	RefMap<int, RefMap<FixedString, AnimationDescriptor>> AnimationSubSets;
	FixedString Type;
};


struct VisualResource : public DeferredLoadableResource
{
	struct BonePosRot
	{
		glm::vec3 Position;
		glm::mat3 Rotation;
	};

	struct Attachment
	{
		FixedString UUID;
		FixedString Name;
	};

	struct ObjectDesc
	{
		FixedString ObjectID;
		FixedString MaterialID;
		uint8_t LOD;
	};

	struct ClothParam
	{
		FixedString UUID;
		float LinearStiffness;
		float BendingStiffness;
		int Iterations;
		float PoseMatching;
		float Margin;
		float MassPerMeterSqr;
		bool AtmosphericWindEnabled;
		float Lift;
		float Drag;
		float FrontalWindSpeed;
		float FrontalWindFrequency;
		float FrontalWindVariance;
	};


	FixedString Template;
	ObjectSet<ObjectDesc> Objects;
	Array<float> LODDistances;
	ObjectSet<Attachment> Attachments;
	RefMap<FixedString, FixedString> KnownAnimationSetOverrides;
	ObjectSet<FixedString> AnimationWaterfall;
	AnimationSet* CustomAnimationSet;
	AnimationSet* ResolvedAnimationSet;
	FixedString BlueprintInstanceResourceID;
	ObjectSet<ClothParam> ClothParams;
	RefMap<FixedString, BonePosRot> Bones;
};

struct VisualSet;

struct VisualSetResource : public NonDeferredLoadableResource
{
	VisualSet* VisualSet;
	VisualSetIndices VisualSetIndices;
	FixedString LastUsedPreviewVisual;
};

struct TextKeyEvent
{
	void* VMT;
	FixedString ID;
	float Time;
	float Length;
	char Track;
	void* Properties;
};

struct TimelineEvent
{
	float Time;
	FixedString ID;
};

struct AnimationResource : public DeferredLoadableResource
{
	FixedString TemplateID;
	ObjectSet<TextKeyEvent*> TextKeys;
	int TextKeyRefcounts[2];
	ObjectSet<TimelineEvent> TimelineEvents;
	float Duration;
	float InitialOffset;
	bool IsLooping;
	bool IsImmediate;
	bool IsPoseBank;
	FixedString PreviewVisualResource;
};


struct Visual : public MoveableObject
{
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Visual;

	virtual void Pick(void* ray, void* pickResult) = 0;
	virtual void UpdateCullFlags() = 0;

	using AddAttachmentProc = bool (Visual* self, Visual* attachedVisual, FixedString const& attachmentBoneName, int16_t dummyAttachmentBoneIndex, VisualAttachmentFlags flags);

	struct Attachment
	{
		Visual* Visual;
		int16_t DummyAttachmentBoneIndex;
		int16_t BoneIndex;
		int field_C;
		FixedString AttachmentBoneName;
		VisualAttachmentFlags Flags;
	};

	struct ObjectDesc
	{
		RenderableObject* Renderable;
		uint8_t field_8;
	};

	ComponentHandle Handle;
	Actor* Actor;
	Skeleton* Skeleton;
	ObjectSet<ObjectDesc> SubObjects;
	ObjectSet<float> LODDistances;
	ObjectSet<Attachment> Attachments;
	Visual* Parent;
	VisualResource* VisualResource;
	IGameObject* GameObject;
	VisualFlags VisualFlags;
	uint8_t TextKeyPrepareFlags;
	uint16_t CullFlags;
	bool PlayingAttachedEffects;
	bool ShowMesh;
	bool HasCloth;
	bool ChildVisualHasCloth;
	float FadeOpacity;
	bool ReceiveColorFromParent;

	template <class T>
	void OverrideMaterialParameter(FixedString const& parameter, T const& value)
	{
		for (auto const& object : SubObjects) {
			auto material = object.Renderable->ActiveAppliedMaterial;
			if (material) {
				material->MaterialParameters.OverrideParameter(parameter, value);
			}
		}

		for (auto const& attachment : Attachments) {
			if (attachment.Visual->ReceiveColorFromParent) {
				for (auto const& object : attachment.Visual->SubObjects) {
					auto material = object.Renderable->ActiveAppliedMaterial;
					if (material) {
						material->MaterialParameters.OverrideParameter(parameter, value);
					}
				}
			}
		}
	}

	void OverrideVec3MaterialParameter(FixedString const& parameter, glm::vec3 const& vec, bool isColor);
	void OverrideVec4MaterialParameter(FixedString const& parameter, glm::vec4 const& vec, bool isColor);
	void OverrideTextureMaterialParameter(FixedString const& parameter, FixedString const& textureId);
};

struct PropertyList
{
	int RenderPasses;
	uint8_t OpaqueChannel;
	uint8_t AlphaChannel;
	uint8_t field_6;
	uint8_t field_7;
	int TransformType;
	uint32_t _Pad;
};

struct RenderableObject : public MoveableObject
{
	virtual void Render(rf::RenderObjectData const&, rf::RenderContext const&) = 0;
	virtual void NotifyTextureMapAdded(TextureMap const*) = 0;
	virtual void NotifyTextureMapRemoved(TextureMap const*) = 0;
	virtual void SetupTransform(rf::RendererCommandBuffer*) = 0;
	virtual void SetupTransformOverrideWorld(rf::RendererCommandBuffer*, glm::mat4 const&) = 0;
	virtual void SetupTransformOverrideLocal(rf::RendererCommandBuffer*, glm::mat4 const&) = 0;
	virtual bool GetAnimatableTransform(glm::mat3x4 const*&, uint64_t&) = 0;
	virtual void CalculateRenderPass() = 0;
	virtual rf::FormatDesc* GetVertexFormatDesc() = 0;
	virtual uint32_t GetVertexFormatNumAttributes() = 0;

	rf::Model* Model;
	void* ClothModel;
	bool IsSimulatedCloth;
	uint32_t _Pad;
	dse::PropertyList PropertyList;
	Visual* ParentVisual;
	ObjectSet<AppliedMaterial*> AppliedMaterials;
	AppliedMaterial* ActiveAppliedMaterial;
	ObjectSet<AppliedMaterial*> AppliedOverlayMaterials;
	bool HasPhysicsProxy;
	PhysicsShape* ClothPhysicsShape;
	uint8_t LOD;
	uint32_t _Pad2;
	std::array<float, 4> MeshRandomData;
	void* RenderCallback;
};

struct AnimatableObject : public RenderableObject
{
	void* Unknown;
	MeshBinding* MeshBinding;
	bool IsRigid;
	uint64_t NumOverrideTransforms;
	Transform* OverrideTransforms;
};


struct WorkerThreadJob : public ProtectedGameObject<WorkerThreadJob>
{
	__int64 field_0;
	__int64 field_8;
};


struct Light : public MoveableObject
{
	WorkerThreadJob WorkerThreadJob;
	BaseComponent BaseComponent;
	glm::vec3 TranslateOffset;
	glm::vec3 TranslateOffset2;
	glm::vec3 Color;
	std::array<float, 2> SpotLightAngles;
	float Radius;
	float Intensity;
	float IntensityOffset;
	float FlickerSpeed;
	float FlickerAmount;
	float MovementSpeed;
	float MovementAmount;
	float VolumetricLightIntensity;
	float VolumetricLightCollisionProbability;
	bool IsFlickering;
	bool IsMoving;
	bool CastShadow;
	bool LightVolume;
	bool IsEnabled;
	uint32_t LightType;
	uint32_t LightVolumeSamples;
	ComponentHandle LightVolumeTexture;
	int LightVolumeMapping;
	bool HasLightVolumeTexture;
	LightTemplate* Template;
	Scene* AssociatedScene;
	bool IsUpdateJobRunning;
};


struct LightProbeCapture
{
	FixedString Atmosphere;
	uint64_t DiffuseIBLTextureHandle;
	uint64_t DiffuseAlphaIBLTextureHandle;
	uint64_t SpecularIBLTextureHandle;
	uint64_t SpecularAlphaIBLTextureHandle;
	bool IsSpecular;
	LightProbe* LightProbe;
};


struct LightProbe : public MoveableObject
{
	BaseComponent Component;
	FixedString ID;
	FixedString AtmosphereTriggerID;
	float Intensity;
	float Radius;
	glm::vec3 Size;
	float TransitionDistance;
	bool InfiniteCapture;
	uint32_t LightProbeType;
	uint32_t LightProbeShape;
	uint32_t CaptureType;
	bool Enabled;
	ObjectSet<LightProbeCapture> Captures;
	LightProbeTemplate* Template;
	Scene* Scene;
	uint8_t CaptureFlags;
};


struct DecalBasicObject : public RenderableObject
{
	bool IsDirty;
	glm::vec3 Dimensions;
	glm::vec2 TilingUV;
	glm::vec2 OffsetUV;
	rf::CameraController* CameraController;
	FixedString CameraControllerID;
};


struct NodeObjectDescription
{
	MoveableObject* Object;
	uint32_t CullFlags;
};


struct RenderNode : public ProtectedGameObject<RenderNode>
{
	void* VMT;
	std::array<RenderNode*, 4> ChildNodes;
	ObjectSet<NodeObjectDescription> Objects;
	float OuterSize;
	float InnerSize;
	int Depth;
	bool NonLeaf;
	bool IsDirty;
	Bound StrictWorldBound;
	Bound LooseWorldBound;
};


struct VisualComponent : public BaseComponent
{
	Visual* Visual;
	VisualComponentFlags Flags;
};

struct VisualFactory : public ComponentFactory<Visual, ObjectFactoryRWLocker>
{
	using DestroyVisualProc = bool (VisualFactory* self, uint64_t handle);
};

using EffectsManager__DestroyEffect = bool (void* self, ComponentHandle const& handle);
using EffectsManager__GetInstanceProc = void* ();


struct InstanceGroupPaintParams
{
	float Slow;
	bool RandomRotX;
	bool RandomRotY;
	bool RandomRotZ;
	bool AlignNormal;
	float ScaleMin;
	float ScaleMax;
};

struct InstanceGroup
{
	int Width;
	int Height;
	FixedString ModName;
	FixedString field_10;
	__int64 field_18;
	ObjectSet<InstanceBatch*> Batches;
	ObjectSet<InstanceBatch*> DirtyBatches;
	Array<void*> BurnRequests;
	InstanceManager* InstanceManager;
	Visual* Visual;
	float BurnRadius;
	float BurnTimeMin;
	float BurnTimeMax;
	float BurnFadeOut;
	Guid FileID;
	FixedString VisualID;
	FixedString DestroyVisualID;
	FixedString BurnEffectID;
	uint16_t GridSize;
	uint8_t Visible;
	bool CastShadow;
	bool ShouldUpdateEmptyBatches;
	bool HasChanges;
	bool CanEverSave;
	LevelTemplate* LevelTemplate;
	glm::mat4 LevelTemplateWorldOffset;
	InstanceGroupPaintParams PaintParams;
};

struct InstanceBatch : public RenderableObject
{
	void* FileFormatIOVMT;
	FixedString ModName;
	FixedString ActiveModName;
	bool IsOverride;
	Array<glm::mat4x3> InstanceTransforms;
	ObjectSet<int> DestroyInstanceIndices;
	InstanceGroup* ParentGroup;
	rf::RendererBase* Renderer;
	rf::Line* BoundLine;
	void* InstanceBatchFormatDesc;
	uint32_t VertexFormatNumAttributes;
	ComponentHandle VertexBufferHandle;
	glm::vec3 GridIndexes;
	uint16_t GridSize;
	bool HasDirtyBatches;
	bool HasChanges;
	Bound Bounds;
	uint16_t CullFlags;
};


END_SE()
