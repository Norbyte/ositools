#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <GameDefinitions/GameObjects/Ai.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaBinding.h>
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

	auto debugger = dse::gExtender->GetLuaDebugger();
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

	void PushInternalFunction(lua_State* L, char const* func)
	{
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, "_Internal"); // stack: Ext, _Internal
		lua_remove(L, -2); // stack: _I
		lua_getfield(L, -1, func); // stack: _I, fn
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


	class DeprecationHelpers
	{
	public:
		void WarnDeprecated56(char const* msg)
		{
			if (gExtender->GetClient().GetExtensionState().GetMergedConfig().MinimumVersion < 56) {
				return;
			}

			if (seenWarnings_.find(msg) == seenWarnings_.end()) {
				seenWarnings_.insert(msg);
				LogOsirisWarning(std::string("DEPRECATION WARNING: ") + msg);
			}
		}

	private:
		std::unordered_set<char const*> seenWarnings_;
	};

	DeprecationHelpers gDeprecationHelpers;

	void WarnDeprecated56(char const* msg)
	{
		gDeprecationHelpers.WarnDeprecated56(msg);
	}


	void ExtensionLibrary::Register(lua_State * L)
	{
		RegisterLib(L);
	}


#if LUA_VERSION_NUM > 501
	// Unsafe libs (IO, OS, package, debug) removed
	static const luaL_Reg loadedlibs[] = {
	  {"_G", luaopen_base},
	  {LUA_TABLIBNAME, luaopen_table},
	  {LUA_STRLIBNAME, luaopen_string},
	  {LUA_MATHLIBNAME, luaopen_math},
	  {LUA_COLIBNAME, luaopen_coroutine},
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
	  {LUA_COLIBNAME, luaopen_coroutine},
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

	void RegisterLib(lua_State* L, char const* name, luaL_Reg const* lib)
	{
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_createtable(L, 0, 0); // stack: ext, lib
		luaL_setfuncs(L, lib, 0);
		lua_setfield(L, -2, name);
		lua_pop(L, 1);
	}

	void RegisterLib(lua_State* L, char const* name, char const* subTableName, luaL_Reg const* lib)
	{
		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, name); // stack: Ext, parent
		lua_createtable(L, 0, 0); // stack: ext, parent, lib
		luaL_setfuncs(L, lib, 0);
		lua_setfield(L, -2, subTableName);
		lua_pop(L, 2);
	}

	int LuaPanic(lua_State * L)
	{
		char const* err = "(Unknown)";
		if (lua_type(L, -1) == LUA_TSTRING) {
			err = lua_tostring(L, -1);
		}

#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gExtender->GetLuaDebugger();
		if (debugger) {
			debugger->OnLuaError(L, err);
		}
#endif

		throw Exception(err);
	}

	void* LuaAlloc(void* ud, void* ptr, size_t osize, size_t nsize)
	{
		(void)ud; (void)osize;  /* not used */
		if (nsize == 0) {
			GameFree(ptr);
			return NULL;
		} else {
			auto newBuf = GameAllocRaw(nsize);
			if (ptr != nullptr) {
				memcpy(newBuf, ptr, std::min(nsize, osize));
				GameFree(ptr);
			}

			return newBuf;
		}
	}

	State::State()
		: lifetimeStack_(lifetimePool_),
		globalLifetime_(lifetimePool_, lifetimePool_.Allocate())
	{
		globalLifetime_.GetLifetime()->SetInfinite();

		L = lua_newstate(LuaAlloc, nullptr);
		*reinterpret_cast<State**>(lua_getextraspace(L)) = this;
#if LUA_VERSION_NUM <= 501
		luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
#endif
		lua_atpanic(L, &LuaPanic);
		OpenLibs();
	}

	void RestoreLevelMaps(std::unordered_set<int32_t> const &);

	State::~State()
	{
		globalLifetime_.GetLifetime()->ClearInfinite();
		globalLifetime_.GetLifetime()->Kill();
		RestoreLevelMaps(OverriddenLevelMaps);
		lua_close(L);
	}

	State* State::FromLua(lua_State* L)
	{
		return *reinterpret_cast<State**>(lua_getextraspace(L));
	}

	LifetimeHolder State::GetCurrentLifetime()
	{
		if (lifetimeStack_.IsEmpty()) {
			OsiErrorS("Attempted to construct Lua object proxy while lifetime stack is empty");
			return LifetimeHolder(lifetimePool_, nullptr);
		} else {
			return lifetimeStack_.GetCurrent();
		}
	}

	void State::LoadBootstrap(STDString const& path, STDString const& modTable)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);
		LifetimePin _l(lifetimeStack_);
		auto lifetime = lifetimeStack_.GetCurrent();
		PushExtFunction(L, "_LoadBootstrap");
		push(L, path);
		push(L, modTable);
		CheckedCall(L, 2, "Ext.LoadBootstrap");
	}

	void State::FinishStartup()
	{
		assert(!startupDone_);
		startupDone_ = true;

#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gExtender->GetLuaDebugMessageHandler();
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
		LifetimePin _(lifetimeStack_);
		status = CallWithTraceback(L, 0, LUA_MULTRET);
		if (status != LUA_OK) {
			LuaError("Failed to execute script: " << lua_tostring(L, -1));
			lua_pop(L, 1); // pop error message from the stack
			return {};
		}

		return lua_gettop(L) - top;
	}

	std::optional<int32_t> State::GetHitChance(stats::Character * attacker, stats::Character * target)
	{
		GetHitChanceEventParams params{ attacker, target };
		ThrowEvent(*this, "GetHitChance", params, false, RestrictAll);
		return params.HitChance;
	}

	bool State::GetSkillDamage(stats::SkillPrototype * skill, stats::DamagePairList * damageList,
		stats::ObjectInstance *attacker, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
		glm::vec3 const& targetPosition, stats::DeathType * pDeathType, int level, bool noRandomization)
	{
		GetSkillDamageEventParams params{ skill, attacker, isFromItem, stealthed, attackerPosition, targetPosition,
			level, noRandomization };
		ThrowEvent(*this, "GetSkillDamage", params, false, RestrictOsiris);
		if (params.DeathType && params.DamageList.Size > 0) {
			for (auto const& dmg : params.DamageList) {
				damageList->AddDamage(dmg.DamageType, dmg.Amount);
			}

			if (pDeathType) {
				*pDeathType = *params.DeathType;
			}
			return true;
		} else {
			return false;
		}
	}

	std::optional<std::pair<int, bool>> State::GetSkillAPCost(stats::SkillPrototype* skill, stats::Character* character, eoc::AiGrid* aiGrid,
		glm::vec3* position, float* radius)
	{
		GetSkillAPCostEventParams params{ skill, character, aiGrid, position, radius };
		ThrowEvent(*this, "GetSkillAPCost", params, false, RestrictAll);
		if (params.AP && params.ElementalAffinity) {
			return std::make_pair(*params.AP, *params.ElementalAffinity);
		} else {
			return {};
		}
	}

	void State::OnNetMessageReceived(STDString const & channel, STDString const & payload, UserId userId)
	{
		NetMessageEventParams params;
		params.Channel = channel;
		params.Payload = payload;
		params.UserID = userId;
		ThrowEvent(*this, "NetMessageReceived", params);
	}

	void State::OnGameSessionLoading()
	{
		EmptyEventParams params;
		ThrowEvent(*this, "SessionLoading", params, false, RestrictAll | ScopeSessionLoad);
	}

	void State::OnGameSessionLoaded()
	{
		EmptyEventParams params;
		ThrowEvent(*this, "SessionLoaded", params, false, RestrictAll);
	}

	void State::OnModuleLoadStarted()
	{
		EmptyEventParams params;
		ThrowEvent(*this, "ModuleLoadStarted", params, false, RestrictAll | ScopeModulePreLoad);
	}

	void State::OnModuleLoading()
	{
		EmptyEventParams params;
		ThrowEvent(*this, "ModuleLoading", params, false, RestrictAll | ScopeModuleLoad);
	}

	void State::OnStatsLoaded()
	{
		EmptyEventParams params;
		ThrowEvent(*this, "StatsLoaded", params, false, RestrictAll | ScopeModuleLoad);
	}

	void State::OnModuleResume()
	{
		EmptyEventParams params;
		ThrowEvent(*this, "ModuleResume", params, false, RestrictAll | ScopeModuleResume);
	}

	void State::OnResetCompleted()
	{
		EmptyEventParams params;
		ThrowEvent(*this, "ResetCompleted", params, false, 0);
	}

	void State::OnUpdate(GameTime const& time)
	{
		TickEventParams params{ time };
		ThrowEvent(*this, "Tick", params, false, 0);

		lua_gc(L, LUA_GCSTEP, 10);
	}

}
