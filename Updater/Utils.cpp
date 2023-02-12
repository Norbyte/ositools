#include "stdafx.h"
#include "GameHelpers.h"

#include <atlbase.h>
#include <psapi.h>
#include <iostream>
#include <fstream>

BEGIN_SE()

void DebugMsg(char const * msg)
{
	OutputDebugStringA(msg);
	OutputDebugStringA("\r\n");
	std::cout << msg << std::endl;
	std::cout.flush();
}

uint8_t const * AsmCallToAbsoluteAddress(uint8_t const * call)
{
	if (call[0] != 0xE8 && call[0] != 0xE9) {
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

std::unique_ptr<GameHelpers> gGameHelpers;

GameHelpers::GameHelpers()
{
	if (FindModule()) {
		FindErrorFuncs();
		AddVectoredExceptionHandler(1, &ThreadNameCaptureFilter);
	}
}

void GameHelpers::ShowError(char const * msg) const
{
	if (!ShowErrorDialog(msg)) {
		MessageBoxA(NULL, msg, "Script Extender Updater Error", MB_OK | MB_ICONERROR);
	}
}

bool GameHelpers::ShowErrorDialog(char const * msg) const
{
	if (EoCClient == nullptr
		|| EoCClientHandleError == nullptr) {
		return false;
	}

	unsigned retries{ 0 };
	while (!CanShowError() && retries < 3000) {
		Sleep(100);
		retries++;
	}

	if (retries >= 3000) {
		return false;
	}

	ClientHandleError(msg, false);
	return true;
}

void GameHelpers::ClientHandleError(char const * msg, bool exitGame) const
{
	if (EoCClientHandleError == nullptr) return;

	std::string filtered(msg);
	for (auto pos = filtered.find("\r\n"); pos != std::wstring::npos; pos = filtered.find("\r\n")) {
		filtered.replace(filtered.begin() + pos, filtered.begin() + pos + 2, "<br>");
	}

	auto clientMsg = FromUTF8(filtered);
	EoCClientHandleError(*EoCClient, (STDWString*)&clientMsg, exitGame, (STDWString*)&clientMsg);
}

bool GameHelpers::CanShowError() const
{
	auto state = GetState();
	return state
		&& (*state == ecl::GameState::Running
		|| *state == ecl::GameState::Paused
		|| *state == ecl::GameState::GameMasterPause
		|| *state == ecl::GameState::Menu
		|| *state == ecl::GameState::Lobby);
}

std::optional<ecl::GameState> GameHelpers::GetState() const
{
	if (EoCClient == nullptr
		|| *EoCClient == nullptr
		|| (*EoCClient)->GameStateMachine == nullptr
		|| *(*EoCClient)->GameStateMachine == nullptr
		|| EoCClientHandleError == nullptr) {
		return {};
	}

	return (*(*EoCClient)->GameStateMachine)->State;
}

bool GameHelpers::FindModule()
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

void GameHelpers::FindErrorFuncs()
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
			EoCClient = (ecl::EoCClient**)AsmLeaToAbsoluteAddress(p);
			EoCClientHandleError = (ecl::EoCClient::HandleErrorProc*)AsmCallToAbsoluteAddress(p + 24);
		}
	}
}

void GameHelpers::SuspendClientThread() const
{
	auto thread = FindClientThread();
	if (thread != nullptr) {
		auto hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread->ThreadId);
		if (hThread && hThread != INVALID_HANDLE_VALUE) {
			SuspendThread(hThread);
			CloseHandle(hThread);
			// The error handler only displays a status message during the loading screen
			ClientHandleError("Checking for Script Extender updates", false);
		}
	} else {
		DEBUG("Could not suspend client thread (thread not found!)");
	}
}

void GameHelpers::ResumeClientThread() const
{
	auto thread = FindClientThread();
	if (thread != nullptr) {
		auto hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread->ThreadId);
		if (hThread && hThread != INVALID_HANDLE_VALUE) {
			ResumeThread(hThread);
			CloseHandle(hThread);
		}
	} else {
		DEBUG("Could not resume client thread (thread not found!)");
	}
}

GameHelpers::ThreadInfo const * GameHelpers::FindClientThread() const
{
	for (auto const & it : threads_) {
		if (it.Name == "ClientInit" || it.Name == "ClientLoadModule") {
			return &it;
		}
	}

	return nullptr;
}

LONG NTAPI GameHelpers::ThreadNameCaptureFilter(_EXCEPTION_POINTERS *ExceptionInfo)
{
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x406D1388) {
		auto info = reinterpret_cast<THREADNAME_INFO *>(&ExceptionInfo->ExceptionRecord->ExceptionInformation);
		if (info->dwType == 0x1000 && info->dwFlags == 0) {
			ThreadInfo thread;
			thread.ThreadId = info->dwThreadID;
			thread.Name = info->szName;
			gGameHelpers->threads_.push_back(thread);
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

[[noreturn]]
void Fail(char const * reason)
{
#if defined(_DEBUG)
	if (IsDebuggerPresent()) {
		DebugBreak();
	}
#endif
	MessageBoxA(NULL, reason, "Script Extender Updater Error", MB_OK | MB_ICONERROR);
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

bool TryCreateDirectory(std::wstring const& path)
{
	if (!PathFileExistsW(path.c_str())) {
		return CreateDirectoryW(path.c_str(), NULL) == TRUE;
	} else {
		return true;
	}
}

bool SaveFile(std::wstring const& path, std::vector<uint8_t> const& body)
{
	std::ofstream f(path, std::ios::binary | std::ios::out);
	if (!f.good()) {
		return false;
	}

	f.write(reinterpret_cast<char const*>(body.data()), body.size());
	return f.good();
}

bool SaveFile(std::wstring const& path, std::string const& body)
{
	std::ofstream f(path, std::ios::binary | std::ios::out);
	if (!f.good()) {
		return false;
	}

	f.write(body.data(), body.size());
	return f.good();
}

bool LoadFile(std::wstring const& path, std::vector<uint8_t>& body)
{
	std::ifstream f(path, std::ios::in | std::ios::binary);
	if (f.good()) {
		f.seekg(0, std::ios::end);
		auto size = f.tellg();
		f.seekg(0, std::ios::beg);

		body.resize(size);
		f.read(reinterpret_cast<char*>(body.data()), size);
		return f.good();
	}

	return false;
}

bool LoadFile(std::wstring const& path, std::string& body)
{
	std::ifstream f(path, std::ios::in | std::ios::binary);
	if (f.good()) {
		f.seekg(0, std::ios::end);
		auto size = f.tellg();
		f.seekg(0, std::ios::beg);

		body.resize(size);
		f.read(body.data(), size);
		return f.good();
	}

	return false;
}

END_SE()
