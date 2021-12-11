#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

struct MaterialParameter
{
	FixedString Parameter;
	FixedString UniformName;
	FixedString GroupName;
	uint16_t ShaderFlags;
	bool Enabled;
};

template <class T>
struct MaterialParameterWithValue : public MaterialParameter
{
	MaterialParameter Name;
	T Value;
};

struct MaterialParameters : public ProtectedGameObject<MaterialParameters>
{
	void* VMT;
	Material* ParentMaterial;
	AppliedMaterial* ParentAppliedMaterial;
	ObjectSet<MaterialParameterWithValue<void*>> TextureMaps;
	ObjectSet<MaterialParameterWithValue<float>> Scalars;
	ObjectSet<MaterialParameterWithValue<glm::vec2>> Vector2s;
	ObjectSet<MaterialParameterWithValue<glm::vec3>> Vector3s;
	ObjectSet<MaterialParameterWithValue<glm::vec4>> Vector4s;
	ObjectSet<MaterialParameterWithValue<void*>> Texture2Ds;
	ObjectSet<MaterialParameterWithValue<void*>> SamplerStates;
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
	ShaderDesc ShaderDescs[12];
	Path ShaderPaths[12];
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
	CRITICAL_SECTION CriticalSection;
	ObjectSet<AppliedMaterial> AppliedMaterials;
};



END_SE()
