#pragma once

#if !defined(OSI_NO_DEBUG_LOG)
#define OsiError(msg) { \
	std::stringstream ss; \
	ss << __FUNCTION__ "(): " msg; \
	gOsirisProxy->LogOsirisError(ss.str()); \
}

#define OsiWarn(msg) { \
	std::stringstream ss; \
	ss << __FUNCTION__ "(): " msg; \
	gOsirisProxy->LogOsirisWarning(ss.str()); \
}

#define OsiMsg(msg) { \
	std::stringstream ss; \
	ss << msg; \
	gOsirisProxy->LogOsirisMsg(ss.str()); \
}

#define OsiErrorS(msg) gOsirisProxy->LogOsirisError(__FUNCTION__ "(): " msg)
#define OsiWarnS(msg) gOsirisProxy->LogOsirisWarning(__FUNCTION__ "(): " msg)
#define OsiMsgS(msg) gOsirisProxy->LogOsirisMsg(__FUNCTION__ "(): " msg)
#else
#define OsiError(msg) (void)0
#define OsiWarn(msg) (void)0
#define OsiMsg(msg) (void)0
#define OsiErrorS(msg) (void)0
#define OsiWarnS(msg) (void)0
#define OsiMsgS(msg) (void)0
#endif


#include <GameDefinitions/Osiris.h>
#include <ExtensionStateClient.h>
#include <ExtensionStateServer.h>
#include "DebugInterface.h"
#include "DebugMessages.h"
#include "Debugger.h"
#include "OsirisWrappers.h"
#include "CustomFunctions.h"
#include "DataLibraries.h"
#include "Functions/FunctionLibrary.h"
#include "NetProtocol.h"
#include <GameDefinitions/Symbols.h>

#include <thread>
#include <mutex>
#include <shared_mutex>

namespace osidbg {


struct ToolConfig
{
#if defined(OSI_EXTENSION_BUILD)
	bool CreateConsole{ false };
	bool EnableLogging{ false };
	bool LogCompile{ false };
	bool EnableExtensions{ true };
	bool EnableDebugger{ false };
#else
	bool CreateConsole{ true };
	bool EnableLogging{ false };
	bool LogCompile{ false };
	bool EnableExtensions{ true };
	bool EnableDebugger{ true };
#endif

	bool SendCrashReports{ true };
	bool EnableAchievements{ true };

#if defined(OSI_EXTENSION_BUILD)
	bool DisableModValidation{ true };
#else
	bool DisableModValidation{ false };
#endif
	bool DumpNetworkStrings{ false };
	uint16_t DebuggerPort{ 9999 };
	uint32_t DebugFlags{ 0 };
	std::wstring LogDirectory;
};


class OsirisProxy
{
public:
	OsirisProxy();

	void Initialize();
	void Shutdown();

	inline ToolConfig & GetConfig()
	{
		return config_;
	}

	void LogOsirisError(std::string const & msg);
	void LogOsirisWarning(std::string const & msg);
	void LogOsirisMsg(std::string const & msg);

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

	inline CustomFunctionLibrary & GetFunctionLibrary()
	{
		return FunctionLibrary;
	}

	inline LibraryManager const & GetLibraryManager() const
	{
		return Libraries;
	}

	inline LibraryManager & GetLibraryManager()
	{
		return Libraries;
	}

	ExtensionState * GetCurrentExtensionState();

	inline ExtensionStateServer & GetServerExtensionState()
	{
		return *ServerExtState;
	}

	inline ExtensionStateClient & GetClientExtensionState()
	{
		return *ClientExtState;
	}

	bool HasFeatureFlag(char const *) const;

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

	inline NetworkManager & GetNetworkManager()
	{
		return networkManager_;
	}

	void ClearPathOverrides();
	void AddPathOverride(std::string const & path, std::string const & overriddenPath);

private:
	OsirisWrappers Wrappers;
	OsirisDynamicGlobals DynGlobals;
	CustomFunctionManager CustomFunctions;
	CustomFunctionInjector CustomInjector;
	CustomFunctionLibrary FunctionLibrary;
	std::unique_ptr<ExtensionStateServer> ServerExtState;
	std::unique_ptr<ExtensionStateClient> ClientExtState;
	LibraryManager Libraries;
	bool LibrariesPostInitialized{ false };
	bool ServerExtensionLoaded{ false };
	bool ClientExtensionLoaded{ false };
	std::unordered_set<DWORD> ClientThreadIds;
	std::unordered_set<DWORD> ServerThreadIds;
	std::recursive_mutex globalStateLock_;
	NetworkManager networkManager_;
	std::shared_mutex pathOverrideMutex_;
	std::unordered_map<std::string, std::string> pathOverrides_;

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

	ToolConfig config_;
	bool extensionsEnabled_{ false };

	std::wstring LogFilename;
	std::wstring LogType;

	bool StoryLoaded{ false };
	std::recursive_mutex storyLoadLock_;

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

	void OnBaseModuleLoaded(void * self);
	void OnClientGameStateChanged(void * self, ClientGameState fromState, ClientGameState toState);
	void OnServerGameStateChanged(void * self, ServerGameState fromState, ServerGameState toState);
	void OnClientGameStateWorkerStart(void * self);
	void OnServerGameStateWorkerStart(void * self);
	void OnSkillPrototypeManagerInit(void * self);
	FileReader * OnFileReaderCreate(ls__FileReader__FileReader next, FileReader * self, Path * path, unsigned int type);
	void PostInitLibraries();
	void ResetExtensionStateServer();
	void LoadExtensionStateServer();
	void ResetExtensionStateClient();
	void LoadExtensionStateClient();

	void OnInitNetworkFixedStrings(void * self, void * arg1);
	void DumpNetworkFixedStrings();

	void AddClientThread(DWORD threadId);
	void AddServerThread(DWORD threadId);
};

extern std::unique_ptr<OsirisProxy> gOsirisProxy;

}
