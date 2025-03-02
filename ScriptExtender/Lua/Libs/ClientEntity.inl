#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/GameObjects/Player.h>
#include <filesystem>
#include <fstream>

BEGIN_SE()

void Effect::LuaPlay()
{
	Play();
}

void Effect::LuaPause()
{
	Pause();
}

void Effect::LuaStop()
{
	Stop();
}

void Effect::LuaStopImmediate()
{
	StopImmediate();
}

void Effect::LuaReloadParameters()
{
	ReloadParameters();
}

void Effect::LuaUpdateEffectParametersFromEffectResource()
{
	UpdateEffectParametersFromEffectResource();
}

END_SE()

/// <lua_module>Entity</lua_module>
BEGIN_NS(ecl::lua::ecs)

void MakeLegacyClientCharacterObjectRef(lua_State* L, ecl::Character* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<ecl::Character>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
	} else {
		push(L, nullptr);
	}
}

void MakeLegacyClientItemObjectRef(lua_State* L, ecl::Item* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<ecl::Item>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
	} else {
		push(L, nullptr);
	}
}

Character* LuaGetCharacter(lua_State* L, int index)
{
	switch (lua_type(L, index)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, index);
		return GetEntityWorld()->GetComponent<ecl::Character>(handle);
	}

	case LUA_TNUMBER:
	{
		NetId netId{ (uint32_t)lua_tointeger(L, index) };
		return GetEntityWorld()->GetComponent<ecl::Character>(netId);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, index);
		return GetEntityWorld()->GetComponent<ecl::Character>(guid);
	}

	default:
		OsiError("Expected character UUID, Handle or NetId");
		return nullptr;
	}
}

UserReturn GetCharacterLegacy(lua_State* L)
{
	ecl::Character* character = LuaGetCharacter(L, 1);
	MakeLegacyClientCharacterObjectRef(L, character);
	return 1;
}

Character* GetCharacter(lua_State* L)
{
	return LuaGetCharacter(L, 1);
}

Item* LuaGetItem(lua_State* L, int index)
{
	switch (lua_type(L, index)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, index);
		return GetEntityWorld()->GetComponent<ecl::Item>(handle);
	}

	case LUA_TNUMBER:
	{
		NetId netId{ (uint32_t)lua_tointeger(L, index) };
		return GetEntityWorld()->GetComponent<ecl::Item>(netId);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, index);
		return GetEntityWorld()->GetComponent<ecl::Item>(guid);
	}

	default:
		OsiError("Expected item UUID, Handle or NetId; got " << lua_typename(L, lua_type(L, index)));
		return nullptr;
	}
}

UserReturn GetItemLegacy(lua_State* L)
{
	ecl::Item* item = LuaGetItem(L, 1);
	MakeLegacyClientItemObjectRef(L, item);
	return 1;
}

Item* GetItem(lua_State* L)
{
	return LuaGetItem(L, 1);
}

CombatComponent* GetCombatComponent(ComponentHandle handle)
{
	return GetEntityWorld()->GetComponent<CombatComponent>(handle);
}

Inventory* GetInventory(ComponentHandle handle)
{
	auto factory = *GetStaticSymbols().ecl__InventoryFactory;
	return factory->Get(handle);
}

Status* GetStatus(lua_State* L)
{
	auto character = LuaGetCharacter(L, 1);
	if (character == nullptr) return 0;

	if (lua_type(L, 2) == LUA_TLIGHTUSERDATA) {
		auto statusHandle = get<ComponentHandle>(L, 2);
		return character->GetStatus(statusHandle);
	} else {
		auto index = lua_tointeger(L, 2);
		NetId statusNetId{ (uint32_t)index };
		return character->GetStatus(statusNetId);
	}
}

IEoCClientObject* GetGameObject(lua_State* L)
{
	switch (lua_type(L, 1)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, 1);
		return GetEntityWorld()->GetGameObject(handle);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, 1);
		return GetEntityWorld()->GetGameObject(guid);
	}

	default:
		OsiError("Expected object GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
		return nullptr;
	}
}

Projectile* LuaGetProjectile(lua_State* L, int index)
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (!level || !level->EntityManager || !level->EntityManager->ProjectileConversionHelpers.Factory) {
		OsiError("No current level!");
		return nullptr;
	}

	switch (lua_type(L, index)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, index);
		return level->EntityManager->ProjectileConversionHelpers.Factory->Get(handle);
	}

	case LUA_TNUMBER:
	{
		NetId netId{ (uint32_t)lua_tointeger(L, index) };
		return level->EntityManager->ProjectileConversionHelpers.Factory->FindByNetId(netId);
	}

	default:
		OsiError("Expected client projectile Handle or NetId; got " << lua_typename(L, lua_type(L, index)));
		return nullptr;
	}
}

Projectile* GetProjectile(lua_State* L)
{
	return LuaGetProjectile(L, 1);
}

fx::Effect* GetEffect(ComponentHandle handle)
{
	auto effectFactory = GetStaticSymbols().GetResourceManager()->EffectFactory;
	if (!effectFactory) {
		return nullptr;
	}

	return effectFactory->EffectFactory.Get(handle);
}

ObjectSet<fx::Effect*>* GetAllEffects()
{
	auto effectFactory = GetStaticSymbols().GetResourceManager()->EffectFactory;
	if (!effectFactory) {
		return nullptr;
	}

	return &effectFactory->EffectPool;
}

eoc::AiGrid* GetAiGrid()
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (!level || !level->AiGrid) {
		OsiError("Current level not available yet!");
		return 0;
	}

	return level->AiGrid;
}

eoc::VisionGrid* GetVisionGrid()
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (!level || !level->VisionGrid) {
		OsiError("Current level not available yet!");
		return 0;
	}

	return level->VisionGrid;
}

Level* GetCurrentLevel()
{
	return GetStaticSymbols().GetCurrentClientLevel();
}

PlayerManager* GetPlayerManager()
{
	return (ecl::PlayerManager*)*GetStaticSymbols().ls__PlayerManager__Instance;
}

TurnManager* GetTurnManager()
{
	return GetEntityWorld()->GetTurnManager();
}

void SetHighlight(short playerID, ComponentHandle handle, int highlightType, bool something)
{
	GetStaticSymbols().ecl_HighlightManager_SetHighlight(*GetStaticSymbols().ecl_HighlightManager, playerID, handle.Handle, highlightType, something);
}

void RefreshEquipmentVisuals(lua_State* L)
{
	auto character = LuaGetCharacter(L, 1);
	if (character == nullptr) return;
	GetStaticSymbols().ecl__Character__RefreshEquipmentVisuals(character);
}

bool SetPortrait(lua_State* L)
{
	auto character = LuaGetCharacter(L, 1);
	if (!character || !character->PlayerData || !character->PlayerData->CustomData.Initialized)
	{
		OsiError("Character doesn't exist or has no PlayerData & CustomData initialized");
		return false;
	}

	// Write DDS to CustomData portrait buffer
	ScratchBuffer& buff = character->PlayerData->CustomData.CustomIconImg;
	int64_t width = lua_tointeger(L, 2);
	int64_t height = lua_tointeger(L, 3);
	int32_t SIZE = 128 + (width * height * 8 / 16);
	lua_len(L, 4);
	int64_t streamLen = lua_tointegerx(L, -1, 0);
	if (streamLen != SIZE) {
		OsiError("DDS has wrong stream size");
		return false;
	}

	// Write portrait to a new buffer
	char* newBuffer = (char*)GameAllocRaw(SIZE, "CustomPortrait");
	int i = 0;
	auto stream = lua_tostring(L, 4);
	while (i < streamLen) {
		char byte = stream[i];
		newBuffer[i] = byte;
		i++;
	}
	std::string test = std::to_string(i);
	LogOsirisWarning(test);

	GameFree(buff.Buffer);
	buff.Buffer = newBuffer;
	buff.Size = SIZE;
	//buff.Size = i;
	// TODO set other fields?
	buff.Capacity = SIZE;
	buff.WritePosition = 0;
	buff.ReadPosition = 0;
	//buff.GrowSize = 10;

	return true;
}

STDString GetPortrait(lua_State* L)
{
	auto character = LuaGetCharacter(L, 1);
	if (!character || !character->PlayerData)
	{
		OsiError("Character doesn't exist or has no PlayerData")
		return "";
	}
	auto icon = character->PlayerData->CustomData.CustomIconImg;
	std::string str = std::string((char*)icon.Buffer, (char*)icon.Buffer + icon.Size);
	STDString newStr = STDString(str);
	return newStr;
}

ComponentHandle NullHandle()
{
	return ComponentHandle(ComponentHandle::NullHandle);
}

void RegisterEntityLib()
{
	DECLARE_MODULE(Entity, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(NullHandle)
	MODULE_FUNCTION(GetCharacter)
	MODULE_FUNCTION(GetCharacterLegacy)
	MODULE_FUNCTION(GetItem)
	MODULE_FUNCTION(GetItemLegacy)
	MODULE_FUNCTION(GetCombatComponent)
	MODULE_FUNCTION(GetInventory)
	MODULE_FUNCTION(GetStatus)
	MODULE_FUNCTION(GetGameObject)
	MODULE_FUNCTION(GetProjectile)
	MODULE_FUNCTION(GetEffect)
	MODULE_FUNCTION(GetAllEffects)
	MODULE_FUNCTION(GetAiGrid)
	MODULE_FUNCTION(GetVisionGrid)
	MODULE_FUNCTION(GetCurrentLevel)
	MODULE_FUNCTION(GetPlayerManager)
	MODULE_FUNCTION(GetTurnManager)
	MODULE_FUNCTION(SetHighlight)
	MODULE_FUNCTION(RefreshEquipmentVisuals)
	MODULE_FUNCTION(SetPortrait)
	MODULE_FUNCTION(GetPortrait)
	END_MODULE()
}

END_NS()
