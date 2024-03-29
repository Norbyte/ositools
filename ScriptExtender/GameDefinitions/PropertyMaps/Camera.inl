BEGIN_CLS(rf::Camera)
P(ViewMatrix)
P(ViewMatrixInverse)
P(ProjectionMatrix)
P(ProjectionMatrixInverse)
P(ViewProjection)
P_REF(CullingPlanes)
P_REF(NVertices)
P_REF(PVertices)
END_CLS()


BEGIN_CLS(rf::CameraController)
P_REF(Transform)
P(ViewDirty)
P(ProjectionDirty)
P(FOV)
P(NearPlane)
P(FarPlane)
P(AspectRatio)
P(field_98)
P(Position)
P(OrthoLeft)
P(OrthoRight)
P(OrthoTop)
P(OrthoBottom)
P_RO(ID)
P_REF(Camera)
P_REF(FrustumPoints)
END_CLS()


BEGIN_CLS(rf::CameraManager)
P_REF(Controllers)
END_CLS()


BEGIN_CLS(DefaultCameraController)
INHERIT(rf::CameraController)
P(MoveSpeedRightVector)
P(MoveSpeedUpVector)
P(SomePositionX)
P(SomePositionY)
P(CameraMoveSpeed)
P(field_364)
P(IsActive)
END_CLS()


BEGIN_CLS(EffectCameraController)
INHERIT(DefaultCameraController)
P(LookAtShake)
P(Wobble)
P(Wobble2)
END_CLS()


BEGIN_CLS(ecl::EocCamera)
INHERIT(EffectCameraController)
P(LookAt)
// P_REF(AtmosphereTrigger)
// P_REF(SoundVolumeTrigger)
P_RO(PlayerIndex)
P(IsInShroud)
P(NeedsLookAtUpdate)
P(ResetAtmosphere)
END_CLS()


BEGIN_CLS(ecl::GameCamera)
INHERIT(ecl::EocCamera)
P_RO(CharacterHandle)
P(TargetLookAt)
P(CurrentLookAt)
P(field_3E0)
P(field_3E4)
P(field_3E8)
P(CurrentCameraDistance)
P(field_3F0)
P(TargetCameraDistance)
P(Position_M)
P(Position2)
P_RO(field_410)
P(field_418)
P(CameraPanX)
P(CameraPanY)
P(InputRotationRate)
P(InputRotationRateMode1)
P(InputCameraDistanceRate)
P(Flags)
P(Flags2)
P(Rotation)
P(RotationRate)
// P_REF(ShroudGenTrigger)
// P_REF(ShroudGenTriggerIndex)
// P_REF(CameraLockAreaTrigger)
// P_REF(CameraLockAreaTriggerIndex)
// P_REF(CameraLockTrigger)
P(CamAvoidanceValue1)
P(CamAvoidanceValue2)
P_REF(Targets)
P(TargetCameraDistance2)
P(SomeFadeFloat)
P(SomeModeFlag)
P(field_499)
P(CameraMode)
END_CLS()


BEGIN_CLS(GlobalCameraSwitches)
P(MoveSpeed)
P(LookAtDistanceThreshold)
P(DistanceAdjustmentSpeedMultiplier)
P(LookAtSpeedPower)
P(DistanceAdjustmentSpeedMultiplier2)
P(LookAtSpeedPower2)
P(MaxCameraDistance)
P(MinCameraDistance)
P(DefaultCameraDistance)
P(MaxCameraDistanceController)
P(MinCameraDistanceController)
P(DefaultCameraDistanceController)
P(MaxCameraDistanceWithTarget)
P(MinCameraDistanceWithTarget)
P(DefaultCameraDistanceWithTarget)
P(TargetHeightOffset)
P(TargetHeightOffsetController)
P(field_44)
P(field_48)
P(FOV)
P(FOVController)
P(CameraAvoidanceMultiplier)
P(CameraAvoidanceMultiplierWithTarget)
P(CamAvoidanceOffset)
P(CamAvoidanceOffsetWithTarget)
P(CamAvoidancePower)
P(CamAvoidancePowerWithTarget)
P(DistanceAdjustmentThreshold)
P(DistanceAdjustmentSpeedPower)
P(BaseCameraDistance)
P(BaseTargetCameraDistance)
P(ViewportFrameCursorThreshold1)
P(ViewportFrameCursorThreshold2)
P(DefaultCameraDistanceOverhead)
P(MaxCameraDistanceOverhead)
P(MinCameraDistanceOverhead)
P(DefaultPosition)
P(DefaultPosition2)
P(DefaultCombatPosition)
P(DefaultCombatPosition2)
P(DefaultOverheadPosition)
P(DefaultOverheadPosition2)
P(DefaultControllerPosition)
P(DefaultControllerPosition2)
END_CLS()
