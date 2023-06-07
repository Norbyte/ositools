#include <GameDefinitions/PropertyMaps/EntitySystem.inl>
#include <GameDefinitions/PropertyMaps/Events.inl>
#include <GameDefinitions/PropertyMaps/Mod.inl>
#include <GameDefinitions/PropertyMaps/Statuses.inl>
#include <GameDefinitions/PropertyMaps/Character.inl>
#include <GameDefinitions/PropertyMaps/Item.inl>
#include <GameDefinitions/PropertyMaps/Scenery.inl>
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
#include <GameDefinitions/PropertyMaps/Input.inl>
#include <GameDefinitions/PropertyMaps/Physics.inl>
#include <GameDefinitions/PropertyMaps/Trigger.inl>
#include <GameDefinitions/PropertyMaps/GameAction.inl>
#include <GameDefinitions/PropertyMaps/MovementStates.inl>
#include <GameDefinitions/PropertyMaps/ActionStates.inl>
#include <GameDefinitions/PropertyMaps/Controllers.inl>
#include <GameDefinitions/PropertyMaps/Player.inl>
#include <GameDefinitions/PropertyMaps/Camera.inl>
#include <GameDefinitions/PropertyMaps/Skill.inl>
#include <GameDefinitions/PropertyMaps/Wall.inl>


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
		return PropertyOperationResult::Success;
	},
	[](lua_State* L, LifetimeHandle const& lifetime, stats::HitDamageInfo* hit, int index, std::size_t offset, uint64_t flag) {
		WarnDeprecated56("Writing EffectFlags directly is deprecated; use the new boolean properties (Hit, Blocked, Dodged, etc.) instead");
		hit->EffectFlags = (stats::HitFlag)get<uint32_t>(L, index);
		return PropertyOperationResult::Success;
	}
);
#endif

END_CLS()


BEGIN_CLS(ArmorDivider)
P(DamagePerHit)
END_CLS()


BEGIN_CLS(NumberDivider)
P(Type)
P(Amount)
P(Accumulator)
END_CLS()


BEGIN_CLS(NumberDividers)
P_REF(ArmorAbsorptionDivider)
P_REF(DamageDividers)
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
		return PropertyOperationResult::Success;
	}
);
pm.AddProperty("OffsetY",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, ai->DataGrid.OffsetY);
		return PropertyOperationResult::Success;
	}
);
pm.AddProperty("OffsetZ",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, ai->DataGrid.OffsetZ);
		return PropertyOperationResult::Success;
	}
);
pm.AddProperty("GridScale",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, ai->DataGrid.GridScale);
		return PropertyOperationResult::Success;
	}
);
pm.AddProperty("Width",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, (double)ai->DataGrid.Width * ai->DataGrid.GridScale);
		return PropertyOperationResult::Success;
	}
);
pm.AddProperty("Height",
	[](lua_State* L, LifetimeHandle const& lifetime, eoc::AiGrid* ai, std::size_t offset, uint64_t flag) {
		push(L, (double)ai->DataGrid.Height * ai->DataGrid.GridScale);
		return PropertyOperationResult::Success;
	}
);
#endif
END_CLS()


BEGIN_CLS(eoc::VisionGrid)
P_FUN(RaycastToPosition, LuaRaycastToPosition)
P_FUN(RaycastToObject, LuaRaycastToObject)
END_CLS()

BEGIN_CLS(Transform)
P(Matrix)
P(Rotate)
P(Translate)
P(Scale)
END_CLS()

BEGIN_CLS(stats::TDamagePair)
P(Amount)
P(DamageType)
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
P_FUN(AsArray, LuaAsArray)
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

BEGIN_CLS(SoundResource)
INHERIT(DeferredLoadableResource)
P_RO(SoundEventID)
P_RO(SoundEvent)
P_RO(SoundBank)
P_RO(SoundEventUUID)
P_RO(Duration)
P_RO(MaxDistance)
P_RO(Preload)
P_RO(SoundCategory)
P_RO(SoundCodec)
P_RO(GMSoundCategory)
P_RO(DisplayName)
P_RO(GMSubSection)
P_RO(Internal)
END_CLS()

BEGIN_CLS(EffectResource)
INHERIT(DeferredLoadableResource)
P_RO(EffectName)
P_REF(Components)
P_REF(Dependencies)
END_CLS()

BEGIN_CLS(SoundComponent)
INHERIT(BaseComponent)
P(Flags)
P(EntityName)
P_RO(LevelName)
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


BEGIN_CLS(GlobalSwitches::ControlSwitch)
P_REF(PickingBoundLimits)
P_REF(PickingAngleLimits)
P_REF(PickingBoundDistances)
P(PickingMaxSmallBound)
P(PickingDistanceBias)
P(PickingAngleBias)
P(PickingCharacterBoundBias)
P(PickingHeightCutoff2)
P(PickingTorchBias)
P(PickingHeightCutoff)
P(ItemReachMaxDistance)
P(JoystickDistanceMultiplier)
P(JoystickDistanceThreshold1)
P(JoystickDistanceThreshold2)
P(JoystickAngleThreshold)
P(HeightMultiplier)
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
P(ShowFeedback)
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
P(EnableAiLogging)
P(AiLevelScaleScores)
P(AiUsePositionScores)
P(PeaceMode)
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
P(ShowSpeechBubble)
P(Difficulty)
P(GameMode)
P(DebugItemColorOverride)
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
P_REF(CameraSwitchesMode0)
P_REF(CameraSwitchesMode1)
P_REF(ControlSwitches)
P_REF(OverlayColors)
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
P(field_BB0)
P(field_BF4)
P(field_C1A)
P(field_C1B)
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
