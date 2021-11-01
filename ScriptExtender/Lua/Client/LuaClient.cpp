#include <stdafx.h>
#include <Lua/Client/LuaBindingClient.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Extender/ScriptExtender.h>
#include <Extender/Client/ExtensionStateClient.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <GameDefinitions/GameObjects/Ai.h>
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


#include <Lua/Shared/LuaShared.inl>

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
		return GenericGetter(L, gPlayerCustomDataPropertyMap);
	}

	int ObjectProxy<ecl::PlayerCustomData>::NewIndex(lua_State* L)
	{
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

		if (prop == GFS.strDisplayNameOverride) {
			if (character->DisplayNameOverride != nullptr) {
				auto const& reference = character->DisplayNameOverride->Handle.ReferenceString;
				auto name = gTempStrings.Make(ToUTF8(reference));
				push(L, name);
			} else {
				push(L, nullptr);
			}
			return 1;
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
		auto slot = (uint32_t)checked_get<ItemSlot32>(L, 2);

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

	int ClientCharacterGetCustomStatValue(lua_State* L)
	{
		StackCheck _(L, 1);
		auto self = checked_get<ObjectProxy<ecl::Character>*>(L, 1);
		auto statId = checked_get<char const*>(L, 2);

		auto character = self->Get(L);
		if (character == nullptr) {
			push(L, nullptr);
			return 1;
		}

		auto entityWorld = ecl::GetEntityWorld();
		auto statsComponent = entityWorld->GetCustomStatsComponentByEntityHandle(character->Base.EntityObjectHandle, false);
		if (statsComponent == nullptr) {
			// The game UI displays nonexistent stat entries as zero, 
			// so we'll do the same in the API
			push(L, 0);
			return 1;
		}

		auto value = statsComponent->StatValues.Find(FixedString(statId));
		if (value) {
			push(L, *value);
		} else {
			push(L, 0);
		}

		return 1;
	}

	int ObjectProxy<ecl::Character>::Index(lua_State* L)
	{
		auto character = Get(L);
		if (!character) return 0;

		auto prop = luaL_checkstring(L, 2);
		FixedString propFS(prop);
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

		if (propFS == GFS.strGetCustomStat) {
			lua_pushcfunction(L, &ClientCharacterGetCustomStatValue);
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
		FixedString propFS(prop);

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

	int GetGameObject(lua_State* L)
	{
		LuaClientPin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve game object handle in restricted context");
		}

		StackCheck _(L, 1);
		Item* item = nullptr;
		Character* character = nullptr;
		Trigger* trigger = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TLIGHTUSERDATA:
		{
			auto handle = checked_get<ObjectHandle>(L, 1);
			if (handle) {
				switch ((ObjectType)handle.GetType()) {
				case ObjectType::ClientCharacter:
					character = GetEntityWorld()->GetCharacter(handle);
					break;

				case ObjectType::ClientItem:
					item = GetEntityWorld()->GetItem(handle);
					break;

				default:
					OsiError("Cannot resolve unsupported client handle type: " << handle.GetType());
					break;
				}
			}

			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, 1);
			character = GetEntityWorld()->GetCharacter(guid, false);
			item = GetEntityWorld()->GetItem(guid, false);
			break;
		}

		default:
			OsiError("Expected object GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
			push(L, nullptr);
			return 1;
		}

		if (item != nullptr) {
			ObjectHandle handle;
			item->GetObjectHandle(handle);
			ObjectProxy<Item>::New(L, handle);
			return 1;
		} else if (character != nullptr) {
			ObjectHandle handle;
			character->GetObjectHandle(handle);
			ObjectProxy<Character>::New(L, handle);
			return 1;
		} else {
			push(L, nullptr);
			return 1;
		}
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
		// FIXME - should be done from Lua state ext!
		auto & networkMgr = gExtender->GetClient().GetNetworkManager();
		auto msg = networkMgr.GetFreeMessage();
		if (msg != nullptr) {
			auto postMsg = msg->GetMessage().mutable_post_lua();
			postMsg->set_channel_name(channel);
			postMsg->set_payload(payload);
			networkMgr.Send(msg);
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

	bool CustomDrawIcon(UIObject* self, FlashCustomDrawCallback* callback);

	struct CustomUI : public ecl::EoCUI
	{
		CustomUI(dse::Path * path)
			: EoCUI(path)
		{}

		void OnFunctionCalled(const char * func, unsigned int numArgs, ig::InvokeDataValue * args) override
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

		void CustomDrawCallback(void* callback) override
		{
			auto cb = reinterpret_cast<FlashCustomDrawCallback*>(callback);
			if (CustomDrawIcon(this, cb)) {
				return;
			}

			EoCUI::CustomDrawCallback(callback);
		}

		void Destroy(bool free) override
		{
			EoCUI::Destroy(false);
			if (free) {
				GameFree(this);
			}
		}

		const char * GetDebugName() override
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
			push(L, nullptr);
			return 1;
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
			break;
		}

		case ig::DataType::Double:
		{
			double val;
			if (s.IgValueGetF64(path, nullptr, nullptr, &val) == 0) {
				push(L, val);
				return 1;
			}
			break;
		}

		case ig::DataType::String:
		case ig::DataType::WString:
		{
			int resultLength{ 0 };
			if (s.IgValueGetStringUTF8(path, nullptr, nullptr, 0x10000, nullptr, &resultLength) == 0) {
				STDString str;
				str.resize(resultLength);
				if (s.IgValueGetStringUTF8(path, nullptr, nullptr, resultLength, str.data(), &resultLength) == 0) {
					push(L, str);
					return 1;
				}
			}
			break;
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
			break;
		}

		push(L, nullptr);
		return 1;
	}


	UIObject * UIObjectProxy::Get()
	{
		auto uiManager = GetStaticSymbols().GetUIObjectManager();
		if (uiManager != nullptr) {
			// FIXME - TEMP CAST
			return (UIObject*)uiManager->Get(handle_);
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

		lua_pushcfunction(L, &EnableCustomDraw);
		lua_setfield(L, -2, "EnableCustomDraw");

		lua_pushcfunction(L, &SetCustomIcon);
		lua_setfield(L, -2, "SetCustomIcon");

		lua_pushcfunction(L, &ClearCustomIcon);
		lua_setfield(L, -2, "ClearCustomIcon");

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
		if (!ui || !ui->FlashPlayer || !ui->FlashPlayer->IggyPlayerRootPath) {
			push(L, nullptr);
			return 0;
		}

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


	// This needs to be persistent for the lifetime of the app, as we don't restore altered VMTs
	std::unordered_map<UIObject::VMT *, UIObject::CustomDrawCallbackProc> OriginalCustomDrawHandlers;

	std::unordered_map<ObjectHandle, std::unordered_map<STDWString, std::unique_ptr<CustomDrawStruct>>> UICustomIcons;

	bool UIDebugCustomDrawCalls{ false };

	bool CustomDrawIcon(UIObject* self, FlashCustomDrawCallback* callback)
	{
		auto customIcons = UICustomIcons.find(self->UIObjectHandle);
		if (customIcons != UICustomIcons.end()) {
			auto icon = customIcons->second.find(callback->Name);
			if (icon != customIcons->second.end() && icon->second->IconMesh != nullptr) {
				auto draw = GetStaticSymbols().ls__UIHelper__CustomDrawObject;
				draw(callback, icon->second->IconMesh);

				if (UIDebugCustomDrawCalls) {
					INFO("Custom draw callback handled: %s -> %s", ToUTF8(callback->Name).c_str(), icon->second->IconName.Str);
				}

				return true;
			}
		}

		return false;
	}

	void UIObjectCustomDrawCallback(UIObject* self, void* callback)
	{
		auto cb = reinterpret_cast<FlashCustomDrawCallback*>(callback);
		if (CustomDrawIcon(self, cb)) {
			return;
		}

		auto vmt = *reinterpret_cast<UIObject::VMT**>(self);
		auto handler = OriginalCustomDrawHandlers.find(vmt);
		if (handler != OriginalCustomDrawHandlers.end()) {
			if (UIDebugCustomDrawCalls) {
				INFO(L"Custom draw callback unhandled: %s", cb->Name);
			}
			handler->second(self, callback);
		} else {
			OsiError("Couldn't find original CustomDrawCallback handler for UI object");
		}
	}

	void DoEnableCustomDraw(UIObject* ui)
	{
		auto vmt = *reinterpret_cast<UIObject::VMT**>(ui);
		if (vmt->CustomDrawCallback == &UIObjectCustomDrawCallback) return;

		// Custom UI element draw calls are already handled, no need to hook them
		if (strcmp(ui->GetDebugName(), "extender::CustomUI") == 0) return;

		WriteAnchor _w((uint8_t*)vmt, sizeof(*vmt));
		OriginalCustomDrawHandlers.insert(std::make_pair(vmt, vmt->CustomDrawCallback));
		vmt->CustomDrawCallback = &UIObjectCustomDrawCallback;
	}

	int UIObjectProxy::EnableCustomDraw(lua_State* L)
	{
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		DoEnableCustomDraw(ui);
		return 0;
	}

	int UIObjectProxy::SetCustomIcon(lua_State* L)
	{
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		auto element = FromUTF8(checked_get<char const*>(L, 2));
		auto icon = checked_get<char const*>(L, 3);
		auto width = checked_get<int>(L, 4);
		auto height = checked_get<int>(L, 5);

		char const* materialGuid = "9169b076-6e8d-44a4-bb52-95eedf9eab63";
		if (lua_gettop(L) >= 6) {
			materialGuid = checked_get<char const*>(L, 6);
		}

		if (width < 1 || height < 1 || width > 1024 || height > 1024) {
			OsiError("Invalid icon size");
			return 0;
		}

		auto const& sym = GetStaticSymbols();
		auto vmt = sym.ls__CustomDrawStruct__VMT;
		auto clear = sym.ls__UIHelper__UIClearIcon;
		auto create = sym.ls__UIHelper__UICreateIconMesh;
		auto draw = sym.ls__UIHelper__CustomDrawObject;

		if (!vmt || !clear || !create || !draw) {
			OsiError("Not all UIHelper symbols are available");
			return 0;
		}

		auto customIcons = UICustomIcons.find(ui->UIObjectHandle);
		if (customIcons == UICustomIcons.end()) {
			UICustomIcons.insert(std::make_pair(ui->UIObjectHandle, std::unordered_map<STDWString, std::unique_ptr<CustomDrawStruct>>()));
		}

		customIcons = UICustomIcons.find(ui->UIObjectHandle);
		auto curIcon = customIcons->second.find(element);
		if (curIcon != customIcons->second.end()) {
			clear(curIcon->second.get());
			customIcons->second.erase(curIcon);
		}

		auto newIcon = std::make_unique<CustomDrawStruct>();
		newIcon->VMT = vmt;
		create(FixedString(icon), newIcon.get(), width, height, FixedString(materialGuid));

		if (newIcon->IconMesh) {
			customIcons->second.insert(std::make_pair(element, std::move(newIcon)));
			DoEnableCustomDraw(ui);
		} else {
			OsiError("Failed to load icon: " << icon);
		}

		return 0;
	}

	int UIObjectProxy::ClearCustomIcon(lua_State* L)
	{
		StackCheck _(L, 0);
		auto ui = CheckUserData(L, 1)->Get();
		if (!ui) return 0;

		auto element = FromUTF8(checked_get<char const*>(L, 2));

		auto customIcons = UICustomIcons.find(ui->UIObjectHandle);
		if (customIcons != UICustomIcons.end()) {
			auto curIcon = customIcons->second.find(element);
			if (curIcon != customIcons->second.end()) {
				auto clear = GetStaticSymbols().ls__UIHelper__UIClearIcon;
				clear(curIcon->second.get());
				customIcons->second.erase(curIcon);
			}
		}

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
			OsiWarn("Cannot capture UI invokes - UI element '" << ui->Path.Name << "' has no flash player!");
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

		// FIXME - TEMP CAST
		auto object = (UIObject*)uiManager->Get(handle);
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
		for (auto uiPtr : uiManager->Components) {
			// FIXME - TEMP CAST
			auto ui = (UIObject*)uiPtr;
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

	int UISetDirty(lua_State* L)
	{
		StackCheck _(L, 0);
		auto handle = checked_get<ObjectHandle>(L, 1);
		auto flags = checked_get<uint64_t>(L, 2);

		auto ui = GetStaticSymbols().GetUIObjectManager();
		if (ui && ui->CharacterDirtyFlags) {
			EnterCriticalSection(&ui->CriticalSection);
			auto curFlags = ui->CharacterDirtyFlags->Find(handle);
			if (curFlags != nullptr) {
				*curFlags |= flags;
			} else {
				*ui->CharacterDirtyFlags->Insert(handle) = flags;
			}
			LeaveCriticalSection(&ui->CriticalSection);
		}

		return 0;
	}

	int UIEnableCustomDrawCallDebugging(lua_State* L)
	{
		StackCheck _(L, 0);
		UIDebugCustomDrawCalls = checked_get<bool>(L, 1);
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
			playerIndex = checked_get<int>(L, 1);
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

	namespace audio
	{

	EoCSoundManager* GetSoundManager()
	{
		auto resourceMgr = GetStaticSymbols().GetResourceManager();
		if (resourceMgr != nullptr && resourceMgr->SoundManager != nullptr) {
			return resourceMgr->SoundManager;
		}

		LuaError("Sound manager is not available!");
		return nullptr;
	}

	std::optional<SoundObjectId> GetSoundObjectId(lua_State* L, int idx)
	{
		auto snd = GetSoundManager();
		if (!snd) return {};

		switch (lua_type(L, idx)) {
		case LUA_TNIL:
			return 0xffffffffffffffffull;

		case LUA_TSTRING:
		{
			auto name = checked_get<char const*>(L, idx);
			if (strcmp(name, "Global") == 0) {
				return 0xffffffffffffffffull;
			} else if (strcmp(name, "Music") == 0) {
				return snd->MusicHandle;
			} else if (strcmp(name, "Ambient") == 0) {
				return snd->AmbientHandle;
			} else if (strcmp(name, "HUD") == 0) {
				return snd->HUDHandle;
			} else if (strcmp(name, "GM") == 0) {
				return snd->GMSoundHandle;
			} else if (strcmp(name, "Player1") == 0) {
				return snd->PlayerSoundHandles[0];
			} else if (strcmp(name, "Player2") == 0) {
				return snd->PlayerSoundHandles[1];
			} else if (strcmp(name, "Player3") == 0) {
				return snd->PlayerSoundHandles[2];
			} else if (strcmp(name, "Player4") == 0) {
				return snd->PlayerSoundHandles[3];
			} else {
				LuaError("Unknown built-in sound object name: " << name);
				return {};
			}
		}

		case LUA_TLIGHTUSERDATA:
		{
			auto handle = checked_get<ObjectHandle>(L, idx);
			if (handle.GetType() == (uint32_t)ObjectType::ClientCharacter) {
				auto character = GetEntityWorld()->GetCharacter(handle);
				if (character) {
					return character->SoundObjectHandles[0];
				} else {
					return {};
				}
			} else {
				LuaError("Only character handles are supported as sound objects");
				return {};
			}
		}

		default:
			LuaError("Must specify nil, character handle or built-in name as sound object");
			return {};
		}
	}

	int SetSwitch(lua_State* L)
	{
		auto soundObject = GetSoundObjectId(L, 1);
		auto switchGroup = checked_get<char const*>(L, 2);
		auto state = checked_get<char const*>(L, 3);

		if (!soundObject) {
			push(L, false);
			return 1;
		}

		bool ok = GetSoundManager()->SetSwitch(switchGroup, state, *soundObject);
		push(L, ok);
		return 1;
	}

	int SetState(lua_State* L)
	{
		auto stateGroup = checked_get<char const*>(L, 1);
		auto state = checked_get<char const*>(L, 2);

		if (!GetSoundManager()) {
			push(L, false);
			return 1;
		}

		bool ok = GetSoundManager()->SetState(stateGroup, state);
		push(L, ok);
		return 1;
	}

	int SetRTPC(lua_State* L)
	{
		auto soundObject = GetSoundObjectId(L, 1);
		auto rtpcName = checked_get<char const*>(L, 2);
		auto value = checked_get<float>(L, 3);

		if (!soundObject) {
			push(L, false);
			return 1;
		}

		bool ok = GetSoundManager()->SetRTPCValue(*soundObject, rtpcName, value) == 1;
		push(L, ok);
		return 1;
	}

	int GetRTPC(lua_State* L)
	{
		auto soundObject = GetSoundObjectId(L, 1);
		auto rtpcName = checked_get<char const*>(L, 2);

		if (!soundObject) {
			push(L, nullptr);
			return 1;
		}

		float value = GetSoundManager()->GetRTPCValue(*soundObject, rtpcName);
		push(L, value);
		return 1;
	}

	int ResetRTPC(lua_State* L)
	{
		auto soundObject = GetSoundObjectId(L, 1);
		auto rtpcName = checked_get<char const*>(L, 2);

		if (!soundObject) {
			push(L, false);
			return 1;
		}

		bool ok = GetSoundManager()->ResetRTPCValue(*soundObject, rtpcName) == 1;
		push(L, ok);
		return 1;
	}

	int Stop(lua_State* L)
	{
		auto snd = GetSoundManager();
		if (!snd) {
			return 0;
		}

		if (lua_gettop(L) == 0) {
			snd->StopAll();
		} else {
			auto soundObject = GetSoundObjectId(L, 1);
			if (soundObject) {
				snd->StopAllOnObject(*soundObject);
			}
		}

		return 0;
	}

	int PauseAllSounds(lua_State* L)
	{
		auto snd = GetSoundManager();
		if (!snd) {
			return 0;
		}

		snd->PauseAllSounds();
		return 0;
	}

	int ResumeAllSounds(lua_State* L)
	{
		auto snd = GetSoundManager();
		if (!snd) {
			return 0;
		}

		snd->ResumeAllSounds();
		return 0;
	}

	int PostEvent(lua_State* L)
	{
		auto soundObject = GetSoundObjectId(L, 1);
		auto eventName = checked_get<char const*>(L, 2);
		float positionSec = 0.0f;
		if (lua_gettop(L) > 2) {
			positionSec = checked_get<float>(L, 3);
		}

		if (!soundObject) {
			push(L, false);
			return 1;
		}

		bool ok = GetSoundManager()->PostEvent(*soundObject, eventName, positionSec, nullptr);
		push(L, ok);
		return 1;
	}

	int PlayExternalSound(lua_State* L)
	{
		auto soundObject = GetSoundObjectId(L, 1);
		auto eventName = checked_get<char const*>(L, 2);
		auto path = checked_get<char const*>(L, 3);
		auto codecId = checked_get<unsigned int>(L, 4);

		if (!soundObject) {
			push(L, false);
			return 1;
		}

		Path lsPath;
		lsPath.Name = GetStaticSymbols().ToPath(path, PathRootType::Data);
		auto eventId = GetSoundManager()->GetIDFromString(eventName);
		bool ok = GetSoundManager()->PlayExternalSound(*soundObject, eventId, lsPath, codecId, nullptr);
		push(L, ok);
		return 1;
	}

	}



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
			{"GetHandleType", GetHandleTypeWrapper},

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
			{"GetGameObject", GetGameObject},
			{"GetAiGrid", GetAiGrid},
			{"NewDamageList", NewDamageList},
			{"GetSurfaceTemplate", GetSurfaceTemplate},
			{"OsirisIsCallable", OsirisIsCallableClientWrapper},
			{"IsDeveloperMode", IsDeveloperModeWrapper},
			{"GetGameMode", GetGameModeWrapper},
			{"GetDifficulty", GetDifficultyWrapper},
			{"Random", LuaRandom},
			{"Round", LuaRoundWrapper},

			// EXPERIMENTAL FUNCTIONS
			{"UpdateShroud", UpdateShroud},
			{"EnableExperimentalPropertyWrites", EnableExperimentalPropertyWritesWrapper},
			{"DumpStack", DumpStackWrapper},
			{"ShowErrorAndExitGame", ShowErrorAndExitGameWrapper},

			{"GetGameState", GetGameState},
			{"GetPickingState", GetPickingState},
			{"AddPathOverride", AddPathOverrideWrapper},
			{"GetPathOverride", GetPathOverrideWrapper},
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
			{"UISetDirty", UISetDirty},
			{"UIEnableCustomDrawCallDebugging", UIEnableCustomDrawCallDebugging},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -

		static const luaL_Reg soundLib[] = {
			{"SetSwitch", audio::SetSwitch},
			{"SetState", audio::SetState},
			{"SetRTPC", audio::SetRTPC},
			{"GetRTPC", audio::GetRTPC},
			{"ResetRTPC", audio::ResetRTPC},
			{"Stop", audio::Stop},
			{"PauseAllSounds", audio::PauseAllSounds},
			{"ResumeAllSounds", audio::ResumeAllSounds},
			{"PostEvent", audio::PostEvent},
			{"PlayExternalSound", audio::PlayExternalSound},
			{0,0}
		};

		lua_getglobal(L, "Ext"); // stack: Ext
		luaL_newlib(L, soundLib); // stack: ext, lib
		lua_setfield(L, -2, "Audio");
		lua_pop(L, 1);
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
		auto debugger = gExtender->GetLuaDebugger();
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
		if (gExtender) {
			auto debugger = gExtender->GetLuaDebugger();
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


	std::optional<STDString> ClientState::GetSkillPropertyDescription(CRPGStats_Object_Property_Extender* prop)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_GetSkillPropertyDescription"); // stack: fn
		LuaSerializer serializer(L, true);
		auto propRef = static_cast<CDivinityStats_Object_Property_Data*>(prop);
		SerializeObjectProperty(serializer, propRef);

		auto result = CheckedCall<std::optional<char const*>>(L, 1, "Ext.GetSkillPropertyDescription");
		if (result) {
			return std::get<0>(*result);
		} else {
			return {};
		}
	}


	void ClientState::OnAppInputEvent(InputEvent const& inputEvent)
	{
		StackCheck _(L, 0);

		PushExtFunction(L, "_OnInputEvent"); // stack: fn
		lua_newtable(L);
		settable(L, "EventId", inputEvent.EventId);
		settable(L, "InputDeviceId", inputEvent.InputDeviceId);
		settable(L, "InputPlayerIndex", inputEvent.InputPlayerIndex);
		settable(L, "Press", (inputEvent.Type & InputType::Press) == InputType::Press);
		settable(L, "Release", (inputEvent.Type & InputType::Release) == InputType::Release);
		settable(L, "ValueChange", (inputEvent.Type & InputType::ValueChange) == InputType::ValueChange);
		settable(L, "Hold", (inputEvent.Type & InputType::Hold) == InputType::Hold);
		settable(L, "Repeat", (inputEvent.Type & InputType::Repeat) == InputType::Repeat);
		settable(L, "AcceleratedRepeat", (inputEvent.Type & InputType::AcceleratedRepeat) == InputType::AcceleratedRepeat);

		CheckedCall<>(L, 1, "Ext.OnInputEvent");
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
}

namespace dse::ecl
{

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
