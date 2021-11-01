#pragma once

#include <GameDefinitions/Osiris.h>
#include <Extender/Shared/StatLoadOrderHelper.h>
#include <Extender/Shared/SavegameSerializer.h>
#include <Extender/Shared/ExtenderConfig.h>
#include <Extender/Shared/ModuleHasher.h>
#include <Osiris/OsirisExtender.h>

#include <Extender/Client/ExtensionStateClient.h>
#include <Extender/Server/ExtensionStateServer.h>
#include <Extender/Client/ScriptExtenderClient.h>
#include <Extender/Server/ScriptExtenderServer.h>
#if !defined(OSI_NO_DEBUGGER)
#include <Osiris/Debugger/DebugInterface.h>
#include <Osiris/Debugger/DebugMessages.h>
#include <Osiris/Debugger/Debugger.h>
#include <Lua/Debugger/LuaDebugger.h>
#include <Lua/Debugger/LuaDebugMessages.h>
#endif
#include <GameHooks/OsirisWrappers.h>
#include <Osiris/Shared/CustomFunctions.h>
#include <GameHooks/DataLibraries.h>
#include <Osiris/Functions/FunctionLibrary.h>
#include "NetProtocol.h"
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/GlobalFixedStrings.h>
#include <Hit.h>
#include <StatusHelpers.h>

#include <thread>
#include <mutex>
#include <shared_mutex>

BEGIN_SE()

class ScriptExtender
{
public:
	ScriptExtender();

	void Initialize();
	void PostStartup();
	void Shutdown();

	inline ExtenderConfig& GetConfig()
	{
		return config_;
	}

	void LogLuaError(std::string_view msg);
	void LogOsirisError(std::string_view msg);
	void LogOsirisWarning(std::string_view msg);
	void LogOsirisMsg(std::string_view msg);

	inline esv::ScriptExtender& GetServer()
	{
		return server_;
	}

	inline ecl::ScriptExtender& GetClient()
	{
		return client_;
	}

	/*inline OsirisExtender const & Osiris() const
	{
		return osiris_;
	}

	inline OsirisExtender & Osiris()
	{
		return osiris_;
	}

	inline OsirisDynamicGlobals const & GetDynamicGlobals() const
	{
		return DynGlobals;
	}

	inline esv::CustomFunctionLibrary & GetFunctionLibrary()
	{
		return FunctionLibrary;
	}*/

	inline LibraryManager const & GetLibraryManager() const
	{
		return Libraries;
	}

	inline LibraryManager & GetLibraryManager()
	{
		return Libraries;
	}

#if !defined(OSI_NO_DEBUGGER)
	inline lua::dbg::Debugger* GetLuaDebugger()
	{
		return luaDebugger_.get();
	}

	inline lua::dbg::DebugMessageHandler* GetLuaDebugMessageHandler()
	{
		return luaDebugMsgHandler_.get();
	}
#endif

	ExtensionStateBase* GetCurrentExtensionState();

	/*inline bool HasServerExtensionState() const
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
	}*/

	bool HasFeatureFlag(char const *) const;

	inline StatLoadOrderHelper& GetStatLoadOrderHelper()
	{
		return statLoadOrderHelper_;
	}

	inline esv::StatusHelpers& GetStatusHelpers()
	{
		return statusHelpers_;
	}

	inline ModuleHasher& Hasher()
	{
		return hasher_;
	}

	void ClearPathOverrides();
	void AddPathOverride(STDString const & path, STDString const & overriddenPath);
	std::optional<STDString> GetPathOverride(STDString const& path);

	std::wstring MakeLogFilePath(std::wstring const& Type, std::wstring const& Extension);
	void InitRuntimeLogging();

private:
	esv::ScriptExtender server_;
	ecl::ScriptExtender client_;
	LibraryManager Libraries;
	//bool LibrariesPostInitialized{ false };
	std::recursive_mutex globalStateLock_;
	std::shared_mutex pathOverrideMutex_;
	std::unordered_map<STDString, STDString> pathOverrides_;
	SavegameSerializer savegameSerializer_;
	StatLoadOrderHelper statLoadOrderHelper_;
	esv::HitProxy hitProxy_;
	esv::StatusHelpers statusHelpers_;
	ModuleHasher hasher_;

	/*NodeVMT * NodeVMTs[(unsigned)NodeType::Max + 1];
	bool ResolvedNodeVMTs{ false };*/

	/*void OnRegisterDIVFunctions(void *, DivFunctions *);
	void OnInitGame(void *);
	void OnDeleteAllData(void *, bool);

	void OnError(char const * Message);
	void OnAssert(bool Successful, char const * Message, bool Unknown2);
	bool CompileWrapper(std::function<bool (void *, wchar_t const *, wchar_t const *)> const & Next, void * Osiris, wchar_t const * Path, wchar_t const * Mode);
	void OnAfterOsirisLoad(void * Osiris, void * Buf, int retval);
	bool MergeWrapper(std::function<bool(void *, wchar_t *)> const & Next, void * Osiris, wchar_t * Src);
	void RuleActionCall(std::function<void(RuleActionNode *, void *, void *, void *, void *)> const & Next, RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4);
	*/
	ExtenderConfig config_;
	bool extensionsEnabled_{ false };
	bool postStartupDone_{ false };

	/*std::wstring LogFilename;
	std::wstring LogType;*/

	/*bool StoryLoaded{ false };
	std::recursive_mutex storyLoadLock_;*/

#if !defined(OSI_NO_DEBUGGER)
	/*std::thread * debuggerThread_{ nullptr };
	std::unique_ptr<OsirisDebugInterface> debugInterface_;
	std::unique_ptr<osidbg::DebugMessageHandler> debugMsgHandler_;
	std::unique_ptr<osidbg::Debugger> debugger_;*/

	std::thread* luaDebuggerThread_{ nullptr };
	std::unique_ptr<LuaDebugInterface> luaDebugInterface_;
	std::unique_ptr<lua::dbg::DebugMessageHandler> luaDebugMsgHandler_;
	std::unique_ptr<lua::dbg::Debugger> luaDebugger_;
#endif

	/*void ResolveNodeVMTs(NodeDb * Db);
	void SaveNodeVMT(NodeType type, NodeVMT * vmt);
	void RestartLogging(std::wstring const & Type);*/

	void OnBaseModuleLoaded(void * self);
	void OnModuleLoadStarted(TranslatedStringRepository * self);
	void OnStatsLoadStarted(CRPGStatsManager* mgr);
	void OnStatsLoadFinished(CRPGStatsManager* mgr);
	void OnSkillPrototypeManagerInit(void * self);
	FileReader * OnFileReaderCreate(ls__FileReader__FileReader next, FileReader * self, Path * path, unsigned int type);
	void OnSavegameVisit(void* osirisHelpers, ObjectVisitor* visitor);

	void OnInitNetworkFixedStrings(eoc::NetworkFixedStrings* self, void * arg1);
};

extern std::unique_ptr<ScriptExtender> gExtender;

END_SE()
