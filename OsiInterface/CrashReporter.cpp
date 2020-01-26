#include "stdafx.h"

#include <DbgHelp.h>
#include <psapi.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

class CrashReporter
{
public:
	static void * Backtrace[32];
	static bool Initialized;
	static LPTOP_LEVEL_EXCEPTION_FILTER PrevExceptionFilter;

	static void Initialize()
	{
		if (Initialized || IsDebuggerPresent()) return;

		PrevExceptionFilter = SetUnhandledExceptionFilter(&OnUnhandledException);
		Initialized = true;
	}

	static void Shutdown()
	{
		if (!Initialized) return;

		SetUnhandledExceptionFilter(PrevExceptionFilter);
		PrevExceptionFilter = nullptr;
		Initialized = false;
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

	static void LaunchCrashReporter(std::wstring & miniDumpPath)
	{
		std::wstring crashReporterPath;
		crashReporterPath.resize(1024);
		DWORD length = GetModuleFileName(gThisModule, crashReporterPath.data(), (DWORD)crashReporterPath.size());
		// Module not found
		if (length == 0) return;
		crashReporterPath.resize(length);

		auto sep = crashReporterPath.find_last_of('\\');
		crashReporterPath = crashReporterPath.substr(0, sep);
		crashReporterPath += L"\\CrashReporter.exe";

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcessW(crashReporterPath.c_str(), miniDumpPath.data(), NULL, NULL, FALSE, 0,
			NULL, NULL, &si, &pi)) {
			return;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	static DWORD WINAPI CrashReporterThread(LPVOID exceptionInfo)
	{
		auto dumpPath = GetMiniDumpPath();
		if (CreateMiniDump((_EXCEPTION_POINTERS *)exceptionInfo, dumpPath)) {
			LaunchCrashReporter(dumpPath);
		}

		ExitProcess(1);
		return 0;
	}

	static LONG OnUnhandledException(_EXCEPTION_POINTERS * exceptionInfo)
	{
		if (IsExtensionRelatedCrash()) {
			// Run crash reporter in a separate thread to ensure that
			// we succeed if the stack is corrupted
			CreateThread(NULL, 0, &CrashReporterThread, exceptionInfo, 0, NULL);
			SuspendThread(GetCurrentThread());

			return EXCEPTION_EXECUTE_HANDLER;
		} else if (PrevExceptionFilter != nullptr) {
			return PrevExceptionFilter(exceptionInfo);
		} else {
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}
};

void * CrashReporter::Backtrace[32];
bool CrashReporter::Initialized{ false };
LPTOP_LEVEL_EXCEPTION_FILTER CrashReporter::PrevExceptionFilter{ nullptr };

void InitCrashReporting()
{
	CrashReporter::Initialize();
}

void ShutdownCrashReporting()
{
	CrashReporter::Shutdown();
}
