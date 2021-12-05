#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <ScriptHelpers.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <fstream>

namespace dse::lua
{
	int NewDamageList(lua_State * L)
	{
		MakeObjectContainer<stats::DamagePairList>(L);
		return 1;
	}

	int GetSurfaceTemplate(lua_State* L)
	{
		auto surfaceType = get<SurfaceType>(L, 1);
		auto tmpl = GetStaticSymbols().GetSurfaceTemplate(surfaceType);
		MakeObjectRef(L, tmpl);
		return 1;
	}

	void AddVoiceMetaData(lua_State * L, char const* speakerGuid, char const* translatedStringKey, char const* source, 
		float length, std::optional<int> priority)
	{
		auto speakerMgr = GetStaticSymbols().eoc__SpeakerManager;
		if (speakerMgr == nullptr || *speakerMgr == nullptr || (*speakerMgr)->SpeakerMetaDataHashMap == nullptr) {
			OsiError("Speaker manager not initialized!");
			return;
		}

		auto speaker = (*speakerMgr)->SpeakerMetaDataHashMap->Insert(FixedString(speakerGuid));
		auto voiceMeta = speaker->Insert(FixedString(translatedStringKey));
		voiceMeta->CodecID = 4;
		voiceMeta->IsRecorded = true;
		voiceMeta->Length = (float)length;
		voiceMeta->Priority = priority ? *priority : 0;

		auto path = GetStaticSymbols().ToPath(source, PathRootType::Data);
		voiceMeta->Source.Name = path;
	}

	WrapLuaFunction(AddVoiceMetaData)
}
