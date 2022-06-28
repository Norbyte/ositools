#include <stdafx.h>
#include <Extender/Server/ScriptExtenderServer.h>
#include <Extender/ScriptExtender.h>

#define STATIC_HOOK(name) decltype(dse::esv::ScriptExtender::name) * decltype(dse::esv::ScriptExtender::name)::gHook;
STATIC_HOOK(gameStateWorkerStart_)
STATIC_HOOK(gameStateChangedEvent_)
STATIC_HOOK(gameStateMachineUpdate_)

BEGIN_NS(esv)

char const* GameStateNames[] =
{
	"Unknown",
	"Uninitialized",
	"Init",
	"Idle",
	"Exit",
	"LoadLevel",
	"LoadModule",
	"LoadGMCampaign",
	"LoadSession",
	"UnloadLevel",
	"UnloadModule",
	"UnloadSession",
	"Sync",
	"Paused",
	"Running",
	"Save",
	"Disconnect",
	"GameMasterPause",
	"BuildStory",
	"ReloadStory",
	"Installation"
};


ScriptExtender::ScriptExtender(ExtenderConfig& config)
	: osiris_(config),
	networkFixedStrings_(network_)
{
}

void ScriptExtender::Initialize()
{
	ResetExtensionState();

	auto& lib = GetStaticSymbols();

	if (!gExtender->GetLibraryManager().CriticalInitializationFailed()) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		if (lib.esv__GameStateEventManager__ExecuteGameStateChangedEvent != nullptr) {
			gameStateChangedEvent_.Wrap(lib.esv__GameStateEventManager__ExecuteGameStateChangedEvent);
		}

		if (lib.esv__GameStateThreaded__GameStateWorker__DoWork != nullptr) {
			gameStateWorkerStart_.Wrap(lib.esv__GameStateThreaded__GameStateWorker__DoWork);
		}

		if (lib.esv__GameStateMachine__Update != nullptr) {
			gameStateMachineUpdate_.Wrap(lib.esv__GameStateMachine__Update);
		}

		DetourTransactionCommit();

		using namespace std::placeholders;
		gameStateChangedEvent_.SetPostHook(std::bind(&ScriptExtender::OnGameStateChanged, this, _1, _2, _3));
		gameStateWorkerStart_.AddPreHook(std::bind(&ScriptExtender::OnGameStateWorkerStart, this, _1));
		gameStateWorkerStart_.AddPostHook(std::bind(&ScriptExtender::OnGameStateWorkerExit, this, _1));
		gameStateMachineUpdate_.AddPostHook(std::bind(&ScriptExtender::OnUpdate, this, _1, _2));
	}
}

void ScriptExtender::Shutdown()
{
	ResetExtensionState();
	osiris_.Shutdown();
}

void ScriptExtender::PostStartup()
{
	// We need to initialize the function library here, as GlobalAllocator isn't available in Init().
	if (!gExtender->GetLibraryManager().CriticalInitializationFailed()) {
		osiris_.Initialize();
	}

	// entityHelpers_.Setup();
}

void ScriptExtender::OnGameStateChanged(void * self, GameState fromState, GameState toState)
{
#if defined(DEBUG_SERVER_CLIENT)
		DEBUG("ScriptExtender::OnServerGameStateChanged(): %s -> %s", 
			ServerGameStateNames[(unsigned)fromState], ServerGameStateNames[(unsigned)toState]);
#endif

	if (fromState != GameState::Unknown) {
		AddThread(GetCurrentThreadId());
	}

	switch (fromState) {
	case GameState::LoadModule:
		INFO("ScriptExtender::OnServerGameStateChanged(): Loaded module");
		LoadExtensionState();
		break;

	case GameState::LoadSession:
		if (extensionState_) {
			extensionState_->OnGameSessionLoaded();
		}
		break;
	}

	switch (toState) {
	// Gift Bag selection in the main menu reloads the module without reconnecting
	// We only need to reset the extender enabled peer list on a disconnect.
	case GameState::Disconnect:
		network_.Reset();
		break;

	case GameState::UnloadSession:
		INFO("ScriptExtender::OnServerGameStateChanged(): Unloading session");
		ResetExtensionState();
		break;

	case GameState::LoadModule:
		network_.ExtendNetworking();
		break;

	case GameState::LoadGMCampaign:
		INFO("ScriptExtender::OnServerGameStateChanged(): Loading GM campaign");
		LoadExtensionState();
		network_.ExtendNetworking();
		break;

	case GameState::LoadSession:
		INFO("ScriptExtender::OnServerGameStateChanged(): Loading game session");
		LoadExtensionState();
		network_.ExtendNetworking();
		networkFixedStrings_.FlushQueuedRequests();
		if (extensionState_) {
			extensionState_->OnGameSessionLoading();
		}
		break;
	}

	LuaServerPin lua(ExtensionState::Get());
	if (lua) {
		lua->OnGameStateChanged(fromState, toState);
	}
}

void ScriptExtender::OnGameStateWorkerStart(void * self)
{
	AddThread(GetCurrentThreadId());
}

void ScriptExtender::OnGameStateWorkerExit(void* self)
{
	RemoveThread(GetCurrentThreadId());
}

void ScriptExtender::OnUpdate(void* self, GameTime* time)
{
	RunPendingTasks();
	if (extensionState_) {
		extensionState_->OnUpdate(*time);
	}
}

bool ScriptExtender::IsInServerThread() const
{
	return IsInThread();
}

void ScriptExtender::ResetLuaState()
{
	if (extensionState_ && extensionState_->GetLua()) {

		auto ext = extensionState_.get();
		ext->AddPostResetCallback([ext]() {
			ext->OnModuleResume();
			auto state = GetStaticSymbols().GetServerState();
			if (state && (state == GameState::Paused || state == GameState::Running || state == GameState::GameMasterPause)) {
				ext->OnGameSessionLoading();
				ext->OnGameSessionLoaded();
				ext->StoryLoaded();
				ext->OnResetCompleted();
			}
		});
		ext->LuaReset(true);
	}
}

bool ScriptExtender::RequestResetClientLuaState()
{
	// FIXME - eliminate direct references to esv__EoCServer
	auto server = GetStaticSymbols().esv__EoCServer;
	if (server && *server && (*server)->GameServer) {
		// Reset clients via a network message if the server is running
		auto msg = network_.GetFreeMessage(ReservedUserId);
		if (msg != nullptr) {
			auto resetMsg = msg->GetMessage().mutable_s2c_reset_lua();
			resetMsg->set_bootstrap_scripts(true);
			network_.Broadcast(msg, ReservedUserId);
			return true;
		} else {
			OsiErrorS("Could not get free message!");
			return false;
		}
	} else {
		return false;
	}
}

void ScriptExtender::ResetExtensionState()
{
	extensionState_ = std::make_unique<ExtensionState>();
	extensionState_->Reset();
	extensionLoaded_ = false;
}

void ScriptExtender::LoadExtensionState()
{
	if (extensionLoaded_) return;

	PostStartup();

	if (!extensionState_) {
		ResetExtensionState();
	}

	extensionState_->LoadConfigs();

	// PostInitLibraries() should be called after extension config is loaded,
	// otherwise it won't hook functions that may be needed later on
	#pragma message("still needed?")
	// PostInitLibraries();

	if (!gExtender->GetLibraryManager().CriticalInitializationFailed()) {
		gExtender->GetLibraryManager().EnableCustomStats();
		gExtender->GetLibraryManager().DisableItemFolding();
		network_.ExtendNetworking();
		osiris_.GetCustomFunctionManager().ClearDynamicEntries();
		extensionState_->LuaReset(true);
	}

	extensionLoaded_ = true;
}

END_NS()