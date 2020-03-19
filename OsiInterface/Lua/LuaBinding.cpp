#include <stdafx.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include "LuaBinding.h"
#include "resource.h"
#include <fstream>

namespace osidbg
{
	LuaRegistryEntry::LuaRegistryEntry()
		: L_(nullptr), ref_(-1)
	{}

	LuaRegistryEntry::LuaRegistryEntry(lua_State * L, int index)
		: L_(L)
	{
		lua_pushvalue(L, index);
		ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	LuaRegistryEntry::~LuaRegistryEntry()
	{
		if (ref_ != -1) {
			luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
		}
	}

	LuaRegistryEntry::LuaRegistryEntry(LuaRegistryEntry && other)
		: L_(other.L_), ref_(other.ref_)
	{
		other.ref_ = -1;
	}

	LuaRegistryEntry & LuaRegistryEntry::operator = (LuaRegistryEntry && other)
	{
		L_ = other.L_;
		ref_ = other.ref_;
		other.ref_ = -1;
		return *this;
	}

	void LuaRegistryEntry::Push()
	{
		assert(ref_ != -1);
		lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);
	}



	int LuaStatGetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, std::optional<int> level);
	int LuaStatSetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, int valueIdx);

	char const * const LuaStatsProxy::MetatableName = "CRPGStats_Object";

	int LuaStatsProxy::LuaIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Level") == 0) {
			lua_push(L, obj_->Level);
			return 1;
		} else if (strcmp(attributeName, "Name") == 0) {
			lua_pushstring(L, obj_->Name);
			return 1;
		}

		return LuaStatGetAttribute(L, obj_, attributeName, level_);
	}

	int LuaStatsProxy::LuaNewIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);
		return LuaStatSetAttribute(L, obj_, attributeName, 3);
	}


	char const * const LuaSkillPrototypeProxy::MetatableName = "eoc::SkillPrototype";

	LuaSkillPrototypeProxy::LuaSkillPrototypeProxy(SkillPrototype * obj, std::optional<int> level)
		: obj_(obj), level_(level)
	{
		stats_ = StatFindObject(obj->RPGStatsObjectIndex);
	}

	int LuaSkillPrototypeProxy::LuaIndex(lua_State * L)
	{
		auto attributeName = luaL_checkstring(L, 2);

		if (strcmp(attributeName, "Level") == 0) {
			lua_push(L, obj_->Level);
			return 1;
		}

		return LuaStatGetAttribute(L, stats_, attributeName, level_);
	}

	std::optional<int32_t> GetCharacterDynamicStat(CDivinityStats_Character * stats, char const * statName)
	{
		bool isBaseStat = strncmp(statName, "Base", 4) == 0;
		return stats->GetStat(isBaseStat ? (statName + 4) : statName, isBaseStat);
	}

	char const * const LuaObjectProxy<CDivinityStats_Character>::MetatableName = "CDivinityStats_Character";

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
				lua_push(L, statIdx++);
				LuaObjectProxy<CharacterDynamicStat>::New(L, *statPtr);
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
				LuaObjectProxy<CDivinityStats_Item>::New(L, weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (strcmp(prop, "OffHandWeapon") == 0) {
			auto weapon = stats->GetOffHandWeapon();
			if (weapon != nullptr) {
				LuaObjectProxy<CDivinityStats_Item>::New(L, weapon);
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

	int LuaObjectProxy<CDivinityStats_Character>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Character stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchStat(L, obj_, prop);
	}

	int LuaObjectProxy<CDivinityStats_Character>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}

	int ItemFetchStat(lua_State * L, CDivinityStats_Item * item, char const * prop)
	{
		if (strcmp(prop, "DynamicStats") == 0) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto statPtr = item->DynamicAttributes_Start; statPtr != item->DynamicAttributes_End; statPtr++) {
				lua_push(L, statIdx++);
				LuaObjectProxy<CDivinityStats_Equipment_Attributes>::New(L, *statPtr);
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
				LuaHandleProxy<esv::PlayerCustomData>::New(L, handle);
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
				LuaHandleProxy<CDivinityStats_Character>::New(L, handle);
				return 1;
			} else {
				OsiError("Character has no stats.");
				return 0;
			}
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterPropertyMap, character, prop, true);
		return fetched ? 1 : 0;
	}


	char const * const LuaObjectProxy<CDivinityStats_Item>::MetatableName = "CDivinityStats_Item";

	int LuaObjectProxy<CDivinityStats_Item>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Item stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, obj_, prop);
	}

	int LuaObjectProxy<CDivinityStats_Item>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaObjectProxy<CDivinityStats_Equipment_Attributes>::MetatableName = "CDivinityStats_Equipment_Attributes";

	int LuaObjectProxy<CDivinityStats_Equipment_Attributes>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Equipment stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto & propMap = obj_->GetPropertyMap();
		auto fetched = LuaPropertyMapGet(L, propMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaObjectProxy<CDivinityStats_Equipment_Attributes>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported!");
	}


	char const * const LuaObjectProxy<CharacterDynamicStat>::MetatableName = "CharacterDynamicStat";

	int LuaObjectProxy<CharacterDynamicStat>::LuaIndex(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Character stats no longer available");

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gCharacterDynamicStatPropertyMap, obj_, prop, true);
		return fetched ? 1 : 0;
	}

	int LuaObjectProxy<CharacterDynamicStat>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported!");
	}


	LuaItemOrCharacterPushPin::LuaItemOrCharacterPushPin(lua_State * L, CRPGStats_Object * obj)
	{
		if (obj == nullptr) {
			lua_pushnil(L);
		} else if (obj->ModifierListIndex == GetStaticSymbols().GetStats()->modifierList.FindIndex(ToFixedString("Character"))) {
			auto ch = reinterpret_cast<CDivinityStats_Character *>(obj);
			character_ = LuaObjectProxy<CDivinityStats_Character>::New(L, ch);
		} else if (obj->ModifierListIndex == GetStaticSymbols().GetStats()->modifierList.FindIndex(ToFixedString("Item"))) {
			auto item = reinterpret_cast<CDivinityStats_Item *>(obj);
			item_ = LuaObjectProxy<CDivinityStats_Item>::New(L, item);
		} else {
			object_ = LuaStatsProxy::New(L, obj, -1);
			OsiWarnS("Could not determine stats type of object");
		}
	}

	LuaItemOrCharacterPushPin::~LuaItemOrCharacterPushPin()
	{
		if (character_) character_->Unbind();
		if (item_) item_->Unbind();
		if (object_) object_->Unbind();
	}


	char const * const LuaHandleProxy<CDivinityStats_Character>::MetatableName = "HCDivinityStats_Character";

	int LuaHandleProxy<CDivinityStats_Character>::LuaIndex(lua_State * L)
	{
		auto character = FindCharacterByHandle(handle_);
		if (character == nullptr) return luaL_error(L, "Character handle invalid");
		if (character->Stats == nullptr) return luaL_error(L, "Character has no stats!");

		auto prop = luaL_checkstring(L, 2);
		return CharacterFetchStat(L, character->Stats, prop);
	}

	int LuaHandleProxy<CDivinityStats_Character>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaHandleProxy<CDivinityStats_Item>::MetatableName = "HCDivinityStats_Item";

	int LuaHandleProxy<CDivinityStats_Item>::LuaIndex(lua_State * L)
	{
		auto item = FindItemByHandle(handle_);
		if (item == nullptr) return luaL_error(L, "Item handle invalid");
		if (item->StatsDynamic == nullptr) return luaL_error(L, "Item has no stats!");

		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, item->StatsDynamic, prop);
	}

	int LuaHandleProxy<CDivinityStats_Item>::LuaNewIndex(lua_State * L)
	{
		return luaL_error(L, "Not supported yet!");
	}


	char const * const LuaStatsExtraDataProxy::MetatableName = "CRPGStats_ExtraData";

	int LuaStatsExtraDataProxy::LuaIndex(lua_State * L)
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



	char const * const LuaDamageList::MetatableName = "CDamageList";

	void LuaDamageList::PopulateMetatable(lua_State * L)
	{
		lua_newtable(L);

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

	int LuaDamageList::Add(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto damageType = lua_checkenum<DamageType>(L, 2);
		auto amount = (int32_t)luaL_checkinteger(L, 3);

		self->damages_.AddDamage(damageType, amount);

		return 0;
	}

	int LuaDamageList::Clear(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		if (lua_gettop(L) >= 2) {
			auto damageType = lua_checkenum<DamageType>(L, 2);
			self->damages_.ClearDamage(damageType);
		} else {
			self->damages_.Clear();
		}

		return 0;
	}

	int LuaDamageList::Multiply(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto multiplier = luaL_checknumber(L, 2);

		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			auto & item = self->damages_[i];
			item.Amount = (int32_t)round(item.Amount * multiplier);
		}

		return 0;
	}

	int LuaDamageList::Merge(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto other = LuaDamageList::CheckUserData(L, 2);

		for (uint32_t i = 0; i < other->damages_.Size; i++) {
			auto & item = other->damages_[i];
			self->damages_.AddDamage(item.DamageType, item.Amount);
		}

		return 0;
	}

	int LuaDamageList::ConvertDamageType(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);
		auto damageType = lua_checkenum<DamageType>(L, 2);

		int32_t totalDamage = 0;
		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			totalDamage += self->damages_[i].Amount;
		}

		self->damages_.Clear();
		self->damages_.AddDamage(damageType, totalDamage);

		return 0;
	}

	int LuaDamageList::AggregateSameTypeDamages(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);

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

	int LuaDamageList::ToTable(lua_State * L)
	{
		auto self = LuaDamageList::CheckUserData(L, 1);

		lua_newtable(L); // Stack: tab

		for (uint32_t i = 0; i < self->damages_.Size; i++) {
			auto const & item = self->damages_[i];

			lua_push(L, i + 1); // Stack: tab, index
			lua_newtable(L); // Stack: tab, index, dmgTab
			auto dmgTypeName = EnumInfo<DamageType>::Find(item.DamageType);
			luaL_settable(L, "DamageType", *dmgTypeName);
			luaL_settable(L, "Amount", item.Amount);

			lua_settable(L, -3); // Stack: tab
		}

		return 1;
	}


	void LuaExtensionLibrary::Register(lua_State * L)
	{
		RegisterLib(L);
		LuaObjectProxy<CDivinityStats_Character>::RegisterMetatable(L);
		LuaObjectProxy<CharacterDynamicStat>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Item>::RegisterMetatable(L);
		LuaObjectProxy<CDivinityStats_Equipment_Attributes>::RegisterMetatable(L);
		LuaHandleProxy<CDivinityStats_Character>::RegisterMetatable(L);
		LuaHandleProxy<CDivinityStats_Item>::RegisterMetatable(L);
		LuaStatsExtraDataProxy::RegisterMetatable(L);
		LuaStatsProxy::RegisterMetatable(L);
		LuaSkillPrototypeProxy::RegisterMetatable(L);
		LuaDamageList::RegisterMetatable(L);
	}

	int LuaExtensionLibrary::Require(lua_State * L)
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
		throw LuaException();
	}

	LuaState::LuaState()
	{
		state_ = luaL_newstate();
		lua_atpanic(state_, &LuaPanic);
		OpenLibs();
	}

	void RestoreLevelMaps(std::unordered_set<int32_t> const &);

	LuaState::~LuaState()
	{
		RestoreLevelMaps(OverriddenLevelMaps);
		lua_close(state_);
	}

	void LuaState::FinishStartup()
	{
		assert(!startupDone_);
		startupDone_ = true;
	}
		
	void LuaState::OpenLibs()
	{
		const luaL_Reg *lib;
		/* "require" functions from 'loadedlibs' and set results to global table */
		for (lib = loadedlibs; lib->func; lib++) {
			luaL_requiref(state_, lib->name, lib->func, 1);
			lua_pop(state_, 1);  /* remove lib */
		}
	}

	bool LuaState::LoadScript(std::string const & script, std::string const & name)
	{
		std::lock_guard lock(mutex_);

		/* Load the file containing the script we are going to run */
		int status = luaL_loadbufferx(state_, script.c_str(), script.size(), name.c_str(), "text");
		if (status != LUA_OK) {
			OsiError("Failed to parse script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1);  /* pop error message from the stack */
			return false;
		}

		/* Ask Lua to run our little script */
		status = CallWithTraceback(0, LUA_MULTRET);
		if (status != LUA_OK) {
			OsiError("Failed to execute script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1); // pop error message from the stack
			return false;
		}

		return true;
	}

	int LuaState::CallWithTraceback(int narg, int nres)
	{
		auto L = state_;
		int base = lua_gettop(L) - narg;  /* function index */
		lua_pushcfunction(L, &LuaState::TracebackHandler);  /* push message handler */
		lua_insert(L, base);  /* put it under function and args */
		int status = lua_pcall(L, narg, nres, base);
		lua_remove(L, base);  /* remove message handler from the stack */
		return status;
	}

	int LuaState::TracebackHandler(lua_State *L)
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

	std::optional<int32_t> LuaState::GetHitChance(CDivinityStats_Character * attacker, CDivinityStats_Character * target)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_GetHitChance"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto luaAttacker = LuaObjectProxy<CDivinityStats_Character>::New(L, attacker); // stack: fn, attacker
		LuaGameObjectPin<CDivinityStats_Character> _(luaAttacker);
		auto luaTarget = LuaObjectProxy<CDivinityStats_Character>::New(L, target); // stack: fn, attacker, target
		LuaGameObjectPin<CDivinityStats_Character> __(luaTarget);

		if (CallWithTraceback(2, 1) != 0) { // stack: retval
			OsiError("GetHitChance handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return {};
		}

		int isnum;
		int isnil = lua_isnil(L, -1);
		auto retval = lua_tointegerx(L, -1, &isnum);
		lua_pop(L, 1); // stack: -

		if (isnum) {
			return std::clamp((int32_t)retval, 0, 100);
		} else {
			if (!isnil) {
				OsiErrorS("GetHitChance returned non-integer value");
			}

			return {};
		}
	}

	bool LuaState::GetSkillDamage(SkillPrototype * skill, DamagePairList * damageList,
		CRPGStats_ObjectInstance *attacker, bool isFromItem, bool stealthed, float * attackerPosition,
		float * targetPosition, DeathType * pDeathType, int level, bool noRandomization)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_GetSkillDamage"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return {};
		}

		auto luaSkill = LuaSkillPrototypeProxy::New(L, skill, -1); // stack: fn, skill
		LuaSkillPrototypePin _(luaSkill);
		LuaItemOrCharacterPushPin _a(L, attacker);

		lua_push(L, isFromItem);
		lua_push(L, stealthed);
		
		// Push attacker position
		lua_newtable(L);
		luaL_settable(L, 1, attackerPosition[0]);
		luaL_settable(L, 2, attackerPosition[1]);
		luaL_settable(L, 3, attackerPosition[2]);

		// Push target position
		lua_newtable(L);
		luaL_settable(L, 1, targetPosition[0]);
		luaL_settable(L, 2, targetPosition[1]);
		luaL_settable(L, 3, targetPosition[2]);

		lua_push(L, level);
		lua_push(L, noRandomization);

		if (CallWithTraceback(8, 2) != 0) { // stack: damageList, deathType
			OsiError("GetSkillDamage handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok;
		if (isnil) {
			ok = false;
		} else {
			ok = true;

			auto deathType = lua_toenum<DeathType>(L, -1);
			if (deathType) {
				if (pDeathType) {
					*pDeathType = *deathType;
				}
			} else {
				OsiErrorS("GetSkillDamage returned invalid death type");
				ok = false;
			}

			if (ok) {
				auto damages = LuaDamageList::AsUserData(L, -2);
				if (damages) {
					auto const & list = damages->Get();
					for (uint32_t i = 0; i < list.Size; i++) {
						auto const & item = list[i];
						damageList->AddDamage(item.DamageType, item.Amount);
					}
				} else {
					OsiErrorS("GetSkillDamage returned invalid damage list object");
					ok = false;
				}
			}
		}

		lua_pop(L, 2); // stack: -
		return ok;
	}

	void LuaState::OnNetMessageReceived(std::string const & channel, std::string const & payload)
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_NetMessageReceived"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1); // stack: -
			return;
		}

		lua_push(L, channel);
		lua_push(L, payload);

		if (CallWithTraceback(2, 0) != 0) { // stack: retval
			OsiError("NetMessageReceived handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	void LuaState::OnGameSessionLoading()
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient | ScopeSessionLoad);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_OnGameSessionLoading"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn

		if (CallWithTraceback(0, 0) != 0) { // stack: -
			OsiError("Ext.OnGameSessionLoading failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	void LuaState::OnModuleLoading()
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient | ScopeModuleLoad);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_OnModuleLoading"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn

		if (CallWithTraceback(0, 0) != 0) { // stack: -
			OsiError("Ext.OnModuleLoading failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	void LuaState::OnModuleResume()
	{
		std::lock_guard lock(mutex_);
		LuaRestriction restriction(*this, RestrictAllClient | ScopeModuleResume);

		auto L = state_;
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_OnModuleResume"); // stack: Ext, fn
		lua_remove(L, -2); // stack: fn

		if (CallWithTraceback(0, 0) != 0) { // stack: -
			OsiError("Ext.OnModuleResume failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}

	std::string LuaState::GetBuiltinLibrary(int resourceId)
	{
		auto hResource = FindResource(gThisModule, MAKEINTRESOURCE(resourceId),
			L"LUA_SCRIPT");

		if (hResource) {
			auto hGlobal = LoadResource(gThisModule, hResource);
			if (hGlobal) {
				auto resourceData = LockResource(hGlobal);
				if (resourceData) {
					DWORD resourceSize = SizeofResource(gThisModule, hResource);
					std::string script;
					script.resize(resourceSize);
					memcpy(script.data(), resourceData, resourceSize);
					return script;
				}
			}
		}

		OsiErrorS("Could not find bootstrap resource!");
		return std::string();
	}

}
