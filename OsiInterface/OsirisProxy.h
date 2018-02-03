#pragma once

#include "DivInterface.h"

namespace osi {

using namespace eoc;

template <typename T>
class WrappedFunction;

template <typename R, typename... Params>
class WrappedFunction<R(Params...)>
{
public:
	typedef R(*FuncType)(Params...);

	bool IsWrapped() const
	{
		return OriginalFunc != nullptr;
	}

	void Wrap(void * Function, FuncType NewFunction)
	{
		if (IsWrapped()) {
			throw std::runtime_error("Tried to wrap function multiple times");
		}

		OriginalFunc = Function;
		NewFunc = NewFunction;
		DetourAttachEx((PVOID *)&OriginalFunc, (PVOID)NewFunc, (PDETOUR_TRAMPOLINE *)&FuncTrampoline, NULL, NULL);
	}

	void Unwrap()
	{
		if (IsWrapped()) {
			DetourDetach((PVOID *)&OriginalFunc, (PVOID)NewFunc);
		}
	}

	inline R operator ()(Params... Args)
	{
		return FuncTrampoline(Args...);
	}

private:
	void * OriginalFunc{ nullptr };
	FuncType NewFunc{ nullptr };
	FuncType FuncTrampoline{ nullptr };
};

class OsirisProxy
{
public:
	void Initialize();
	void Shutdown();
	void EnableHooks(bool Enabled);
	void SetupLogging(bool Enabled, DebugFlag LogLevel, std::wstring const & Path);

private:
	WrappedFunction<int(void *, DivFunctions *)> RegisterDivFunctions;
	void * OsirisObject{ nullptr };
	OsirisManager * Manager{ nullptr };
	VariableDb ** OsirisVariables{ nullptr };
	TypeDb ** OsirisTypes{ nullptr };
	TypeDb ** OsirisFunctions{ nullptr };
	TypeDb ** OsirisObjects{ nullptr };
	void ** OsirisGoals{ nullptr };
	AdapterDb ** OsirisAdapters{ nullptr };
	DatabaseDb ** OsirisDatabases{ nullptr };
	NodeDb ** OsirisNodes{ nullptr };
	DebugFlag * GlobalDebugFlags{ nullptr };

	HMODULE OsirisModule{ NULL };

	std::map<void *, NodeType> NodeVMTs;

	int RegisterDIVFunctionsWrapper(void *, DivFunctions *);
	static int SRegisterDIVFunctionsWrapper(void *, DivFunctions *);

	bool CallWrapper(uint32_t FunctionHandle, CallParam * Params);
	static bool SCallWrapper(uint32_t FunctionHandle, CallParam * Params);
	DivFunctions::CallProc CallOriginal;

	bool QueryWrapper(uint32_t FunctionHandle, CallParam * Params);
	static bool SQueryWrapper(uint32_t FunctionHandle, CallParam * Params);
	DivFunctions::CallProc QueryOriginal;

	void ErrorWrapper(char const * Message);
	static void SErrorWrapper(char const * Message);
	DivFunctions::ErrorMessageProc ErrorOriginal;

	void AssertWrapper(char const * Unknown, char const * Message, bool Unknown2);
	static void SAssertWrapper(char const * Unknown, char const * Message, bool Unknown2);
	DivFunctions::AssertProc AssertOriginal;

	COsirisCloseLogFileProc OsirisCloseLogFileProc;
	COsirisOpenLogFileProc OsirisOpenLogFileProc;
	COsirisReadHeaderProc OsirisReadHeaderProc;

	int OsirisReadHeader(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags);
	static int SOsirisReadHeader(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags);
	WrappedFunction<int(void *, void *, unsigned __int8 *, unsigned __int8 *, unsigned __int8 *, unsigned __int8 *, char *, unsigned int *)> WrappedOsirisReadHeader;

	bool HooksEnabled{ false };
	bool LoggingEnabled{ false };
	DebugFlag DesiredLogLevel;
	std::wstring LogDirectory;
	std::wstring LogFilename;

	void DebugDumpCall(char const * Type, OsirisFunctionHandle Handle, CallParam * Params);
	void FindOsirisGlobals(FARPROC CtorProc);
	void FindDebugFlags(FARPROC SetOptionProc);
	void ResolveNodeVMTs(NodeDb * Db);
	void RestartLogging();
};

extern std::unique_ptr<OsirisProxy> gOsirisProxy;

}
