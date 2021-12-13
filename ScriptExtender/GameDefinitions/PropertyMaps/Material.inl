BEGIN_CLS(MaterialParameter)
P_RO(Parameter)
P_RO(UniformName)
P_RO(GroupName)
P_RO(ShaderFlags)
P_RO(Enabled)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<float>)
P_REF(Name)
P_RO(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<glm::vec2>)
P_REF(Name)
P_RO(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<glm::vec3>)
P_REF(Name)
P_RO(Value)
END_CLS()


BEGIN_CLS(MaterialParameterWithValue<glm::vec4>)
P_REF(Name)
P_RO(Value)
END_CLS()


BEGIN_CLS(MaterialParameters)
P_REF(ParentMaterial)
P_REF(ParentAppliedMaterial)
// FIXME - TextureMaps
P_REF(Scalars)
P_REF(Vector2s)
P_REF(Vector3s)
P_REF(Vector4s)
// FIXME - Texture2Ds, SamplerStates
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
P_REF(AppliedMaterials)
END_CLS()
