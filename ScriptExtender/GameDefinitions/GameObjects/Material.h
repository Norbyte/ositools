#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

struct MaterialParameter
{
	FixedString Parameter;
	FixedString UniformName;
#if !defined(OSI_EOCAPP)
	FixedString GroupName;
#endif
	uint16_t ShaderFlags;
	bool Enabled;
};

template <class T>
struct MaterialParameterWithValue : public MaterialParameter
{
	T Value;
};

struct MaterialVector3
{
	glm::vec3 Value;
	bool IsColor;
};

struct alignas(16) MaterialVector4
{
	glm::vec4 Value;
	bool IsColor;
	uint8_t _Pad[15];
};

struct MaterialTexture2D
{
	FixedString ID;
#if !defined(OSI_EOCAPP)
	bool IgnoreTexelDensity{ false };
#endif
};

struct MaterialSamplerState
{
	uint8_t TextureFilterOverride;
	uint8_t TextureAddressMode;
};

template <class T>
struct MaterialParameterSet : public ObjectSet<MaterialParameterWithValue<T>>
{
public:
	bool Override(FixedString const& name, T const& value)
	{
		for (auto& param : *this) {
			if (param.Parameter == name) {
				param.Value = value;
				return true;
			}
		}

		return false;
	}
};

struct MaterialParameters : public ProtectedGameObject<MaterialParameters>
{
	void* VMT;
	Material* ParentMaterial;
	AppliedMaterial* ParentAppliedMaterial;
	MaterialParameterSet<TextureMap*> TextureMaps;
	MaterialParameterSet<float> Scalars;
	MaterialParameterSet<glm::vec2> Vector2s;
	MaterialParameterSet<MaterialVector3> Vector3s;
	MaterialParameterSet<MaterialVector4> Vector4s;
	MaterialParameterSet<MaterialTexture2D> Texture2Ds;
	MaterialParameterSet<MaterialSamplerState> SamplerStates;

	bool OverrideParameter(FixedString const& parameter, TextureMap* value);
	bool OverrideParameter(FixedString const& parameter, float value);
	bool OverrideParameter(FixedString const& parameter, glm::vec2 const& value);
	bool OverrideParameter(FixedString const& parameter, MaterialVector3 const& value);
	bool OverrideParameter(FixedString const& parameter, MaterialVector4 const& value);
	bool OverrideParameter(FixedString const& parameter, MaterialTexture2D const& value);
	bool OverrideParameter(FixedString const& parameter, MaterialSamplerState const& value);
};

struct AppliedMaterial : public ProtectedGameObject<AppliedMaterial>
{
	Material* Material;
	RenderableObject* Renderable;
	bool HasDebugMaterial;
	int DefaultShaderType;
	int ForwardShaderType;
	int ShadowShaderType;
	int DepthShaderType;
	ComponentHandle DefaultShaderHandle;
	ComponentHandle ForwardShaderHandle;
	ComponentHandle ShadowShaderHandle;
	ComponentHandle DepthShaderHandle;
	ComponentHandle DefaultVertexFormatHandle;
	ComponentHandle ForwardVertexFormatHandle;
	ComponentHandle ShadowVertexFormatHandle;
	ComponentHandle DepthVertexFormatHandle;
	uint8_t BlendState;
	MaterialParameters MaterialParameters;
	float OverlayOffset;
	bool IsOverlay;
	bool ForceForwardShading;
	bool ForcePostRefractionAlpha;
};

struct ShaderDesc
{
	FixedString VSHash;
	FixedString PSHash;
	uint16_t Flags;
};

struct Material : public ProtectedGameObject<Material>
{
	void* VMT;
	FixedString MaterialID;
	std::array<ShaderDesc, 12> ShaderDescs;
	std::array<Path, 12> ShaderPaths;
	int ShadingModel;
	int ForwardLightingMode;
	uint8_t BlendState;
	uint8_t RasterizerState;
	uint8_t DepthState;
	int StencilRef;
	int UVCount;
	uint16_t Flags;
	uint8_t MaterialPassHint;
	int MaterialType;
	MaterialParameters MaterialParameters;
#if !defined(OSI_EOCAPP)
	CRITICAL_SECTION CriticalSection;
	ObjectSet<AppliedMaterial> AppliedMaterials;
#endif
};

struct MaterialInfo
{
	void* VMT;
	ComponentHandle GameObjectHandle;
	FixedString MaterialUUID;
	float OverlayOffset;
	StatusMaterialApplyFlags ApplyFlags;
	MaterialInfoFlags Flags;
};

struct OverlayMaterialRequest
{
	MaterialInfo** pOutputMaterialInfo;
	bool* pCompleted;
	float FadeValue;
	ComponentHandle GameObjectHandle;
	FixedString MaterialUUID;
	float OverlayOffset;
	StatusMaterialApplyFlags ApplyFlags;
	MaterialInfoFlags Flags;
};

END_SE()


BEGIN_NS(ecl)

struct MaterialInfo : public dse::MaterialInfo
{
	RefMap<FixedString, float>* ScalarParamaters;
	RefMap<FixedString, glm::vec2>* Vec2Parameters;
	RefMap<FixedString, glm::vec3>* Vec3Parameters;
	RefMap<FixedString, glm::vec4>* Vec4Parameters;
	uint8_t RefCount;
};

struct FadingMaterialInfo : public MaterialInfo
{
	FixedString FadeParameter;
	float FadeValue;
	float InitialFadeValue;
	uint32_t StateFlags;
	uint8_t RequestFlags;
};

struct MaterialManager : public ProtectedGameObject<MaterialManager>
{
	using RemoveMaterialProc = void (MaterialManager* self, uint64_t gameObjectHandle, FixedString const& materialUuid);
	using AddOverlayMaterialProc = MaterialInfo* (MaterialManager* self, uint64_t gameObjectHandle, FixedString const& materialUuid,
		StatusMaterialApplyFlags applyFlags, bool applyNormalMap, bool force, float overlayOffset);
	using AddFadingOverlayMaterialProc = FadingMaterialInfo* (MaterialManager* self, uint64_t gameObjectHandle, FixedString const& materialUuid,
		FixedString const& fadeParameter, float fadeValue,
		StatusMaterialApplyFlags applyFlags, bool applyNormalMap, bool startWithFullFade, bool force, float overlayOffset);
	using AddReplacementMaterialProc = MaterialInfo* (MaterialManager* self, uint64_t gameObjectHandle, FixedString const& materialUuid,
		StatusMaterialApplyFlags applyFlags, bool applyNormalMap, bool force);

	RefMap<ComponentHandle, ObjectSet<FadingMaterialInfo*>*> FadingMaterials;
	RefMap<ComponentHandle, ObjectSet<MaterialInfo*>*> Materials;
	CRITICAL_SECTION CriticalSection;
	ObjectSet<OverlayMaterialRequest> OverlayMaterialRequests;
};

END_NS()
