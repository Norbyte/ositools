#include <stdafx.h>
#include <Extender/Client/ScriptExtenderClient.h>
#include <Extender/ScriptExtender.h>
#include <Version.h>

#include <Extender/Client/StatusHelpers.inl>

#define STATIC_HOOK(name) decltype(dse::ecl::ScriptExtender::name) * decltype(dse::ecl::ScriptExtender::name)::gHook;
STATIC_HOOK(gameStateWorkerStart_)
STATIC_HOOK(gameStateChangedEvent_)
STATIC_HOOK(gameStateMachineUpdate_)
STATIC_HOOK(gameStateLoadIncLocalProgress_)

#include <Extender/Shared/ThreadedExtenderState.inl>
#include <Extender/Shared/ModuleHasher.inl>

BEGIN_SE()

void InitCrashReporting();
void ShutdownCrashReporting();

END_SE()

BEGIN_NS(ecl)

char const* ClientGameStateNames[] =
{
	"Unknown",
	"Init",
	"InitMenu",
	"InitNetwork",
	"InitConnection",
	"Idle",
	"LoadMenu",
	"Menu",
	"Exit",
	"SwapLevel",
	"LoadLevel",
	"LoadModule",
	"LoadSession",
	"LoadGMCampaign",
	"UnloadLevel",
	"UnloadModule",
	"UnloadSession",
	"Paused",
	"PrepareRunning",
	"Running",
	"Disconnect",
	"Join",
	"Save",
	"StartLoading",
	"StopLoading",
	"StartServer",
	"Movie",
	"Installation",
	"GameMasterPause",
	"ModReceiving",
	"Lobby",
	"BuildStory",
	"LoadLoca"
};

ScriptExtender::ScriptExtender(ExtenderConfig& config)
	: config_(config),
	networkFixedStrings_(network_)
{
}

void ScriptExtender::Initialize()
{
	// Wrap state change functions even if extension startup failed, otherwise
	// we won't be able to show any startup errors

	auto& lib = GetStaticSymbols();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	if (lib.ecl__GameStateEventManager__ExecuteGameStateChangedEvent != nullptr) {
		gameStateChangedEvent_.Wrap(lib.ecl__GameStateEventManager__ExecuteGameStateChangedEvent);
	}

	if (lib.ecl__GameStateThreaded__GameStateWorker__DoWork != nullptr) {
		gameStateWorkerStart_.Wrap(lib.ecl__GameStateThreaded__GameStateWorker__DoWork);
	}

	if (lib.ecl__GameStateMachine__Update != nullptr) {
		gameStateMachineUpdate_.Wrap(lib.ecl__GameStateMachine__Update);
	}

	if (lib.ecl__GameStateLoad__IncLocalProgress != nullptr) {
		gameStateLoadIncLocalProgress_.Wrap(lib.ecl__GameStateLoad__IncLocalProgress);
	}

	DetourTransactionCommit();

	using namespace std::placeholders;
	gameStateChangedEvent_.SetPostHook(std::bind(&ScriptExtender::OnGameStateChanged, this, _1, _2, _3));
	gameStateWorkerStart_.AddPreHook(std::bind(&ScriptExtender::OnGameStateWorkerStart, this, _1));
	gameStateWorkerStart_.AddPostHook(std::bind(&ScriptExtender::OnGameStateWorkerExit, this, _1));
	gameStateMachineUpdate_.SetPostHook(std::bind(&ScriptExtender::OnUpdate, this, _1, _2));
	gameStateLoadIncLocalProgress_.SetPostHook(std::bind(&ScriptExtender::OnIncLocalProgress, this, _1, _2, _3));
}

void ScriptExtender::Shutdown()
{
	DEBUG("ecl::ScriptExtender::Shutdown: Exiting");
	ResetExtensionState();
}

void ScriptExtender::PostStartup()
{
	if (postStartupDone_) return;

	statusHelpers_.PostStartup();
	postStartupDone_ = true;
}

bool IsLoadingState(GameState state)
{
	return state == GameState::Init
		|| state == GameState::InitMenu
		|| state == GameState::InitNetwork
		|| state == GameState::InitConnection
		|| state == GameState::LoadMenu
		|| state == GameState::SwapLevel
		|| state == GameState::LoadLevel
		|| state == GameState::LoadModule
		|| state == GameState::LoadSession
		|| state == GameState::LoadGMCampaign
		|| state == GameState::UnloadLevel
		|| state == GameState::UnloadModule
		|| state == GameState::UnloadSession
		|| state == GameState::PrepareRunning
		|| state == GameState::Installation
		|| state == GameState::ModReceiving;
}

void ScriptExtender::OnGameStateChanged(void* self, GameState fromState, GameState toState)
{
	if (config_.SendCrashReports) {
		// We need to initialize the crash reporter after the game engine has started,
		// otherwise the game will overwrite the top level exception filter
		InitCrashReporting();
	}

	// Check to make sure that startup is done even if the extender was loaded when the game was already in GameState::Init
	if (toState != GameState::Unknown
		&& toState != GameState::StartLoading
		&& toState != GameState::InitMenu
		&& !gExtender->GetLibraryManager().InitializationFailed()) {
		// We need to initialize the function library here, as GlobalAllocator isn't available in Init().
		gExtender->PostStartup();
	}

	if (toState == GameState::Menu
		&& gExtender->GetLibraryManager().InitializationFailed()) {
		gExtender->PostStartup();
	}

	if (toState == GameState::LoadModule && config_.DisableModValidation) {
		if (GetStaticSymbols().GetGlobalSwitches()) {
			GetStaticSymbols().GetGlobalSwitches()->EnableModuleHashing = false;
			INFO("Disabled module hashing");
		} else {
			WARN("Could not disable mod hashing - GlobalSwitches not mapped");
		}
	}

#if defined(DEBUG_SERVER_CLIENT)
	DEBUG("ecl::ScriptExtender::OnClientGameStateChanged(): %s -> %s",
		ClientGameStateNames[(unsigned)fromState], ClientGameStateNames[(unsigned)toState]);
#endif

	if (fromState != GameState::Unknown) {
		AddThread(GetCurrentThreadId());
	}

	if (gExtender->WasInitialized()) {
		if (IsLoadingState(toState)) {
			UpdateClientProgress(FromUTF8(EnumInfo<GameState>::Find(toState).GetStringOrDefault()));
		} else {
			UpdateClientProgress(L"");
		}
	}

	switch (fromState) {
	case GameState::LoadModule:
		INFO("ecl::ScriptExtender::OnClientGameStateChanged(): Loaded module");
		LoadExtensionState(ExtensionStateContext::Game);
		break;

	// Initialize client state when exiting from a game and returning to menu
	case GameState::LoadMenu:
		LoadExtensionState(ExtensionStateContext::Game);
		break;

	case GameState::LoadSession:
		if (extensionState_) {
			extensionState_->OnGameSessionLoaded();
		}
		break;

	case GameState::InitConnection:
		network_.ExtendNetworking();
		networkFixedStrings_.RequestFromServer();
		break;
	}

	switch (toState) {
	case GameState::Init:
		ResetExtensionState();
		break;

	case GameState::InitNetwork:
	case GameState::Disconnect:
		network_.Reset();
		networkFixedStrings_.ClientReset();
		break;

	case GameState::UnloadModule:
		// Clear stored NetworkFixedString updates from previous session
		// Server will send a new list when it enters LoadModule state
		networkFixedStrings_.ClientReset();
		gExtender->Hasher().ClearCaches();
		break;

	case GameState::UnloadSession:
		INFO("ecl::ScriptExtender::OnClientGameStateChanged(): Unloading session");
		ResetExtensionState();
		break;

	case GameState::LoadModule:
		gExtender->InitRuntimeLogging();
		if (config_.DeveloperMode) {
			RegisterFlashTraceCallbacks();
		}
		break;

	case GameState::LoadGMCampaign:
		INFO("ecl::ScriptExtender::OnClientGameStateChanged(): Loading GM campaign");
		LoadExtensionState(ExtensionStateContext::Game);
		network_.ExtendNetworking();
		break;

	case GameState::LoadSession:
		INFO("ecl::ScriptExtender::OnClientGameStateChanged(): Loading game session");
		LoadExtensionState(ExtensionStateContext::Game);
		network_.ExtendNetworking();
		if (extensionState_) {
			extensionState_->OnGameSessionLoading();
		}

		if (config_.DeveloperMode) {
			RegisterFlashTraceCallbacks();
		}
		break;

	case GameState::LoadLevel:
		if (extensionState_ && extensionState_->GetLua()) {
			extensionState_->GetLua()->OnLevelLoading();
		}
		break;

	case GameState::Running:
		if (fromState == GameState::PrepareRunning) {
			networkFixedStrings_.ClientLoaded();
		}
		break;
	}

	if (extensionState_) {
		LuaClientPin lua(*extensionState_);
		if (lua) {
			lua->OnGameStateChanged(fromState, toState);
		}
	}
}


void ScriptExtender::OnGameStateWorkerStart(void* self)
{
	AddThread(GetCurrentThreadId());
}

void ScriptExtender::OnGameStateWorkerExit(void* self)
{
	AddThread(GetCurrentThreadId());
}

void ScriptExtender::OnUpdate(void* self, GameTime* time)
{
	RunPendingTasks();
	if (extensionState_) {
		extensionState_->OnUpdate(*time);
	}
}

void ScriptExtender::OnIncLocalProgress(void* self, int progress, char const* state)
{
	if (strcmp(state, "EffectManager") != 0) {
		UpdateClientProgress(FromUTF8(state));
	} else {
		UpdateClientProgress(L"");
	}
}

void ScriptExtender::UpdateServerProgress(STDWString const& status)
{
	serverStatus_ = status;
	ShowLoadingProgress();
}

void ScriptExtender::UpdateClientProgress(STDWString const& status)
{
	clientStatus_ = status;
	ShowLoadingProgress();
}

void ScriptExtender::ShowLoadingProgress()
{
	auto uiMgr = GetStaticSymbols().GetUIObjectManager();
	if (!uiMgr) return;
	auto loadingScreenUi = (UILoadingScreen*)uiMgr->GetByType(23);
	if (!loadingScreenUi || (loadingScreenUi->Flags & UIObjectFlags::OF_Visible) != UIObjectFlags::OF_Visible) return;

	STDWString statusText;

	if (!clientStatus_.empty()) {
		statusText += L"[C] ";
		statusText += clientStatus_;
	} else if (!serverStatus_.empty()) {
		statusText += L"[S] ";
		statusText += serverStatus_;
	}

	statusText += L" (Script Extender v";
	statusText += std::to_wstring(CurrentVersion);
#if defined(_DEBUG)
	statusText += L" Devel";
#endif
	statusText += L")";
	loadingScreenUi->NextStatusText = statusText;
}

bool ScriptExtender::IsInClientThread() const
{
	return IsInThread();
}


void ScriptExtender::ResetLuaState()
{
	if (extensionState_ && extensionState_->GetLua()) {
		// Do a direct (local) reset if server is not available (main menu, etc.)
		auto ext = extensionState_.get();

		ext->AddPostResetCallback([&ext]() {
			ext->OnModuleResume();
			auto state = GetStaticSymbols().GetClientState();
			if (state && (state == GameState::Paused || state == GameState::Running || state == GameState::GameMasterPause)) {
				ext->OnGameSessionLoading();
				ext->OnGameSessionLoaded();
				ext->OnResetCompleted();
			}
		});
		ext->LuaReset(true);
	}
}

void ScriptExtender::ResetExtensionState()
{
	extensionState_ = std::make_unique<ExtensionState>();
	extensionState_->Reset();
	gExtender->ClearPathOverrides();
	extensionLoaded_ = false;
}

void ScriptExtender::LoadExtensionState(ExtensionStateContext ctx)
{
	if (extensionLoaded_ && (!extensionState_ || ctx == extensionState_->Context())) {
		return;
	}

	PostStartup();

	if (!extensionState_) {
		ResetExtensionState();
	}

	extensionState_->LoadConfigs();

	if (!gExtender->GetLibraryManager().CriticalInitializationFailed()) {
		OsiMsg("Initializing client with target context " << ContextToString(ctx));
		gExtender->GetLibraryManager().ApplyCodePatches();
		network_.ExtendNetworking();
		extensionState_->LuaReset(ctx, true);
	}

	extensionLoaded_ = true;
}

void ScriptExtender::RegisterFlashTraceCallbacks()
{
	auto const& sym = GetStaticSymbols();
	if (sym.IgSetWarningCallback != nullptr) {
		sym.IgSetWarningCallback(&FlashWarningCallback, nullptr);
	}

	if (sym.IgSetTraceCallbackUTF8 != nullptr) {
		sym.IgSetTraceCallbackUTF8(&FlashTraceCallback, nullptr);
	}
}

void ScriptExtender::FlashTraceCallback(void * ctx, void * player, char const * message)
{
	DEBUG("Flash: %s", message);
}

void ScriptExtender::FlashWarningCallback(void * ctx, void * player, int code, char const * message)
{
	if (code == 201 || code == 408) {
		return;
	}

	if (code == 503) {
		STDString errmsg(message);
		if (errmsg.find("onEventResolution") != std::string::npos
			|| errmsg.find("onEventResize") != std::string::npos) {
			return;
		}
	}

	WARN("Flash: (%d) %s", code, message);
}

END_NS()
