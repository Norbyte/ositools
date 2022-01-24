#include <GameDefinitions/Symbols.h>

BEGIN_NS(ecl::lua::audio)

using namespace dse::lua;

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
		auto name = get<char const*>(L, idx);
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
		auto handle = get<ComponentHandle>(L, idx);
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
	auto switchGroup = get<char const*>(L, 2);
	auto state = get<char const*>(L, 3);

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
	auto stateGroup = get<char const*>(L, 1);
	auto state = get<char const*>(L, 2);

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
	auto rtpcName = get<char const*>(L, 2);
	auto value = get<float>(L, 3);

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
	auto rtpcName = get<char const*>(L, 2);

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
	auto rtpcName = get<char const*>(L, 2);

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
	auto eventName = get<char const*>(L, 2);
	float positionSec = 0.0f;
	if (lua_gettop(L) > 2) {
		positionSec = get<float>(L, 3);
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
	auto eventName = get<char const*>(L, 2);
	auto path = get<char const*>(L, 3);
	auto codecId = get<unsigned int>(L, 4);

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

void RegisterAudioLib()
{
	DECLARE_MODULE(Audio, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(SetSwitch)
	MODULE_FUNCTION(SetState)
	MODULE_FUNCTION(SetRTPC)
	MODULE_FUNCTION(GetRTPC)
	MODULE_FUNCTION(ResetRTPC)
	MODULE_FUNCTION(Stop)
	MODULE_FUNCTION(PauseAllSounds)
	MODULE_FUNCTION(ResumeAllSounds)
	MODULE_FUNCTION(PostEvent)
	MODULE_FUNCTION(PlayExternalSound)
	END_MODULE()
}

END_NS()
