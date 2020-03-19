#include <stdafx.h>
#include <LuaBindingClient.h>
#include <OsirisProxy.h>
#include <ExtensionStateClient.h>
#include "resource.h"

namespace osidbg
{
	void LuaExtensionLibraryClient::Register(lua_State * L)
	{
		LuaExtensionLibrary::Register(L);
		LuaUIObjectProxy::RegisterMetatable(L);
	}

	int GetExtensionVersion(lua_State * L);
	int OsiPrint(lua_State* L);
	int OsiPrintError(lua_State* L);
	int JsonParse(lua_State * L);
	int JsonStringify(lua_State * L);
	int IsModLoaded(lua_State* L);
	int GetModLoadOrder(lua_State* L);
	int GetModInfo(lua_State* L);
	int GetStatEntries(lua_State * L);
	int GetSkillSet(lua_State * L);
	int GetEquipmentSet(lua_State * L);
	int StatGetAttribute(lua_State * L);
	int StatSetAttribute(lua_State * L);
	int StatAddCustomDescription(lua_State * L);
	int StatSetLevelScaling(lua_State * L);
	int GetStat(lua_State * L);
	int NewDamageList(lua_State * L);
	int OsirisIsCallableClient(lua_State * L);
	int AddPathOverride(lua_State * L);
	int LuaRandom(lua_State * L);
	int LuaRound(lua_State * L);


	int PostMessageToServer(lua_State * L)
	{
		auto channel = luaL_checkstring(L, 1);
		auto payload = luaL_checkstring(L, 2);

		auto & networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeClientMessage();
		if (msg != nullptr) {
			auto postMsg = msg->GetMessage().mutable_post_lua_message();
			postMsg->set_channel_name(channel);
			postMsg->set_payload(payload);
			networkMgr.ClientSend(msg);
		} else {
			OsiErrorS("Could not get free message!");
		}

		return 0;
	}


	class UIObjectRegistry
	{
	public:
		struct ObjectMetadata
		{
			uint32_t NextInvokeId;
		};

		ObjectMetadata * GetMetadata(ObjectHandle handle);
	};

	UIObjectRegistry gUIObjectRegistry;


	struct CustomUI : public ecl::EoCUI
	{
		CustomUI(osidbg::Path * path)
			: EoCUI(path)
		{}

		void OnFunctionCalled(const char * func, unsigned int a2, InvokeDataValue * arg)
		{
			// auto ui 

			EoCUI::OnFunctionCalled(func, a2, arg);
		}

		void Destroy(bool a1)
		{
			if (a1) {
				GameFree(this);
			}
		}

		const char * GetDebugName()
		{
			return "extender::CustomUI";
		}

		static UIObject * Creator(osidbg::Path * path)
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
			val.StringVal.Set(lua_tostring(L, index));
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
			lua_pushboolean(L, val.BoolVal);
			break;

		case InvokeDataValueType::IDV_Double:
			lua_pushnumber(L, val.DoubleVal);
			break;

		case InvokeDataValueType::IDV_String:
			lua_pushstring(L, val.StringVal.GetPtr());
			break;

		default:
			luaL_error(L, "Flash value of type %d cannot be passed to Lua", (unsigned)val.TypeId);
		}
	}


	UIObject * LuaUIObjectProxy::Get()
	{
		auto uiManager = GetStaticSymbols().UIObjectManager__GetInstance();
		return uiManager->Get(handle_);
	}


	char const * const LuaUIObjectProxy::MetatableName = "ecl::EoCUI";

	void LuaUIObjectProxy::PopulateMetatable(lua_State * L)
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

		lua_pushcfunction(L, &AddInvokeName);
		lua_setfield(L, -2, "AddInvokeName");

		lua_pushcfunction(L, &GetValue);
		lua_setfield(L, -2, "GetValue");

		lua_pushcfunction(L, &SetValue);
		lua_setfield(L, -2, "SetValue");

		lua_setfield(L, -2, "__index");
	}


	int LuaUIObjectProxy::LuaIndex(lua_State * L)
	{
		auto ui = Get();
		if (ui == nullptr) return luaL_error(L, "UI object not available");

		auto prop = luaL_checkstring(L, 2);
		// FIXME

		return 0;
	}


	int LuaUIObjectProxy::SetPosition(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		int pos[2];
		pos[0] = (int)luaL_checkinteger(L, 2);
		pos[1] = (int)luaL_checkinteger(L, 3);

		ui->SetPos(pos);
		return 0;
	}


	int LuaUIObjectProxy::Resize(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		int pos[2];
		pos[0] = (int)luaL_checkinteger(L, 2);
		pos[1] = (int)luaL_checkinteger(L, 3);

		ui->FlashPlayer->SetSize(pos);
		return 0;
	}


	int LuaUIObjectProxy::Show(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		ui->Show();
		return 0;
	}


	int LuaUIObjectProxy::Hide(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		ui->Hide();
		return 0;
	}


	int LuaUIObjectProxy::Invoke(lua_State * L)
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
			OsiError("UI object has no invoke named '" << name << "'");
			return 0;
		}
		
		auto numArgs = lua_gettop(L) - 2;
		std::vector<InvokeDataValue> args;
		args.resize(numArgs);
		for (auto i = 0; i < numArgs; i++) {
			LuaToInvokeDataValue(L, i + 3, args[i]);
		}

		bool ok = ui->FlashPlayer->InvokeArgs(*invokeId, args.data(), numArgs);
		lua_pushboolean(L, ok);
		return 1;
	}


	int LuaUIObjectProxy::GotoFrame(lua_State * L)
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


	int LuaUIObjectProxy::AddInvokeName(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		auto name = luaL_checkstring(L, 2);
		auto index = ui->FlashPlayer->Invokes.Set.Size;
		ui->FlashPlayer->AddInvokeName(index, name);
		return 0;
	}


	int LuaUIObjectProxy::SetValue(lua_State * L)
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


	int LuaUIObjectProxy::GetValue(lua_State * L)
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

		InvokeDataValueType type = InvokeDataValueType::IDV_NoneVal;
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

		auto & sym = GetStaticSymbols();
		auto absPath = sym.ToPath(path, PathRootType::Data);

		auto uiManager = sym.UIObjectManager__GetInstance();
		std::optional<uint32_t> creatorId;
		uiManager->UIObjectCreators.Iterate([&absPath, &creatorId](uint32_t id, UIObjectFunctor * value) {
			if (strcmp(value->Path.Name.GetPtr(), absPath.c_str()) == 0) {
				creatorId = id;
			}
		});

		if (!creatorId) {
			auto creator = GameAlloc<UIObjectFunctor>();
			creator->Path.Name.Set(absPath);
			creator->CreateProc = CustomUI::Creator;

			sym.UIObjectManager__RegisterUIObjectCreator(uiManager, NextCustomCreatorId, creator);
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

		LuaUIObjectProxy::New(L, handle);
		return 1;
	}


	void LuaExtensionLibraryClient::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Version", GetExtensionVersion},
			{"Require", Require},
			{"Print", OsiPrint},
			{"PrintError", OsiPrintError},

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

			{"NewDamageList", NewDamageList},
			{"OsirisIsCallable", OsirisIsCallableClient},
			{"Random", LuaRandom},
			{"Round", LuaRound},

			{"AddPathOverride", AddPathOverride},
			{"PostMessageToServer", PostMessageToServer},
			{"CreateUI", CreateUI},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}


	LuaStateClient::LuaStateClient()
	{
		library_.Register(state_);

		auto baseLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY);
		LoadScript(baseLib, "BuiltinLibrary.lua");
		auto clientLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY_CLIENT);
		LoadScript(clientLib, "BuiltinLibraryClient.lua");

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_pushstring(L, "ExtraData"); // stack: Ext, "ExtraData"
		LuaStatsExtraDataProxy::New(L); // stack: Ext, "ExtraData", ExtraDataProxy
		lua_settable(L, -3); // stack: Ext
		lua_pop(L, 1); // stack: -

		// Ext is not writeable after loading SandboxStartup!
		auto sandbox = GetBuiltinLibrary(IDR_LUA_SANDBOX_STARTUP);
		LoadScript(sandbox, "SandboxStartup.lua");
	}

	bool LuaStateClient::SkillGetDescriptionParam(SkillPrototype * prototype, CDivinityStats_Character * character,
		ObjectSet<STDString> const & paramTexts, std::wstring & replacement)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_SkillGetDescriptionParam"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return false;
		}

		auto skill = stats->objects.Find(prototype->RPGStatsObjectIndex);
		if (skill == nullptr) {
			OsiError("Invalid RPGStats index in SkillPrototype?");
			return false;
		}

		auto luaSkill = LuaStatsProxy::New(L, skill, std::optional<int32_t>()); // stack: fn, skill
		LuaStatsPin _(luaSkill);
		auto luaCharacter = LuaObjectProxy<CDivinityStats_Character>::New(L, character); // stack: fn, skill, character
		LuaGameObjectPin<CDivinityStats_Character> _2(luaCharacter);

		for (uint32_t i = 0; i < paramTexts.Set.Size; i++) {
			lua_pushstring(L, paramTexts[i].GetPtr()); // stack: fn, skill, character, params...
		}

		if (CallWithTraceback(2 + paramTexts.Set.Size, 1) != 0) { // stack: retval
			OsiError("SkillGetDescriptionParam handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok;
		if (isnil) {
			ok = false;
		} else {
			auto str = lua_tostring(L, -1);
			if (str != nullptr) {
				replacement = FromUTF8(str);
				ok = true;
			} else {
				OsiErrorS("SkillGetDescriptionParam returned non-string value");
				ok = false;
			}
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}


	bool LuaStateClient::StatusGetDescriptionParam(StatusPrototype * prototype, CDivinityStats_Character * statusSource,
		CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, std::wstring & replacement)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_StatusGetDescriptionParam"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return false;
		}

		auto status = stats->objects.Find(prototype->RPGStatsObjectIndex);
		if (status == nullptr) {
			OsiError("Invalid RPGStats index in StatusPrototype?");
			return false;
		}

		if (character == nullptr) {
			character = statusSource;
		}

		auto luaStatus = LuaStatsProxy::New(L, status, std::optional<int32_t>()); // stack: fn, status
		LuaStatsPin _(luaStatus);

		auto luaSrcCharacter = LuaObjectProxy<CDivinityStats_Character>::New(L, statusSource); // stack: fn, status, srcCharacter, character
		LuaGameObjectPin<CDivinityStats_Character> _2(luaSrcCharacter);

		auto luaCharacter = LuaObjectProxy<CDivinityStats_Character>::New(L, character); // stack: fn, status, srcCharacter, character
		LuaGameObjectPin<CDivinityStats_Character> _3(luaCharacter);

		for (uint32_t i = 0; i < paramTexts.Set.Size; i++) {
			lua_pushstring(L, paramTexts[i].GetPtr()); // stack: fn, status, srcCharacter, character, params...
		}

		if (CallWithTraceback(3 + paramTexts.Set.Size, 1) != 0) { // stack: retval
			OsiError("StatusGetDescriptionParam handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok;
		if (isnil) {
			ok = false;
		} else {
			auto str = lua_tostring(L, -1);
			if (str != nullptr) {
				replacement = FromUTF8(str);
				ok = true;
			} else {
				OsiErrorS("StatusGetDescriptionParam returned non-string value");
				ok = false;
			}
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}


	ExtensionStateClient & ExtensionStateClient::Get()
	{
		return gOsirisProxy->GetClientExtensionState();
	}


	LuaState * ExtensionStateClient::GetLua()
	{
		if (Lua) {
			return Lua.get();
		} else {
			return nullptr;
		}
	}

	ModManager * ExtensionStateClient::GetModManager()
	{
		return GetModManagerClient();
	}

	void ExtensionStateClient::DoLuaReset()
	{
		Lua.reset();
		Lua = std::make_unique<LuaStateClient>();
	}

	void ExtensionStateClient::LuaStartup()
	{
		ExtensionState::LuaStartup();

		LuaClientPin lua(*this);
		auto gameState = GetStaticSymbols().GetClientState();
		if (gameState
			&& (*gameState == ClientGameState::LoadLevel
				|| (*gameState == ClientGameState::LoadModule && WasStatLoadTriggered())
				|| *gameState == ClientGameState::LoadSession
				|| *gameState == ClientGameState::LoadGMCampaign
				|| *gameState == ClientGameState::Paused
				|| *gameState == ClientGameState::PrepareRunning
				|| *gameState == ClientGameState::Running
				|| *gameState == ClientGameState::GameMasterPause)) {
			lua->OnModuleResume();
			OsiWarn("Client resume -- state " << (unsigned)*gameState);
		} else {
			OsiWarn("Client NO resume -- state " << (unsigned)*gameState);
		}
	}

}
