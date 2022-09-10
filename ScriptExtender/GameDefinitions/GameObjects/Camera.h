#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>

BEGIN_NS(rf)

struct Camera
{
	glm::mat4 ViewMatrix;
	glm::mat4 ViewMatrixInverse;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ProjectionMatrixInverse;
	glm::mat4 ViewProjection;
	std::array<glm::vec4, 6> CullingPlanes;
	std::array<glm::vec3, 6> NVertices;
	std::array<glm::vec3, 6> PVertices;
};

struct CameraController : public ProtectedGameObject<CameraController>
{
	virtual ~CameraController() = 0;
	virtual bool Enter() = 0;
	virtual bool Exit() = 0;
	virtual float GetCurrentFov() = 0;
	virtual float GetCurrentAspectRatio() = 0;
	virtual void Update(GameTime const&) = 0;
	virtual bool LookAt(glm::vec3 const&, glm::vec3 const&, bool) = 0;
	virtual bool EditorLookAt(glm::vec3 const&) = 0;
	virtual void EditorZoomExtents(glm::vec3 const&, float) = 0;
	virtual void SetTarget(ComponentHandle) = 0;
	virtual void SetWorldTransform(glm::mat4 const&) = 0;
	virtual void WarpMouse(bool) = 0;
	virtual void ClearInputState() = 0;
	virtual uint16_t* OnInputEvent(uint16_t* eventRetVal, InputEvent const& inputEvent) = 0;
	virtual void StopMotion() = 0;
	virtual bool SetOrientation(float, float) = 0;
	virtual void SetMoveSpeed(float) = 0;
	virtual void SetLookAtShake(float, float) = 0;
	virtual void SetWobble(float, float) = 0;
	virtual glm::vec3 GetDistance_M() = 0;

#if defined(OSI_EOCAPP)
	uint64_t unkn_8;
#endif
	Transform Transform;
#if defined(OSI_EOCAPP)
	uint32_t _Pad_8c;
#endif
	bool ViewDirty;
	bool ProjectionDirty;
	bool UnknFlag3;
	float FOV;
	float NearPlane;
	float FarPlane;
	float AspectRatio;
	float field_98;
	glm::vec3 Position;
#if defined(OSI_EOCAPP)
	uint64_t unkn_b8;
#endif
	float OrthoLeft;
	float OrthoRight;
	float OrthoTop;
	float OrthoBottom;
	FixedString ID;
#if defined(OSI_EOCAPP)
	uint64_t unkn_d8;
#endif
	Camera Camera;
	std::array<glm::vec3, 8> FrustumPoints;
};

struct CameraManager
{
	using GetInstanceProc = CameraManager * ();

	Map<FixedString, CameraController*> Controllers;
	CRITICAL_SECTION CriticalSection;
};

END_NS()


BEGIN_SE()

struct DefaultCameraController : public rf::CameraController
{
	float MoveSpeedRightVector;
	float MoveSpeedUpVector;
	float SomePositionX;
	float SomePositionY;
	float CameraMoveSpeed;
	int field_364;
	bool IsActive;
};

struct EffectCameraController : public DefaultCameraController
{
	glm::vec3 LookAtShake;
	float Wobble;
	float Wobble2;
#if defined(OSI_EOCAPP)
	uint64_t unkn_3a8;
#endif
};

END_SE()


BEGIN_NS(ecl)

struct EocCamera : public EffectCameraController
{
	void* GameEventManagerVMT;
	glm::vec3 LookAt;
	void* AtmosphereTrigger;
	ComponentHandle SoundVolumeTrigger;
	PlayerId PlayerIndex;
	bool IsInShroud;
	bool NeedsLookAtUpdate;
	bool ResetAtmosphere;
};

struct GameCamera : public EocCamera
{
	void* FileFormatIOVMT;
	ComponentHandle CharacterHandle;
	glm::vec3 TargetLookAt;
	glm::vec3 CurrentLookAt;
	int field_3E0;
	int field_3E4;
	int field_3E8;
	float CurrentCameraDistance;
	int field_3F0;
	float TargetCameraDistance;
	glm::vec3 Position_M;
	glm::vec3 Position2;
	__int64 field_410;
	int field_418;
	float CameraPanX;
	float CameraPanY;
	float InputRotationRate;
	float InputRotationRateMode1;
	float InputCameraDistanceRate;
	uint8_t Flags;
	uint8_t Flags2;
	float Rotation;
	float RotationRate;
	void* ShroudGenTrigger;
	int ShroudGenTriggerIndex;
	void* CameraLockAreaTrigger;
	int CameraLockAreaTriggerIndex;
	void* CameraLockTrigger;
	float CamAvoidanceValue1;
	float CamAvoidanceValue2;
	ObjectSet<ComponentHandle> Targets;
	float TargetCameraDistance2;
	float SomeFadeFloat;
	bool SomeModeFlag;
	uint8_t field_499;
	int CameraMode;
};

END_NS()
