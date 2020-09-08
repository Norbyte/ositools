#include <stdafx.h>
#include <OsirisProxy.h>
#include <PropertyMaps.h>
#include <GameDefinitions/Ai.h>
#include <GameDefinitions/RootTemplates.h>
#include "LuaBinding.h"
#include "resource.h"
#include <fstream>

// Callback from the Lua runtime when a handled (i.e. pcall/xpcall'd) error was thrown.
// This is needed to capture errors for the Lua debugger, as there is no
// built-in mechanism to capture handled errors.
//
// Lua ldebug.c (line ~639) must be patched like this:
//
// void nse_lua_report_handled_error(lua_State* L);
// 
// l_noret luaG_errormsg(lua_State* L) {
// 	if (L->errfunc != 0) {  /* is there an error handling function? */
// 	  nse_lua_report_handled_error(L);
//    StkId errfunc = restorestack(L, L->errfunc);
//
void nse_lua_report_handled_error(lua_State* L)
{
#if !defined(OSI_NO_DEBUGGER)
	char const* err = "(Unknown)";
	if (lua_type(L, -1) == LUA_TSTRING) {
		err = lua_tostring(L, -1);
	}

	auto debugger = dse::gOsirisProxy->GetLuaDebugger();
	if (debugger) {
		debugger->OnLuaError(L, err);
	}
#endif
}

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

	void RegistryEntry::Push() const
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


	void PushModFunction(lua_State* L, char const* mod, char const* func)
	{
		lua_getglobal(L, "Mods"); // stack: Mods
		lua_getfield(L, -1, mod); // stack: Mods, mod
		lua_remove(L, -2); // stack: mod
		lua_getfield(L, -1, func); // stack: mod, fn
		lua_remove(L, -2); // stack: fn
	}


	int LuaStatGetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, std::optional<int> level);
	int LuaStatSetAttribute(lua_State * L, CRPGStats_Object * object, char const * attributeName, int valueIdx);

	char const * const StatsProxy::MetatableName = "CRPGStats_Object";

	int StatsProxy::Index(lua_State * L)
	{
		if (obj_ == nullptr) return luaL_error(L, "Attempted to read property of null CRPGStats_Object object");
		auto attributeName = luaL_checkstring(L, 2);

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
		stats_ = obj->GetStats();
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

	char const * const ObjectProxy<CDivinityStats_Character>::MetatableName = "CDivinityStats_Character";

	int CharacterFetchStat(lua_State * L, CDivinityStats_Character * stats, char const* propStr, FixedString const& prop)
	{
		if (prop == GFS.strSight) {
			push(L, stats->Sight);
			return 1;
		}

		std::optional<int32_t> dynamicStat;
		if (!prop && strncmp(propStr, "Base", 4) == 0) {
			dynamicStat = stats->GetStat(ToFixedString(propStr + 4), true);
		} else {
			dynamicStat = stats->GetStat(prop, false);
		}

		if (dynamicStat) {
			push(L, *dynamicStat);
			return 1;
		}

		if (prop == GFS.strModId) {
			push(L, gOsirisProxy->GetStatLoadOrderHelper().GetStatsEntryMod(stats->Name));
			return 1;
		}

		if (prop == GFS.strDynamicStats) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto dynamicStat : stats->DynamicStats) {
				push(L, statIdx++);
				ObjectProxy<CharacterDynamicStat>::New(L, dynamicStat);
				lua_settable(L, -3);
			}

			return 1;
		}

		if (prop == GFS.strMainWeapon) {
			auto weapon = stats->GetMainWeapon();
			if (weapon != nullptr) {
				ObjectProxy<CDivinityStats_Item>::New(L, weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (prop == GFS.strOffHandWeapon) {
			auto weapon = stats->GetOffHandWeapon();
			if (weapon != nullptr) {
				ObjectProxy<CDivinityStats_Item>::New(L, weapon);
				return 1;
			} else {
				return 0;
			}
		}

		if (strncmp(propStr, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(propStr + 7);
			if (talentId) {
				bool hasTalent = stats->HasTalent(*talentId, false);
				push(L, hasTalent);
				return 1;
			} else {
				return 0;
			}
		}

		if (prop == GFS.strNotSneaking) {
			push(L, (bool)(stats->Flags & StatCharacterFlags::IsSneaking));
			return 1;
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = stats->GetAbility(*abilityId, false);
			push(L, abilityLevel);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterStatsPropertyMap, stats, prop, false);
		if (fetched) {
			return 1;
		}

		if (stats->Character != nullptr) {
			if (prop == GFS.strCharacter) {
				ObjectHandle handle;
				stats->Character->GetObjectHandle(handle);
				if (handle.GetType() == (uint32_t)ObjectType::ClientCharacter) {
					ObjectProxy<ecl::Character>::New(L, handle);
				} else if (handle.GetType() == (uint32_t)ObjectType::ServerCharacter) {
					ObjectProxy<esv::Character>::New(L, handle);
				} else {
					ERR("Unknown character handle type: %d", handle.GetType());
					push(L, nullptr);
				}
				return 1;
			}

			if (prop == GFS.strRotation) {
				push(L, *stats->Character->GetRotation());
				return 1;
			}

			if (prop == GFS.strPosition) {
				push(L, *stats->Character->GetTranslate());
				return 1;
			}

			if (prop == GFS.strMyGuid) {
				push(L, *stats->Character->GetGuid());
				return 1;
			}

			if (prop == GFS.strNetID) {
				push(L, stats->Character->NetID);
				return 1;
			}
		}

		OsiError("Unknown character stats property: " << prop);
		return 0;
	}

	CDivinityStats_Character* ObjectProxy<CDivinityStats_Character>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto character = esv::GetEntityWorld()->GetCharacter(handle_);
		if (character == nullptr) luaL_error(L, "Character handle invalid");
		if (character->Stats == nullptr) luaL_error(L, "Character has no stats!");
		return character->Stats;
	}

	int CharacterGetItemBySlot(lua_State* L)
	{
		auto self = checked_get<ObjectProxy<CDivinityStats_Character>*>(L, 1);
		auto slot = checked_get<ItemSlot>(L, 2);

		auto item = self->Get(L)->GetItemBySlot(slot, true);
		if (item != nullptr) {
			ObjectProxy<CDivinityStats_Item>::New(L, item);
			return 1;
		} else {
			return 0;
		}
	}
	
	int ObjectProxy<CDivinityStats_Character>::Index(lua_State * L)
	{
		auto stats = Get(L);
		if (!stats) return 0;
		
		auto prop = luaL_checkstring(L, 2);
		auto fs = ToFixedString(prop);

		if (fs == GFS.strGetItemBySlot) {
			lua_pushcfunction(L, &CharacterGetItemBySlot);
			return 1;
		}

		return CharacterFetchStat(L, stats, prop, fs);
	}

	int ObjectProxy<CDivinityStats_Character>::NewIndex(lua_State * L)
	{
		return GenericSetter(L, gCharacterStatsPropertyMap, true);
	}

	int ItemFetchStat(lua_State * L, CDivinityStats_Item * item, char const * prop)
	{
		if (strcmp(prop, "DynamicStats") == 0) {
			lua_newtable(L);
			unsigned statIdx = 1;
			for (auto dynamicStat : item->DynamicAttributes) {
				push(L, statIdx++);
				ObjectProxy<CDivinityStats_Equipment_Attributes>::New(L, dynamicStat);
				lua_settable(L, -3);
			}

			return 1;
		}

		if (strncmp(prop, "TALENT_", 7) == 0) {
			auto talentId = EnumInfo<TalentType>::Find(prop + 7);
			if (talentId) {
				bool hasTalent = item->HasTalent(*talentId);
				push(L, hasTalent);
				return 1;
			} else {
				return 0;
			}
		}

		auto abilityId = EnumInfo<AbilityType>::Find(prop);
		if (abilityId) {
			int abilityLevel = item->GetAbility(*abilityId);
			push(L, abilityLevel);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gItemStatsPropertyMap, item, prop, false);
		if (fetched) {
			return 1;
		}

		return LuaStatGetAttribute(L, item, prop, {});
	}


	char const * const ObjectProxy<CDivinityStats_Item>::MetatableName = "CDivinityStats_Item";

	CDivinityStats_Item* ObjectProxy<CDivinityStats_Item>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		auto item = esv::GetEntityWorld()->GetItem(handle_);
		if (item == nullptr) luaL_error(L, "Item handle invalid");
		if (item->Stats == nullptr) luaL_error(L, "Item has no stats!");
		return item->Stats;
	}

	int ObjectProxy<CDivinityStats_Item>::Index(lua_State * L)
	{
		auto stats = Get(L);
		if (!stats) return 0;


		auto prop = luaL_checkstring(L, 2);
		return ItemFetchStat(L, stats, prop);
	}

	int ObjectProxy<CDivinityStats_Item>::NewIndex(lua_State * L)
	{
		return GenericSetter(L, gItemStatsPropertyMap, true);
	}


	char const * const ObjectProxy<CDivinityStats_Equipment_Attributes>::MetatableName = "CDivinityStats_Equipment_Attributes";

	CDivinityStats_Equipment_Attributes* ObjectProxy<CDivinityStats_Equipment_Attributes>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "Equipment stats no longer available");
		return nullptr;
	}

	int ObjectProxy<CDivinityStats_Equipment_Attributes>::Index(lua_State * L)
	{
		auto stats = Get(L);
		if (!stats) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto & propMap = obj_->GetPropertyMap();
		auto fetched = LuaPropertyMapGet(L, propMap, stats, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<CDivinityStats_Equipment_Attributes>::NewIndex(lua_State * L)
	{
		auto stats = Get(L);
		if (!stats) return 0;

		auto& propMap = obj_->GetPropertyMap();
		return GenericSetter(L, propMap, true);
	}


	char const * const ObjectProxy<CharacterDynamicStat>::MetatableName = "CharacterDynamicStat";

	CharacterDynamicStat* ObjectProxy<CharacterDynamicStat>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "Character stats no longer available");
		return nullptr;
	}

	int ObjectProxy<CharacterDynamicStat>::Index(lua_State * L)
	{
		auto stats = Get(L);
		if (!stats) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gCharacterDynamicStatPropertyMap, stats, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<CharacterDynamicStat>::NewIndex(lua_State * L)
	{
		return GenericSetter(L, gCharacterDynamicStatPropertyMap, true);
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


	char const * const StatsExtraDataProxy::MetatableName = "CRPGStats_ExtraData";

	int StatsExtraDataProxy::Index(lua_State * L)
	{
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr || stats->ExtraData == nullptr) return luaL_error(L, "Stats not available");

		auto key = luaL_checkstring(L, 2);
		auto extraData = stats->ExtraData->Properties.Find(ToFixedString(key));
		if (extraData != nullptr) {
			push(L, *extraData);
			return 1;
		} else {
			return 0;
		}
	}




	char const* const ObjectProxy<CombatComponentTemplate>::MetatableName = "eoc::CombatComponentTemplate";

	CombatComponentTemplate* ObjectProxy<CombatComponentTemplate>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "CharacterTemplate not bound!");
		return nullptr;
	}

	int ObjectProxy<CombatComponentTemplate>::Index(lua_State* L)
	{
		auto obj = Get(L);
		if (!obj) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gCombatComponentTemplatePropertyMap, obj, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<CombatComponentTemplate>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gCombatComponentTemplatePropertyMap, true);
	}



	char const* const ObjectProxy<CharacterTemplate>::MetatableName = "eoc::CharacterTemplate";

	CharacterTemplate* ObjectProxy<CharacterTemplate>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "CharacterTemplate not bound!");
		return nullptr;
	}

	int ObjectProxy<CharacterTemplate>::Index(lua_State* L)
	{
		auto obj = Get(L);
		if (!obj) return 0;

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "CombatTemplate") == 0) {
			ObjectProxy<CombatComponentTemplate>::New(L, &obj->CombatComponent);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gCharacterTemplatePropertyMap, obj, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<CharacterTemplate>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gCharacterTemplatePropertyMap, true);
	}



	char const* const ObjectProxy<ItemTemplate>::MetatableName = "eoc::ItemTemplate";

	ItemTemplate* ObjectProxy<ItemTemplate>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "ItemTemplate not bound!");
		return nullptr;
	}

	int ObjectProxy<ItemTemplate>::Index(lua_State* L)
	{
		auto obj = Get(L);
		if (!obj) return 0;

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "CombatTemplate") == 0) {
			ObjectProxy<CombatComponentTemplate>::New(L, &obj->CombatComponent);
			return 1;
		}

		auto fetched = LuaPropertyMapGet(L, gItemTemplatePropertyMap, obj, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<ItemTemplate>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gItemTemplatePropertyMap, true);
	}



	char const* const ObjectProxy<ProjectileTemplate>::MetatableName = "eoc::ProjectileTemplate";

	ProjectileTemplate* ObjectProxy<ProjectileTemplate>::Get(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "ProjectileTemplate not bound!");
		return nullptr;
	}

	int ObjectProxy<ProjectileTemplate>::Index(lua_State* L)
	{
		auto obj = Get(L);
		if (!obj) return 0;

		auto prop = luaL_checkstring(L, 2);
		auto fetched = LuaPropertyMapGet(L, gProjectileTemplatePropertyMap, obj, prop, true);
		return fetched ? 1 : 0;
	}

	int ObjectProxy<ProjectileTemplate>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gProjectileTemplatePropertyMap, true);
	}


	char const* const ObjectProxy<eoc::AiGrid>::MetatableName = "eoc::AiGrid";

	eoc::AiGrid* ObjectProxy<eoc::AiGrid>::Get(lua_State* L)
	{
		return obj_;
	}

	int AiGridSearchForCell(lua_State* L)
	{
		auto grid = ObjectProxy<eoc::AiGrid>::CheckedGet(L, 1);
		auto x = checked_get<float>(L, 2);
		auto z = checked_get<float>(L, 3);
		auto radius = checked_get<float>(L, 4);
		auto flags = checked_get_flags<ESurfaceFlag>(L, 5);
		auto bias = checked_get<float>(L, 6);

		auto search = GetStaticSymbols().eoc__AiGrid__SearchForCell;
		if (!search) {
			OsiError("AiGrid::SearchForCell not mapped!");
			return 0;
		}

		auto result = search(grid, x, z, radius, flags, nullptr, bias);
		push(L, !result);
		return 1;
	}

	int AiGridGetCellInfo(lua_State* L)
	{
		auto grid = ObjectProxy<eoc::AiGrid>::CheckedGet(L, 1);
		auto x = checked_get<float>(L, 2);
		auto z = checked_get<float>(L, 3);

		auto cell = grid->GetCell(glm::vec2(x, z));
		if (!cell) {
			OsiError("Could not find AiGrid cell at " << x << ";" << z);
			return 0;
		}

		auto groundIdx = grid->GetSurfaceIndex(cell, 0);
		auto cloudIdx = grid->GetSurfaceIndex(cell, 1);
		auto meta = grid->GetAiMetaData(cell);

		auto height = cell->Height * 0.25f + grid->DataGrid.OffsetY;

		lua_newtable(L);
		settable(L, "Flags", cell->AiFlags);
		settable(L, "Height", height);

		lua_newtable(L);
		if (meta != nullptr) {
			int32_t aiIdx = 1;
			for (auto ai : meta->Ai) {
				ObjectHandle handle;
				ai->GameObject->GetObjectHandle(handle);
				settable(L, aiIdx++, handle);
			}
		}

		lua_setfield(L, -2, "Objects");

		if (gOsirisProxy->IsInServerThread()) {
			auto level = GetStaticSymbols().GetCurrentServerLevel();
			if (!level || level->AiGrid != grid || !level->SurfaceManager) {
				OsiError("Current level not available yet!");
			} else {
				if (groundIdx != -1) {
					auto surface = level->SurfaceManager->Surfaces[groundIdx];
					settable(L, "GroundSurface", surface->MyHandle);
				}

				if (cloudIdx != -1) {
					auto surface = level->SurfaceManager->Surfaces[cloudIdx];
					settable(L, "CloudSurface", surface->MyHandle);
				}
			}
		}

		return 1;
	}

	int AiGridGetAiFlags(lua_State* L)
	{
		auto grid = ObjectProxy<eoc::AiGrid>::CheckedGet(L, 1);
		auto x = checked_get<float>(L, 2);
		auto z = checked_get<float>(L, 3);

		auto cell = grid->GetCell(glm::vec2(x, z));
		if (!cell) {
			OsiError("Could not find AiGrid cell at " << x << ";" << z);
			return 0;
		}

		push(L, cell->AiFlags);
		return 1;
	}

	int AiGridSetAiFlags(lua_State* L)
	{
		// Only allow updating Walkable/Reachable flags for now
		constexpr uint64_t UpdateFlags = 5;

		auto grid = ObjectProxy<eoc::AiGrid>::CheckedGet(L, 1);
		auto x = checked_get<float>(L, 2);
		auto z = checked_get<float>(L, 3);
		auto flags = checked_get<uint64_t>(L, 4);

		auto cell = grid->GetCell(glm::vec2(x, z));
		if (!cell) {
			OsiError("Could not find AiGrid cell at " << x << ";" << z);
			return 0;
		}

		cell->AiFlags = (flags & UpdateFlags) | (cell->AiFlags & ~UpdateFlags);
		return 0;
	}

	int ObjectProxy<eoc::AiGrid>::Index(lua_State* L)
	{
		auto grid = Get(L);
		if (!grid) return 0;

		auto prop = checked_get<FixedString>(L, 2);

		if (prop == GFS.strSearchForCell) {
			lua_pushcfunction(L, &AiGridSearchForCell);
			return 1;
		}

		if (prop == GFS.strGetCellInfo) {
			lua_pushcfunction(L, &AiGridGetCellInfo);
			return 1;
		}

		if (prop == GFS.strGetAiFlags) {
			lua_pushcfunction(L, &AiGridGetAiFlags);
			return 1;
		}

		if (prop == GFS.strSetAiFlags) {
			lua_pushcfunction(L, &AiGridSetAiFlags);
			return 1;
		}

		if (prop == GFS.strOffsetX) {
			push(L, grid->DataGrid.OffsetX);
			return 1;
		}

		if (prop == GFS.strOffsetY) {
			push(L, grid->DataGrid.OffsetY);
			return 1;
		}

		if (prop == GFS.strOffsetZ) {
			push(L, grid->DataGrid.OffsetZ);
			return 1;
		}

		if (prop == GFS.strGridScale) {
			push(L, grid->DataGrid.GridScale);
			return 1;
		}

		if (prop == GFS.strWidth) {
			push(L, (double)grid->DataGrid.Width * grid->DataGrid.GridScale);
			return 1;
		}

		if (prop == GFS.strHeight) {
			push(L, (double)grid->DataGrid.Height * grid->DataGrid.GridScale);
			return 1;
		}

		OsiError("Unknown eoc::AiGrid property: " << prop);
		return 0;
	}

	int ObjectProxy<eoc::AiGrid>::NewIndex(lua_State* L)
	{
		return luaL_error(L, "__newindex not supported for eoc::AiGrid");
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
		for (auto const& dmg : self->damages_) {
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

		for (auto& dmg : self->damages_) {
			dmg.Amount = (int32_t)round(dmg.Amount * multiplier);
		}

		return 0;
	}

	int DamageList::Merge(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		auto other = DamageList::CheckUserData(L, 2);

		for (auto const& dmg : other->damages_) {
			self->damages_.AddDamage(dmg.DamageType, dmg.Amount);
		}

		return 0;
	}

	int DamageList::ConvertDamageType(lua_State * L)
	{
		auto self = DamageList::CheckUserData(L, 1);
		auto damageType = checked_get<DamageType>(L, 2);

		int32_t totalDamage = 0;
		for (auto const& dmg : self->damages_) {
			totalDamage += dmg.Amount;
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
			setfield(L, "DamageType", item.DamageType);
			setfield(L, "Amount", item.Amount);

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
		ObjectProxy<CharacterTemplate>::RegisterMetatable(L);
		ObjectProxy<ItemTemplate>::RegisterMetatable(L);
		ObjectProxy<ProjectileTemplate>::RegisterMetatable(L);
		ObjectProxy<CombatComponentTemplate>::RegisterMetatable(L);
		ObjectProxy<eoc::AiGrid>::RegisterMetatable(L);
		StatsExtraDataProxy::RegisterMetatable(L);
		StatsProxy::RegisterMetatable(L);
		SkillPrototypeProxy::RegisterMetatable(L);
		DamageList::RegisterMetatable(L);
	}

	int ExtensionLibrary::Include(lua_State * L)
	{
		auto modGuid = luaL_checkstring(L, 1);
		auto fileName = luaL_checkstring(L, 2);

		bool replaceGlobals = !lua_isnil(L, 3);
		auto globalsIdx = lua_gettop(L) + 1;

		if (replaceGlobals) {
			luaL_checktype(L, 3, LUA_TTABLE);
#if LUA_VERSION_NUM > 501
			lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
			lua_pushvalue(L, 3);
			lua_rawseti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
#endif
		}

		auto nret = gOsirisProxy->GetCurrentExtensionState()
			->LuaLoadModScript(modGuid, fileName, true, replaceGlobals ? 3 : 0);

		if (replaceGlobals) {
#if LUA_VERSION_NUM > 501
			lua_pushvalue(L, globalsIdx);
			lua_rawseti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
			lua_remove(L, globalsIdx);
#endif
		}

		if (nret) {
			return *nret;
		} else {
			return 0;
		}
	}


#if LUA_VERSION_NUM > 501
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

#else
	// Unsafe libs (IO, OS, package, debug) removed
	static const luaL_Reg loadedlibs[] = {
	  {"", luaopen_base},
	  {LUA_TABLIBNAME, luaopen_table},
	  {LUA_STRLIBNAME, luaopen_string},
	  {LUA_MATHLIBNAME, luaopen_math},
	  {LUA_BITLIBNAME, luaopen_bit},
	  // debug table is stripped in the sandbox startup script
	  {LUA_DBLIBNAME, luaopen_debug},
	  {NULL, NULL}
	};

	static const luaL_Reg lj_lib_preload[] = {
	#if LJ_HASFFI
	  { LUA_FFILIBNAME,	luaopen_ffi },
	#endif
	  { NULL,		NULL }
	};

#endif

	int LuaPanic(lua_State * L)
	{
		char const* err = "(Unknown)";
		if (lua_type(L, -1) == LUA_TSTRING) {
			err = lua_tostring(L, -1);
		}

#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gOsirisProxy->GetLuaDebugger();
		if (debugger) {
			debugger->OnLuaError(L, err);
		}
#endif

		throw Exception(err);
	}

	State::State()
	{
		L = luaL_newstate();
#if LUA_VERSION_NUM <= 501
		luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
#endif
		lua_atpanic(L, &LuaPanic);
		OpenLibs();
	}

	void RestoreLevelMaps(std::unordered_set<int32_t> const &);

	State::~State()
	{
		RestoreLevelMaps(OverriddenLevelMaps);
		lua_close(L);
	}

	void State::LoadBootstrap(STDString const& path, STDString const& modTable)
	{
		CallExt("_LoadBootstrap", RestrictAll, ReturnType<>{}, path, modTable);
	}

	void State::FinishStartup()
	{
		assert(!startupDone_);
		startupDone_ = true;

#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gOsirisProxy->GetLuaDebugMessageHandler();
		if (debugger && debugger->IsDebuggerReady()) {
			debugger->SendModInfo();
		}
#endif
	}
		
	void State::OpenLibs()
	{
#if LUA_VERSION_NUM > 501
		const luaL_Reg *lib;
		/* "require" functions from 'loadedlibs' and set results to global table */
		for (lib = loadedlibs; lib->func; lib++) {
			luaL_requiref(L, lib->name, lib->func, 1);
			lua_pop(L, 1);  /* remove lib */
		}
#else
		const luaL_Reg* lib;
		for (lib = loadedlibs; lib->func; lib++) {
			lua_pushcfunction(L, lib->func);
			lua_pushstring(L, lib->name);
			lua_call(L, 1, 0);
		}
		luaL_findtable(L, LUA_REGISTRYINDEX, "_PRELOAD",
			sizeof(lj_lib_preload) / sizeof(lj_lib_preload[0]) - 1);
		for (lib = lj_lib_preload; lib->func; lib++) {
			lua_pushcfunction(L, lib->func);
			lua_setfield(L, -2, lib->name);
		}
		lua_pop(L, 1);
#endif
	}

	std::optional<int> State::LoadScript(STDString const & script, STDString const & name, int globalsIdx)
	{
		int top = lua_gettop(L);

		/* Load the file containing the script we are going to run */
		int status = luaL_loadbufferx(L, script.c_str(), script.size(), name.c_str(), "text");
		if (status != LUA_OK) {
			LuaError("Failed to parse script: " << lua_tostring(L, -1));
			lua_pop(L, 1);  /* pop error message from the stack */
			return {};
		}

#if LUA_VERSION_NUM <= 501
		if (globalsIdx != 0) {
			lua_pushvalue(L, globalsIdx);
			lua_setfenv(L, -2);
		}
#endif

		/* Ask Lua to run our little script */
		status = CallWithTraceback(L, 0, LUA_MULTRET);
		if (status != LUA_OK) {
			LuaError("Failed to execute script: " << lua_tostring(L, -1));
			lua_pop(L, 1); // pop error message from the stack
			return {};
		}

		return lua_gettop(L) - top;
	}

	std::optional<int32_t> State::GetHitChance(CDivinityStats_Character * attacker, CDivinityStats_Character * target)
	{
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_GetHitChance"); // stack: fn
		auto _{ PushArguments(L,
			std::tuple{Push<ObjectProxy<CDivinityStats_Character>>(attacker),
			Push<ObjectProxy<CDivinityStats_Character>>(target)}) };

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

				for (auto const& dmg : (*damages)->Get()) {
					damageList->AddDamage(dmg.DamageType, dmg.Amount);
				}

				return true;
			}
		}

		return false;
	}

	std::optional<std::pair<int, bool>> State::GetSkillAPCost(SkillPrototype* skill, CDivinityStats_Character* character, eoc::AiGrid* aiGrid,
		glm::vec3* position, float* radius)
	{
		Restriction restriction(*this, RestrictAll);

		PushExtFunction(L, "_GetSkillAPCost");

		auto luaSkill = SkillPrototypeProxy::New(L, skill, -1);
		UnbindablePin _(luaSkill);
		ItemOrCharacterPushPin _ch(L, character);

		if (aiGrid != nullptr) {
			ObjectProxy<eoc::AiGrid>::New(L, aiGrid);
		} else {
			push(L, nullptr);
		}

		if (position != nullptr) {
			push(L, *position);
		} else {
			push(L, nullptr);
		}

		if (radius != nullptr) {
			push(L, *radius);
		} else {
			push(L, nullptr);
		}

		auto result = CheckedCall<std::optional<bool>, std::optional<int>>(L, 5, "Ext.GetSkillAPCost");
		if (result) {
			auto ap = std::get<1>(*result);
			auto elementalAffinity = std::get<0>(*result);

			if (ap && elementalAffinity) {
				return std::make_pair(*ap, *elementalAffinity);
			}
		}

		return {};
	}

	void State::OnNetMessageReceived(STDString const & channel, STDString const & payload, UserId userId)
	{
		CallExt("_NetMessageReceived", 0, ReturnType<>{}, channel, payload, userId.Id);
	}

	void State::OnGameSessionLoading()
	{
		CallExt("_OnGameSessionLoading", RestrictAll | ScopeSessionLoad, ReturnType<>{});
	}

	void State::OnGameSessionLoaded()
	{
		CallExt("_OnGameSessionLoaded", RestrictAll, ReturnType<>{});
	}

	void State::OnModuleLoadStarted()
	{
		CallExt("_OnModuleLoadStarted", RestrictAll | ScopeModulePreLoad, ReturnType<>{});
	}

	void State::OnModuleLoading()
	{
		CallExt("_OnModuleLoading", RestrictAll | ScopeModuleLoad, ReturnType<>{});
	}

	void State::OnStatsLoaded()
	{
		CallExt("_OnStatsLoaded", RestrictAll | ScopeModuleLoad, ReturnType<>{});
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
