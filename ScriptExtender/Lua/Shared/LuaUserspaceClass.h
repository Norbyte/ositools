#pragma once

#include <GameDefinitions/Base/Base.h>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_NS(lua)

class UserspaceClassBase
{
public:
	inline UserspaceClassBase(lua_State* L, Ref const& reg)
		: L_(L), ref_(L, reg)
	{}

	inline ~UserspaceClassBase() {}

	void Push() const
	{
		ref_.Push();
	}

	template <class Ret, class ...Args>
	std::optional<Ret> CallMethod(char const* name, Overload<Ret> ov, Args... args)
	{
		ProtectedMethodCaller<std::tuple<Args...>, Ret> caller;
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
	std::optional<Ret> CallOptionalMethod(char const* name, Overload<Ret> ov, Args... args)
	{
		ProtectedMethodCaller<std::tuple<Args...>, Ret> caller;
		caller.Self = ref_;
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		caller.Optional = true;
		if (caller.Call(L_)) {
			return caller.Retval.Value;
		} else {
			return {};
		}
	}

	template <class Ret, class ...Args>
	Ret CallMethodOrDefault(char const* name, Ret const& defaultVal, Args... args)
	{
		ProtectedMethodCaller<std::tuple<Args...>, Ret> caller;
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
	bool CallMethod(char const* name, Overload<void> ov, Args... args)
	{
		ProtectedMethodCaller<std::tuple<Args...>, void> caller;
		caller.Self = ref_;
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		return caller.Call(L_);
	}

	template <class ...Args>
	bool CallOptionalMethod(char const* name, Overload<void> ov, Args... args)
	{
		ProtectedMethodCaller<std::tuple<Args...>, void> caller;
		caller.Self = ref_;
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		caller.Optional = true;
		return caller.Call(L_);
	}

private:
	lua_State* L_;
	RegistryEntry ref_;
};

// Reference that may outlive the Lua state/variable that it is referencing.
// (eg. statuses, controllers, tasks that are bound to game entities, not the Lua state)
class PersistentUserspaceClassBase
{
public:
	inline PersistentUserspaceClassBase(lua_State* L, int index)
		: L_(L), ref_(L, index)
	{}
	
	inline PersistentUserspaceClassBase(lua_State* L, PersistentRef const& reg)
		: L_(L), ref_(reg)
	{}

	inline ~PersistentUserspaceClassBase() {}

	virtual bool IsValid() const = 0;

	template <class Ret, class ...Args>
	std::optional<Ret> CallMethod(char const* name, Overload<Ret> ov, Args... args)
	{
		if (!IsValid()) {
			return {};
		}

		ProtectedMethodCaller<std::tuple<Args...>, Ret> caller;
		caller.Self = ref_.MakeRef(L_);
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
		if (!IsValid()) {
			return {};
		}

		ProtectedMethodCaller<std::tuple<Args...>, Ret> caller;
		caller.Self = ref_.MakeRef(L_);
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		if (caller.Call(L_)) {
			return caller.Retval.Value;
		} else {
			return defaultVal;
		}
	}

	template <class ...Args>
	bool CallMethod(char const* name, Overload<void> ov, Args... args)
	{
		if (!IsValid()) {
			return false;
		}

		ProtectedMethodCaller<std::tuple<Args...>, void> caller;
		caller.Self = ref_.MakeRef(L_);
		caller.Method = name;
		caller.Args = std::tuple<Args...>(std::forward<Args>(args)...);
		caller.Call(L_);
		return true;
	}

protected:
	lua_State* L_;
	PersistentRef ref_;
};

// Reference that supports dynamic rebinding
// (i.e. can be resurrected after the Lua state was recreated)
class RebindableUserspaceClassBase : public PersistentUserspaceClassBase
{
public:
	inline RebindableUserspaceClassBase(lua_State* L, int index)
		: PersistentUserspaceClassBase(L, index)
	{}
	
	inline RebindableUserspaceClassBase(lua_State* L, PersistentRef const& reg)
		: PersistentUserspaceClassBase(L, reg)
	{}

	inline ~RebindableUserspaceClassBase() {}

	virtual bool Rebind() = 0;

	bool RebindIfRequired()
	{
		if (!IsValid()) {
			return Rebind();
		} else {
			return true;
		}
	}
};

template <class T>
class UserObjectConstructor
{
public:
	UserObjectConstructor(lua_State* L, lua::Ref const& ref)
		: ref_(L, ref)
	{}

	template <class ...Args>
	lua::RegistryEntry Construct(lua_State* L, Args... args)
	{
		lua::StackCheck _(L);
		lua::ProtectedFunctionCaller<std::tuple<Args...>, dse::lua::RegistryEntry> caller;
		caller.Function = ref_;
		caller.Args = std::tuple(args...);
		if (caller.Call(L, "constructing user object")) {
			return std::move(caller.Retval.Value);
		} else {
			return {};
		}
	}

private:
	lua::RegistryEntry ref_;
};

END_NS()
