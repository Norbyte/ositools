#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

class GenericPropertyMap
{
public:
	using TFallbackGetter = bool(lua_State* L, LifetimeHolder const& lifetime, void* object, FixedString const& prop);
	using TFallbackSetter = bool(lua_State* L, LifetimeHolder const& lifetime, void* object, FixedString const& prop, int index);

	struct RawPropertyAccessors
	{
		using Getter = bool (lua_State* L, LifetimeHolder const& lifetime, void* object, std::size_t offset, uint64_t flag);
		using Setter = bool (lua_State* L, LifetimeHolder const& lifetime, void* object, int index, std::size_t offset, uint64_t flag);

		FixedString Name;
		Getter* Get;
		Setter* Set;
		std::size_t Offset;
		uint64_t Flag;
	};

	void Init();
	void Finish();
	bool GetRawProperty(lua_State* L, LifetimeHolder const& lifetime, void* object, FixedString const& prop) const;
	bool SetRawProperty(lua_State* L, LifetimeHolder const& lifetime, void* object, FixedString const& prop, int index) const;
	void AddRawProperty(char const* prop, typename RawPropertyAccessors::Getter* getter,
		typename RawPropertyAccessors::Setter* setter, std::size_t offset, uint64_t flag = 0);

	FixedString Name;
	std::unordered_map<FixedString, RawPropertyAccessors> Properties;
	std::vector<FixedString> Parents;
	TFallbackGetter* FallbackGetter{ nullptr };
	TFallbackSetter* FallbackSetter{ nullptr };
	bool IsInitializing{ false };
	bool Initialized{ false };
};

inline bool GenericSetNonWriteableProperty(lua_State* L, LifetimeHolder const& lifetime, void* obj, int index, std::size_t offset, uint64_t)
{
	return false;
}

template <class T>
class LuaPropertyMap : public GenericPropertyMap
{
public:
	struct PropertyAccessors
	{
		using Getter = bool (lua_State* L, LifetimeHolder const& lifetime, T* object, std::size_t offset, uint64_t flag);
		using Setter = bool (lua_State* L, LifetimeHolder const& lifetime, T* object, int index, std::size_t offset, uint64_t flag);
		using FallbackGetter = bool (lua_State* L, LifetimeHolder const& lifetime, T* object, FixedString const& prop);
		using FallbackSetter = bool (lua_State* L, LifetimeHolder const& lifetime, T* object, FixedString const& prop, int index);
	};

	inline bool GetProperty(lua_State* L, LifetimeHolder const& lifetime, T* object, FixedString const& prop) const
	{
#if defined(DEBUG_TRAP_GETTERS)
		__try {
			return GetRawProperty(L, lifetime, (void*)object, prop);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			ERR("Exception while reading property %s.%s", Name.GetString(), prop.GetString());
			return false;
		}
#else
		return GetRawProperty(L, lifetime, (void*)object, prop);
#endif
	}

	inline bool SetProperty(lua_State* L, LifetimeHolder const& lifetime, T* object, FixedString const& prop, int index) const
	{
#if defined(DEBUG_TRAP_GETTERS)
		__try {
			return SetRawProperty(L, lifetime, (void*)object, prop, index);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			ERR("Exception while writing property %s.%s", Name.GetString(), prop.GetString());
			return false;
		}
#else
		return SetRawProperty(L, lifetime, (void*)object, prop, index);
#endif
	}

	inline bool GetProperty(lua_State* L, LifetimeHolder const& lifetime, T* object, RawPropertyAccessors const& prop) const
	{
		auto getter = (typename PropertyAccessors::Getter*)prop.Get;

#if defined(DEBUG_TRAP_GETTERS)
		__try {
			return getter(L, lifetime, object, prop.Offset, prop.Flag);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			ERR("Exception while reading property %s.%s", Name.GetString(), prop.Name.GetString());
			return false;
		}
#else
		return getter(L, lifetime, object, prop.Offset, prop.Flag);
#endif
	}

	inline bool SetProperty(lua_State* L, LifetimeHolder const& lifetime, T* object, RawPropertyAccessors const& prop, int index) const
	{
		auto setter = (typename PropertyAccessors::Setter*)prop.Set;

#if defined(DEBUG_TRAP_GETTERS)
		__try {
			return setter(L, lifetime, object, index, prop.Offset, prop.Flag);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			ERR("Exception while writing property %s.%s", Name.GetString(), prop.Name.GetString());
			return false;
		}
#else
		return setter(L, lifetime, object, index, prop.Offset, prop.Flag);
#endif
	}

	inline void AddProperty(char const* prop, typename PropertyAccessors::Getter* getter,
		typename PropertyAccessors::Setter* setter = nullptr, std::size_t offset = 0, uint64_t flag = 0)
	{
		if (setter == nullptr) {
			setter = (typename PropertyAccessors::Setter*)&GenericSetNonWriteableProperty;
		}

		AddRawProperty(prop, (RawPropertyAccessors::Getter*)getter, (RawPropertyAccessors::Setter*)setter, offset, flag);
	}

	inline void SetFallback(PropertyAccessors::FallbackGetter* getter, PropertyAccessors::FallbackSetter* setter)
	{
		FallbackGetter = (TFallbackGetter*)getter;
		FallbackSetter = (TFallbackSetter*)setter;
	}
};

template <class T>
struct StaticLuaPropertyMap
{
	using ObjectType = T;
	using TPropertyMap = LuaPropertyMap<T>;

	static TPropertyMap PropertyMap;
};

END_NS()
