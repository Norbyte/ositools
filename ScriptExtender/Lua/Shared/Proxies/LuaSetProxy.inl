#include <Lua/Shared/Proxies/LuaSetProxy.h>

BEGIN_NS(lua)

char const* const SetProxy::MetatableName = "Set";

int SetProxy::Index(lua_State* L)
{
	StackCheck _(L, 1);
	auto impl = GetImpl();
	if (!lifetime_.IsAlive(L)) {
		luaL_error(L, "Attempted to read dead Set<%s>", impl->GetType().TypeName.GetString());
		push(L, nullptr);
		return 1;
	}

	push(L, impl->HasElement(L, 2));
	return 1;
}

int SetProxy::NewIndex(lua_State* L)
{
	StackCheck _(L, 0);
	auto impl = GetImpl();
	if (!lifetime_.IsAlive(L)) {
		luaL_error(L, "Attempted to write dead Set<%s>", impl->GetType().TypeName.GetString());
		return 0;
	}

	auto val = get<bool>(L, 3);
	if (val) {
		impl->AddElement(L, 3);
	} else {
		impl->RemoveElement(L, 3);
	}

	return 0;
}

int SetProxy::Length(lua_State* L)
{
	StackCheck _(L, 1);
	auto impl = GetImpl();
	if (!lifetime_.IsAlive(L)) {
		luaL_error(L, "Attempted to get length of dead Set<%s>", impl->GetType().TypeName.GetString());
		push(L, nullptr);
		return 1;
	}

	push(L, impl->Length());
	return 1;
}

int SetProxy::Next(lua_State* L)
{
	auto impl = GetImpl();
	if (!lifetime_.IsAlive(L)) {
		luaL_error(L, "Attempted to iterate dead Set<%s>", impl->GetType().TypeName.GetString());
		return 0;
	}

	if (lua_type(L, 2) == LUA_TNIL) {
		return impl->Next(L, -1);
	} else {
		auto key = get<int>(L, 2);
		return impl->Next(L, key);
	}
}

int SetProxy::ToString(lua_State* L)
{
	StackCheck _(L, 1);
	char entityName[200];
	if (lifetime_.IsAlive(L)) {
		_snprintf_s(entityName, std::size(entityName) - 1, "Set<%s> (%p)", GetImpl()->GetType().TypeName.GetString(), GetImpl()->GetRaw());
	} else {
		_snprintf_s(entityName, std::size(entityName) - 1, "Set<%s> (%p, DEAD REFERENCE)", GetImpl()->GetType().TypeName.GetString(), GetImpl()->GetRaw());
	}

	push(L, entityName);
	return 1;
}

bool SetProxy::IsEqual(lua_State* L, SetProxy* other)
{
	return GetImpl()->GetRaw() == other->GetImpl()->GetRaw();
}

END_NS()
