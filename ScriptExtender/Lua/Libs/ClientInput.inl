#include <Lua/Shared/LuaMethodHelpers.h>
#include <GameDefinitions/Input.h>

BEGIN_SE()

FixedString InputDeviceIdToString(InputDeviceId id)
{
	switch (id) {
	case InputDeviceId::C: return GFS.strC;
	case InputDeviceId::Key: return GFS.strKey;
	case InputDeviceId::Mouse: return GFS.strMouse;
	case InputDeviceId::Touchbar: return GFS.strTouchbar;
	case InputDeviceId::Unknown: 
	default:
		return GFS.strUnknown;
	}
}

InputDeviceId StringToInputDeviceId(FixedString id)
{
	if (id == GFS.strUnknown) return InputDeviceId::Unknown;
	if (id == GFS.strC) return InputDeviceId::C;
	if (id == GFS.strKey) return InputDeviceId::Key;
	if (id == GFS.strMouse) return InputDeviceId::Mouse;
	if (id == GFS.strTouchbar) return InputDeviceId::Touchbar;
	return InputDeviceId::Unknown;
}

END_SE()

/// <lua_module>Input</lua_module>
BEGIN_NS(ecl::lua::input)

bool InjectInput(FixedString deviceId, InputRawType inputId, InputState state, float value1, float value2, std::optional<bool> immediate)
{
	InjectInputData input;
	input.Input.DeviceId = StringToInputDeviceId(deviceId);
	input.Input.InputId = inputId;
	input.Value.State = state;
	input.Value.Value = value1;
	input.Value.Value2 = value2;

	auto inputMgr = GetStaticSymbols().GetInputManager();
	if (immediate && *immediate) {
		auto inject = GetStaticSymbols().ls__InputManager__InjectInput;
		return inject(inputMgr, input);
	} else {
		inputMgr->InputInjects.push_back(input);
		return true;
	}
}

void RegisterInputLib()
{
	DECLARE_MODULE(Input, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(InjectInput)
	END_MODULE()
}

END_NS()
