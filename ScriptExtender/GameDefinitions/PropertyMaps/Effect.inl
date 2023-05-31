BEGIN_CLS(ecl::MultiEffectHandler)
P_RO(Position)
P_RO(TargetObjectHandle)
P_RO(ListenForTextKeysHandle)
P_RO(WeaponBones)
P_REF(TextKeyEffects)
P_REF(Effects)
P_REF(AttachedVisualComponents)
P_REF(Visuals)
P_REF(WeaponAttachments)
P_RO(ListeningOnTextKeys)
END_CLS()


BEGIN_CLS(ecl::MultiEffectHandler::EffectInfo)
P_RO(Effect)
P_REF(BoneNames)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(ecl::MultiEffectHandler::MultiEffectVisual)
// FIXME - P_RO(VisualEntityHandle)
P_REF(MultiEffectHandler)
P_REF(OS_FS)
END_CLS()


BEGIN_CLS(ecl::MultiEffectHandler::WeaponAttachmentInfo)
P_RO(EffectName)
P_RO(BoneNames)
P_RO(VisualId)
END_CLS()


BEGIN_CLS(WeaponAnimData)
P(FirstTextEventTime)
P(TextEventTime)
P(TimeDelta)
END_CLS()


BEGIN_CLS(ecl::BeamEffectHandler)
P(SourcePosition)
P(SourceHandle)
P(TargetHandle)
P(SourceHandle2)
P_REF(TextKeyEffectDescs)
P_REF(TextKeyEffects)
P_REF(Effects)
P(ListeningOnTextKeys)
END_CLS()


BEGIN_CLS(ecl::lua::visual::ClientMultiVisual)
INHERIT(ecl::MultiEffectHandler)
P_REF(AttachedVisuals)
P_RO(Handle)
P_FUN(Delete, Delete)
P_FUN(ParseFromStats, ParseFromStats)
P_FUN(AddVisual, AddVisual)
END_CLS()


BEGIN_CLS(Effect)
INHERIT(Visual)
P_REF(Resource)
P_RO(EffectFlags)
P(SoundObjectHandle)
P_REF(CachedWorldTransform)
P(EffectName)
P_REF(ParentEffect)
P(CachedParentFadeOpacity)
P(FreezeFadeOpacity)
P_RO(RefCount)
END_CLS()


BEGIN_CLS(aspk::Input)
P(HasValue)
P_RO(Name)
END_CLS()

BEGIN_CLS(aspk::TypedInput<float>)
INHERIT(aspk::Input)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedInput<glm::vec3>)
INHERIT(aspk::Input)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedInput<glm::vec4>)
INHERIT(aspk::Input)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedInput<FixedString>)
INHERIT(aspk::Input)
P(Value)
END_CLS()


BEGIN_CLS(aspk::Phase)
P(Duration)
P(PlayCount)
END_CLS()


BEGIN_CLS(aspk::EffectHeader)
P(Duration)
P_REF(Phases)
END_CLS()


BEGIN_CLS(aspk::ColorARGBKeyFrameData)
P(Time)
P(Color)
END_CLS()


BEGIN_CLS(aspk::FloatKeyFrameData)
P(Time)
P(Value)
END_CLS()


BEGIN_CLS(lsfx::Parameter)
P_RO(Name)
P(Value)
END_CLS()


BEGIN_CLS(lsfx::Property)
P_RO(FullName)
P_RO(AttributeName)
P_REF(Parameters)
END_CLS()


BEGIN_CLS(aspk::Property)
INHERIT(lsfx::Property)
P_RO(Input)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<bool>)
INHERIT(aspk::Property)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<int32_t>)
INHERIT(aspk::Property)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<glm::ivec2>)
INHERIT(aspk::Property)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<Array<aspk::ColorARGBKeyFrameData*>>)
INHERIT(aspk::Property)
P_REF(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<float>)
INHERIT(aspk::Property)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<glm::vec2>)
INHERIT(aspk::Property)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<Array<aspk::FloatKeyFrameData*>>)
INHERIT(aspk::Property)
P_REF(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<STDString>)
INHERIT(aspk::Property)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<glm::vec3>)
INHERIT(aspk::Property)
P(Value)
END_CLS()

BEGIN_CLS(aspk::TypedProperty<FixedString>)
INHERIT(aspk::Property)
P(Value)
END_CLS()


BEGIN_CLS(lsfx::Module)
P_RO(Name)
P_REF(PropertyNames)
P_REF(BoundProperties)
END_CLS()


BEGIN_CLS(lsfx::Component)
P_REF(Properties)
P_RO(UUID)
P_RO(Name)
P_REF(Modules)
END_CLS()


BEGIN_CLS(aspk::Component)
INHERIT(lsfx::Component)
P_REF(NamedProperties)
P_RO(TrackGroupIndex)
P(StartTime)
P(EndTime)
P(IsActive)
P(StayAlive)
END_CLS()


BEGIN_CLS(aspk::Effect)
INHERIT(Effect)
P(PlayToEnd)
P(Speed)
P_REF(Components)
P(Paused)
P(RequestAdvancePhase)
P(NeedsRewind)
P(NumPhaseLoops)
P(PhaseTime)
P(RemainingTime)
P(RemainingPhaseTime)
P(CurrentTime)
P(CurrentPhase)
P_REF(EffectHeader)
P_REF(Inputs)
END_CLS()


BEGIN_CLS(fx::Effect)
INHERIT(aspk::Effect)
P(HasBound)
P(IsInitialized)
END_CLS()


BEGIN_CLS(esv::Effect)
INHERIT(BaseComponent)
P_RO(NetID)
P(Loop)
P(ForgetEffect)
P_RO(IsForgotten)
P_RO(IsDeleted)
P_RO(Duration)
P(EffectName)
P(Target)
P(BeamTarget)
P(Position)
P(Rotation)
P(Scale)
P(Bone)
P(BeamTargetBone)
P(BeamTargetPos)
P(DetachBeam)
P_FUN(Delete, Delete)
END_CLS()