#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <ScriptHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(lua::io)

std::optional<STDString> LoadFile(char const* path, std::optional<FixedString> context)
{
	if (!context || *context == GFS.struser) {
		return script::LoadExternalFile(path, PathRootType::GameStorage);
	} else if (context == GFS.strdata) {
		return script::LoadExternalFile(path, PathRootType::Data);
	} else {
		LuaError("Unknown file loading context: " << context->GetString());
		return {};
	}
}

bool SaveFile(char const* path, char const* contents)
{
	return script::SaveExternalFile(path, PathRootType::GameStorage, contents);
}

void AddPathOverride(char const* path, char const* overridePath)
{
	gExtender->AddPathOverride(path, overridePath);
}

std::optional<STDString> GetPathOverride(char const* path)
{
	return gExtender->GetPathOverride(path);
}

void RegisterIOLib(lua_State* L)
{
	static const luaL_Reg ioLib[] = {
		{"LoadFile", LuaWrapFunction(&LoadFile)},
		{"SaveFile", LuaWrapFunction(&SaveFile)},
		{"AddPathOverride", LuaWrapFunction(&AddPathOverride)},
		{"GetPathOverride", LuaWrapFunction(&GetPathOverride)},
		{0,0}
	};

	RegisterLib(L, "IO", ioLib);
}

END_NS()
