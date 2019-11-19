#pragma once

#include <CustomFunctions.h>

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace osidbg
{
	extern char const * const LuaOsiSignatureTag;

	struct LuaOsiSignature
	{
		std::string name;
		uint32_t functionHandle;
		FunctionType type;
		uint32_t numArgs;
		uint32_t numInputArgs;
		OsiSymbolInfo::ParamInfo args[32];

		static LuaOsiSignature * TypeCheck(lua_State *L, int index);
		static LuaOsiSignature * New(lua_State *L);
		static void Register(lua_State* L);
	};

	class OsiArgumentPool
	{
	public:
		OsiArgumentPool();

		OsiArgumentDesc * AllocateArguments(uint32_t num);
		void ReleaseArguments(uint32_t num);

	private:
		std::vector<OsiArgumentDesc> argumentPool_;
		uint32_t usedArguments_;
	};

	class OsiArgumentListPin
	{
	public:
		OsiArgumentListPin(uint32_t numArgs);
		~OsiArgumentListPin();

		inline OsiArgumentDesc * Args() const
		{
			return args_;
		}

	private:
		uint32_t numArgs_;
		OsiArgumentDesc * args_;
	};


	class OsiProxyLibrary
	{
	public:
		OsiProxyLibrary();

		void Register(lua_State * L);
		void RegisterLib(lua_State * L);
		void CreateSignatureTable(lua_State * L);
		std::string GenerateOsiHelpers();

		static void LuaToOsiArg(lua_State * L, int i, OsiArgumentDesc & arg, OsiSymbolInfo::ParamInfo const & desc);
		static void OsiToLuaArg(lua_State * L, OsiArgumentDesc const & arg);

		static int Call(lua_State * L);
		static int Event(lua_State * L);
		static int Query(lua_State * L);

	private:
		OsiArgumentPool argumentPool_;
	};


	class LuaState
	{
	public:
		lua_State * state_;
		OsiProxyLibrary proxy_;

		LuaState();
		~LuaState();

		void OpenLibs();


		bool LoadScript(std::string const & script, std::string const & name = "");
		void Call(char const * func, char const * arg);
	};
}
