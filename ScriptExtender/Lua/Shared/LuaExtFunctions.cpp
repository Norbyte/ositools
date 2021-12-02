#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <Version.h>
#include <ScriptHelpers.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

#include <fstream>
#include <json/json.h>

namespace dse::lua
{
	void OsiArgsToStream(lua_State * L, std::stringstream & ss)
	{
		int nargs = lua_gettop(L);  /* number of arguments */

		lua_getglobal(L, "tostring");
		for (int i = 1; i <= nargs; i++) {
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */
			lua_call(L, 1, 1);
			const char * str = lua_tostring(L, -1);  /* get result */
			if (str == nullptr)
				luaL_error(L, "'tostring' must return a string to 'print'");
			if (i > 1) ss << "\t";
			ss << str;
			lua_pop(L, 1);  /* pop result */
		}
	}

	int GetExtensionVersion(lua_State* L)
	{
		return CurrentVersion;
	}

	WrapLuaFunction(GetExtensionVersion)

	std::optional<STDString> GetGameVersion(lua_State* L)
	{
		GameVersionInfo gameVersion;
		if (gExtender->GetLibraryManager().GetGameVersion(gameVersion)) {
			std::stringstream ss;
			ss << "v" << gameVersion.Major << "." << gameVersion.Minor << "." << gameVersion.Revision << "." << gameVersion.Build;
			return ss.str().c_str();
		} else {
			return {};
		}
	}

	WrapLuaFunction(GetGameVersion)

	int MonotonicTime(lua_State* L)
	{
		using namespace std::chrono;
		return (int)time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
	}

	WrapLuaFunction(MonotonicTime)

	int OsiPrint(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		if (ss.str() == "Condition 1") return 0;
		gExtender->LogOsirisMsg(ss.str());
		return 0;
	}

	int OsiPrintWarning(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		gExtender->LogOsirisWarning(ss.str());
		return 0;
	}

	int OsiPrintError(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		gExtender->LogLuaError(ss.str());
		return 0;
	}

	// int64 handle to double conversion hack for use in Flash external interface calls
	// (Some of the builtin functions treat handles as double values)
	double HandleToDouble(lua_State* L, ComponentHandle handle)
	{
		return *reinterpret_cast<double*>(&handle);
	}

	WrapLuaFunction(HandleToDouble)

	ComponentHandle DoubleToHandle(lua_State* L, double dbl)
	{
		return ComponentHandle(*reinterpret_cast<int64_t*>(&dbl));
	}

	WrapLuaFunction(DoubleToHandle)

	STDString GetHandleType(lua_State* L, ComponentHandle handle)
	{
		auto type = EnumInfo<ObjectType>::Find((ObjectType)handle.GetType());
		if (type) {
			return type.Str;
		} else {
			return "Unknown";
		}
	}

	WrapLuaFunction(GetHandleType)

	std::optional<STDString> LoadFile(lua_State* L, char const* path, std::optional<char const*> context)
	{
		if (!context || strcmp(*context, "user") == 0) {
			return script::LoadExternalFile(path, PathRootType::GameStorage);
		} else if (strcmp(*context, "data") == 0) {
			return script::LoadExternalFile(path, PathRootType::Data);
		} else {
			LuaError("Unknown file loading context: " << *context);
			return {};
		}
	}

	WrapLuaFunction(LoadFile)

	int SaveFile(lua_State* L, char const* path, char const* contents)
	{
		return script::SaveExternalFile(path, PathRootType::GameStorage, contents);
	}

	WrapLuaFunction(SaveFile)

	bool IsModLoaded(lua_State* L, char const* modNameGuid)
	{
		auto modUuid = NameGuidToFixedString(modNameGuid);
		if (modUuid) {
			auto modManager = gExtender->GetCurrentExtensionState()->GetModManager();
			for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
				if (mod.Info.ModuleUUID == modUuid) {
					return true;
				}
			}
		}

		return false;
	}

	WrapLuaFunction(IsModLoaded)

	int GetModLoadOrder(lua_State* L)
	{
		StackCheck _(L, 1);
		lua_newtable(L);

		auto modManager = gExtender->GetCurrentExtensionState()->GetModManager();
		auto & mods = modManager->BaseModule.LoadOrderedModules;
		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const & mod = mods[i];
			settable(L, i + 1, mod.Info.ModuleUUID.Str);
		}

		return 1;
	}

	int GetModInfo(lua_State* L)
	{
		StackCheck _(L, 1);
		Module const * module{ nullptr };
		auto modUuid = NameGuidToFixedString(luaL_checkstring(L, 1));
		if (modUuid) {
			auto modManager = gExtender->GetCurrentExtensionState()->GetModManager();
			for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
				if (mod.Info.ModuleUUID == modUuid) {
					module = &mod;
					break;
				}
			}
		}

		if (module != nullptr) {
			lua_newtable(L);
			setfield(L, "UUID", module->Info.ModuleUUID);
			setfield(L, "Name", module->Info.Name);
			setfield(L, "Version", module->Info.ModVersion.Ver);
			setfield(L, "PublishVersion", module->Info.PublishVersion.Ver);
			setfield(L, "Directory", module->Info.Directory);
			setfield(L, "Author", module->Info.Author);
			setfield(L, "Description", module->Info.Description);
			setfield(L, "ModuleType", module->Info.ModuleType);
			
			lua_newtable(L);
			auto & dependents = module->DependentModules;
			for (uint32_t i = 0; i < dependents.Size; i++) {
				auto const & mod = dependents[i];
				settable(L, i + 1, mod.Info.ModuleUUID);
			}
			lua_setfield(L, -2, "Dependencies");
		} else {
			push(L, nullptr);
		}

		return 1;
	}

	void LuaDebugBreak(lua_State* L)
	{
#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gExtender->GetLuaDebugger();
		if (debugger) {
			debugger->DebugBreak(L);
		}
#endif
	}

	WrapLuaFunction(LuaDebugBreak)


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

	unsigned GetDifficulty(lua_State * L)
	{
		return GetStaticSymbols().GetGlobalSwitches()->Difficulty;
	}

	WrapLuaFunction(GetDifficulty)

	STDString GetGameMode(lua_State * L)
	{
		auto gameMode = GetStaticSymbols().GetGlobalSwitches()->GameMode;
		switch (gameMode) {
		case 0: return "Campaign";
		case 1: return "GameMaster";
		case 2: return "Arena";
		default: return "";
		}
	}

	WrapLuaFunction(GetGameMode)

	bool IsDeveloperMode(lua_State * L)
	{
		return gExtender->GetConfig().DeveloperMode;
	}

	WrapLuaFunction(IsDeveloperMode)

	void AddPathOverride(lua_State * L, char const* path, char const* overridePath)
	{
		gExtender->AddPathOverride(path, overridePath);
	}

	WrapLuaFunction(AddPathOverride)

	std::optional<STDString> GetPathOverride(lua_State * L, char const* path)
	{
		return gExtender->GetPathOverride(path);
	}

	WrapLuaFunction(GetPathOverride)

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

	STDString GetTranslatedString(lua_State* L, char const* translatedStringKey, std::optional<char const*> fallbackText)
	{
		STDWString translated;
		if (script::GetTranslatedString(translatedStringKey, translated)) {
			return ToUTF8(translated);
		} else {
			return fallbackText ? *fallbackText : "";
		}
	}

	WrapLuaFunction(GetTranslatedString)

	std::tuple<STDString, FixedString> GetTranslatedStringFromKey(lua_State* L, FixedString key)
	{
		TranslatedString translated;
		if (script::GetTranslatedStringFromKey(key, translated)) {
			return { ToUTF8(translated.Handle.ReferenceString), translated.Handle.Handle };
		} else {
			return {};
		}
	}

	WrapLuaFunction(GetTranslatedStringFromKey)

	unsigned NextDynamicStringHandleId{ 1 };

	std::optional<STDString> CreateTranslatedString(lua_State* L, char const* keyStr, char const* value)
	{
		FixedString key(keyStr);

		STDString handleStr = "ExtStr_";
		handleStr += std::to_string(NextDynamicStringHandleId++);
		FixedString handle(handleStr.c_str());

		if (script::CreateTranslatedStringKey(key, handle)) {
			STDWString str(FromUTF8(value));
			if (script::CreateTranslatedString(handle, str)) {
				return handleStr;
			}
		}

		return {};
	}

	WrapLuaFunction(CreateTranslatedString)

	bool CreateTranslatedStringKey(lua_State* L, char const* keyStr, char const* handleStr)
	{
		FixedString key(keyStr);
		FixedString handle(handleStr);
		return script::CreateTranslatedStringKey(key, handle);
	}

	WrapLuaFunction(CreateTranslatedStringKey)

	bool CreateTranslatedStringHandle(lua_State* L, char const* handleStr, char const* value)
	{
		FixedString handle(handleStr);

		STDWString str(FromUTF8(value));
		return script::CreateTranslatedString(handle, str);
	}

	WrapLuaFunction(CreateTranslatedStringHandle)

	// Variation of Lua builtin math_random() with custom RNG
	int LuaRandom(lua_State *L)
	{
		auto state = gExtender->GetCurrentExtensionState();

		lua_Integer low, up;
		switch (lua_gettop(L)) {  /* check number of arguments */
		case 0: {  /* no arguments */
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			push(L, (lua_Number)dist(state->OsiRng));  /* Number between 0 and 1 */
			return 1;
		}
		case 1: {  /* only upper limit */
			low = 1;
			up = luaL_checkinteger(L, 1);
			break;
		}
		case 2: {  /* lower and upper limits */
			low = luaL_checkinteger(L, 1);
			up = luaL_checkinteger(L, 2);
			break;
		}
		default: return luaL_error(L, "wrong number of arguments");
		}
		/* random integer in the interval [low, up] */
		luaL_argcheck(L, low <= up, 1, "interval is empty");
#if LUA_VERSION_NUM > 501
		luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1,
			"interval too large");
#endif

		std::uniform_int_distribution<int64_t> dist(low, up);
		push(L, dist(state->OsiRng));
		return 1;
	}

	int64_t LuaRound(lua_State *L, double val)
	{
		return (int64_t)round(val);
	}

	WrapLuaFunction(LuaRound)

	char const * OsiToLuaTypeName(ValueType type)
	{
		switch (type) {
		case ValueType::Integer:
		case ValueType::Integer64:
			return "integer";

		case ValueType::Real:
			return "number";

		case ValueType::String:
			return "string";

		case ValueType::GuidString:
		case ValueType::SplineGuid:
		case ValueType::LevelTemplateGuid:
			return "string GUID";

		case ValueType::CharacterGuid:
			return "string Character GUID";

		case ValueType::ItemGuid:
			return "string Item GUID";

		case ValueType::TriggerGuid:
			return "string Trigger GUID";

		default:
			return "any";
		}
	}

	STDString GenerateIdeHelpers(bool builtinOnly)
	{
		STDString helpers;
		helpers.reserve(0x20000);

		STDString functionComment, functionDefn;

		auto functions = gExtender->GetServer().Osiris().GetGlobals().Functions;

		(*functions)->Iterate([&helpers, &functionComment, &functionDefn, builtinOnly](STDString const & key, Function const * func) {
			if (builtinOnly
				&& func->Type != FunctionType::Event
				&& func->Type != FunctionType::Call
				&& func->Type != FunctionType::Query
				&& func->Type != FunctionType::SysCall
				&& func->Type != FunctionType::SysQuery) {
				return;
			}

			functionComment.clear();
			functionDefn.clear();
			auto const & outParams = func->Signature->OutParamList;
			auto numOutParams = outParams.numOutParams();

			auto types = func->Signature->Params->Params.Head;
			for (auto i = 0; i < func->Signature->Params->Params.Size; i++) {
				types = types->Next;

				if (!outParams.isOutParam(i)) {
					functionComment += "--- @param arg";
					functionComment += std::to_string(i + 1);
					functionComment += " ";
					functionComment += OsiToLuaTypeName((ValueType)types->Item.Type);
					functionComment += "\r\n";
				}
			}

			if (numOutParams > 0) {
				auto types = func->Signature->Params->Params.Head;
				for (auto i = 0; i < func->Signature->Params->Params.Size; i++) {
					types = types->Next;

					if (outParams.isOutParam(i)) {
						functionComment += "--- @return ";
						functionComment += OsiToLuaTypeName((ValueType)types->Item.Type);
						functionComment += "\r\n";
					}
				}
			} else {
				if (func->Type == FunctionType::SysQuery
					|| func->Type == FunctionType::Query
					|| func->Type == FunctionType::UserQuery) {
					functionComment += "--- @return boolean Did the query succeed?\r\n";
				}
			}

			functionDefn += func->Signature->Name;
			functionDefn += " = function (";

			auto pendingInParams = func->Signature->Params->Params.Size - numOutParams;
			for (auto i = 0; i < func->Signature->Params->Params.Size; i++) {
				if (!outParams.isOutParam(i)) {
					functionDefn += "arg";
					functionDefn += std::to_string(i + 1);
					if (--pendingInParams > 0) {
						functionDefn += ", ";
					}
				}
			}

			functionDefn += ") end\r\n\r\n";

			helpers += functionComment;
			helpers += "Osi.";
			helpers += functionDefn;

			// Export global name if function is a builtin
			if (func->Type == FunctionType::Event
				|| func->Type == FunctionType::Call
				|| func->Type == FunctionType::Query
				|| func->Type == FunctionType::SysCall
				|| func->Type == FunctionType::SysQuery) {
				helpers += functionComment;
				helpers += functionDefn;
			}
		});

		return helpers;
	}

	void GenerateIdeHelpers(lua_State * L, std::optional<bool> builtinOnly)
	{
#if defined(OSI_EOCAPP)
		if (gExtender->GetConfig().DeveloperMode) {
#endif
			auto lua = State::FromLua(L);
			if (lua->RestrictionFlags & State::RestrictOsiris) {
				luaL_error(L, "GenerateIdeHelpers() can only be called when Osiris is available");
			}

			auto helpers = GenerateIdeHelpers(builtinOnly && *builtinOnly);

			auto path = GetStaticSymbols().ToPath("", PathRootType::Data);
			path += "Mods/";
			path += ToUTF8(GetModManagerServer()->BaseModule.Info.Directory);
			path += "/Story/RawFiles/Lua/OsiIdeHelpers.lua";

			std::ofstream f(path.c_str(), std::ios::out | std::ios::binary);
			if (!f.good()) {
				OsiError("Could not open file to save IDE helpers: '" << path << "'");
				return;
			}

			f.write(helpers.c_str(), helpers.size());
#if defined(OSI_EOCAPP)
		} else {
			OsiError("GenerateIdeHelpers() only supported in developer mode");
		}
#endif
	}

	WrapLuaFunction(GenerateIdeHelpers)

	void EnableExperimentalPropertyWrites(lua_State* L)
	{
		OsiWarn("EnableExperimentalPropertyWrites() is no longer needed in v53!");
	}

	WrapLuaFunction(EnableExperimentalPropertyWrites)

	void ShowErrorAndExitGame(lua_State* L, STDWString message)
	{
		gExtender->GetLibraryManager().ShowStartupError(message, true);
	}

	WrapLuaFunction(ShowErrorAndExitGame)

	int DebugDumpLifetimes(lua_State* L)
	{
		auto const& pool = State::FromLua(L)->GetLifetimePool().GetAllocator();

		unsigned l1free{ 0 }, l1partial{ 0 }, l1full{ 0 };
		unsigned l2free{ 0 }, l2partial{ 0 }, l2full{ 0 };
		unsigned l3free{ 0 }, l3partial{ 0 }, l3full{ 0 };
		unsigned totalObjs{ 0 };

		for (auto i = 0; i < std::size(pool.l1_); i++) {
			if (pool.l1_[i] == 0) l1full++;
			else if (pool.l1_[i] == 0xffffffffffffffffull) l1free++;
			else l1partial++;
		}

		for (auto i = 0; i < std::size(pool.l2_); i++) {
			if (pool.l2_[i] == 0) l2full++;
			else if (pool.l2_[i] == 0xffffffffffffffffull) l2free++;
			else l2partial++;
		}

		for (auto i = 0; i < std::size(pool.l3_); i++) {
			if (pool.l3_[i] == 0) l3full++;
			else if (pool.l3_[i] == 0xffffffffffffffffull) l3free++;
			else l3partial++;
			totalObjs += (unsigned)_mm_popcnt_u64(pool.l3_[i]);
		}

		std::cout << " === LIFETIME STATS === " << std::endl;
		std::cout << "L1: " << l1free << " free pages, " << l1partial << " partially saturated pages, " << l1full << " full pages" << std::endl;
		std::cout << "L2: " << l2free << " free pages, " << l2partial << " partially saturated pages, " << l2full << " full pages" << std::endl;
		std::cout << "L3: " << l3free << " free pages, " << l3partial << " partially saturated pages, " << l3full << " full pages" << std::endl;
		std::cout << "Objects: " << 262144 << " in pool, " << totalObjs << " free" << std::endl;

		return 0;
	}

	void DumpStack(lua_State* L)
	{
		auto top = lua_gettop(L);
		for (int idx = 1; idx <= top; idx++) {
			switch (lua_type(L, idx)) {
			case LUA_TNIL:
				INFO("<%d> nil");
				break;

			case LUA_TBOOLEAN:
				INFO("<%d> %s", idx, lua_toboolean(L, idx) ? "true" : "false");
				break;

			case LUA_TLIGHTUSERDATA:
				INFO("<%d> Handle %ull", idx, (uint64_t)lua_touserdata(L, idx));
				break;

			case LUA_TNUMBER:
				INFO("<%d> %f", idx, (float)lua_tonumber(L, idx));
				break;

			case LUA_TSTRING:
				INFO("<%d> String '%s'", idx, lua_tostring(L, idx));
				break;

			default:
				INFO("<%d> %s", idx, lua_typename(L, lua_type(L, idx)));
				break;
			}
		}
	}

	WrapLuaFunction(DumpStack)
}
