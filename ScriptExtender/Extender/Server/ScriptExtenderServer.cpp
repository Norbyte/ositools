#include <stdafx.h>
#include <Extender/Server/ScriptExtenderServer.h>
#include <Extender/ScriptExtender.h>

#include <Extender/Server/StatusHelpers.inl>
#include <Extender/Server/CustomSkills.inl>

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

		gameStateChangedEvent_.SetPostHook(&ScriptExtender::OnGameStateChanged, this);
		gameStateWorkerStart_.SetWrapper(&ScriptExtender::OnGameStateWorkerRun, this);
		gameStateMachineUpdate_.SetPostHook(&ScriptExtender::OnUpdate, this);
	}
}

void ScriptExtender::Shutdown()
{
	ResetExtensionState();
	osiris_.Shutdown();
}

void ScriptExtender::PostStartup()
{
	if (postStartupDone_) return;

	// We need to initialize the function library here, as GlobalAllocator isn't available in Init().
	if (!gExtender->GetLibraryManager().CriticalInitializationFailed()) {
		osiris_.Initialize();
	}

	statusHelpers_.PostStartup();

	postStartupDone_ = true;
}

bool IsLoadingState(GameState state)
{
	return state == GameState::Init
		|| state == GameState::LoadLevel
		|| state == GameState::LoadModule
		|| state == GameState::LoadSession
		|| state == GameState::UnloadLevel
		|| state == GameState::UnloadModule
		|| state == GameState::UnloadSession
		|| state == GameState::Sync
		|| state == GameState::BuildStory
		|| state == GameState::ReloadStory
		|| state == GameState::Installation;
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
		LoadExtensionState(ExtensionStateContext::Game);
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
		LoadExtensionState(ExtensionStateContext::Game);
		network_.ExtendNetworking();
		break;

	case GameState::LoadSession:
		INFO("ScriptExtender::OnServerGameStateChanged(): Loading game session");
		LoadExtensionState(ExtensionStateContext::Game);
		network_.ExtendNetworking();
		networkFixedStrings_.FlushQueuedRequests();
		if (extensionState_) {
			extensionState_->OnGameSessionLoading();
		}
		break;

	case GameState::LoadLevel:
		if (extensionState_ && extensionState_->GetLua()) {
			extensionState_->GetLua()->OnLevelLoading();
		}
		break;
	}

	if (gExtender->WasInitialized()) {
		if (IsLoadingState(toState)) {
			gExtender->GetClient().UpdateServerProgress(FromUTF8(EnumInfo<GameState>::Find(toState).GetStringOrDefault()));
		} else {
			gExtender->GetClient().UpdateServerProgress(L"");
		}
	}

	if (extensionState_) {
		LuaServerPin lua(*extensionState_);
		if (lua) {
			lua->OnGameStateChanged(fromState, toState);
		}
	}
}

void ScriptExtender::OnGameStateWorkerRun(void (*wrapped)(void*), void* self)
{
	AddThread(GetCurrentThreadId());
	wrapped(self);
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
		OsiMsg("Initializing server with target context " << ContextToString(ctx));
		gExtender->GetLibraryManager().ApplyCodePatches();
		customSkillVmts_.Initialize();
		network_.ExtendNetworking();
		osiris_.GetCustomFunctionManager().ClearDynamicEntries();
		extensionState_->GetModVariables().OnModuleLoading();
		extensionState_->LuaReset(ctx, true);
	}

	extensionLoaded_ = true;
}

void ScriptExtender::OnSavegameVisit(ObjectVisitor* visitor)
{
	savegameSerializer_.SavegameVisit(visitor);
}

END_NS()
