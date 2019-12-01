#include <stdafx.h>
#include <OsirisProxy.h>
#include <LuaBinding.h>
#include <fstream>
#include <regex>


namespace osidbg
{
	void LuaToOsi(lua_State * L, int i, TypedValue & tv, ValueType osiType)
	{
		tv.VMT = gOsirisProxy->GetGlobals().TypedValueVMT;
		tv.TypeId = (uint32_t)osiType;

		auto type = lua_type(L, i);
		switch (osiType) {
		case ValueType::Integer:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			tv.Value.Val.Int32 = (int32_t)lua_tointeger(L, i);
			break;

		case ValueType::Integer64:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			tv.Value.Val.Int64 = (int64_t)lua_tointeger(L, i);
			break;

		case ValueType::Real:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			// TODO - set lua internal type to float?
			tv.Value.Val.Float = (float)lua_tonumber(L, i);
			break;

		case ValueType::String:
		case ValueType::GuidString:
		case ValueType::CharacterGuid:
		case ValueType::ItemGuid: // TODO ...
			if (type != LUA_TSTRING) {
				luaL_error(L, "String expected for argument %d, got %s", i, lua_typename(L, type));
			}

			// TODO - not sure if we're the owners of the string or the TypedValue is
			tv.Value.Val.String = _strdup(lua_tostring(L, i));
			if (tv.Value.Val.String == nullptr) {
				luaL_error(L, "Could not cast argument %d to string", i);
			}
			break;

		default:
			luaL_error(L, "Unhandled Osi argument type %d", osiType);
			break;
		}
	}

	TypedValue * LuaToOsi(lua_State * L, int i, ValueType osiType)
	{
		auto tv = new TypedValue();
		LuaToOsi(L, i, *tv, osiType);
		return tv;
	}

	void LuaToOsi(lua_State * L, int i, OsiArgumentValue & arg, ValueType osiType)
	{
		arg.TypeId = osiType;
		auto type = lua_type(L, i);
		switch (osiType) {
		case ValueType::Integer:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			arg.Int32 = (int32_t)lua_tointeger(L, i);
			break;

		case ValueType::Integer64:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			arg.Int64 = (int64_t)lua_tointeger(L, i);
			break;

		case ValueType::Real:
			if (type != LUA_TNUMBER) {
				luaL_error(L, "Number expected for argument %d, got %s", i, lua_typename(L, type));
			}

			// TODO - set lua internal type to float?
			arg.Float = (float)lua_tonumber(L, i);
			break;

		case ValueType::String:
		case ValueType::GuidString:
		case ValueType::CharacterGuid:
		case ValueType::ItemGuid: // TODO ...
			if (type != LUA_TSTRING) {
				luaL_error(L, "String expected for argument %d, got %s", i, lua_typename(L, type));
			}

			arg.String = lua_tostring(L, i);
			if (arg.String == nullptr) {
				luaL_error(L, "Could not cast argument %d to string", i);
			}
			break;

		default:
			luaL_error(L, "Unhandled Osi argument type %d", osiType);
			break;
		}
	}

	void OsiToLua(lua_State * L, OsiArgumentValue const & arg)
	{
		switch (arg.TypeId) {
		case ValueType::Integer:
			lua_pushinteger(L, arg.Int32);
			break;

		case ValueType::Integer64:
			lua_pushinteger(L, arg.Int64);
			break;

		case ValueType::Real:
			lua_pushnumber(L, arg.Float);
			break;

		case ValueType::String:
		case ValueType::GuidString:
		case ValueType::CharacterGuid:
		case ValueType::ItemGuid: // TODO ...
			lua_pushstring(L, arg.String);
			break;

		default:
			luaL_error(L, "Unhandled Osi argument type %d", arg.TypeId);
			break;
		}
	}

	void OsiToLua(lua_State * L, TypedValue const & tv)
	{
		switch ((ValueType)tv.TypeId) {
		case ValueType::Integer:
			lua_pushinteger(L, tv.Value.Val.Int32);
			break;

		case ValueType::Integer64:
			lua_pushinteger(L, tv.Value.Val.Int64);
			break;

		case ValueType::Real:
			lua_pushnumber(L, tv.Value.Val.Float);
			break;

		case ValueType::String:
		case ValueType::GuidString:
		case ValueType::CharacterGuid:
		case ValueType::ItemGuid: // TODO ...
			lua_pushstring(L, tv.Value.Val.String);
			break;

		default:
			luaL_error(L, "Unhandled Osi TypedValue type %d", tv.TypeId);
			break;
		}
	}


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


	bool LuaOsiFunction::Bind(Function const * func, LuaState & state)
	{
		if (func->Type == FunctionType::Query
			|| func->Type == FunctionType::SysQuery
			|| func->Type == FunctionType::UserQuery) {

			auto numArgs = func->Signature->Params->Params.Size;
			auto adapter = state.GetIdentityAdapterMap().FindAdapter((uint8_t)numArgs);
			if (adapter == nullptr) {
				OsiError("Couldn't bind query '" << func->Signature->Name 
					<< "': No identity adapter found for arity " << numArgs);
				return false;
			}

			adapter_.Manager = *gOsirisProxy->GetGlobals().Adapters;
			adapter_.Id = adapter->Id;
		}

		function_ = func;
		state_ = &state;
		return true;
	}

	void LuaOsiFunction::Unbind()
	{
		function_ = nullptr;
	}

	int LuaOsiFunction::LuaCall(lua_State * L)
	{
		if (function_ == nullptr) {
			luaL_error(L, "Attempted to call an unbound Osiris function");
		}

		int numArgs = lua_gettop(L);
		if (numArgs < 1) {
			luaL_error(L, "Called Osi function without 'self' argument?");
		}

		switch (function_->Type) {
		case FunctionType::Call:
			OsiCall(L);
			return 0;

		case FunctionType::Event:
		case FunctionType::Database:
		case FunctionType::Proc:
			OsiInsert(L, false);
			return 0;

		case FunctionType::Query:
			return OsiQuery(L);

		case FunctionType::SysQuery:
		case FunctionType::UserQuery:
			return OsiUserQuery(L);

		case FunctionType::SysCall:
		default:
			luaL_error(L, "Cannot call function of type %d", function_->Type);
			return 0;
		}
	}

	void LuaOsiFunction::OsiCall(lua_State * L)
	{
		auto funcArgs = function_->Signature->Params->Params.Size;
		int numArgs = lua_gettop(L);
		if (numArgs - 1 != funcArgs) {
			luaL_error(L, "Incorrect number of arguments for '%s'; expected %d, got %d",
				function_->Signature->Name, funcArgs, numArgs - 1);
		}

		OsiArgumentListPin<OsiArgumentDesc> args(state_->GetArgumentDescPool(), (uint32_t)funcArgs);
		auto argType = function_->Signature->Params->Params.Head->Next;
		for (uint32_t i = 0; i < funcArgs; i++) {
			auto arg = args.Args() + i;
			if (i > 0) {
				args.Args()[i - 1].NextParam = arg;
			}
			LuaToOsi(L, i + 2, arg->Value, (ValueType)argType->Item.Type);
			argType = argType->Next;
		}

		gOsirisProxy->GetWrappers().Call.CallWithHooks(function_->GetHandle(), args.Args());
	}

	void LuaOsiFunction::OsiInsert(lua_State * L, bool deleteTuple)
	{
		auto funcArgs = function_->Signature->Params->Params.Size;
		int numArgs = lua_gettop(L);
		if (numArgs - 1 != funcArgs) {
			luaL_error(L, "Incorrect number of arguments for '%s'; expected %d, got %d",
				function_->Signature->Name, funcArgs, numArgs - 1);
		}

		if (gGameType == GameType::Unknown) {
			luaL_error(L, "Story not loaded yet");
		}

		if (function_->Node.Id == 0) {
			luaL_error(L, "Function has no node");
		}

		OsiArgumentListPin<TypedValue> tvs(state_->GetTypedValuePool(), (uint32_t)funcArgs);
		OsiArgumentListPin<ListNode<TypedValue *>> nodes(state_->GetTypedValueNodePool(), (uint32_t)funcArgs + 1);

		TuplePtrLL tuple;
		auto & args = tuple.Items();
		auto argType = function_->Signature->Params->Params.Head->Next;
		args.Init(nodes.Args());

		auto prev = args.Head;
		for (uint32_t i = 0; i < funcArgs; i++) {
			auto tv = tvs.Args() + i;
			LuaToOsi(L, i + 2, *tv, (ValueType)argType->Item.Type);
			auto node = nodes.Args() + i + 1;
			args.Insert(tv, node, prev);
			prev = node;
			argType = argType->Next;
		}

		auto node = (*gOsirisProxy->GetGlobals().Nodes)->Db.Start[function_->Node.Id - 1];
		if (deleteTuple) {
			node->DeleteTuple(&tuple);
		} else {
			node->InsertTuple(&tuple);
		}
	}

	int LuaOsiFunction::OsiQuery(lua_State * L)
	{
		auto outParams = function_->Signature->OutParamList.numOutParams();
		auto numParams = function_->Signature->Params->Params.Size;
		auto inParams = numParams - outParams;

		int numArgs = lua_gettop(L);
		if (numArgs - 1 != inParams) {
			luaL_error(L, "Incorrect number of IN arguments for '%s'; expected %d, got %d",
				function_->Signature->Name, inParams, numArgs - 1);
		}

		OsiArgumentListPin<OsiArgumentDesc> args(state_->GetArgumentDescPool(), (uint32_t)numParams);
		auto argType = function_->Signature->Params->Params.Head->Next;
		uint32_t inputArg = 2;
		for (uint32_t i = 0; i < numParams; i++) {
			auto arg = args.Args() + i;
			if (i > 0) {
				args.Args()[i - 1].NextParam = arg;
			}

			if (function_->Signature->OutParamList.isOutParam(i)) {
				arg->Value.TypeId = (ValueType)argType->Item.Type;
			} else {
				LuaToOsi(L, inputArg++, arg->Value, (ValueType)argType->Item.Type);
			}

			argType = argType->Next;
		}

		bool handled = gOsirisProxy->GetWrappers().Query.CallWithHooks(function_->GetHandle(), args.Args());
		if (handled) {
			for (uint32_t i = 0; i < numParams; i++) {
				if (function_->Signature->OutParamList.isOutParam(i)) {
					OsiToLua(L, args.Args()[i].Value);
				}
			}
		} else {
			for (uint32_t i = 0; i < outParams; i++) {
				lua_pushnil(L);
			}
		}

		return outParams;
	}

	int LuaOsiFunction::OsiUserQuery(lua_State * L)
	{
		auto outParams = function_->Signature->OutParamList.numOutParams();
		auto numParams = function_->Signature->Params->Params.Size;
		auto inParams = numParams - outParams;

		int numArgs = lua_gettop(L);
		if (numArgs - 1 != inParams) {
			luaL_error(L, "Incorrect number of IN arguments for '%s'; expected %d, got %d",
				function_->Signature->Name, inParams, numArgs - 1);
		}

		OsiArgumentListPin<ListNode<TupleLL::Item>> nodes(state_->GetTupleNodePool(), (uint32_t)numParams + 1);

		VirtTupleLL tuple;
		
		auto & args = tuple.Data.Items;
		auto argType = function_->Signature->Params->Params.Head->Next;
		args.Init(nodes.Args());

		auto prev = args.Head;
		uint32_t inputArgIndex = 0;
		for (uint32_t i = 0; i < numParams; i++) {
			auto node = nodes.Args() + i + 1;
			args.Insert(node, prev);
			node->Item.Index = i;
			if (!function_->Signature->OutParamList.isOutParam(i)) {
				LuaToOsi(L, inputArgIndex + 2, node->Item.Value, (ValueType)argType->Item.Type);
				inputArgIndex++;
			} else {
				node->Item.Value.VMT = gOsirisProxy->GetGlobals().TypedValueVMT;
				node->Item.Value.TypeId = (uint32_t)ValueType::None;
			}

			prev = node;
			argType = argType->Next;
		}

		auto node = (*gOsirisProxy->GetGlobals().Nodes)->Db.Start[function_->Node.Id - 1];
		bool valid = node->IsValid(&tuple, &adapter_);
		if (valid) {
			if (outParams > 0) {
				auto retType = function_->Signature->Params->Params.Head->Next;
				auto ret = args.Head->Next;
				for (uint32_t i = 0; i < numParams; i++) {
					if (function_->Signature->OutParamList.isOutParam(i)) {
						OsiToLua(L, ret->Item.Value);
					}

					ret = ret->Next;
					retType = retType->Next;
				}

				return outParams;
			} else {
				lua_pushboolean(L, 1);
				return 1;
			}
		} else {
			if (outParams > 0) {
				for (uint32_t i = 0; i < outParams; i++) {
					lua_pushnil(L);
				}

				return outParams;
			} else {
				lua_pushboolean(L, 0);
				return 1;
			}
		}
	}



	char const * const LuaOsiFunctionNameProxy::MetatableName = "LuaOsiFunctionNameProxy";

	LuaOsiFunctionNameProxy::LuaOsiFunctionNameProxy(std::string const & name, LuaState & state)
		: name_(name), state_(state), generationId_(state_.GenerationId())
	{}

	void LuaOsiFunctionNameProxy::UnbindAll()
	{
		functions_.clear();
	}

	int LuaOsiFunctionNameProxy::LuaCall(lua_State * L)
	{
		if (generationId_ != state_.GenerationId()) {
			// Clear cached functions if story was reloaded
			UnbindAll();
			generationId_ = state_.GenerationId();
		}

		auto arity = (uint32_t)lua_gettop(L) - 1;

		auto func = TryGetFunction(arity);
		if (func == nullptr) {
			luaL_error(L, "No function named '%s' exists that can be called with %d parameters.",
				name_.c_str(), arity);
		}

		return func->LuaCall(L);
	}

	LuaOsiFunction * LuaOsiFunctionNameProxy::TryGetFunction(uint32_t arity)
	{
		if (functions_.size() > arity
			&& functions_[arity].IsBound()) {
			return &functions_[arity];
		}

		// Look for Call/Proc/Event/Query (number of OUT args == 0)
		auto func = LookupOsiFunction(arity);
		if (func != nullptr && func->Signature->OutParamList.numOutParams() == 0) {
			return CreateFunctionMapping(arity, func);
		}

		for (uint32_t args = arity + 1; args < arity + MaxQueryOutParams; args++) {
			// Look for Query/UserQuery (number of OUT args > 0)
			auto func = LookupOsiFunction(args);
			if (func != nullptr) {
				auto outParams = func->Signature->OutParamList.numOutParams();
				auto params = func->Signature->Params->Params.Size - outParams;
				if (params == arity) {
					return CreateFunctionMapping(arity, func);
				}
			}
		}

		return nullptr;
	}

	LuaOsiFunction * LuaOsiFunctionNameProxy::CreateFunctionMapping(uint32_t arity, Function const * func)
	{
		if (functions_.size() <= arity) {
			functions_.resize(arity + 1);
		}

		if (functions_[arity].Bind(func, state_)) {
			return &functions_[arity];
		} else {
			return nullptr;
		}
	}

	uint32_t FunctionNameHash(char const * str)
	{
		uint32_t hash{ 0 };
		while (*str) {
			hash = (*str++ | 0x20) + 129 * (hash % 4294967);
		}

		return hash;
	}

	Function const * LuaOsiFunctionNameProxy::LookupOsiFunction(uint32_t arity)
	{
		auto functions = gOsirisProxy->GetGlobals().Functions;

		std::string sig = name_;
		sig += "/";
		sig += std::to_string(arity);
		STDString osiSig;
		osiSig.Set(sig);

		auto hash = FunctionNameHash(name_.c_str()) + arity;
		auto func = (*functions)->Find(hash, osiSig);
		if (func == nullptr 
			|| ((*func)->Node.Id == 0 
				&& (*func)->Type != FunctionType::Call
				&& (*func)->Type != FunctionType::Query)) {
			return nullptr;
		}

		return *func;
	}


	char const * const OsiProxyLibrary::NameResolverMetatableName = "OsiProxyNameResolver";


	OsiProxyLibrary::OsiProxyLibrary()
	{
	}

	void OsiProxyLibrary::RegisterNameResolverMetatable(lua_State * L)
	{
		lua_register(L, NameResolverMetatableName, nullptr);
		luaL_newmetatable(L, NameResolverMetatableName); // stack: mt
		lua_pushcfunction(L, &LuaIndexResolverTable); // stack: mt, &LuaIndexResolverTable
		lua_setfield(L, -2, "__index"); // mt.__index = &LuaIndexResolverTable; stack: mt
		lua_pop(L, 1); // stack: mt
	}

	void OsiProxyLibrary::CreateNameResolver(lua_State * L)
	{
		lua_newtable(L); // stack: osi
		luaL_setmetatable(L, NameResolverMetatableName); // stack: osi
		lua_setglobal(L, "Osi"); // stack: -
	}

	int OsiProxyLibrary::LuaIndexResolverTable(lua_State * L)
	{
		luaL_checktype(L, 1, LUA_TTABLE);
		auto name = luaL_checkstring(L, 2);

		LuaStatePin lua(ExtensionState::Get());
		LuaOsiFunctionNameProxy::New(L, name, std::ref(lua.Get())); // stack: tab, name, proxy

		lua_pushvalue(L, 1); // stack: fun, tab
		lua_pushstring(L, name); // stack: fun, tab, name
		lua_pushvalue(L, -3); // stack: fun, tab, name, fun
		lua_settable(L, -3); // stack: fun
		lua_pop(L, 1);
		return 1;
	}

	void OsiProxyLibrary::Register(lua_State * L)
	{
		RegisterLib(L);
		LuaOsiFunctionNameProxy::RegisterMetatable(L);
		RegisterNameResolverMetatable(L);
		CreateNameResolver(L);
	}

	void OsiProxyLibrary::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"Require", Require},
			{"NewCall", NewCall},
			{"NewQuery", NewQuery},
			{"NewEvent", NewEvent},
			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -
	}

	std::string OsiProxyLibrary::GenerateOsiHelpers()
	{
		std::stringstream ss;

		auto const & sigs = gOsirisProxy->GetCustomFunctionInjector().OsiSymbols();
		for (auto const & sig : sigs) {
			if (sig.EoCFunctionId != 0 && sig.params.size() <= 16) {
				ss << sig.name << " = Osi." << sig.name << "\r\n";
			}
		}

		return ss.str();
	}

	ValueType StringToValueType(std::string const & s)
	{
		if (s == "INTEGER") {
			return ValueType::Integer;
		} else if (s == "INTEGER") {
			return ValueType::Integer;
		} else if (s == "INTEGER64") {
			return ValueType::Integer64;
		} else if (s == "REAL") {
			return ValueType::Real;
		} else if (s == "STRING") {
			return ValueType::String;
		} else if (s == "GUIDSTRING") {
			return ValueType::GuidString;
		} else if (s == "CHARACTERGUID") {
			return ValueType::CharacterGuid;
		} else if (s == "ITEMGUID") {
			return ValueType::ItemGuid;
		} else {
			OsiError("Unknown Osiris value type: " << s);
			return ValueType::None;
		}
	}


	bool CustomLuaCall::Call(OsiArgumentDesc const & params)
	{
		if (!ValidateArgs(params)) {
			return false;
		}

		LuaStatePin lua(ExtensionState::Get());
		if (!lua) {
			OsiError("Call failed: Lua state not initialized");
			return false;
		}

		auto L = lua->State();
		handler_.Push();

		auto param = &params;
		int numParams{ 0 };
		while (param != nullptr) {
			OsiToLua(L, param->Value);
			numParams++;
			param = param->NextParam;
		}

		if (lua_pcall(L, numParams, 0, 0) != 0) {
			OsiError("Handler for '" << Name() << "' failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		return true;
	}


	bool LuaState::Query(std::string const & name, LuaRegistryEntry * func,
		std::vector<CustomFunctionParam> const & signature, OsiArgumentDesc & params)
	{
		auto L = State();
		auto stackSize = lua_gettop(L);

		try {
			return QueryInternal(name, func, signature, params);
		} catch (...) {
			auto stackRemaining = lua_gettop(L) - stackSize;
			if (stackRemaining > 0) {
				OsiError("Query '" << name << "' failed: " << lua_tostring(L, -1));
				lua_pop(L, stackRemaining);
			} else {
				OsiError("Internal error during query '" << name << "'");
			}

			return false;
		}
	}


	bool LuaState::QueryInternal(std::string const & name, LuaRegistryEntry * func,
		std::vector<CustomFunctionParam> const & signature, OsiArgumentDesc & params)
	{
		auto L = State();
		auto stackSize = lua_gettop(L);
		if (func) {
			func->Push();
		} else {
			lua_getglobal(L, name.c_str());
		}

		int numParams{ 0 };
		int numOutParams{ 0 };

		auto param = &params;
		int paramIndex{ 0 };
		while (param != nullptr) {
			if (signature[paramIndex].Dir == FunctionArgumentDirection::In) {
				OsiToLua(L, param->Value);
				numParams++;
			} else {
				numOutParams++;
			}

			param = param->NextParam;
			paramIndex++;
		}

		if (lua_pcall(L, numParams, LUA_MULTRET, 0) != 0) {
			OsiError("Handler for '" << name << "' failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		auto numReturnValues = lua_gettop(L) - stackSize;
		if (numReturnValues == 0) {
			// No values returned, assume that the query failed
			return false;
		} else if (numOutParams == 0) {
			// Query has no OUT parameters; it should return "true" or "false".
			if (numReturnValues == 1) {
				auto retType = lua_type(L, -1);
				if (retType != LUA_TBOOLEAN) {
					OsiError("Handler for '" << name << "' returned type " << retType << "; expected boolean");
					return false;
				}

				auto ok = lua_toboolean(L, -1);
				lua_pop(L, numReturnValues);
				return ok ? true : false;
			} else {
				lua_pop(L, numReturnValues);
				OsiError("Handler for '" << name << "' returned " << numReturnValues << " values; expected 1 bool");
				return false;
			}
		} else if (numOutParams != numReturnValues) {
			lua_pop(L, numReturnValues);
			// Query has OUT parameters
			// Number of returned values should match the number of OUT params in the query
			OsiError("Handler for '" << name << "' returned " << numReturnValues << " values; expected " << numOutParams);
			return false;
		} else {
			// Lua call returned correct number of OUT parameters

			param = &params;
			paramIndex = 0;
			int stackIndex{ -numOutParams };
			int numNulls{ 0 };
			while (param != nullptr) {
				if (signature[paramIndex].Dir == FunctionArgumentDirection::Out) {
					if (lua_isnil(L, stackIndex)) {
						numNulls++;
					} else {
						LuaToOsi(L, stackIndex, param->Value, signature[paramIndex].Type);
					}

					numParams++;
					stackIndex++;
				}

				param = param->NextParam;
				paramIndex++;
			}

			lua_pop(L, numReturnValues);

			if (numNulls > 0 && numNulls != numOutParams) {
				OsiError("Handler for '" << name << "' returned " << numNulls <<
					" nil values; every return value must be nil or non-nil");
				return false;
			} else if (numNulls == numOutParams) {
				return false;
			} else {
				return true;
			}
		}

		return true;
	}


	bool CustomLuaQuery::Query(OsiArgumentDesc & params)
	{
		if (!ValidateArgs(params)) {
			return false;
		}

		LuaStatePin lua(ExtensionState::Get());
		if (!lua) {
			OsiError("Call failed: Lua state not initialized");
			return false;
		}

		return lua->Query(Name(), &handler_, Params(), params);
	}


	int OsiProxyLibrary::Require(lua_State * L)
	{
		auto modGuid = luaL_checkstring(L, 1);
		auto fileName = luaL_checkstring(L, 2);
		ExtensionState::Get().LuaLoadGameFile(modGuid, fileName);
		return 0;
	}

	const std::regex inOutParamRe("^\\s*(\\[(in|out)\\])?\\(([A-Z0-9]+)\\)(_[a-zA-Z0-9]+)\\s*$");
	const std::regex inParamRe("^\\s*\\(([A-Z0-9]+)\\)(_[a-zA-Z0-9]+)\\s*$");

	CustomFunctionParam ParseCustomFunctionParam(lua_State * L, std::string const & param, bool isQuery)
	{
		CustomFunctionParam parsed;

		std::smatch paramMatch;
		if (!std::regex_match(param, paramMatch, isQuery ? inOutParamRe : inParamRe)) {
			luaL_error(L, "Parameter string malformed: %s", param.c_str());
		}

		if (isQuery && paramMatch[2].matched) {
			auto dir = paramMatch[2].str();
			if (dir == "in") {
				parsed.Dir = FunctionArgumentDirection::In;
			} else if (dir == "out") {
				parsed.Dir = FunctionArgumentDirection::Out;
			} else {
				luaL_error(L, "Invalid parameter direction: %s", dir.c_str());
			}
		} else {
			parsed.Dir = FunctionArgumentDirection::In;
		}

		auto type = paramMatch[isQuery ? 3 : 1].str();
		parsed.Type = StringToValueType(type);
		if (parsed.Type == ValueType::None) {
			luaL_error(L, "Unsupported parameter type: %s", type.c_str());
		}

		parsed.Name = paramMatch[isQuery ? 4 : 2].str().substr(1);
		return parsed;
	}

	void ParseCustomFunctionParams(lua_State * L, char const * s, 
		std::vector<CustomFunctionParam> & params, bool isQuery)
	{
		std::string param;
		std::istringstream paramStream(s);

		while (std::getline(paramStream, param, ',')) {
			auto parsedParam = ParseCustomFunctionParam(L, param, isQuery);
			params.push_back(parsedParam);
		}
	}

	int OsiProxyLibrary::NewCall(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (!lua) luaL_error(L, "Exiting");

		if (lua->StartupDone()) luaL_error(L, "Attempted to register call after Lua startup phase");

		luaL_checktype(L, 1, LUA_TFUNCTION);
		auto funcName = luaL_checkstring(L, 2);
		auto args = luaL_checkstring(L, 3);

		std::vector<CustomFunctionParam> argList;
		ParseCustomFunctionParams(L, args, argList, false);

		LuaRegistryEntry func(L, 1);
		auto call = std::make_unique<CustomLuaCall>(funcName, argList, std::move(func));

		auto & functionMgr = gOsirisProxy->GetCustomFunctionManager();
		functionMgr.RegisterDynamic(std::move(call));
		
		return 0;
	}

	int OsiProxyLibrary::NewQuery(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (!lua) luaL_error(L, "Exiting");

		if (lua->StartupDone()) luaL_error(L, "Attempted to register query after Lua startup phase");

		luaL_checktype(L, 1, LUA_TFUNCTION);
		auto funcName = luaL_checkstring(L, 2);
		auto args = luaL_checkstring(L, 3);

		std::vector<CustomFunctionParam> argList;
		ParseCustomFunctionParams(L, args, argList, true);

		LuaRegistryEntry func(L, 1);
		auto query = std::make_unique<CustomLuaQuery>(funcName, argList, std::move(func));

		auto & functionMgr = gOsirisProxy->GetCustomFunctionManager();
		functionMgr.RegisterDynamic(std::move(query));

		return 0;
	}

	int OsiProxyLibrary::NewEvent(lua_State * L)
	{
		LuaStatePin lua(ExtensionState::Get());
		if (!lua) luaL_error(L, "Exiting");

		if (lua->StartupDone()) luaL_error(L, "Attempted to register event after Lua startup phase");

		auto funcName = luaL_checkstring(L, 1);
		auto args = luaL_checkstring(L, 2);

		std::vector<CustomFunctionParam> argList;
		ParseCustomFunctionParams(L, args, argList, false);

		auto customEvt = std::make_unique<CustomEvent>(funcName, argList);

		auto & functionMgr = gOsirisProxy->GetCustomFunctionManager();
		functionMgr.RegisterDynamic(std::move(customEvt));

		return 0;
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
		: identityAdapters_(gOsirisProxy->GetGlobals())
	{
		state_ = luaL_newstate();
		OpenLibs();

		identityAdapters_.UpdateAdapters();

		proxy_.Register(state_);

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

rawget = function ()
	error("rawget() has been disabled for security reasons")
end

rawset = function ()
	error("rawset() has been disabled for security reasons")
end

rawequal = function ()
	error("rawequal() has been disabled for security reasons")
end

debug = nil
)";
		LoadScript(sandbox, "sandbox");
	}

	LuaState::~LuaState()
	{
		if (gOsirisProxy) {
			gOsirisProxy->GetCustomFunctionManager().ClearDynamicEntries();
		}

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
		status = lua_pcall(state_, 0, LUA_MULTRET, 0);
		if (status != LUA_OK) {
			OsiError("Failed to execute script: " << lua_tostring(state_, -1));
			lua_pop(state_, 1); // pop error message from the stack
			return false;
		}

		return true;
	}

	void LuaState::StoryLoaded()
	{
		generationId_++;
		identityAdapters_.UpdateAdapters();
		if (!identityAdapters_.HasAllAdapters()) {
			OsiWarn("Not all identity adapters are available - some queries may not work!");
		}
	}

	void LuaState::StoryFunctionMappingsUpdated()
	{
		auto helpers = proxy_.GenerateOsiHelpers();
		LoadScript(helpers, "bootstrapper");
	}

	void ExtensionState::StoryLoaded()
	{
		if (Lua) {
			Lua->StoryLoaded();
		}
	}

	void ExtensionState::StoryFunctionMappingsUpdated()
	{
		if (Lua) {
			Lua->StoryFunctionMappingsUpdated();
		}
	}


	void ExtensionState::IncLuaRefs()
	{
		assert(Lua);
		LuaRefs++;
	}

	void ExtensionState::DecLuaRefs()
	{
		assert(LuaRefs > 0);
		LuaRefs--;
		if (LuaRefs == 0 && LuaPendingDelete) {
			LuaResetInternal();
		}
	}

	void ExtensionState::LuaReset(bool startup)
	{
		if (!EnableLua) {
			OsiWarn("Lua extensions not enabled; not initializing Lua VM");
			return;
		}

		if (LuaPendingDelete) {
			OsiWarn("State delete is already pending!");
		}

		LuaPendingDelete = true;
		if (startup) {
			LuaPendingStartup = true;
		}

		if (LuaRefs == 0) {
			LuaResetInternal();
		} else {
			OsiWarn("Lua state deletion deferred (" << LuaRefs << " references still alive)");
		}
	}

	void ExtensionState::LuaResetInternal()
	{
		assert(LuaPendingDelete);
		assert(LuaRefs == 0);

		LuaPendingDelete = false;

		// Destroy previous instance first to make sure that no dangling
		// references are made to the old state while constructing the new
		Lua.reset();
		Lua = std::make_unique<LuaState>();
		Lua->StoryFunctionMappingsUpdated();
		OsiWarn("LUA VM reset.");

		if (LuaPendingStartup) {
			LuaPendingStartup = false;
			LuaStartup();
		}
	}

	void ExtensionState::LuaStartup()
	{
		LuaStatePin lua(*this);
		if (!lua) {
			OsiError("Called when the Lua VM has not been initialized!");
			return;
		}

		auto modManager = GetModManager();
		if (modManager == nullptr) {
			OsiError("Could not bootstrap Lua modules - mod manager not available");
			return;
		}

		auto & mods = modManager->BaseModule.LoadOrderedModules.Set;

		OsiWarn("Bootstrapping Lua modules ...");
		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const & mod = mods.Buf[i];
			auto dir = ToUTF8(mod.Info.Directory.GetPtr());
			auto bootstrapFile = "Mods/" + dir + "/Story/RawFiles/Lua/Bootstrap.lua";
			auto reader = gOsirisProxy->GetLibraryManager().MakeFileReader(bootstrapFile);
			if (reader.IsLoaded()) {
				OsiWarn("Found bootstrap file: " << bootstrapFile);
				LuaLoadGameFile(reader);
			}
		}
		
		lua->FinishStartup();
	}

	void ExtensionState::LuaLoadExternalFile(std::string const & path)
	{
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

		LuaStatePin lua(*this);
		if (!lua) {
			OsiError("Called when the Lua VM has not been initialized!");
			return;
		}

		lua->LoadScript(s);
		OsiWarn("Loaded external script: " << path);
	}

	void ExtensionState::LuaLoadGameFile(FileReaderPin & reader)
	{
		if (!reader.IsLoaded()) {
			OsiError("Attempted to load script from invalid file reader");
			return;
		}

		LuaStatePin lua(*this);
		if (!lua) {
			OsiError("Called when the Lua VM has not been initialized!");
			return;
		}

		lua->LoadScript(reader.ToString());
	}

	void ExtensionState::LuaLoadGameFile(std::string const & path)
	{
		auto reader = gOsirisProxy->GetLibraryManager().MakeFileReader(path);
		if (!reader.IsLoaded()) {
			OsiError("Script file could not be opened: " << path);
			return;
		}

		LuaLoadGameFile(reader);
		OsiWarn("Loaded game script: " << path);
	}

	void ExtensionState::LuaLoadGameFile(std::string const & modNameGuid, std::string const & fileName)
	{
		auto mod = GetModManager()->FindModByNameGuid(modNameGuid);
		if (mod == nullptr) {
			OsiError("Mod does not exist or is not loaded: " << modNameGuid);
			return;
		}

		std::string path("Mods/");
		path += ToUTF8(mod->Info.Directory.GetPtr());
		path += "/Story/RawFiles/Lua/";
		path += fileName;

		LuaLoadGameFile(path);
	}
}
