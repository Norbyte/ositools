#pragma once

#include <GameDefinitions/BaseTypes.h>

namespace dse::script {

	std::optional<STDString> GetPathForExternalIo(std::string_view scriptPath);
	std::optional<STDString> LoadExternalFile(std::string_view path);
	bool SaveExternalFile(std::string_view path, std::string_view contents);

	bool GetTranslatedString(char const* handle, STDWString& translated);
	bool GetTranslatedStringFromKey(FixedString const& key, TranslatedString& translated);
	bool CreateTranslatedStringKey(FixedString const& key, FixedString const& handle);
	bool CreateTranslatedString(FixedString const& handle, STDWString const& string);
}