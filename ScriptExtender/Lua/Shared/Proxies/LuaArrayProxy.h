#pragma once

#include <span>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>
#include <Lua/Shared/Proxies/LuaCppClass.h>

BEGIN_SE()

BY_VAL(uint8_t);
BY_VAL(int16_t);
BY_VAL(uint16_t);
BY_VAL(int32_t);
BY_VAL(uint32_t);
BY_VAL(int64_t);
BY_VAL(uint64_t);
BY_VAL(float);
BY_VAL(double);
BY_VAL(bool);
BY_VAL(ComponentHandle);
BY_VAL(EntityHandle);
BY_VAL(TemplateHandle);
BY_VAL(FixedString);
BY_VAL(STDString);
BY_VAL(STDWString);
BY_VAL(Guid);
BY_VAL(Path);
BY_VAL(NetId);
BY_VAL(UserId);
BY_VAL(glm::ivec2);
BY_VAL(glm::vec2);
BY_VAL(glm::vec3);
BY_VAL(glm::vec4);
BY_VAL(glm::mat3);
BY_VAL(glm::mat3x4);
BY_VAL(glm::mat4x3);
BY_VAL(glm::mat4);
BY_VAL(TypeInformationRef);

END_SE()

BEGIN_NS(lua)

class ArrayProxyImplBase
{
public:
	ArrayProxyImplBase();
	inline virtual ~ArrayProxyImplBase() {};
	void Register();
	virtual int GetRegistryIndex() const;
	virtual TypeInformation const& GetArrayType() const = 0;
	virtual TypeInformation const& GetElementType() const = 0;
	virtual bool GetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex) = 0;
	virtual bool SetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex, int luaIndex) = 0;
	virtual int Next(lua_State* L, CppObjectMetadata& self, int key) = 0;
	virtual unsigned Length(CppObjectMetadata& self) = 0;

private:
	int registryIndex_{ -1 };
};


template <class TArray, class TElement>
class ArrayProxyByRefImpl : public ArrayProxyImplBase
{
public:
	TypeInformation const& GetElementType() const override
	{
		return GetTypeInfo<TElement>();
	}
	
	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TArray>();
	}

	bool GetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (arrayIndex > 0 && arrayIndex <= (unsigned)obj->size()) {
			MakeObjectRef(L, self.Lifetime, &(*obj)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex, int luaIndex) override
	{
		// Appending/swapping elements to by-ref arrays not supported for now
		return false;
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		return (unsigned)obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int key) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (key >= 0 && key < (int)obj->size()) {
			push(L, ++key);
			MakeObjectRef(L, self.Lifetime, &(*obj)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}
};


template <class TArray, class TElement>
class ArrayProxyByValImpl : public ArrayProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<TElement>, "ArrayProxyByValImpl template parameter should not be a pointer type!");

	TypeInformation const& GetElementType() const override
	{
		return GetTypeInfo<TElement>();
	}

	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TArray>();
	}

	bool GetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (arrayIndex > 0 && arrayIndex <= obj->size()) {
			LuaWrite(L, (*obj)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex, int luaIndex) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (arrayIndex > 0 && arrayIndex <= obj->size()) {
			lua_pushvalue(L, luaIndex);
			LuaRead(L, (*obj)[arrayIndex - 1]);
			lua_pop(L, 1);
			return true;
		} else if (arrayIndex == obj->size() + 1) {
			TElement val;
			lua_pushvalue(L, luaIndex);
			LuaRead(L, val);
			lua_pop(L, 1);

			obj->push_back(val);
			return true;
		} else {
			return false;
		}
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		return (unsigned)obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int key) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (key >= 0 && key < (int)obj->size()) {
			push(L, ++key);
			LuaWrite(L, (*obj)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}
};


template <class TArray, class TElement>
class FixedSizeArrayProxyByRefImpl : public ArrayProxyImplBase
{
public:
	TypeInformation const& GetElementType() const override
	{
		return GetTypeInfo<TElement>();
	}

	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TArray>();
	}

	bool GetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (arrayIndex > 0 && arrayIndex <= (int)obj->size()) {
			MakeObjectRef(L, self.Lifetime, &(*obj)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex, int luaIndex) override
	{
		// Appending/swapping elements to by-ref arrays not supported for now
		return false;
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		return (unsigned)obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int key) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (key >= 0 && key < (int)obj->size()) {
			push(L, ++key);
			MakeObjectRef(L, self.Lifetime, &(*obj)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}
};


template <class TArray, class TElement>
class FixedSizeArrayProxyByValImpl : public ArrayProxyImplBase
{
public:
	static_assert(!std::is_pointer_v<TElement>, "FixedSizeArrayProxyByValImpl template parameter should not be a pointer type!");

	TypeInformation const& GetElementType() const override
	{
		return GetTypeInfo<TElement>();
	}

	TypeInformation const& GetArrayType() const override
	{
		return GetTypeInfo<TArray>();
	}

	bool GetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (arrayIndex > 0 && arrayIndex <= obj->size()) {
			LuaWrite(L, (*obj)[arrayIndex - 1]);
			return true;
		} else {
			return false;
		}
	}

	bool SetElement(lua_State* L, CppObjectMetadata& self, unsigned arrayIndex, int luaIndex) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (arrayIndex > 0 && arrayIndex <= obj->size()) {
			lua_pushvalue(L, luaIndex);
			LuaRead(L, (*obj)[arrayIndex - 1]);
			lua_pop(L, 1);
			return true;
		} else {
			// Cannot append elements to a gsl::span
			return false;
		}
	}

	unsigned Length(CppObjectMetadata& self) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		return (unsigned)obj->size();
	}

	int Next(lua_State* L, CppObjectMetadata& self, int key) override
	{
		auto obj = reinterpret_cast<TArray*>(self.Ptr);
		if (key >= 0 && key < (int)obj->size()) {
			push(L, ++key);
			LuaWrite(L, (*obj)[key - 1]);
			return 2;
		} else {
			return 0;
		}
	}
};


class ArrayProxyMetatable : public LightCppObjectMetatable<ArrayProxyMetatable>, public Indexable, public NewIndexable,
	public Lengthable, public Iterable, public Stringifiable, public EqualityComparable
{
public:
	static constexpr MetatableTag MetaTag = MetatableTag::ArrayProxy;

	template <class TImpl>
	static ArrayProxyImplBase* GetImplementation()
	{
		static ArrayProxyImplBase* impl = new TImpl();
		return impl;
	}

	inline static void MakeImpl(lua_State* L, void* object, LifetimeHandle const& lifetime, ArrayProxyImplBase* impl)
	{
		lua_push_cppobject(L, MetatableTag::ArrayProxy, impl->GetRegistryIndex(), object, lifetime);
	}

	template <class T>
	inline static auto MakeByRef(lua_State* L, Array<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByRefImpl<Array<T>, T>>());
	}

	template <class T>
	inline static auto MakeByRef(lua_State* L, Vector<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByRefImpl<Vector<T>, T>>());
	}

	template <class T>
	inline static auto MakeByRef(lua_State* L, std::span<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<FixedSizeArrayProxyByRefImpl<std::span<T>, T>>());
	}

	template <class T, class Allocator, bool StoreSize>
	inline static auto MakeByRef(lua_State* L, Set<T, Allocator, StoreSize>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByRefImpl<Set<T, Allocator, StoreSize>, T>>());
	}

	template <class T, class TSizeType, class Allocator>
	inline static auto MakeByRef(lua_State* L, PrimitiveSmallSet<T, TSizeType, Allocator>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByRefImpl<PrimitiveSmallSet<T, TSizeType, Allocator>, T>>());
	}

	template <class T>
	inline static auto MakeByRef(lua_State* L, Queue<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByRefImpl<Queue<T>, T>>());
	}

	template <class T, int Size>
	inline static auto MakeByRef(lua_State* L, std::array<T, Size>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<FixedSizeArrayProxyByRefImpl<std::array<T, Size>, T>>());
	}

	template <class T>
	inline static auto MakeByVal(lua_State* L, Array<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByValImpl<Array<T>, T>>());
	}

	template <class T>
	inline static auto MakeByVal(lua_State* L, Vector<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByValImpl<Vector<T>, T>>());
	}

	template <class T>
	inline static auto MakeByVal(lua_State* L, std::span<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<FixedSizeArrayProxyByValImpl<std::span<T>, T>>());
	}

	template <class T, class Allocator, bool StoreSize>
	inline static auto MakeByVal(lua_State* L, Set<T, Allocator, StoreSize>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByValImpl<Set<T, Allocator, StoreSize>, T>>());
	}

	template <class T, class TSizeType, class Allocator>
	inline static auto MakeByVal(lua_State* L, PrimitiveSmallSet<T, TSizeType, Allocator>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByValImpl<PrimitiveSmallSet<T, TSizeType, Allocator>, T>>());
	}

	template <class T>
	inline static auto MakeByVal(lua_State* L, Queue<T>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<ArrayProxyByValImpl<Queue<T>, T>>());
	}

	template <class T, int Size>
	inline static auto MakeByVal(lua_State* L, std::array<T, Size>* object, LifetimeHandle const& lifetime)
	{
		MakeImpl(L, object, lifetime, GetImplementation<FixedSizeArrayProxyByValImpl<std::array<T, Size>, T>>());
	}

	static int Index(lua_State* L, CppObjectMetadata& self);
	static int NewIndex(lua_State* L, CppObjectMetadata& self);
	static int Length(lua_State* L, CppObjectMetadata& self);
	static int Next(lua_State* L, CppObjectMetadata& self);
	static int ToString(lua_State* L, CppObjectMetadata& self);
	static bool IsEqual(lua_State* L, CppObjectMetadata& self, CppObjectMetadata& other);
	static char const* GetTypeName(lua_State* L, CppObjectMetadata& self);
};

template <class T>
struct IsArrayLike { static constexpr bool Value = false; };

template <class T>
struct IsArrayLike<Array<T>> { static constexpr bool Value = true; using TElement = T; };

template <class T>
struct IsArrayLike<Vector<T>> { static constexpr bool Value = true; using TElement = T; };

template <class T>
struct IsArrayLike<std::span<T>> { static constexpr bool Value = true; using TElement = T; };

template <class T, class Allocator, bool StoreSize>
struct IsArrayLike<Set<T, Allocator, StoreSize>> { static constexpr bool Value = true; using TElement = T; };

template <class T, class Allocator, bool StoreSize>
struct IsArrayLike<ObjectSet<T, Allocator, StoreSize>> { static constexpr bool Value = true; using TElement = T; };

template <class T, class TSizeType, class Allocator>
struct IsArrayLike<PrimitiveSmallSet<T, TSizeType, Allocator>> { static constexpr bool Value = true; using TElement = T; };

template <class T>
struct IsArrayLike<Queue<T>> { static constexpr bool Value = true; using TElement = T; };

template <class T, size_t Size>
struct IsArrayLike<std::array<T, Size>> { static constexpr bool Value = true; using TElement = T; };

template <class T>
inline void push_array_ref_proxy(lua_State* L, LifetimeHandle const& lifetime, T* v)
{
	ArrayProxyMetatable::MakeByRef<T>(L, v, lifetime);
}

void* checked_get_array_proxy(lua_State* L, int index, int propertyMapIndex);

template <class T>
inline T* checked_get_array_proxy(lua_State* L, int index)
{
	auto ptr = checked_get_array_proxy(L, index, ArrayProxyMetatable::GetImplementation<T>()->GetRegistryIndex());
	return reinterpret_cast<T*>(ptr);
}

END_NS()
