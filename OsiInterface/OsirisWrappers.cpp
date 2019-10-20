#include "stdafx.h"
#include "OsirisProxy.h"
#include "NodeHooks.h"
#include "DebugInterface.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <psapi.h>

namespace osidbg
{

#define STATIC_HOOK(name) decltype(OsirisWrappers::name) * decltype(OsirisWrappers::name)::gHook;

STATIC_HOOK(RegisterDivFunctions)
STATIC_HOOK(InitGame)
STATIC_HOOK(DeleteAllData)
STATIC_HOOK(GetFunctionMappings)
STATIC_HOOK(OpenLogFile)
STATIC_HOOK(CloseLogFile)
STATIC_HOOK(ReadHeader)
STATIC_HOOK(Compile)
STATIC_HOOK(Load)
STATIC_HOOK(Merge)
STATIC_HOOK(Event)
STATIC_HOOK(RuleActionCall)
STATIC_HOOK(Call)
STATIC_HOOK(Query)
STATIC_HOOK(Error)
STATIC_HOOK(Assert)
STATIC_HOOK(CreateFileW)
STATIC_HOOK(CloseHandle)


OsirisWrappers::OsirisWrappers()
{
	using namespace std::placeholders;
	RegisterDivFunctions.AddPreHook(std::bind(&OsirisWrappers::RegisterDIVFunctionsPreHook, this, _1, _2));
}

void OsirisWrappers::RegisterDIVFunctionsPreHook(void * Osiris, DivFunctions * Functions)
{
	CallOriginal = Functions->Call;
	QueryOriginal = Functions->Query;
	ErrorOriginal = Functions->ErrorMessage;
	AssertOriginal = Functions->Assert;

	Functions->Call = &CallWrapper;
	Functions->Query = &QueryWrapper;
	Functions->ErrorMessage = &ErrorWrapper;
	Functions->Assert = &AssertWrapper;
}

void OsirisWrappers::Initialize()
{
	Kernel32Module = LoadLibrary(L"kernel32.dll");
	if (Kernel32Module == NULL) {
		Fail(L"Could not load kernel32.dll");
	}

	OsirisModule = LoadLibrary(L"osiris_x64.dll");
	if (OsirisModule == NULL) {
		Fail(L"Could not load osiris_x64.dll");
	}

	MODULEINFO moduleInfo;
	if (!GetModuleInformation(GetCurrentProcess(), OsirisModule, &moduleInfo, sizeof(moduleInfo))) {
		Fail(L"Could not get module info of osiris_x64.dll");
	}

	OsirisDllStart = moduleInfo.lpBaseOfDll;
	OsirisDllSize = moduleInfo.SizeOfImage;

	OriginalRuleActionCallProc = (RuleActionCallProc)FindRuleActionCallProc();
	if (!OriginalRuleActionCallProc) {
		Fail(L"Could not locate RuleAction::Call in osiris_x64.dll");
	}

	FARPROC OsirisCtorProc = GetProcAddress(OsirisModule, "??0COsiris@@QEAA@XZ");
	if (OsirisCtorProc == NULL) {
		Fail(L"Could not locate COsiris::COsiris() in osiris_x64.dll");
	}

	FindOsirisGlobals(OsirisCtorProc);

	FARPROC SetOptionProc = GetProcAddress(OsirisModule, "?SetOption@COsiris@@QEAAXI@Z");
	if (SetOptionProc == NULL) {
		Fail(L"Could not locate COsiris::SetOption in osiris_x64.dll");
	}

	FindDebugFlags(SetOptionProc);

#if 0
	Debug(L"OsirisWrappers::Initialize: Detouring functions");
#endif

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	RegisterDivFunctions.Wrap(OsirisModule, "?RegisterDIVFunctions@COsiris@@QEAAXPEAUTOsirisInitFunction@@@Z");
	InitGame.Wrap(OsirisModule, "?InitGame@COsiris@@QEAA_NXZ");
	DeleteAllData.Wrap(OsirisModule, "?DeleteAllData@COsiris@@QEAAX_N@Z");
	GetFunctionMappings.Wrap(OsirisModule, "?GetFunctionMappings@COsiris@@QEAAXPEAPEAUMappingInfo@@PEAI@Z");
	OpenLogFile.Wrap(OsirisModule, "?OpenLogFile@COsiris@@QEAA_NPEB_W0@Z");
	CloseLogFile.Wrap(OsirisModule, "?CloseLogFile@COsiris@@QEAAXXZ");
	ReadHeader.Wrap(OsirisModule, "?_ReadHeader@COsiris@@IEAA_NAEAVCOsiSmartBuf@@AEAE111PEADAEAI@Z");
	Load.Wrap(OsirisModule, "?Load@COsiris@@QEAA_NAEAVCOsiSmartBuf@@@Z");
	Compile.Wrap(OsirisModule, "?Compile@COsiris@@QEAA_NPEB_W0@Z");
	Merge.Wrap(OsirisModule, "?Merge@COsiris@@QEAA_NPEB_W@Z");
	Event.Wrap(OsirisModule, "?Event@COsiris@@QEAA?AW4ReturnCode@osi@@IPEAVCOsiArgumentDesc@@@Z");
	RuleActionCall.Wrap(OriginalRuleActionCallProc);

	Call.Wrap(&CallWrapper);
	Query.Wrap(&QueryWrapper);
	Error.Wrap(&ErrorWrapper);
	// FIXME - PERF Assert.Wrap(&AssertWrapper);

	CreateFileW.Wrap(Kernel32Module, "CreateFileW");
	CloseHandle.Wrap(Kernel32Module, "CloseHandle");

	DetourTransactionCommit();
}

void OsirisWrappers::Shutdown()
{
#if 0
	Debug(L"OsirisWrappers::Shutdown: Unregistering hooks");
#endif
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	RegisterDivFunctions.Unwrap();
	InitGame.Unwrap();
	DeleteAllData.Unwrap();
	GetFunctionMappings.Unwrap();
	OpenLogFile.Unwrap();
	CloseLogFile.Unwrap();
	ReadHeader.Unwrap();
	Load.Unwrap();
	Compile.Unwrap();
	Merge.Unwrap();
	RuleActionCall.Unwrap();

	Call.Unwrap();
	Query.Unwrap();
	Error.Unwrap();
	Assert.Unwrap();

	CreateFileW.Unwrap();
	CloseHandle.Unwrap();

	DetourTransactionCommit();

	FreeModule(Kernel32Module);
	FreeModule(OsirisModule);
}

bool OsirisWrappers::CallWrapper(uint32_t FunctionId, OsiArgumentDesc * Params)
{
	return gOsirisProxy->GetWrappers().CallOriginal(FunctionId, Params);
}

bool OsirisWrappers::QueryWrapper(uint32_t FunctionId, OsiArgumentDesc * Params)
{
	return gOsirisProxy->GetWrappers().QueryOriginal(FunctionId, Params);
}

void OsirisWrappers::ErrorWrapper(char const * Message)
{
	gOsirisProxy->GetWrappers().ErrorOriginal(Message);
}

void OsirisWrappers::AssertWrapper(bool Successful, char const * Message, bool Unknown2)
{
	gOsirisProxy->GetWrappers().AssertOriginal(Successful, Message, Unknown2);
}

void * OsirisWrappers::FindRuleActionCallProc()
{
#if 0
	Debug(L"OsirisWrappers::FindRuleActionCallProc");
#endif
	uint8_t * Addr = static_cast<uint8_t *>(OsirisDllStart);

	// Function prologue of RuleAction::Call
	static const uint8_t instructions[18] = {
		0x40, 0x55, // push rbp
		0x53, // push rbx
		0x56, // push rsi
		0x41, 0x56, // push r14
		0x48, 0x8D, 0x6C, 0x24, 0xC1, // lea rbp, [rsp-3Fh]
		0x48, 0x81, 0xEC, 0x88, 0x00, 0x00, 0x00 // sub rsp, 88h
	};

	// Look for prologue in the entire osiris DLL
	for (uint8_t * ptr = Addr; ptr < Addr + OsirisDllSize; ptr++)
	{
		if (*reinterpret_cast<uint64_t *>(ptr) == *reinterpret_cast<uint64_t const *>(&instructions[0])
			&& memcmp(instructions, ptr, sizeof(instructions)) == 0)
		{
			return ptr;
		}
	}

	return nullptr;
}

uint8_t * ResolveRealFunctionAddress(uint8_t * Address)
{
	// Resolve function pointer through relocations
	for (uint8_t * ptr = Address; ptr < Address + 64; ptr++)
	{
		// Look for the instruction "cmp qword ptr [rip+xxxxxx], 0"
		if (ptr[0] == 0x48 && ptr[1] == 0x83 && ptr[2] == 0x3d && ptr[6] == 0x00 &&
			// Look for the instruction "jmp xxxx"
			ptr[13] == 0xe9)
		{
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 14);
			return ptr + relOffset + 18;
		}
	}

	// Could not find any relocations
	return Address;
}

void OsirisWrappers::FindOsirisGlobals(FARPROC CtorProc)
{
#if 0
	Debug(L"OsirisProxy::FindOsirisGlobals:");
#endif
	uint8_t * Addr = ResolveRealFunctionAddress((uint8_t *)CtorProc);

	// Try to find pointers of Osiris globals
	const unsigned NumGlobals = 8;
	uint8_t * globals[NumGlobals];
	unsigned foundGlobals = 0;
	for (uint8_t * ptr = Addr; ptr < Addr + 0x300; ptr++)
	{
		// Look for the instruction "jmp short $+7"
		if (ptr[0] == 0xEB && ptr[1] == 0x07 &&
			// Look for the instruction "mov cs:[rip + xxx], <64-bit register>"
			ptr[2] == 0x48 && ptr[3] == 0x89 && (ptr[4] & 0xC7) == 0x05)
		{
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 5);
			uint64_t osiPtr = (uint64_t)ptr + relOffset + 9;
			globals[foundGlobals++] = (uint8_t *)osiPtr;
			if (foundGlobals == NumGlobals) break;
		}
	}

	if (foundGlobals < NumGlobals)
	{
		Fail(L"Could not locate global Osiris variables");
	}

	Globals.Variables = (VariableDb **)globals[0];
	Globals.Types = (OsiTypeDb **)globals[1];
	Globals.Functions = (FunctionDb **)globals[2];
	Globals.Objects = (ObjectDb **)globals[3];
	Globals.Goals = (GoalDb **)globals[4];
	Globals.Adapters = (AdapterDb **)globals[5];
	Globals.Databases = (DatabaseDb **)globals[6];
	Globals.Nodes = (NodeDb **)globals[7];

#if 0
	Debug(L"\tVariables = %p", Globals.Variables);
	Debug(L"\tTypes = %p", Globals.Types);
	Debug(L"\tFunctions = %p", Globals.Functions);
	Debug(L"\tObjects = %p", Globals.Objects);
	Debug(L"\tGoals = %p", Globals.Goals);
	Debug(L"\tAdapters = %p", Globals.Adapters);
	Debug(L"\tDatabases = %p", Globals.Databases);
	Debug(L"\tNodes = %p", Globals.Nodes);
#endif
}

void OsirisWrappers::FindDebugFlags(FARPROC SetOptionProc)
{
	uint8_t * Addr = ResolveRealFunctionAddress((uint8_t *)SetOptionProc);

	// Try to find pointer of global var DebugFlags
	for (uint8_t * ptr = Addr; ptr < Addr + 0x80; ptr++)
	{
		// Look for the instruction "mov ecx, cs:xxx"
		if (ptr[0] == 0x8B && ptr[1] == 0x0D &&
			// Look for the instruction "shr e*x, 14h"
			ptr[8] == 0xC1 && ptr[10] == 0x14)
		{
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 2);
			uint64_t dbgPtr = (uint64_t)ptr + relOffset + 6;
			Globals.DebugFlags = (DebugFlag *)dbgPtr;
			break;
		}
	}

	if (Globals.DebugFlags == nullptr) {
		Fail(L"Could not locate global variable DebugFlags");
	}

#if 0
	Debug(L"OsirisProxy::FindDebugFlags: DebugFlags = %p", Globals.DebugFlags);
#endif
}

}