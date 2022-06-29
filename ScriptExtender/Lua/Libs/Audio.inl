#include <GameDefinitions/Symbols.h>

/// <lua_module>Audio</lua_module>
BEGIN_NS(ecl::lua::audio)

EoCSoundManager* GetSoundManager()
{
	auto resourceMgr = GetStaticSymbols().GetResourceManager();
	if (resourceMgr != nullptr && resourceMgr->SoundManager != nullptr) {
		return resourceMgr->SoundManager;
	}

	LuaError("Sound manager is not available!");
	return nullptr;
}

SoundObjectId GetSoundObjectId(lua_State* L, int idx)
{
	auto snd = GetSoundManager();
	if (!snd) return InvalidSoundObjectId;

	switch (lua_type(L, idx)) {
	case LUA_TNIL:
		return InvalidSoundObjectId;

	case LUA_TSTRING:
	{
		auto name = get<char const*>(L, idx);
		if (strcmp(name, "Global") == 0) {
			return InvalidSoundObjectId;
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
			luaL_error(L, "Unknown built-in sound object name: %s", name);
		}
	}

	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, idx);
		if (handle.GetType() == (uint32_t)ObjectHandleType::ClientCharacter) {
			auto character = GetEntityWorld()->GetComponent<Character>(handle);
			if (character) {
				return character->SoundObjectHandles[0];
			} else {
				luaL_error(L, "No character object exists with the specified handle");
				return InvalidSoundObjectId;
			}
		} else {
			luaL_error(L, "Only character handles are supported as sound objects");
			return InvalidSoundObjectId;
		}
	}

	default:
		luaL_error(L, "Must specify nil, character handle or built-in name as sound object");
		return InvalidSoundObjectId;
	}
}

END_NS()

BEGIN_NS(lua)

SoundObjectId do_get(lua_State* L, int index, Overload<SoundObjectId>)
{
	return ecl::lua::audio::GetSoundObjectId(L, index);
}

END_NS()

BEGIN_NS(ecl::lua::audio)

using namespace dse::lua;

bool SetSwitch(SoundObjectId soundObject, char const* switchGroup, char const* state)
{
	return GetSoundManager()->SetSwitch(switchGroup, state, soundObject);
}

bool SetState(char const* stateGroup, char const* state)
{
	return GetSoundManager()->SetState(stateGroup, state);
}

bool SetRTPC(SoundObjectId soundObject, char const* rtpcName, float value)
{
	return GetSoundManager()->SetRTPCValue(soundObject, rtpcName, value) == 1;
}

float GetRTPC(SoundObjectId soundObject, char const* rtpcName)
{
	return GetSoundManager()->GetRTPCValue(soundObject, rtpcName);
}

bool ResetRTPC(SoundObjectId soundObject, char const* rtpcName)
{
	return GetSoundManager()->ResetRTPCValue(soundObject, rtpcName) == 1;
}

void Stop(std::optional<SoundObjectId> soundObject)
{
	auto snd = GetSoundManager();
	if (!snd) {
		return;
	}

	if (soundObject) {
		snd->StopAllOnObject(*soundObject);
	} else {
		snd->StopAll();
	}
}

void PauseAllSounds()
{
	auto snd = GetSoundManager();
	if (!snd) {
		return;
	}

	snd->PauseAllSounds();
}

void ResumeAllSounds()
{
	auto snd = GetSoundManager();
	if (!snd) {
		return;
	}

	snd->ResumeAllSounds();
}

bool PostEvent(SoundObjectId soundObject, char const* eventName, std::optional<float> positionSec)
{
	return GetSoundManager()->PostEvent(soundObject, eventName, positionSec.value_or(0.0f), nullptr);
}

bool PlayExternalSound(SoundObjectId soundObject, char const* eventName, char const* path, unsigned int codecId)
{
	Path lsPath;
	lsPath.Name = GetStaticSymbols().ToPath(path, PathRootType::Data);
	auto eventId = GetSoundManager()->GetIDFromString(eventName);
	return GetSoundManager()->PlayExternalSound(soundObject, eventId, lsPath, codecId, nullptr);
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
