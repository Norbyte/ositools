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


BEGIN_CLS(ecl::lua::effect::ClientMultiEffect)
INHERIT(ecl::MultiEffectHandler)
P_FUN(Delete, Delete)
END_CLS()
