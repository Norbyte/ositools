#include "stdafx.h"
#include "ScriptHelpers.h"
#include "OsirisProxy.h"
#include <fstream>

namespace dse::script {

#define SAFE_PATH_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_."

std::optional<STDString> GetPathForExternalIo(std::string_view scriptPath)
{
	STDString path(scriptPath);

	if (path.find_first_not_of(SAFE_PATH_CHARS) != STDString::npos
		|| path.find("..") != STDString::npos) {
		OsiError("Illegal file name for external file access: '" << path << "'");
		return {};
	}

	auto storageRoot = GetStaticSymbols().ToPath("/Osiris Data", PathRootType::GameStorage);
	if (storageRoot.empty()) {
		OsiErrorS("Could not fetch game storage path");
		return {};
	}

	auto storageRootWstr = FromUTF8(storageRoot);
	BOOL created = CreateDirectory(storageRootWstr.c_str(), NULL);
	if (created == FALSE) {
		DWORD lastError = GetLastError();
		if (lastError != ERROR_ALREADY_EXISTS) {
			OsiError("Could not create storage root directory: " << storageRoot);
			return {};
		}
	}

	return storageRoot + "/" + path;
}

std::optional<STDString> LoadExternalFile(std::string_view path)
{
	auto absolutePath = GetPathForExternalIo(path);
	if (!absolutePath) return {};

	std::ifstream f(absolutePath->c_str(), std::ios::in | std::ios::binary);
	if (!f.good()) {
		OsiError("Could not open file: '" << path << "'");
		return {};
	}

	STDString body;
	f.seekg(0, std::ios::end);
	body.resize(f.tellg());
	f.seekg(0, std::ios::beg);
	f.read(body.data(), body.size());

	return body;
}

bool SaveExternalFile(std::string_view path, std::string_view contents)
{
	auto absolutePath = GetPathForExternalIo(path);
	if (!absolutePath) return false;

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
	auto getRepoInstance = GetStaticSymbols().TranslatedStringRepository__GetInstance;
	auto repoInstance = GetStaticSymbols().TranslatedStringRepository__Instance;
	auto getter = GetStaticSymbols().TranslatedStringRepository__Get;

	TranslatedStringRepository* repo{ nullptr };
	if (getRepoInstance) {
		repo = getRepoInstance();
	} else if (repoInstance) {
		repo = *repoInstance;
	}

	if (repo == nullptr || getter == nullptr) {
		OsiError("TranslatedStringRepository functions not mapped!");
		return false;
	}

	auto key = ToFixedString(handle);
	if (!key) {
		return false;
	}

	RuntimeStringHandle runtimeString;
	runtimeString.Handle = key;
	uint64_t length{ 0 };
	auto translatedString = getter(repo, &runtimeString, &length, 0, 0, true);

	if (translatedString) {
		translated = *translatedString;
		return true;
	} else {
		return false;
	}
}

bool GetTranslatedStringFromKey(FixedString const& key, TranslatedString& translated)
{
	auto getKeyMgrInstance = GetStaticSymbols().TranslatedStringKeyManager__GetInstance;
	auto keyMgrInstance = GetStaticSymbols().TranslatedStringKeyManager__Instance;
	auto getter = GetStaticSymbols().TranslatedStringKeyManager__GetTranlatedStringFromKey;

	TranslatedStringKeyManager* keyMgr{ nullptr };
	if (getKeyMgrInstance) {
		keyMgr = getKeyMgrInstance();
	} else if (keyMgrInstance) {
		keyMgr = *keyMgrInstance;
	}

	if (keyMgr == nullptr || getter == nullptr) {
		OsiError("TranslatedStringKeyManager functions not mapped!");
		return false;
	}

	if (getter(keyMgr, translated, key, false)) {
		return GetTranslatedString(translated.Str1.Handle.Str, translated.Str1.WStr);
	} else {
		return false;
	}
}

}
