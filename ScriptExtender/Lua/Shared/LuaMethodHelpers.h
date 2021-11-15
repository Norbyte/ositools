#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <Lua/Shared/Proxies/LuaArrayProxy.h>
#include <Lua/Shared/Proxies/LuaMapProxy.h>
#include <Lua/Shared/Proxies/LuaSetProxy.h>

BEGIN_NS(lua)

template <class T>
auto MakeObjectRef(lua_State* L, LifetimeHolder const& lifetime, T* value)
{
	if constexpr (IsArrayLike<T>::Value) {
		if constexpr (ByVal<IsArrayLike<T>::TElement>::Value) {
			return ArrayProxy::MakeByVal<IsArrayLike<T>::TElement>(L, value, lifetime);
		} else {
			return ArrayProxy::MakeByRef<IsArrayLike<T>::TElement>(L, value, lifetime);
		}
	} else if constexpr (IsMapLike<T>::Value) {
		static_assert(ByVal<IsMapLike<T>::TKey>::Value, "Map key is a type that we cannot serialize by-value?");

		if constexpr (ByVal<IsMapLike<T>::TValue>::Value) {
			return MapProxy::MakeByVal(L, value, lifetime);
		} else {
			return MapProxy::MakeByRef(L, value, lifetime);
		}
	} else if constexpr (IsSetLike<T>::Value) {
		if constexpr (ByVal<IsSetLike<T>::TElement>::Value) {
			return SetProxy::MakeByVal<IsSetLike<T>::TElement>(L, value, lifetime);
		} else {
			return SetProxy::MakeByRef<IsSetLike<T>::TElement>(L, value, lifetime);
		}
	} else if constexpr (std::is_pointer_v<T>) {
		if constexpr (std::is_const_v<std::remove_pointer_t<T>>) {
			if (value != nullptr) {
				return MakeObjectRef(L, lifetime, const_cast<std::remove_const_t<std::remove_pointer_t<T>>*>(*value));
			} else {
				push(L, nullptr);
				return (ObjectProxyRefImpl<std::remove_const_t<std::remove_pointer_t<T>>>*)nullptr;
			}
		} else {
			return MakeObjectRef(L, lifetime, *value);
		}
	} else {
		if (value != nullptr) {
			return ObjectProxy2::MakeRef(L, value, lifetime);
		} else {
			push(L, nullptr);
			return (ObjectProxyRefImpl<T>*)nullptr;
		}
	}
}

template <class T>
auto MakeObjectRef(lua_State* L, LifetimeHolder const& lifetime, OverrideableProperty<T>* value)
{
	return MakeObjectRef(L, lifetime, &value->Value);
}

template <class T>
auto MakeObjectRef(lua_State* L, T* value)
{
	return MakeObjectRef(L, GetCurrentLifetime(), value);
}


template <class T>
inline void PushReturnValue(lua_State* L, T& v)
{
	if constexpr (std::is_pointer_v<T>) {
		MakeObjectRef(L, v);
	} else {
		LuaWrite(L, v);
	}
}

template <class T>
inline void PushReturnValue(lua_State* L, Array<T>* v)
{
	if constexpr (!ByVal<T>::Value) {
		MakeObjectRef(L, v);
	} else {
		LuaWrite(L, v);
	}
}

template <class T>
inline void PushReturnValue(lua_State* L, Vector<T>* v)
{
	if constexpr (!ByVal<T>::Value) {
		MakeObjectRef(L, v);
	} else {
		LuaWrite(L, v);
	}
}

template <class T>
inline void PushReturnValue(lua_State* L, ObjectSet<T>* v)
{
	if constexpr (!ByVal<T>::Value) {
		MakeObjectRef(L, v);
	} else {
		LuaWrite(L, v);
	}
}

template <class TKey, class TValue>
inline void PushReturnValue(lua_State* L, RefMap<TKey, TValue>* v)
{
	if constexpr (!ByVal<TValue>::Value) {
		MakeObjectRef(L, v);
	} else {
		LuaWrite(L, v);
	}
}

template <class TKey, class TValue>
inline void PushReturnValue(lua_State* L, Map<TKey, TValue>* v)
{
	if constexpr (!ByVal<TValue>::Value) {
		MakeObjectRef(L, v);
	} else {
		LuaWrite(L, v);
	}
}

template <class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, void (T::* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 0);
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	(obj->*fun)(L, checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	return 0;
}

template <class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, UserReturn (T::* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto nret = (obj->*fun)(L, checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	return (int)nret;
}

template <class R, class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, R (T::* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 1);
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto retval = (obj->*fun)(L, checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	PushReturnValue(L, retval);
	return 1;
}

template <class R, class T, class ...Args>
inline int CallMethod(lua_State* L, R(T::* fun)(lua_State*, Args...)) {
	return CallMethodHelper(L, fun, std::index_sequence_for<Args...>());
}

template <class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, void (T::* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 0);
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	(obj->*fun)(checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	return 0;
}

template <class R, class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, R (T::* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 1);
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto retval = (obj->*fun)(checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	PushReturnValue(L, retval);
	return 1;
}

template <class R, class T, class ...Args>
inline int CallMethod(lua_State* L, R(T::* fun)(Args...)) {
	return CallMethodHelper(L, fun, std::index_sequence_for<Args...>());
}

// Special case of CallMethod that only accepts functions that return exactly 1 value
template <class R, class T, class ...Args, size_t ...Indices>
inline int CallGetterHelper(lua_State* L, R (T::* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 1);
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto retval = (obj->*fun)(checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	PushReturnValue(L, retval);
	return 1;
}

template <class R, class T, class ...Args>
inline int CallGetter(lua_State* L, R(T::* fun)(Args...)) {
	return CallGetterHelper(L, fun, std::index_sequence_for<Args...>());
}

END_NS()
