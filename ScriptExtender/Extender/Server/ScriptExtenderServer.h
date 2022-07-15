#pragma once

#include <Extender/Shared/ScriptExtenderBase.h>
#include <Extender/Shared/ExtenderConfig.h>
#include <GameDefinitions/Osiris.h>
#include <Extender/Server/ExtensionStateServer.h>
#include <Extender/Server/NetworkManagerServer.h>
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

	bool IsInServerThread() const;
	void ResetLuaState();
	bool RequestResetClientLuaState();
	void ResetExtensionState();
	void LoadExtensionState();

private:
	OsirisExtender osiris_;
	std::unique_ptr<ExtensionState> extensionState_;
	bool extensionLoaded_{ false };
	NetworkManager network_;
	NetworkFixedStringSender networkFixedStrings_;

	enum class GameStateChangedEventTag {};
	PostHookableFunction<GameStateChangedEventTag, void(void*, GameState, GameState)> gameStateChangedEvent_;

	void OnBaseModuleLoaded(void * self);
	void OnGameStateChanged(void * self, GameState fromState, GameState toState);
	void OnGameStateWorkerStart(void * self);
	void OnGameStateWorkerExit(void* self);
	void OnUpdate(void* self, GameTime* time);

	enum class GameStateWorkerStartTag {};
	enum class GameStateMachcineUpdateTag {};
	HookableFunction<GameStateWorkerStartTag, void(void*)> gameStateWorkerStart_;
	HookableFunction<GameStateMachcineUpdateTag, void(void*, GameTime*)> gameStateMachineUpdate_;
};

END_NS()
