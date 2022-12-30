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
	void push(lua_State* L, EntityHandle const& h)
	{
		auto state = State::FromLua(L);
		if (state->IsClient()) {
			EntityProxy::Make(L, h, ecl::GetEntityWorld());
		} else {
			EntityProxy::Make(L, h, esv::GetEntityWorld());
		}
	}


	RegistryEntry::RegistryEntry()
		: L_(nullptr), ref_(-1)
	{}

	RegistryEntry::RegistryEntry(lua_State * L, int index)
		: L_(L)
	{
		lua_pushvalue(L, index);
		ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	RegistryEntry::RegistryEntry(lua_State* L, Ref const& local)
		: L_(L)
	{
		if ((bool)local) {
			local.Push(L);
			ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
		} else {
			ref_ = -1;
		}
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
		if (ref_ != -1) {
			luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
		}

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

	void RegistryEntry::Bind(lua_State* L, Ref const& ref)
	{
		assert(ref_ == -1);

		L_ = L;
		if (ref) {
			ref.Push(L);
			ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
		} else {
			ref_ = -1;
		}
	}


	PersistentRegistryEntry::PersistentRegistryEntry()
		: L_(nullptr), generationId_(0), ref_(-1)
	{}

	PersistentRegistryEntry::PersistentRegistryEntry(lua_State * L, int index)
		: L_(L), generationId_(get_generation_id(L))
	{
		lua_pushvalue(L, index);
		ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	PersistentRegistryEntry::PersistentRegistryEntry(lua_State* L, Ref const& local)
		: L_(L), generationId_(get_generation_id(L))
	{
		if ((bool)local) {
			local.Push(L);
			ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
		} else {
			ref_ = -1;
		}
	}

	PersistentRegistryEntry::~PersistentRegistryEntry()
	{
		Release();
	}

	PersistentRegistryEntry::PersistentRegistryEntry(PersistentRegistryEntry&& other)
		: L_(other.L_), generationId_(other.generationId_), ref_(other.ref_)
	{
		other.ref_ = -1;
	}

	PersistentRegistryEntry& PersistentRegistryEntry::operator = (PersistentRegistryEntry&& other)
	{
		Release();

		L_ = other.L_;
		generationId_ = other.generationId_;
		ref_ = other.ref_;
		other.ref_ = -1;
		return *this;
	}

	void PersistentRegistryEntry::Release()
	{
		if (ref_ != -1) {
			auto state = gExtender->GetCurrentExtensionState()->GetLua();
			if (state && state->GetGenerationId() == generationId_) {
				luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
			}

			ref_ = -1;
		}
	}

	bool PersistentRegistryEntry::IsValid(lua_State* L) const
	{
		return L != nullptr 
			&& ref_ != -1
			&& State::FromLua(L)->GetGenerationId() == generationId_;
	}

	bool PersistentRegistryEntry::TryPush(lua_State* L) const
	{
		if (IsValid(L)) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref_);
			return true;
		} else {
			return false;
		}
	}

	void PersistentRegistryEntry::Bind(lua_State* L, Ref const& ref)
	{
		assert(ref_ == -1);

		L_ = L;
		generationId_ = get_generation_id(L);
		if (ref) {
			ref.Push(L);
			ref_ = luaL_ref(L, LUA_REGISTRYINDEX);
		} else {
			ref_ = -1;
		}
	}


	int TracebackHandler(lua_State * L)
	{
		const char *msg = lua_tostring(L, 1);
		if (msg == NULL) {  /* is error object not a string? */
			if (luaL_callmetaevent(L, 1, TM_TOSTRING) &&  /* does it have a metamethod */
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
#if defined(DEBUG)
		assert(gExtender->GetCurrentExtensionState()->GetLuaRefs() > 0);
#endif

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


	ModuleRegistry gModuleRegistry;

	void ModuleRegistry::RegisterModule(ModuleDefinition const& module)
	{
		modules_.push_back(module);
	}

	void ModuleRegistry::ConstructState(lua_State* L, ModuleRole role)
	{
		for (auto const& module : modules_) {
			if (role == module.Role || module.Role == ModuleRole::Both) {
				InstantiateModule(L, module);
			}
		}
	}

	void ModuleRegistry::RegisterTypeInformation()
	{
		assert(!modules_.empty());
		for (auto const& module : modules_) {
			RegisterModuleTypeInformation(module);
		}
	}

	void ModuleRegistry::RegisterModuleTypeInformation(ModuleDefinition const& module)
	{
		STDString name;
		name = module.Table.GetString();
		if (module.SubTable) {
			name += ".";
			name += module.SubTable.GetString();
		}

		STDString exportName = "Module_";
		switch (module.Role) {
		case ModuleRole::Both: break;
		case ModuleRole::Client: exportName += "Client"; break;
		case ModuleRole::Server: exportName += "Server"; break;
		}

		exportName += name;

		TypeInformation& mod = TypeInformationRepository::GetInstance().RegisterType(FixedString{ exportName });
		mod.Kind = LuaTypeId::Module;
		mod.NativeName = FixedString(name);
		switch (module.Role) {
		case ModuleRole::Both: mod.ModuleRole = FixedString("Both"); break;
		case ModuleRole::Client: mod.ModuleRole = FixedString("Client"); break;
		case ModuleRole::Server: mod.ModuleRole = FixedString("Server"); break;
		}

		for (auto const& func : module.Functions) {
			mod.Methods.insert(std::make_pair(func.Name, func.Signature));
		}
	}

	void ModuleRegistry::MakeLuaFunctionTable(ModuleDefinition const& module, std::vector<luaL_Reg>& lib)
	{
		lib.reserve(module.Functions.size() + 1);
		for (auto const& fun : module.Functions) {
			lib.push_back({ fun.Name.GetString(), fun.Func });
		}
		lib.push_back({ nullptr, nullptr });
	}

	void ModuleRegistry::InstantiateNamedModule(lua_State* L, char const* name, ModuleDefinition const& module)
	{
		std::vector<luaL_Reg> lib;
		MakeLuaFunctionTable(module, lib);

		lua_getglobal(L, "Ext"); // stack: Ext
		lua_createtable(L, 0, 0); // stack: ext, lib
		luaL_setfuncs(L, lib.data(), 0);
		lua_setfield(L, -2, name);
		lua_pop(L, 1);
	}

	void ModuleRegistry::InstantiateNamedModule(lua_State* L, char const* name, char const* subTableName, ModuleDefinition const& module)
	{
		std::vector<luaL_Reg> lib;
		MakeLuaFunctionTable(module, lib);

		lua_getglobal(L, "Ext"); // stack: Ext
		lua_getfield(L, -1, name); // stack: Ext, parent
		lua_createtable(L, 0, 0); // stack: ext, parent, lib
		luaL_setfuncs(L, lib.data(), 0);
		lua_setfield(L, -2, subTableName);
		lua_pop(L, 2);
	}

	void ModuleRegistry::InstantiateModule(lua_State* L, char const* prefix, ModuleDefinition const& module)
	{
		if (!module.SubTable) {
			InstantiateNamedModule(L, (STDString(prefix) + module.Table.GetString()).c_str(), module);
		} else {
			InstantiateNamedModule(L, (STDString(prefix) + module.Table.GetString()).c_str(), module.SubTable.GetString(), module);
		}
	}

	void ModuleRegistry::InstantiateModule(lua_State* L, ModuleDefinition const& module)
	{
		InstantiateModule(L, "", module);

		if (module.Role == ModuleRole::Both || module.Role == ModuleRole::Server) {
			InstantiateModule(L, "Server", module);
		}

		if (module.Role == ModuleRole::Both || module.Role == ModuleRole::Client) {
			InstantiateModule(L, "Client", module);
		}
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

	State::State(uint32_t generationId)
		: generationId_(generationId),
		lifetimeStack_(lifetimePool_),
		globalLifetime_(lifetimePool_.Allocate())
	{
		L = lua_newstate(LuaAlloc, nullptr);
		internal_ = lua_new_internal_state();
		lua_setup_cppobjects(L, &LuaCppAlloc, &LuaCppFree, &LuaCppGetLightMetatable, &LuaCppGetMetatable, &LuaCppCanonicalize);
		lua_setup_strcache(L, &LuaCacheString, &LuaReleaseString);
		*reinterpret_cast<State**>(lua_getextraspace(L)) = this;
#if LUA_VERSION_NUM <= 501
		luaJIT_setmode(L, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
#endif
		lua_atpanic(L, &LuaPanic);
		OpenLibs();
	}

	void RestoreLevelMaps(bool isClient);

	State::~State()
	{
		lifetimePool_.Release(globalLifetime_);
		lua_close(L);
		if (internal_) {
			lua_release_internal_state(internal_);
		}
	}

	void State::Shutdown()
	{
		RestoreLevelMaps(IsClient());
		customDamageTypes_.Clear();
	}

	State* State::FromLua(lua_State* L)
	{
		return *reinterpret_cast<State**>(lua_getextraspace(L));
	}

	LifetimeHandle State::GetCurrentLifetime()
	{
		if (lifetimeStack_.IsEmpty()) {
			OsiErrorS("Attempted to construct Lua object proxy while lifetime stack is empty");
			return LifetimeHandle{};
		} else {
			return lifetimeStack_.GetCurrent();
		}
	}

	void State::LoadBootstrap(STDString const& path, STDString const& modTable)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);
		LifetimeStackPin _l(lifetimeStack_);
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
		LifetimeStackPin _(lifetimeStack_);
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
		GetHitChanceEvent params { 
			.Attacker = attacker, 
			.Target = target
		};
		ThrowEvent("GetHitChance", params, false, RestrictAll);
		return params.HitChance;
	}

	std::optional<int32_t> State::GetAttackAPCost(stats::Character* attacker)
	{
		GetAttackAPCostEvent params {
			.Attacker = attacker
		};
		ThrowEvent("GetAttackAPCost", params, false, RestrictAll);
		return params.APCost;
	}

	bool State::GetSkillDamage(stats::SkillPrototype * skill, stats::DamagePairList * damageList,
		stats::ObjectInstance *attacker, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
		glm::vec3 const& targetPosition, stats::DeathType * pDeathType, int level, bool noRandomization)
	{
		GetSkillDamageEvent params { 
			.Skill = skill, 
			.Attacker = attacker, 
			.IsFromItem = isFromItem, 
			.Stealthed = stealthed, 
			.AttackerPosition = attackerPosition, 
			.TargetPosition = targetPosition,
			.Level = level, 
			.NoRandomization = noRandomization
		};
		ThrowEvent("GetSkillDamage", params, false, RestrictOsiris);
		if (params.DeathType && params.DamageList.size() > 0) {
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
		GetSkillAPCostEvent params { 
			.Skill = skill, 
			.Character = character, 
			.AiGrid = aiGrid, 
			.Position = position, 
			.Radius = radius
		};
		ThrowEvent("GetSkillAPCost", params, false, RestrictAll);
		if (params.AP && params.ElementalAffinity) {
			return std::make_pair(*params.AP, *params.ElementalAffinity);
		} else {
			return {};
		}
	}

	void State::OnNetMessageReceived(STDString const & channel, STDString const & payload, UserId userId)
	{
		NetMessageEvent params;
		params.Channel = channel;
		params.Payload = payload;
		params.UserID = userId;
		ThrowEvent("NetMessageReceived", params);
	}

	EventResult State::DispatchEvent(EventBase& evt, char const* eventName, bool canPreventAction, uint32_t restrictions)
	{
		auto stackSize = lua_gettop(L) - 2;

		try {
			Restriction restriction(*this, restrictions);
			evt.Name = FixedString(eventName);
			evt.CanPreventAction = canPreventAction;

			if (!CheckedCall(L, 1, "_ThrowEvent")) {
				return EventResult::Failed;
			}

			if (evt.ActionPrevented) {
				return EventResult::ActionPrevented;
			} else {
				return EventResult::Successful;
			}
		} catch (Exception&) {
			auto stackRemaining = lua_gettop(L) - stackSize;
			if (stackRemaining > 0) {
				LuaError("Failed to dispatch event '" << eventName << "': " << lua_tostring(L, -1));
				lua_pop(L, stackRemaining);
			}
			else {
				LuaError("Internal error while dispatching event '" << eventName << "'");
			}

			return EventResult::Failed;
		}
	}

	void State::OnGameSessionLoading()
	{
		EmptyEvent params;
		ThrowEvent("SessionLoading", params, false, RestrictAll | ScopeSessionLoad);
	}

	void State::OnGameSessionLoaded()
	{
		EmptyEvent params;
		ThrowEvent("SessionLoaded", params, false, RestrictAll);
	}

	void State::OnModuleLoadStarted()
	{
		EmptyEvent params;
		ThrowEvent("ModuleLoadStarted", params, false, RestrictAll | ScopeModulePreLoad);
	}

	void State::OnModuleLoading()
	{
		EmptyEvent params;
		ThrowEvent("ModuleLoading", params, false, RestrictAll | ScopeModuleLoad);
	}

	void State::OnStatsLoaded()
	{
		EmptyEvent params;
		ThrowEvent("StatsLoaded", params, false, RestrictAll | ScopeModuleLoad);
	}

	void State::OnModuleResume()
	{
		EmptyEvent params;
		ThrowEvent("ModuleResume", params, false, RestrictAll | ScopeModuleResume);
	}

	void State::OnResetCompleted()
	{
		EmptyEvent params;
		ThrowEvent("ResetCompleted", params, false, 0);
	}

	void State::OnUpdate(GameTime const& time)
	{
		TickEvent params{ .Time = time };
		ThrowEvent("Tick", params, false, 0);

		lua_gc(L, LUA_GCSTEP, 10);
	}

	void State::OnStatsStructureLoaded()
	{
		EmptyEvent params;
		ThrowEvent("StatsStructureLoaded", params, false, 0);
	}

}
