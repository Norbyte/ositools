#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>Debug</lua_module>
BEGIN_NS(lua::debug)

void DebugDumpLifetimes(lua_State* L)
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
}

void DumpNetworking()
{
	auto server = (*GetStaticSymbols().esv__EoCServer)->GameServer;
	auto loadProto = (esv::LoadProtocol*)server->Protocols[4];

	INFO(" === NETWORK DUMP === ");
	INFO("LoadProtocol state: %d", loadProto->State);
	INFO("Received LEVEL_START: %d", loadProto->NumLevelStartPlayers);
	INFO("Received SWAP_READY: %d", loadProto->NumSwapReadyPeers);

	INFO("ModuleLoaded responses:");
	for (auto const& load : loadProto->OS_ModuleLoaded) {
		INFO("    Peer %d", load.PeerId);
	}

	INFO("SessionLoaded responses:");
	for (auto const& load : loadProto->OS_SessionLoaded) {
		INFO("    Peer %d (%s)", load.PeerId, load.field_8.GetString());
	}

	INFO("LevelLoaded responses:");
	for (auto const& load : loadProto->OS_LevelLoaded) {
		INFO("    Peer %d (%s)", load.PeerId, load.field_8.GetString());
	}

	INFO("FileTransferOutbox items:");
	for (auto const& item : loadProto->OS_FileTransferOutbox) {
		INFO("    Peer %d, user %d", item.PeerId, item.UserId_M);
		for (auto const& path : item.OS_FixedString) {
			INFO("        FS %s", path.GetString());
		}
		for (auto const& path : item.OS_Path) {
			INFO("        Path %s", path.Name.c_str());
		}
	}
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

STDString DoGenerateIdeHelpers(bool builtinOnly)
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

void GenerateIdeHelpers(lua_State* L, std::optional<bool> builtinOnly)
{
#if defined(OSI_EOCAPP)
	if (gExtender->GetConfig().DeveloperMode) {
#endif
		auto lua = State::FromLua(L);
		if ((lua->RestrictionFlags & State::RestrictOsiris) || !gExtender->GetServer().IsInServerThread()) {
			luaL_error(L, "GenerateIdeHelpers() can only be called when Osiris is available");
		}

		auto helpers = DoGenerateIdeHelpers(builtinOnly && *builtinOnly);

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

bool IsDeveloperMode()
{
	return gExtender->GetConfig().DeveloperMode;
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

// Development-only function for testing crash reporting
void Crash(int type)
{
#if defined(_DEBUG)
	switch (type) {
	case 0:
	{
		int d = 0;
		std::cout << (1 / d);
		break;
	}

	case 1:
		abort();
		break;

	case 2:
		*(uint32_t*)0 = 0;
		break;
	}
#endif
}

void RegisterDebugLib()
{
	DECLARE_MODULE(Debug, Both)
	BEGIN_MODULE()
	MODULE_FUNCTION(DumpStack)
	MODULE_FUNCTION(DumpNetworking)
	MODULE_FUNCTION(DebugDumpLifetimes)
	MODULE_FUNCTION(GenerateIdeHelpers)
	MODULE_NAMED_FUNCTION("DebugBreak", LuaDebugBreak)
	MODULE_FUNCTION(IsDeveloperMode)
	MODULE_FUNCTION(Crash)
	END_MODULE()
}

END_NS()
