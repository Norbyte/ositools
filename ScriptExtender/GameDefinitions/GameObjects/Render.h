#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

struct MoveableObject : public ProtectedGameObject<MoveableObject>
{
	void* VMT;
	Transform LocalTransform;
	Transform WorldTransform;
	Scene *Scene;
	void* CullFunction;
	void* CameraSortCallback;
	uint8_t ObjectFlags;
	void* Node;
	uint8_t DirtyFlags;
	Bound WorldBound;
	Bound LocalBound;
	Bound BaseBound;
};

struct Visual : public MoveableObject
{
	struct Attachment
	{
		Visual* Visual;
		int16_t DummyAttachmentBoneIndex;
		int16_t BoneIndex;
		int field_C;
		FixedString AttachmentBoneName;
		uint32_t Flags;
	};

	struct ObjectDesc
	{
		RenderableObject* Renderable;
		uint8_t field_8;
	};

	ComponentHandle Handle;
	void* Actor;
	void* Skeleton;
	ObjectSet<ObjectDesc> OS_ObjectDesc;
	ObjectSet<float> LODDistances;
	ObjectSet<Attachment> OS_Attachment;
	Visual* Parent;
	void* VisualResource;
	IGameObject* GameObject;
	uint8_t VisualFlags;
	uint8_t TextKeyPrepareFlags;
	uint16_t CullFlags;
	bool PlayingAttachedEffects;
	bool ShowMesh;
	bool HasCloth;
	bool ChildVisualHasCloth;
	float FadeOpacity;
	bool ReceiveColorFromParent;
};

struct PhysicsShape
{
	void* VMT;
	FixedString Name;
	glm::vec3 Translate;
	glm::mat3 Rotate;
	float Scale;
};

struct PropertyList
{
	int RenderPasses;
	uint8_t OpaqueChannel;
	uint8_t AlphaChannel;
	uint8_t field_6;
	uint8_t field_7;
	int TransformType;
};

struct RenderableObject : public MoveableObject
{
	rf::Model* Model;
	void* ClothModel;
	bool IsSimulatedCloth_M;
	PropertyList PropertyList;
	Visual* ParentVisual;
	ObjectSet<void*> AppliedMaterials;
	Material* ActiveMaterial;
	ObjectSet<void*> AppliedOverlayMaterials;
	char HasPhysicsProxy;
	PhysicsShape* ClothPhysicsShape;
	uint8_t LOD;
	float MeshRandomData[4];
	void* RenderCallback;
};

struct WorkerThreadJob
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
	float SpotLightAngles[2];
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
	int LightType;
	int LightVolumeSamples;
	void* LightVolumeTexture;
	int LightVolumeMapping;
	uint8_t field_24C;
	LightTemplate* Template;
	Scene* AssociatedScene;
	bool IsUpdateJobRunning;
};


struct VisualComponent : public BaseComponent
{
	Visual* Visual;
	VisualComponentFlags Flags;
};

END_SE()
