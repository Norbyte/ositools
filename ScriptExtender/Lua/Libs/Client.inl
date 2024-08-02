#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>Client</lua_module>
BEGIN_NS(ecl::lua::client)

std::optional<GameState> GetGameState()
{
	return GetStaticSymbols().GetClientState();
}

ModManager* GetModManager()
{
	return GetModManagerClient();
}

/// <summary>
/// Updates shroud data for a cell in the specified position.
/// </summary>
void UpdateShroud(float x, float y, ShroudType layer, int value)
{
	if (value < 0 || value > 255) {
		OsiError("Can only set shroud cell values between 0 and 255");
		return;
	}

	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (!level || !level->ShroudManager || !level->ShroudManager->ShroudData) {
		OsiError("No ShroudManager for current level?");
		return;
	}

	level->ShroudManager->ShroudData->SetByteAtPos(layer, x, y, (uint8_t)value);
}

rf::CameraManager* GetCameraManager()
{
	return GetStaticSymbols().GetCameraManager();
}

ecl::GameControl* GetGameControl()
{
	return *GetStaticSymbols().ecl_GameControl;
}

void RegisterClientLib()
{
	DECLARE_MODULE(Client, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetGameState)
	MODULE_FUNCTION(GetModManager)
	MODULE_FUNCTION(UpdateShroud)
	MODULE_FUNCTION(GetCameraManager)
	MODULE_FUNCTION(GetGameControl)
	END_MODULE()
}

END_NS()
