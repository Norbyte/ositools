#include <stdafx.h>
#include <Lua/LuaBindingClient.h>
#include <OsirisProxy.h>
#include <ExtensionStateClient.h>
#include <PropertyMaps.h>
#include "resource.h"

namespace dse::lua
{
	char const* const ObjectProxy<ecl::Status>::MetatableName = "ecl::Status";


	PropertyMapBase& ClientStatusToPropertyMap(ecl::Status* status)
	{
		// TODO - add property maps for statuses
		return gEclStatusPropertyMap;
	}


	int ObjectProxy<ecl::Status>::Index(lua_State* L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Status object no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto& propertyMap = ClientStatusToPropertyMap(obj_);
		auto fetched = LuaPropertyMapGet(L, propertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<ecl::Status>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const* const ObjectProxy<ecl::PlayerCustomData>::MetatableName = "ecl::PlayerCustomData";

	ecl::PlayerCustomData* ObjectProxy<ecl::PlayerCustomData>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto character = ecl::GetEntityWorld()->GetCharacter(handle_);
		if (character == nullptr) luaL_error(L, "Character handle invalid");

		if (character->PlayerData == nullptr
			// Always false on the client for some reason
			/*|| !character->PlayerData->CustomData.Initialized*/) {
			OsiError("Character has no player data, or custom data was not initialized.");
			return nullptr;
		}

		return &character->PlayerData->CustomData;
	}

	int ObjectProxy<ecl::PlayerCustomData>::Index(lua_State* L)
	{
		auto customData = Get(L);
		if (!customData) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gPlayerCustomDataPropertyMap, customData, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<ecl::PlayerCustomData>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const* const ObjectProxy<ecl::Character>::MetatableName = "ecl::Character";

	int ClientCharacterFetchProperty(lua_State* L, ecl::Character* character, char const* prop)
	{
		if (strcmp(prop, "PlayerCustomData") == 0) {
			if (character->PlayerData != nullptr
				// Always false on the client for some reason
				/*&& character->PlayerData->CustomData.Initialized*/) {
				ObjectHandle handle;
				character->GetObjectHandle(handle);
				ObjectProxy<ecl::PlayerCustomData>::New(L, handle);
				return 1;
			}
			else {
				OsiError("Character has no player data, or custom data was not initialized.");
				return 0;
			}
		}

		if (strcmp(prop, "Stats") == 0) {
			if (character->Stats != nullptr) {
				// FIXME - use handle based proxy
				ObjectProxy<CDivinityStats_Character>::New(L, character->Stats);
				return 1;
			}
			else {
				OsiError("Character has no stats.");
				return 0;
			}
		}

		auto fetched = LuaPropertyMapGet(L, gEclCharacterPropertyMap, character, prop, true);
		return fetched ? 1 : 0;
	}

	ecl::Character* ObjectProxy<ecl::Character>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto character = ecl::GetEntityWorld()->GetCharacter(handle_);
		if (character == nullptr) luaL_error(L, "Character handle invalid");
		return character;
	}

#include <Lua/LuaShared.inl>

	int ObjectProxy<ecl::Character>::Index(lua_State* L)
	{
		auto character = Get(L);
		if (!character) return 0;

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "HasTag") == 0) {
			lua_pushcfunction(L, &CharacterHasTag<ecl::Character>);
			return 1;
		}

		if (strcmp(prop, "GetTags") == 0) {
			lua_pushcfunction(L, &CharacterGetTags<ecl::Character>);
			return 1;
		}

		if (strcmp(prop, "GetStatus") == 0) {
			lua_pushcfunction(L, (&CharacterGetStatus<ecl::Character, ecl::Status>));
			return 1;
		}

		if (strcmp(prop, "GetStatusByType") == 0) {
			lua_pushcfunction(L, (&CharacterGetStatusByType<ecl::Character, ecl::Status>));
			return 1;
		}

		if (strcmp(prop, "GetStatuses") == 0) {
			lua_pushcfunction(L, (&CharacterGetStatuses<ecl::Character>));
			return 1;
		}

		return ClientCharacterFetchProperty(L, character, prop);
	}

	int ObjectProxy<ecl::Character>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const* const ObjectProxy<ecl::Item>::MetatableName = "ecl::Item";

	ecl::Item* ObjectProxy<ecl::Item>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto item = ecl::GetEntityWorld()->GetItem(handle_);
		if (item == nullptr) luaL_error(L, "Item handle invalid");
		return item;
	}

	int ObjectProxy<ecl::Item>::Index(lua_State* L)
	{
		auto item = Get(L);
		if (!item) return 0;

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "Stats") == 0) {
			if (item->Stats != nullptr) {
				// FIXME - use handle based proxy
				ObjectProxy<CDivinityStats_Item>::New(L, item->Stats);
				return 1;
			}
			else {
				OsiError("Item has no stats.");
				return 0;
			}
		}

		bool fetched = false;
		if (item->Stats != nullptr) {
			fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item->Stats, prop, false);
		}

		if (!fetched) {
			fetched = LuaPropertyMapGet(L, gEclItemPropertyMap, item, prop, true);
		}

		return fetched ? 1 : 0;
	}

	int ObjectProxy<ecl::Item>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}
}


namespace dse::ecl::lua
{
	using namespace dse::lua;

	void ExtensionLibraryClient::Register(lua_State * L)
	{
		ExtensionLibrary::Register(L);
		ObjectProxy<Status>::RegisterMetatable(L);
		ObjectProxy<ecl::PlayerCustomData>::RegisterMetatable(L);
		ObjectProxy<Character>::RegisterMetatable(L);
		ObjectProxy<Item>::RegisterMetatable(L);
		UIObjectProxy::RegisterMetatable(L);
	}


	ecl::Character* GetCharacter(lua_State* L, int index)
	{
		ecl::Character* character = nullptr;
		switch (lua_type(L, index)) {
		case LUA_TNUMBER:
		{
			auto value = lua_tointeger(L, index);
			if (value > 0xffffffff) {
				ObjectHandle handle{ value };
				if (handle.GetType() == (uint32_t)ObjectType::ServerCharacter) {
					OsiError("Attempted to resolve server ObjectHandle on the client");
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
		LuaClientPin lua(ExtensionState::Get());

		ecl::Character* character = GetCharacter(L, 1);

		if (character != nullptr) {
			ObjectHandle handle;
			character->GetObjectHandle(handle);
			ObjectProxy<ecl::Character>::New(L, handle);
			return 1;
		}
		else {
			return 0;
		}
	}

	int GetItem(lua_State* L)
	{
		LuaClientPin lua(ExtensionState::Get());

		ecl::Item* item = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TNUMBER:
		{
			auto value = lua_tointeger(L, 1);
			if (value > 0xffffffff) {
				ObjectHandle handle{ value };
				if (handle.GetType() == (uint32_t)ObjectType::ServerItem) {
					OsiError("Attempted to resolve server ObjectHandle on the client");
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
			OsiError("Expected item UUID, Handle or NetId");
			return 0;
		}

		if (item != nullptr) {
			ObjectHandle handle;
			item->GetObjectHandle(handle);
			ObjectProxy<ecl::Item>::New(L, handle);
			return 1;
		} else {
			return 0;
		}
	}

	int GetStatus(lua_State* L)
	{
		LuaClientPin lua(ExtensionState::Get());

		auto character = GetCharacter(L, 1);
		if (character == nullptr) return 0;

		auto index = lua_tointeger(L, 2);
		if (index > 0xffffffff) {
			OsiError("Resolving statuses by Handle is not supported on the client");
			return 0;
		} 
		
		NetId statusNetId{ (uint32_t)index };
		auto status = character->GetStatus(statusNetId);
		if (status != nullptr) {
			ObjectHandle characterHandle;
			character->GetObjectHandle(characterHandle);
			StatusHandleProxy::New(L, characterHandle, statusNetId);
			return 1;
		}

		OsiError("Character has no status with NetId 0x" << std::hex << index);
		return 0;
	}


	int OsirisIsCallableClient(lua_State* L)
	{
		push(L, false);
		return 1;
	}

	int PostMessageToServer(lua_State * L)
	{
		auto channel = luaL_checkstring(L, 1);
		auto payload = luaL_checkstring(L, 2);

		auto & networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeClientMessage();
		if (msg != nullptr) {
			auto postMsg = msg->GetMessage().mutable_post_lua();
			postMsg->set_channel_name(channel);
			postMsg->set_payload(payload);
			networkMgr.ClientSend(msg);
		} else {
			OsiErrorS("Could not get free message!");
		}

		return 0;
	}


	char const* const StatusHandleProxy::MetatableName = "ecl::HStatus";

	int StatusHandleProxy::Index(lua_State* L)
	{
		auto character = GetEntityWorld()->GetCharacter(character_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");

		auto status = character->GetStatus(statusNetId_);
		if (status == nullptr) return luaL_error(L, "Status handle invalid");

		auto prop = luaL_checkstring(L, 2);
		auto& propertyMap = ClientStatusToPropertyMap(status);
		auto fetched = LuaPropertyMapGet(L, propertyMap, status, prop, true);
		return fetched ? 1 : 0;
	}

	int StatusHandleProxy::NewIndex(lua_State* L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	struct CustomUI : public ecl::EoCUI
	{
		CustomUI(dse::Path * path)
			: EoCUI(path)
		{}

		void OnFunctionCalled(const char * func, unsigned int numArgs, InvokeDataValue * args)
		{
			LuaClientPin lua(ExtensionState::Get());
			if (lua) {
				lua->OnUICall(UIObjectHandle, func, numArgs, args);
			}

			EoCUI::OnFunctionCalled(func, numArgs, args);
		}

		void Destroy(bool free)
		{
			EoCUI::Destroy(false);
			if (free) {
				GameFree(this);
			}
		}

		const char * GetDebugName()
		{
			return "extender::CustomUI";
		}

		static UIObject * Creator(dse::Path * path)
		{
			return GameAlloc<CustomUI>(path);
		}
	};


	void LuaToInvokeDataValue(lua_State * L, int index, InvokeDataValue & val)
	{
		switch (lua_type(L, index)) {
		case LUA_TNUMBER:
			val.TypeId = InvokeDataValueType::IDV_Double;
			val.DoubleVal = lua_tonumber(L, index);
			break;

		case LUA_TBOOLEAN:
			val.TypeId = InvokeDataValueType::IDV_Bool;
			val.BoolVal = lua_toboolean(L, index);
			break;

		case LUA_TSTRING:
			val.TypeId = InvokeDataValueType::IDV_String;
			val.StringVal = lua_tostring(L, index);
			break;

		default:
			luaL_error(L, "Cannot pass value of type %s to Flash", lua_typename(L, lua_type(L, index)));
		}
	}


	void InvokeDataValueToLua(lua_State * L, InvokeDataValue const & val)
	{
		switch (val.TypeId) {
		case InvokeDataValueType::IDV_NoneVal:
			lua_pushnil(L);
			break;

		case InvokeDataValueType::IDV_Bool:
			push(L, val.BoolVal);
			break;

		case InvokeDataValueType::IDV_Double:
			push(L, val.DoubleVal);
			break;

		case InvokeDataValueType::IDV_String:
			push(L, val.StringVal);
			break;

		default:
			OsiError("Flash value of type " << (unsigned)val.TypeId << " cannot be passed to Lua");
			lua_pushnil(L);
			break;
		}
	}


	UIObject * UIObjectProxy::Get()
	{
		auto uiManager = GetStaticSymbols().GetUIObjectManager();
		if (uiManager != nullptr) {
			return uiManager->Get(handle_);
		} else {
			return nullptr;
		}
	}


	char const * const UIObjectProxy::MetatableName = "ecl::EoCUI";

	void UIObjectProxy::PopulateMetatable(lua_State * L)
	{
		lua_newtable(L);

		lua_pushcfunction(L, &SetPosition);
		lua_setfield(L, -2, "SetPosition");

		lua_pushcfunction(L, &Resize);
		lua_setfield(L, -2, "Resize");

		lua_pushcfunction(L, &Show);
		lua_setfield(L, -2, "Show");

		lua_pushcfunction(L, &Hide);
		lua_setfield(L, -2, "Hide");

		lua_pushcfunction(L, &Invoke);
		lua_setfield(L, -2, "Invoke");

		lua_pushcfunction(L, &GotoFrame);
		lua_setfield(L, -2, "GotoFrame");

		lua_pushcfunction(L, &GetValue);
		lua_setfield(L, -2, "GetValue");

		lua_pushcfunction(L, &SetValue);
		lua_setfield(L, -2, "SetValue");

		lua_pushcfunction(L, &GetHandle);
		lua_setfield(L, -2, "GetHandle");

		lua_pushcfunction(L, &Destroy);
		lua_setfield(L, -2, "Destroy");

		lua_pushcfunction(L, &ExternalInterfaceCall);
		lua_setfield(L, -2, "ExternalInterfaceCall");

		lua_pushcfunction(L, &CaptureExternalInterfaceCalls);
		lua_setfield(L, -2, "CaptureExternalInterfaceCalls");

		lua_setfield(L, -2, "__index");
	}


	int UIObjectProxy::SetPosition(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		int pos[2];
		pos[0] = (int)luaL_checkinteger(L, 2);
		pos[1] = (int)luaL_checkinteger(L, 3);

		ui->SetPos(pos);
		return 0;
	}


	int UIObjectProxy::Resize(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		int pos[2];
		pos[0] = (int)luaL_checkinteger(L, 2);
		pos[1] = (int)luaL_checkinteger(L, 3);

		ui->FlashPlayer->SetSize(pos);
		return 0;
	}


	int UIObjectProxy::Show(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		ui->Show();
		return 0;
	}


	int UIObjectProxy::Hide(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		ui->Hide();
		return 0;
	}


	int UIObjectProxy::Invoke(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		auto root = ui->FlashPlayer->GetRootObject();
		if (!root) return 0;

		auto name = luaL_checkstring(L, 2);

		auto & invokes = ui->FlashPlayer->Invokes.Set;
		std::optional<uint32_t> invokeId;
		for (uint32_t i = 0; i < invokes.Size; i++) {
			if (strcmp(name, invokes[i].Name) == 0) {
				invokeId = i;
				break;
			}
		}

		if (!invokeId) {
			invokeId = ui->FlashPlayer->Invokes.Set.Size;
			ui->FlashPlayer->AddInvokeName(*invokeId, name);
		}

		auto numArgs = lua_gettop(L) - 2;
		std::vector<InvokeDataValue> args;
		args.resize(numArgs);
		for (auto i = 0; i < numArgs; i++) {
			LuaToInvokeDataValue(L, i + 3, args[i]);
		}

		bool ok = ui->FlashPlayer->InvokeArgs(*invokeId, args.data(), numArgs);
		push(L, ok);
		return 1;
	}


	int UIObjectProxy::GotoFrame(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		auto frame = luaL_checkinteger(L, 2);
		bool force = false;
		if (lua_gettop(L) >= 3) {
			force = lua_toboolean(L, 3);
		}

		if (force) {
			ui->FlashPlayer->GotoFrame2(frame);
		} else {
			ui->FlashPlayer->GotoFrame(frame);
		}

		return 0;
	}


	int UIObjectProxy::SetValue(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		auto root = ui->FlashPlayer->GetRootObject();
		if (!root) return 0;

		auto path = luaL_checkstring(L, 2);
		InvokeDataValue value;
		LuaToInvokeDataValue(L, 3, value);
		int arrayIndex = -1;
		if (lua_gettop(L) >= 4) {
			arrayIndex = (int)luaL_checkinteger(L, 4);
		}

		root->SetValue(path, value, arrayIndex);
		return 0;
	}


	int UIObjectProxy::GetValue(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		auto root = ui->FlashPlayer->GetRootObject();
		if (!root) return 0;

		auto path = luaL_checkstring(L, 2);
		auto typeName = luaL_checkstring(L, 3);
		int arrayIndex = -1;
		if (lua_gettop(L) >= 4) {
			arrayIndex = (int)luaL_checkinteger(L, 4);
		}

		InvokeDataValueType type = IDV_NoneVal;
		if (strcmp(typeName, "number") == 0) {
			type = InvokeDataValueType::IDV_Double;
		} else if (strcmp(typeName, "boolean") == 0) {
			type = InvokeDataValueType::IDV_Bool;
		} else if (strcmp(typeName, "string") == 0) {
			type = InvokeDataValueType::IDV_String;
		} else {
			luaL_error(L, "Unknown value type for Flash fetch: %s", typeName);
		}

		InvokeDataValue value;
		if (root->GetValue(path, type, value, arrayIndex)) {
			InvokeDataValueToLua(L, value);
			return 1;
		} else {
			return 0;
		}
	}


	int UIObjectProxy::GetHandle(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		push(L, ui->UIObjectHandle.Handle);
		return 1;
	}


	int UIObjectProxy::Destroy(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		ui->RequestDelete();
		return 0;
	}


	int UIObjectProxy::ExternalInterfaceCall(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		auto function = luaL_checkstring(L, 2);
		auto numArgs = lua_gettop(L) - 2;
		std::vector<InvokeDataValue> args;
		args.resize(numArgs);
		for (auto i = 0; i < numArgs; i++) {
			LuaToInvokeDataValue(L, i + 3, args[i]);
		}

		ui->OnFunctionCalled(function, numArgs, args.data());
		return 0;
	}


	// This needs to be persistent for the lifetime of the app, as we don't restore altered VMTs
	std::unordered_map<UIObject::VMT *, UIObject::OnFunctionCalledProc> OriginalUIObjectCallHandlers;


	static void UIObjectFunctionCallCapture(UIObject * self, const char * function, unsigned int numArgs, InvokeDataValue * args)
	{
		LuaClientPin lua(ExtensionState::Get());
		if (lua) {
			lua->OnUICall(self->UIObjectHandle, function, numArgs, args);
		}

		auto vmt = *reinterpret_cast<UIObject::VMT **>(self);
		auto handler = OriginalUIObjectCallHandlers.find(vmt);
		if (handler != OriginalUIObjectCallHandlers.end()) {
			handler->second(self, function, numArgs, args);
		} else {
			OsiError("Couldn't find original OnFunctionCalled handler for UI object");
		}
	}


	int UIObjectProxy::CaptureExternalInterfaceCalls(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		// Custom UI element calls are captured by default, no need to hook them
		if (strcmp(ui->GetDebugName(), "extender::CustomUI") == 0) return 0;

		auto vmt = *reinterpret_cast<UIObject::VMT **>(ui);
		if (vmt->OnFunctionCalled == &UIObjectFunctionCallCapture) return 0;

		WriteAnchor _((uint8_t *)vmt, sizeof(*vmt));
		OriginalUIObjectCallHandlers.insert(std::make_pair(vmt, vmt->OnFunctionCalled));
		vmt->OnFunctionCalled = &UIObjectFunctionCallCapture;

		return 0;
	}


	uint32_t NextCustomCreatorId = 1000;

	int CreateUI(lua_State * L)
	{
		auto name = luaL_checkstring(L, 1);
		auto path = luaL_checkstring(L, 2);
		auto layer = (int)luaL_checkinteger(L, 3);

		uint32_t flags;
		if (lua_gettop(L) >= 4) {
			flags = (uint32_t)luaL_checkinteger(L, 4); // 0x20021
		} else {
			flags = (uint32_t)(UIObjectFlags::OF_Load | UIObjectFlags::OF_PlayerInput1 | UIObjectFlags::OF_DeleteOnChildDestroy);
		}

		// FIXME - playerId, registerInvokeNames?

		LuaClientPin pin(ExtensionState::Get());
		auto ui = pin->GetUIObject(name);
		if (ui != nullptr) {
			OsiError("An UI object with name '" << name << "' already exists!");
			UIObjectProxy::New(L, ui->UIObjectHandle);
			return 1;
		}

		auto & sym = GetStaticSymbols();
		auto absPath = sym.ToPath(path, PathRootType::Data);

		auto uiManager = sym.GetUIObjectManager();
		if (uiManager == nullptr) {
			OsiError("Couldn't get symbol for UIObjectManager!");
			return 0;
		}

		if (sym.EoCUI__ctor == nullptr || sym.EoCUI__vftable == nullptr) {
			OsiError("Couldn't get symbol for ecl::EoCUI::vftable!");
			return 0;
		}

		std::optional<uint32_t> creatorId;
		uiManager->UIObjectCreators.Iterate([&absPath, &creatorId](uint32_t id, UIObjectFunctor * value) {
			if (value->Path.Name == absPath.c_str()) {
				creatorId = id;
			}
		});

		if (!creatorId) {
			auto creator = GameAlloc<UIObjectFunctor>();
			creator->Path.Name = absPath;
			creator->CreateProc = CustomUI::Creator;

			sym.RegisterUIObjectCreator(uiManager, NextCustomCreatorId, creator);
			creatorId = NextCustomCreatorId++;
		}

		ObjectHandle handle;
		sym.UIObjectManager__CreateUIObject(uiManager, &handle, layer, *creatorId, flags, 0x80, 0);

		if (!handle) {
			OsiError("Failed to create UI object");
			return 0;
		}

		auto object = uiManager->Get(handle);
		if (!object) {
			OsiError("Failed to look up constructed UI object");
			return 0;
		}

		if (!object->FlashPlayer) {
			OsiError("Flash player initialization failed");
			return 0;
		}

		pin->OnClientUIObjectCreated(name, handle);
		UIObjectProxy::New(L, handle);
		return 1;
	}

	int GetUI(lua_State * L)
	{
		auto name = luaL_checkstring(L, 1);

		LuaClientPin pin(ExtensionState::Get());
		auto ui = pin->GetUIObject(name);
		if (ui != nullptr) {
			UIObjectProxy::New(L, ui->UIObjectHandle);
			return 1;
		} else {
			return 0;
		}
	}

	int GetBuiltinUI(lua_State * L)
	{
		auto path = luaL_checkstring(L, 1);
		auto absPath = GetStaticSymbols().ToPath(path, PathRootType::Data);

		auto uiManager = GetStaticSymbols().GetUIObjectManager();
		if (uiManager == nullptr) {
			OsiError("Couldn't get symbol for UIObjectManager!");
			return 0;
		}

		for (auto ui : uiManager->Objects) {
			if (ui != nullptr && ui->FlashPlayer != nullptr
				&& absPath == ui->Path.Name.c_str()) {
				UIObjectProxy::New(L, ui->UIObjectHandle);
				return 1;
			}
		}

		return 0;
	}

	int DestroyUI(lua_State * L)
	{
		auto name = luaL_checkstring(L, 1);

		LuaClientPin pin(ExtensionState::Get());
		auto ui = pin->GetUIObject(name);
		if (ui != nullptr) {
			ui->RequestDelete();
		} else {
			OsiError("No UI object exists with name '" << name << "'!");
		}

		return 0;
	}


	void ExtensionLibraryClient::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Version", GetExtensionVersion},
			{"Include", Include},
			{"Print", OsiPrint},
			{"PrintWarning", OsiPrintWarning},
			{"PrintError", OsiPrintError},

			{"SaveFile", SaveFile},
			{"LoadFile", LoadFile},

			{"JsonParse", JsonParse},
			{"JsonStringify", JsonStringify},

			{"IsModLoaded", IsModLoaded},
			{"GetModLoadOrder", GetModLoadOrder},
			{"GetModInfo", GetModInfo},

			{"GetStatEntries", GetStatEntries},
			{"GetSkillSet", GetSkillSet},
			{"GetEquipmentSet", GetEquipmentSet},
			{"StatGetAttribute", StatGetAttribute},
			{"StatSetAttribute", StatSetAttribute},
			{"StatAddCustomDescription", StatAddCustomDescription},
			{"StatSetLevelScaling", StatSetLevelScaling},
			{"GetStat", GetStat},
			{"GetDeltaMod", GetDeltaMod},
			{"UpdateDeltaMod", UpdateDeltaMod},

			{"GetCharacter", GetCharacter},
			{"GetItem", GetItem},
			{"GetStatus", GetStatus},
			{"NewDamageList", NewDamageList},
			{"OsirisIsCallable", OsirisIsCallableClient},
			{"IsDeveloperMode", IsDeveloperMode},
			{"Random", LuaRandom},
			{"Round", LuaRound},

			{"AddPathOverride", AddPathOverride},
			{"AddVoiceMetaData", AddVoiceMetaData},
			{"GetTranslatedString", GetTranslatedString},

			{"PostMessageToServer", PostMessageToServer},
			{"CreateUI", CreateUI},
			{"GetUI", GetUI},
			{"GetBuiltinUI", GetBuiltinUI},
			{"DestroyUI", DestroyUI},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}


	ClientState::ClientState()
	{
		library_.Register(L);

		auto baseLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY);
		LoadScript(baseLib, "BuiltinLibrary.lua");
		auto clientLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY_CLIENT);
		LoadScript(clientLib, "BuiltinLibraryClient.lua");
		auto gameMathLib = GetBuiltinLibrary(IDR_LUA_GAME_MATH);
		LoadScript(gameMathLib, "Game.Math.lua");

		lua_getglobal(L, "Ext"); // stack: Ext
		push(L, "ExtraData"); // stack: Ext, "ExtraData"
		StatsExtraDataProxy::New(L); // stack: Ext, "ExtraData", ExtraDataProxy
		lua_settable(L, -3); // stack: Ext
		lua_pop(L, 1); // stack: -

		// Ext is not writeable after loading SandboxStartup!
		auto sandbox = GetBuiltinLibrary(IDR_LUA_SANDBOX_STARTUP);
		LoadScript(sandbox, "SandboxStartup.lua");
	}

	ClientState::~ClientState()
	{
		auto & sym = GetStaticSymbols();
		auto uiManager = sym.GetUIObjectManager();
		for (auto & obj : clientUI_) {
			sym.UIObjectManager__DestroyUIObject(uiManager, &obj.second);
		}
	}

	void ClientState::OnUICall(ObjectHandle uiObjectHandle, const char * func, unsigned int numArgs, InvokeDataValue * args)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_UICall"); // stack: fn

		UIObjectProxy::New(L, uiObjectHandle);
		push(L, func);
		for (uint32_t i = 0; i < numArgs; i++) {
			InvokeDataValueToLua(L, args[i]);
		}

		CheckedCall<>(L, 2 + numArgs, "Ext.UICall");
	}

	std::optional<STDWString> ClientState::SkillGetDescriptionParam(SkillPrototype * prototype,
		CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, bool isFromItem)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		auto skill = prototype->GetStats();
		if (skill == nullptr) {
			return {};
		}

		PushExtFunction(L, "_SkillGetDescriptionParam"); // stack: fn

		auto _{ PushArguments(L,
			std::tuple{Push<StatsProxy>(skill, std::optional<int32_t>()),
			Push<ObjectProxy<CDivinityStats_Character>>(character)}) };
		push(L, isFromItem);

		for (auto const& paramText : paramTexts) {
			push(L, paramText); // stack: fn, skill, character, params...
		}

		auto result = CheckedCall<std::optional<char const *>>(L, 3 + paramTexts.Set.Size, "Ext.SkillGetDescriptionParam");
		if (result) {
			auto description = std::get<0>(*result);
			if (description) {
				return FromUTF8(*description);
			} else {
				return {};
			}
		} else {
			return {};
		}
	}


	std::optional<STDWString> ClientState::StatusGetDescriptionParam(StatusPrototype * prototype, CRPGStats_ObjectInstance* owner,
		CRPGStats_ObjectInstance* statusSource, ObjectSet<STDString> const & paramTexts)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		auto status = prototype->GetStats();
		if (status == nullptr) {
			return {};
		}

		PushExtFunction(L, "_StatusGetDescriptionParam"); // stack: fn

		auto luaStatus = Push<StatsProxy>(status, std::optional<int32_t>())(L);
		ItemOrCharacterPushPin luaSource(L, statusSource);
		ItemOrCharacterPushPin luaOwner(L, owner);

		for (auto const& paramText : paramTexts) {
			push(L, paramText); // stack: fn, status, srcCharacter, character, params...
		}

		auto result = CheckedCall<std::optional<char const *>>(L, 3 + paramTexts.Set.Size, "Ext.StatusGetDescriptionParam");
		if (result) {
			auto description = std::get<0>(*result);
			if (description) {
				return FromUTF8(*description);
			} else {
				return {};
			}
		} else {
			return {};
		}
	}


	void ClientState::OnClientUIObjectCreated(char const * name, ObjectHandle handle)
	{
		clientUI_.insert(std::make_pair(name, handle));
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
}

namespace dse::ecl
{

	ExtensionState & ExtensionState::Get()
	{
		return gOsirisProxy->GetClientExtensionState();
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
			OsiWarn("Client resume -- state " << (unsigned)*gameState);
		} else {
			OsiWarn("Client NO resume -- state " << (unsigned)*gameState);
		}
	}

}
