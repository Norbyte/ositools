#include <stdafx.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include "LuaBinding.h"
#include "resource.h"
#include <fstream>

namespace dse::lua
{
	RegistryEntry::RegistryEntry()
		: L_(nullptr), ref_(-1)
	{}

	RegistryEntry::RegistryEntry(lua_State * L, int index)
		: L_(L)
	{
		lua_pushvalue(L, index);
		ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	RegistryEntry::~RegistryEntry()
	{
		if (ref_ != -1) {
			luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
		}
	}

	RegistryEntry::RegistryEntry(RegistryEntry && other)
		: L_(other.L_), ref_(other.ref_)
	{
		other.ref_ = -1;
	}

	RegistryEntry & RegistryEntry::operator = (RegistryEntry && other)
	{
		L_ = other.L_;
		ref_ = other.ref_;
		other.ref_ = -1;
		return *this;
	}

	void RegistryEntry::Push()
	{
		assert(ref_ != -1);
		lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);
	}


	int TracebackHandler(lua_State * L)
	{
		const char *msg = lua_tostring(L, 1);
		if (msg == NULL) {  /* is error object not a string? */
			if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
				lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
				return 1;  /* that is the message */
			else
				msg = lua_pushfstring(L, "(error object is a %s value)",
					luaL_typename(L, 1));
		}
		luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
		return 1;  /* return the traceback */
	}

	int CallWithTraceback(lua_State * L, int narg, int nres)
	{
		int base = lua_gettop(L) - narg;  /* function index */
		lua_pushcfunction(L, &TracebackHandler);  /* push message handler */
		lua_insert(L, base);  /* put it under function and args */
		int status = lua_pcall(L, narg, nres, base);
		lua_remove(L, base);  /* remove message handler from the stack */
		return status;
	}


	void PushExtFunction(lua_State * L, char const * func)
	{
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, func); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
	}


	int LuaStatGetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, std::optional<int> level);
	int LuaStatSetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, int valueIdx);

	char const * const StatsProxy::MetatableName = "CRPGStats_Object";

	int StatsProxy::Index(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Attempted to read property of null CRPGStats_Object object");
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Level") == 0) {
			push(L, obj_->Level);
			return 1;
		} else if (strcmp(attributeName, "Name") == 0) {
			lua_pushstring(L, obj_->Name);
			return 1;
		}

		return LuaStatGetAttribute(L, obj_, attributeName, level_);
	}

	int StatsProxy::NewIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);
		return LuaStatSetAttribute(L, obj_, attributeName, 3);
	}


	char const * const SkillPrototypeProxy::MetatableName = "eoc::SkillPrototype";

	SkillPrototypeProxy::SkillPrototypeProxy(SkillPrototype * obj, std::optional<int> level)
		: obj_(obj), level_(level)
	{
		stats_ = StatFindObject(obj->RPGStatsObjectIndex);
	}

	int SkillPrototypeProxy::Index(lua_State * L)
	{
		if (stats_ == nullptr) return luaL_error(L, "Attempted to read property of null SkillPrototype object");
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Level") == 0) {
			push(L, obj_->Level);
			return 1;
		}

		return LuaStatGetAttribute(L, stats_, attributeName, level_);
	}

	std::optional<int32_t> GetCharacterDynamicStat(CDivinityStats_Character * stats, char const * statName)
	{
		bool isBaseStat = strncmp(statName, "Base", 4) == 0;
		return stats->GetStat(isBaseStat ? (statName + 4) : statName, isBaseStat);
	}

	char const * const ObjectProxy<CDivinityStats_Character>::MetatableName = "CDivinityStats_Character";

	int CharacterFetchStat(lua_State * L, CDivinityStats_Character * stats, char const * prop)
	{
		auto dynamicStat = GetCharacterDynamicStat(stats, prop);
		if (dynamicStat) {
			lua_pushinteger(L, *dynamicStat);
			return 1;
		}

		if (strcmp(prop, "DynamicStats") == 0) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto statPtr = stats->DynamicStats; statPtr != stats->DynamicStatsEnd; statPtr++) {
				push(L, statIdx++);
				ObjectProxy<CharacterDynamicStat>::New(L, *statPtr);
				lua_settable(L, -3);
			}

			return 1;
		}

		if (strcmp(prop, "DamageBoost") == 0) {
			lua_pushinteger(L, stats->GetDamageBoost());
			return 1;
		}

		if (strcmp(prop, "MainWeapon") == 0) {
			auto weapon = stats->GetMainWeapon();
			if (weapon != nullptr) {
				ObjectProxy<CDivinityStats_Item>::New(L, weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (strcmp(prop, "OffHandWeapon") == 0) {
			auto weapon = stats->GetOffHandWeapon();
			if (weapon != nullptr) {
				ObjectProxy<CDivinityStats_Item>::New(L, weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (strncmp(prop, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(prop + 7);
			if (talentId) {
				bool hasTalent = stats->HasTalent(*talentId, false);
				lua_pushboolean(L, hasTalent);
				return 1;
			} else {
				return 0;
			}
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = stats->GetAbility(*abilityId, false);
			lua_pushinteger(L, abilityLevel);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterStatsPropertyMap, stats, prop, false);
		if (fetched) {
			return 1;
		}

		if (stats->Character != nullptr) {
			fetched = LuaPropertyMapGet(L, gEoCServerObjectPropertyMap, stats->Character, prop, false);
			if (fetched) {
				return 1;
			}
		}

		OsiError("Unknown character stats property: " << prop);
		return 0;
	}

	int ObjectProxy<CDivinityStats_Character>::Index(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Character stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchStat(L, obj_, prop);
	}

	int ObjectProxy<CDivinityStats_Character>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}

	int ItemFetchStat(lua_State * L, CDivinityStats_Item * item, char const * prop)
	{
		if (strcmp(prop, "DynamicStats") == 0) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto statPtr = item->DynamicAttributes_Start; statPtr != item->DynamicAttributes_End; statPtr++) {
				push(L, statIdx++);
				ObjectProxy<CDivinityStats_Equipment_Attributes>::New(L, *statPtr);
				lua_settable(L, -3);
			}

			return 1;
		}

		if (strncmp(prop, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(prop + 7);
			if (talentId) {
				bool hasTalent = item->HasTalent(*talentId);
				lua_pushboolean(L, hasTalent);
				return 1;
			} else {
				return 0;
			}
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = item->GetAbility(*abilityId);
			lua_pushinteger(L, abilityLevel);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item, prop, false);
		if (fetched) {
			return 1;
		}

		return LuaStatGetAttribute(L, item, prop, {});
	}

	int CharacterFetchProperty(lua_State * L, esv::Character * character, char const * prop)
	{
		if (strcmp(prop, "PlayerCustomData") == 0) {
			if (character->PlayerData != nullptr
				&& character->PlayerData->CustomData.Initialized) {
				ObjectHandle handle;
				character->GetObjectHandle(&handle);
				HandleProxy<esv::PlayerCustomData>::New(L, handle);
				return 1;
			} else {
				OsiError("Character has no player data, or custom data was not initialized.");
				return 0;
			}
		}

		if (strcmp(prop, "Stats") == 0) {
			if (character->Stats != nullptr) {
				ObjectHandle handle;
				character->GetObjectHandle(&handle);
				HandleProxy<CDivinityStats_Character>::New(L, handle);
				return 1;
			} else {
				OsiError("Character has no stats.");
				return 0;
			}
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterPropertyMap, character, prop, true);
		return fetched ? 1 : 0;
	}


	char const * const ObjectProxy<CDivinityStats_Item>::MetatableName = "CDivinityStats_Item";

	int ObjectProxy<CDivinityStats_Item>::Index(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Item stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, obj_, prop);
	}

	int ObjectProxy<CDivinityStats_Item>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const ObjectProxy<CDivinityStats_Equipment_Attributes>::MetatableName = "CDivinityStats_Equipment_Attributes";

	int ObjectProxy<CDivinityStats_Equipment_Attributes>::Index(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Equipment stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto & propMap = obj_->GetPropertyMap();
		auto fetched = LuaPropertyMapGet(L, propMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<CDivinityStats_Equipment_Attributes>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported!");
	}


	char const * const ObjectProxy<CharacterDynamicStat>::MetatableName = "CharacterDynamicStat";

	int ObjectProxy<CharacterDynamicStat>::Index(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Character stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gCharacterDynamicStatPropertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<CharacterDynamicStat>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported!");
	}


	ItemOrCharacterPushPin::ItemOrCharacterPushPin(lua_State * L, CRPGStats_Object * obj)
	{
		if (obj == nullptr) {
			lua_pushnil(L);
		} else if (obj->ModifierListIndex == GetStaticSymbols().GetStats()->modifierList.FindIndex(ToFixedString("Character"))) {
			auto ch = reinterpret_cast<CDivinityStats_Character *>(obj);
			character_ = ObjectProxy<CDivinityStats_Character>::New(L, ch);
		} else if (obj->ModifierListIndex == GetStaticSymbols().GetStats()->modifierList.FindIndex(ToFixedString("Item"))) {
			auto item = reinterpret_cast<CDivinityStats_Item *>(obj);
			item_ = ObjectProxy<CDivinityStats_Item>::New(L, item);
		} else {
			object_ = StatsProxy::New(L, obj, -1);
			OsiWarnS("Could not determine stats type of object");
		}
	}

	ItemOrCharacterPushPin::~ItemOrCharacterPushPin()
	{
		if (character_) character_->Unbind();
		if (item_) item_->Unbind();
		if (object_) object_->Unbind();
	}


	char const * const HandleProxy<CDivinityStats_Character>::MetatableName = "HCDivinityStats_Character";

	int HandleProxy<CDivinityStats_Character>::Index(lua_State * L)
	{
		auto character = FindCharacterByHandle(handle_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");
		if (character->Stats == nullptr) return luaL_error(L, "Character has no stats!");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchStat(L, character->Stats, prop);
	}

	int HandleProxy<CDivinityStats_Character>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const HandleProxy<CDivinityStats_Item>::MetatableName = "HCDivinityStats_Item";

	int HandleProxy<CDivinityStats_Item>::Index(lua_State * L)
	{
		auto item = FindItemByHandle(handle_);
		if (item == nullptr) return luaL_error(L, "Item handle invalid");
		if (item->StatsDynamic == nullptr) return luaL_error(L, "Item has no stats!");

		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, item->StatsDynamic, prop);
	}

	int HandleProxy<CDivinityStats_Item>::NewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const StatsExtraDataProxy::MetatableName = "CRPGStats_ExtraData";

	int StatsExtraDataProxy::Index(lua_State * L)
	{
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr || stats->ExtraData == nullptr) return luaL_error(L, "Stats not available");

		auto key = luaL_checkstring(L, 2);
		auto extraData = stats->ExtraData->Properties.Find(key);
		if (extraData != nullptr) {
			lua_pushnumber(L, *extraData);
			return 1;
		} else {
			return 0;
		}
	}



	char const * const DamageList::MetatableName = "CDamageList";

	void DamageList::PopulateMetatable(lua_State * L)
	{
		lua_newtable(L);

		lua_pushcfunction(L, &GetByType);
		lua_setfield(L, -2, "GetByType");

		lua_pushcfunction(L, &Add);
		lua_setfield(L, -2, "Add");

		lua_pushcfunction(L, &Clear);
		lua_setfield(L, -2, "Clear");

		lua_pushcfunction(L, &Multiply);
		lua_setfield(L, -2, "Multiply");

		lua_pushcfunction(L, &Merge);
		lua_setfield(L, -2, "Merge");

		lua_pushcfunction(L, &ConvertDamageType);
		lua_setfield(L, -2, "ConvertDamageType");

		lua_pushcfunction(L, &AggregateSameTypeDamages);
		lua_setfield(L, -2, "AggregateSameTypeDamages");

		lua_pushcfunction(L, &ToTable);
		lua_setfield(L, -2, "ToTable");

		lua_setfield(L, -2, "__index");
	}

	int DamageList::GetByType(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		auto damageType = checked_get<DamageType>(L, 2);

		int32_t amount = 0;
		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			auto const & dmg = self->damages_.Buf[i];
			if (dmg.DamageType == damageType) {
				amount += dmg.Amount;
			}
		}

		push(L, amount);
		return 1;
	}

	int DamageList::Add(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		auto damageType = checked_get<DamageType>(L, 2);
		auto amount = (int32_t)luaL_checkinteger(L, 3);

		self->damages_.AddDamage(damageType, amount);

		return 0;
	}

	int DamageList::Clear(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		if (lua_gettop(L) >= 2) {
			auto damageType = checked_get<DamageType>(L, 2);
			self->damages_.ClearDamage(damageType);
		} else {
			self->damages_.Clear();
		}

		return 0;
	}

	int DamageList::Multiply(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		auto multiplier = luaL_checknumber(L, 2);

		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			auto & item = self->damages_[i];
			item.Amount = (int32_t)round(item.Amount * multiplier);
		}

		return 0;
	}

	int DamageList::Merge(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		auto other = DamageList::CheckUserData(L, 2);

		for (uint32_t i = 0; i < other->damages_.Size; i++) {
			auto & item = other->damages_[i];
			self->damages_.AddDamage(item.DamageType, item.Amount);
		}

		return 0;
	}

	int DamageList::ConvertDamageType(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		auto damageType = checked_get<DamageType>(L, 2);

		int32_t totalDamage = 0;
		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			totalDamage += self->damages_[i].Amount;
		}

		self->damages_.Clear();
		self->damages_.AddDamage(damageType, totalDamage);

		return 0;
	}

	int DamageList::AggregateSameTypeDamages(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);

		for (uint32_t i = self->damages_.Size; i > 0; i--) {
			auto & src = self->damages_[i - 1];
			for (uint32_t j = i - 1; j > 0; j--) {
				auto & dest = self->damages_[j - 1];
				if (src.DamageType == dest.DamageType) {
					dest.Amount += src.Amount;
					self->damages_.Remove(i - 1);
					break;
				}
			}
		}

		return 0;
	}

	int DamageList::ToTable(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);

		lua_newtable(L); // Stack: tab

		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			auto const & item = self->damages_[i];

			push(L, i + 1); // Stack: tab, index
			lua_newtable(L); // Stack: tab, index, dmgTab
			auto dmgTypeName = EnumInfo<DamageType>::Find(item.DamageType);
			settable(L, "DamageType", *dmgTypeName);
			settable(L, "Amount", item.Amount);

			lua_settable(L, -3); // Stack: tab
		}

		return 1;
	}


	void ExtensionLibrary::Register(lua_State * L)
	{
		RegisterLib(L);
		ObjectProxy<CDivinityStats_Character>::RegisterMetatable(L);
		ObjectProxy<CharacterDynamicStat>::RegisterMetatable(L);
		ObjectProxy<CDivinityStats_Item>::RegisterMetatable(L);
		ObjectProxy<CDivinityStats_Equipment_Attributes>::RegisterMetatable(L);
		HandleProxy<CDivinityStats_Character>::RegisterMetatable(L);
		HandleProxy<CDivinityStats_Item>::RegisterMetatable(L);
		StatsExtraDataProxy::RegisterMetatable(L);
		StatsProxy::RegisterMetatable(L);
		SkillPrototypeProxy::RegisterMetatable(L);
		DamageList::RegisterMetatable(L);
	}

	int ExtensionLibrary::Require(lua_State * L)
	{
		auto modGuid = luaL_checkstring(L, 1);
		auto fileName = luaL_checkstring(L, 2);
		gOsirisProxy->GetCurrentExtensionState()->LuaLoadModScript(modGuid, fileName);
		return 0;
	}


	// Unsafe libs (IO, OS, package, debug) removed
	static const luaL_Reg loadedlibs[] = {
	  {"_G", luaopen_base},
	  {LUA_TABLIBNAME, luaopen_table},
	  {LUA_STRLIBNAME, luaopen_string},
	  {LUA_MATHLIBNAME, luaopen_math},
	  // debug table is stripped in the sandbox startup script
	  {LUA_DBLIBNAME, luaopen_debug},
	  {NULL, NULL}
	};

	int LuaPanic(lua_State * L)
	{
		throw Exception();
	}

	State::State()
	{
		L = luaL_newstate();
		lua_atpanic(L, &LuaPanic);
		OpenLibs();
	}

	void RestoreLevelMaps(std::unordered_set<int32_t> const &);

	State::~State()
	{
		RestoreLevelMaps(OverriddenLevelMaps);
		lua_close(L);
	}

	void State::FinishStartup()
	{
		assert(!startupDone_);
		startupDone_ = true;
	}
		
	void State::OpenLibs()
	{
		const luaL_Reg *lib;
		/* "require" functions from 'loadedlibs' and set results to global table */
		for (lib = loadedlibs; lib->func; lib++) {
			luaL_requiref(L, lib->name, lib->func, 1);
			lua_pop(L, 1);  /* remove lib */
		}
	}

	bool State::LoadScript(STDString const & script, STDString const & name)
	{
		std::lock_guard lock(mutex_);

		/* Load the file containing the script we are going to run */
		int status = luaL_loadbufferx(L, script.c_str(), script.size(), name.c_str(), "text");
		if (status != LUA_OK) {
			OsiError("Failed to parse script: " << lua_tostring(L, -1));
			lua_pop(L, 1);  /* pop error message from the stack */
			return false;
		}

		/* Ask Lua to run our little script */
		status = CallWithTraceback(L, 0, LUA_MULTRET);
		if (status != LUA_OK) {
			OsiError("Failed to execute script: " << lua_tostring(L, -1));
			lua_pop(L, 1); // pop error message from the stack
			return false;
		}

		return true;
	}

	std::optional<int32_t> State::GetHitChance(CDivinityStats_Character * attacker, CDivinityStats_Character * target)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_GetHitChance"); // stack: fn
		auto _{ PushArguments(L,
			std::tuple{Push<ObjectProxy<CDivinityStats_Character>>(attacker),
			Push < ObjectProxy<CDivinityStats_Character>>(target)}) };

		auto result = CheckedCall<std::optional<int32_t>>(L, 2, "Ext.GetHitChance");
		if (result) {
			return std::get<0>(*result);
		} else {
			return {};
		}
	}

	bool State::GetSkillDamage(SkillPrototype * skill, DamagePairList * damageList,
		CRPGStats_ObjectInstance *attacker, bool isFromItem, bool stealthed, float * attackerPosition,
		float * targetPosition, DeathType * pDeathType, int level, bool noRandomization)
	{
		std::lock_guard lock(mutex_);
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_GetSkillDamage"); // stack: fn

		auto luaSkill = SkillPrototypeProxy::New(L, skill, -1); // stack: fn, skill
		UnbindablePin _(luaSkill);
		ItemOrCharacterPushPin _a(L, attacker);

		push(L, isFromItem);
		push(L, stealthed);
		
		// Push attacker position
		lua_newtable(L);
		settable(L, 1, attackerPosition[0]);
		settable(L, 2, attackerPosition[1]);
		settable(L, 3, attackerPosition[2]);

		// Push target position
		lua_newtable(L);
		settable(L, 1, targetPosition[0]);
		settable(L, 2, targetPosition[1]);
		settable(L, 3, targetPosition[2]);

		push(L, level);
		push(L, noRandomization);

		auto result = CheckedCall<std::optional<DeathType>, std::optional<DamageList *>>(L, 8, "Ext.GetSkillDamage");
		if (result) {
			auto deathType = std::get<0>(*result);
			auto damages = std::get<1>(*result);

			if (deathType && damages) {
				if (pDeathType) {
					*pDeathType = *deathType;
				}

				auto const & list = (*damages)->Get();
				for (uint32_t i = 0; i < list.Size; i++) {
					auto const & item = list[i];
					damageList->AddDamage(item.DamageType, item.Amount);
				}

				return true;
			}
		}

		return false;
	}

	void State::OnNetMessageReceived(STDString const & channel, STDString const & payload)
	{
		CallExt("_NetMessageReceived", 0, ReturnType<>{}, channel, payload);
	}

	void State::OnGameSessionLoading()
	{
		CallExt("_OnGameSessionLoading", RestrictAll | ScopeSessionLoad, ReturnType<>{});
	}

	void State::OnGameSessionLoaded()
	{
		CallExt("_OnGameSessionLoaded", RestrictAll, ReturnType<>{});
	}

	void State::OnModuleLoading()
	{
		CallExt("_OnModuleLoading", RestrictAll | ScopeModuleLoad, ReturnType<>{});
	}

	void State::OnModuleResume()
	{
		CallExt("_OnModuleResume", RestrictAll | ScopeModuleResume, ReturnType<>{});
	}

	STDString State::GetBuiltinLibrary(int resourceId)
	{
		auto hResource = FindResource(gThisModule, MAKEINTRESOURCE(resourceId),
			L"LUA_SCRIPT");

		if (hResource) {
			auto hGlobal = LoadResource(gThisModule, hResource);
			if (hGlobal) {
				auto resourceData = LockResource(hGlobal);
				if (resourceData) {
					DWORD resourceSize = SizeofResource(gThisModule, hResource);
					STDString script;
					script.resize(resourceSize);
					memcpy(script.data(), resourceData, resourceSize);
					return script;
				}
			}
		}

		OsiErrorS("Could not find bootstrap resource!");
		return STDString();
	}

}
