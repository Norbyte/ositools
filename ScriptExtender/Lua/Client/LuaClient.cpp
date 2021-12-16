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

void AddVoiceMetaData(lua_State* L, char const* speakerGuid, char const* translatedStringKey, char const* source,
	float length, std::optional<int> priority);

template <>
ecl::Character* ObjectProxyHandleBasedRefImpl<ecl::Character>::Get() const
{
	auto self = ecl::GetEntityWorld()->GetCharacter(handle_);
	if (!lifetime_.IsAlive()) {
		WarnDeprecated56("An access was made to an ecl::Character instance after its lifetime has expired; this behavior is deprecated.");
	}

	return self;
}

void MakeLegacyClientCharacterObjectRef(lua_State* L, ecl::Character* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<ecl::Character>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
	} else {
		push(L, nullptr);
	}
}

template <>
ecl::Item* ObjectProxyHandleBasedRefImpl<ecl::Item>::Get() const
{
	auto self = ecl::GetEntityWorld()->GetItem(handle_);
	if (!lifetime_.IsAlive()) {
		WarnDeprecated56("An access was made to an ecl::Item instance after its lifetime has expired; this behavior is deprecated.");
	}

	return self;
}

void MakeLegacyClientItemObjectRef(lua_State* L, ecl::Item* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<ecl::Item>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
	} else {
		push(L, nullptr);
	}
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
		if ((ObjectType)handle.GetType() == ObjectType::Visual) {
			auto visual = GetStaticSymbols().GetResourceManager()->VisualFactory->Get(handle);
			if (visual) {
				visual->DetachFromScene();
			}
		}

		GetStaticSymbols().GetResourceManager()->DestroyVisual(handle);
	}

	pendingVisualDeletes_.Clear();
}


void VisualSystem::RequestDeleteVisual(ComponentHandle handle)
{
	pendingVisualDeletes_.Add(handle);
}

END_NS()

BEGIN_NS(ecl::lua)

using namespace dse::lua;

LifetimeHolder GetClientLifetime()
{
	assert(gExtender->GetClient().IsInClientThread());
	return ExtensionState::Get().GetLua()->GetCurrentLifetime();
}

LifetimePool& GetClientLifetimePool()
{
	assert(gExtender->GetClient().IsInClientThread());
	return ExtensionState::Get().GetLua()->GetLifetimePool();
}

void ExtensionLibraryClient::Register(lua_State * L)
{
	ExtensionLibrary::Register(L);
	UIFlashObject::RegisterMetatable(L);
	UIFlashArray::RegisterMetatable(L);
	UIFlashFunction::RegisterMetatable(L);
}


ecl::Character* GetCharacter(lua_State* L, int index)
{
	ecl::Character* character = nullptr;
	switch (lua_type(L, index)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, index);
		if (handle.GetType() == (uint32_t)ObjectType::ServerCharacter) {
			OsiError("Attempted to resolve server ComponentHandle on the client");
		}
		else {
			character = GetEntityWorld()->GetCharacter(handle);
		}
		break;
	}

	case LUA_TNUMBER:
	{
		auto value = lua_tointeger(L, index);
		if (value > 0xffffffff) {
			OsiError("Resolving integer object handles is deprecated since v52!")
			ComponentHandle handle{ value };
			if (handle.GetType() == (uint32_t)ObjectType::ServerCharacter) {
				OsiError("Attempted to resolve server ComponentHandle on the client");
			} else {
				character = GetEntityWorld()->GetCharacter(handle);
			}
		} else {
			NetId netId{ (uint32_t)value };
			character = GetEntityWorld()->GetCharacter(netId);
		}
		break;
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, index);
		character = GetEntityWorld()->GetCharacter(guid);
		break;
	}

	default:
		OsiError("Expected character UUID, Handle or NetId");
		break;
	}

	return character;
}

int GetCharacter(lua_State* L)
{
	StackCheck _(L, 1);
	ecl::Character* character = GetCharacter(L, 1);
	MakeLegacyClientCharacterObjectRef(L, character);
	return 1;
}

int GetItem(lua_State* L)
{
	StackCheck _(L, 1);

	ecl::Item* item = nullptr;
	switch (lua_type(L, 1)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, 1);
		if (handle.GetType() == (uint32_t)ObjectType::ServerItem) {
			OsiError("Attempted to resolve server ComponentHandle on the client");
		} else {
			item = GetEntityWorld()->GetItem(handle);
		}
		break;
	}

	case LUA_TNUMBER:
	{
		auto value = lua_tointeger(L, 1);
		if (value > 0xffffffff) {
			ComponentHandle handle{ value };
			if (handle.GetType() == (uint32_t)ObjectType::ServerItem) {
				OsiError("Attempted to resolve server ComponentHandle on the client");
			} else {
				item = GetEntityWorld()->GetItem(handle);
			}
		} else {
			NetId netId{ (uint32_t)value };
			item = GetEntityWorld()->GetItem(netId);
		}
		break;
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, 1);
		item = GetEntityWorld()->GetItem(guid);
		break;
	}

	default:
		OsiError("Expected item UUID, Handle or NetId; got " << lua_typename(L, lua_type(L, 1)));
		break;
	}

	MakeLegacyClientItemObjectRef(L, item);
	return 1;
}

int GetStatus(lua_State* L)
{
	StackCheck _(L, 1);

	auto character = GetCharacter(L, 1);
	if (character == nullptr) return 0;

	ecl::Status* status;
	if (lua_type(L, 2) == LUA_TLIGHTUSERDATA) {
		auto statusHandle = get<ComponentHandle>(L, 2);
		status = character->GetStatus(statusHandle);
	} else {
		auto index = lua_tointeger(L, 2);
		NetId statusNetId{ (uint32_t)index };
		status = character->GetStatus(statusNetId);
	}

	MakeObjectRef(L, status);
	return 1;
}

IEoCClientObject* GetGameObjectInternal(ComponentHandle const& handle)
{
	if (handle) {
		switch ((ObjectType)handle.GetType()) {
		case ObjectType::ClientCharacter:
			return GetEntityWorld()->GetCharacter(handle);

		case ObjectType::ClientItem: 
			return GetEntityWorld()->GetItem(handle);

		default:
			OsiError("Cannot resolve unsupported client handle type: " << handle.GetType());
			return nullptr;
		}
	} else {
		return nullptr;
	}
}

IEoCClientObject* GetGameObjectInternal(char const* nameGuid)
{
	auto character = GetEntityWorld()->GetCharacter(nameGuid, false);
	if (character) {
		return character;
	}

	auto item = GetEntityWorld()->GetItem(nameGuid, false);
	if (item) {
		return item;
	}

	return nullptr;
}

int GetGameObject(lua_State* L)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		return luaL_error(L, "Attempted to resolve game object handle in restricted context");
	}

	StackCheck _(L, 1);
	switch (lua_type(L, 1)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, 1);
		MakeObjectRef(L, GetGameObjectInternal(handle));
		break;
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, 1);
		MakeObjectRef(L, GetGameObjectInternal(guid));
		break;
	}

	default:
		OsiError("Expected object GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
		push(L, nullptr);
		break;
	}

	return 1;
}

// FIXME - move to Level->AiGrid!
int GetAiGrid(lua_State* L)
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (!level || !level->AiGrid) {
		OsiError("Current level not available yet!");
		return 0;
	}

	MakeObjectRef(L, level->AiGrid);
	return 1;
}


int GetGameState(lua_State* L)
{
	StackCheck _(L, 1);
	auto state = GetStaticSymbols().GetClientState();
	if (state) {
		push(L, *state);
	} else {
		lua_pushnil(L);
	}

	return 1;
}

int GetPickingState(lua_State* L)
{
	StackCheck _(L, 1);
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (level == nullptr || level->PickingHelperManager == nullptr) {
		push(L, nullptr);
		return 1;
	}

	int playerIndex{ 1 };
	if (lua_gettop(L) >= 1) {
		playerIndex = get<int>(L, 1);
	}

	auto helper = level->PickingHelperManager->PlayerHelpers.Find(playerIndex);
	if (helper == nullptr) {
		push(L, nullptr);
		return 1;
	}

	auto const& base = (*helper)->b;
	lua_newtable(L);
	if ((*helper)->GameObjectPick) {
		setfield(L, "WorldPosition", (*helper)->GameObjectPick->WorldPos.Position);
	}

	setfield(L, "WalkablePosition", base.WalkablePickPos.Position);

	if (base.HoverCharacterHandle2) {
		setfield(L, "HoverCharacter", base.HoverCharacterHandle2);
		setfield(L, "HoverCharacterPosition", base.HoverCharacterPickPos.Position);
	}

	if (base.HoverCharacterHandle) {
		setfield(L, "HoverCharacter2", base.HoverCharacterHandle);
	}

	if (base.HoverItemHandle) {
		setfield(L, "HoverItem", base.HoverItemHandle);
		setfield(L, "HoverItemPosition", base.HoverItemPickPos.Position);
	}

	if (base.HoverCharacterOrItemHandle) {
		setfield(L, "HoverEntity", base.HoverCharacterOrItemHandle);
	}

	if (base.PlaceablePickHandle) {
		setfield(L, "PlaceableEntity", base.PlaceablePickHandle);
		setfield(L, "PlaceablePosition", base.PlaceablePickInfo.Position);
	}

	return 1;
}

int UpdateShroud(lua_State* L)
{
	StackCheck _(L, 0);
	auto x = get<float>(L, 1);
	auto y = get<float>(L, 2);
	auto layer = get<ShroudType>(L, 3);
	auto value = get<int>(L, 4);

	if (value < 0 || value > 255) {
		OsiError("Can only set shroud cell values between 0 and 255");
		return 0;
	}

	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (!level || !level->ShroudManager || !level->ShroudManager->ShroudData) {
		OsiError("No ShroudManager for current level?");
		return 0;
	}

	level->ShroudManager->ShroudData->SetByteAtPos(layer, x, y, (uint8_t)value);
	return 0; 
}

void ExtensionLibraryClient::RegisterLib(lua_State * L)
{
	static const luaL_Reg extLib[] = {
		{"GetCharacter", GetCharacter},
		{"GetItem", GetItem},
		{"GetStatus", GetStatus},
		{"GetGameObject", GetGameObject},
		{"GetAiGrid", GetAiGrid},
		{"NewDamageList", NewDamageList},

		{"GetGameState", GetGameState},
		{"GetPickingState", GetPickingState},
		{"AddVoiceMetaData", LuaWrapFunction(AddVoiceMetaData)},

		// EXPERIMENTAL FUNCTIONS
		{"UpdateShroud", UpdateShroud},
		{0,0}
	};

	luaL_newlib(L, extLib); // stack: lib
	lua_setglobal(L, "Ext"); // stack: -

	RegisterSharedLibraries(L);
	RegisterClientLibraries(L);
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
	// FIXME - TEMP CAST
	auto uiManager = GetStaticSymbols().GetUIObjectManager();
	params.UI = (UIObject*)uiManager->Get(uiObjectHandle);
	ThrowEvent(*this, "UIObjectCreated", params);
}

void ClientState::OnUICall(UIObject* ui, const char * func, unsigned int numArgs, ig::InvokeDataValue * args)
{
	UICallEventParams params;
	params.UI = ui;
	params.Function = func;
	params.When = "Before";
	params.Args = std::span<ig::InvokeDataValue>(args, numArgs);
	ThrowEvent(*this, "UICall", params);
}

void ClientState::OnAfterUICall(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args)
{
	UICallEventParams params;
	params.UI = ui;
	params.Function = func;
	params.When = "After";
	params.Args = std::span<ig::InvokeDataValue>(args, numArgs);
	ThrowEvent(*this, "UICall", params);
}

void ClientState::OnUIInvoke(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args)
{
	UICallEventParams params;
	params.UI = ui;
	params.Function = func;
	params.When = "Before";
	params.Args = std::span<ig::InvokeDataValue>(args, numArgs);
	ThrowEvent(*this, "UIInvoke", params);
}

void ClientState::OnAfterUIInvoke(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args)
{
	UICallEventParams params;
	params.UI = ui;
	params.Function = func;
	params.When = "After";
	params.Args = std::span<ig::InvokeDataValue>(args, numArgs);
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
			// FIXME - TEMP CAST
			return (UIObject*)uiManager->Get(it->second);
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
