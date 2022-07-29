#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

struct EventBase
{
	FixedString Name;
	bool CanPreventAction{ false };
	bool ActionPrevented{ false };
	bool Stopped{ false };

	void StopPropagation();
	void PreventAction();
};

struct EmptyEvent : public EventBase {};

END_NS()
