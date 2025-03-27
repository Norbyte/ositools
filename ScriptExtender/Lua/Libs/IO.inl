#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <ScriptHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>IO</lua_module>
BEGIN_NS(lua::io)

ObjectSet<FixedString> Enumerate(std::optional<STDString> path)
{
	return script::EnumerateDirectory(path.value_or(""), PathRootType::GameStorage);
}

bool IsFile(char const* path)
{
	return script::IsFile(path, PathRootType::GameStorage);
}

bool IsDirectory(char const* path)
{
	return script::IsDirectory(path, PathRootType::GameStorage);
}

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

bool SaveFile(char const* path, char const* contents, std::optional<int> contentLen)
{
	return script::SaveExternalFile(path, PathRootType::GameStorage, contentLen ? std::string_view(contents, contentLen.value()) : contents);
}

void AddPathOverride(char const* path, char const* overridePath)
{
	gExtender->AddPathOverride(path, overridePath);
}

std::optional<STDString> GetPathOverride(char const* path)
{
	return gExtender->GetPathOverride(path);
}

void RegisterIOLib()
{
	DECLARE_MODULE(IO, Both)
	BEGIN_MODULE()
	MODULE_FUNCTION(Enumerate)
	MODULE_FUNCTION(IsFile)
	MODULE_FUNCTION(IsDirectory)
	MODULE_FUNCTION(LoadFile)
	MODULE_FUNCTION(SaveFile)
	MODULE_FUNCTION(AddPathOverride)
	MODULE_FUNCTION(GetPathOverride)
	END_MODULE()
}

END_NS()
