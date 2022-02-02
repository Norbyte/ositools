BEGIN_CLS(Skeleton)
// No simple way to expose bones for now
END_CLS()


BEGIN_CLS(MeshBinding)
P_REF(Link)
P_RO(Transform)
P_REF(Bound)
END_CLS()


BEGIN_CLS(PhysicsRagdoll)
// No simple way to expose for now
END_CLS()


BEGIN_CLS(Actor)
P_REF(Skeleton)
P_REF(MeshBindings)
P_REF(PhysicsRagdoll)
P_RO(TextKeyPrepareFlags)
P_REF(Visual)
P_REF(Time)
END_CLS()


BEGIN_CLS(MoveableObject)
P_REF(LocalTransform)
P_REF(WorldTransform)
// TODO - no reason to expose - P_REF(ContainingScene)
P_RO(ObjectFlags)
P_RO(DirtyFlags)
END_CLS()


BEGIN_CLS(AnimationSet::AnimationDescriptor)
P_RO(ID)
P_RO(Name)
END_CLS()


BEGIN_CLS(AnimationSet)
P_REF(AnimationSubSets)
P_RO(Type)
END_CLS()


BEGIN_CLS(VisualResource::ClothParam)
P(UUID)
P(LinearStiffness)
P(BendingStiffness)
P(Iterations)
P(PoseMatching)
P(Margin)
P(MassPerMeterSqr)
P(AtmosphericWindEnabled)
P(Lift)
P(Drag)
P(FrontalWindSpeed)
P(FrontalWindFrequency)
P(FrontalWindVariance)
END_CLS()


BEGIN_CLS(VisualResource::ObjectDesc)
P(ObjectID)
P(MaterialID)
P(LOD)
END_CLS()


BEGIN_CLS(VisualResource::Attachment)
P(UUID)
P(Name)
END_CLS()


BEGIN_CLS(VisualResource::BonePosRot)
P(Position)
P(Rotation)
END_CLS()


BEGIN_CLS(VisualResource)
P_RO(Template)
P_REF(Objects)
P_REF(LODDistances)
P_REF(Attachments)
P_REF(KnownAnimationSetOverrides)
P_REF(AnimationWaterfall)
P_REF(CustomAnimationSet)
P_REF(ResolvedAnimationSet)
P_RO(BlueprintInstanceResourceID)
P_REF(ClothParams)
P_REF(Bones)
END_CLS()


BEGIN_CLS(Visual::Attachment)
P_REF(Visual)
P_RO(DummyAttachmentBoneIndex)
P_RO(BoneIndex)
P_RO(AttachmentBoneName)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(Visual::ObjectDesc)
P_REF(Renderable)
P_RO(field_8)
END_CLS()


BEGIN_CLS(Visual)
INHERIT(MoveableObject)
P_RO(Handle)
P_REF(Actor)
P_REF(Skeleton)
P_REF(SubObjects)
P_REF(LODDistances)
P_REF(Attachments)
P_REF(Parent)
P_REF(VisualResource)
P_REF(GameObject)
P_BITMASK(VisualFlags)
P_RO(TextKeyPrepareFlags)
P_RO(CullFlags)
P_RO(PlayingAttachedEffects)
P_RO(ShowMesh)
P_RO(HasCloth)
P_RO(ChildVisualHasCloth)
P_RO(FadeOpacity)
P_RO(ReceiveColorFromParent)

P_FUN(OverrideScalarMaterialParameter, OverrideMaterialParameter<float>)
P_FUN(OverrideVec2MaterialParameter, OverrideMaterialParameter<glm::vec2>)
P_FUN(OverrideVec3MaterialParameter, OverrideVec3MaterialParameter)
P_FUN(OverrideVec4MaterialParameter, OverrideVec4MaterialParameter)
P_FUN(OverrideTextureMaterialParameter, OverrideTextureMaterialParameter)
END_CLS()


BEGIN_CLS(PhysicsShape)
P_RO(Name)
P_RO(Translate)
P_RO(Rotate)
P_RO(Scale)
END_CLS()


BEGIN_CLS(dse::PropertyList)
P_RO(RenderPasses)
P_RO(OpaqueChannel)
P_RO(AlphaChannel)
P_RO(field_6)
P_RO(field_7)
P_RO(TransformType)
END_CLS()


BEGIN_CLS(RenderableObject)
INHERIT(MoveableObject)
// TODO - Model, ClothModel
P_RO(IsSimulatedCloth)
P_REF(PropertyList)
P_REF(AppliedMaterials)
P_REF(ActiveAppliedMaterial)
P_REF(AppliedOverlayMaterials)
P_RO(HasPhysicsProxy)
P_REF(ClothPhysicsShape)
P_RO(LOD)
END_CLS()


BEGIN_CLS(AnimatableObject)
INHERIT(RenderableObject)
P_REF(MeshBinding)
// TODO - OverrideTransforms?
END_CLS()


BEGIN_CLS(Light)
INHERIT(MoveableObject)
P_RO(TranslateOffset)
P_RO(TranslateOffset2)
P_RO(Color)
P_RO(Radius)
P_RO(Intensity)
P_RO(IntensityOffset)
P_RO(FlickerSpeed)
P_RO(FlickerAmount)
P_RO(MovementSpeed)
P_RO(MovementAmount)
P_RO(VolumetricLightIntensity)
P_RO(VolumetricLightCollisionProbability)
P_RO(IsFlickering)
P_RO(IsMoving)
P_RO(CastShadow)
P_RO(LightVolume)
P_RO(IsEnabled)
P_RO(LightType)
P_RO(LightVolumeSamples)
P_RO(LightVolumeMapping)
P_RO(IsUpdateJobRunning)
END_CLS()

