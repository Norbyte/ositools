// Shared container for all property lists

#include <stdafx.h>

#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Symbols.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Lua/Shared/Proxies/LuaPropertyMapHelpers.h>
#include <Lua/Shared/Proxies/LuaEvent.h>
#include <Lua/Server/LuaBindingServer.h>
#include <Lua/Client/LuaBindingClient.h>

#include <GameDefinitions/GameObjects/Ai.h>
#include <GameDefinitions/GameObjects/Surface.h>
#include <GameDefinitions/Components/Trigger.h>
#include <Hit.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(lua)

void CopyRawProperties(GenericPropertyMap const& base, GenericPropertyMap& child, STDString const& baseClsName)
{
	for (auto const& prop : base.Properties) {
		child.AddRawProperty(prop.first.GetString(), prop.second.Get, prop.second.Set, prop.second.Offset);
	}

	for (auto const& parent : base.Parents) {
		child.Parents.push_back(parent);
	}

	child.Parents.push_back(FixedString(baseClsName));
}

template <class T>
void AddBitmaskProperty(GenericPropertyMap& pm, std::size_t offset)
{
	for (auto const& label : EnumInfo<T>::Values) {
		pm.AddRawProperty(label.Key.GetString(),
			&(GenericGetOffsetBitmaskFlag<T>),
			&(GenericSetOffsetBitmaskFlag<T>),
			offset,
			(uint64_t)label.Value
		);
	}
}

bool CharacterSetFlag(lua_State* L, LifetimeHolder const& lifetime, void* obj, int index, std::size_t offset, uint64_t flag)
{
	auto ch = reinterpret_cast<esv::Character*>(obj);
	auto set = get<bool>(L, index);
	if (set) {
		ch->SetFlags(flag);
	} else {
		ch->ClearFlags(flag);
	}

	return true;
}

// Lua property map and object proxy template specialization declarations

#define BEGIN_CLS(name) LuaPropertyMap<name> StaticLuaPropertyMap<name>::PropertyMap;
#define BEGIN_CLS_TN(name, typeName) LuaPropertyMap<name> StaticLuaPropertyMap<name>::PropertyMap;

#define END_CLS()
#define INHERIT(base)
#define P(prop)
#define P_RO(prop)
#define P_REF(prop)
#define P_BITMASK(prop)
#define PN(name, prop)
#define PN_RO(name, prop)
#define PN_REF(name, prop)
#define P_GETTER(prop, fun)
#define P_FUN(prop, fun)
#define P_FALLBACK(getter, setter)

#include <GameDefinitions/PropertyMaps/AllPropertyMaps.inl>

#undef BEGIN_CLS
#undef BEGIN_CLS_TN
#undef END_CLS
#undef INHERIT
#undef P
#undef P_RO
#undef P_REF
#undef P_BITMASK
#undef PN
#undef PN_RO
#undef PN_REF
#undef P_GETTER
#undef P_FUN
#undef P_FALLBACK

#if defined(DEBUG)
bool EnableWriteProtectedWrites{ true };
#else
bool EnableWriteProtectedWrites{ false };
#endif

// Property registrations

	void InitObjectProxyPropertyMaps()
	{
		static bool initialized{ false };
		if (initialized) return;

#define GENERATING_PROPMAP

#define BEGIN_CLS(cls) { \
	using PM = StaticLuaPropertyMap<cls>; \
	auto& pm = StaticLuaPropertyMap<cls>::PropertyMap; \
	pm.Init(); \
	pm.Name = FixedString(#cls);

#define BEGIN_CLS_TN(cls, typeName) { \
	using PM = StaticLuaPropertyMap<cls>; \
	auto& pm = StaticLuaPropertyMap<cls>::PropertyMap; \
	pm.Init(); \
	pm.Name = FixedString(#typeName);

#define END_CLS() pm.Finish(); \
	}

#define INHERIT(base) { \
		auto& basePm = StaticLuaPropertyMap<base>::PropertyMap; \
		InheritProperties(basePm, pm, #base); \
	}

#define P(prop) \
	pm.AddRawProperty(#prop, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define P_RO(prop) \
	pm.AddRawProperty(#prop, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSetOffsetWriteProtectedProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define P_REF(prop) \
	pm.AddRawProperty(#prop, \
		&(GenericGetOffsetRefProperty<decltype(PM::ObjectType::prop)>), \
		&GenericSetNonWriteableProperty, \
		offsetof(PM::ObjectType, prop) \
	);

#define P_BITMASK(prop) AddBitmaskProperty<decltype(PM::ObjectType::prop)>(pm, offsetof(PM::ObjectType, prop));

#define PN(name, prop) \
	pm.AddRawProperty(#name, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define PN_RO(name, prop) \
	pm.AddRawProperty(#name, \
		&(GenericGetOffsetProperty<decltype(PM::ObjectType::prop)>), \
		&(GenericSetOffsetWriteProtectedProperty<decltype(PM::ObjectType::prop)>), \
		offsetof(PM::ObjectType, prop) \
	);

#define PN_REF(name, prop) \
	pm.AddRawProperty(#name, \
		&(GenericGetOffsetRefProperty<decltype(PM::ObjectType::prop)>), \
		&GenericSetNonWriteableProperty, \
		offsetof(PM::ObjectType, prop) \
	);

// FIXME - avoid generating a separate push function for each closure
#define P_GETTER(name, fun) \
	pm.AddProperty(#name, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, std::size_t offset, uint64_t flag) { \
			CallGetter(L, &PM::ObjectType::fun); \
			return true; \
		}, \
		(PM::TPropertyMap::PropertyAccessors::Setter*)&GenericSetNonWriteableProperty, 0 \
	);

// FIXME - avoid generating a separate push function for each closure
#define P_FUN(name, fun) \
	pm.AddProperty(#name, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, std::size_t offset, uint64_t flag) { \
			lua_pushcfunction(L, [](lua_State* L) -> int { \
				return CallMethod(L, &PM::ObjectType::fun); \
			}); \
			return true; \
		}, \
		(PM::TPropertyMap::PropertyAccessors::Setter*)&GenericSetNonWriteableProperty, 0 \
	);

#define P_FALLBACK(getter, setter) pm.SetFallback(getter, setter);

#include <GameDefinitions/PropertyMaps/AllPropertyMaps.inl>

#undef GENERATING_PROPMAP
#undef BEGIN_CLS
#undef BEGIN_CLS_TN
#undef END_CLS
#undef INHERIT
#undef P
#undef P_RO
#undef P_REF
#undef P_BITMASK
#undef PN
#undef PN_RO
#undef PN_REF
#undef P_GETTER
#undef P_FUN
#undef P_FALLBACK

		initialized = true;
	}
END_NS()

BEGIN_SE()

#define BEGIN_CLS(name) char const* const TypeInfo<name>::TypeName = #name;
#define BEGIN_CLS_TN(name, typeName) char const* const TypeInfo<name>::TypeName = #typeName;
#define END_CLS()
#define INHERIT(base)
#define P(prop)
#define P_RO(prop)
#define P_REF(prop)
#define P_BITMASK(prop)
#define PN(name, prop)
#define PN_RO(name, prop)
#define PN_REF(name, prop)
#define P_GETTER(prop, fun)
#define P_FUN(prop, fun)
#define P_FALLBACK(getter, setter)

#include <GameDefinitions/PropertyMaps/AllPropertyMaps.inl>

#undef BEGIN_CLS
#undef BEGIN_CLS_TN
#undef END_CLS
#undef INHERIT
#undef P
#undef P_RO
#undef P_REF
#undef P_BITMASK
#undef PN
#undef PN_RO
#undef PN_REF
#undef P_GETTER
#undef P_FUN
#undef P_FALLBACK


#define BEGIN_BITMASK_NS(NS, T, type) char const* const TypeInfo<NS::T>::TypeName = #T;
#define BEGIN_ENUM_NS(NS, T, type) char const* const TypeInfo<NS::T>::TypeName = #T;
#define BEGIN_BITMASK(T, type) char const* const TypeInfo<T>::TypeName = #T;
#define BEGIN_ENUM(T, type) char const* const TypeInfo<T>::TypeName = #T;
#define E(label)
#define EV(label, value)
#define END_ENUM_NS()
#define END_ENUM()
#include <GameDefinitions/Enumerations.inl>
#undef BEGIN_BITMASK_NS
#undef BEGIN_ENUM_NS
#undef BEGIN_BITMASK
#undef BEGIN_ENUM
#undef E
#undef EV
#undef END_ENUM_NS
#undef END_ENUM

char const* const TypeInfo<ig::InvokeDataValue>::TypeName = "ig::InvokeDataValue";
char const* const TypeInfo<ObjectSet<stats::PropertyStatus*>>::TypeName = "ObjectSet<PropertyStatus>";
char const* const TypeInfo<ObjectSet<FixedString>>::TypeName = "ObjectSet<FixedString>";
char const* const TypeInfo<ObjectSet<uint32_t>>::TypeName = "ObjectSet<uint32_t>";

END_SE()
