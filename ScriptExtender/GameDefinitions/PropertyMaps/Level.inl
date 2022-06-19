BEGIN_CLS(LevelDesc)
P_RO(LevelName)
P_RO(UniqueKey)
P_RO(CustomDisplayLevelName)
P_REF(Paths)
P_RO(Type)
END_CLS()


BEGIN_CLS(Level)
P_REF(LevelDesc)
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
P_REF(PermanetInfluences)
P_REF(TemporaryStatuses)
P_REF(Statuses)
P(HasWeatherProofTalent)
END_CLS()


BEGIN_CLS(esv::EnvironmentalStatusManager)
P(Timer)
P_REF(EnvironmentalInfluences)
END_CLS()


BEGIN_CLS(esv::Level)
INHERIT(Level)
P_REF(AiGrid)
// P_REF(VisionGrid)
/*P_REF(EntityManager)
P_REF(SurfaceManager)
P_REF(GameActionManager)
P_REF(ShroudManager)*/
P_REF(EnvironmentalStatusManager)
// P_REF(LevelCacheTemplateManager)
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
