#pragma once

#include <GameDefinitions/Osiris.h>
#include <ExtensionStateClient.h>
#include <ExtensionStateServer.h>
#if !defined(OSI_NO_DEBUGGER)
#include "DebugInterface.h"
#include "DebugMessages.h"
#include "Debugger.h"
#endif
#include "OsirisWrappers.h"
#include "CustomFunctions.h"
#include "DataLibraries.h"
#include "Functions/FunctionLibrary.h"
#include "NetProtocol.h"
#include <GameDefinitions/Symbols.h>

#include <thread>
#include <mutex>
#include <shared_mutex>

namespace dse {


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
#if defined(_DEBUG)
	bool DeveloperMode{ true };
#else
	bool DeveloperMode{ false };
#endif // defined(_DEBUG)
#else
	bool DisableModValidation{ false };
	bool DeveloperMode{ true };
#endif // defined(OSI_EXTENSION_BUILD)

	bool DumpNetworkStrings{ false };
#if defined(OSI_EXTENSION_BUILD)
	bool SyncNetworkStrings{ true };
#else
	bool SyncNetworkStrings{ false };
#endif
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

	void LogOsirisError(std::string_view msg);
	void LogOsirisWarning(std::string_view msg);
	void LogOsirisMsg(std::string_view msg);

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

	inline esv::CustomFunctionLibrary & GetFunctionLibrary()
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

	ExtensionStateBase* GetCurrentExtensionState();

	inline bool HasServerExtensionState() const
	{
		return (bool)ServerExtState;
	}

	inline esv::ExtensionState & GetServerExtensionState()
	{
		return *ServerExtState;
	}

	inline bool HasClientExtensionState() const
	{
		return (bool)ClientExtState;
	}

	inline ecl::ExtensionState & GetClientExtensionState()
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

	inline OsirisWrappers & GetWrappers()
	{
		return Wrappers;
	}

	inline OsirisWrappers const & GetWrappers() const
	{
		return Wrappers;
	}

	inline NetworkManager & GetNetworkManager()
	{
		return networkManager_;
	}

	inline NetworkFixedStringSynchronizer& NetworkFixedStringSync()
	{
		return networkFixedStrings_;
	}

	void ClearPathOverrides();
	void AddPathOverride(STDString const & path, STDString const & overriddenPath);

	bool IsInServerThread() const;
	bool IsInClientThread() const;
	void AttachConsoleThread(bool server);

	std::wstring MakeLogFilePath(std::wstring const& Type, std::wstring const& Extension);

private:
	OsirisWrappers Wrappers;
	OsirisDynamicGlobals DynGlobals;
	CustomFunctionManager CustomFunctions;
	CustomFunctionInjector CustomInjector;
	esv::CustomFunctionLibrary FunctionLibrary;
	std::unique_ptr<esv::ExtensionState> ServerExtState;
	std::unique_ptr<ecl::ExtensionState> ClientExtState;
	LibraryManager Libraries;
	bool LibrariesPostInitialized{ false };
	bool ServerExtensionLoaded{ false };
	bool ClientExtensionLoaded{ false };
	std::unordered_set<DWORD> ClientThreadIds;
	std::unordered_set<DWORD> ServerThreadIds;
	std::recursive_mutex globalStateLock_;
	NetworkManager networkManager_;
	std::shared_mutex pathOverrideMutex_;
	std::unordered_map<STDString, STDString> pathOverrides_;
	NetworkFixedStringSynchronizer networkFixedStrings_;

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
	bool functionLibraryInitialized_{ false };

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

	void OnBaseModuleLoaded(void * self);
	void OnClientGameStateChanged(void * self, ecl::GameState fromState, ecl::GameState toState);
	void OnServerGameStateChanged(void * self, esv::GameState fromState, esv::GameState toState);
	void OnClientGameStateWorkerStart(void * self);
	void OnServerGameStateWorkerStart(void * self);
	void OnSkillPrototypeManagerInit(void * self);
	FileReader * OnFileReaderCreate(ls__FileReader__FileReader next, FileReader * self, Path * path, unsigned int type);
	void PostInitLibraries();
	void ResetExtensionStateServer();
	void LoadExtensionStateServer();
	void ResetExtensionStateClient();
	void LoadExtensionStateClient();

	void OnInitNetworkFixedStrings(eoc::NetworkFixedStrings* self, void * arg1);
	void RegisterFlashTraceCallbacks();

	void AddClientThread(DWORD threadId);
	void AddServerThread(DWORD threadId);
	
	static void FlashTraceCallback(void * ctx, void * player, char const * message);
	static void FlashWarningCallback(void * ctx, void * player, int code, char const * message);
};

extern std::unique_ptr<OsirisProxy> gOsirisProxy;

}
