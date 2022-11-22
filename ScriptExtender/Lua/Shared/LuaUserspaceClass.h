#pragma once

#include <GameDefinitions/Base/Base.h>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_NS(lua)

class UserspaceClassBase
{
public:
	inline UserspaceClassBase(lua_State* L, int index)
		: L_(L), ref_(L, index)
	{}
	
	inline UserspaceClassBase(lua_State* L, RegistryOrLocalRef const& reg)
		: L_(L), ref_(reg)
	{}

	inline ~UserspaceClassBase() {}

	void Push() const
	{
		ref_.Push(L_);
	}

	template <class Ret, class ...Args>
	std::optional<Ret> CallMethod(char const* name, Overload<Ret> ov, Args... args)
	{
		ProtectedFunctionCaller<std::tuple<Args...>, Ret> caller;
		caller.Self = ref_;
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		if (caller.Call(L_)) {
			return caller.Retval.Value;
		} else {
			return {};
		}
	}

	template <class Ret, class ...Args>
	Ret CallMethodOrDefault(char const* name, Ret const& defaultVal, Args... args)
	{
		ProtectedFunctionCaller<std::tuple<Args...>, Ret> caller;
		caller.Self = ref_;
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		if (caller.Call(L_)) {
			return caller.Retval.Value;
		} else {
			return defaultVal;
		}
	}

	template <class ...Args>
	void CallMethod(char const* name, Overload<void> ov, Args... args)
	{
		ProtectedFunctionCaller<std::tuple<Args...>, void> caller;
		caller.Self = ref_;
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		caller.Call(L_);
	}

private:
	lua_State* L_;
	RegistryOrLocalRef ref_;
};

END_NS()
