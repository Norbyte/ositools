#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

BEGIN_SE()

struct PhysicsObject;

// Unmapped
struct PhysicsHit {};

struct PhysicsShape : public ProtectedGameObject<PhysicsShape>
{
	void* VMT;
	FixedString Name;
	glm::vec3 Translate;
	glm::mat3 Rotate;
	float Scale;
};

struct PhysicsScene : public ProtectedGameObject<PhysicsScene>
{
	virtual ~PhysicsScene() = 0;
	virtual bool Raycast(glm::vec3 const&, glm::vec3 const&, PhysicsHit&, uint32_t physicsType, unsigned int) = 0;
	virtual void Invalidate(ObjectSet<glm::i32vec2>&) = 0;
	virtual void InvalidateAll() = 0;
	virtual void CreateRenderableObjects(ObjectSet<RenderableObject*>&) = 0;
	virtual void field_28() = 0;

	ObjectSet<PhysicsObject*> Objects;
};

struct PhysicsObject : public ProtectedGameObject<PhysicsObject>
{
	virtual ~PhysicsObject() = 0;
	virtual bool Raycast(glm::vec3 const&, glm::vec3 const&, PhysicsHit&, uint32_t physicsType, unsigned int) = 0;
	virtual void Invalidate(ObjectSet<glm::i32vec2>&) = 0;
	virtual void InvalidateAll() = 0;
	virtual void CreateRenderableObjects(ObjectSet<RenderableObject*>&) = 0;
	virtual void field_28() = 0;

	void* VMT;
	FixedString TemplateID;
	PhysicsScene* PhysicsScene;
	IGameObject* GameObject;
	void* PhysicsResource;
	ObjectSet<PhysicsShape*> Shapes;
	glm::vec3 Translate;
	glm::mat3 Rotate;
	float Scale;
	uint32_t PhysicsGroups;
	uint32_t CollidesWith;
	float ManagedScale;
};

struct PhysicsRagdoll : public ProtectedGameObject<PhysicsRagdoll>
{
	void* RagdollSubsystem;
	void* PhysicsTemplate;
	float field_10;
	int ActiveCount;
	int field_18;
	int field_1C;
	ObjectSet<glm::mat4> matrix4s;
	ObjectSet<int8_t> chars;
	__int64 field_60;
	bool HasHitEvent;
	float IgnoreHitsForTime;
	Skeleton* Skeleton;
	void* Animation;
	glm::mat4 Transform;
};

END_NS()
