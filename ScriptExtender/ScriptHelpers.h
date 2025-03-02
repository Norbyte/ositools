#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Components/Item.h>

namespace dse::script {

	std::optional<STDWString> GetPathForExternalIo(std::string_view scriptPath, PathRootType root);
	ObjectSet<FixedString> EnumerateDirectory(std::string_view directoryPath, PathRootType root);
	bool IsFile(std::string_view path, PathRootType root);
	bool IsDirectory(std::string_view path, PathRootType root);
	std::optional<STDString> LoadExternalFile(std::string_view path, PathRootType root);
	bool SaveExternalFile(std::string_view path, PathRootType root, std::string_view contents);
	bool CreateGameStorageDirectories(std::string_view path);

	bool GetTranslatedString(char const* handle, STDWString& translated);
	bool GetTranslatedString(char const* handle, char const* fallback, TranslatedString& translated);
	bool GetTranslatedStringFromKey(FixedString const& key, TranslatedString& translated);
	bool CreateTranslatedStringKey(FixedString const& key, FixedString const& handle);
	bool CreateTranslatedString(FixedString const& handle, STDWString const& string);

	bool CreateItemDefinition(char const* templateGuid, ObjectSet<eoc::ItemDefinition>& definition);
	bool ParseItem(esv::Item* item, ObjectSet<eoc::ItemDefinition>& definition, bool recursive);
}
