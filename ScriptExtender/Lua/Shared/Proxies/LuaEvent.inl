#include <Lua/Shared/Proxies/LuaEvent.h>

BEGIN_NS(lua)

char const* const EventObject::MetatableName = "Event";

int EventObject::StopPropagation(lua_State* L)
{
	auto self = EventObject::CheckUserData(L, 1);
	self->eventStopped_ = true;
	return 0;
}

int EventObject::Index(lua_State* L)
{
	StackCheck _(L, 1);
	auto impl = GetImpl();
	if (!lifetime_.IsAlive()) {
		luaL_error(L, "Attempted to read dead event of type '%s'", impl->GetType().TypeName.GetString());
		push(L, nullptr);
		return 1;
	}

	auto prop = get<FixedString>(L, 2);

	if (prop == GFS.strName) {
		push(L, eventName_);
	} else if (prop == GFS.strReadOnly) {
		push(L, !writeable_);
	} else if (prop == GFS.strCanPreventAction) {
		push(L, !canPreventAction_);
	} else if (prop == GFS.strStopped) {
		push(L, eventStopped_);
	} else if (prop == GFS.strStopPropagation) {
		push(L, &EventObject::StopPropagation);
	} else if (!impl->GetProperty(L, lifetime_.Get(), prop)) {
		push(L, nullptr);
	}

	return 1;
}

int EventObject::NewIndex(lua_State* L)
{
	StackCheck _(L, 0);
	auto impl = GetImpl();
	if (!lifetime_.IsAlive()) {
		luaL_error(L, "Attempted to write dead event of type '%s'", impl->GetType().TypeName.GetString());
		return 0;
	}

	auto prop = get<FixedString>(L, 2);
	if (writeable_) {
		impl->SetProperty(L, lifetime_.Get(), prop, 3);
	} else {
		luaL_error(L, "Event '%s' is not writeable in this context (while attempting to write property %s.%s)",
			eventName_, impl->GetType().TypeName.GetString(), prop.GetString());
		return false;
	}

	return 0;
}

int EventObject::Next(lua_State* L)
{
	auto impl = GetImpl();
	if (!lifetime_.IsAlive()) {
		luaL_error(L, "Attempted to iterate dead event of type '%s'", impl->GetType().TypeName.GetString());
		return 0;
	}

	if (lua_type(L, 2) == LUA_TNIL) {
		return impl->Next(L, lifetime_.Get(), FixedString{});
	} else {
		auto key = get<FixedString>(L, 2);
		return impl->Next(L, lifetime_.Get(), key);
	}
}

int EventObject::ToString(lua_State* L)
{
	StackCheck _(L, 1);
	char entityName[200];
	if (lifetime_.IsAlive()) {
		_snprintf_s(entityName, std::size(entityName) - 1, "Event %s (%s)", eventName_, GetImpl()->GetType().TypeName.GetString());
	} else {
		_snprintf_s(entityName, std::size(entityName) - 1, "Event %s (%s, DEAD REFERENCE)", eventName_, GetImpl()->GetType().TypeName.GetString());
	}

	push(L, entityName);
	return 1;
}

int EventObject::GC(lua_State* L)
{
	this->~EventObject();
	return 0;
}

END_NS()
