#include <Windows.h>
#include <iostream>

int main(int argc, char ** argv)
{
	STARTUPINFOA startupInfo = { sizeof(startupInfo) };
	PROCESS_INFORMATION processInfo;
	BOOL created = CreateProcessA(NULL, "DivinityEngine2.exe", NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startupInfo, &processInfo);
	if (!created)
	{
		std::cerr << "Could not launch editor process: error " << GetLastError() << std::endl;
		return 2;
	}

	const char * DebugDllPath = "OsirisDebugBackend.dll";
	LPVOID dllPathPtr = VirtualAllocEx(processInfo.hProcess, NULL, strlen(DebugDllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (dllPathPtr == NULL)
	{
		TerminateProcess(processInfo.hProcess, 1);
		std::cerr << "Could not allocate memory for DLL name: error " << GetLastError() << std::endl;
		return 2;
	}

	BOOL pathWriteOk = WriteProcessMemory(processInfo.hProcess, dllPathPtr, DebugDllPath, strlen(DebugDllPath) + 1, NULL);
	if (!pathWriteOk)
	{
		TerminateProcess(processInfo.hProcess, 1);
		std::cerr << "Could not write DLL name: error " << GetLastError() << std::endl;
		return 2;
	}

	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	LPVOID pLoadLibrary = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryA");

	HANDLE hLoaderThread = CreateRemoteThread(processInfo.hProcess, NULL, NULL, 
		(LPTHREAD_START_ROUTINE)pLoadLibrary, dllPathPtr, NULL, NULL);
	if (hLoaderThread == NULL)
	{
		TerminateProcess(processInfo.hProcess, 1);
		std::cerr << "Could not start library loader thread: error " << GetLastError() << std::endl;
		return 2;
	}

	WaitForSingleObject(hLoaderThread, INFINITE);
	CloseHandle(hLoaderThread);

	ResumeThread(processInfo.hThread);
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

    return 0;
}

