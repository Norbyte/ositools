#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/Proxies/LuaArrayProxy.inl>
#include <Lua/Shared/Proxies/LuaEntityProxy.inl>
#include <Lua/Shared/Proxies/LuaEvent.inl>
#include <Lua/Shared/Proxies/LuaMapProxy.inl>
#include <Lua/Shared/Proxies/LuaObjectProxy.inl>
#include <Lua/Shared/Proxies/LuaCppObjectProxy.inl>
#include <Lua/Shared/Proxies/LuaEnumValue.inl>

BEGIN_SE()

template <class Fun>
TypeInformation DoConstructFunctionSignature(Fun f)
{
	TypeInformation sig;
	ConstructFunctionSignature(sig, f);
	return sig;
}

#define DECLARE_MODULE(name, role) { \
	ModuleDefinition mod; \
	mod.Role = ModuleRole::role; \
	mod.Table = FixedString{#name};

#define DECLARE_SUBMODULE(name, sub, role) { \
	ModuleDefinition mod; \
	mod.Role = ModuleRole::role; \
	mod.Table = FixedString{#name}; \
	mod.SubTable = FixedString{#sub};

#define BEGIN_MODULE() mod.Functions = {
#define MODULE_FUNCTION(fun) { FixedString{#fun}, LuaWrapFunction(&fun), DoConstructFunctionSignature(&fun) },
#define MODULE_NAMED_FUNCTION(name, fun){ FixedString{name}, LuaWrapFunction(&fun), DoConstructFunctionSignature(&fun) },
#define END_MODULE() }; \
	gModuleRegistry.RegisterModule(mod); \
}

END_SE()
