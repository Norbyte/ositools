#pragma once

#include <Extender/Shared/ScriptExtenderBase.h>
#include <Extender/Shared/ExtenderConfig.h>
#include <GameDefinitions/Osiris.h>
#include <Extender/Server/ExtensionStateServer.h>
#include <Extender/Server/NetworkManagerServer.h>
#include <Extender/Server/StatusHelpers.h>
#include <Extender/Server/OsirisStatusHelpers.h>
#include <Extender/Shared/SavegameSerializer.h>
#include <GameHooks/OsirisWrappers.h>
#include <GameDefinitions/Symbols.h>
#include <Osiris/OsirisExtender.h>
#include <GameHooks/Wrappers.h>
#include <Osiris/OsirisExtender.h>

BEGIN_NS(esv)

class ScriptExtender : public ThreadedExtenderState
{
public:
	ScriptExtender(ExtenderConfig& config);

	void Initialize();
	void PostStartup();
	void Shutdown();

	inline bool HasExtensionState() const
	{
		return (bool)extensionState_;
	}

	inline ExtensionState & GetExtensionState() const
	{
		return *extensionState_;
	}

	inline NetworkManager& GetNetworkManager()
	{
		return network_;
	}

	inline NetworkFixedStringSender& GetNetworkFixedStrings()
	{
		return networkFixedStrings_;
	}

	inline OsirisExtender& Osiris()
	{
		return osiris_;
	}

	inline StatusHelpers& GetStatusHelpers()
	{
		return statusHelpers_;
	}

	inline OsirisStatusHelpers& GetOsirisStatusHelpers()
	{
		return osirisStatusHelpers_;
	}

	bool IsInServerThread() const;
	void ResetLuaState();
	bool RequestResetClientLuaState();
	void ResetExtensionState();
	void LoadExtensionState(ExtensionStateContext ctx);
	void OnSavegameVisit(ObjectVisitor* visitor);


	enum class GameStateWorkerStartTag {};
	enum class GameStateMachineUpdateTag {};
	enum class GameStateChangedEventTag {};
	HookableFunction<GameStateWorkerStartTag, void(void*)> gameStateWorkerStart_;
	HookableFunction<GameStateMachineUpdateTag, void(void*, GameTime*)> gameStateMachineUpdate_;
	PostHookableFunction<GameStateChangedEventTag, void(void*, GameState, GameState)> gameStateChangedEvent_;

private:
	OsirisExtender osiris_;
	std::unique_ptr<ExtensionState> extensionState_;
	bool extensionLoaded_{ false };
	bool postStartupDone_{ false };
	NetworkManager network_;
	NetworkFixedStringSender networkFixedStrings_;
	StatusHelpers statusHelpers_;
	OsirisStatusHelpers osirisStatusHelpers_;
	SavegameSerializer savegameSerializer_;

	void OnBaseModuleLoaded(void * self);
	void OnGameStateChanged(void * self, GameState fromState, GameState toState);
	void OnGameStateWorkerStart(void * self);
	void OnGameStateWorkerExit(void* self);
	void OnUpdate(void* self, GameTime* time);
};

END_NS()
