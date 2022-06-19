#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(ecl::lua::client)

std::optional<GameState> GetGameState()
{
	return GetStaticSymbols().GetClientState();
}

ModManager* GetModManager()
{
	return GetModManagerClient();
}

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

void RegisterClientLib()
{
	DECLARE_MODULE(Client, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetGameState)
	MODULE_FUNCTION(GetModManager)
	MODULE_FUNCTION(UpdateShroud)
	END_MODULE()
}

END_NS()
