BEGIN_CLS(LevelDesc)
P_RO(LevelName)
P_RO(UniqueKey)
P_RO(CustomDisplayLevelName)
P_REF(Paths)
P_RO(Type)
END_CLS()


BEGIN_CLS(Level)
P_REF(LevelDesc)
P_REF(LocalTemplateManager)
// TODO incomplete - Not exposed yet
// P_REF(Scene)
P_REF(PhysicsScene)
P_REF(GameObjects)
P_REF(ActivePersistentLevelTemplates)
END_CLS()


BEGIN_CLS(esv::EnvironmentalInfluences::Status)
P_RO(Handle)
P(FirstAttempt)
P(IsForced)
END_CLS()


BEGIN_CLS(esv::EnvironmentalInfluences::PermanentInfluence)
P(Strength)
P(WeatherStrength)
END_CLS()


BEGIN_CLS(esv::EnvironmentalInfluences::TemporaryStatus)
P_RO(Handle)
P(Strength)
P(WeatherStrength)
P(LifeTime)
P(IsForced)
END_CLS()


BEGIN_CLS(esv::EnvironmentalInfluences)
P_REF(Unknown)
P_RO(OwnerHandle)
P_REF(PermanentInfluences)
P_REF(TemporaryStatuses)
P_REF(Statuses)
P(HasWeatherProofTalent)
END_CLS()


BEGIN_CLS(esv::EnvironmentalStatusManager)
P(Timer)
P_REF(EnvironmentalInfluences)
END_CLS()


BEGIN_CLS(esv::ItemConversionHelpers)
P_REF(RegisteredItems)
P_REF(ActivatedItems)
P_REF(GlobalItemHandles)
END_CLS()


BEGIN_CLS(esv::CharacterConversionHelpers)
P_REF(RegisteredCharacters)
P_REF(ActivatedCharacters)
END_CLS()


BEGIN_CLS(esv::TriggerConversionHelpers)
P_REF(RegisteredTriggers)
END_CLS()


BEGIN_CLS(esv::ProjectileConversionHelpers)
P_REF(RegisteredProjectiles)
END_CLS()


BEGIN_CLS(esv::EntityManager)
P_REF(ItemConversionHelpers)
P_REF(CharacterConversionHelpers)
P_REF(TriggerConversionHelpers)
P_REF(ProjectileConversionHelpers)
END_CLS()


BEGIN_CLS(esv::CharacterManager::AnimationBlueprintEntry)
P_REF(Character)
// TODO - AnimationBlueprintComponent
END_CLS()


BEGIN_CLS(esv::CharacterManager::TransformParams)
P(PeerID)
P(TargetCharacterHandle)
P(Template)
P(TemplateCharacterHandle)
P(EquipmentSetName)
P(TransformType)
P(Flags)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(esv::CharacterManager)
P_REF(RegisteredCharacters)
P_REF(ActiveCharacters)
P_REF(PendingTransforms)
P_REF(NetPendingTransforms)
P_REF(ActiveAnimationBlueprints)
END_CLS()


BEGIN_CLS(esv::ItemManager::TransformParams)
P(ItemHandle)
P(TemplateHandle)
P(Flags)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(esv::ItemManager)
P_REF(Mover)
P_REF(Items)
P_REF(ActiveItems)
P_REF(PendingTransforms)
P_REF(NetPendingTransforms)
END_CLS()


BEGIN_CLS(esv::ProjectileManager)
P_REF(ActiveProjectiles)
P_REF(ProjectilesToDestroy)
END_CLS()


BEGIN_CLS(esv::SurfaceManager)
P_REF(SurfaceActions)
P_REF(Surfaces)
P_REF(SurfaceCells)
P_REF(SurfaceCellSetsByLayer)
END_CLS()


BEGIN_CLS(esv::GameActionManager)
P_REF(GameActions)
END_CLS()


BEGIN_CLS(esv::EffectManager)
P_REF(Effects)
P_REF(DeletedEffects)
P_REF(ForgottenEffects)
END_CLS()


BEGIN_CLS(esv::Level)
INHERIT(Level)
P_REF(EntityManager)
P_REF(AiGrid)
// P_REF(VisionGrid)
// P_REF(SightManager)
P_REF(CharacterManager)
P_REF(ItemManager)
P_REF(ProjectileManager)
P_REF(SurfaceManager)
P_REF(GameActionManager)
P_REF(EffectManager)
// P_REF(ShroudManager)
P_REF(EnvironmentalStatusManager)
P_REF(LevelCacheTemplateManager)
END_CLS()


BEGIN_CLS(esv::LevelManager)
P_REF(CurrentLevel)
P_REF(Levels)
P_REF(LevelDescs)
P_REF(Levels2)
END_CLS()


BEGIN_CLS(ecl::Level)
INHERIT(Level)
P_REF(AiGrid)
// etc ...
END_CLS()


BEGIN_CLS(ecl::LevelManager)
P_REF(CurrentLevel)
P_REF(Levels)
P_REF(LevelDescs)
P_REF(Levels2)
END_CLS()
