#include <stdafx.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include <Version.h>
#include "LuaBinding.h"
#include <fstream>
#include <json/json.h>

namespace osidbg
{
	void JsonParse(lua_State * L, Json::Value & val);

	void JsonParseArray(lua_State * L, Json::Value & val)
	{
		lua_newtable(L);
		int idx = 1;
		for (auto it = val.begin(), end = val.end(); it != end; ++it) {
			lua_pushinteger(L, idx++);
			JsonParse(L, *it);
			lua_settable(L, -3);
		}
	}

	void JsonParseObject(lua_State * L, Json::Value & val)
	{
		lua_newtable(L);
		for (auto it = val.begin(), end = val.end(); it != end; ++it) {
			JsonParse(L, it.key());
			JsonParse(L, *it);
			lua_settable(L, -3);
		}
	}

	void JsonParse(lua_State * L, Json::Value & val)
	{
		switch (val.type()) {
		case Json::nullValue:
			lua_pushnil(L);
			break;

		case Json::intValue:
			lua_pushinteger(L, val.asInt64());
			break;

		case Json::uintValue:
			lua_pushinteger(L, (int64_t)val.asUInt64());
			break;

		case Json::realValue:
			lua_pushnumber(L, val.asDouble());
			break;

		case Json::stringValue:
			lua_pushstring(L, val.asCString());
			break;

		case Json::booleanValue:
			lua_pushboolean(L, val.asBool() ? 1 : 0);
			break;

		case Json::arrayValue:
			JsonParseArray(L, val);
			break;

		case Json::objectValue:
			JsonParseObject(L, val);
			break;

		default:
			luaL_error(L, "Attempted to parse unknown Json value");
		}
	}


	int JsonParse(lua_State * L)
	{
		size_t length;
		auto json = luaL_checklstring(L, 1, &length);

		Json::CharReaderBuilder factory;
		std::unique_ptr<Json::CharReader> reader(factory.newCharReader());

		Json::Value root;
		std::string errs;
		if (!reader->parse(json, json + length, &root, &errs)) {
			return luaL_error(L, "Unable to parse JSON: %s", errs.c_str());
		}

		JsonParse(L, root);
		return 1;
	}

	Json::Value JsonStringify(lua_State * L, int index, int depth);

	Json::Value JsonStringifyObject(lua_State * L, int index, int depth)
	{
		Json::Value arr(Json::objectValue);
		lua_pushnil(L);

		if (index < 0) index--;

		while (lua_next(L, index) != 0) {
			Json::Value val(JsonStringify(L, -1, depth + 1));

			if (lua_type(L, -2) == LUA_TSTRING) {
				auto key = lua_tostring(L, -2);
				arr[key] = val;
			} else if (lua_type(L, -2) == LUA_TNUMBER) {
				lua_pushvalue(L, -2);
				auto key = lua_tostring(L, -1);
				arr[key] = val;
				lua_pop(L, 1);
			} else {
				throw std::runtime_error("Can only stringify string or number table keys");
			}

			lua_pop(L, 1);
		}

		return arr;
	}

	Json::Value JsonStringifyArray(lua_State * L, int index, int depth)
	{
		Json::Value arr(Json::arrayValue);
		lua_pushnil(L);

		if (index < 0) index--;

		while (lua_next(L, index) != 0) {
			arr.append(JsonStringify(L, -1, depth + 1));
			lua_pop(L, 1);
		}

		return arr;
	}

	bool JsonCanStringifyAsArray(lua_State * L, int index)
	{
		Json::Value arr(Json::objectValue);
		lua_pushnil(L);

		if (index < 0) index--;

		int next = 1;
		bool isArray = true;
		while (lua_next(L, index) != 0) {
			if (lua_isinteger(L, -2)) {
				auto key = lua_tointeger(L, -2);
				if (key != next++) {
					isArray = false;
				}
			} else {
				isArray = false;
			}

			lua_pop(L, 1);
		}

		return isArray;
	}

	Json::Value JsonStringifyTable(lua_State * L, int index, int depth)
	{
		if (JsonCanStringifyAsArray(L, index)) {
			return JsonStringifyArray(L, index, depth);
		} else {
			return JsonStringifyObject(L, index, depth);
		}
	}


	Json::Value JsonStringify(lua_State * L, int index, int depth)
	{
		if (depth > 64) {
			throw std::runtime_error("Recursion depth exceeded while stringifying JSON");
		}

		switch (lua_type(L, index)) {
		case LUA_TNIL:
			return Json::Value(Json::nullValue);

		case LUA_TBOOLEAN:
			return Json::Value(lua_toboolean(L, index) == 1);

		case LUA_TNUMBER:
			if (lua_isinteger(L, index)) {
				return Json::Value(lua_tointeger(L, index));
			} else {
				return Json::Value(lua_tonumber(L, index));
			}

		case LUA_TSTRING:
			return Json::Value(lua_tostring(L, index));

		case LUA_TTABLE:
			return JsonStringifyTable(L, index, depth);

		case LUA_TLIGHTUSERDATA:
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		default:
			throw std::runtime_error("Attempted to stringify a lightuserdata, userdata, function or thread value");
		}
	}


	int JsonStringify(lua_State * L)
	{
		int nargs = lua_gettop(L);
		if (nargs != 1) {
			return luaL_error(L, "JsonStringify expects exactly one parameter.");
		}

		Json::Value root;
		try {
			root = JsonStringify(L, 1, 0);
		} catch (std::runtime_error & e) {
			return luaL_error(L, "%s", e.what());
		}

		Json::StreamWriterBuilder builder;
		builder["indentation"] = "\t";
		std::stringstream ss;
		std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(root, &ss);

		int nargs2 = lua_gettop(L);
		lua_pushstring(L, ss.str().c_str());
		return 1;
	}


	int EnableStatOverride(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (!(lua->RestrictionFlags & LuaState::ScopeSessionLoad)) {
			return luaL_error(L, "EnableStatOverride() can only be called during session load");
		}

		auto stat = luaL_checkstring(L, 1);
		gOsirisProxy->GetFunctionLibrary().EnableStatOverride(stat);
		return 0;
	}

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
		lua_pushinteger(L, CurrentVersion);
		return 1;
	}

	int OsiPrint(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		gOsirisProxy->LogOsirisMsg(ss.str());
		return 0;
	}

	int OsiPrintError(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		gOsirisProxy->LogOsirisError(ss.str());
		return 0;
	}

	int IsModLoaded(lua_State* L)
	{
		auto modUuid = NameGuidToFixedString(luaL_checkstring(L, 1));
		if (modUuid) {
			auto & mods = GetModManager()->BaseModule.LoadOrderedModules.Set;
			for (uint32_t i = 0; i < mods.Size; i++) {
				auto const & mod = mods.Buf[i];
				if (mod.Info.ModuleUUID == modUuid) {
					lua_pushboolean(L, 1);
					return 1;
				}
			}
		}

		lua_pushboolean(L, 0);
		return 1;
	}

	int GetStatEntries(lua_State * L)
	{
		FixedString statType;
		if (!lua_isnil(L, 1)) {
			auto statTypeName = luaL_checkstring(L, 1);
			statType = ToFixedString(statTypeName);
			if (!statType) {
				OsiError("Invalid stat entry type: " << statTypeName);
				return 0;
			}
		}
		
		auto stats = gOsirisProxy->GetLibraryManager().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 0;
		}

		lua_newtable(L);
		auto & objects = stats->objects.Primitives;
		int32_t index = 1;
		for (uint32_t i = 0; i < objects.ItemCount; i++) {
			auto object = objects.Buf[i];
			if (statType) {
				auto type = stats->GetTypeInfo(object);
				if (type->Name != statType) {
					continue;
				}
			}

			lua_pushinteger(L, index++);
			lua_pushstring(L, object->Name);
			lua_settable(L, -3);
		}

		return 1;
	}


	int StatGetAttribute(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);

		auto stats = gOsirisProxy->GetLibraryManager().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 0;
		}

		auto object = stats->objects.Find(statName);
		if (object == nullptr) {
			OsiError("Stat object '" << statName << "' does not exist");
			return 0;
		}

		auto value = stats->GetAttributeFixedString(object, attributeName);
		if (!value) {
			auto intval = stats->GetAttributeInt(object, attributeName);
			if (!intval) {
				OsiError("Stat object '" << statName << "' has no attribute named '" << attributeName << "'");
				return 0;
			} else {
				lua_pushinteger(L, *intval);
			}
		} else {
			lua_pushstring(L, value->Str);
		}

		return 1;
	}


	int StatSetAttribute(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (!(lua->RestrictionFlags & LuaState::ScopeModuleLoad)) {
			return luaL_error(L, "StatSetAttribute() can only be called during module load");
		}

		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);
		auto valueType = lua_type(L, 3);

		auto stats = gOsirisProxy->GetLibraryManager().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 0;
		}

		auto object = stats->objects.Find(statName);
		if (object == nullptr) {
			OsiError("Stat object '" << statName << "' does not exist");
			return 0;
		}

		if (valueType == LUA_TSTRING) {
			auto value = luaL_checkstring(L, 3);
			stats->SetAttributeString(object, attributeName, value);
		} else if (valueType == LUA_TNUMBER) {
			auto value = (int32_t)luaL_checkinteger(L, 3);
			stats->SetAttributeInt(object, attributeName, value);
		} else {
			return luaL_error(L, "Expected a string or integer attribute value.");
		}

		return 0;
	}

	esv::Character * GetCharacter(lua_State * L, int index)
	{
		esv::Character * character = nullptr;
		switch (lua_type(L, index)) {
		case LUA_TNUMBER:
		{
			auto handle = ObjectHandle(lua_tointeger(L, index));
			character = FindCharacterByHandle(handle);
			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, index);
			character = FindCharacterByNameGuid(guid);
			break;
		}

		default:
			OsiError("Expected character GUID or handle");
			break;
		}

		return character;
	}

	int GetCharacter(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & LuaState::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve character handle in restricted context");
		}

		esv::Character * character = GetCharacter(L, 1);

		if (character != nullptr) {
			ObjectHandle handle;
			character->GetObjectHandle(&handle);
			LuaHandleProxy<esv::Character>::New(L, handle);
			return 1;
		} else {
			return 0;
		}
	}

	int GetItem(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & LuaState::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve item handle in restricted context");
		}

		esv::Item * item = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TNUMBER:
		{
			auto handle = ObjectHandle(lua_tointeger(L, 1));
			item = FindItemByHandle(handle);
			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, 2);
			item = FindItemByNameGuid(guid);
			break;
		}

		default:
			OsiError("Expected item GUID or handle");
			return 0;
		}

		if (item != nullptr) {
			ObjectHandle handle;
			item->GetObjectHandle(&handle);
			LuaHandleProxy<esv::Item>::New(L, handle);
			return 1;
		} else {
			return 0;
		}
	}

	int GetStatus(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (lua->RestrictionFlags & LuaState::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve status handle in restricted context");
		}

		esv::Character * character = GetCharacter(L, 1);
		if (character == nullptr) return 0;

		auto statusHandle = ObjectHandle(lua_tointeger(L, 2));
		auto status = character->GetStatusByHandle(statusHandle, true);
		if (status != nullptr) {
			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			LuaStatusHandleProxy::New(L, characterHandle, statusHandle);
			return 1;
		} else {
			OsiError("Character has no status with handle " << statusHandle.Handle);
			return 0;
		}
	}

	int OsirisIsCallable(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		bool allowed = (lua->RestrictionFlags & LuaState::RestrictOsiris) != 0;
		lua_pushboolean(L, allowed);
		return 1;
	}

	// Variation of Lua builtin math_random() with custom RNG
	int LuaRandom(lua_State *L)
	{
		auto & state = ExtensionState::Get();

		lua_Integer low, up;
		switch (lua_gettop(L)) {  /* check number of arguments */
		case 0: {  /* no arguments */
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			lua_pushnumber(L, (lua_Number)dist(state.OsiRng));  /* Number between 0 and 1 */
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
		luaL_argcheck(L, low >= 0 || up <= LUA_MAXINTEGER + low, 1,
			"interval too large");

		std::uniform_int_distribution<int64_t> dist(low, up);
		lua_pushinteger(L, (lua_Integer)dist(state.OsiRng));
		return 1;
	}

}
