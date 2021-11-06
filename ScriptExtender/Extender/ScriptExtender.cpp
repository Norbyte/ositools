#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <Osiris/Shared/NodeHooks.h>
#include "Version.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <psapi.h>
#include <regex>

#include <Extender/Shared/StatLoadOrderHelper.inl>
#include <Extender/Shared/SavegameSerializer.inl>

#undef DEBUG_SERVER_CLIENT

void InitCrashReporting();
void ShutdownCrashReporting();

namespace dse
{

std::unique_ptr<ScriptExtender> gExtender;

#if !defined(OSI_NO_DEBUGGER)
void LuaDebugThreadRunner(LuaDebugInterface& intf)
{
	intf.Run();
}
#endif

ScriptExtender::ScriptExtender()
	: /*CustomInjector(Wrappers, CustomFunctions),
	FunctionLibrary(*this),*/
	server_(config_),
	client_(config_),
	hitProxy_(*this)
{
}

void ScriptExtender::Initialize()
{
	if (config_.SendCrashReports) {
		InitCrashReporting();
	}

	extensionsEnabled_ = config_.EnableExtensions;

	GameVersionInfo gameVersion;
	if (Libraries.GetGameVersion(gameVersion)) {
		if (gameVersion.Major == 3 && gameVersion.Minor == 0) {
			Fail(L"Script Extender doesn't support the Classic version of D:OS 2!");
		}

		if (gameVersion.IsSupported()) {
			INFO("Game version v%d.%d.%d.%d OK", gameVersion.Major, gameVersion.Minor, gameVersion.Revision, gameVersion.Build);
		} else {
			ERR("Game version v%d.%d.%d.%d is not supported, please upgrade!", gameVersion.Major, gameVersion.Minor, gameVersion.Revision, gameVersion.Build);
			// Hard exit below a certain version as th EoCClient error display UI won't work anymore
			Fail(L"Script Extender doesn't support game versions below v3.6.54, please upgrade!");
		}
	} else {
		ERR("Failed to retrieve game version info.");
	}

	DEBUG("ScriptExtender::Initialize: Starting");
	auto initStart = std::chrono::high_resolution_clock::now();
	/*Wrappers.Initialize();

	Wrappers.RegisterDivFunctions.AddPreHook(std::bind(&ScriptExtender::OnRegisterDIVFunctions, this, _1, _2));
	Wrappers.InitGame.SetPreHook(std::bind(&ScriptExtender::OnInitGame, this, _1));
	Wrappers.DeleteAllData.SetPreHook(std::bind(&ScriptExtender::OnDeleteAllData, this, _1, _2));
	Wrappers.Error.SetPreHook(std::bind(&ScriptExtender::OnError, this, _1));
	Wrappers.Assert.SetPreHook(std::bind(&ScriptExtender::OnAssert, this, _1, _2, _3));
	Wrappers.Compile.SetWrapper(std::bind(&ScriptExtender::CompileWrapper, this, _1, _2, _3, _4));
	Wrappers.Load.AddPostHook(std::bind(&ScriptExtender::OnAfterOsirisLoad, this, _1, _2, _3));
	Wrappers.Merge.SetWrapper(std::bind(&ScriptExtender::MergeWrapper, this, _1, _2, _3));
#if !defined(OSI_NO_DEBUGGER)
	Wrappers.RuleActionCall.SetWrapper(std::bind(&ScriptExtender::RuleActionCall, this, _1, _2, _3, _4, _5, _6));
#endif
*/
	if (!Libraries.FindLibraries(gameVersion.Revision)) {
		ERR("ScriptExtender::Initialize: Could not load libraries; skipping scripting extension initialization.");
		extensionsEnabled_ = false;
	}

	server_.Initialize();
	client_.Initialize();

	// FIXME - SEPARATE FROM OSI!
	auto& Wrappers = gExtender->GetServer().Osiris().GetWrappers();
	using namespace std::placeholders;
	// Wrap state change functions even if extension startup failed, otherwise
	// we won't be able to show any startup errors
	Wrappers.InitializeExtensions();
	Wrappers.InitNetworkFixedStrings.SetPostHook(std::bind(&ScriptExtender::OnInitNetworkFixedStrings, this, _1, _2));
	/*Wrappers.ClientGameStateChangedEvent.SetPostHook(std::bind(&ScriptExtender::OnClientGameStateChanged, this, _1, _2, _3));
	Wrappers.ServerGameStateChangedEvent.SetPostHook(std::bind(&ScriptExtender::OnServerGameStateChanged, this, _1, _2, _3));
	Wrappers.ClientGameStateWorkerStart.AddPreHook(std::bind(&ScriptExtender::OnClientGameStateWorkerStart, this, _1));
	Wrappers.ServerGameStateWorkerStart.AddPreHook(std::bind(&ScriptExtender::OnServerGameStateWorkerStart, this, _1));
	Wrappers.ClientGameStateWorkerStart.AddPostHook(std::bind(&ScriptExtender::OnClientGameStateWorkerExit, this, _1));
	Wrappers.ServerGameStateWorkerStart.AddPostHook(std::bind(&ScriptExtender::OnServerGameStateWorkerExit, this, _1));*/
	Wrappers.SkillPrototypeManagerInit.SetPreHook(std::bind(&ScriptExtender::OnSkillPrototypeManagerInit, this, _1));
	Wrappers.FileReader__ctor.SetWrapper(std::bind(&ScriptExtender::OnFileReaderCreate, this, _1, _2, _3, _4));
	Wrappers.esv__OsirisVariableHelper__SavegameVisit.SetPreHook(std::bind(&ScriptExtender::OnSavegameVisit, this, _1, _2));
	//Wrappers.TranslatedStringRepository__UnloadOverrides.SetPreHook(std::bind(&ScriptExtender::OnModuleLoadStarted, this, _1));
	Wrappers.RPGStats__Load.AddPreHook(std::bind(&ScriptExtender::OnStatsLoadStarted, this, _1));
	Wrappers.RPGStats__Load.AddPostHook(std::bind(&ScriptExtender::OnStatsLoadFinished, this, _1));

#if !defined(OSI_NO_DEBUGGER)
	if (config_.EnableLuaDebugger && luaDebuggerThread_ == nullptr) {
		DEBUG("Starting Lua debugger server");
		try {
			luaDebugInterface_ = std::make_unique<LuaDebugInterface>(config_.LuaDebuggerPort);
			luaDebugMsgHandler_ = std::make_unique<lua::dbg::DebugMessageHandler>(std::ref(*luaDebugInterface_));
			luaDebuggerThread_ = new std::thread(std::bind(LuaDebugThreadRunner, std::ref(*luaDebugInterface_)));
			luaDebugger_ = std::make_unique<lua::dbg::Debugger>(std::ref(*luaDebugMsgHandler_));
			DEBUG("Lua debugger listening on 127.0.0.1:%d; DBG protocol version %d",
				config_.LuaDebuggerPort, lua::dbg::DebugMessageHandler::ProtocolVersion);
		} catch (std::exception& e) {
			ERR("Lua debugger startup failed: %s", e.what());
		}
	}
#endif

	auto initEnd = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(initEnd - initStart).count();
	DEBUG("Library startup took %d ms", ms);

	if (config_.SendCrashReports) {
		ShutdownCrashReporting();
	}
}

void ScriptExtender::Shutdown()
{
	DEBUG("ScriptExtender::Shutdown: Exiting");
	server_.Shutdown();
	client_.Shutdown();
	hooks_.UnhookAll();

	/*ResetExtensionStateServer();
	ResetExtensionStateClient();
	Wrappers.Shutdown();*/
}

void ScriptExtender::LogLuaError(std::string_view msg)
{
#if !defined(OSI_EOCAPP)
	std::string log = "[Osiris] {E} ";
#else
	std::string log;
#endif
	log += msg;
	gConsole.Debug(DebugMessageType::Error, msg.data());
	server_.Osiris().LogError(msg);
}

void ScriptExtender::LogOsirisError(std::string_view msg)
{
#if !defined(OSI_EOCAPP)
	std::string log = "[Osiris] {E} ";
#else
	std::string log;
#endif
	log += msg;
	gConsole.Debug(DebugMessageType::Error, msg.data());
	server_.Osiris().LogError(msg);

#if !defined(OSI_NO_DEBUGGER)
	if (luaDebugger_) {
		luaDebugger_->OnGenericError(msg.data());
	}
#endif
}

void ScriptExtender::LogOsirisWarning(std::string_view msg)
{
#if !defined(OSI_EOCAPP)
	std::string log = "[Osiris] {W} ";
#else
	std::string log;
#endif
	log += msg;
	gConsole.Debug(DebugMessageType::Warning, log.c_str());
	server_.Osiris().LogWarning(msg);
}

void ScriptExtender::LogOsirisMsg(std::string_view msg)
{
#if !defined(OSI_EOCAPP)
	std::string log = "[Osiris] ";
#else
	std::string log;
#endif
	log += msg;
	gConsole.Debug(DebugMessageType::Osiris, log.c_str());
	server_.Osiris().LogMessage(msg);
}

/*void ScriptExtender::RestartLogging(std::wstring const & Type)
{
	DebugFlag NewFlags = (DebugFlag)((config_.DebugFlags & 0xffff0000) | (*Wrappers.Globals.DebugFlags & 0x0000ffff));

	if (LogFilename.empty() || LogType != Type) {
		LogFilename = MakeLogFilePath(Type, L"log");
		LogType = Type;

		if (!LogFilename.empty()) {
			DEBUG(L"ScriptExtender::RestartLogging: Starting %s debug logging.\r\n"
				"\tPath=%s", Type.c_str(), LogFilename.c_str());
		}
	}

	Wrappers.CloseLogFile.CallOriginal(DynGlobals.OsirisObject);

	if (!LogFilename.empty()) {
		*Wrappers.Globals.DebugFlags = NewFlags;
		Wrappers.OpenLogFile.CallOriginal(DynGlobals.OsirisObject, LogFilename.c_str(), L"ab+");
	}
}*/

std::wstring ScriptExtender::MakeLogFilePath(std::wstring const & Type, std::wstring const & Extension)
{
	if (config_.LogDirectory.empty()) {
		config_.LogDirectory = FromUTF8(GetStaticSymbols().ToPath("/Extender Logs", PathRootType::GameStorage));
	}

	if (config_.LogDirectory.empty()) {
		return L"";
	}

	BOOL created = CreateDirectoryW(config_.LogDirectory.c_str(), NULL);
	if (created == FALSE) {
		DWORD lastError = GetLastError();
		if (lastError != ERROR_ALREADY_EXISTS) {
			std::wstringstream err;
			err << L"Could not create log directory '" << config_.LogDirectory << "': Error " << lastError;
			return L"";
		}
	}

	auto now = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t(now);
	std::tm tm;
	gmtime_s(&tm, &tt);

	std::wstringstream ss;
	ss << config_.LogDirectory << L"\\"
		<< Type << L" "
		<< std::setfill(L'0')
		<< (tm.tm_year + 1900) << L"-"
		<< std::setw(2) << (tm.tm_mon + 1) << L"-"
		<< std::setw(2) << tm.tm_mday << L" "
		<< std::setw(2) << tm.tm_hour << L"-"
		<< std::setw(2) << tm.tm_min << L"-"
		<< std::setw(2) << tm.tm_sec << L"." << Extension;
	return ss.str();
}

/*void ScriptExtender::HookNodeVMTs()
{
	ResolveNodeVMTs(*Wrappers.Globals.Nodes);

	if (ResolvedNodeVMTs && !gNodeVMTWrappers) {
		gNodeVMTWrappers = std::make_unique<NodeVMTWrappers>(NodeVMTs);
	}
}

void ScriptExtender::OnRegisterDIVFunctions(void * Osiris, DivFunctions * Functions)
{
	// FIXME - register before OsirisWrappers!
	StoryLoaded = false;
	DynGlobals.OsirisObject = Osiris;
	uint8_t * interfaceLoadPtr = nullptr;
	// uint8_t * errorMessageFunc = ResolveRealFunctionAddress((uint8_t *)Functions->ErrorMessage);
	uint8_t * errorMessageFunc = ResolveRealFunctionAddress((uint8_t *)Wrappers.ErrorOriginal);

	// Try to find ptr of gOsirisInterface
	OsirisInterface * osirisInterface = nullptr;
	for (uint8_t * ptr = errorMessageFunc; ptr < errorMessageFunc + 64; ptr++) {
		// Look for the instruction "mov rbx, cs:gOsirisInterface"
		if (ptr[0] == 0x48 && ptr[1] == 0x8B && ptr[2] == 0x1D && ptr[6] < 0x02) {
			auto osiPtr = AsmResolveInstructionRef(ptr);
			osirisInterface = *(OsirisInterface **)osiPtr;
			DynGlobals.Manager = osirisInterface->Manager;
			break;
		}
	}

	if (DynGlobals.Manager == nullptr) {
		Fail("Could not locate OsirisInterface");
	}

	// Look for TypedValue::VMT
	uint8_t const copyCtor1[] = {
		0x48, 0x89, 0x5C, 0x24, 0x08, // mov     [rsp+arg_0], rbx
		0x48, 0x89, 0x74, 0x24, 0x10, // mov     [rsp+arg_8], rsi
		0x57, // push    rdi
		0x48, 0x83, 0xEC, 0x20, // sub     rsp, 20h
		0x33, 0xF6, // xor     esi, esi
		0x48, 0x8D, 0x05 // lea     rax, TypedValue::VMT
	};

	auto start = reinterpret_cast<uint8_t *>(Wrappers.OsirisDllStart);
	auto end = start + Wrappers.OsirisDllSize - sizeof(copyCtor1);

	for (auto p = start; p < end; p++) {
		if (*p == 0x48
			&& memcmp(copyCtor1, p, sizeof(copyCtor1)) == 0) {
			Wrappers.Globals.TypedValueVMT = (void *)AsmResolveInstructionRef(p + 17);
			break;
		}
	}

	if (config_.EnableLogging) {
		RestartLogging(L"Osiris Runtime");
	}

#if 0
	DEBUG("ScriptExtender::OnRegisterDIVFunctions: Initializing story.");
	DEBUG("\tErrorMessageProc = %p", errorMessageFunc);
	DEBUG("\tOsirisManager = %p", Globals.Manager);
	DEBUG("\tOsirisInterface = %p", osirisInterface);
#endif

#if !defined(OSI_NO_DEBUGGER)
	// FIXME - move to DebuggerHooks
	if (config_.EnableDebugger) {
		if (debuggerThread_ == nullptr) {
			DEBUG("Starting debugger server");
			try {
				debugInterface_ = std::make_unique<OsirisDebugInterface>(config_.DebuggerPort);
				debugMsgHandler_ = std::make_unique<osidbg::DebugMessageHandler>(std::ref(*debugInterface_));
				debuggerThread_ = new std::thread(std::bind(OsirisDebugThreadRunner, std::ref(*debugInterface_)));
				DEBUG("Osiris debugger listening on 127.0.0.1:%d; DBG protocol version %d", 
					config_.DebuggerPort, osidbg::DebugMessageHandler::ProtocolVersion);
			} catch (std::exception & e) {
				ERR("Osiris debugger start failed: %s", e.what());
			}
		}
	}
#endif
}

void ScriptExtender::OnInitGame(void * Osiris)
{
	DEBUG("ScriptExtender::OnInitGame()");
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_) {
		debugger_->GameInitHook();
	}
#endif
}

void ScriptExtender::OnDeleteAllData(void * Osiris, bool DeleteTypes)
{
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_) {
		DEBUG("ScriptExtender::OnDeleteAllData()");
		debugger_->DeleteAllDataHook();
		debugger_.reset();
	}
#endif
}

void ScriptExtender::OnError(char const * Message)
{
	ERR("Osiris Error: %s", Message);
}

void ScriptExtender::OnAssert(bool Successful, char const * Message, bool Unknown2)
{
	if (!Successful) {
		WARN("Osiris Assert: %s", Message);
	}
}

bool ScriptExtender::CompileWrapper(std::function<bool(void *, wchar_t const *, wchar_t const *)> const & Next, void * Osiris, wchar_t const * Path, wchar_t const * Mode)
{
	DEBUG(L"ScriptExtender::CompileWrapper: Starting compilation of '%s'", Path);
	auto OriginalFlags = *Wrappers.Globals.DebugFlags;
	std::wstring storyPath;

	if (extensionsEnabled_) {
		CustomFunctions.PreProcessStory(Path);
	}

	if (config_.LogCompile || config_.LogFailedCompile) {
		if (!config_.LogCompile) {
			*Wrappers.Globals.DebugFlags = (DebugFlag)(OriginalFlags & ~DebugFlag::DF_CompileTrace);
		}

		RestartLogging(L"Compile");

		if (config_.LogCompile) {
			storyPath = MakeLogFilePath(L"Compile", L"div");
			CopyFileW(Path, storyPath.c_str(), TRUE);
		}
	}

	auto ret = Next(Osiris, Path, Mode);

	if (ret) {
		DEBUG("ScriptExtender::CompileWrapper: Success.");
	} else {
		ERR("ScriptExtender::CompileWrapper: Compilation FAILED.");
	}

	if (config_.LogCompile || config_.LogFailedCompile) {
		*Wrappers.Globals.DebugFlags = OriginalFlags;
		Wrappers.CloseLogFile.CallOriginal(DynGlobals.OsirisObject);

		if (ret) {
			if (config_.LogCompile) {
				DeleteFileW(storyPath.c_str());
			} else if (!LogFilename.empty()) {
				DeleteFileW(LogFilename.c_str());
			}
		}
	}

	return ret;
}

void ScriptExtender::OnAfterOsirisLoad(void * Osiris, void * Buf, int retval)
{
	std::lock_guard _(storyLoadLock_);

#if !defined(OSI_NO_DEBUGGER)
	if (debuggerThread_ != nullptr) {
		HookNodeVMTs();
	}
#endif

	StoryLoaded = true; 
	DEBUG("ScriptExtender::OnAfterOsirisLoad: %d nodes", (*Wrappers.Globals.Nodes)->Db.Size);

#if !defined(OSI_NO_DEBUGGER)
	if (debuggerThread_ != nullptr && gNodeVMTWrappers) {
		debugger_.reset();
		debugger_ = std::make_unique<osidbg::Debugger>(Wrappers.Globals, std::ref(*debugMsgHandler_));
		debugger_->StoryLoaded();
	}
#endif

	if (extensionsEnabled_) {
		esv::ExtensionState::Get().StoryLoaded();
	}
}*/

// FIXME - move this to client state!
void ScriptExtender::OnInitNetworkFixedStrings(eoc::NetworkFixedStrings * self, void * arg1)
{
	if (config_.DumpNetworkStrings) {
		// client_.GetNetworkFixedStrings().Dump();
	}

	if (client_.IsInClientThread()) {
		client_.GetNetworkFixedStrings().UpdateFromServer();
	}
}

void ScriptExtender::OnStatsLoadStarted(CRPGStatsManager* mgr)
{
	statLoadOrderHelper_.OnLoadStarted();
}

void ScriptExtender::OnStatsLoadFinished(CRPGStatsManager* mgr)
{
	statLoadOrderHelper_.OnLoadFinished();
	auto state = GetCurrentExtensionState();
	if (state) {
		state->OnStatsLoaded();
	}
}

/*bool ScriptExtender::MergeWrapper(std::function<bool (void *, wchar_t *)> const & Next, void * Osiris, wchar_t * Src)
{
	DEBUG("ScriptExtender::MergeWrapper() - Started merge");

	if (ServerExtState) {
		ServerExtState->StorySetMerging(true);
	}

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->MergeStarted();
	}
#endif

	bool retval = Next(Osiris, Src);

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->MergeFinished();
	}
#endif

	if (ServerExtState) {
		ServerExtState->StorySetMerging(false);
	}

	DEBUG("ScriptExtender::MergeWrapper() - Finished merge");
	return retval;
}

void ScriptExtender::RuleActionCall(std::function<void (RuleActionNode *, void *, void *, void *, void *)> const & Next, RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4)
{
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->RuleActionPreHook(Action);
	}
#endif

	Next(Action, a1, a2, a3, a4);

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->RuleActionPostHook(Action);
	}
#endif
}*/

bool ScriptExtender::HasFeatureFlag(char const * flag) const
{
	return (server_.HasExtensionState() && server_.GetExtensionState().HasFeatureFlag(flag))
		|| (client_.HasExtensionState() && client_.GetExtensionState().HasFeatureFlag(flag));
}

ExtensionStateBase* ScriptExtender::GetCurrentExtensionState()
{
	if (server_.IsInServerThread()) {
		if (server_.HasExtensionState()) {
			return &server_.GetExtensionState();
		} else {
			return nullptr;
		}
	} else if (client_.IsInClientThread()) {
		if (client_.HasExtensionState()) {
			return &client_.GetExtensionState();
		} else {
			return nullptr;
		}
	} else {
		WARN("Called from unknown thread %d?", GetCurrentThreadId());
		if (client_.HasExtensionState()) {
			return &client_.GetExtensionState();
		} else {
			return nullptr;
		}
	}
}

void ScriptExtender::OnBaseModuleLoaded(void * self)
{
}

#pragma message("Move event separatelyy to client/server")
/*
void ScriptExtender::OnModuleLoadStarted(TranslatedStringRepository* self)
{
	LoadExtensionStateClient();
	if (ClientExtState) {
		ClientExtState->OnModuleLoadStarted();
	}
}
*/

void ScriptExtender::OnSkillPrototypeManagerInit(void * self)
{
	if (!extensionsEnabled_) return;

	std::lock_guard _(globalStateLock_);
	auto extState = GetCurrentExtensionState();
	if (extState == nullptr) {
		ERR("Extension not initialized in OnSkillPrototypeManagerInit?");
		return;
	}

	auto modManager = extState->GetModManager();
	if (modManager == nullptr) {
		ERR("Module info not available in OnSkillPrototypeManagerInit?");
		return;
	}

	STDWString loadMsg = L"Loading ";
	loadMsg += modManager->BaseModule.Info.Name;
	loadMsg += L" (Script Extender v";
	loadMsg += std::to_wstring(CurrentVersion);
#if defined(_DEBUG)
	loadMsg += L" Devel";
#endif
	loadMsg += L")";
	Libraries.ShowStartupMessage(loadMsg, false);
	
	// FIXME - remove this hook, should be done in extension state!
	if (server_.IsInServerThread()) {
		server_.LoadExtensionState();
	} else {
		client_.LoadExtensionState();
	}

	extState->OnModuleLoading();
}

void ScriptExtender::ClearPathOverrides()
{
	std::unique_lock lock(pathOverrideMutex_);
	pathOverrides_.clear();
}

void ScriptExtender::AddPathOverride(STDString const & path, STDString const & overriddenPath)
{
	auto absolutePath = GetStaticSymbols().ToPath(path, PathRootType::Data);
	auto absoluteOverriddenPath = GetStaticSymbols().ToPath(overriddenPath, PathRootType::Data);

	std::unique_lock lock(pathOverrideMutex_);
	pathOverrides_.insert(std::make_pair(absolutePath, absoluteOverriddenPath));
}

std::optional<STDString> ScriptExtender::GetPathOverride(STDString const & path)
{
	auto absolutePath = GetStaticSymbols().ToPath(path, PathRootType::Data);

	std::unique_lock lock(pathOverrideMutex_);
	auto it = pathOverrides_.find(absolutePath);
	if (it != pathOverrides_.end()) {
		return it->second;
	} else {
		return {};
	}
}

FileReader * ScriptExtender::OnFileReaderCreate(FileReader::CtorProc* next, FileReader * self, Path const& path, unsigned int type)
{
	if (!pathOverrides_.empty()) {
		std::shared_lock lock(pathOverrideMutex_);
		auto it = pathOverrides_.find(path.Name);
		if (it != pathOverrides_.end() && !hasher_.isHashing()) {
			DEBUG("FileReader path override: %s -> %s", path.Name.c_str(), it->second.c_str());
			Path overriddenPath;
			overriddenPath.Name = it->second;
#if !defined(OSI_EOCAPP)
			overriddenPath.Unknown = path->Unknown;
#endif
			lock.unlock();
			return next(self, overriddenPath, type);
		}
	}

	if (path.Name.size() > 4 
		&& memcmp(path.Name.data() + path.Name.size() - 4, ".txt", 4) == 0) {
		statLoadOrderHelper_.OnStatFileOpened(path);
	}

	DisableCrashReporting _;
	return next(self, path, type);
}

void ScriptExtender::OnSavegameVisit(void* osirisHelpers, ObjectVisitor* visitor)
{
	savegameSerializer_.SavegameVisit(visitor);
}

void ScriptExtender::PostStartup()
{
	if (postStartupDone_) return;

	if (Libraries.PostStartupFindLibraries()) {
		gExtender->GetServer().Osiris().GetWrappers().InitializeDeferredExtensions();
		// FunctionLibrary.PostStartup();
		hooks_.HookAll();
		server_.PostStartup();
		client_.PostStartup();
		hitProxy_.PostStartup();
		hasher_.PostStartup();

		using namespace std::placeholders;
		hooks_.FileReader__ctor.SetWrapper(std::bind(&ScriptExtender::OnFileReaderCreate, this, _1, _2, _3, _4));
	}

	GameVersionInfo gameVersion;
	if (Libraries.GetGameVersion(gameVersion) && !gameVersion.IsSupported()) {
		std::wstringstream ss;
		ss << L"Your game version (v" << gameVersion.Major << L"." << gameVersion.Minor << L"." << gameVersion.Revision << L"." << gameVersion.Build
			<< L") is not supported by the Script Extender; please upgrade to at least v3.6.54";
		Libraries.ShowStartupError(ss.str().c_str(), true, false);
	} else if (Libraries.CriticalInitializationFailed()) {
		Libraries.ShowStartupError(L"A severe error has occurred during Script Extender initialization. Extension features will be unavailable.", true, false);
	} else if (Libraries.InitializationFailed()) {
		Libraries.ShowStartupError(L"An error has occurred during Script Extender initialization. Some extension features might be unavailable.", true, false);
	}

	postStartupDone_ = true;
}

void ScriptExtender::InitRuntimeLogging()
{
	if (!config_.LogRuntime) return;

	auto path = MakeLogFilePath(L"Extender Runtime", L"log");
	gConsole.OpenLogFile(path);
	DEBUG(L"Extender runtime log written to '%s'", path.c_str());
}


}
