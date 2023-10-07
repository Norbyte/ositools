#include <stdafx.h>
#include "ScriptHelpers.h"
#include <Extender/ScriptExtender.h>
#include <filesystem>
#include <fstream>

namespace dse::script {

#define SAFE_PATH_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_./ "

bool IsSafeRelativePath(STDString const& path)
{
	if (path.find_first_not_of(SAFE_PATH_CHARS) != STDString::npos
		|| path.find("..") != STDString::npos) {
		OsiError("Illegal file name for external file access: '" << path << "'");
		return false;
	} else {
		return true;
	}
}

std::optional<STDWString> GetPathForExternalIo(std::string_view scriptPath, PathRootType root)
{
	STDString path(scriptPath);

	if (!IsSafeRelativePath(path)) {
		return {};
	}

	auto storageRoot = GetStaticSymbols().ToPath("/Osiris Data", root);
	if (storageRoot.empty()) {
		OsiErrorS("Could not fetch game storage path");
		return {};
	}

	return FromUTF8(storageRoot + "/" + path);
}

ObjectSet<FixedString> EnumerateDirectory(std::string_view directoryPath, PathRootType root)
{
	ObjectSet<FixedString> paths;

	if (root == PathRootType::GameStorage) {
		auto rawAbsolutePath = GetPathForExternalIo(directoryPath, PathRootType::GameStorage);
		if (!rawAbsolutePath) return {};
		auto absolutePath = std::filesystem::path(rawAbsolutePath.value());

		if (!std::filesystem::exists(absolutePath) || !std::filesystem::is_directory(absolutePath)) {
			LuaError("Path does not exist or is not a directory");
			return {};
		}

		try {
			for (const auto& entry : std::filesystem::directory_iterator(absolutePath)) {
				auto entryAbsolutePath = FixedString(entry.path().generic_string());
				auto relativePath = std::filesystem::proximate(entry, absolutePath);

				paths.push_back(FixedString(relativePath.generic_string()));
			}
		}
		catch (std::filesystem::filesystem_error& e) {
			LuaError("Could not read directory: " << e.what());
			return {};
		}
	} else {
		LuaError("Unsupported file context");
	}

	return paths;
}

bool IsFile(std::string_view path, PathRootType root)
{
	if (root == PathRootType::GameStorage) {
		auto absolutePath = GetPathForExternalIo(path, PathRootType::GameStorage);
		if (!absolutePath) return false;

		return std::filesystem::is_regular_file(absolutePath.value());
	} else {
		LuaError("Unsupported file context");
		return false;
	}
}

bool IsDirectory(std::string_view path, PathRootType root)
{
	if (root == PathRootType::GameStorage) {
		auto absolutePath = GetPathForExternalIo(path, PathRootType::GameStorage);
		if (!absolutePath) return false;

		return std::filesystem::is_directory(absolutePath.value());
	}
	else {
		LuaError("Unsupported file context");
		return false;
	}
}

std::optional<STDString> LoadExternalFile(std::string_view path, PathRootType root)
{
	if (!IsSafeRelativePath(STDString(path))) {
		return {};
	}

	if (root == PathRootType::Data) {
		auto reader = GetStaticSymbols().MakeFileReader(path, root);
		if (reader.IsLoaded()) {
			return reader.ToString();
		}
	} else {
		auto absolutePath = GetPathForExternalIo(path, root);
		if (!absolutePath) return {};

		std::ifstream f(absolutePath->c_str(), std::ios::in | std::ios::binary);
		if (f.good()) {
			STDString body;
			f.seekg(0, std::ios::end);
			body.resize(f.tellg());
			f.seekg(0, std::ios::beg);
			f.read(body.data(), body.size());
			return body;
		}
	}

	return {};
}

bool SaveExternalFile(std::string_view path, PathRootType root, std::string_view contents)
{
	auto absolutePath = GetPathForExternalIo(path, root);
	if (!absolutePath) return false;

	auto dirEnd = absolutePath->find_last_of('/');
	if (dirEnd == std::string::npos) return false;

	auto storageDir = absolutePath->substr(0, dirEnd);
	if (!std::filesystem::exists(storageDir) || !std::filesystem::is_directory(storageDir)) {
		bool created = std::filesystem::create_directories(storageDir);
		if (!created) {
			OsiError("Could not create storage directory: " << ToUTF8(storageDir));
			return {};
		}
	}

	std::ofstream f(absolutePath->c_str(), std::ios::out | std::ios::binary);
	if (!f.good()) {
		OsiError("Could not open file: '" << path << "'");
		return false;
	}

	f.write(contents.data(), contents.length());
	return true;
}

bool GetTranslatedString(char const* handle, STDWString& translated)
{
	auto repo = GetStaticSymbols().GetTranslatedStringRepository();
	auto getter = GetStaticSymbols().TranslatedStringRepository__Get;

	if (repo == nullptr || getter == nullptr) {
		OsiError("TranslatedStringRepository functions not mapped!");
		return false;
	}

	FixedString key(handle);
	if (!key) {
		return false;
	}

	RuntimeStringHandle runtimeString;
	runtimeString.Handle = key;
	uint64_t length{ 0 };
	auto translatedString = getter(repo, &runtimeString, &length, 0, 0, true);

	if (translatedString && !translatedString->empty()) {
		translated = *translatedString;
		return true;
	} else {
		return false;
	}
}

bool GetTranslatedString(char const* handle, char const* fallback, TranslatedString& translated)
{
	STDWString ts;
	if (GetTranslatedString(handle, ts)) {
		translated.Handle.Handle = FixedString(handle);
		translated.Handle.ReferenceString = ts;
		return true;
	} else {
		translated.Handle.Handle = GFS.strNullStringHandle;
		translated.Handle.ReferenceString = FromUTF8(fallback);
		return false;
	}
}

bool GetTranslatedStringFromKey(FixedString const& key, TranslatedString& translated)
{
	auto keyMgr = GetStaticSymbols().GetTranslatedStringKeyManager();
	auto getter = GetStaticSymbols().TranslatedStringKeyManager__GetTranlatedStringFromKey;

	if (keyMgr == nullptr || getter == nullptr) {
		OsiError("TranslatedStringKeyManager functions not mapped!");
		return false;
	}

	bool ok = getter(keyMgr, translated, key, false);

	MARK_EXTERNAL_WSTR(translated.Handle.ReferenceString);
	MARK_EXTERNAL_STR(translated.Handle.Str);
	MARK_EXTERNAL_WSTR(translated.ArgumentString.ReferenceString);
	MARK_EXTERNAL_STR(translated.ArgumentString.Str);

	if (ok) {
		// If the string key is not localized, use the content from the localization LSB.
		if (translated.Handle.Handle != GFS.strNullStringHandle) {
			return GetTranslatedString(translated.Handle.Handle.GetString(), translated.Handle.ReferenceString);
		} else {
			return !translated.Handle.ReferenceString.empty();
		}
	} else {
		return false;
	}
}

bool CreateTranslatedStringKey(FixedString const& key, FixedString const& handle)
{
	auto keyMgr = GetStaticSymbols().GetTranslatedStringKeyManager();
	if (keyMgr == nullptr) {
		OsiError("TranslatedStringKeyManager not mapped!");
		return false;
	}

	auto mapping = keyMgr->StringKeys.try_get_ptr(key);
	if (mapping) {
		mapping->Handle.Handle = handle;
	} else {
		TranslatedString str;
		str.Handle.Handle = handle;
		keyMgr->StringKeys.insert(key, str);
	}

	return true;
}

bool CreateTranslatedString(FixedString const& handle, STDWString const& string)
{
	auto repo = GetStaticSymbols().GetTranslatedStringRepository();
	auto getter = GetStaticSymbols().TranslatedStringRepository__Get;

	if (repo == nullptr || getter == nullptr) {
		OsiError("TranslatedStringRepository functions not mapped!");
		return false;
	}

	EnterCriticalSection(&repo->CriticalSection);
	auto str = repo->TranslatedStringOverrides[0]->try_get_ptr(handle);
	if (str) {
		**str = string;
	} else {
		auto s = GameAlloc<STDWString>(string);
		repo->TranslatedStringOverrides[0]->insert(handle, s);
	}

	LeaveCriticalSection(&repo->CriticalSection);

	return true;
}

bool CreateItemDefinition(char const* templateGuid, ObjectSet<eoc::ItemDefinition>& definition)
{
	auto templateGuidFs = NameGuidToFixedString(templateGuid);
	if (!templateGuidFs) {
		OsiError("Invalid template GUID passed to ItemConstructBegin: " << templateGuid);
		return false;
	}

	definition.resize(1);

	eoc::ItemDefinition& item = definition[0];
	new (&item) eoc::ItemDefinition();
	item.UUID = GFS.strEmpty;
	item.RootTemplate = templateGuidFs;
	item.OriginalRootTemplate = templateGuidFs;
	item.WorldRot = glm::mat3x3(1.0f);
	item.FS4 = GFS.strEmpty;
	item.ItemType = GFS.strEmpty;
	item.GenerationStatsId = GFS.strEmpty;
	item.GenerationItemType = GFS.strEmpty;
	item.Key = GFS.strEmpty;
	item.StatsEntryName = GFS.strEmpty;
	item.Skills = GFS.strEmpty;
	return true;
}

bool ParseItem(esv::Item* item, ObjectSet<eoc::ItemDefinition>& definition, bool recursive)
{
	auto parseItem = GetStaticSymbols().esv__ParseItem;
	if (parseItem == nullptr) {
		OsiErrorS("esv::ParseItem not found!");
		return false;
	}

	definition.clear();
	parseItem(item, &definition, false, false);

	if (!definition.size() || (!recursive && definition.size() != 1)) {
		OsiError("Something went wrong during item parsing. Item set size: " << definition.size());
		return false;
	}

	return true;
}

}
