BEGIN_CLS(MaterialParameter)
P_RO(Parameter)
P_RO(UniformName)
P_RO(ShaderFlags)
P_RO(Enabled)
END_CLS()


BEGIN_CLS(MaterialVector3)
P(Value)
P(IsColor)
END_CLS()


BEGIN_CLS(MaterialVector4)
P(Value)
P(IsColor)
END_CLS()


BEGIN_CLS(MaterialTexture2D)
P(ID)
END_CLS()


BEGIN_CLS(MaterialSamplerState)
P(TextureFilterOverride)
P(TextureAddressMode)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<float>)
INHERIT(MaterialParameter)
P(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<glm::vec2>)
INHERIT(MaterialParameter)
P(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<MaterialVector3>)
INHERIT(MaterialParameter)
P_REF(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<MaterialVector4>)
INHERIT(MaterialParameter)
P_REF(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<MaterialTexture2D>)
INHERIT(MaterialParameter)
P_REF(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<MaterialSamplerState>)
INHERIT(MaterialParameter)
P_REF(Value)
END_CLS()


BEGIN_CLS(MaterialParameters)
P_REF(ParentMaterial)
P_REF(ParentAppliedMaterial)
// FIXME - TextureMaps
P_REF_TY(Scalars, ObjectSet<MaterialParameterWithValue<float>>)
P_REF_TY(Vector2s, ObjectSet<MaterialParameterWithValue<glm::vec2>>)
P_REF_TY(Vector3s, ObjectSet<MaterialParameterWithValue<MaterialVector3>>)
P_REF_TY(Vector4s, ObjectSet<MaterialParameterWithValue<MaterialVector4>>)
P_REF_TY(Texture2Ds, ObjectSet<MaterialParameterWithValue<MaterialTexture2D>>)
P_REF_TY(SamplerStates, ObjectSet<MaterialParameterWithValue<MaterialSamplerState>>)
END_CLS()


BEGIN_CLS(AppliedMaterial)
P_REF(Material)
P_REF(Renderable)
P_RO(HasDebugMaterial)
P_RO(DefaultShaderType)
P_RO(ForwardShaderType)
P_RO(ShadowShaderType)
P_RO(DepthShaderType)
P_RO(DefaultShaderHandle)
P_RO(ForwardShaderHandle)
P_RO(ShadowShaderHandle)
P_RO(DepthShaderHandle)
P_RO(DefaultVertexFormatHandle)
P_RO(ForwardVertexFormatHandle)
P_RO(ShadowVertexFormatHandle)
P_RO(DepthVertexFormatHandle)
P_RO(BlendState)
P_REF(MaterialParameters)
P_RO(OverlayOffset)
P_RO(IsOverlay)
P_RO(ForceForwardShading)
P_RO(ForcePostRefractionAlpha)
END_CLS()


BEGIN_CLS(ShaderDesc)
P_RO(VSHash)
P_RO(PSHash)
P_RO(Flags)
END_CLS()


BEGIN_CLS(Material)
P_RO(MaterialID)
P_REF(ShaderDescs)
P_REF(ShaderPaths)
P_RO(ShadingModel)
P_RO(ForwardLightingMode)
P_RO(BlendState)
P_RO(RasterizerState)
P_RO(DepthState)
P_RO(StencilRef)
P_RO(UVCount)
P_RO(Flags)
P_RO(MaterialPassHint)
P_RO(MaterialType)
P_REF(MaterialParameters)
END_CLS()


BEGIN_CLS(MaterialInfo)
P_RO(GameObjectHandle)
P_RO(MaterialUUID)
P(OverlayOffset)
P(ApplyFlags)
P_BITMASK(ApplyFlags)
P_RO(Flags)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(OverlayMaterialRequest)
P(FadeValue)
P(GameObjectHandle)
P(MaterialUUID)
P(OverlayOffset)
P(ApplyFlags)
P_BITMASK(ApplyFlags)
P_RO(Flags)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(ecl::MaterialInfo)
INHERIT(MaterialInfo)
P_REF(ScalarParamaters)
P_REF(Vec2Parameters)
P_REF(Vec3Parameters)
P_REF(Vec4Parameters)
P_RO(RefCount)
END_CLS()


BEGIN_CLS(ecl::FadingMaterialInfo)
INHERIT(ecl::MaterialInfo)
P(FadeParameter)
P(FadeValue)
P(InitialFadeValue)
P_RO(StateFlags)
P_RO(RequestFlags)
END_CLS()


BEGIN_CLS(ecl::MaterialManager)
P_REF(FadingMaterials)
P_REF(Materials)
P_REF(OverlayMaterialRequests)
END_CLS()
