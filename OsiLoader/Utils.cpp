#include "stdafx.h"
#include "ErrorUtils.h"

#include <Shellapi.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <Shldisp.h>
#include <atlbase.h>
#include <psapi.h>

uint8_t const * AsmCallToAbsoluteAddress(uint8_t const * call)
{
	if (call[0] != 0xE8) {
		return nullptr;
	}

	int32_t rel = *(int32_t const *)(call + 1);
	return call + rel + 5;
}

uint8_t const * AsmLeaToAbsoluteAddress(uint8_t const * lea)
{
	if ((lea[0] != 0x48 && lea[0] != 0x4C) || (lea[1] != 0x8D && lea[1] != 0x8B)) {
		return nullptr;
	}

	int32_t rel = *(int32_t const *)(lea + 3);
	return lea + rel + 7;
}

std::unique_ptr<ErrorUtils> gErrorUtils;

ErrorUtils::ErrorUtils()
{
	if (FindModule()) {
		FindErrorFuncs();
	}
}

void ErrorUtils::ShowError(wchar_t const * msg)
{
	if (!ShowErrorDialog(msg)) {
		MessageBoxW(NULL, msg, L"Osiris Loader Error", MB_OK | MB_ICONERROR);
	}
}

bool ErrorUtils::ShowErrorDialog(wchar_t const * msg)
{
	if (EoCClient == nullptr
		|| EoCClientHandleError == nullptr) {
		return false;
	}

	unsigned retries{ 0 };
	while (!CanShowError() && retries < 600) {
		Sleep(100);
		retries++;
	}

	if (retries >= 300) {
		return false;
	}

	STDWString str;
	str.Size = wcslen(msg);
	str.Capacity = 0xfff; // Used to bypass 7-character inline buffer check
	str.BufPtr = const_cast<wchar_t *>(msg);
	EoCClientHandleError(*EoCClient, &str, false, &str);
	return true;
}

bool ErrorUtils::CanShowError()
{
	if (EoCClient == nullptr
		|| *EoCClient == nullptr
		|| (*EoCClient)->State == nullptr
		|| *(*EoCClient)->State == nullptr
		|| EoCClientHandleError == nullptr) {
		return false;
	}

	auto state = (*(*EoCClient)->State)->State;
	return state == 19
		|| state == 17
		|| state == 28
		|| state == 7
		|| state == 30;
}

bool ErrorUtils::FindModule()
{
	eocApp_ = GetModuleHandleW(L"EoCApp.exe");
	if (eocApp_ == NULL) {
		return false;
	}

	MODULEINFO moduleInfo;
	if (!GetModuleInformation(GetCurrentProcess(), eocApp_, &moduleInfo, sizeof(moduleInfo))) {
		return false;
	}

	moduleStart_ = (uint8_t const *)moduleInfo.lpBaseOfDll;
	moduleSize_ = moduleInfo.SizeOfImage;
	return true;
}

void ErrorUtils::FindErrorFuncs()
{
	uint8_t const fragment1[] = { 
		0x4C, 0x8D, 0x8D, 0x08, 0x04, 0x00, 0x00, // lea     r9, [rbp+8F8h+var_4F0]
		0x41, 0xB0, 0x01, // mov     r8b, 1
		0x48, 0x8D, 0x95, 0xE8, 0x03, 0x00, 0x00, // lea     rdx, [rbp+8F8h+var_510]
		0xE8 // call    ecl__EocClient__HandleError
	};

	uint8_t const fragment2[] = {
		0x48, 0x8D, 0x8D, 0xE8, 0x03, 0x00, 0x00 // lea     rdx, [rbp+8F8h+var_510]
	};

	auto moduleEnd = moduleStart_ + moduleSize_;
	for (auto p = moduleStart_; p < moduleEnd; p++) {
		if (*p == 0x48 && p[1] == 0x8B && p[2] == 0x0D
			&& memcmp(p + 7, fragment1, sizeof(fragment1)) == 0
			&& memcmp(p + 29, fragment2, sizeof(fragment2)) == 0) {
			EoCClient = (eclEoCClient **)AsmLeaToAbsoluteAddress(p);
			EoCClientHandleError = (EoCClient__HandleError)AsmCallToAbsoluteAddress(p + 24);
		}
	}
}


[[noreturn]]
void Fail(char const * reason)
{
#if defined(_DEBUG)
	DebugBreak();
#endif
	MessageBoxA(NULL, reason, "Osiris Loader Error", MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 1);
}


std::string ToUTF8(std::wstring const & s)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0, NULL, NULL);
	std::string converted;
	converted.resize(size);
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), converted.data(), (int)converted.size(), NULL, NULL);
	return converted;
}


std::wstring FromUTF8(std::string const & s)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
	std::wstring converted;
	converted.resize(size);
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), converted.data(), (int)converted.size());
	return converted;
}


//
// Helper class for exception-safe scope-based 
// CoInitialize/CoUninitialize calls.
//
class CComScopedInit
{
public:

	CComScopedInit()
	{
		HRESULT hr = ::CoInitialize(NULL);
		if (FAILED(hr))
			AtlThrow(hr);
	}

	~CComScopedInit()
	{
		::CoUninitialize();
	}


	// Ban copy
private:
	CComScopedInit(const CComScopedInit&);
	CComScopedInit& operator=(const CComScopedInit&);
};


//
// Unzip a zip file to the specified folder.
//
HRESULT UnzipToFolder(PCWSTR pszZipFile, PCWSTR pszDestFolder)
{
	CComScopedInit comInit;

	HRESULT hr;

	CComPtr<IShellDispatch> spISD;
	hr = spISD.CoCreateInstance(CLSID_Shell);
	if (FAILED(hr))
		return hr;

	CComVariant vtZipFile(pszZipFile);
	CComPtr<Folder> spZipFile;
	hr = spISD->NameSpace(vtZipFile, &spZipFile);
	if (FAILED(hr))
		return hr;

	CComVariant vtDestFolder(pszDestFolder);
	CComPtr<Folder> spDestination;
	hr = spISD->NameSpace(vtDestFolder, &spDestination);
	if (FAILED(hr))
		return hr;
	if (!spDestination)
		return E_POINTER;

	CComPtr<FolderItems> spFilesInside;
	hr = spZipFile->Items(&spFilesInside);
	if (FAILED(hr))
		return hr;

	CComPtr<IDispatch> spDispItem;
	hr = spFilesInside.QueryInterface(&spDispItem);
	if (FAILED(hr))
		return hr;

	CComVariant vtItem(spDispItem);
	CComVariant vtOptions(FOF_NO_UI);
	hr = spDestination->CopyHere(vtItem, vtOptions);
	if (FAILED(hr))
		return hr;

	return S_OK;
}
