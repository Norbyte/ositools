#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Ai.h>

BEGIN_NS(eoc)

template <class T>
struct VisionHashSet
{
	struct Bucket
	{
		Bucket* Next;
		T Value;
	};

	uint32_t HashSize;
	Bucket** Buckets;
	ObjectSet<uint32_t> BucketIndices;
	uint32_t NumItems;
};


struct Visibility
{
	AiGrid* AiGrid;
	VisionGrid* VisionGrid;
	bool IsActivePlayer;
	VisionHashSet<glm::ivec2> VisiblePositions;
	float SightRange;
	glm::vec3 Translate;
	VisionHashSet<glm::ivec2> LastVisiblePositions;
	float LastSightRange;
	glm::vec3 LastTranslate;
	bool Invisible;
	ObjectSet<void*> VisibilitySubJobs;
};


struct VisionCellChange
{
	int TileX;
	int TileZ;
	bool WasAdded;
	SurfaceType SurfaceType;
	float Height;
	float MaxHeight;
	VisionFlags VisionFlags;
};


struct VisionTile
{
	float16 TerrainHeight;
	float16 MaxHeight;
};


struct VisionTiles
{
	void* LevelManager;
	int Width;
	int Height;
	void* field_10;
	VisionTile* BufferStart;
	VisionTile* BufferEnd;
};


struct VisionObjectData
{
	ComponentHandle Ai;
	VisionFlags VisionFlags;
	float16 MaxY;
	float16 MinY;
};


struct VisionGrid : public ProtectedGameObject<VisionGrid>
{
	struct RaycastHit
	{
		VisionObjectData Hit;
		glm::vec3 HitPosition;
	};

	using RaycastProc = VisionFlags* (VisionGrid* self, VisionFlags& losFlags, glm::vec3 const& fromPos,
		glm::vec3 const& toPos, VisionFlags checkFlags, RaycastHit* pHit);
#if defined(OSI_EOCAPP)
	using Check3DLineProc = VisionFlags* (VisionFlags& losFlags, glm::vec3 const& fromPos,
		Ai* ai, VisionFlags const& checkFlags, glm::vec3 const* bboxBias);
#else
	using Check3DLineProc = VisionFlags * (VisionGrid* self, VisionFlags& losFlags, glm::vec3 const& fromPos,
		Ai* ai, VisionFlags checkFlags, glm::vec3 const* bboxBias);
#endif

	void* VMT;
	void* WorkerThreadJobData;
	DataGrid DataGrid;
	void* LevelManager;
	VisionTiles* Tiles;
	int field_40;
	ObjectSet<Ai*> Ais;
	RefMap<uint32_t, ObjectSet<VisionObjectData>> Objects;
	VisionGridVisual* Visual;
	ObjectSet<eoc::Visibility*> Visibilities;
	uint8_t VisibilityUpdateTick;
	ObjectSet<VisionCellChange> SurfaceCellChanges;
	bool NeedsUpdate;

	VisionFlags Raycast(glm::vec3 const& fromPos, glm::vec3 const& toPos, VisionFlags checkFlags, RaycastHit* pHit);
	VisionFlags Check3DLine(glm::vec3 const& fromPos, Ai* ai, VisionFlags checkFlags, glm::vec3 const* bboxBias);

	VisionFlags LuaRaycastToPosition(glm::vec3 fromPos, glm::vec3 toPos, VisionFlags checkFlags);
	VisionFlags LuaRaycastToObject(glm::vec3 fromPos, ProxyParam<Ai> ai, VisionFlags checkFlags, std::optional<glm::vec3> bboxBias);
};

END_NS()
