#pragma once

#include "DivInterface.h"
#include "DebugInterface.h"
#include "DebugMessages.h"
#include "Debugger.h"
#include <detours.h>
#include <thread>

namespace osidbg {

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

struct OsirisGlobals
{
	void * OsirisObject{ nullptr };
	OsirisManager * Manager{ nullptr };
	VariableDb ** Variables{ nullptr };
	TypeDb ** Types{ nullptr };
	TypeDb ** Functions{ nullptr };
	TypeDb ** Objects{ nullptr };
	GoalDb ** Goals{ nullptr };
	AdapterDb ** Adapters{ nullptr };
	DatabaseDb ** Databases{ nullptr };
	NodeDb ** Nodes{ nullptr };
	DebugFlag * DebugFlags{ nullptr };
};

class OsirisProxy
{
public:
	void Initialize();
	void Shutdown();
	void EnableHooks(bool Enabled);
	void SetupLogging(bool Enabled, DebugFlag LogLevel, std::wstring const & Path);

	inline OsirisGlobals const & GetGlobals() const
	{
		return Globals;
	}

private:
	WrappedFunction<int(void *, DivFunctions *)> RegisterDivFunctions;
	WrappedFunction<int(void *)> InitGame;
	OsirisGlobals Globals;

	HMODULE OsirisModule{ NULL };
	void * OsirisDllStart{ nullptr };
	uint32_t OsirisDllSize{ 0 };

	NodeVMT * NodeVMTs[(unsigned)NodeType::Max + 1];
	bool ResolvedNodeVMTs{ false };

	int RegisterDIVFunctionsWrapper(void *, DivFunctions *);
	static int SRegisterDIVFunctionsWrapper(void *, DivFunctions *);

	int InitGameWrapper(void *);
	static int SInitGameWrapper(void *);

	bool CallWrapper(uint32_t FunctionHandle, CallParam * Params);
	static bool SCallWrapper(uint32_t FunctionHandle, CallParam * Params);
	DivFunctions::CallProc CallOriginal;

	bool QueryWrapper(uint32_t FunctionHandle, CallParam * Params);
	static bool SQueryWrapper(uint32_t FunctionHandle, CallParam * Params);
	DivFunctions::CallProc QueryOriginal;

	void ErrorWrapper(char const * Message);
	static void SErrorWrapper(char const * Message);
	DivFunctions::ErrorMessageProc ErrorOriginal;

	void AssertWrapper(bool Successful, char const * Message, bool Unknown2);
	static void SAssertWrapper(bool Successful, char const * Message, bool Unknown2);
	DivFunctions::AssertProc AssertOriginal;

	COsirisCloseLogFileProc OsirisCloseLogFileProc;
	COsirisOpenLogFileProc OsirisOpenLogFileProc;
	COsirisInitGameProc OsirisInitGameProc;
	COsirisReadHeaderProc OsirisReadHeaderProc;
	COsirisLoadProc OsirisLoadProc;
	RuleActionCallProc OriginalRuleActionCallProc;

	int OsirisReadHeader(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags);
	static int SOsirisReadHeader(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags);
	WrappedFunction<int(void *, void *, unsigned __int8 *, unsigned __int8 *, unsigned __int8 *, unsigned __int8 *, char *, unsigned int *)> WrappedOsirisReadHeader;

	int OsirisLoad(void * Osiris, void * Buf);
	static int SOsirisLoad(void * Osiris, void * Buf);
	WrappedFunction<int (void *, void *)> WrappedOsirisLoad;

	void RuleActionCall(RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4);
	static void SRuleActionCall(RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4);
	WrappedFunction<void (RuleActionNode *, void *, void *, void *, void *)> WrappedRuleActionCall;

	bool HooksEnabled{ true };
	bool LoggingEnabled{ false };
	DebugFlag DesiredLogLevel;
	std::wstring LogDirectory;
	std::wstring LogFilename;

	std::thread * DebuggerThread{ nullptr };
	bool StoryLoaded{ false };

	std::unique_ptr<DebugInterface> debugInterface_;
	std::unique_ptr<DebugMessageHandler> debugMsgHandler_;
	std::unique_ptr<Debugger> debugger_;

	void DebugDumpCall(char const * Type, OsirisFunctionHandle Handle, CallParam * Params);
	void * FindRuleActionCallProc();
	void FindOsirisGlobals(FARPROC CtorProc);
	void FindDebugFlags(FARPROC SetOptionProc);
	void ResolveNodeVMTs(NodeDb * Db);
	void SaveNodeVMT(NodeType type, NodeVMT * vmt);
	void HookNodeVMTs();
	void RestartLogging();
};

extern std::unique_ptr<OsirisProxy> gOsirisProxy;

}
