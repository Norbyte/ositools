#pragma once

#include <Extender/Shared/ScriptExtenderBase.h>
#include <Extender/Shared/ExtenderConfig.h>
#include <Extender/Client/ExtensionStateClient.h>
#include <Extender/Client/NetworkManagerClient.h>
#include <Extender/Client/StatusHelpers.h>
#include <Extender/Shared/CustomSkills.h>
#include <GameDefinitions/Symbols.h>

BEGIN_NS(ecl)

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

	inline ExtensionState& GetExtensionState() const
	{
		return *extensionState_;
	}

	inline NetworkManager& GetNetworkManager()
	{
		return network_;
	}

	inline NetworkFixedStringReceiver& GetNetworkFixedStrings()
	{
		return networkFixedStrings_;
	}

	inline StatusHelpers& GetStatusHelpers()
	{
		return statusHelpers_;
	}

	inline CustomSkillVMTWrappers& GetSkillVMTs()
	{
		return customSkillVmts_;
	}

	bool IsInClientThread() const;
	void ResetLuaState();
	void ResetExtensionState();
	void LoadExtensionState(ExtensionStateContext ctx);

	void UpdateServerProgress(STDWString const& status);
	void UpdateClientProgress(STDWString const& status);

	enum class GameStateWorkerStartTag {};
	enum class GameStateMachineUpdateTag {};
	enum class GameStateChangedEventTag {};
	enum class GameStateLoadIncLocalProgress {};
	WrappableFunction<GameStateWorkerStartTag, void(void*)> gameStateWorkerStart_;
	WrappableFunction<GameStateMachineUpdateTag, void(void*, GameTime*)> gameStateMachineUpdate_;
	WrappableFunction<GameStateChangedEventTag, void(void*, GameState, GameState)> gameStateChangedEvent_;
	WrappableFunction<GameStateLoadIncLocalProgress, void(void*, int, char const*)> gameStateLoadIncLocalProgress_;

private:
	ExtenderConfig& config_;
	std::unique_ptr<ExtensionState> extensionState_;
	bool extensionLoaded_{ false };
	bool postStartupDone_{ false };
	NetworkManager network_;
	NetworkFixedStringReceiver networkFixedStrings_;
	StatusHelpers statusHelpers_;
	CustomSkillVMTWrappers customSkillVmts_;
	STDWString serverStatus_;
	STDWString clientStatus_;

	void OnBaseModuleLoaded(void* self);
	void OnGameStateChanged(void* self, GameState fromState, GameState toState);
	void OnGameStateWorkerRun(void (* wrapped)(void*), void* self);
	void OnUpdate(void* self, GameTime* time);
	void OnIncLocalProgress(void* self, int progress, char const* state);
	void ShowLoadingProgress();

	void RegisterFlashTraceCallbacks();

	static void FlashTraceCallback(void* ctx, void* player, char const* message);
	static void FlashWarningCallback(void* ctx, void* player, int code, char const* message);
};

END_NS()
