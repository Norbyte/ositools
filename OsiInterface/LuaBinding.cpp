#include <stdafx.h>
#include <OsirisProxy.h>
#include <LuaBinding.h>
#include <fstream>


namespace osidbg
{
	static char const * const LuaOsiSignatureTag = "TLuaOsiSignature";

	LuaOsiSignature * LuaOsiSignature::TypeCheck(lua_State *L, int index)
	{
		luaL_checktype(L, index, LUA_TUSERDATA);
		auto sig = (LuaOsiSignature *)luaL_checkudata(L, index, LuaOsiSignatureTag);
		if (sig == nullptr) luaL_error(L, "Expected an OsiSignature");
		return sig;
	}

	LuaOsiSignature * LuaOsiSignature::New(lua_State *L)
	{
		auto sig = (LuaOsiSignature *)lua_newuserdata(L, sizeof(LuaOsiSignature));
		new (sig) LuaOsiSignature();
		luaL_setmetatable(L, LuaOsiSignatureTag);
		return sig;
	}

	void LuaOsiSignature::Register(lua_State* L)
	{
		lua_register(L, LuaOsiSignatureTag, nullptr);
		luaL_newmetatable(L, LuaOsiSignatureTag);
		lua_pop(L, 1);
	}


	OsiArgumentPool::OsiArgumentPool()
	{
		argumentPool_.resize(1024);
		usedArguments_ = 0;
	}

	OsiArgumentDesc * OsiArgumentPool::AllocateArguments(uint32_t num)
	{
		if (usedArguments_ + num > argumentPool_.size()) {
			throw std::runtime_error("Ran out of argument descriptors");
		}

		auto ptr = argumentPool_.data() + usedArguments_;
		for (uint32_t i = 0; i < num; i++) {
			new (ptr + i) OsiArgumentDesc();

			if (i > 0) {
				ptr[i - 1].NextParam = ptr + i;
			}
		}

		usedArguments_ += num;
		return ptr;
	}

	void OsiArgumentPool::ReleaseArguments(uint32_t num)
	{
		usedArguments_ -= num;
	}


	OsiArgumentListPin::OsiArgumentListPin(uint32_t numArgs)
		: numArgs_(numArgs)
	{
		args_ = ExtensionState::Get().OsiArgumentPool.AllocateArguments(numArgs_);
	}

	OsiArgumentListPin::~OsiArgumentListPin()
	{
		ExtensionState::Get().OsiArgumentPool.ReleaseArguments(numArgs_);
	}


	OsiProxyLibrary::OsiProxyLibrary()
	{
	}

	void OsiProxyLibrary::Register(lua_State * L)
	{
		RegisterLib(L);

		LuaOsiSignature::Register(L);
		CreateSignatureTable(L);
	}

	void OsiProxyLibrary::RegisterLib(lua_State * L)
	{
		static const luaL_Reg osiLib[] = {
			{"Call", Call},
			{"Query", Query},
			{"Event", Event},
			{0,0}
		};

		luaL_newlib(L, osiLib);
		lua_setglobal(L, "Osi");
	}

	void OsiProxyLibrary::CreateSignatureTable(lua_State * L)
	{
		lua_createtable(L, 0, 1024); /* creates and pushes new table on top of Lua stack */

		auto const & sigs = gOsirisProxy->GetCustomFunctionInjector().OsiSymbols();
		for (auto const & sig : sigs) {
			if (sig.EoCFunctionId != 0
				&& sig.params.size() <= 32) {
				auto luaSig = LuaOsiSignature::New(L);
				luaSig->name = sig.name;
				luaSig->functionHandle = sig.EoCFunctionId;
				luaSig->type = sig.type;
				luaSig->numInputArgs = 0;
				luaSig->numArgs = (uint32_t)sig.params.size();
				for (auto i = 0; i < sig.params.size(); i++) {
					luaSig->args[i] = sig.params[i];
					if (!luaSig->args[i].output) {
						luaSig->numInputArgs++;
					}
				}

				lua_setfield(L, -2, sig.name.c_str());  /* table["name"] = row->name. Pops key value */
			}
		}

		lua_setglobal(L, "Sig");
	}

	std::string OsiProxyLibrary::GenerateOsiHelpers()
	{
		std::stringstream ss;

		auto const & sigs = gOsirisProxy->GetCustomFunctionInjector().OsiSymbols();
		for (auto const & sig : sigs) {
			if (sig.EoCFunctionId != 0 && sig.params.size() <= 32) {
				ss << "function " << sig.name << "(...)\r\n";
				ss << "    return Osi.";
				switch (sig.type) {
				case FunctionType::Call: ss << "Call"; break;
				case FunctionType::Query: ss << "Query"; break;
				case FunctionType::Event: ss << "Event"; break;
				}

				ss << "(Sig." << sig.name << ", ...)\r\n";
				ss << "end\r\n\r\n";
			}
		}

		return ss.str();
	}

	void OsiProxyLibrary::LuaToOsiArg(lua_State * L, int i, OsiArgumentDesc & arg, OsiSymbolInfo::ParamInfo const & desc)
	{
		arg.Value.TypeId = desc.type;
		auto type = lua_type(L, i);
		switch (desc.type) {
		case ValueType::Integer:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			arg.Value.Int32 = (int32_t)lua_tointeger(L, i);
			break;

		case ValueType::Integer64:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			arg.Value.Int64 = (int64_t)lua_tointeger(L, i);
			break;

		case ValueType::Real:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			// TODO - set lua internal type to float?
			arg.Value.Float = (float)lua_tonumber(L, i);
			break;

		case ValueType::String:
		case ValueType::GuidString:
		case ValueType::CharacterGuid:
		case ValueType::ItemGuid: // TODO ...
			if (type != LUA_TSTRING) {
				luaL_error(L, "String expected for argument %d, got %s", i, lua_typename(L, type));
			}

			arg.Value.String = lua_tostring(L, i);
			if (arg.Value.String == nullptr) {
				luaL_error(L, "Could not cast argument %d to string", i);
			}
			break;

		default:
			throw std::runtime_error("Unhandled Osi argument type");
		}
	}

	void OsiProxyLibrary::OsiToLuaArg(lua_State * L, OsiArgumentDesc const & arg)
	{
		switch (arg.Value.TypeId) {
		case ValueType::Integer:
			lua_pushinteger(L, arg.Value.Int32);
			break;

		case ValueType::Integer64:
			lua_pushinteger(L, arg.Value.Int64);
			break;

		case ValueType::Real:
			lua_pushnumber(L, arg.Value.Float);
			break;

		case ValueType::String:
		case ValueType::GuidString:
		case ValueType::CharacterGuid:
		case ValueType::ItemGuid: // TODO ...
			lua_pushstring(L, arg.Value.String);
			break;

		default:
			throw std::runtime_error("Unhandled Osi argument type");
		}
	}

	int OsiProxyLibrary::Call(lua_State * L)
	{
		int numArgs = lua_gettop(L);
		if (numArgs < 1) {
			luaL_error(L, "Osiris call has no function data!");
		}

		auto signature = LuaOsiSignature::TypeCheck(L, 1);
		if (signature->type != FunctionType::Call) {
			luaL_error(L, "Expected a Call node");
		}

		if (numArgs - 1 != signature->numArgs) {
			luaL_error(L, "Incorrect number of arguments for call '%s'; expected %d, got %d", 
				signature->name.c_str(), signature->numArgs, numArgs - 1);
		}

		OsiArgumentListPin args(signature->numArgs);
		for (uint32_t i = 0; i < signature->numArgs; i++) {
			LuaToOsiArg(L, i + 2, args.Args()[i], signature->args[i]);
		}

		gOsirisProxy->GetWrappers().Call.CallWithHooks(signature->functionHandle, args.Args());
		return 0;
	}

	int OsiProxyLibrary::Event(lua_State * L)
	{
		int numArgs = lua_gettop(L);
		if (numArgs < 1) {
			luaL_error(L, "Osiris event has no function data!");
		}

		auto signature = LuaOsiSignature::TypeCheck(L, 1);
		if (signature->type != FunctionType::Event) {
			luaL_error(L, "Expected an Event node");
		}

		if (numArgs - 1 != signature->numArgs) {
			luaL_error(L, "Incorrect number of arguments for event '%s'; expected %d, got %d", 
				signature->name.c_str(), signature->numArgs, numArgs - 1);
		}

		OsiArgumentListPin args(signature->numArgs);
		for (uint32_t i = 0; i < signature->numArgs; i++) {
			LuaToOsiArg(L, i + 2, args.Args()[i], signature->args[i]);
		}

		auto osiris = gOsirisProxy->GetDynamicGlobals().OsirisObject;
		gOsirisProxy->GetWrappers().Event.CallWithHooks(osiris, signature->functionHandle, args.Args());
		return 0;
	}

	int OsiProxyLibrary::Query(lua_State * L)
	{
		int numArgs = lua_gettop(L);
		if (numArgs < 1) {
			luaL_error(L, "Osiris query has no function data!");
		}

		auto signature = LuaOsiSignature::TypeCheck(L, 1);
		if (signature->type != FunctionType::Query) {
			luaL_error(L, "Expected a Query node");
		}

		if (numArgs - 1 != signature->numInputArgs) {
			luaL_error(L, "Incorrect number of arguments for query '%s'; expected %d, got %d", 
				signature->name.c_str(), signature->numInputArgs, numArgs - 1);
		}

		OsiArgumentListPin args(signature->numArgs);
		uint32_t inputArg = 2;
		for (uint32_t i = 0; i < signature->numArgs; i++) {
			if (signature->args[i].output) {
				args.Args()[i].Value.TypeId = signature->args[i].type;
			} else {
				LuaToOsiArg(L, inputArg++, args.Args()[i], signature->args[i]);
			}
		}

		bool handled = gOsirisProxy->GetWrappers().Query.CallWithHooks(signature->functionHandle, args.Args());
		uint32_t numOutputs = signature->numArgs - signature->numInputArgs;
		if (handled) {
			for (uint32_t i = 0; i < signature->numArgs; i++) {
				if (signature->args[i].output) {
					OsiToLuaArg(L, args.Args()[i]);
					numOutputs++;
				}
			}
		} else {
			for (uint32_t i = 0; i < signature->numArgs - signature->numInputArgs; i++) {
				lua_pushnil(L);
			}
		}

		return signature->numArgs - signature->numInputArgs;
	}


	// Unsafe libs (IO, OS, package, debug) removed
	static const luaL_Reg loadedlibs[] = {
	  {"_G", luaopen_base},
	  {LUA_TABLIBNAME, luaopen_table},
	  {LUA_STRLIBNAME, luaopen_string},
	  {LUA_MATHLIBNAME, luaopen_math},
	  {NULL, NULL}
	};


	LuaState::LuaState()
	{
		state_ = luaL_newstate();
		OpenLibs();

		proxy_.Register(state_);
		auto helpers = proxy_.GenerateOsiHelpers();
		LoadScript(helpers, "bootstrapper");

		auto sandbox = R"(
dofile = function ()
	error("dofile() has been disabled for security reasons")
end

loadfile = function ()
	error("loadfile() has been disabled for security reasons")
end

load = function ()
	error("load() has been disabled for security reasons")
end

loadstring = function ()
	error("loadstring() has been disabled for security reasons")
end

_G = {})";
		LoadScript(sandbox, "sandbox");
	}

	LuaState::~LuaState()
	{
		lua_close(state_);
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
		/* Load the file containing the script we are going to run */
		int status = luaL_loadbufferx(state_, script.c_str(), script.size(), name.c_str(), "text");
		if (status != LUA_OK) {
			OsiError("Failed to parse script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1);  /* pop error message from the stack */
			return false;
		}

		/* Ask Lua to run our little script */
		status = lua_pcall(state_, 0, LUA_MULTRET, 0);
		if (status != LUA_OK) {
			OsiError("Failed to execute script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1); // pop error message from the stack
			return false;
		}

		return true;
	}

	void LuaState::Call(char const * func, char const * arg)
	{
		auto L = state_;

		// call a function defined in Lua
		// push functions and arguments
		lua_getglobal(L, func); // function to be called
		lua_pushstring(L, arg); // push 1st argument */

		// do the call (1 arguments, 0 result)
		auto status = lua_pcall(L, 1, 0, 0);
		if (status != LUA_OK) {
			OsiError("Failed to call function '" << func << "': " << lua_tostring(L, -1));
			lua_pop(L, 1); // pop error message from the stack
			return;
		}
		/*
		// retrieve result
		if (!lua_isnumber(L, -1))
			error(L, "function `f' must return a number");
		z = lua_tonumber(L, -1);
		lua_pop(L, 1); // pop returned value
		*/
	}


	void ExtensionState::LuaReset()
	{
		Lua = std::make_unique<LuaState>();
		OsiWarn("LUA VM reset.");
	}

	void ExtensionState::LuaStartup()
	{
		auto modManager = GetModManager();
		if (modManager == nullptr) {
			OsiError("Could not bootstrap Lua modules - mod manager not available");
			return;
		}

		auto & mods = modManager->BaseModule.LoadOrderedModules.Set;

		OsiWarn("Bootstrapping Lua modules ...");
		for (auto i = 0; i < mods.Size; i++) {
			auto const & mod = mods.Buf[i];
			auto dir = ToUTF8(mod.Info.Directory.GetPtr());
			auto bootstrapFile = "Mods/" + dir + "/Story/RawFiles/Lua/Bootstrap.lua";
			auto reader = gOsirisProxy->GetLibraryManager().MakeFileReader(bootstrapFile);
			if (reader != nullptr && reader->IsLoaded) {
				OsiWarn("Found bootstrap file: " << bootstrapFile);
				LuaLoadGameFile(reader);
			}
		}
	}

	void ExtensionState::LuaLoadExternalFile(std::string const & path)
	{
		if (!Lua) {
			OsiError("Called when the Lua VM has not been initialized!");
			return;
		}

		std::ifstream f(path, std::ios::in | std::ios::binary);
		if (!f.good()) {
			OsiError("File does not exist: " << path);
			return;
		}

		f.seekg(0, std::ios::end);
		auto length = f.tellg();
		f.seekg(0, std::ios::beg);
		std::string s(length, '\0');
		f.read(const_cast<char *>(s.data()), length);
		f.close();

		Lua->LoadScript(s);
		OsiWarn("Loaded external script: " << path);
	}

	void ExtensionState::LuaLoadGameFile(FileReader * reader)
	{
		if (!Lua) {
			OsiError("Called when the Lua VM has not been initialized!");
			return;
		}

		if (reader == nullptr || !reader->IsLoaded) {
			OsiError("Attempted to load script from invalid file reader");
			return;
		}

		std::string script;
		script.resize(reader->FileSize);
		memcpy(script.data(), reader->ScratchBufPtr, reader->FileSize);

		Lua->LoadScript(script);
	}

	void ExtensionState::LuaLoadGameFile(std::string const & path)
	{
		if (!Lua) {
			OsiError("Called when the Lua VM has not been initialized!");
			return;
		}

		auto reader = gOsirisProxy->GetLibraryManager().MakeFileReader(path);
		if (reader == nullptr || !reader->IsLoaded) {
			OsiError("Script file could not be opened: " << path);
			return;
		}

		LuaLoadGameFile(reader);
		OsiWarn("Loaded game script: " << path);
	}

	void ExtensionState::LuaCall(char const * func, char const * arg)
	{
		if (!Lua) {
			OsiError("Called when the Lua VM has not been initialized!");
			return;
		}

		Lua->Call(func, arg);
	}
}
