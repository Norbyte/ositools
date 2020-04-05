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

}
