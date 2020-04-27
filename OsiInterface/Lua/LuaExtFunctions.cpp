#include <stdafx.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include <Version.h>
#include <Lua/LuaBinding.h>
#include <ScriptHelpers.h>

#include <fstream>
#include <json/json.h>

namespace dse::lua
{
	void JsonParse(lua_State * L, Json::Value & val);

	void JsonParseArray(lua_State * L, Json::Value & val)
	{
		lua_newtable(L);
		int idx = 1;
		for (auto it = val.begin(), end = val.end(); it != end; ++it) {
			push(L, idx++);
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
			push(L, val.asInt64());
			break;

		case Json::uintValue:
			push(L, (int64_t)val.asUInt64());
			break;

		case Json::realValue:
			push(L, val.asDouble());
			break;

		case Json::stringValue:
			push(L, val.asCString());
			break;

		case Json::booleanValue:
			push(L, val.asBool());
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
#if LUA_VERSION_NUM > 501
			if (lua_isinteger(L, -2)) {
				auto key = lua_tointeger(L, -2);
				if (key != next++) {
					isArray = false;
				}
			} else {
				isArray = false;
			}
#else
			if (lua_isnumber(L, -2)) {
				auto key = lua_tonumber(L, -2);
				if (abs(key - next++) < 0.0001) {
					isArray = false;
				}
			} else {
				isArray = false;
			}
#endif

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
#if LUA_VERSION_NUM > 501
			if (lua_isinteger(L, index)) {
				return Json::Value(lua_tointeger(L, index));
			} else {
				return Json::Value(lua_tonumber(L, index));
			}
#else
			return Json::Value(lua_tonumber(L, index));
#endif

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
		if (nargs < 1) {
			return luaL_error(L, "JsonStringify expects at least one parameter.");
		}

		if (nargs > 2) {
			return luaL_error(L, "JsonStringify expects at most two parameters.");
		}

		bool beautify{ true };
		if (nargs >= 2) {
			beautify = lua_toboolean(L, 2) == 1;
		}

		Json::Value root;
		try {
			root = JsonStringify(L, 1, 0);
		} catch (std::runtime_error & e) {
			return luaL_error(L, "%s", e.what());
		}

		Json::StreamWriterBuilder builder;
		if (beautify) {
			builder["indentation"] = "\t";
		}
		std::stringstream ss;
		std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(root, &ss);

		push(L, ss.str());
		return 1;
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
		push(L, CurrentVersion);
		return 1;
	}

	int MonotonicTime(lua_State* L)
	{
		using namespace std::chrono;
		auto now = time_point_cast<milliseconds>(steady_clock::now()).time_since_epoch().count();
		push(L, now);
		return 1;
	}

	int OsiPrint(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		gOsirisProxy->LogOsirisMsg(ss.str());
		return 0;
	}

	int OsiPrintWarning(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		gOsirisProxy->LogOsirisWarning(ss.str());
		return 0;
	}

	int OsiPrintError(lua_State* L)
	{
		std::stringstream ss;
		OsiArgsToStream(L, ss);
		gOsirisProxy->LogOsirisError(ss.str());
		return 0;
	}

	int LoadFile(lua_State* L)
	{
		auto path = checked_get<char const*>(L, 1);

		auto loaded = script::LoadExternalFile(path);
		if (loaded) {
			push(L, *loaded);
			return 1;
		} else {
			return 0;
		}
	}

	int SaveFile(lua_State* L)
	{
		auto path = checked_get<char const*>(L, 1);
		auto contents = checked_get<char const*>(L, 2);

		push(L, script::SaveExternalFile(path, contents));
		return 1;
	}

	int IsModLoaded(lua_State* L)
	{
		auto modUuid = NameGuidToFixedString(luaL_checkstring(L, 1));
		if (modUuid) {
			auto modManager = gOsirisProxy->GetCurrentExtensionState()->GetModManager();
			for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
				if (mod.Info.ModuleUUID == modUuid) {
					push(L, true);
					return 1;
				}
			}
		}

		push(L, false);
		return 1;
	}

	int GetModLoadOrder(lua_State* L)
	{
		lua_newtable(L);

		auto modManager = gOsirisProxy->GetCurrentExtensionState()->GetModManager();
		auto & mods = modManager->BaseModule.LoadOrderedModules.Set;
		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const & mod = mods[i];
			settable(L, i + 1, mod.Info.ModuleUUID.Str);
		}

		return 1;
	}

	int GetModInfo(lua_State* L)
	{
		Module const * module{ nullptr };
		auto modUuid = NameGuidToFixedString(luaL_checkstring(L, 1));
		if (modUuid) {
			auto modManager = gOsirisProxy->GetCurrentExtensionState()->GetModManager();
			for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
				if (mod.Info.ModuleUUID == modUuid) {
					module = &mod;
					break;
				}
			}
		}

		if (module != nullptr) {
			lua_newtable(L);
			settable(L, "UUID", module->Info.ModuleUUID);
			settable(L, "Name", module->Info.Name);
			settable(L, "Version", module->Info.Version);
			settable(L, "PublishVersion", module->Info.PublishVersion);
			settable(L, "Directory", module->Info.Directory);
			settable(L, "Author", module->Info.Author);
			settable(L, "Description", module->Info.Description);
			settable(L, "ModuleType", module->Info.ModuleType);
			
			push(L, "Dependencies");
			lua_newtable(L);
			auto & dependents = module->DependentModules.Set;
			for (uint32_t i = 0; i < dependents.Size; i++) {
				auto const & mod = dependents[i];
				settable(L, i + 1, mod.Info.ModuleUUID);
			}
			lua_settable(L, -3);

			return 1;
		} else {
			return 0;
		}
	}

	void FetchSkillSetEntries(lua_State * L, CRPGStatsManager * stats)
	{
		int32_t index = 1;
		for (auto skillSet : stats->SkillSetManager->Primitives) {
			settable(L, index++, skillSet->Name);
		}
	}

	void FetchEquipmentSetEntries(lua_State * L, CRPGStatsManager * stats)
	{
		int32_t index = 1;
		for (auto equipmentSet : stats->EquipmentSetManager->Primitives) {
			settable(L, index++, equipmentSet->Name);
		}
	}

	void FetchStatEntries(lua_State * L, CRPGStatsManager * stats, FixedString const& statType)
	{
		int32_t index = 1;
		for (auto object : stats->objects.Primitives) {
			if (statType) {
				auto type = stats->GetTypeInfo(object);
				if (type == nullptr || type->Name != statType) {
					continue;
				}
			}

			settable(L, index++, object->Name);
		}
	}

	int GetStatEntries(lua_State * L)
	{
		char const * statTypeName{ nullptr };
		if (!lua_isnil(L, 1)) {
			statTypeName = luaL_checkstring(L, 1);
		}
		
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 0;
		}

		lua_newtable(L);
		if (statTypeName && strcmp(statTypeName, "SkillSet") == 0) {
			FetchSkillSetEntries(L, stats);
		} else if (statTypeName && strcmp(statTypeName, "EquipmentSet") == 0) {
			FetchEquipmentSetEntries(L, stats);
		} else {
			FixedString statType;
			if (statTypeName != nullptr) {
				statType = ToFixedString(statTypeName);
				if (!statType) {
					OsiError("Invalid stat entry type: " << statTypeName);
					return 0;
				}
			}

			FetchStatEntries(L, stats, statType);
		}

		return 1;
	}

	int GetSkillSet(lua_State * L)
	{
		auto skillSetName = luaL_checkstring(L, 1);

		auto skillSets = GetStaticSymbols().GetStats()->SkillSetManager;
		auto skillSet = skillSets->Find(skillSetName);
		if (skillSet == nullptr) {
			return 0;
		}

		lua_newtable(L);
		int32_t index = 1;
		for (auto const& skill : skillSet->Skills) {
			settable(L, index++, skill);
		}

		return 1;
	}

	int GetEquipmentSet(lua_State * L)
	{
		auto equipmentSetName = luaL_checkstring(L, 1);

		auto equipmentSets = GetStaticSymbols().GetStats()->EquipmentSetManager;
		auto equipmentSet = equipmentSets->Find(equipmentSetName);
		if (equipmentSet == nullptr) {
			return 0;
		}

		lua_newtable(L);
		int32_t index = 1;
		for (auto group = equipmentSet->FirstGroup; group != equipmentSet->LastGroup; group++) {
			for (auto const& equipment : (*group)->Equipment) {
				settable(L, index++, equipment);
			}
		}

		return 1;
	}

	void RequirementToLua(lua_State * L, CRPGStats_Requirement const & requirement)
	{
		lua_newtable(L);

		auto requirementLabel = EnumInfo<RequirementType>::Find(requirement.RequirementId);
		if (requirementLabel) {
			settable(L, "Requirement", requirementLabel);
		} else {
			OsiError("Unknown requirement ID: " << (unsigned)requirement.RequirementId);
			settable(L, "Requirement", "(Unknown)");
		}

		if (requirement.RequirementId == RequirementType::Tag) {
			settable(L, "Param", requirement.StringParam);
		} else {
			settable(L, "Param", requirement.IntParam);
		}
		
		settable(L, "Not", requirement.Negate);
	}

	void RequirementsToLua(lua_State * L, ObjectSet<CRPGStats_Requirement, GameMemoryAllocator, true> const & requirements)
	{
		lua_newtable(L);
		for (uint32_t i = 0; i < requirements.Set.Size; i++) {
			push(L, i + 1);
			RequirementToLua(L, requirements[i]);
			lua_settable(L, -3);
		}
	}

	void LuaToRequirement(lua_State * L, CRPGStats_Requirement & requirement)
	{
		auto requirementLabel = gettable<char const *, char const *>(L, "Requirement");
		auto requirementId = EnumInfo<RequirementType>::Find(requirementLabel);
		if (!requirementId) {
			luaL_error(L, "Unknown requirement type: %s", requirementLabel);
		}

		requirement.RequirementId = *requirementId;
		if (*requirementId == RequirementType::Tag) {
			auto param = gettable<char const *, char const *>(L, "Param");
			requirement.StringParam = MakeFixedString(param);
			requirement.IntParam = -1;
		} else {
			auto param = gettable<char const *, int32_t>(L, "Param");
			requirement.IntParam = param;
		}

		requirement.Negate = gettable<char const *, bool>(L, "Not");
	}

	void LuaToRequirements(lua_State * L, ObjectSet<CRPGStats_Requirement, GameMemoryAllocator, true> & requirements)
	{
#if LUA_VERSION_NUM > 501
		lua_len(L, -1);
		auto len = lua_tointeger(L, -1);
		lua_pop(L, 1);
#else
		auto len = lua_objlen(L, -1);
#endif

		requirements.Set.Reallocate((uint32_t)len);
		requirements.Set.Size = (uint32_t)len;

		for (uint32_t i = 0; i < requirements.Set.Size; i++) {
			push(L, i + 1);
			lua_gettable(L, -2);
			LuaToRequirement(L, requirements[i]);
			lua_pop(L, 1);
		}
	}

	void ObjectPropertyToLua(lua_State* L, CDivinityStats_Object_Property_Data const* property)
	{
		auto stats = GetStaticSymbols().GetStats();

		lua_newtable(L);

		push(L, "Context");
		lua_newtable(L);
		int ctxIndex = 1;
		if ((bool)(property->PropertyContext & CRPGStats_Object_PropertyContext::Target)) {
			settable(L, ctxIndex++, "Target");
		}
		if ((bool)(property->PropertyContext & CRPGStats_Object_PropertyContext::AoE)) {
			settable(L, ctxIndex++, "AoE");
		}
		if ((bool)(property->PropertyContext & CRPGStats_Object_PropertyContext::Self)) {
			settable(L, ctxIndex++, "Self");
		}
		if ((bool)(property->PropertyContext & CRPGStats_Object_PropertyContext::SelfOnHit)) {
			settable(L, ctxIndex++, "SelfOnHit");
		}
		if ((bool)(property->PropertyContext & CRPGStats_Object_PropertyContext::SelfOnEquip)) {
			settable(L, ctxIndex++, "SelfOnEquip");
		}
		lua_settable(L, -3);
		
		if (property->Conditions) {
			STDString name(property->Name.Str);
			if (name[name.length() - 1] == ')') {
				auto ifPos = name.find("_IF(");
				if (ifPos != std::string::npos) {
					auto condition = name.substr(ifPos + 4, name.length() - ifPos - 5);
					settable(L, "Condition", condition);
				}
			}
		}

		switch (property->TypeId) {
		case CRPGStats_Object_Property_Type::Custom:
		{
			auto prop = (CDivinityStats_Object_Property_Custom*)property;
			settable(L, "Type", "Custom");
			settable(L, "Action", prop->Name);
			break;
		}

		case CRPGStats_Object_Property_Type::Status:
		{
			auto prop = (CDivinityStats_Object_Property_Status*)property;
			settable(L, "Type", "Status");
			settable(L, "Action", prop->Status);
			settable(L, "StatusChance", prop->StatusChance);
			settable(L, "Duration", prop->Duration);
			settable(L, "Arg3", prop->Argument3);
			settable(L, "Arg4", prop->Argument4);
			settable(L, "Arg5", prop->Argument5);
			settable(L, "SurfaceBoost", prop->HasBoost);
			// FIXME - SurfaceBoosts
			break;
		}

		case CRPGStats_Object_Property_Type::SurfaceChange:
		{
			auto prop = (CDivinityStats_Object_Property_SurfaceChange*)property;
			auto surfaceChange = stats->EnumIndexToLabel(GFS.strSurfaceChange, prop->SurfaceChange);
			settable(L, "Type", "SurfaceChange");
			settable(L, "Action", surfaceChange);
			settable(L, "Arg1", prop->Arg1);
			settable(L, "Arg2", prop->Arg2);
			settable(L, "Arg3", prop->Arg3);
			settable(L, "Arg4", prop->Arg4);
			break;
		}

		case CRPGStats_Object_Property_Type::GameAction:
		{
			auto prop = (CDivinityStats_Object_Property_GameAction*)property;
			auto gameAction = stats->EnumIndexToLabel(GFS.strGameAction, prop->GameAction);
			settable(L, "Type", "GameAction");
			settable(L, "Action", gameAction);
			settable(L, "Arg1", prop->Arg1);
			settable(L, "Arg2", prop->Arg2);
			settable(L, "Arg3", prop->Arg3);
			settable(L, "Arg4", prop->Arg4);
			settable(L, "Arg5", prop->Arg5);
			auto statusHealType = stats->EnumIndexToLabel(GFS.strStatusHealType, prop->StatusHealType);
			if (statusHealType) {
				settable(L, "StatusHealType", statusHealType);
			}
			break;
		}

		case CRPGStats_Object_Property_Type::OsirisTask:
		{
			auto prop = (CDivinityStats_Object_Property_OsirisTask*)property;
			settable(L, "Type", "OsirisTask");
			auto osirisTask = stats->EnumIndexToLabel(GFS.strOsirisTask, prop->OsirisTask);
			settable(L, "Action", osirisTask);
			settable(L, "Chance", prop->Chance);
			settable(L, "VitalityOnRevive", prop->VitalityOnRevive);
			break;
		}

		case CRPGStats_Object_Property_Type::Sabotage:
		{
			auto prop = (CDivinityStats_Object_Property_Sabotage*)property;
			settable(L, "Type", "Sabotage");
			settable(L, "Action", "Sabotage");
			settable(L, "Amount", prop->Amount);
			break;
		}

		case CRPGStats_Object_Property_Type::Summon:
		{
			auto prop = (CDivinityStats_Object_Property_Summon*)property;
			settable(L, "Type", "Summon");
			settable(L, "Template", prop->Template);
			settable(L, "Duration", prop->Duration);
			settable(L, "IsTotem", prop->IsTotem);
			settable(L, "Skill", prop->Skill);
			break;
		}

		case CRPGStats_Object_Property_Type::Force:
		{
			auto prop = (CDivinityStats_Object_Property_Force*)property;
			settable(L, "Type", "Force");
			settable(L, "Action", "Force");
			settable(L, "Distance", prop->Distance);
			break;
		}

		default:
			WARN("Couldn't convert unknown property type %d to Lua!", property->TypeId);
		}
	}

	void ObjectPropertyListToLua(lua_State* L, CRPGStats_Object_Property_List const& properties)
	{
		lua_newtable(L);
		for (uint32_t i = 0; i < properties.Properties.Primitives.Set.Size; i++) {
			push(L, i + 1);
			auto prop = static_cast<CDivinityStats_Object_Property_Data*>(properties.Properties.Primitives[i]);
			ObjectPropertyToLua(L, prop);
			lua_settable(L, -3);
		}
	}

	struct ObjectPropertyInfo
	{
		CRPGStats_Object_Property_Type Type;
		void* VMT;
	};

	static bool ObjectPropertyVMTsMapped{ false };
	static void* SkillPropertiesVMT{ nullptr };
	static std::unordered_map<std::string, ObjectPropertyInfo> ObjectPropertyTypes = {
		{"Custom", {CRPGStats_Object_Property_Type::Custom, nullptr}},
		{"Status", {CRPGStats_Object_Property_Type::Status, nullptr}},
		{"SurfaceChange", {CRPGStats_Object_Property_Type::SurfaceChange, nullptr}},
		{"GameAction", {CRPGStats_Object_Property_Type::GameAction, nullptr}},
		{"OsirisTask", {CRPGStats_Object_Property_Type::OsirisTask, nullptr}},
		{"Sabotage", {CRPGStats_Object_Property_Type::Sabotage, nullptr}},
		{"Summon", {CRPGStats_Object_Property_Type::Summon, nullptr}},
		{"Force", {CRPGStats_Object_Property_Type::Force, nullptr}}
	};

	void MapObjectPropertyVMTs()
	{
		if (ObjectPropertyVMTsMapped) return;
		std::unordered_map<CRPGStats_Object_Property_Type, void*> VMTs;
		auto stats = GetStaticSymbols().GetStats();
		for (auto object : stats->objects.Primitives) {
			object->PropertyList.Iterate([&VMTs](auto const& k, auto const& propList) {
				SkillPropertiesVMT = *(void**)propList;
				for (auto prop : propList->Properties.Primitives) {
					VMTs.insert(std::make_pair(prop->TypeId, *(void**)prop));
				}
			});
		}

		for (auto& it : ObjectPropertyTypes) {
			auto vmtIt = VMTs.find(it.second.Type);
			if (vmtIt != VMTs.end()) {
				it.second.VMT = vmtIt->second;
			}
		}

		ObjectPropertyVMTsMapped = true;
	}

	CDivinityStats_Object_Property_Data* LuaToObjectProperty(lua_State* L, int index, FixedString const& propertyName)
	{
		auto stats = GetStaticSymbols().GetStats();

		auto type = checked_gettable<char const*, char const*>(L, "Type");
		auto typeIt = ObjectPropertyTypes.find(type);
		if (typeIt == ObjectPropertyTypes.end()) {
			OsiError("Unknown object property type: " << type);
			return nullptr;
		}

		if (typeIt->second.VMT == nullptr) {
			OsiError("Cannot construct object property of type  " << type << " - VMT not mapped!");
			return nullptr;
		}

		CDivinityStats_Object_Property_Data* prop{ nullptr };
		switch (typeIt->second.Type) {
		case CRPGStats_Object_Property_Type::Custom:
		{
			auto custom = new CDivinityStats_Object_Property_Custom();
			auto actionStr = checked_gettable<char const*, char const*>(L, "Action");
			auto action = stats->EnumLabelToIndex(GFS.strCustomProperties, actionStr);
			if (!action) {
				OsiError("Unknown Custom Properties value: " << actionStr);
				return nullptr;
			}
			custom->CustomProperties = *action;
			prop = custom;
			break;
		}

		case CRPGStats_Object_Property_Type::Status:
		{
			auto status = new CDivinityStats_Object_Property_Status();
			status->Status = MakeFixedString(checked_gettable<char const*, char const*>(L, "Action"));
			status->StatusChance = checked_gettable<char const*, float>(L, "StatusChance");
			status->Duration = checked_gettable<char const*, float>(L, "Duration");
			status->Argument3 = MakeFixedString(checked_gettable<char const*, char const*>(L, "Arg3"));
			status->Argument4 = checked_gettable<char const*, int>(L, "Arg4");
			status->Argument5 = checked_gettable<char const*, int>(L, "Arg5");
			status->HasBoost = checked_gettable<char const*, bool>(L, "SurfaceBoost");
			prop = status;
			break;
		}

		case CRPGStats_Object_Property_Type::SurfaceChange:
		{
			auto change = new CDivinityStats_Object_Property_SurfaceChange();
			auto actionStr = checked_gettable<char const*, char const*>(L, "Action");
			auto action = stats->EnumLabelToIndex(GFS.strSurfaceChange, actionStr);
			if (!action) {
				OsiError("Unknown Surface Change value: " << actionStr);
				return nullptr;
			}
			change->SurfaceChange = *action;

			change->Arg1 = checked_gettable<char const*, float>(L, "Arg1");
			change->Arg2 = checked_gettable<char const*, float>(L, "Arg2");
			change->Arg3 = checked_gettable<char const*, float>(L, "Arg3");
			change->Arg4 = checked_gettable<char const*, float>(L, "Arg4");
			prop = change;
			break;
		}

		case CRPGStats_Object_Property_Type::GameAction:
		{
			auto gameAction = new CDivinityStats_Object_Property_GameAction();
			auto actionStr = checked_gettable<char const*, char const*>(L, "Action");
			auto action = stats->EnumLabelToIndex(GFS.strGameAction, actionStr);
			if (!action) {
				OsiError("Unknown Custom Properties value: " << actionStr);
				return nullptr;
			}
			gameAction->GameAction = *action;

			gameAction->Arg1 = checked_gettable<char const*, float>(L, "Arg1");
			gameAction->Arg2 = checked_gettable<char const*, float>(L, "Arg2");
			gameAction->Arg3 = MakeFixedString(checked_gettable<char const*, char const*>(L, "Arg3"));
			gameAction->Arg4 = checked_gettable<char const*, float>(L, "Arg4");
			gameAction->Arg5 = checked_gettable<char const*, float>(L, "Arg5");
			auto statusHealType = gettable<char const*, char const*>(L, "StatusHealType");
			if (statusHealType) {
				auto healTypeIndex = stats->EnumLabelToIndex(GFS.strStatusHealType, statusHealType);
				if (!healTypeIndex) {
					OsiError("Unknown StatusHealType value: " << statusHealType);
					return nullptr;
				}

				gameAction->StatusHealType = *healTypeIndex;
			}

			prop = gameAction;
			break;
		}

		case CRPGStats_Object_Property_Type::OsirisTask:
		{
			auto osirisTask = new CDivinityStats_Object_Property_OsirisTask();
			auto actionStr = checked_gettable<char const*, char const*>(L, "Action");
			auto action = stats->EnumLabelToIndex(GFS.strOsirisTask, actionStr);
			if (!action) {
				OsiError("Unknown Osiris Task value: " << actionStr);
				return nullptr;
			}
			osirisTask->OsirisTask = *action;
			osirisTask->Chance = checked_gettable<char const*, float>(L, "Chance");
			osirisTask->VitalityOnRevive = checked_gettable<char const*, int>(L, "VitalityOnRevive");
			prop = osirisTask;
			break;
		}

		case CRPGStats_Object_Property_Type::Sabotage:
		{
			auto sabotage = new CDivinityStats_Object_Property_Sabotage();
			sabotage->Amount = checked_gettable<char const*, int>(L, "Amount");
			prop = sabotage;
			break;
		}

		case CRPGStats_Object_Property_Type::Summon:
		{
			auto summon = new CDivinityStats_Object_Property_Summon();
			summon->Template = MakeFixedString(checked_gettable<char const*, char const*>(L, "Template"));
			summon->Duration = checked_gettable<char const*, float>(L, "Duration");
			summon->IsTotem = checked_gettable<char const*, bool>(L, "IsTotem");
			summon->Skill = MakeFixedString(checked_gettable<char const*, char const*>(L, "Skill"));
			prop = summon;
			break;
		}

		case CRPGStats_Object_Property_Type::Force:
		{
			auto force = new CDivinityStats_Object_Property_Force();
			force->Distance = checked_gettable<char const*, int>(L, "Distance");
			prop = force;
			break;
		}

		default:
			luaL_error(L, "Unhandled object property type!");
			return nullptr;
		}

		STDString name = std::to_string(index).c_str();

		*(void**)prop = typeIt->second.VMT;
		prop->Name = MakeFixedString(name.c_str());
		prop->TypeId = typeIt->second.Type;
		prop->PropertyContext = (CRPGStats_Object_PropertyContext)0;
		prop->Conditions = nullptr;

		auto conditions = gettable<char const*, char const*>(L, "Condition");
		if (conditions && *conditions) {
			auto scriptCheckBlock = stats->BuildScriptCheckBlockFromProperties(conditions);
			if (scriptCheckBlock) {
				auto statConditions = GameAlloc<CDivinityStats_Condition>();
				statConditions->ScriptCheckBlock = scriptCheckBlock;
				statConditions->Name = propertyName;
				prop->Conditions = statConditions;
			} else {
				OsiWarn("Failed to parse conditions: " << conditions);
			}

			name += "_IF(";
			name += conditions;
			name += ")";
		}

		push(L, "Context");
		lua_gettable(L, -2);
		for (auto idx : iterate(L, -1)) {
			auto context = checked_get<char const*>(L, idx);
			if (strcmp(context, "Target") == 0) {
				prop->PropertyContext |= CRPGStats_Object_PropertyContext::Target;
			} else if (strcmp(context, "AoE") == 0) {
				prop->PropertyContext |= CRPGStats_Object_PropertyContext::AoE;
			} else if (strcmp(context, "Self") == 0) {
				prop->PropertyContext |= CRPGStats_Object_PropertyContext::Self;
			} else if (strcmp(context, "SelfOnHit") == 0) {
				prop->PropertyContext |= CRPGStats_Object_PropertyContext::SelfOnHit;
			} else if (strcmp(context, "SelfOnEquip") == 0) {
				prop->PropertyContext |= CRPGStats_Object_PropertyContext::SelfOnEquip;
			} else {
				OsiError("Unknown PropertyContext: " << context);
			}
		}
		lua_pop(L, 1);

		return prop;
	}

	CRPGStats_Object_Property_List* LuaToObjectPropertyList(lua_State* L, FixedString const& propertyName)
	{
		MapObjectPropertyVMTs();
		if (!SkillPropertiesVMT) {
			OsiError("Cannot construct object property list - VMT not mapped!");
			return nullptr;
		}

		auto properties = new CRPGStats_Object_Property_List();
		*(void**)properties = SkillPropertiesVMT;
		properties->Properties.NameHashMap.Init(31);
		properties->Name = propertyName;

		auto index = 0;
		for (auto idx : iterate(L, -1)) {
			auto prop = LuaToObjectProperty(L, index++, propertyName);
			if (prop) {
				properties->Properties.Add(prop->Name, prop);
				properties->AllPropertyContexts = prop->PropertyContext;
			}
		}

		return properties;
	}

	int LuaStatGetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, std::optional<int> level)
	{
		auto stats = GetStaticSymbols().GetStats();

		auto attributeFS = ToFixedString(attributeName);
		if (!attributeFS) {
			OsiError("Invalid stats attribute name: " << object->Name);
			return 0;
		}

		if (attributeFS == GFS.strLevel) {
			push(L, object->Level);
			return 1;
		} else if (attributeFS == GFS.strName) {
			push(L, object->Name);
			return 1;
		} else if (attributeFS == GFS.strUsing) {
			if (object->Using) {
				auto parent = stats->objects.Find(object->Using);
				if (parent != nullptr) {
					push(L, parent->Name);
					return 1;
				}
			}

			return 0;
		} else if (attributeFS == GFS.strRequirements) {
			RequirementsToLua(L, object->Requirements);
			return 1;
		} else if (attributeFS == GFS.strMemorizationRequirements) {
			RequirementsToLua(L, object->MemorizationRequirements);
			return 1;
		} else if (attributeFS == GFS.strAIFlags) {
			push(L, object->AIFlags);
			return 1;
		} else if (attributeFS == GFS.strComboCategory) {
			lua_newtable(L);
			auto index = 1;
			for (auto const& category : object->ComboCategories) {
				settable(L, index++, category);
			}
			return 1;
		} else if (attributeFS == GFS.strSkillProperties || attributeFS == GFS.strExtraProperties) {
			auto propertyList = object->PropertyList.Find(attributeFS);
			if (propertyList) {
				ObjectPropertyListToLua(L, **propertyList);
				return 1;
			} else {
				return 0;
			}
		}

		int index;
		auto attrInfo = stats->GetAttributeInfo(object, attributeFS, index);
		if (attrInfo && attrInfo->Name == GFS.strConditions) {
			auto conditions = object->ConditionList.Find(attributeFS);
			if (conditions) {
				OsiError("Conditions property '" << attributeFS << "' is not readable");
				return 1;
			} else {
				return 0;
			}
		}

		auto value = stats->GetAttributeString(object, attributeFS);
		if (!value) {
			std::optional<int> intval;
			if (level) {
				if (*level == -1) {
					*level = object->Level;
				}

				intval = stats->GetAttributeIntScaled(object, attributeFS, *level);
			} else {
				intval = stats->GetAttributeInt(object, attributeFS);
			}

			if (!intval) {
				OsiError("Stat object '" << object->Name << "' has no attribute named '" << attributeFS << "'");
				return 0;
			} else {
				push(L, *intval);
			}
		} else {
			push(L, *value);
		}

		return 1;
	}

	int StatGetAttribute(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);

		auto object = StatFindObject(statName);
		if (!object) return 0;

		return LuaStatGetAttribute(L, object, attributeName, {});
	}

	int StatGetAttributeScaled(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);

		auto object = StatFindObject(statName);
		if (!object) return 0;

		return LuaStatGetAttribute(L, object, attributeName, {});
	}

	int LuaStatSetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, int valueIdx)
	{
		LuaVirtualPin lua(gOsirisProxy->GetCurrentExtensionState());
		if (!(lua->RestrictionFlags & State::ScopeModuleLoad)) {
			return luaL_error(L, "StatSetAttribute() can only be called during module load");
		}

		auto attributeFS = ToFixedString(attributeName);
		if (!attributeFS) {
			OsiError("Invalid stats attribute name: " << object->Name);
			return 0;
		}

		if (attributeFS == GFS.strRequirements) {
			LuaToRequirements(L, object->Requirements);
			return 0;
		} else if (attributeFS == GFS.strMemorizationRequirements) {
			LuaToRequirements(L, object->MemorizationRequirements);
			return 0;
		} else if (attributeFS == GFS.strAIFlags) {
			object->AIFlags = (uint64_t)lua_tointeger(L, valueIdx);
			return 0;
		} else if (attributeFS == GFS.strComboCategory) {
			object->ComboCategories.Set.Clear();
			for (auto category : iterate(L, valueIdx)) {
				auto categoryName = checked_get<char const*>(L, category);
				object->ComboCategories.Set.Add(MakeFixedString(categoryName));
			}

			return 0;
		} else if (attributeFS == GFS.strSkillProperties || attributeFS == GFS.strExtraProperties) {
			STDString name = object->Name;
			name += "_";
			name += attributeName;
			auto statsPropertyKey = MakeFixedString(name.c_str());

			auto newList = LuaToObjectPropertyList(L, statsPropertyKey);
			if (newList) {
				auto propertyList = object->PropertyList.Find(ToFixedString(attributeName));
				if (propertyList) {
					// FIXME - add Remove() support!
					object->PropertyList.Clear();
					GameFree(*propertyList);
				}

				object->PropertyList.Insert(MakeFixedString(attributeName), newList);
			}

			return 0;
		}

		int index;
		auto stats = GetStaticSymbols().GetStats();
		auto attrInfo = stats->GetAttributeInfo(object, attributeFS, index);
		if (attrInfo && attrInfo->Name == GFS.strConditions) {
			auto conditions = object->ConditionList.Find(attributeFS);
			if (conditions) {
				auto value = luaL_checkstring(L, valueIdx);
				auto scriptCheckBlock = stats->BuildScriptCheckBlockFromProperties(value);
				if (scriptCheckBlock) {
					auto statConditions = GameAlloc<CDivinityStats_Condition>();
					statConditions->ScriptCheckBlock = scriptCheckBlock;
					STDString name = object->Name;
					name += "_";
					name += attributeName;
					statConditions->Name = MakeFixedString(name.c_str());
					*conditions = statConditions;
				} else {
					OsiWarn("Failed to parse conditions: " << value);
				}
			} else {
				// FIXME - not implemented yet!
				OsiWarnS("Adding new Conditions entries not implemented yet!");
			}

			return 0;
		}

		bool ok{ false };
		switch (lua_type(L, valueIdx)) {
		case LUA_TSTRING:
		{
			auto value = luaL_checkstring(L, valueIdx);
			ok = stats->SetAttributeString(object, attributeFS, value);
			break;
		}

		case LUA_TNUMBER:
		{
			auto value = (int32_t)luaL_checkinteger(L, valueIdx);
			ok = stats->SetAttributeInt(object, attributeFS, value);
			break;
		}

		default:
			return luaL_error(L, "Expected a string or integer attribute value.");
		}

		push(L, ok);
		return 1;
	}

	int StatSetAttribute(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);

		auto object = StatFindObject(statName);
		if (!object) return 0;

		return LuaStatSetAttribute(L, object, attributeName, 3);
	}

	int StatAddCustomDescription(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		auto attributeName = luaL_checkstring(L, 2);
		auto description = luaL_checkstring(L, 3);

		LuaVirtualPin lua(gOsirisProxy->GetCurrentExtensionState());
		if (!(lua->RestrictionFlags & State::ScopeModuleLoad)) {
			return luaL_error(L, "StatAddCustomDescription() can only be called during module load");
		}

		auto object = StatFindObject(statName);
		if (!object) return 0;

		auto props = object->PropertyList.Find(ToFixedString(attributeName));
		if (props == nullptr || *props == nullptr) {
			OsiError("Stat object '" << object->Name << "' has no property list named '" << attributeName << "'");
			return 0;
		}

		// FIXME - assign name + add to map properly
		auto customProp = GameAlloc<CRPGStats_Object_Property_CustomDescription>();
		customProp->PropertyContext = (CRPGStats_Object_PropertyContext)0;
		customProp->TypeId = CRPGStats_Object_Property_Type::CustomDescription;
		customProp->Conditions = nullptr;
		customProp->TextLine1 = FromUTF8(description);
		(*props)->Properties.Primitives.Set.Add(customProp);

		return 0;
	}


	struct CRPGStats_CustomLevelMap : public CRPGStats_LevelMap
	{
		RegistryEntry Function;
		CRPGStats_LevelMap * OriginalLevelMap{ nullptr };

		CRPGStats_CustomLevelMap() {}
		~CRPGStats_CustomLevelMap() override {}
		
		void SetModifierList(int modifierListIndex, int modifierIndex) override
		{
			OsiError("Not supported!");
		}

		void SetModifierList(FixedString const& modifierListName, FixedString const& modifierName) override
		{
			OsiError("Not supported!");
		}

		int64_t GetScaledValue(int attributeValue, int level) override
		{
			auto value = LuaGetScaledValue(attributeValue, level);
			if (value) {
				return *value;
			} else {
				return OriginalLevelMap->GetScaledValue(attributeValue, level);
			}
		}

		std::optional<int64_t> LuaGetScaledValue(int attributeValue, int level)
		{
			// GetScaledValue must always use the client pin, as the override function is
			// reigstered from the client state
			ecl::LuaClientPin pin(ecl::ExtensionState::Get());
			if (!pin) return {};

			std::lock_guard _(pin->GetMutex());
			Restriction restriction(*pin, State::RestrictAll);

			auto L = pin->GetState();
			Function.Push();

			push(L, attributeValue);
			push(L, level);

			if (lua_pcall(L, 2, 1, 0) != 0) { // stack: retval
				OsiError("Level scaled value fetch failed: " << lua_tostring(L, -1));
				lua_pop(L, 1);
				return {};
			}

			if (lua_type(L, -1) != LUA_TNUMBER) {
				OsiErrorS("Level scaled fetcher returned non-numeric value");
				return {};
			}

			auto value = lua_tointeger(L, -1);
			lua_pop(L, 1); // stack: -
			return value;
		}
	};

	void RestoreLevelMaps(std::unordered_set<int32_t> const & levelMapIds)
	{
		auto & levelMaps = GetStaticSymbols().GetStats()->LevelMaps.Primitives.Set;
		for (auto levelMapIndex : levelMapIds) {
			auto levelMap = static_cast<CRPGStats_CustomLevelMap *>(levelMaps.Buf[levelMapIndex]);
			levelMaps.Buf[levelMapIndex] = levelMap->OriginalLevelMap;
		}

		if (!levelMapIds.empty()) {
			OsiWarn("Restored " << levelMapIds.size() << " level map overrides (Lua VM deleted)");
		}
	}

	int StatSetLevelScaling(lua_State * L)
	{
		auto modifierListName = luaL_checkstring(L, 1);
		auto modifierName = luaL_checkstring(L, 2);
		luaL_checktype(L, 3, LUA_TFUNCTION);

		LuaVirtualPin lua(gOsirisProxy->GetCurrentExtensionState());
		if (!(lua->RestrictionFlags & (State::ScopeModuleLoad | State::ScopeModuleResume))) {
			return luaL_error(L, "StatSetLevelScaling() can only be called during module load/resume");
		}

		auto stats = GetStaticSymbols().GetStats();
		auto modifier = stats->GetModifierInfo(ToFixedString(modifierListName), ToFixedString(modifierName));
		if (modifier == nullptr) {
			OsiError("Modifier list '" << modifierListName << "' or modifier '" << modifierName << "' does not exist!");
			return 0;
		}

		if (modifier->LevelMapIndex == -1) {
			OsiError("Modifier list '" << modifierListName << "', modifier '" << modifierName << "' is not level scaled!");
			return 0;
		}

		CRPGStats_LevelMap * originalLevelMap;
		auto currentLevelMap = stats->LevelMaps.Find(modifier->LevelMapIndex);
		
		auto it = lua->OverriddenLevelMaps.find(modifier->LevelMapIndex);
		if (it != lua->OverriddenLevelMaps.end()) {
			auto overridden = static_cast<CRPGStats_CustomLevelMap *>(currentLevelMap);
			originalLevelMap = overridden->OriginalLevelMap;
		} else {
			originalLevelMap = currentLevelMap;
		}

		auto levelMap = GameAlloc<CRPGStats_CustomLevelMap>();
		levelMap->ModifierListIndex = originalLevelMap->ModifierListIndex;
		levelMap->ModifierIndex = originalLevelMap->ModifierIndex;
		levelMap->RPGEnumerationIndex = originalLevelMap->RPGEnumerationIndex;
		levelMap->Name = originalLevelMap->Name;
		levelMap->Function = RegistryEntry(L, 3);
		levelMap->OriginalLevelMap = originalLevelMap;

		stats->LevelMaps.Primitives.Set.Buf[modifier->LevelMapIndex] = levelMap;
		lua->OverriddenLevelMaps.insert(modifier->LevelMapIndex);

		return 0;
	}

	int GetStat(lua_State * L)
	{
		auto statName = luaL_checkstring(L, 1);
		std::optional<int> level;
		if (lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
			level = (int32_t)luaL_checkinteger(L, 2);
		}
		
		auto object = StatFindObject(statName);
		if (object != nullptr) {
			StatsProxy::New(L, object, level);
			return 1;
		} else {
			return 0;
		}
	}

	int GetDeltaMod(lua_State* L)
	{
		auto name = checked_get<char const *>(L, 1);
		auto modifierType = checked_get<char const*>(L, 2);

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 0;
		}

		auto deltaModType = stats->DeltaMods.Find(modifierType);
		if (deltaModType == nullptr) {
			OsiError("Unknown DeltaMod ModifierType: " << modifierType);
			return 0;
		}

		auto deltaMod = deltaModType->Find(name);
		if (deltaMod == nullptr) {
			return 0;
		}

		lua_newtable(L);
		settable(L, "ModifierType", deltaMod->ModifierType);
		auto slot = EnumInfo<ItemSlot>::Find((ItemSlot)deltaMod->SlotType);
		settable(L, "SlotType", EnumInfo<ItemSlot>::Find((ItemSlot)deltaMod->SlotType));
		settable(L, "WeaponType", EnumInfo<WeaponType>::Find(deltaMod->WeaponType));
		settable(L, "ArmorType", EnumInfo<ArmorType>::Find(deltaMod->ArmorType));
		settable(L, "Handedness", EnumInfo<HandednessType>::Find(deltaMod->Handedness));
		settable(L, "Name", deltaMod->Name);
		settable(L, "BoostType", deltaMod->BoostType);
		settable(L, "MinLevel", deltaMod->MinLevel);
		settable(L, "MaxLevel", deltaMod->MaxLevel);
		settable(L, "Frequency", deltaMod->Frequency);

		push(L, "Boosts");
		lua_newtable(L);
		int index = 1;
		for (uint32_t i = 0; i < deltaMod->BoostIndices.Set.Size; i++) {
			auto boost = stats->objects.Find(deltaMod->BoostIndices[i]);
			if (boost != nullptr) {
				push(L, index++);
				lua_newtable(L);
				settable(L, "Boost", boost->Name);
				settable(L, "Count", deltaMod->BoostCounts[i]);
				lua_settable(L, -3);
			}
		}
		lua_settable(L, -3);

		return 1;
	}

	int UpdateDeltaMod(lua_State* L)
	{
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = checked_gettable<char const*, char const*>(L, "Name", 1);
		auto modifierType = checked_gettable<char const*, char const*>(L, "ModifierType", 1);

		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return 0;
		}

		auto deltaModType = stats->DeltaMods.Find(modifierType);
		if (deltaModType == nullptr) {
			OsiError("Unknown DeltaMod ModifierType: " << modifierType);
			return 0;
		}

		auto deltaMod = deltaModType->Find(name);
		if (deltaMod == nullptr) {
			OsiError("Cannot update nonexistent DeltaMod: " << name);
			return 0;
		}

		deltaMod->ModifierType = MakeFixedString(modifierType);
		deltaMod->SlotType = (int)checked_gettable<char const*, ItemSlot>(L, "SlotType", 1);
		deltaMod->WeaponType = checked_gettable<char const*, WeaponType>(L, "WeaponType", 1);
		deltaMod->ArmorType = checked_gettable<char const*, ArmorType>(L, "ArmorType", 1);
		deltaMod->Handedness = checked_gettable<char const*, HandednessType>(L, "Handedness", 1);
		deltaMod->Name = MakeFixedString(name);
		deltaMod->BoostType = MakeFixedString(checked_gettable<char const*, char const*>(L, "BoostType", 1));
		deltaMod->MinLevel = checked_gettable<char const*, int>(L, "MinLevel", 1);
		deltaMod->MaxLevel = checked_gettable<char const*, int>(L, "MaxLevel", 1);
		deltaMod->MinLevelBoosted = deltaMod->MinLevel;
		deltaMod->MaxLevelBoosted = deltaMod->MaxLevel;
		deltaMod->Frequency = checked_gettable<char const*, int>(L, "Frequency", 1);

		deltaMod->BoostCounts.Set.Clear();
		deltaMod->BoostIndices.Set.Clear();
		push(L, "Boosts");
		lua_gettable(L, 1);

		for (auto valueIndex : iterate(L, -1)) {
			auto boost = MakeFixedString(checked_gettable<char const*, char const*>(L, "Boost", valueIndex - 1));
			auto flag = checked_gettable<char const*, int>(L, "Count", valueIndex - 1);
			auto object = stats->objects.FindIndex(boost);
			if (object != -1) {
				deltaMod->BoostIndices.Set.Add(object);
				deltaMod->BoostCounts.Set.Add(flag);
			}
		}

		return 1;
	}

	int NewDamageList(lua_State * L)
	{
		DamageList::New(L);
		return 1;
	}

	int IsDeveloperMode(lua_State * L)
	{
		push(L, gOsirisProxy->GetConfig().DeveloperMode);
		return 1;
	}

	int AddPathOverride(lua_State * L)
	{
		auto path = luaL_checkstring(L, 1);
		auto overridePath = luaL_checkstring(L, 2);
		gOsirisProxy->AddPathOverride(path, overridePath);
		return 0;
	}

	int AddVoiceMetaData(lua_State * L)
	{
		auto speakerGuid = luaL_checkstring(L, 1);
		auto translatedStringKey = luaL_checkstring(L, 2);
		auto source = luaL_checkstring(L, 3);
		auto length = checked_get<float>(L, 4);
		int32_t priority = 0;
		if (lua_gettop(L) > 4) {
			priority = checked_get<int>(L, 5);
		}

		auto speakerMgr = GetStaticSymbols().eoc__SpeakerManager;
		if (speakerMgr == nullptr || *speakerMgr == nullptr) {
			OsiError("Speaker manager not initialized!");
			return 0;
		}

		auto speaker = (*speakerMgr)->SpeakerMetaDataHashMap->Insert(MakeFixedString(speakerGuid));
		auto voiceMeta = speaker->Insert(MakeFixedString(translatedStringKey));
		voiceMeta->CodecID = 4;
		voiceMeta->IsRecorded = true;
		voiceMeta->Length = (float)length;
		voiceMeta->Priority = priority;

		auto path = GetStaticSymbols().ToPath(source, PathRootType::Data);
		voiceMeta->Source.Name = path;
		return 0;
	}

	int GetTranslatedString(lua_State* L)
	{
		auto translatedStringKey = luaL_checkstring(L, 1);
		char const* fallbackText = "";
		if (lua_gettop(L) > 1) {
			fallbackText = luaL_checkstring(L, 2);
		}

		STDWString translated;
		if (script::GetTranslatedString(translatedStringKey, translated)) {
			push(L, translated);
		} else {
			push(L, fallbackText);
		}

		return 1;
	}

	// Variation of Lua builtin math_random() with custom RNG
	int LuaRandom(lua_State *L)
	{
		auto state = gOsirisProxy->GetCurrentExtensionState();

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

	int LuaRound(lua_State *L)
	{
		auto val = luaL_checknumber(L, 1);
		push(L, round(val));
		return 1;
	}

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

	STDString GenerateIdeHelpers()
	{
		STDString helpers;
		helpers.reserve(0x20000);

		auto functions = gOsirisProxy->GetGlobals().Functions;

		(*functions)->Iterate([&helpers](STDString const & key, Function const * func) {
			auto const & outParams = func->Signature->OutParamList;
			auto numOutParams = outParams.numOutParams();

			auto types = func->Signature->Params->Params.Head;
			for (auto i = 0; i < func->Signature->Params->Params.Size; i++) {
				types = types->Next;

				if (!outParams.isOutParam(i)) {
					helpers += "--- @param arg";
					helpers += std::to_string(i + 1);
					helpers += " ";
					helpers += OsiToLuaTypeName((ValueType)types->Item.Type);
					helpers += "\r\n";
				}
			}

			if (numOutParams > 0) {
				auto types = func->Signature->Params->Params.Head;
				for (auto i = 0; i < func->Signature->Params->Params.Size; i++) {
					types = types->Next;

					if (outParams.isOutParam(i)) {
						helpers += "--- @return ";
						helpers += OsiToLuaTypeName((ValueType)types->Item.Type);
						helpers += "\r\n";
					}
				}
			} else {
				if (func->Type == FunctionType::SysQuery
					|| func->Type == FunctionType::Query
					|| func->Type == FunctionType::UserQuery) {
					helpers += "--- @return boolean Did the query succeed?\r\n";
				}
			}

			helpers += "Osi.";
			helpers += func->Signature->Name;
			helpers += " = function (";

			auto pendingInParams = func->Signature->Params->Params.Size - numOutParams;
			for (auto i = 0; i < func->Signature->Params->Params.Size; i++) {
				if (!outParams.isOutParam(i)) {
					helpers += "arg";
					helpers += std::to_string(i + 1);
					if (--pendingInParams > 0) {
						helpers += ", ";
					}
				}
			}

			helpers += ") end\r\n\r\n";
		});

		return helpers;
	}

	int GenerateIdeHelpers(lua_State * L)
	{
#if !defined(OSI_EOCAPP)
		esv::LuaServerPin lua(esv::ExtensionState::Get());
		if (lua->RestrictionFlags & State::RestrictOsiris) {
			return luaL_error(L, "GenerateIdeHelpers() can only be called when Osiris is available");
		}

		auto helpers = GenerateIdeHelpers();

		auto path = GetStaticSymbols().ToPath("", PathRootType::Data);
		path += "Mods/";
		path += ToUTF8(GetModManagerServer()->BaseModule.Info.Directory);
		path += "/Story/RawFiles/Lua/OsiIdeHelpers.lua";

		std::ofstream f(path.c_str(), std::ios::out | std::ios::binary);
		if (!f.good()) {
			OsiError("Could not open file to save IDE helpers: '" << path << "'");
			return 0;
		}

		f.write(helpers.c_str(), helpers.size());
#else
		OsiError("GenerateIdeHelpers() only supported in editor mode");
#endif
		return 0;
	}
}
