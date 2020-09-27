#include <stdafx.h>
#include <Lua/LuaBindingClient.h>
#include <Lua/LuaSerializers.h>
#include <OsirisProxy.h>
#include <ExtensionStateClient.h>
#include <PropertyMaps.h>
#include <GameDefinitions/Ai.h>
#include "resource.h"

namespace dse::lua
{
	char const* const ObjectProxy<ecl::Status>::MetatableName = "ecl::Status";


	PropertyMapBase& ClientStatusToPropertyMap(ecl::Status* status)
	{
		// TODO - add property maps for statuses
		return gEclStatusPropertyMap;
	}

	ecl::Status* ObjectProxy<ecl::Status>::Get(lua_State* L)
	{
		if (obj_ == nullptr) luaL_error(L, "Status object no longer available");
		return obj_;
	}

	int ObjectProxy<ecl::Status>::Index(lua_State* L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Status object no longer available");

		StackCheck _(L, 1);
		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "StatusType") == 0) {
			push(L, obj_->GetStatusId());
			return 1;
		}

		auto& propertyMap = ClientStatusToPropertyMap(obj_);
		auto fetched = LuaPropertyMapGet(L, propertyMap, obj_, prop, true);
		if (!fetched) push(L, nullptr);
		return 1;
	}

	int ObjectProxy<ecl::Status>::NewIndex(lua_State* L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Status object no longer available");

		StackCheck _(L, 0);
		auto& propertyMap = ClientStatusToPropertyMap(obj_);
		return GenericSetter(L, propertyMap);
	}


#include <Lua/LuaShared.inl>

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

		StackCheck _(L, 1);
		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gPlayerCustomDataPropertyMap, customData, prop, true);
		if (!fetched) push(L, nullptr);
		return 1;
	}

	int ObjectProxy<ecl::PlayerCustomData>::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		return GenericSetter(L, gPlayerCustomDataPropertyMap);
	}


	char const* const ObjectProxy<ecl::Character>::MetatableName = "ecl::Character";

	void ClientGetInventoryItems(lua_State* L, ObjectHandle inventoryHandle)
	{
		lua_newtable(L);

		auto inventory = ecl::FindInventoryByHandle(inventoryHandle);
		if (inventory != nullptr) {
			int32_t index = 1;
			for (auto itemHandle : inventory->ItemsBySlot) {
				if (itemHandle) {
					auto item = ecl::GetEntityWorld()->GetItem(itemHandle);
					if (item != nullptr) {
						settable(L, index++, item->MyGuid);
					}
				}
			}
		}
	}

	int ClientCharacterFetchProperty(lua_State* L, ecl::Character* character, FixedString const& prop)
	{
		if (prop == GFS.strPlayerCustomData) {
			if (character->PlayerData != nullptr
				// Always false on the client for some reason
				/*&& character->PlayerData->CustomData.Initialized*/) {
				ObjectHandle handle;
				character->GetObjectHandle(handle);
				ObjectProxy<ecl::PlayerCustomData>::New(L, handle);
			} else {
				OsiError("Character has no player data, or custom data was not initialized.");
				push(L, nullptr);
			}
			return 1;
		}

		if (prop == GFS.strStats) {
			if (character->Stats != nullptr) {
				// FIXME - use handle based proxy
				ObjectProxy<CDivinityStats_Character>::New(L, character->Stats);
			} else {
				OsiError("Character has no stats.");
				push(L, nullptr);
			}
			return 1;
		}

		if (prop == GFS.strHandle) {
			push(L, character->Base.Component.Handle);
			return 1;
		}

		if (prop == GFS.strRootTemplate) {
			ObjectProxy<CharacterTemplate>::New(L, character->Template);
			return 1;
		}

		if (prop == GFS.strDisplayName) {
			return GameObjectGetDisplayName<ecl::Character>(L, character);
		}

		auto fetched = LuaPropertyMapGet(L, gEclCharacterPropertyMap, character, prop, true);
		if (!fetched) push(L, nullptr);
		return 1;
	}

	ecl::Character* ObjectProxy<ecl::Character>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto character = ecl::GetEntityWorld()->GetCharacter(handle_);
		if (character == nullptr) luaL_error(L, "Character handle invalid");
		return character;
	}

	int ClientCharacterGetInventoryItems(lua_State* L)
	{
		StackCheck _(L, 1);
		auto self = checked_get<ObjectProxy<ecl::Character>*>(L, 1);

		ClientGetInventoryItems(L, self->Get(L)->InventoryHandle);

		return 1;
	}

	int ClientCharacterGetItemBySlot(lua_State* L)
	{
		StackCheck _(L, 1);
		auto self = checked_get<ObjectProxy<ecl::Character>*>(L, 1);
		auto slot = (uint32_t)checked_get<ItemSlot>(L, 2);

		auto inventory = ecl::FindInventoryByHandle(self->Get(L)->InventoryHandle);
		if (inventory != nullptr && slot < inventory->ItemsBySlot.Size) {
			auto itemHandle = inventory->ItemsBySlot[slot];
			if (itemHandle) {
				auto item = ecl::GetEntityWorld()->GetItem(itemHandle);
				if (item != nullptr) {
					push(L, item->MyGuid);
					return 1;
				}
			}
		}

		lua_pushnil(L);
		return 1;
	}

	int ObjectProxy<ecl::Character>::Index(lua_State* L)
	{
		auto character = Get(L);
		if (!character) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto propFS = ToFixedString(prop);
		if (!propFS) {
			OsiError("Illegal property name: " << prop);
			return 0;
		}

		StackCheck _(L, 1);
		if (propFS == GFS.strGetInventoryItems) {
			lua_pushcfunction(L, &ClientCharacterGetInventoryItems);
			return 1;
		}

		if (propFS == GFS.strGetItemBySlot) {
			lua_pushcfunction(L, &ClientCharacterGetItemBySlot);
			return 1;
		}

		if (propFS == GFS.strHasTag) {
			lua_pushcfunction(L, &GameObjectHasTag<ecl::Character>);
			return 1;
		}

		if (propFS == GFS.strGetTags) {
			lua_pushcfunction(L, &GameObjectGetTags<ecl::Character>);
			return 1;
		}

		if (propFS == GFS.strGetStatus) {
			lua_pushcfunction(L, (&GameObjectGetStatus<ecl::Character, ecl::Status>));
			return 1;
		}

		if (propFS == GFS.strGetStatusByType) {
			lua_pushcfunction(L, (&GameObjectGetStatusByType<ecl::Character, ecl::Status>));
			return 1;
		}

		if (propFS == GFS.strGetStatuses) {
			lua_pushcfunction(L, (&GameObjectGetStatuses<ecl::Character>));
			return 1;
		}

		if (propFS == GFS.strGetStatusObjects) {
			lua_pushcfunction(L, (&GameObjectGetStatusObjects<ecl::Character, ecl::Status>));
			return 1;
		}

		if (propFS == GFS.strSetScale) {
			lua_pushcfunction(L, (&GameObjectSetScale<ecl::Character>));
			return 1;
		}

		return ClientCharacterFetchProperty(L, character, propFS);
	}

	int ObjectProxy<ecl::Character>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gEclCharacterPropertyMap);
	}


	char const* const ObjectProxy<ecl::Item>::MetatableName = "ecl::Item";

	ecl::Item* ObjectProxy<ecl::Item>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto item = ecl::GetEntityWorld()->GetItem(handle_);
		if (item == nullptr) luaL_error(L, "Item handle invalid");
		return item;
	}

	int ClientItemGetInventoryItems(lua_State* L)
	{
		StackCheck _(L, 1);
		auto self = checked_get<ObjectProxy<ecl::Item>*>(L, 1);

		ClientGetInventoryItems(L, self->Get(L)->InventoryHandle);

		return 1;
	}

	int ItemGetOwnerCharacter(lua_State* L)
	{
		StackCheck _(L, 1);
		auto self = checked_get<ObjectProxy<ecl::Item>*>(L, 1);

		auto inventory = ecl::FindInventoryByHandle(self->Get(L)->InventoryParentHandle);

		for (;;) {
			if (inventory == nullptr) {
				break;
			}

			auto parent = inventory->ParentHandle;
			if (parent.GetType() == (uint32_t)ObjectType::ClientItem) {
				auto item = ecl::GetEntityWorld()->GetItem(parent);
				if (item) {
					inventory = ecl::FindInventoryByHandle(item->InventoryParentHandle);
				} else {
					break;
				}
			} else if (parent.GetType() == (uint32_t)ObjectType::ClientCharacter) {
				auto character = ecl::GetEntityWorld()->GetCharacter(parent);
				if (character) {
					push(L, character->MyGuid);
					return 1;
				} else {
					break;
				}
			} else {
				break;
			}
		}

		lua_pushnil(L);
		return 1;
	}

	int ObjectProxy<ecl::Item>::Index(lua_State* L)
	{
		auto item = Get(L);
		if (!item) return 0;

		StackCheck _(L, 1);
		auto prop = luaL_checkstring(L, 2);
		auto propFS = ToFixedString(prop);

		if (propFS == GFS.strGetInventoryItems) {
			lua_pushcfunction(L, &ClientItemGetInventoryItems);
			return 1;
		}

		if (propFS == GFS.strGetOwnerCharacter) {
			lua_pushcfunction(L, &ItemGetOwnerCharacter);
			return 1;
		}

		if (propFS == GFS.strGetDeltaMods) {
			lua_pushcfunction(L, &ItemGetDeltaMods<ecl::Item>);
			return 1;
		}

		if (propFS == GFS.strHasTag) {
			lua_pushcfunction(L, &GameObjectHasTag<ecl::Item>);
			return 1;
		}

		if (propFS == GFS.strGetTags) {
			lua_pushcfunction(L, &GameObjectGetTags<ecl::Item>);
			return 1;
		}

		if (propFS == GFS.strGetStatus) {
			lua_pushcfunction(L, (&GameObjectGetStatus<ecl::Item, ecl::Status>));
			return 1;
		}

		if (propFS == GFS.strGetStatusByType) {
			lua_pushcfunction(L, (&GameObjectGetStatusByType<ecl::Item, ecl::Status>));
			return 1;
		}

		if (propFS == GFS.strGetStatuses) {
			lua_pushcfunction(L, (&GameObjectGetStatuses<ecl::Item>));
			return 1;
		}

		if (propFS == GFS.strGetStatusObjects) {
			lua_pushcfunction(L, (&GameObjectGetStatusObjects<ecl::Item, ecl::Status>));
			return 1;
		}

		if (propFS == GFS.strStats) {
			if (item->Stats != nullptr) {
				// FIXME - use handle based proxy
				ObjectProxy<CDivinityStats_Item>::New(L, item->Stats);
				return 1;
			} else {
				OsiError("Item has no stats.");
				push(L, nullptr);
				return 1;
			}
		}

		if (propFS == GFS.strHandle) {
			push(L, item->Base.Component.Handle);
			return 1;
		}

		if (propFS == GFS.strRootTemplate) {
			ObjectProxy<ItemTemplate>::New(L, item->CurrentTemplate);
			return 1;
		}

		if (propFS == GFS.strDisplayName) {
			return GameObjectGetDisplayName<ecl::Item>(L, item);
		}

		bool fetched = false;
		if (item->Stats != nullptr) {
			fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item->Stats, propFS, false);
		}

		if (!fetched) {
			fetched = LuaPropertyMapGet(L, gEclItemPropertyMap, item, propFS, true);
		}

		if (!fetched) push(L, nullptr);
		return 1;
	}

	int ObjectProxy<ecl::Item>::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		return GenericSetter(L, gEclItemPropertyMap);
	}
}


namespace dse::ecl::lua
{
	using namespace dse::lua;

	void ExtensionLibraryClient::Register(lua_State * L)
	{
		ExtensionLibrary::Register(L);
		StatusHandleProxy::RegisterMetatable(L);
		ObjectProxy<Status>::RegisterMetatable(L);
		ObjectProxy<PlayerCustomData>::RegisterMetatable(L);
		ObjectProxy<Character>::RegisterMetatable(L);
		ObjectProxy<Item>::RegisterMetatable(L);
		UIObjectProxy::RegisterMetatable(L);
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
			auto handle = checked_get<ObjectHandle>(L, index);
			if (handle.GetType() == (uint32_t)ObjectType::ServerCharacter) {
				OsiError("Attempted to resolve server ObjectHandle on the client");
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
		StackCheck _(L, 1);
		LuaClientPin lua(ExtensionState::Get());

		ecl::Character* character = GetCharacter(L, 1);

		if (character != nullptr) {
			ObjectHandle handle;
			character->GetObjectHandle(handle);
			ObjectProxy<ecl::Character>::New(L, handle);
		} else {
			push(L, nullptr);
		}

		return 1;
	}

	int GetItem(lua_State* L)
	{
		StackCheck _(L, 1);
		LuaClientPin lua(ExtensionState::Get());

		ecl::Item* item = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TLIGHTUSERDATA:
		{
			auto handle = checked_get<ObjectHandle>(L, 1);
			if (handle.GetType() == (uint32_t)ObjectType::ServerItem) {
				OsiError("Attempted to resolve server ObjectHandle on the client");
			} else {
				item = GetEntityWorld()->GetItem(handle);
			}
			break;
		}

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
			OsiError("Expected item UUID, Handle or NetId; got " << lua_typename(L, lua_type(L, 1)));
			return 0;
		}

		if (item != nullptr) {
			ObjectHandle handle;
			item->GetObjectHandle(handle);
			ObjectProxy<ecl::Item>::New(L, handle);
		} else {
			push(L, nullptr);
		}

		return 1;
	}

	int GetStatus(lua_State* L)
	{
		StackCheck _(L, 1);
		LuaClientPin lua(ExtensionState::Get());

		auto character = GetCharacter(L, 1);
		if (character == nullptr) return 0;

		ecl::Status* status;
		if (lua_type(L, 2) == LUA_TLIGHTUSERDATA) {
			auto statusHandle = checked_get<ObjectHandle>(L, 2);
			status = character->GetStatus(statusHandle);

			if (status != nullptr) {
				ObjectHandle characterHandle;
				character->GetObjectHandle(characterHandle);
				StatusHandleProxy::New(L, characterHandle, statusHandle);
				return 1;
			}

			OsiError("Character has no status with ObjectHandle 0x" << std::hex << statusHandle.Handle);
		} else {
			auto index = lua_tointeger(L, 2);
			NetId statusNetId{ (uint32_t)index };
			status = character->GetStatus(statusNetId);

			if (status != nullptr) {
				ObjectHandle characterHandle;
				character->GetObjectHandle(characterHandle);
				StatusHandleProxy::New(L, characterHandle, statusNetId);
				return 1;
			}

			OsiError("Character has no status with NetId 0x" << std::hex << index);
		}

		push(L, nullptr);
		return 1;
	}

	int GetAiGrid(lua_State* L)
	{
		auto level = GetStaticSymbols().GetCurrentClientLevel();
		if (!level || !level->AiGrid) {
			OsiError("Current level not available yet!");
			return 0;
		}

		ObjectProxy<eoc::AiGrid>::New(L, level->AiGrid);
		return 1;
	}

	bool OsirisIsCallableClient(lua_State* L)
	{
		return false;
	}


	void PostMessageToServer(lua_State * L, char const* channel, char const* payload)
	{
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
	}

	char const* const StatusHandleProxy::MetatableName = "ecl::HStatus";

	ecl::Status* StatusHandleProxy::Get(lua_State* L)
	{
		auto character = GetEntityWorld()->GetCharacter(character_);
		if (character == nullptr) {
			luaL_error(L, "Character handle invalid");
			return nullptr;
		}

		ecl::Status* status;
		if (statusHandle_) {
			status = character->GetStatus(statusHandle_);
		} else {
			status = character->GetStatus(statusNetId_);
		}

		if (status == nullptr) luaL_error(L, "Status handle invalid");

		return status;
	}

	int StatusHandleProxy::Index(lua_State* L)
	{
		StackCheck _(L, 1);
		auto status = Get(L);

		auto prop = luaL_checkstring(L, 2);
		auto& propertyMap = ClientStatusToPropertyMap(status);
		auto fetched = LuaPropertyMapGet(L, propertyMap, status, prop, true);
		if (!fetched) push(L, nullptr);
		return 1;
	}

	int StatusHandleProxy::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		auto status = Get(L);
		if (!status) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto& propertyMap = ClientStatusToPropertyMap(status);
		LuaPropertyMapSet(L, 3, propertyMap, status, prop, true);
		return 0;
	}


	struct CustomUI : public ecl::EoCUI
	{
		CustomUI(dse::Path * path)
			: EoCUI(path)
		{}

		void OnFunctionCalled(const char * func, unsigned int numArgs, ig::InvokeDataValue * args)
		{
			{
				LuaClientPin lua(ExtensionState::Get());
				if (lua) {
					lua->OnUICall(UIObjectHandle, func, numArgs, args);
				}
			}

			EoCUI::OnFunctionCalled(func, numArgs, args);

			{
				LuaClientPin lua(ExtensionState::Get());
				if (lua) {
					lua->OnAfterUICall(UIObjectHandle, func, numArgs, args);
				}
			}
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


	void LuaToInvokeDataValue(lua_State * L, int index, ig::InvokeDataValue & val)
	{
		switch (lua_type(L, index)) {
		case LUA_TNUMBER:
			val.TypeId = ig::DataType::Double;
			val.DoubleVal = lua_tonumber(L, index);
			break;

		case LUA_TBOOLEAN:
			val.TypeId = ig::DataType::Bool;
			val.BoolVal = lua_toboolean(L, index);
			break;

		case LUA_TSTRING:
			val.TypeId = ig::DataType::String;
			val.StringVal = lua_tostring(L, index);
			break;

		default:
			luaL_error(L, "Cannot pass value of type %s to Flash", lua_typename(L, lua_type(L, index)));
		}
	}


	void InvokeDataValueToLua(lua_State * L, ig::InvokeDataValue const & val)
	{
		switch (val.TypeId) {
		case ig::DataType::None:
			lua_pushnil(L);
			break;

		case ig::DataType::Bool:
			push(L, val.BoolVal);
			break;

		case ig::DataType::Double:
			push(L, val.DoubleVal);
			break;

		case ig::DataType::String:
			push(L, val.StringVal);
			break;

		case ig::DataType::WString:
			push(L, val.WStringVal);
			break;

		default:
			OsiError("Flash value of type " << (unsigned)val.TypeId << " cannot be passed to Lua");
			lua_pushnil(L);
			break;
		}
	}


	UIFlashPath::UIFlashPath() {}

	UIFlashPath::UIFlashPath(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path)
		: paths_(parents)
	{
		paths_.push_back(*path);

		for (auto i = 1; i < paths_.size(); i++) {
			paths_[i].Parent = &paths_[i - 1];
		}
	}

	ig::IggyValuePath* UIFlashPath::Last()
	{
		return &*paths_.rbegin();
	}

	int PushFlashRef(lua_State* L, std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path);
	bool SetFlashValue(lua_State* L, ig::IggyValuePath* path, int idx);


	char const* const UIFlashObject::MetatableName = "FlashObject";

	UIFlashObject::UIFlashObject(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path)
		: path_(parents, path)
	{}

	int UIFlashObject::Index(lua_State* L)
	{
		StackCheck _(L, 1);
		ig::IggyValuePath path;
		auto name = checked_get<char const*>(L, 2);
		if (GetStaticSymbols().IgValuePathMakeNameRef(&path, path_.Last(), name)) {
			return PushFlashRef(L, path_.paths_, &path);
		} else {
			push(L, nullptr);
			return 1;
		}
	}

	int UIFlashObject::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		ig::IggyValuePath path;
		auto name = checked_get<char const*>(L, 2);
		if (GetStaticSymbols().IgValuePathMakeNameRef(&path, path_.Last(), name)) {
			SetFlashValue(L, &path, 3);
		}

		return 0;
	}



	char const* const UIFlashArray::MetatableName = "FlashArray";

	UIFlashArray::UIFlashArray(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path)
		: path_(parents, path)
	{}

	int UIFlashArray::Index(lua_State * L)
	{
		StackCheck _(L, 1);
		ig::IggyValuePath path;
		auto index = checked_get<int>(L, 2);
		if (GetStaticSymbols().IgValuePathPathMakeArrayRef(&path, path_.Last(), index, path_.Last()->Iggy)) {
			return PushFlashRef(L, path_.paths_, &path);
		} else {
			push(L, nullptr);
			return 1;
		}
	}

	int UIFlashArray::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		ig::IggyValuePath path;
		auto index = checked_get<int>(L, 2);
		if (GetStaticSymbols().IgValuePathPathMakeArrayRef(&path, path_.Last(), index, path_.Last()->Iggy)) {
			SetFlashValue(L, &path, 3);
		}

		return 0;
	}

	int UIFlashArray::Length(lua_State* L)
	{
		StackCheck _(L, 1);
		uint32_t length;
		if (GetStaticSymbols().IgValueGetArrayLength(path_.Last(), nullptr, nullptr, &length) == 0) {
			push(L, length);
			return 1;
		} else {
			push(L, nullptr);
			return 1;
		}
	}



	char const* const UIFlashFunction::MetatableName = "FlashFunction";

	UIFlashFunction::UIFlashFunction(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path)
		: path_(parents, path)
	{}

	bool LuaToFlashValue(lua_State * L, ig::IggyDataValue& value, int idx)
	{
		switch (lua_type(L, idx)) {
		case LUA_TNIL:
			value.TypeId = ig::DataType::None;
			break;

		case LUA_TBOOLEAN:
			value.TypeId = ig::DataType::Bool;
			value.Int64 = checked_get<bool>(L, idx) ? 1 : 0;
			break;

		case LUA_TNUMBER:
			value.TypeId = ig::DataType::Double;
			value.Double = checked_get<double>(L, idx);
			break;

		case LUA_TSTRING:
		{
			auto str = const_cast<char*>(checked_get<char const*>(L, idx));
			value.TypeId = ig::DataType::String;
			value.String = str;
			value.StringLength = (int)strlen(str);
			break;
		}

		default:
			OsiError("Can't convert Lua type '" << lua_typename(L, lua_type(L, idx)) << "' to Flash parameter!");
			return false;
		}

		return true;
	}

	int PushFlashValue(lua_State* L, ig::IggyDataValue const& value)
	{
		switch (value.TypeId) {
		case ig::DataType::None:
		case ig::DataType::Null:
			lua_pushnil(L);
			return 1;

		case ig::DataType::Bool:
			push(L, value.Int32 != 0);
			return 1;

		case ig::DataType::Double:
			push(L, value.Double);
			return 1;

		case ig::DataType::String:
			push(L, value.String);
			return 1;

		case ig::DataType::WString:
			push(L, ToUTF8(value.WString));
			return 1;

		default:
			OsiError("Don't know how to push Flash type " << (unsigned)value.TypeId << "!");
			lua_pushnil(L);
			return 1;
		}
	}

	int UIFlashFunction::LuaCall(lua_State* L)
	{
		StackCheck _(L, 1);
		int numArgs = lua_gettop(L) - 1;

		auto object = &path_.paths_[path_.paths_.size() - 2];
		auto method = path_.Last()->Name;

		std::vector<ig::IggyDataValue> args;
		args.resize(numArgs);

		for (auto i = 0; i < numArgs; i++) {
			LuaToFlashValue(L, args[i], i + 2);
		}

		ig::IggyDataValue result;
		if (GetStaticSymbols().IgPlayerCallMethod(object->Iggy, &result, object, method, numArgs, args.data()) != 0) {
			push(L, nullptr);
			return 1;
		}

		return PushFlashValue(L, result);
	}

	bool SetFlashValue(lua_State* L, ig::IggyValuePath* path, int idx)
	{
		auto const& s = GetStaticSymbols();

		switch (lua_type(L, idx)) {
		case LUA_TBOOLEAN:
		{
			auto val = checked_get<bool>(L, idx);
			return s.IgValueSetBoolean(path, nullptr, nullptr, val ? 1 : 0) == 0;
		}

		case LUA_TNUMBER:
		{
			auto val = checked_get<double>(L, idx);
			return s.IgValueSetF64(path, nullptr, nullptr, val) == 0;
		}

		case LUA_TSTRING:
		{
			auto val = checked_get<char const*>(L, idx);
			return s.IgValueSetStringUTF8(path, nullptr, nullptr, val, (int)strlen(val)) == 0;
		}

		case LUA_TUSERDATA:
		{
			auto arr = UIFlashArray::AsUserData(L, idx);
			if (arr != nullptr) {
				OsiError("Assigning Flash arrays to values not supported yet!");
				return false;
			}

			auto obj = UIFlashObject::AsUserData(L, idx);
			if (obj != nullptr) {
				OsiError("Assigning Flash objects to values not supported yet!");
				return false;
			}

			auto fun = UIFlashFunction::AsUserData(L, idx);
			if (fun != nullptr) {
				OsiError("Assigning Flash functions to values not supported yet!");
				return false;
			}

			OsiError("Only Flash array/object/function userdata types can be assigned to Flash values!");
			return false;
		}

		default:
			OsiError("Can't convert Lua type '" << lua_typename(L, lua_type(L, idx))  << "' to Flash!");
			return false;
		}
	}

	int PushFlashRef(lua_State* L, std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path)
	{
		auto const& s = GetStaticSymbols();

		ig::DataType type;
		if (s.IgValueGetType(path, nullptr, nullptr, &type) != 0) {
			return 0;
		}

		switch (type) {
		case ig::DataType::None:
		case ig::DataType::Null:
			lua_pushnil(L);
			return 1;

		case ig::DataType::Bool:
		{
			uint32_t val;
			if (s.IgValueGetBoolean(path, nullptr, nullptr, &val) == 0) {
				push(L, val != 0);
				return 1;
			}
			else {
				return 0;
			}
		}

		case ig::DataType::Double:
		{
			double val;
			if (s.IgValueGetF64(path, nullptr, nullptr, &val) == 0) {
				push(L, val);
				return 1;
			}
			else {
				return 0;
			}
		}

		case ig::DataType::String:
		case ig::DataType::WString:
		{
			int resultLength{ 0 };
			if (s.IgValueGetStringUTF8(path, nullptr, nullptr, 0x10000, nullptr, &resultLength) != 0) {
				return 0;
			}

			STDString str;
			str.resize(resultLength);
			if (s.IgValueGetStringUTF8(path, nullptr, nullptr, resultLength, str.data(), &resultLength) == 0) {
				push(L, str);
				return 1;
			} else {
				return 0;
			}
		}

		case ig::DataType::Array:
			UIFlashArray::New(L, parents, path);
			return 1;

		case ig::DataType::Object:
		case ig::DataType::Object2:
			UIFlashObject::New(L, parents, path);
			return 1;

		case ig::DataType::Function:
			UIFlashFunction::New(L, parents, path);
			return 1;

		default:
			OsiError("Don't know how to handle Flash type " << (unsigned)type << "!");
			return 0;
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

		lua_pushcfunction(L, &GetPlayerHandle);
		lua_setfield(L, -2, "GetPlayerHandle");

		lua_pushcfunction(L, &GetTypeId);
		lua_setfield(L, -2, "GetTypeId");

		lua_pushcfunction(L, &GetRoot);
		lua_setfield(L, -2, "GetRoot");

		lua_pushcfunction(L, &Destroy);
		lua_setfield(L, -2, "Destroy");

		lua_pushcfunction(L, &ExternalInterfaceCall);
		lua_setfield(L, -2, "ExternalInterfaceCall");

		lua_pushcfunction(L, &CaptureExternalInterfaceCalls);
		lua_setfield(L, -2, "CaptureExternalInterfaceCalls");

		lua_pushcfunction(L, &CaptureInvokes);
		lua_setfield(L, -2, "CaptureInvokes");

		lua_setfield(L, -2, "__index");
	}


	int UIObjectProxy::SetPosition(lua_State * L)
	{
		StackCheck _(L, 0);
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
		StackCheck _(L, 0);
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
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		ui->Show();
		return 0;
	}


	int UIObjectProxy::Hide(lua_State * L)
	{
		StackCheck _(L, 0);
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

		StackCheck _(L, 1);
		auto name = luaL_checkstring(L, 2);

		auto & invokes = ui->FlashPlayer->Invokes;
		std::optional<uint32_t> invokeId;
		for (uint32_t i = 0; i < invokes.Size; i++) {
			if (strcmp(name, invokes[i].Name) == 0) {
				invokeId = i;
				break;
			}
		}

		if (!invokeId) {
			invokeId = ui->FlashPlayer->Invokes.Size;
			ui->FlashPlayer->AddInvokeName(*invokeId, name);
		}

		auto numArgs = lua_gettop(L) - 2;
		std::vector<ig::InvokeDataValue> args;
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
		StackCheck _(L, 0);
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
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		auto root = ui->FlashPlayer->GetRootObject();
		if (!root) return 0;

		auto path = luaL_checkstring(L, 2);
		ig::InvokeDataValue value;
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

		StackCheck _(L, 1);
		auto path = luaL_checkstring(L, 2);
		auto typeName = lua_tostring(L, 3);
		int arrayIndex = -1;
		if (lua_gettop(L) >= 4) {
			arrayIndex = (int)luaL_checkinteger(L, 4);
		}

		ig::InvokeDataValue value;
		ig::DataType type{ ig::DataType::None };
		if (typeName != nullptr) {
			if (strcmp(typeName, "number") == 0) {
				type = ig::DataType::Double;
			} else if (strcmp(typeName, "boolean") == 0) {
				type = ig::DataType::Bool;
			} else if (strcmp(typeName, "string") == 0) {
				type = ig::DataType::String;
			} else {
				luaL_error(L, "Unknown value type for Flash fetch: %s", typeName);
			}
		}

		if (root->GetValueWorkaround(path, type, value, arrayIndex)) {
			InvokeDataValueToLua(L, value);
			return 1;
		} else {
			push(L, nullptr);
			return 1;
		}
	}


	int UIObjectProxy::GetHandle(lua_State * L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		push(L, ui->UIObjectHandle);
		return 1;
	}


	int UIObjectProxy::GetPlayerHandle(lua_State* L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		ObjectHandle handle;
		if (ui->Type == 104) {
			// ecl::UIExamine (104) doesn't implement GetPlayerHandle(), but we need it
			auto examine = reinterpret_cast<ecl::UIExamine*>(ui);
			handle = examine->ObjectBeingExamined;
		} else {
			ui->GetPlayerHandle(&handle);
		}

		if (handle) {
			push(L, handle);
		} else {
			lua_pushnil(L);
		}
		return 1;
	}


	int UIObjectProxy::GetTypeId(lua_State* L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		push(L, ui->Type);
		return 1;
	}

	int UIObjectProxy::GetRoot(lua_State* L)
	{
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui || !ui->FlashPlayer) return 0;

		StackCheck _(L, 1);
		std::vector<ig::IggyValuePath> path;
		return PushFlashRef(L, path, ui->FlashPlayer->IggyPlayerRootPath);
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
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		auto function = luaL_checkstring(L, 2);
		auto numArgs = lua_gettop(L) - 2;
		std::vector<ig::InvokeDataValue> args;
		args.resize(numArgs);
		for (auto i = 0; i < numArgs; i++) {
			LuaToInvokeDataValue(L, i + 3, args[i]);
		}

		ui->OnFunctionCalled(function, numArgs, args.data());
		return 0;
	}


	// This needs to be persistent for the lifetime of the app, as we don't restore altered VMTs
	std::unordered_map<UIObject::VMT *, UIObject::OnFunctionCalledProc> OriginalUIObjectCallHandlers;

	void UIObjectFunctionCallCapture(UIObject* self, const char* function, unsigned int numArgs, ig::InvokeDataValue* args)
	{
		{
			LuaClientPin lua(ExtensionState::Get());
			if (lua) {
				lua->OnUICall(self->UIObjectHandle, function, numArgs, args);
			}
		}

		auto vmt = *reinterpret_cast<UIObject::VMT**>(self);
		auto handler = OriginalUIObjectCallHandlers.find(vmt);
		if (handler != OriginalUIObjectCallHandlers.end()) {
			handler->second(self, function, numArgs, args);
		} else {
			OsiError("Couldn't find original OnFunctionCalled handler for UI object");
		}

		{
			LuaClientPin lua(ExtensionState::Get());
			if (lua) {
				lua->OnAfterUICall(self->UIObjectHandle, function, numArgs, args);
			}
		}
	}

	int UIObjectProxy::CaptureExternalInterfaceCalls(lua_State* L)
	{
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		// Custom UI element calls are captured by default, no need to hook them
		if (strcmp(ui->GetDebugName(), "extender::CustomUI") == 0) return 0;

		auto vmt = *reinterpret_cast<UIObject::VMT**>(ui);
		if (vmt->OnFunctionCalled == &UIObjectFunctionCallCapture) return 0;

		WriteAnchor _w((uint8_t*)vmt, sizeof(*vmt));
		OriginalUIObjectCallHandlers.insert(std::make_pair(vmt, vmt->OnFunctionCalled));
		vmt->OnFunctionCalled = &UIObjectFunctionCallCapture;

		return 0;
	}


	struct FlashPlayerHooks
	{
		bool Hooked{ false };
		ig::FlashPlayer::VMT* VMT{ nullptr };
		ig::FlashPlayer::VMT::Invoke6Proc OriginalInvoke6{ nullptr };
		ig::FlashPlayer::VMT::Invoke5Proc OriginalInvoke5{ nullptr };
		ig::FlashPlayer::VMT::Invoke4Proc OriginalInvoke4{ nullptr };
		ig::FlashPlayer::VMT::Invoke3Proc OriginalInvoke3{ nullptr };
		ig::FlashPlayer::VMT::Invoke2Proc OriginalInvoke2{ nullptr };
		ig::FlashPlayer::VMT::Invoke1Proc OriginalInvoke1{ nullptr };
		ig::FlashPlayer::VMT::Invoke0Proc OriginalInvoke0{ nullptr };
		ig::FlashPlayer::VMT::InvokeArgsProc OriginalInvokeArgs{ nullptr };

		void Hook(ig::FlashPlayer::VMT* vmt);
	};

	// Persistent for the lifetime of the app, as we don't restore FlashPlayer VMTs either
	FlashPlayerHooks gFlashPlayerHooks;

	ObjectHandle FindUIObjectHandle(ig::FlashPlayer* player)
	{
		auto uiManager = GetStaticSymbols().GetUIObjectManager();
		if (uiManager == nullptr) {
			OsiError("Couldn't get symbol for UIObjectManager!");
			return {};
		}

		for (auto const& ui : uiManager->UIObjects) {
			if (ui->FlashPlayer == player) {
				return ui->UIObjectHandle;
			}
		}

		return {};
	}

	template <class ...Args>
	void OnFlashPlayerPreInvoke(ig::FlashPlayer* self, int64_t invokeId, Args... args)
	{
		LuaClientPin lua(ExtensionState::Get());
		if (lua) {
			std::vector<ig::InvokeDataValue> invokeArgs{ (*args)... };
			lua->OnUIInvoke(FindUIObjectHandle(self), self->Invokes[(uint32_t)invokeId].Name, 
				(uint32_t)invokeArgs.size(), invokeArgs.data());
		}
	}

	template <class ...Args>
	void OnFlashPlayerPostInvoke(ig::FlashPlayer* self, int64_t invokeId, Args... args)
	{
		LuaClientPin lua(ExtensionState::Get());
		if (lua) {
			std::vector<ig::InvokeDataValue> invokeArgs{ (*args)... };
			lua->OnAfterUIInvoke(FindUIObjectHandle(self), self->Invokes[(uint32_t)invokeId].Name, 
				(uint32_t)invokeArgs.size(), invokeArgs.data());
		}
	}

	static bool FlashPlayerInvoke6Capture(ig::FlashPlayer* self, int64_t invokeId,
		ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3, ig::InvokeDataValue* a4, ig::InvokeDataValue* a5, ig::InvokeDataValue* a6)
	{
		OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3, a4, a5, a6);
		auto result = gFlashPlayerHooks.OriginalInvoke6(self, invokeId, a1, a2, a3, a4, a5, a6);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3, a4, a5, a6);
		return result;
	}

	static bool FlashPlayerInvoke5Capture(ig::FlashPlayer* self, int64_t invokeId,
		ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3, ig::InvokeDataValue* a4, ig::InvokeDataValue* a5)
	{
		OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3, a4, a5);
		auto result = gFlashPlayerHooks.OriginalInvoke5(self, invokeId, a1, a2, a3, a4, a5);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3, a4, a5);
		return result;
	}

	static bool FlashPlayerInvoke4Capture(ig::FlashPlayer* self, int64_t invokeId,
		ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3, ig::InvokeDataValue* a4)
	{
		OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3, a4);
		auto result = gFlashPlayerHooks.OriginalInvoke4(self, invokeId, a1, a2, a3, a4);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3, a4);
		return result;
	}

	static bool FlashPlayerInvoke3Capture(ig::FlashPlayer* self, int64_t invokeId,
		ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3)
	{
		OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3);
		auto result = gFlashPlayerHooks.OriginalInvoke3(self, invokeId, a1, a2, a3);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3);
		return result;
	}

	static bool FlashPlayerInvoke2Capture(ig::FlashPlayer* self, int64_t invokeId, ig::InvokeDataValue* a1, ig::InvokeDataValue* a2)
	{
		OnFlashPlayerPreInvoke(self, invokeId, a1, a2);
		auto result = gFlashPlayerHooks.OriginalInvoke2(self, invokeId, a1, a2);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2);
		return result;
	}

	static bool FlashPlayerInvoke1Capture(ig::FlashPlayer* self, int64_t invokeId, ig::InvokeDataValue* a1)
	{
		OnFlashPlayerPreInvoke(self, invokeId, a1);
		auto result = gFlashPlayerHooks.OriginalInvoke1(self, invokeId, a1);
		OnFlashPlayerPostInvoke(self, invokeId, a1);
		return result;
	}

	static bool FlashPlayerInvoke0Capture(ig::FlashPlayer* self, int64_t invokeId)
	{
		OnFlashPlayerPreInvoke(self, invokeId);
		auto result = gFlashPlayerHooks.OriginalInvoke0(self, invokeId);
		OnFlashPlayerPostInvoke(self, invokeId);
		return result;
	}

	static bool FlashPlayerInvokeArgsCapture(ig::FlashPlayer* self, int64_t invokeId, ig::InvokeDataValue* args, unsigned numArgs)
	{
		{
			LuaClientPin lua(ExtensionState::Get());
			if (lua) {
				lua->OnUIInvoke(FindUIObjectHandle(self), self->Invokes[(uint32_t)invokeId].Name, numArgs, args);
			}
		}

		auto result = gFlashPlayerHooks.OriginalInvokeArgs(self, invokeId, args, numArgs);

		{
			LuaClientPin lua(ExtensionState::Get());
			if (lua) {
				lua->OnAfterUIInvoke(FindUIObjectHandle(self), self->Invokes[(uint32_t)invokeId].Name, numArgs, args);
			}
		}

		return result;
	}

	void FlashPlayerHooks::Hook(ig::FlashPlayer::VMT* vmt)
	{
		if (Hooked) return;

		WriteAnchor _((uint8_t*)vmt, sizeof(*vmt));
		VMT = vmt;
		OriginalInvoke6 = vmt->Invoke6;
		OriginalInvoke5 = vmt->Invoke5;
		OriginalInvoke4 = vmt->Invoke4;
		OriginalInvoke3 = vmt->Invoke3;
		OriginalInvoke2 = vmt->Invoke2;
		OriginalInvoke1 = vmt->Invoke1;
		OriginalInvoke0 = vmt->Invoke0;
		OriginalInvokeArgs = vmt->InvokeArgs;

		vmt->Invoke6 = &FlashPlayerInvoke6Capture;
		vmt->Invoke5 = &FlashPlayerInvoke5Capture;
		vmt->Invoke4 = &FlashPlayerInvoke4Capture;
		vmt->Invoke3 = &FlashPlayerInvoke3Capture;
		vmt->Invoke2 = &FlashPlayerInvoke2Capture;
		vmt->Invoke1 = &FlashPlayerInvoke1Capture;
		vmt->Invoke0 = &FlashPlayerInvoke0Capture;
		vmt->InvokeArgs = &FlashPlayerInvokeArgsCapture;

		Hooked = true;
	}


	int UIObjectProxy::CaptureInvokes(lua_State* L)
	{
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		// Custom UI element calls are captured by default, no need to hook them
		if (strcmp(ui->GetDebugName(), "extender::CustomUI") == 0) return 0;

		if (ui->FlashPlayer == nullptr) {
			OsiError("Cannot capture UI invokes - UI element '" << ui->Path.Name << "' has no flash player!");
			return 0;
		}

		auto vmt = *reinterpret_cast<ig::FlashPlayer::VMT**>(ui->FlashPlayer);
		gFlashPlayerHooks.Hook(vmt);

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

		pin->OnCustomClientUIObjectCreated(name, handle);
		UIObjectProxy::New(L, handle);
		return 1;
	}

	int GetUI(lua_State * L)
	{
		StackCheck _(L, 1);
		auto name = luaL_checkstring(L, 1);

		LuaClientPin pin(ExtensionState::Get());
		auto ui = pin->GetUIObject(name);
		if (ui != nullptr) {
			UIObjectProxy::New(L, ui->UIObjectHandle);
		} else {
			push(L, nullptr);
		}

		return 1;
	}

	int GetUIByType(lua_State* L)
	{
		StackCheck _(L, 1);
		auto typeId = checked_get<int>(L, 1);

		UIObject* ui{ nullptr };
		auto uiManager = GetStaticSymbols().GetUIObjectManager();
		if (uiManager != nullptr) {
			ui = uiManager->GetByType(typeId);
		}

		if (ui != nullptr) {
			UIObjectProxy::New(L, ui->UIObjectHandle);
		} else {
			push(L, nullptr);
		}

		return 1;
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

		StackCheck _(L, 1);
		for (auto ui : uiManager->Objects) {
			if (ui != nullptr && ui->FlashPlayer != nullptr
				&& absPath == ui->Path.Name.c_str()) {
				UIObjectProxy::New(L, ui->UIObjectHandle);
				return 1;
			}
		}

		push(L, nullptr);
		return 1;
	}

	int DestroyUI(lua_State * L)
	{
		StackCheck _(L, 0);
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

	int UpdateShroud(lua_State* L)
	{
		StackCheck _(L, 0);
		auto x = checked_get<float>(L, 1);
		auto y = checked_get<float>(L, 2);
		auto layer = checked_get<ShroudType>(L, 3);
		auto value = checked_get<int>(L, 4);

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


	WrapLuaFunction(OsirisIsCallableClient)
	WrapLuaFunction(PostMessageToServer)

	void ExtensionLibraryClient::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Version", GetExtensionVersionWrapper},
			{"GameVersion", GetGameVersionWrapper},
			{"MonotonicTime", MonotonicTimeWrapper},
			{"Include", Include},
			{"Print", OsiPrint},
			{"PrintWarning", OsiPrintWarning},
			{"PrintError", OsiPrintError},
			{"HandleToDouble", HandleToDoubleWrapper},
			{"DoubleToHandle", DoubleToHandleWrapper},

			{"SaveFile", SaveFileWrapper},
			{"LoadFile", LoadFileWrapper},

			{"JsonParse", JsonParse},
			{"JsonStringify", JsonStringify},

			{"IsModLoaded", IsModLoadedWrapper},
			{"GetModLoadOrder", GetModLoadOrder},
			{"GetModInfo", GetModInfo},

			{"DebugBreak", LuaDebugBreakWrapper},

			{"GetStatEntries", GetStatEntries},
			{"GetStatEntriesLoadedBefore", GetStatEntriesLoadedBefore},
			{"GetSkillSet", GetSkillSet},
			{"UpdateSkillSet", UpdateSkillSet},
			{"GetEquipmentSet", GetEquipmentSet},
			{"UpdateEquipmentSet", UpdateEquipmentSet},
			{"GetTreasureTable", GetTreasureTable},
			{"UpdateTreasureTable", UpdateTreasureTable},
			{"GetTreasureCategory", GetTreasureCategory},
			{"UpdateTreasureCategory", UpdateTreasureCategory},
			{"GetItemCombo", GetItemCombo},
			{"UpdateItemCombo", UpdateItemCombo},
			{"GetItemComboPreviewData", GetItemComboPreviewData},
			{"UpdateItemComboPreviewData", UpdateItemComboPreviewData},
			{"GetItemComboProperty", GetItemComboProperty},
			{"UpdateItemComboProperty", UpdateItemComboProperty},
			{"GetItemGroup", GetItemGroup},
			{"GetNameGroup", GetNameGroup},

			{"StatGetAttribute", StatGetAttribute},
			{"StatSetAttribute", StatSetAttribute},
			{"StatAddCustomDescription", StatAddCustomDescriptionWrapper},
			{"StatSetLevelScaling", StatSetLevelScaling},
			{"GetStat", GetStat},
			{"CreateStat", CreateStat},
			{"SyncStat", SyncStatWrapper},
			{"GetDeltaMod", GetDeltaMod},
			{"UpdateDeltaMod", UpdateDeltaMod},
			{"EnumIndexToLabel", EnumIndexToLabel},
			{"EnumLabelToIndex", EnumLabelToIndex},

			{"GetCharacter", GetCharacter},
			{"GetItem", GetItem},
			{"GetStatus", GetStatus},
			{"GetAiGrid", GetAiGrid},
			{"NewDamageList", NewDamageList},
			{"OsirisIsCallable", OsirisIsCallableClientWrapper},
			{"IsDeveloperMode", IsDeveloperModeWrapper},
			{"Random", LuaRandom},
			{"Round", LuaRoundWrapper},

			// EXPERIMENTAL FUNCTIONS
			{"UpdateShroud", UpdateShroud},
			{"EnableExperimentalPropertyWrites", EnableExperimentalPropertyWritesWrapper},
			{"DumpStack", DumpStackWrapper},

			{"GetGameState", GetGameState},
			{"AddPathOverride", AddPathOverrideWrapper},
			{"AddVoiceMetaData", AddVoiceMetaDataWrapper},
			{"GetTranslatedString", GetTranslatedStringWrapper},
			{"GetTranslatedStringFromKey", GetTranslatedStringFromKeyWrapper},
			{"CreateTranslatedString", CreateTranslatedStringWrapper},
			{"CreateTranslatedStringKey", CreateTranslatedStringKeyWrapper},
			{"CreateTranslatedStringHandle", CreateTranslatedStringHandleWrapper},

			{"PostMessageToServer", PostMessageToServerWrapper},
			{"CreateUI", CreateUI},
			{"GetUI", GetUI},
			{"GetUIByType", GetUIByType},
			{"GetBuiltinUI", GetBuiltinUI},
			{"DestroyUI", DestroyUI},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}


	ClientState::ClientState()
	{
		StackCheck _(L, 0);
		library_.Register(L);

		auto baseLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY);
		LoadScript(baseLib, "BuiltinLibrary.lua");
		auto clientLib = GetBuiltinLibrary(IDR_LUA_BUILTIN_LIBRARY_CLIENT);
		LoadScript(clientLib, "BuiltinLibraryClient.lua");
		auto gameMathLib = GetBuiltinLibrary(IDR_LUA_GAME_MATH);
		LoadScript(gameMathLib, "Game.Math.lua");
		auto gameTooltipLib = GetBuiltinLibrary(IDR_LUA_GAME_TOOLTIP);
		LoadScript(gameTooltipLib, "Game.Tooltip.lua");

		lua_getglobal(L, "Ext"); // stack: Ext
		StatsExtraDataProxy::New(L); // stack: Ext, ExtraDataProxy
		lua_setfield(L, -2, "ExtraData"); // stack: Ext
		lua_pop(L, 1); // stack: -

		// Ext is not writeable after loading SandboxStartup!
		auto sandbox = GetBuiltinLibrary(IDR_LUA_SANDBOX_STARTUP);
		LoadScript(sandbox, "SandboxStartup.lua");

#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gOsirisProxy->GetLuaDebugger();
		if (debugger) {
			debugger->ClientStateCreated(this);
		}
#endif
	}

	ClientState::~ClientState()
	{
		auto & sym = GetStaticSymbols();
		auto uiManager = sym.GetUIObjectManager();
		for (auto & obj : clientUI_) {
			sym.UIObjectManager__DestroyUIObject(uiManager, &obj.second);
		}

#if !defined(OSI_NO_DEBUGGER)
		if (gOsirisProxy) {
			auto debugger = gOsirisProxy->GetLuaDebugger();
			if (debugger) {
				debugger->ClientStateDeleted();
			}
		}
#endif
	}

	void ClientState::OnCreateUIObject(ObjectHandle uiObjectHandle)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_UIObjectCreated");
		UIObjectProxy::New(L, uiObjectHandle);
		CheckedCall<>(L, 1, "Ext.UIObjectCreated");
	}

	void ClientState::OnUICall(ObjectHandle uiObjectHandle, const char * func, unsigned int numArgs, ig::InvokeDataValue * args)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_UICall"); // stack: fn

		UIObjectProxy::New(L, uiObjectHandle);
		push(L, func);
		push(L, "Before");
		for (uint32_t i = 0; i < numArgs; i++) {
			InvokeDataValueToLua(L, args[i]);
		}

		CheckedCall<>(L, 3 + numArgs, "Ext.UICall");
	}

	void ClientState::OnAfterUICall(ObjectHandle uiObjectHandle, const char* func, unsigned int numArgs, ig::InvokeDataValue* args)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_UICall"); // stack: fn

		UIObjectProxy::New(L, uiObjectHandle);
		push(L, func);
		push(L, "After");
		for (uint32_t i = 0; i < numArgs; i++) {
			InvokeDataValueToLua(L, args[i]);
		}

		CheckedCall<>(L, 3 + numArgs, "Ext.UICall");
	}

	void ClientState::OnUIInvoke(ObjectHandle uiObjectHandle, const char* func, unsigned int numArgs, ig::InvokeDataValue* args)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_UIInvoke"); // stack: fn

		UIObjectProxy::New(L, uiObjectHandle);
		push(L, func);
		push(L, "Before");
		for (uint32_t i = 0; i < numArgs; i++) {
			InvokeDataValueToLua(L, args[i]);
		}

		CheckedCall<>(L, 3 + numArgs, "Ext.UIInvoke");
	}

	void ClientState::OnAfterUIInvoke(ObjectHandle uiObjectHandle, const char* func, unsigned int numArgs, ig::InvokeDataValue* args)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_UIInvoke"); // stack: fn

		UIObjectProxy::New(L, uiObjectHandle);
		push(L, func);
		push(L, "After");
		for (uint32_t i = 0; i < numArgs; i++) {
			InvokeDataValueToLua(L, args[i]);
		}

		CheckedCall<>(L, 3 + numArgs, "Ext.UIInvoke");
	}

	std::optional<STDWString> ClientState::SkillGetDescriptionParam(SkillPrototype * prototype,
		CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, bool isFromItem)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);

		auto skill = prototype->GetStats();
		if (skill == nullptr) {
			return {};
		}

		PushExtFunction(L, "_SkillGetDescriptionParam"); // stack: fn

		auto _a{ PushArguments(L,
			std::tuple{Push<SkillPrototypeProxy>(prototype, std::optional<int32_t>()),
			Push<ObjectProxy<CDivinityStats_Character>>(character)}) };
		push(L, isFromItem);

		for (auto const& paramText : paramTexts) {
			push(L, paramText); // stack: fn, skill, character, params...
		}

		auto result = CheckedCall<std::optional<char const *>>(L, 3 + paramTexts.Size, "Ext.SkillGetDescriptionParam");
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
		StackCheck _(L, 0);
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

		auto result = CheckedCall<std::optional<char const *>>(L, 3 + paramTexts.Size, "Ext.StatusGetDescriptionParam");
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


	void ClientState::OnGameStateChanged(GameState fromState, GameState toState)
	{
		StackCheck _(L, 0);
		PushExtFunction(L, "_GameStateChanged"); // stack: fn
		push(L, fromState);
		push(L, toState);
		CheckedCall<>(L, 2, "Ext.GameStateChanged");
	}


	void ClientState::OnCustomClientUIObjectCreated(char const * name, ObjectHandle handle)
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
		}
	}

}
