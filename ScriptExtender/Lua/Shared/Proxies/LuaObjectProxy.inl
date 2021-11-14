#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <GameDefinitions/EntitySystem.h>

BEGIN_NS(lua)

bool GenericPropertyMap::GetRawProperty(lua_State* L, LifetimeHolder const& lifetime, void* object, FixedString const& prop) const
{
	auto it = Properties.find(prop);
	if (it == Properties.end()) {
		return 0;
	}

	return it->second.Get(L, lifetime, object, it->second.Offset, it->second.Flag);
}

bool GenericPropertyMap::SetRawProperty(lua_State* L, LifetimeHolder const& lifetime, void* object, FixedString const& prop, int index) const
{
	auto it = Properties.find(prop);
	if (it == Properties.end()) {
		return 0;
	}

	return it->second.Set(L, lifetime, object, index, it->second.Offset, it->second.Flag);
}

void GenericPropertyMap::AddRawProperty(char const* prop, typename RawPropertyAccessors::Getter* getter,
	typename RawPropertyAccessors::Setter* setter, std::size_t offset, uint64_t flag)
{
	auto key = FixedString(prop);
	assert(Properties.find(key) == Properties.end());
	Properties.insert(std::make_pair(key, RawPropertyAccessors{ key, getter, setter, offset, flag }));
}


char const* const ObjectProxy2::MetatableName = "Object";

int ObjectProxy2::Index(lua_State* L)
{
	StackCheck _(L, 1);
	auto impl = GetImpl();
	if (!lifetime_.IsAlive()) {
		luaL_error(L, "Attempted to read dead object of type '%s'", impl->GetTypeName());
		push(L, nullptr);
		return 1;
	}

	auto prop = get<FixedString>(L, 2);
	if (!impl->GetProperty(L, prop)) {
		push(L, nullptr);
	}

	return 1;
}

int ObjectProxy2::NewIndex(lua_State* L)
{
	StackCheck _(L, 0);
	auto impl = GetImpl();
	if (!lifetime_.IsAlive()) {
		luaL_error(L, "Attempted to write dead object of type '%s'", impl->GetTypeName());
		return 0;
	}

	auto prop = get<FixedString>(L, 2);
	impl->SetProperty(L, prop, 3);
	return 0;
}

int ObjectProxy2::Next(lua_State* L)
{
	auto impl = GetImpl();
	if (!lifetime_.IsAlive()) {
		luaL_error(L, "Attempted to iterate dead object of type '%s'", impl->GetTypeName());
		return 0;
	}

	if (lua_type(L, 2) == LUA_TNIL) {
		return impl->Next(L, FixedString{});
	} else {
		auto key = get<FixedString>(L, 2);
		return impl->Next(L, key);
	}
}

int ObjectProxy2::ToString(lua_State* L)
{
	StackCheck _(L, 1);
	char entityName[200];
	if (lifetime_.IsAlive()) {
		_snprintf_s(entityName, std::size(entityName) - 1, "%s (%p)", GetImpl()->GetTypeName().GetString(), GetImpl()->GetRaw());
	} else {
		_snprintf_s(entityName, std::size(entityName) - 1, "%s (%p, DEAD)", GetImpl()->GetTypeName().GetString(), GetImpl()->GetRaw());
	}

	push(L, entityName);
	return 1;
}

int ObjectProxy2::GC(lua_State* L)
{
	this->~ObjectProxy2();
	return 0;
}

void* ObjectProxy2::CheckedGetRaw(lua_State* L, int index, FixedString const& typeName)
{
	auto proxy = Userdata<ObjectProxy2>::CheckUserData(L, index);
	if (proxy->GetImpl()->IsA(typeName)) {
		auto obj = proxy->GetRaw();
		if (obj == nullptr) {
			luaL_error(L, "Argument %d: got object of type '%s' whose lifetime has expired", index, typeName.GetString());
			return nullptr;
		} else {
			return obj;
		}
	} else {
		luaL_error(L, "Argument %d: expected an object of type '%s', got '%s'", index, typeName.GetString(), proxy->GetImpl()->GetTypeName());
		return nullptr;
	}
}

END_NS()
