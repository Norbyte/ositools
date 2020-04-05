#pragma once

#include <GameDefinitions/BaseTypes.h>

namespace dse::script {

	std::optional<STDString> GetPathForExternalIo(std::string_view scriptPath);
	std::optional<STDString> LoadExternalFile(std::string_view path);
	bool SaveExternalFile(std::string_view path, std::string_view contents);

}