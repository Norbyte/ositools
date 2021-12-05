#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <Lua/Shared/Proxies/LuaArrayProxy.h>
#include <Lua/Shared/Proxies/LuaMapProxy.h>
#include <Lua/Shared/Proxies/LuaSetProxy.h>

BEGIN_NS(lua)

template <class T>
inline void MakeObjectRef(lua_State* L, LifetimeHolder const& lifetime, T* value)
{
	if (value == nullptr) {
		push(L, nullptr);
		return;
	}

	if constexpr (LuaPolymorphic<T>::IsPolymorphic) {
		return LuaPolymorphic<T>::MakeRef(L, value, lifetime);
	} else if constexpr (IsArrayLike<T>::Value) {
		if constexpr (ByVal<IsArrayLike<T>::TElement>::Value) {
			ArrayProxy::MakeByVal<IsArrayLike<T>::TElement>(L, value, lifetime);
		} else {
			ArrayProxy::MakeByRef<IsArrayLike<T>::TElement>(L, value, lifetime);
		}
	} else if constexpr (IsMapLike<T>::Value) {
		static_assert(ByVal<IsMapLike<T>::TKey>::Value, "Map key is a type that we cannot serialize by-value?");

		if constexpr (ByVal<IsMapLike<T>::TValue>::Value) {
			MapProxy::MakeByVal(L, value, lifetime);
		} else {
			MapProxy::MakeByRef(L, value, lifetime);
		}
	} else if constexpr (IsSetLike<T>::Value) {
		if constexpr (ByVal<IsSetLike<T>::TElement>::Value) {
			SetProxy::MakeByVal<IsSetLike<T>::TElement>(L, value, lifetime);
		} else {
			SetProxy::MakeByRef<IsSetLike<T>::TElement>(L, value, lifetime);
		}
	} else if constexpr (std::is_pointer_v<T>) {
		if constexpr (std::is_const_v<std::remove_pointer_t<T>>) {
			MakeObjectRef(L, lifetime, const_cast<std::remove_const_t<std::remove_pointer_t<T>>*>(*value));
		} else {
			MakeObjectRef(L, lifetime, *value);
		}
	} else {
		if constexpr (LuaLifetimeInfo<T>::HasInfiniteLifetime) {
			ObjectProxy2::MakeRef(L, value, State::FromLua(L)->GetGlobalLifetime());
		} else {
			ObjectProxy2::MakeRef(L, value, lifetime);
		}
	}
}

template <class T>
inline auto MakeObjectRef(lua_State* L, LifetimeHolder const& lifetime, OverrideableProperty<T>* value)
{
	return MakeObjectRef(L, lifetime, &value->Value);
}

void MakeUIObjectRef(lua_State* L, LifetimeHolder const& lifetime, UIObject* value);

inline auto MakeObjectRef(lua_State* L, LifetimeHolder const& lifetime, UIObject* value)
{
	return MakeUIObjectRef(L, lifetime, value);
}

template <class T>
inline auto MakeObjectRef(lua_State* L, T* value)
{
	return MakeObjectRef(L, State::FromLua(L)->GetCurrentLifetime(), value);
}

template <class T, class... Args>
inline auto MakeObjectContainer(lua_State* L, Args... args)
{
	return ObjectProxy2::MakeContainer<T, Args...>(L, State::FromLua(L)->GetLifetimePool(), args...);
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

template <class T>
inline void PushReturnValue(lua_State* L, RefReturn<T> v)
{
	MakeObjectRef(L, v.Object);
}

template <class T>
inline void PushReturnValue(lua_State* L, ByValReturn<T> v)
{
	LuaWrite(L, v.Object);
}

template <typename ...Args>
void PushReturnTupleDummy(Args&&...) { }

template <class ...Args, size_t ...Indices>
inline void PushReturnTuple(lua_State* L, std::tuple<Args...> const& v, std::index_sequence<Indices...>)
{
	PushReturnTupleDummy((push<Args>(L, std::get<Indices>(v)), 0)...);
}

template <class ...Args>
inline void PushReturnValue(lua_State* L, std::tuple<Args...> v)
{
	return PushReturnTuple(L, v, std::index_sequence_for<Args...>());
}

template <class T>
inline int ReturnValueSize(Overload<T>)
{
	return 1;
}

template <class ...Args>
inline int ReturnValueSize(Overload<std::tuple<Args...>>)
{
	return sizeof...(Args);
}

// No return value, lua_State passed
template <class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, void (T::* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 0);
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	(obj->*fun)(L, checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	return 0;
}

// Return values pushed by callee, lua_State passed
template <class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, UserReturn (T::* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto nret = (obj->*fun)(L, checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	return (int)nret;
}

// 1 return value, lua_State passed
template <class R, class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, R (T::* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, ReturnValueSize(Overload<R>{}));
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto retval = (obj->*fun)(L, checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	PushReturnValue(L, retval);
	return ReturnValueSize(Overload<R>{});
}

// Index_sequence wrapper for CallMethodHelper(), lua_State passed
template <class R, class T, class ...Args>
inline int CallMethod(lua_State* L, R(T::* fun)(lua_State*, Args...)) {
	return CallMethodHelper(L, fun, std::index_sequence_for<Args...>());
}

// No return value, lua_State not passed
template <class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, void (T::* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 0);
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	(obj->*fun)(checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	return 0;
}

// 1 return value, lua_State not passed
template <class R, class T, class ...Args, size_t ...Indices>
inline int CallMethodHelper(lua_State* L, R (T::* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, ReturnValueSize(Overload<R>{}));
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto retval = (obj->*fun)(checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	PushReturnValue(L, retval);
	return ReturnValueSize(Overload<R>{});
}

// Index_sequence wrapper for CallMethodHelper(), lua_State not passed
template <class R, class T, class ...Args>
inline int CallMethod(lua_State* L, R (T::* fun)(Args...)) {
	return CallMethodHelper(L, fun, std::index_sequence_for<Args...>());
}


template <class R, class T, class ...Args, size_t ...Indices>
inline int CallGetterHelper(lua_State* L, R (T::* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, ReturnValueSize(Overload<R>{}));
	auto obj = ObjectProxy2::CheckedGet<T>(L, 1);
	auto retval = (obj->*fun)(checked_get_param_cv<Args>(L, 2 + (int)Indices)...);
	PushReturnValue(L, retval);
	return ReturnValueSize(Overload<R>{});
}

template <class R, class T, class ...Args>
inline int CallGetter(lua_State* L, R(T::* fun)(Args...)) {
	return CallGetterHelper(L, fun, std::index_sequence_for<Args...>());
}



// No return value, lua_State passed
template <class ...Args, size_t ...Indices>
inline int CallFunctionHelper(lua_State* L, void (* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 0);
	fun(L, checked_get_param_cv<Args>(L, 1 + (int)Indices)...);
	return 0;
}

// Return values pushed by callee, lua_State passed
template <class ...Args, size_t ...Indices>
inline int CallFunctionHelper(lua_State* L, UserReturn (* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	auto nret = fun(L, checked_get_param_cv<Args>(L, 1 + (int)Indices)...);
	return (int)nret;
}

// 1 return value, lua_State passed
template <class R, class ...Args, size_t ...Indices>
inline int CallFunctionHelper(lua_State* L, R (* fun)(lua_State*, Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, ReturnValueSize(Overload<R>{}));
	auto retval = fun(L, checked_get_param_cv<Args>(L, 1 + (int)Indices)...);
	PushReturnValue(L, retval);
	return ReturnValueSize(Overload<R>{});
}

// Index_sequence wrapper for CallFunctionHelper(), lua_State passed
template <class R, class ...Args>
inline int CallFunction(lua_State* L, R(* fun)(lua_State*, Args...)) {
	return CallFunctionHelper(L, fun, std::index_sequence_for<Args...>());
}

// No return value, lua_State not passed
template <class ...Args, size_t ...Indices>
inline int CallFunctionHelper(lua_State* L, void (* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, 0);
	fun(checked_get_param_cv<Args>(L, 1 + (int)Indices)...);
	return 0;
}

// 1 return value, lua_State not passed
template <class R, class ...Args, size_t ...Indices>
inline int CallFunctionHelper(lua_State* L, R (* fun)(Args...), std::index_sequence<Indices...>) {
	StackCheck _(L, ReturnValueSize(Overload<R>{}));
	auto retval = fun(checked_get_param_cv<Args>(L, 1 + (int)Indices)...);
	PushReturnValue(L, retval);
	return ReturnValueSize(Overload<R>{});
}

// Index_sequence wrapper for CallFunctionHelper(), lua_State not passed
template <class R, class ...Args>
inline int CallFunction(lua_State* L, R (* fun)(Args...)) {
	return CallFunctionHelper(L, fun, std::index_sequence_for<Args...>());
}

#define LuaWrapFunction(fun) [](lua_State* L) { return CallFunction(L, fun); }

END_NS()
