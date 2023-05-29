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
#include <Extender/Shared/CustomDamageTypes.inl>
#include <Extender/Shared/CustomRequirements.inl>
#include <Extender/Shared/CustomConditions.inl>
#include <Extender/Shared/CustomSkills.inl>
#include <Extender/Shared/UserVariables.inl>

#undef DEBUG_SERVER_CLIENT

BEGIN_SE()

void InitCrashReporting();
void ShutdownCrashReporting();

END_SE()

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
	: server_(config_),
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
			Fail(L"Script Extender doesn't support game versions below v3.6.117, please upgrade!");
		}
	} else {
		ERR("Failed to retrieve game version info.");
	}

	DEBUG("ScriptExtender::Initialize: Starting");
	auto initStart = std::chrono::high_resolution_clock::now();
	
	InitStaticSymbols();

	if (!Libraries.FindLibraries(gameVersion.Revision)) {
		ERR("ScriptExtender::Initialize: Could not load libraries; skipping scripting extension initialization.");
		extensionsEnabled_ = false;
	}

	server_.Initialize();
	client_.Initialize();

	// FIXME - SEPARATE FROM OSI!
	auto& Wrappers = gExtender->GetServer().Osiris().GetWrappers();
	// Wrap state change functions even if extension startup failed, otherwise
	// we won't be able to show any startup errors
	Wrappers.InitializeExtensions();
	Wrappers.InitNetworkFixedStrings.SetPostHook(&ScriptExtender::OnInitNetworkFixedStrings, this);
	Wrappers.SkillPrototypeManagerInit.SetPreHook(&ScriptExtender::OnSkillPrototypeManagerInit, this);
	Wrappers.esv__OsirisVariableHelper__SavegameVisit.SetPreHook(&ScriptExtender::OnSavegameVisit, this);

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
	engineHooks_.UnhookAll();

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

void ScriptExtender::OnStatsLoad(stats::RPGStats::LoadProc* wrapped, stats::RPGStats* mgr, ObjectSet<STDString>* paths)
{
	statLoadOrderHelper_.OnLoadStarted();

	if (client_.IsInClientThread()) {
		client_.LoadExtensionState(ExtensionStateContext::Load);
	} else if (server_.IsInServerThread()) {
		server_.LoadExtensionState(ExtensionStateContext::Load);
	}

	wrapped(mgr, paths);

	statLoadOrderHelper_.OnLoadFinished();
	if (client_.IsInClientThread()) {
		client_.LoadExtensionState(ExtensionStateContext::Game);
	} else if (server_.IsInServerThread()) {
		server_.LoadExtensionState(ExtensionStateContext::Game);
	}

	auto state = GetCurrentExtensionState();
	if (state) {
		state->OnStatsLoaded();
	}
}

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
	PostStartup();

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
	
	if (server_.IsInServerThread()) {
		server_.LoadExtensionState(ExtensionStateContext::Game);
	} else {
		client_.LoadExtensionState(ExtensionStateContext::Game);
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
			overriddenPath.Unknown = path.Unknown;
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
	server_.OnSavegameVisit(visitor);
}

void ScriptExtender::PostStartup()
{
	if (postStartupDone_) return;

	if (Libraries.PostStartupFindLibraries()) {
		gExtender->GetServer().Osiris().GetWrappers().InitializeDeferredExtensions();
		// FunctionLibrary.PostStartup();
		engineHooks_.HookAll();
		if (!gExtender->GetLibraryManager().CriticalInitializationFailed()) {
			hooks_.Startup();
		}

		server_.PostStartup();
		client_.PostStartup();
		hitProxy_.PostStartup();
		hasher_.PostStartup();

		luaBuiltinBundle_.SetResourcePath(config_.LuaBuiltinResourceDirectory);
		if (!luaBuiltinBundle_.LoadBuiltinResource(IDR_LUA_BUILTIN_BUNDLE)) {
			ERR("Failed to load Lua builtin resource bundle!");
		}

		engineHooks_.FileReader__ctor.SetWrapper(&ScriptExtender::OnFileReaderCreate, this);
		engineHooks_.RPGStats__Load.SetWrapper(&ScriptExtender::OnStatsLoad, this);
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
