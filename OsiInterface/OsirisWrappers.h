#pragma once

#include "OsiInterface.h"
#include "Wrappers.h"
#include <detours.h>
#include <thread>

namespace osidbg {

uint8_t * ResolveRealFunctionAddress(uint8_t * Address);

#define HOOKABLE(ty, name) enum class name##Tag {}; \
	HookableFunction<name##Tag, ty> name

#define WRAPPABLE(ty, name) enum class name##Tag {}; \
	WrappableFunction<name##Tag, ty> name

class OsirisWrappers
{
public:
	OsirisWrappers();

	void Initialize();
	void Shutdown();

	OsirisStaticGlobals Globals;

	void * OsirisDllStart{ nullptr };
	uint32_t OsirisDllSize{ 0 };

	HOOKABLE(int(void *, DivFunctions *), RegisterDivFunctions);
	HOOKABLE(int(void *), InitGame);
	HOOKABLE(int(void *, bool), DeleteAllData);
	HOOKABLE(void(void *, MappingInfo **, uint32_t *), GetFunctionMappings);

	HOOKABLE(void(void * Osiris, wchar_t const * Path, wchar_t const * Mode), OpenLogFile);
	HOOKABLE(void(void * Osiris), CloseLogFile);

	HOOKABLE(int(void *, void *, unsigned __int8 *, unsigned __int8 *, unsigned __int8 *, unsigned __int8 *, char *, unsigned int *), ReadHeader);
	WRAPPABLE(bool(void *, wchar_t const *, wchar_t const *), Compile);
	HOOKABLE(int(void *, void *), Load);
	WRAPPABLE(bool(void *, wchar_t *), Merge);
	HOOKABLE(ReturnCode(void *, uint32_t, OsiArgumentDesc *), Event);

	RuleActionCallProc OriginalRuleActionCallProc;
	WRAPPABLE(void(RuleActionNode *, void *, void *, void *, void *), RuleActionCall);

	WRAPPABLE(bool(uint32_t FunctionHandle, OsiArgumentDesc * Params), Call);
	WRAPPABLE(bool(uint32_t FunctionHandle, OsiArgumentDesc * Params), Query);
	HOOKABLE(void(char const * Message), Error);
	HOOKABLE(void(bool Successful, char const * Message, bool Unknown2), Assert);

	HOOKABLE(HANDLE(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE), CreateFileW);

	DivFunctions::CallProc CallOriginal;
	DivFunctions::CallProc QueryOriginal;
	DivFunctions::ErrorMessageProc ErrorOriginal;
	DivFunctions::AssertProc AssertOriginal;

private:
	HMODULE Kernel32Module{ NULL };
	HMODULE OsirisModule{ NULL };

	void * FindRuleActionCallProc();
	void FindOsirisGlobals(FARPROC CtorProc);
	void FindDebugFlags(FARPROC SetOptionProc);
	void RegisterDIVFunctionsPreHook(void * Osiris, DivFunctions * Functions);

	static bool CallWrapper(uint32_t FunctionHandle, OsiArgumentDesc * Params);
	static bool QueryWrapper(uint32_t FunctionHandle, OsiArgumentDesc * Params);
	static void ErrorWrapper(char const * Message);
	static void AssertWrapper(bool Successful, char const * Message, bool Unknown2);
};

}
