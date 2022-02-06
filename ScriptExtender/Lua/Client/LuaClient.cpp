#include <stdafx.h>
#include <Lua/Client/LuaBindingClient.h>
#include <Lua/Client/ClientUI.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>
#include <Extender/Client/ExtensionStateClient.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <GameDefinitions/GameObjects/Ai.h>
#include "resource.h"

#include <Lua/Client/ClientEntitySystem.inl>
#include <Lua/Client/ClientCharacter.inl>
#include <Lua/Client/ClientItem.inl>

BEGIN_NS(lua)

template <>
ecl::Character* ObjectProxyHandleBasedRefImpl<ecl::Character>::Get() const
{
	auto self = ecl::GetEntityWorld()->GetComponent<ecl::Character>(handle_);
	if (!lifetime_.IsAlive()) {
		WarnDeprecated56("An access was made to an ecl::Character instance after its lifetime has expired; this behavior is deprecated.");
	}

	return self;
}

template <>
ecl::Item* ObjectProxyHandleBasedRefImpl<ecl::Item>::Get() const
{
	auto self = ecl::GetEntityWorld()->GetComponent<ecl::Item>(handle_);
	if (!lifetime_.IsAlive()) {
		WarnDeprecated56("An access was made to an ecl::Item instance after its lifetime has expired; this behavior is deprecated.");
	}

	return self;
}

END_NS()

BEGIN_NS(ecl::lua::visual)

VisualSystem::~VisualSystem()
{
	auto del = GetStaticSymbols().ecl__MultiEffectHandler__Delete;
	for (auto const& effect : visuals_) {
		del(effect.get(), true);
	}
}

ClientMultiVisual* VisualSystem::Create()
{
	auto effect = MakeGameUnique<ClientMultiVisual>();
	visuals_.push_back(std::move(effect));
	return visuals_.rbegin()->get();
}

void VisualSystem::Destroy(ClientMultiVisual* effect)
{
	auto it = std::find_if(visuals_.begin(), visuals_.end(), [&](auto const& v) { return v.get() == effect; });
	if (it != visuals_.end()) {
		auto del = GetStaticSymbols().ecl__MultiEffectHandler__Delete;
		del(it->get(), false);
		visuals_.erase(it);
	}
}

void VisualSystem::Update()
{
	auto update = GetStaticSymbols().ecl__MultiEffectHandler__Update;
	for (auto const& effect : visuals_) {
		update(effect.get());
	}

	for (auto const& handle : pendingVisualDeletes_) {
		if ((ObjectHandleType)handle.GetType() == ObjectHandleType::Visual) {
			auto visual = GetStaticSymbols().GetResourceManager()->VisualFactory->Get(handle);
			if (visual) {
				visual->DetachFromScene();
			}
		}

		GetStaticSymbols().GetResourceManager()->DestroyVisual(handle);
	}

	pendingVisualDeletes_.clear();
}


void VisualSystem::RequestDeleteVisual(ComponentHandle handle)
{
	pendingVisualDeletes_.push_back(handle);
}

END_NS()

BEGIN_NS(ecl::lua)

using namespace dse::lua;

void ExtensionLibraryClient::Register(lua_State * L)
{
	ExtensionLibrary::Register(L);
	UIFlashObject::RegisterMetatable(L);
	UIFlashArray::RegisterMetatable(L);
	UIFlashFunction::RegisterMetatable(L);
}

void ExtensionLibraryClient::RegisterLib(lua_State * L)
{
	static const luaL_Reg extLib[] = {
		{0,0}
	};

	luaL_newlib(L, extLib); // stack: lib
	lua_setglobal(L, "Ext"); // stack: -

	RegisterSharedMetatables(L);
	gModuleRegistry.ConstructState(L, ModuleRole::Client);
}

ClientState* ClientState::FromLua(lua_State* L)
{
	assert(gExtender->GetClient().IsInClientThread());
	auto self = static_cast<ClientState*>(State::FromLua(L));
	assert(self->IsClient());
	return self;
}

ClientState::ClientState()
{}

ClientState::~ClientState()
{
	auto & sym = GetStaticSymbols();
	auto uiManager = sym.GetUIObjectManager();
	for (auto & obj : clientUI_) {
		sym.UIObjectManager__DestroyUIObject(uiManager, &obj.second);
	}

#if !defined(OSI_NO_DEBUGGER)
	if (gExtender) {
		auto debugger = gExtender->GetLuaDebugger();
		if (debugger) {
			debugger->ClientStateDeleted();
		}
	}
#endif
}

void ClientState::Initialize()
{
	StackCheck _(L, 0);
	library_.Register(L);

	gExtender->GetClient().GetExtensionState().LuaLoadBuiltinFile("ClientStartup.lua");

	lua_getglobal(L, "Ext"); // stack: Ext
	StatsExtraDataProxy::New(L); // stack: Ext, ExtraDataProxy
	lua_setfield(L, -2, "ExtraData"); // stack: Ext
	lua_pop(L, 1); // stack: -

	// Ext is not writeable after loading SandboxStartup!
	gExtender->GetClient().GetExtensionState().LuaLoadBuiltinFile("SandboxStartup.lua");

#if !defined(OSI_NO_DEBUGGER)
	auto debugger = gExtender->GetLuaDebugger();
	if (debugger) {
		debugger->ClientStateCreated(this);
	}
#endif
}

bool ClientState::IsClient()
{
	return true;
}

void ClientState::OnUpdate(GameTime const& time)
{
	State::OnUpdate(time);

	visualSystem_.Update();
}

void ClientState::OnCreateUIObject(ComponentHandle uiObjectHandle)
{
	UIObjectCreatedEventParams params;
	auto uiManager = GetStaticSymbols().GetUIObjectManager();
	params.UI = uiManager->Get(uiObjectHandle);
	ThrowEvent(*this, "UIObjectCreated", params);
}

// Workaround for strange bug where an InvokeDataValue is corrupted after an Invoke handler call
bool AreInvokeParamsPlausible(std::span<ig::InvokeDataValue> const& params)
{
	for (auto const& param : params) {
		if (param.TypeId > ig::DataType::UserData2) {
			return false;
		}

		if (param.TypeId == ig::DataType::String && param.StringVal.size() > 0xffff) {
			return false;
		}

		if (param.TypeId == ig::DataType::WString && param.WStringVal.size() > 0xffff) {
			return false;
		}
	}

	return true;
}

EventResult ClientState::OnUICall(UICallEventParams& params)
{
	if (!AreInvokeParamsPlausible(params.Args)) return EventResult::Failed;

	params.When = "Before";
	return ThrowEvent(*this, "UICall", params, true);
}

void ClientState::OnAfterUICall(UICallEventParams& params)
{
	if (!AreInvokeParamsPlausible(params.Args)) return;

	params.When = "After";
	ThrowEvent(*this, "UICall", params);
}

EventResult ClientState::OnUIInvoke(UICallEventParams& params)
{
	if (!AreInvokeParamsPlausible(params.Args)) return EventResult::Failed;

	params.When = "Before";
	return ThrowEvent(*this, "UIInvoke", params, true);
}

void ClientState::OnAfterUIInvoke(UICallEventParams& params)
{
	if (!AreInvokeParamsPlausible(params.Args)) return;

	params.When = "After";
	ThrowEvent(*this, "UIInvoke", params);
}

std::optional<STDWString> ClientState::SkillGetDescriptionParam(stats::SkillPrototype * prototype,
	stats::Character * character, ObjectSet<STDString> const & paramTexts, bool isFromItem)
{
	SkillGetDescriptionEventParams params{ prototype, character, paramTexts, isFromItem };
	ThrowEvent(*this, "SkillGetDescriptionParam", params);

	if (!params.Description.empty()) {
		return FromUTF8(params.Description);
	} else {
		return {};
	}
}


std::optional<STDWString> ClientState::StatusGetDescriptionParam(stats::StatusPrototype * prototype, stats::ObjectInstance* owner,
	stats::ObjectInstance* statusSource, ObjectSet<STDString> const & paramTexts)
{
	StatusGetDescriptionEventParams params{ prototype, owner, statusSource, paramTexts };
	ThrowEvent(*this, "StatusGetDescriptionParam", params);

	if (!params.Description.empty()) {
		return FromUTF8(params.Description);
	} else {
		return {};
	}
}

void ClientState::OnGameStateChanged(GameState fromState, GameState toState)
{
	GameStateChangeEventParams params{ fromState, toState };
	ThrowEvent(*this, "GameStateChanged", params);
}


void ClientState::OnCustomClientUIObjectCreated(char const * name, ComponentHandle handle)
{
	clientUI_.insert(std::make_pair(name, handle));
}


std::optional<STDString> ClientState::GetSkillPropertyDescription(stats::PropertyExtender* prop)
{
	SkillGetPropertyDescriptionEventParams params{ prop };
	ThrowEvent(*this, "SkillGetPropertyDescription", params);

	if (!params.Description.empty()) {
		return params.Description;
	} else {
		return {};
	}
}


void ClientState::OnAppInputEvent(InputEvent const& inputEvent)
{
	InputEventParams params{ const_cast<InputEvent*>(&inputEvent) };
	ThrowEvent(*this, "InputEvent", params);
}


UIObject * ClientState::GetUIObject(char const * name)
{
	auto it = clientUI_.find(name);
	if (it != clientUI_.end()) {
		auto uiManager = GetStaticSymbols().GetUIObjectManager();
		if (uiManager != nullptr) {
			return uiManager->Get(it->second);
		}
	}

	return nullptr;
}

END_NS()

BEGIN_NS(ecl)

ExtensionState & ExtensionState::Get()
{
	return gExtender->GetClient().GetExtensionState();
}


lua::State * ExtensionState::GetLua()
{
	if (Lua) {
		return Lua.get();
	} else {
		return nullptr;
	}
}

ModManager * ExtensionState::GetModManager()
{
	return GetModManagerClient();
}

void ExtensionState::DoLuaReset()
{
	Lua.reset();
	Lua = std::make_unique<lua::ClientState>();
	Lua->Initialize();
}

void ExtensionState::LuaStartup()
{
	ExtensionStateBase::LuaStartup();

	LuaClientPin lua(*this);
	auto gameState = GetStaticSymbols().GetClientState();
	if (gameState
		&& (*gameState == GameState::LoadLevel
			|| (*gameState == GameState::LoadModule && WasStatLoadTriggered())
			|| *gameState == GameState::LoadSession
			|| *gameState == GameState::LoadGMCampaign
			|| *gameState == GameState::Paused
			|| *gameState == GameState::PrepareRunning
			|| *gameState == GameState::Running
			|| *gameState == GameState::GameMasterPause)) {
		lua->OnModuleResume();
	}
}

END_NS()
