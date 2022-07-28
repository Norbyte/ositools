#include <Lua/Shared/Proxies/LuaCppObjectProxy.h>
#include <Lua/Shared/LuaBinding.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(lua)

struct CppObjectProxyHelpers
{
	static int Next(lua_State* L, GenericPropertyMap const& pm, void* object, LifetimeHandle const& lifetime, FixedString const& key)
	{
		if (!key) {
			if (!pm.Properties.empty()) {
				StackCheck _(L, 2);
				auto it = pm.Properties.begin();
				push(L, it->first);
				if (!pm.GetRawProperty(L, lifetime, object, it->first)) {
					push(L, nullptr);
				}

				return 2;
			}
		} else {
			auto it = pm.Properties.find(key);
			if (it != pm.Properties.end()) {
				++it;
				if (it != pm.Properties.end()) {
					StackCheck _(L, 2);
					push(L, it->first);
					if (!pm.GetRawProperty(L, lifetime, object, it->first)) {
						push(L, nullptr);
					}

					return 2;
				}
			}
		}

		return 0;
	}

	static bool IsA(GenericPropertyMap const& map, FixedString const& typeName)
	{
		if (map.Name == typeName) {
			return true;
		}

		for (auto const& parent : map.Parents) {
			if (parent == typeName) {
				return true;
			}
		}

		return false;
	}
};

int CppPropertyMapManager::RegisterPropertyMap(GenericPropertyMap* mt)
{
	propertyMaps_.push_back(mt);
	return (int)(propertyMaps_.size() - 1);
}

GenericPropertyMap* CppPropertyMapManager::GetPropertyMap(int index)
{
	if (index >= 0 && (uint32_t)index < propertyMaps_.size()) {
		return propertyMaps_[index];
	} else {
		return nullptr;
	}
}

int CppMetatableManager::RegisterMetatable(CMetatable* mt)
{
	metatables_.push_back(mt);
	return (int)(metatables_.size() - 1);
}

CMetatable* CppMetatableManager::GetMetatable(int index)
{
	if (index >= 0 && (uint32_t)index < metatables_.size()) {
		return metatables_[index];
	} else {
		return nullptr;
	}
}

CppMetatableManager& CppMetatableManager::FromLua(lua_State* L)
{
	return State::FromLua(L)->GetMetatableManager();
}


int LightObjectProxyByRefMetatable::Index(lua_State* L, CppObjectMetadata& self)
{
	StackCheck _(L, 1);
	if (!self.Lifetime.IsAlive(L)) {
		luaL_error(L, "Attempted to read object of type '%s' whose lifetime has expired", GetTypeName(L, self));
		return 0;
	}

	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	auto prop = get<FixedString>(L, 2);
	if (!pm->GetRawProperty(L, self.Lifetime, self.Ptr, prop)) {
		luaL_error(L, "Object of type '%s' has no property named '%s'", GetTypeName(L, self), prop.GetString());
		return 0;
	}

	return 1;
}

int LightObjectProxyByRefMetatable::NewIndex(lua_State* L, CppObjectMetadata& self)
{
	StackCheck _(L, 0);
	if (!self.Lifetime.IsAlive(L)) {
		luaL_error(L, "Attempted to write object of type '%s' whose lifetime has expired", GetTypeName(L, self));
		return 0;
	}

	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	auto prop = get<FixedString>(L, 2);
	if (!pm->SetRawProperty(L, self.Lifetime, self.Ptr, prop, 3)) {
		luaL_error(L, "Object of type '%s' has no property named '%s'", GetTypeName(L, self), prop.GetString());
		return 0;
	}

	return 0;
}

int LightObjectProxyByRefMetatable::ToString(lua_State* L, CppObjectMetadata& self)
{
	StackCheck _(L, 1);

	char entityName[200];
	if (self.Lifetime.IsAlive(L)) {
		_snprintf_s(entityName, std::size(entityName) - 1, "%s (%p)", GetTypeName(L, self), self.Ptr);
	} else {
		_snprintf_s(entityName, std::size(entityName) - 1, "%s (%p, DEAD REFERENCE)", GetTypeName(L, self), self.Ptr);
	}

	push(L, entityName);
	return 1;
}

int LightObjectProxyByRefMetatable::IsEqual(lua_State* L, CppObjectMetadata& self, CppObjectMetadata& other)
{
	push(L, self.Ptr == other.Ptr && self.PropertyMapTag == other.PropertyMapTag);
	return 1;
}

int LightObjectProxyByRefMetatable::Next(lua_State* L, CppObjectMetadata& self)
{
	if (!self.Lifetime.IsAlive(L)) {
		luaL_error(L, "Attempted to iterate object of type '%s' whose lifetime has expired", GetTypeName(L, self));
		return 0;
	}

	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	if (lua_type(L, 2) == LUA_TNIL) {
		return CppObjectProxyHelpers::Next(L, *pm, self.Ptr, self.Lifetime, FixedString{});
	} else {
		auto key = get<FixedString>(L, 2);
		return CppObjectProxyHelpers::Next(L, *pm, self.Ptr, self.Lifetime, key);
	}
}

int LightObjectProxyByRefMetatable::Name(lua_State* L, CppObjectMetadata& self)
{
	push(L, GetTypeName(L, self));
	return 1;
}

char const* LightObjectProxyByRefMetatable::GetTypeName(lua_State* L, CppObjectMetadata& self)
{
	auto pm = gExtender->GetPropertyMapManager().GetPropertyMap(self.PropertyMapTag);
	return pm->Name.GetString();
}

int LightCppObjectMetatable<LightObjectProxyByRefMetatable>::registryIndex_{ -1 };

END_NS()
