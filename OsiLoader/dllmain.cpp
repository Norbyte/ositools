#include "stdafx.h"

#include "DxgiWrapper.h"
#include "HttpFetcher.h"
#include "ErrorUtils.h"

#include <Shlwapi.h>
#include <Shlobj.h>

#include <vector>
#include <thread>
#include <string>
#include <iostream>
#include <fstream>

HRESULT UnzipToFolder(PCWSTR pszZipFile, PCWSTR pszDestFolder);

#define UPDATER_HOST L"nb-stor.s3.eu-central-1.amazonaws.com"
#define UPDATER_PATH L"/dos/OsiExtender/Latest.zip"


class OsiLoader
{
public:
	void Launch()
	{
		UpdatePaths();
		std::string reason;
		bool updated = TryToUpdate(reason);

		if (!updated && !ExtenderDLLExists()) {
			auto msg = FromUTF8("Osiris Extender initialization failed:<br>" + reason);
			gErrorUtils->ShowError(msg.c_str());
			return;
		}

		HMODULE handle = LoadLibraryW(dllPath_.c_str());
		if (handle == NULL) {
			gErrorUtils->ShowError(L"Osiris Extender initialization failed:<br>Unable to load extender DLL.");
		}
	}

	bool TryToUpdate(std::string & reason)
	{
		HttpFetcher fetcher(UPDATER_HOST);

		std::string etag;
		if (!fetcher.FetchETag(UPDATER_PATH, etag)) {
			reason = "Something went wrong while checking for updates. Please make sure you're connected to the internet and try again";
			return false;
		}

		std::string currentETag = ReadETag();
		if (currentETag == etag) {
			return true;
		}

		std::vector<uint8_t> response;
		if (!fetcher.Fetch(UPDATER_PATH, response)) {
			reason = "Something went wrong while downloading updates. Please make sure you're connected to the internet and try again";
			return false;
		}

		auto zipPath = extensionPath_ + L"\\Update.zip";
		SaveFile(zipPath, response);
		HRESULT hr = UnzipToFolder(zipPath.c_str(), extensionPath_.c_str());
		if (SUCCEEDED(hr)) {
			SaveETag(etag);
			return true;
		} else {
			reason = "Unable to extract update package.";
			return false;
		}
	}

private:
	std::wstring appDataPath_;
	std::wstring extensionPath_;
	std::wstring dllPath_;
	bool isGame_{ false };

	bool ExtenderDLLExists()
	{
		return PathFileExists(dllPath_.c_str());
	}

	std::string ReadETag()
	{
		if (!ExtenderDLLExists()) {
			// Force update if extension DLL is missing
			return "";
		}

		auto etagPath = extensionPath_ + L"\\.etag";
		std::ifstream f(etagPath, std::ios::binary | std::ios::in);
		if (!f.good()) {
			return "";
		}

		f.seekg(0, std::ios::end);
		auto size = f.tellg();
		f.seekg(0, std::ios::beg);

		std::string s;
		s.resize(size);
		f.read(s.data(), size);
		return s;
	}

	void SaveETag(std::string const & etag)
	{
		auto etagPath = extensionPath_ + L"\\.etag";
		std::ofstream f(etagPath, std::ios::binary | std::ios::out);
		if (!f.good()) {
			return;
		}

		f.write(etag.c_str(), etag.size());
	}

	void SaveFile(std::wstring const & path, std::vector<uint8_t> const & body)
	{
		std::ofstream f(path, std::ios::binary | std::ios::out);
		if (!f.good()) {
			return;
		}

		f.write(reinterpret_cast<char const *>(body.data()), body.size());
	}

	void UpdatePaths()
	{
		isGame_ = (GetModuleHandleW(L"EoCApp.exe") != NULL);

		TCHAR appDataPath[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appDataPath)))
		{
			appDataPath_ = appDataPath;
		}

		extensionPath_ = appDataPath_ + L"\\OsirisExtender";
		if (!PathFileExists(extensionPath_.c_str())) {
			CreateDirectory(extensionPath_.c_str(), NULL);
		}

		if (isGame_) {
			dllPath_ = extensionPath_ + L"\\OsiExtenderEoCApp.dll";
		} else {
			dllPath_ = extensionPath_ + L"\\OsiExtenderEoCPlugin.dll";
		}
	}
};

bool ShouldLoad()
{
	return GetModuleHandleW(L"EoCApp.exe") != NULL
		|| GetModuleHandleW(L"DivinityEngine2.exe") != NULL;
}

DWORD WINAPI UpdaterThread(LPVOID param)
{
	gErrorUtils = std::make_unique<ErrorUtils>();
	OsiLoader loader;
	loader.Launch();
	return 0;
}

void StartUpdaterThread()
{
	CreateThread(NULL, 0, &UpdaterThread, NULL, 0, NULL);
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (ShouldLoad()) {
			gDxgiWrapper = std::make_unique<DxgiWrapper>();
			StartUpdaterThread();
		}
		break;

	case DLL_PROCESS_DETACH:
		if (gDxgiWrapper) {
			gDxgiWrapper.reset();
		}
		break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

