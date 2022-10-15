#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <Lua/Shared/Proxies/LuaArrayProxy.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

BEGIN_NS(lua)

template <class T>
PropertyOperationResult GenericGetOffsetProperty(lua_State* L, LifetimeHandle const& lifetime, void* obj, std::size_t offset, uint64_t)
{
	auto* value = (T*)((std::uintptr_t)obj + offset);
	return (LuaWrite(L, *value) == 1) ? PropertyOperationResult::Success : PropertyOperationResult::Unknown;
}

template <class T>
PropertyOperationResult GenericSetOffsetProperty(lua_State* L, LifetimeHandle const& lifetime, void* obj, int index, std::size_t offset, uint64_t)
{
	auto* value = (T*)((std::uintptr_t)obj + offset);
	lua_pushvalue(L, index);
	LuaRead(L, *value);
	lua_pop(L, 1);
	return PropertyOperationResult::Success;
}

template <class UnderlyingType>
PropertyOperationResult GenericGetOffsetBitmaskFlag(lua_State* L, LifetimeHandle const& lifetime, void* obj, std::size_t offset, uint64_t flag)
{
	auto value = *(UnderlyingType*)((std::uintptr_t)obj + offset);
	push(L, (value & (UnderlyingType)flag) == (UnderlyingType)flag);
	return PropertyOperationResult::Success;
}

template <class UnderlyingType>
PropertyOperationResult GenericSetOffsetBitmaskFlag(lua_State* L, LifetimeHandle const& lifetime, void* obj, int index, std::size_t offset, uint64_t flag)
{
	auto* value = (UnderlyingType*)((std::uintptr_t)obj + offset);
	auto set = get<bool>(L, index);
	if (set) {
		*value |= (UnderlyingType)flag;
	} else {
		*value &= (UnderlyingType)~flag;
	}

	return PropertyOperationResult::Success;
}

template <class T>
PropertyOperationResult GenericGetOffsetRefProperty(lua_State* L, LifetimeHandle const& lifetime, void* obj, std::size_t offset, uint64_t)
{
	auto* value = (T*)((std::uintptr_t)obj + offset);
	MakeObjectRef(L, lifetime, value);
	return PropertyOperationResult::Success;
}

void CopyRawProperties(GenericPropertyMap const& base, GenericPropertyMap& child);

template <class T, class T2>
inline void InheritProperties(LuaPropertyMap<T> const& base, LuaPropertyMap<T2>& child)
{
	static_assert(std::is_base_of_v<T, T2>, "Can only copy properties from base class");
	static_assert(static_cast<T*>(reinterpret_cast<T2*>(nullptr)) == reinterpret_cast<T*>(nullptr), "Base and child class should start at same base ptr");

	// Check to make sure that the property map we're inheriting from is already initialized
	assert(base.Initialized);
	assert(child.IsInitializing);

	CopyRawProperties(base, child);

	if (child.FallbackGetter == nullptr) {
		child.FallbackGetter = base.FallbackGetter;
	}

	if (child.FallbackSetter == nullptr) {
		child.FallbackSetter = base.FallbackSetter;
	}
}

END_NS()
