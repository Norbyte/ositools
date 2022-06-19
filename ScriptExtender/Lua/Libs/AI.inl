BEGIN_NS(eoc)

bool AiGrid::SearchForCell(float x, float z, float radius, ESurfaceFlag flags, float bias)
{
	auto search = GetStaticSymbols().eoc__AiGrid__SearchForCell;
	if (!search) {
		OsiError("AiGrid::SearchForCell not mapped!");
		return false;
	}

	return !search(this, x, z, radius, flags, nullptr, bias);
}

UserReturn AiGrid::GetCellInfo(lua_State* L, float x, float z)
{
	using namespace dse::lua;

	auto cell = GetCell(glm::vec2(x, z));
	if (!cell) {
		OsiError("Could not find AiGrid cell at " << x << ";" << z);
		push(L, nullptr);
		return 0;
	}

	auto groundIdx = GetSurfaceIndex(cell, 0);
	auto cloudIdx = GetSurfaceIndex(cell, 1);
	auto meta = GetAiMetaData(cell);

	auto height = cell->Height * 0.25f + DataGrid.OffsetY;

	lua_newtable(L);
	settable(L, "Flags", cell->AiFlags);
	settable(L, "Height", height);

	lua_newtable(L);
	if (meta != nullptr) {
		int32_t aiIdx = 1;
		for (auto ai : meta->Ai) {
			ComponentHandle handle;
			ai->GameObject->GetObjectHandle(handle);
			settable(L, aiIdx++, handle);
		}
	}

	lua_setfield(L, -2, "Objects");

	if (gExtender->GetServer().IsInServerThread()) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (!level || level->AiGrid != this || !level->SurfaceManager) {
			OsiError("Current level not available yet!");
		} else {
			if (groundIdx != AiGridTile::InvalidIndex) {
				auto surface = level->SurfaceManager->Surfaces[groundIdx];
				settable(L, "GroundSurface", surface->MyHandle);
			}

			if (cloudIdx != AiGridTile::InvalidIndex) {
				auto surface = level->SurfaceManager->Surfaces[cloudIdx];
				settable(L, "CloudSurface", surface->MyHandle);
			}
		}
	}

	return 1;
}

std::optional<float> AiGrid::GetHeight(float x, float z)
{
	auto cell = GetCell(glm::vec2(x, z));
	if (!cell) {
		OsiError("Could not find AiGrid cell at " << x << ";" << z);
		return {};
	}

	auto meta = GetAiMetaData(cell);
	return cell->Height * 0.25f + DataGrid.OffsetY;
}

std::optional<uint64_t> AiGrid::GetAiFlags(float x, float z)
{
	auto cell = GetCell(glm::vec2(x, z));
	if (!cell) {
		OsiError("Could not find AiGrid cell at " << x << ";" << z);
		return {};
	}

	return cell->AiFlags;
}

void AiGrid::SetAiFlags(float x, float z, uint64_t aiFlags)
{
	// Only allow updating Walkable/Reachable flags for now
	constexpr uint64_t UpdateFlags = 5;

	auto cell = GetCell(glm::vec2(x, z));
	if (!cell) {
		OsiError("Could not find AiGrid cell at " << x << ";" << z);
		return;
	}

	cell->AiFlags = (aiFlags & UpdateFlags) | (cell->AiFlags & ~UpdateFlags);
}

bool AiGrid::UpdateAiFlagsInRect(float minX, float minZ, float maxX, float maxZ, uint64_t setFlags, uint64_t clearFlags)
{
	// Only allow updating Walkable/Reachable flags for now
	constexpr uint64_t UpdateFlags = 5;

	setFlags &= UpdateFlags;
	clearFlags &= UpdateFlags;

	auto topLeft = GetCell(glm::vec2(minX, minZ));
	if (!topLeft) {
		OsiError("Could not find top left AiGrid cell at " << minX << ";" << minZ);
		return false;
	}

	auto bottomRight = GetCell(glm::vec2(maxX, maxZ));
	if (!bottomRight) {
		OsiError("Could not find bottom right AiGrid cell at " << maxX << ";" << maxZ);
		return false;
	}

	for (auto z = minZ; z <= maxZ; z += DataGrid.GridScale) {
		for (auto x = minX; x <= maxX; x += DataGrid.GridScale) {
			auto cell = GetCell(glm::vec2(x, z));
			if (cell) {
				cell->AiFlags = (cell->AiFlags & ~clearFlags) | setFlags;
			}
		}
	}

	return true;
}

ObjectSet<glm::vec2> AiGrid::FindCellsInRect(float minX, float minZ, float maxX, float maxZ, uint64_t anyFlags, uint64_t allFlags)
{
	ObjectSet<glm::vec2> hits;

	auto topLeft = GetCell(glm::vec2(minX, minZ));
	if (!topLeft) {
		OsiError("Could not find top left AiGrid cell at " << minX << ";" << minZ);
		return hits;
	}

	auto bottomRight = GetCell(glm::vec2(maxX, maxZ));
	if (!bottomRight) {
		OsiError("Could not find bottom right AiGrid cell at " << maxX << ";" << maxZ);
		return hits;
	}

	for (auto z = minZ; z <= maxZ; z += DataGrid.GridScale) {
		for (auto x = minX; x <= maxX; x += DataGrid.GridScale) {
			auto cell = GetCell(glm::vec2(x, z));
			if (cell && (anyFlags == 0 || (cell->AiFlags & anyFlags) != 0) && (cell->AiFlags & allFlags) == allFlags) {
				hits.push_back(glm::vec2{ x, z });
			}
		}
	}

	return hits;
}

void AiGrid::SetHeight(float x, float z, float height)
{
	auto cell = GetCell(glm::vec2(x, z));
	if (!cell) {
		LuaError("Could not find AiGrid cell at " << x << ";" << z);
		return;
	}

	float cellHeight = (height - DataGrid.OffsetY) / 0.25f;
	if (cellHeight <= -32767.0f || cellHeight >= 32767.0f) {
		LuaError("Cell height value out of bounds: " << height);
		return;
	}

	cell->Height = (int16_t)cellHeight;
}

END_NS()
