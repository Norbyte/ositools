#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <Lua/Shared/Proxies/LuaArrayProxy.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

BEGIN_NS(lua)

extern bool EnableWriteProtectedWrites;

template <class T>
bool GenericGetOffsetProperty(lua_State* L, LifetimeHolder const& lifetime, void* obj, std::size_t offset, uint64_t)
{
	auto* value = (T*)((std::uintptr_t)obj + offset);
	return LuaWrite(L, *value) == 1;
}

template <class T>
bool GenericSetOffsetProperty(lua_State* L, LifetimeHolder const& lifetime, void* obj, int index, std::size_t offset, uint64_t)
{
	auto* value = (T*)((std::uintptr_t)obj + offset);
	lua_pushvalue(L, index);
	LuaRead(L, *value);
	lua_pop(L, 1);
	return true;
}

template <class T>
bool GenericGetOffsetBitmaskFlag(lua_State* L, LifetimeHolder const& lifetime, void* obj, std::size_t offset, uint64_t flag)
{
	auto value = *(T*)((std::uintptr_t)obj + offset);
	push(L, (value & (T)flag) == (T)flag);
	return 1;
}

template <class T>
bool GenericSetOffsetBitmaskFlag(lua_State* L, LifetimeHolder const& lifetime, void* obj, int index, std::size_t offset, uint64_t flag)
{
	auto* value = (T*)((std::uintptr_t)obj + offset);
	auto set = get<bool>(L, index);
	if (set) {
		*value |= (T)flag;
	} else {
		*value &= (T)~flag;
	}

	return true;
}

template <class T>
bool GenericSetOffsetWriteProtectedProperty(lua_State* L, LifetimeHolder const& lifetime, void* obj, int index, std::size_t offset, uint64_t flag)
{
	if (EnableWriteProtectedWrites) {
		return GenericSetOffsetProperty<T>(L, lifetime, obj, index, offset, flag);
	} else {
		OsiError("Attempted to set a write-protected property");
		return false;
	}
}

inline bool SetPropertyWriteProtected(lua_State* L, LifetimeHolder const& lifetime, void* obj, int index, std::size_t offset, uint64_t)
{
	return false;
}

template <class T>
bool GenericGetOffsetRefProperty(lua_State* L, LifetimeHolder const& lifetime, void* obj, std::size_t offset, uint64_t)
{
	auto* value = (T*)((std::uintptr_t)obj + offset);
	MakeObjectRef(L, lifetime, value);
	return true;
}

inline bool GenericSetOffsetRefProperty(lua_State* L, LifetimeHolder const& lifetime, void* obj, int index, std::size_t offset, uint64_t)
{
	return false;
}

void CopyRawProperties(GenericPropertyMap const& base, GenericPropertyMap& child, STDString const& baseClsName);

template <class T, class T2>
inline void CopyProperties(LuaPropertyMap<T> const& base, LuaPropertyMap<T2>& child, STDString const& baseClsName)
{
	static_assert(std::is_base_of_v<T, T2>, "Can only copy properties from base class");
	static_assert(static_cast<T*>(reinterpret_cast<T2*>(nullptr)) == reinterpret_cast<T*>(nullptr), "Base and child class should start at same base ptr");
	CopyRawProperties(base, child, baseClsName);
}

END_NS()
