#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>
#include <Lua/Shared/Proxies/LuaPropertyMap.h>

BEGIN_NS(lua)

LifetimeHandle LifetimeFromState(lua_State* L);
LifetimeHandle GlobalLifetimeFromState(lua_State* L);

class ObjectProxyImplBase
{
public:
	inline virtual ~ObjectProxyImplBase() {};
	virtual FixedString const& GetTypeName() const = 0;
	virtual void* GetRaw(lua_State* L) = 0;
	virtual bool GetProperty(lua_State* L, FixedString const& prop) = 0;
	virtual bool SetProperty(lua_State* L, FixedString const& prop, int index) = 0;
	virtual int Next(lua_State* L, FixedString const& key) = 0;
	virtual bool IsA(FixedString const& typeName) = 0;
};

template <class T>
struct ObjectProxyHelpers
{
	static bool GetProperty(lua_State* L, T* object, LifetimeHandle const& lifetime, FixedString const& prop)
	{
		auto const& map = StaticLuaPropertyMap<T>::PropertyMap;
		auto result = map.GetProperty(L, lifetime, object, prop);
		switch (result) {
		case PropertyOperationResult::Success:
			break;

		case PropertyOperationResult::NoSuchProperty:
			luaL_error(L, "Property does not exist: %s::%s - property does not exist", GetTypeInfo<T>().TypeName.GetString(), prop.GetString());
			push(L, nullptr);
			break;

		case PropertyOperationResult::Unknown:
		default:
			luaL_error(L, "Cannot get property %s::%s - unknown error", GetTypeInfo<T>().TypeName.GetString(), prop.GetString());
			push(L, nullptr);
			break;
		}

		return true;
	}

	static bool SetProperty(lua_State* L, T* object, LifetimeHandle const& lifetime, FixedString const& prop, int index)
	{
		auto const& map = StaticLuaPropertyMap<T>::PropertyMap;
		auto result = map.SetProperty(L, lifetime, object, prop, index);
		switch (result) {
		case PropertyOperationResult::Success:
			return true;

		case PropertyOperationResult::NoSuchProperty:
			luaL_error(L, "Cannot set property %s::%s - property does not exist", GetTypeInfo<T>().TypeName.GetString(), prop.GetString());
			return false;

		case PropertyOperationResult::ReadOnly:
			luaL_error(L, "Cannot set property %s::%s - property is read-only", GetTypeInfo<T>().TypeName.GetString(), prop.GetString());
			return false;

		case PropertyOperationResult::UnsupportedType:
			luaL_error(L, "Cannot set property %s::%s - cannot write properties of this type", GetTypeInfo<T>().TypeName.GetString(), prop.GetString());
			return false;

		case PropertyOperationResult::Unknown:
		default:
			luaL_error(L, "Cannot set property %s::%s - unknown error", GetTypeInfo<T>().TypeName.GetString(), prop.GetString());
			return false;
		}
	}

	static int Next(lua_State* L, T* object, LifetimeHandle const& lifetime, FixedString const& key)
	{
		auto const& map = StaticLuaPropertyMap<T>::PropertyMap;
		if (!key) {
			if (!map.Properties.empty()) {
				StackCheck _(L, 2);
				auto it = map.Properties.begin();
				push(L, it->first);
				if (map.GetProperty(L, lifetime, object, it->second) != PropertyOperationResult::Success) {
					push(L, nullptr);
				}

				return 2;
			}
		} else {
			auto it = map.Properties.find(key);
			if (it != map.Properties.end()) {
				++it;
				if (it != map.Properties.end()) {
					StackCheck _(L, 2);
					push(L, it->first);
					if (map.GetProperty(L, lifetime, object, it->second) != PropertyOperationResult::Success) {
						push(L, nullptr);
					}

					return 2;
				}
			}
		}

		return 0;
	}

	static bool IsA(FixedString const& typeName)
	{
		auto const& map = StaticLuaPropertyMap<T>::PropertyMap;
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

template <class T>
class ObjectProxyRefImpl : public ObjectProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<T>, "ObjectProxyImpl template parameter should not be a pointer type!");
	static_assert(!std::is_const_v<T>, "ObjectProxyImpl template parameter should not be CV-qualified!");
	static_assert(!std::is_volatile_v<T>, "ObjectProxyImpl template parameter should not be CV-qualified!");

	ObjectProxyRefImpl(LifetimeHandle const& lifetime, T * obj)
		: object_(obj), lifetime_(lifetime)
	{
		assert(obj != nullptr);
	}
		
	~ObjectProxyRefImpl() override
	{}

	T* Get() const
	{
		return object_;
	}

	void* GetRaw(lua_State* L) override
	{
		return object_;
	}

	FixedString const& GetTypeName() const override
	{
		return StaticLuaPropertyMap<T>::PropertyMap.Name;
	}

	bool GetProperty(lua_State* L, FixedString const& prop) override
	{
		return ObjectProxyHelpers<T>::GetProperty(L, object_, lifetime_, prop);
	}

	bool SetProperty(lua_State* L, FixedString const& prop, int index) override
	{
		return ObjectProxyHelpers<T>::SetProperty(L, object_, lifetime_, prop, index);
	}

	int Next(lua_State* L, FixedString const& key) override
	{
		return ObjectProxyHelpers<T>::Next(L, object_, lifetime_, key);
	}

	bool IsA(FixedString const& typeName) override
	{
		return ObjectProxyHelpers<T>::IsA(typeName);
	}

private:
	T* object_;
	LifetimeHandle lifetime_;
};

template <class T>
class ObjectProxyHandleBasedRefImpl : public ObjectProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<T>, "ObjectProxyImpl template parameter should not be a pointer type!");
	static_assert(!std::is_const_v<T>, "ObjectProxyImpl template parameter should not be CV-qualified!");
	static_assert(!std::is_volatile_v<T>, "ObjectProxyImpl template parameter should not be CV-qualified!");

	ObjectProxyHandleBasedRefImpl(LifetimeHandle const& containerLifetime, ComponentHandle handle, LifetimeHandle const& lifetime)
		: handle_(handle), containerLifetime_(containerLifetime), lifetime_(lifetime)
	{}

	~ObjectProxyHandleBasedRefImpl() override
	{}

	T* Get(lua_State* L) const;

	void* GetRaw(lua_State* L) override
	{
		return Get(L);
	}

	FixedString const& GetTypeName() const override
	{
		return StaticLuaPropertyMap<T>::PropertyMap.Name;
	}

	bool GetProperty(lua_State* L, FixedString const& prop) override
	{
		auto object = Get(L);
		if (!object) return false;
		return ObjectProxyHelpers<T>::GetProperty(L, object, LifetimeFromState(L), prop);
	}

	bool SetProperty(lua_State* L, FixedString const& prop, int index) override
	{
		auto object = Get(L);
		if (!object) return false;
		return ObjectProxyHelpers<T>::SetProperty(L, object, LifetimeFromState(L), prop, index);
	}

	int Next(lua_State* L, FixedString const& key) override
	{
		auto object = Get(L);
		if (!object) return 0;
		return ObjectProxyHelpers<T>::Next(L, object, LifetimeFromState(L), key);
	}

	bool IsA(FixedString const& typeName) override
	{
		return ObjectProxyHelpers<T>::IsA(typeName);
	}

private:
	ComponentHandle handle_;
	LifetimeHandle containerLifetime_;
	LifetimeHandle lifetime_;
};

// Object proxy that owns the contained object and deletes the object on GC
template <class T>
class ObjectProxyOwnerImpl : public ObjectProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<T>, "ObjectProxyImpl template parameter should not be a pointer type!");

	ObjectProxyOwnerImpl(lua_State* L, LifetimeHandle const& lifetime, T* obj)
		: lifetime_(L, lifetime), 
		object_(obj, &GameDelete<T>)
	{
		assert(obj != nullptr);
	}

	~ObjectProxyOwnerImpl() override
	{
		lifetime_.GetLifetime()->Kill();
	}

	T* Get() const
	{
		return object_.get();
	}

	void* GetRaw(lua_State* L) override
	{
		return object_.get();
	}

	FixedString const& GetTypeName() const override
	{
		return StaticLuaPropertyMap<T>::PropertyMap.Name;
	}

	bool GetProperty(lua_State* L, FixedString const& prop) override
	{
		return ObjectProxyHelpers<T>::GetProperty(L, object_.get(), lifetime_.Get(), prop);
	}

	bool SetProperty(lua_State* L, FixedString const& prop, int index) override
	{
		return ObjectProxyHelpers<T>::SetProperty(L, object_.get(), lifetime_.Get(), prop, index);
	}

	int Next(lua_State* L, FixedString const& key) override
	{
		return ObjectProxyHelpers<T>::Next(L, object_.get(), lifetime_.Get(), key);
	}

	bool IsA(FixedString const& typeName) override
	{
		return ObjectProxyHelpers<T>::IsA(typeName);
	}

private:
	GameUniquePtr<T> object_;
	LifetimeOwnerPin lifetime_;
};


// Object proxy that contains the object (i.e. the object is allocated next to the proxy impl in the Lua heap)
template <class T>
class ObjectProxyContainerImpl : public ObjectProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<T>, "ObjectProxyImpl template parameter should not be a pointer type!");

	template <class... Args>
	ObjectProxyContainerImpl(lua_State* L, LifetimeHandle const& lifetime, Args... args)
		: lifetime_(L, lifetime),
		object_(args...)
	{}

	~ObjectProxyContainerImpl() override
	{}

	T* Get() const
	{
		return &object_;
	}

	void* GetRaw(lua_State* L) override
	{
		return &object_;
	}

	FixedString const& GetTypeName() const override
	{
		return StaticLuaPropertyMap<T>::PropertyMap.Name;
	}

	bool GetProperty(lua_State* L, FixedString const& prop) override
	{
		return ObjectProxyHelpers<T>::GetProperty(L, &object_, lifetime_, prop);
	}

	bool SetProperty(lua_State* L, FixedString const& prop, int index) override
	{
		return ObjectProxyHelpers<T>::SetProperty(L, &object_, lifetime_, prop, index);
	}

	int Next(lua_State* L, FixedString const& key) override
	{
		return ObjectProxyHelpers<T>::Next(L, &object_, lifetime_, key);
	}

	bool IsA(FixedString const& typeName) override
	{
		return ObjectProxyHelpers<T>::IsA(typeName);
	}

private:
	LifetimeOwnerPin lifetime_;
	T object_;
};


class ObjectProxy2 : private Userdata<ObjectProxy2>, public Indexable, public NewIndexable,
	public Iterable, public Stringifiable, public EqualityComparable
{
public:
	static char const * const MetatableName;

	template <class TImpl, class T, class... Args>
	inline static TImpl* MakeImpl(lua_State* L, T* object, LifetimeHandle const& lifetime, Args... args)
	{
		auto self = NewWithExtraData(L, sizeof(TImpl), lifetime);
		return new (self->GetImpl()) TImpl(lifetime, object, args...);
	}

	template <class T>
	inline static ObjectProxyRefImpl<T>* MakeRef(lua_State* L, T* object, LifetimeHandle const& lifetime)
	{
		auto self = NewWithExtraData(L, sizeof(ObjectProxyRefImpl<T>), lifetime);
		return new (self->GetImpl()) ObjectProxyRefImpl<T>(lifetime, object);
	}

	template <class T>
	inline static ObjectProxyOwnerImpl<T>* MakeOwner(lua_State* L, LifetimePool& pool, T* obj)
	{
		auto lifetime = pool.Allocate();
		auto self = NewWithExtraData(L, sizeof(ObjectProxyOwnerImpl<T>), lifetime);
		return new (self->GetImpl()) ObjectProxyOwnerImpl<T>(L, lifetime, obj);
	}

	template <class T, class... Args>
	inline static ObjectProxyContainerImpl<T>* MakeContainer(lua_State* L, LifetimePool& pool, Args... args)
	{
		auto lifetime = pool.Allocate();
		auto self = NewWithExtraData(L, sizeof(ObjectProxyContainerImpl<T>), lifetime);
		return new (self->GetImpl()) ObjectProxyContainerImpl<T>(L, lifetime, args...);
	}

	template <class T>
	inline static ObjectProxyHandleBasedRefImpl<T>* MakeHandle(lua_State* L, ComponentHandle handle, LifetimeHandle const& lifetime)
	{
		auto self = NewWithExtraData(L, sizeof(ObjectProxyHandleBasedRefImpl<T>), GlobalLifetimeFromState(L));
		return new (self->GetImpl()) ObjectProxyHandleBasedRefImpl<T>(GlobalLifetimeFromState(L), handle, lifetime);
	}

	static void* CheckedGetRaw(lua_State* L, int index, FixedString const& typeName);
	static void* TryGetRaw(lua_State* L, int index, FixedString const& typeName);

	template <class T>
	inline static T* CheckedGet(lua_State* L, int index)
	{
		auto const& typeName = StaticLuaPropertyMap<T>::PropertyMap.Name;
		auto obj = CheckedGetRaw(L, index, typeName);
		return reinterpret_cast<T *>(obj);
	}

	template <class T>
	inline static T* TryGet(lua_State* L, int index)
	{
		auto const& typeName = StaticLuaPropertyMap<T>::PropertyMap.Name;
		auto obj = TryGetRaw(L, index, typeName);
		return reinterpret_cast<T *>(obj);
	}

	inline ObjectProxyImplBase* GetImpl()
	{
		return reinterpret_cast<ObjectProxyImplBase*>(this + 1);
	}

	inline bool IsAlive(lua_State* L) const
	{
		return lifetime_.IsAlive(L);
	}

	inline void* GetRaw(lua_State* L)
	{
		if (!lifetime_.IsAlive(L)) {
			return nullptr;
		}

		return GetImpl()->GetRaw(L);
	}

	template <class T>
	T* Get(lua_State* L)
	{
		if (!lifetime_.IsAlive(L)) {
			return nullptr;
		}

		if (GetImpl()->IsA(StaticLuaPropertyMap<T>::PropertyMap.Name)) {
			return reinterpret_cast<T*>(GetImpl()->GetRaw(L));
		} else {
			return nullptr;
		}
	}

private:
	LifetimeHandle lifetime_;

	ObjectProxy2(LifetimeHandle const& lifetime)
		: lifetime_(lifetime)
	{}

	~ObjectProxy2()
	{
		GetImpl()->~ObjectProxyImplBase();
	}

protected:
	friend Userdata<ObjectProxy2>;

	int Index(lua_State* L);
	int NewIndex(lua_State* L);
	int Next(lua_State* L);
	int ToString(lua_State* L);
	bool IsEqual(lua_State* L, ObjectProxy2* other);
};

END_NS()
