#include "stdafx.h"
#include "HttpUploader.h"

#include <DbgHelp.h>
#include <psapi.h>
#include "resource.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

extern HMODULE gThisModule;

class CrashReporter
{
public:
	static void * Backtrace[16];
	static bool Initialized;
	static std::wstring MiniDumpPath;

	static void Initialize()
	{
		if (Initialized || IsDebuggerPresent()) return;

		SetUnhandledExceptionFilter(&OnUnhandledException);
		Initialized = true;
	}

	static std::wstring GetMiniDumpPath()
	{
		std::wstring tempPath;
		DWORD tempPathLen = GetTempPathW(0, NULL);
		tempPath.resize(tempPathLen);
		GetTempPathW(tempPathLen, tempPath.data());
		tempPath.resize(tempPathLen - 1);

		tempPath += L"\\OsiExtMiniDump.dmp";
		return tempPath;
	}

	static bool CreateMiniDump(_EXCEPTION_POINTERS * exceptionInfo, std::wstring const & dumpPath)
	{
		HANDLE dumpFile = CreateFileW(dumpPath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if (dumpFile == INVALID_HANDLE_VALUE) return false;

		_MINIDUMP_EXCEPTION_INFORMATION dumpExcInfo;
		dumpExcInfo.ThreadId = ::GetCurrentThreadId();
		dumpExcInfo.ExceptionPointers = exceptionInfo;
		dumpExcInfo.ClientPointers = FALSE;

		bool written = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			dumpFile, MiniDumpNormal, &dumpExcInfo, NULL, NULL);
		CloseHandle(dumpFile);
		return written;
	}

	// Inspects the current stack to check whether the extension DLL
	// is present in any of the frames. If it is, the crash may be extension related.
	// If no extension frames are present, we skip error reporting.
	// noinline needed to ensure that the error handler stack is always 2 levels deep
	static __declspec(noinline) bool IsExtensionRelatedCrash()
	{
		void * moduleStart, * moduleEnd;
		MODULEINFO moduleInfo;
		if (!GetModuleInformation(GetCurrentProcess(), gThisModule, &moduleInfo, sizeof(moduleInfo))) {
			return false;
		}

		moduleStart = moduleInfo.lpBaseOfDll;
		moduleEnd = (uint8_t *)moduleStart + moduleInfo.SizeOfImage;

		auto traceSize = CaptureStackBackTrace(2, (DWORD)std::size(Backtrace), Backtrace, NULL);
		for (auto i = 0; i < traceSize; i++) {
			if (Backtrace[i] >= moduleStart && Backtrace[i] < moduleEnd) {
				return true;
			}
		}

		return false;
	}

	static void UploadMinidump()
	{
		std::ifstream f(MiniDumpPath, std::ios::in | std::ios::binary);
		if (!f.good()) return;
		std::vector<uint8_t> crashDump;
		f.seekg(0, std::ios::end);
		auto dumpSize = f.tellg();
		crashDump.resize(dumpSize);
		f.seekg(0, std::ios::beg);
		f.read((char *)crashDump.data(), dumpSize);

		std::vector<uint8_t> response;
		std::string errorReason;
		std::string dumpId;
		HttpUploader uploader(L"osicrashreports.norbyte.dev");

		bool succeeded = uploader.Upload(L"/submit.php", crashDump, response);
		if (succeeded) {
			if (response.size() < 4 || memcmp(response.data(), "OK:", 3) != 0) {
				errorReason = "Server returned illegible response";
			} else {
				dumpId.resize(response.size() - 3);
				memcpy(dumpId.data(), response.data() + 3, response.size() - 3);
			}
		} else {
			errorReason = uploader.GetLastError();
		}

		std::wstring resultText;
		if (succeeded) {
			resultText = L"Crash dump successfully uploaded.\r\nYour crash dump reference number is ";
			resultText += FromUTF8(dumpId) + L".";
			MessageBoxW(NULL, resultText.c_str(), L"Script Extender Crash",
				MB_OK | MB_ICONINFORMATION | MB_TASKMODAL | MB_SETFOREGROUND | MB_TOPMOST);
		} else {
			resultText = L"Unable to upload crash dump. Reason:\r\n";
			resultText += FromUTF8(errorReason);
			MessageBoxW(NULL, resultText.c_str(), L"Script Extender Crash",
				MB_OK | MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND | MB_TOPMOST);
		}
	}

	static DWORD WINAPI CrashReporterThread(LPVOID exceptionInfo)
	{
		auto result = MessageBoxW(NULL, L"The game has unexpectedly crashed.\r\n\r\n"
			L"Would you like to send the crash information to the Script Extender team?\r\n"
			"Additional details about what went wrong can help to create a solution.\r\n"
			"No information besides the crash dump will be uploaded.",
			L"Script Extender Crash",
			MB_YESNO | MB_ICONINFORMATION | MB_TASKMODAL | MB_SETFOREGROUND | MB_TOPMOST);
		if (result == IDYES) {
			auto dumpPath = GetMiniDumpPath();
			if (CreateMiniDump((_EXCEPTION_POINTERS *)exceptionInfo, dumpPath)) {
				MiniDumpPath = dumpPath;
				UploadMinidump();
			}
		}

		ExitProcess(1);
		return 0;
	}

	static LONG OnUnhandledException(_EXCEPTION_POINTERS * exceptionInfo)
	{
		if (IsExtensionRelatedCrash()) {
			CreateThread(NULL, 0, &CrashReporterThread, exceptionInfo, 0, NULL);
			SuspendThread(GetCurrentThread());

			return EXCEPTION_EXECUTE_HANDLER;
		} else {
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}
};

void * CrashReporter::Backtrace[16];
bool CrashReporter::Initialized{ false };
std::wstring CrashReporter::MiniDumpPath;

void InitCrashReporting()
{
	CrashReporter::Initialize();
}
