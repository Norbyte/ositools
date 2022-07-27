#include <GameDefinitions/PropertyMaps/EntitySystem.inl>
#include <GameDefinitions/PropertyMaps/Events.inl>
#include <GameDefinitions/PropertyMaps/Mod.inl>
#include <GameDefinitions/PropertyMaps/Statuses.inl>
#include <GameDefinitions/PropertyMaps/Character.inl>
#include <GameDefinitions/PropertyMaps/Item.inl>
#include <GameDefinitions/PropertyMaps/Level.inl>
#include <GameDefinitions/PropertyMaps/Stats.inl>
#include <GameDefinitions/PropertyMaps/Surface.inl>
#include <GameDefinitions/PropertyMaps/RootTemplates.inl>
#include <GameDefinitions/PropertyMaps/Projectile.inl>
#include <GameDefinitions/PropertyMaps/UseActions.inl>
#include <GameDefinitions/PropertyMaps/Render.inl>
#include <GameDefinitions/PropertyMaps/Material.inl>
#include <GameDefinitions/PropertyMaps/Effect.inl>
#include <GameDefinitions/PropertyMaps/UI.inl>
#include <GameDefinitions/PropertyMaps/CharacterCreation.inl>
#include <GameDefinitions/PropertyMaps/Alignment.inl>
#include <GameDefinitions/PropertyMaps/Ai.inl>
#include <GameDefinitions/PropertyMaps/Combat.inl>


BEGIN_CLS(TypeInformation)
P_RO(TypeName)
P_RO(Kind)
P_RO(NativeName)
P_RO(KeyType)
P_RO(ElementType)
P_RO(ParentType)
P_REF(Members)
P_REF(Methods)
P_RO(HasWildcardProperties)
P_REF(EnumValues)
P_REF(ReturnValues)
P_REF(Params)
P_RO(VarargParams)
P_RO(VarargsReturn)
P_RO(IsBuiltin)
P_RO(ModuleRole)
END_CLS()


BEGIN_CLS(RuntimeStringHandle)
P(Handle)
P(ReferenceString)
END_CLS()


BEGIN_CLS(TranslatedString)
P_REF(Handle)
P_REF(ArgumentString)
END_CLS()


BEGIN_CLS(stats::HitDamageInfo)
P(Equipment)
P(TotalDamageDone)
P(DamageDealt)
P(DeathType)
P(DamageType)
P(AttackDirection)
P(ArmorAbsorption)
P(LifeSteal)
P_BITMASK(EffectFlags)
P(HitWithWeapon)
P_REF(DamageList)

// EffectFlags is an integer in v55
#if defined(GENERATING_PROPMAP)
pm.AddProperty("EffectFlags",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::HitDamageInfo* hit, std::size_t offset, uint64_t flag) {
		WarnDeprecated56("Reading EffectFlags directly is deprecated; use the new boolean properties (Hit, Blocked, Dodged, etc.) instead");
		push(L, (uint32_t)hit->EffectFlags);
		return true;
	},
	[](lua_State* L, LifetimeHandle const& lifetime, stats::HitDamageInfo* hit, int index, std::size_t offset, uint64_t flag) {
		WarnDeprecated56("Writing EffectFlags directly is deprecated; use the new boolean properties (Hit, Blocked, Dodged, etc.) instead");
		hit->EffectFlags = (stats::HitFlag)get<uint32_t>(L, index);
		return true;
	}
);
#endif

END_CLS()


BEGIN_CLS(Bound)
P(Center)
P(Radius)
P(Min)
P(Max)
P(IsCenterSet)
END_CLS()


BEGIN_CLS(GameTime)
P_RO(Time)
P_RO(DeltaTime)
P_RO(Ticks)
END_CLS()


BEGIN_CLS(esv::DamageHelpers)
P(SimulateHit)
P(HitType)
P(NoHitRoll)
P(ProcWindWalker)
P(ForceReduceDurability)
P(HighGround)
P(CriticalRoll)
P(HitReason)
P(DamageSourceType)
P(Strength)
END_CLS()


BEGIN_CLS(Trigger)
// P_RO(Handle) // Defunct, use GetObjectHandle() instead
// P_RO(UUID) // Defunct, use GetGuid() instead
P_RO(SyncFlags)
P_RO(Translate)
P_RO(TriggerType)
P_RO(IsGlobal)
P_RO(Level)
P_RO(Rotate)
P_RO(Rotate2)
END_CLS()


// FIXME - stub!
BEGIN_CLS(esv::AtmosphereTrigger)
END_CLS()


BEGIN_CLS(SoundVolumeTriggerData)
P_RO(AmbientSound)
P_RO(Occlusion)
P_RO(AuxBus1)
P_RO(AuxBus2)
P_RO(AuxBus3)
P_RO(AuxBus4)
END_CLS()


BEGIN_CLS(esv::ASAttack)
P_RO(TargetHandle)
P_RO(TargetPosition)
P_RO(IsFinished)
P_RO(AlwaysHit)
P_RO(TimeRemaining)
P_RO(AnimationFinished)
P_RO(TotalHits)
P_RO(TotalHitOffHand)
P_RO(TotalShoots)
P_RO(TotalShootsOffHand)
P_RO(HitCount)
P_RO(HitCountOffHand)
P_RO(ShootCount)
P_RO(ShootCountOffHand)
P_RO(MainWeaponHandle)
P_RO(OffWeaponHandle)
P_RO(MainHandHitType)
P_RO(OffHandHitType)
P_RO(ProjectileUsesHitObject)
P_RO(ProjectileStartPosition)
P_RO(ProjectileTargetPosition)
P_RO(DamageDurability)
END_CLS()


BEGIN_CLS(esv::ASPrepareSkill)
P_RO(SkillId)
P_RO(PrepareAnimationInit)
P_RO(PrepareAnimationLoop)
P_RO(IsFinished)
P_RO(IsEntered)
END_CLS()


BEGIN_CLS(esv::SkillState)
P_RO(SkillId)
P_RO(CharacterHandle)
P_RO(SourceItemHandle)
P_RO(CanEnter)
P_RO(IsFinished)
P_RO(IgnoreChecks)
P_RO(IsStealthed)
P_RO(PrepareTimerRemaining)
P_RO(ShouldExit)
P_RO(CleanseStatuses)
P_RO(StatusClearChance)
P_RO(CharacterHasSkill)
END_CLS()

BEGIN_CLS(eoc::AiGrid)
P_FUN(SearchForCell, SearchForCell)
P_FUN(GetCellInfo, GetCellInfo)
P_FUN(GetHeight, GetHeight)
P_FUN(SetHeight, SetHeight)
P_FUN(GetAiFlags, GetAiFlags)
P_FUN(SetAiFlags, SetAiFlags)
P_FUN(UpdateAiFlagsInRect, UpdateAiFlagsInRect)
P_FUN(FindCellsInRect, FindCellsInRect)

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("OffsetX", float)
ADD_TYPE("OffsetY", float)
ADD_TYPE("OffsetZ", float)
ADD_TYPE("GridScale", float)
ADD_TYPE("Width", uint32_t)
ADD_TYPE("Height", uint32_t)
#endif

#if defined(GENERATING_PROPMAP)
pm.AddProperty("OffsetX",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, ai->DataGrid.OffsetX);
		return true;
	}
);
pm.AddProperty("OffsetY",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, ai->DataGrid.OffsetY);
		return true;
	}
);
pm.AddProperty("OffsetZ",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, ai->DataGrid.OffsetZ);
		return true;
	}
);
pm.AddProperty("GridScale",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, ai->DataGrid.GridScale);
		return true;
	}
);
pm.AddProperty("Width",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, (double)ai->DataGrid.Width * ai->DataGrid.GridScale);
		return true;
	}
);
pm.AddProperty("Height",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, (double)ai->DataGrid.Height * ai->DataGrid.GridScale);
		return true;
	}
);
#endif
END_CLS()


BEGIN_CLS(Transform)
P(Matrix)
P(Rotate)
P(Translate)
P(Scale)
END_CLS()

BEGIN_CLS(stats::DamagePairList)
P_FUN(GetByType, GetByType)
P_FUN(Add, AddDamage)
P_FUN(Clear, ClearAll)
P_FUN(Multiply, Multiply)
P_FUN(Merge, LuaMerge)
P_FUN(ConvertDamageType, ConvertDamageType)
P_FUN(AggregateSameTypeDamages, AggregateSameTypeDamages)
P_FUN(ToTable, LuaToTable)
P_FUN(CopyFrom, LuaCopyFrom)
END_CLS()

BEGIN_CLS(esv::Trigger)
END_CLS()

BEGIN_CLS(Resource)
P_RO(SourceFile)
P_RO(PackageName)
P_RO(ModName)
P_RO(UUID)
P_RO(Name)
P_RO(IsLocalized)
P_RO(IsActive)
P_RO(IsDirty)
P_RO(IsOriginal)
END_CLS()

BEGIN_CLS(DeferredLoadableResource)
INHERIT(Resource)
END_CLS()


BEGIN_CLS(GlobalSwitches::SoundSetting)
P(Value)
P(Value2)
P(Value3)
P_RO(ConfigKey)
P_RO(PreviewKeyName)
P_RO(RTPCKeyName)
P_RO(Name)
END_CLS()


BEGIN_CLS(GlobalSwitches::SomeOption)
P(A)
P(B)
P(C)
P(D)
END_CLS()


BEGIN_CLS(GlobalSwitches)
P(DebugViewType)
P(Cleave_M)
P(ShowCloths)
P(ShowDrawStats)
P(ShowDebugLines)
P(ShowFPS)
P(ShowPhysics)
P(ShowRagdollInfo)
P(ShowRaycasting)
P(ShowPhysXBoxes)
P(YieldOnLostFocus)
P(EnableAngularCulling)
P(LoadScenery)
P(LoadTextShaders)
P(TexelDensityMin)
P(TexelDensityIdeal)
P(TexelDensityMax)
P(TexelDensityExtreme)
P(UIScaling)
P(NodeWaitTimeMultiplier)
P(EnableGenome)
P(UpdateOffstageOverlayMaterials_M)
P(UpdateInvisibilityOverlayMaterials_M)
P(FileLoadingLog)
P(FileSavingLog)
P(MouseSensitivity)
P(MouseScrollSensitivity)
P(ControllerSensitivity)
P(ControllerStickDeadZone)
P(ControllerStickPressDeadZone)
P(ControllerTriggerDeadZone)
P(ServerMonitor)
P(StoryLog)
P(Story)
P(StoryEvents)
P(DisableStoryPatching)
P(ForceStoryPatching)
P(StatsArgPassed)
P(EnableSoundErrorLogging)
P(ChatLanguage)
P(ShowLocalizationMarkers)
P(ForcePort)
P(EnablePortmapping)
P(GameVisibilityOnline)
P(GameVisibilityLAN)
P(GameVisibilityDirect)
P(DisableLocalMessagePassing)
P(EnableSteamP2P)
P(DirectConnectAddress)
P(UpdateScene)
P(UpdatePhysXScene)
P(DoUnlearnCheck)
P(VisualizeTextures)
P(EnableModuleHashing)
P(SomePhysXRagdollFlag)
P(UseLevelCache)
P(LoadAllEffectPools)
P(ScriptLog)
P(ShroudEnabled)
P(GodMode)
P(Fading)
P(CheckRequirements)
P(UseEndTurnFallback)
P(ShowCharacterCreation)
P(ShorOriginIntroInCC)
P(DisableArmorSavingThrows)
P(AlwaysShowSplitterInTrade)
P(ResetTutorialsOnNewGame)
P(ControllerLayout)
P(RotateMinimap)
P(ShowOverheadText)
P(ShowOverheadDialog)
P_REF(SoundSettings)
P(SoundQuality)
P(MuteSoundWhenNotFocused)
P(SoundPartyLosingThreshold)
P(SoundPartyWinningThreshold)
P(GameCameraRotation)
P(CameraSpeedMultiplier)
P(GameCameraRotationLocked)
P(GameCameraEnableCloseUpDialog)
P(GameCameraEnableDynamicCombatCamera)
P(GameCameraShakeEnabled)
P(GameCamAvoidScenery)
P(GameCameraControllerMode)
P(NrOfQuickSaves)
P(NrOfAutoSaves)
P(NrOfReloadSaves)
P(MaxNrOfQuickSaves)
P(MaxNrOfAutoSaves)
P(MaxNrOfReloadSaves)
P(CanAutoSave)
P(ForceSplitscreen)
P(MaxAmountDialogsInLog)
P(ControllerCharacterWalkThreshold)
P(ControllerCharacterRunThreshold)
P(ControllerMoveSweepCone)
P(AIBoundsSizeMultiplier)
P(WeaponRangeMultiplier)
P(RotateRampSpeed)
P(MaxRotateSpeed)
P(MoveDirectionCount)
P(FadeCharacters)
P(SomeAiLogFlag)
P(AutoIdentifyItems)
P(EnableVoiceLogging)
P(MouseLock)
P(DisableEdgePanning)
P(DualDialogsEnabled)
P(EscClosesAllUI)
P(ShowSubtitles)
P(EnableGameOver)
P(UseRadialContextMenu)
P(LogSaveLoadErrors)
P(EnableAiThinking)
P(EnableBlending)
P(OverheadZoomModifier)
P(OverheadZoomEnabled)
P(AddGenericKeyWords)
P(AddStoryKeyWords)
P(CacheDialogs)
P(AllowXPGain)
P(ShowSubtitles2)
P(Difficulty)
P(GameMode)
P(ItemColorOverride)
P(PeaceCharacterHighlightMode)
P(CombatCharacterHighlightMode)
P(TacticalCharacterHighlightMode)
P(ArenaCharacterHighlightMode)
P(TutorialBoxMode)
P(PeaceCharacterHighlightFlag)
P(CombatCaracterHighlightFlag)
P(TacticalCharacterHighlightFlag)
P(ArenaCharacterHighlightFlag)
P(ShowBuildVersion)
P(SomeConditionalStoryLogField)
P(NextServerMode)
P(ServerMode)
P(ServerFrameCap)
P(GameMasterBind)
P(AutoRemoveHotbarSkills)
P(AutoFillHotbarCategories)
P(AllowMovementFreePointer)

#if !defined(_NDEBUG)
P(field_E)
P(field_11)
P(field_14)
P(field_16)
P(field_19)
P(field_1B)
P(field_1D)
P(field_1E)
P(field_1F)
P(field_20)
P(field_21)
P(field_3D)
P(field_63)
P(field_89)
P(field_C0)
P(field_C8)
P(field_C9)
P(field_CC)
P(field_CD)
P(field_CE)
P(field_CF)
P(field_DD)
P(field_DE)
P(field_DF)
P(field_E8)
P(field_E9)
P(field_EA)
P(field_EB)
P(field_EC)
P(field_ED)
P(field_EE)
P(field_EF)
P(field_B8E)
P(field_B8F)
P(field_BAA)
P(field_BB0)
P(field_BC9)
P(field_BCA)
P(field_BCB)
P(field_BCC)
P(field_BCD)
P(field_BCE)
P(field_BCF)
P(field_BD2)
P(field_BD3)
P(field_BF4)
P(field_BF5)
P(field_BF6)
P(field_BF7)
P(field_C1A)
P(field_C1B)
P_REF(Floats1)
P_REF(Floats2)
P_REF(Floats3)
P_REF(Options2)
#endif
END_CLS()



BEGIN_CLS(GraphicSettings)
P(Fullscreen)
P(FakeFullscreenEnabled)
P(ScreenWidth)
P(ScreenHeight)
P(RefreshRateNumerator)
P(RefreshRateDenominator)
P(MonitorIndex)
P(VSync)
P(VSyncDivider)
P(TripleBuffering)
P(ClothGPUAcceleration)
P(GammaCorrection)
P(HDRGamma)
P(TextureDetail)
P(TextureFiltering)
P(TextureStreamingEnabled)
P(ModelDetail)
P(AnimationAllowedPixelError)
P(LightingDetail)
P(EnableSpotLightsSMAA)
P(ShadowsEnabled)
P(PointLightShadowsEnabled)
P(ShadowQuality)
P(MaxDrawDistance)
P(MaxDrawDistance2)
P(AntiAliasing)
P(BloomEnabled)
P(DOFEnabled)
P(GodRaysEnabled)
P(LensFlareEnabled)
P(MotionBlurEnabled)
P(SSAOEnabled)
P(EnableSSR)
P(UseForwardRendering)
P(EnableLightAssignmentStage)
P(FrameCapEnabled)
P(FrameCapFPS)
P(RenderMT)
P(HDRPaperWhite)
P(HDRMaxNits)
P(PostProcessingInjectTexture)
P(ShowHDRCalibration)
END_CLS()
