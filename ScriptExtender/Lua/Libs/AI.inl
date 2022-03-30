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
