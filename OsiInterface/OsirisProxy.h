#pragma once

#include "OsiInterface.h"
#include "DebugInterface.h"
#include "DebugMessages.h"
#include "Debugger.h"
#include "OsirisWrappers.h"
#include "CustomFunctions.h"
#include "DataLibraries.h"
#include "Functions/FunctionLibrary.h"

#include <thread>

namespace osidbg {

class OsirisProxy
{
public:
	OsirisProxy();
	void Initialize();
	void Shutdown();
	void EnableDebugging(bool Enabled, uint16_t Port);
	void EnableExtensions(bool Enabled);
	void SetupLogging(bool Enabled, DebugFlag LogLevel, std::wstring const & Path);
	void EnableCompileLogging(bool Log);

	inline OsirisStaticGlobals const & GetGlobals() const
	{
		return Wrappers.Globals;
	}

	inline OsirisDynamicGlobals const & GetDynamicGlobals() const
	{
		return DynGlobals;
	}

	inline CustomFunctionManager & GetCustomFunctionManager()
	{
		return CustomFunctions;
	}

	inline CustomFunctionInjector const & GetCustomFunctionInjector() const
	{
		return CustomInjector;
	}

	inline LibraryManager const & GetLibraryManager() const
	{
		return Libraries;
	}

	inline void * GetOsirisDllStart() const
	{
		return Wrappers.OsirisDllStart;
	}

	inline uint32_t GetOsirisDllSize() const
	{
		return Wrappers.OsirisDllSize;
	}

	inline OsirisWrappers const & GetWrappers() const
	{
		return Wrappers;
	}

private:
	OsirisWrappers Wrappers;
	OsirisDynamicGlobals DynGlobals;
	CustomFunctionManager CustomFunctions;
	CustomFunctionInjector CustomInjector;
	CustomFunctionLibrary FunctionLibrary;
	LibraryManager Libraries;

	NodeVMT * NodeVMTs[(unsigned)NodeType::Max + 1];
	bool ResolvedNodeVMTs{ false };

	void OnRegisterDIVFunctions(void *, DivFunctions *);
	void OnInitGame(void *);
	void OnDeleteAllData(void *, bool);

	void OnError(char const * Message);
	void OnAssert(bool Successful, char const * Message, bool Unknown2);
	bool CompileWrapper(std::function<bool (void *, wchar_t const *, wchar_t const *)> const & Next, void * Osiris, wchar_t const * Path, wchar_t const * Mode);
	void OnAfterOsirisLoad(void * Osiris, void * Buf, int retval);
	bool MergeWrapper(std::function<bool(void *, wchar_t *)> const & Next, void * Osiris, wchar_t * Src);
	void RuleActionCall(std::function<void(RuleActionNode *, void *, void *, void *, void *)> const & Next, RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4);

	bool DebuggingEnabled{ false };
	uint16_t DebuggerPort;

	bool ExtensionsEnabled{ false };

	bool LoggingEnabled{ false };
	bool CompilationLogEnabled{ false };
	DebugFlag DesiredLogLevel;
	std::wstring LogDirectory;
	std::wstring LogFilename;
	std::wstring LogType;

	bool StoryLoaded{ false };

#if !defined(OSI_NO_DEBUGGER)
	std::thread * DebuggerThread{ nullptr };
	std::unique_ptr<DebugInterface> debugInterface_;
	std::unique_ptr<DebugMessageHandler> debugMsgHandler_;
	std::unique_ptr<Debugger> debugger_;
	bool DebugDisableLogged{ false };
#endif

	void ResolveNodeVMTs(NodeDb * Db);
	void SaveNodeVMT(NodeType type, NodeVMT * vmt);
	void HookNodeVMTs();
	void RestartLogging(std::wstring const & Type);
	std::wstring MakeLogFilePath(std::wstring const & Type, std::wstring const & Extension);
};

extern std::unique_ptr<OsirisProxy> gOsirisProxy;

}
