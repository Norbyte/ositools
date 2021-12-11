#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Render.h>

BEGIN_SE()

struct TerrainPatchDescription
{
	glm::ivec2 Index;
	glm::ivec2 DimensionInVertices;
	glm::ivec2 DimensionInQuads;
	__int64 field_18;
	glm::ivec2 DimensionInWorldUnits;
	__int64 field_28;
	int field_30;
	STDString Path;
	__int64 field_58;
	__int64 field_60;
	__int64 LoadedIndices;
	int NumIndices_M;
	__int64 vertexMap;
	int NumVertices;
	char AllowPicking_M;
	char field_85;
	FixedString field_88;
	int Version;
};

struct TerrainPatch : public RenderableObject
{
	TerrainPatchDescription PatchDesc;
	int DimensionInVertices[2];
	int DimensionInWorldUnits[2];
	int DimensionInQuads[2];
	int Index[2];
	bool IsRenderDataDirty;
	char field_311;
	uint16_t CullFlags;
	char IsEditable;
	char MaterialIndex;
	ObjectSet<void*> Layers; // TerrainLayer
	ObjectSet<void*> Layers2; // TerrainLayer
	FixedString field_358;
	void* PickingData;
};

END_SE()
