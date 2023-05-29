#pragma once

#include <cstdint>

#define BEGIN_SE() namespace dse {
#define END_SE() }

#define BEGIN_NS(ns) namespace dse::ns {
#define END_NS() }

BEGIN_SE()

// Helper struct to allow function overloading without (real) template-dependent parameters
template <class>
struct Overload {};

// Base class for game objects that cannot be copied.
template <class T>
class Noncopyable
{
public:
	inline Noncopyable() {}

	Noncopyable(const Noncopyable &) = delete;
	T & operator = (const T &) = delete;
	Noncopyable(Noncopyable &&) = delete;
	T & operator = (T &&) = delete;
};

// Base class for game objects that are managed entirely
// by the game and we cannot create/copy them.
template <class T>
class ProtectedGameObject
{
public:
	ProtectedGameObject(const ProtectedGameObject &) = delete;
	T & operator = (const T &) = delete;
	ProtectedGameObject(ProtectedGameObject &&) = delete;
	T & operator = (T &&) = delete;

protected:
	ProtectedGameObject() = delete;
	//~ProtectedGameObject() = delete;
};

// Tag for engine objects that have a Lua property map
struct HasObjectProxy {};

// Base class for game objects that are managed entirely
// by the game and we cannot create/copy them.
// Temporary hack until we have a better fix for HasObjectProxy
template <class T>
class ProtectedProxyGameObject : public HasObjectProxy
{
public:
	ProtectedProxyGameObject(const ProtectedProxyGameObject&) = delete;
	T& operator = (const T&) = delete;
	ProtectedProxyGameObject(ProtectedProxyGameObject&&) = delete;
	T& operator = (T&&) = delete;

protected:
	ProtectedProxyGameObject() = delete;
	//~ProtectedProxyGameObject() = delete;
};

template <class T>
struct HasObjectProxyTag {
	static constexpr bool HasProxy = false;
};

#define HAS_OBJECT_PROXY(cls) template<> struct HasObjectProxyTag<cls> { static constexpr bool HasProxy = true; }

// Tag indicating whether a specific type should be handled as a value (by-val) 
// or as an object via an object/array proxy (by-ref)
template <class T>
struct ByVal {
	static constexpr bool Value = std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_enum_v<T>;
};

#define BY_VAL(cls) template<> struct ByVal<cls> { static constexpr bool Value = true; }

// Prevents implicit casting between aliases of integral types (eg. NetId and UserId)
// Goal is to prevent accidental mixups between different types
template <class TValue, class Tag>
class TypedIntegral
{
public:
	using ValueType = TValue;

	inline constexpr TypedIntegral() : value_() {}
	inline constexpr TypedIntegral(TypedIntegral<TValue, Tag> const& t) : value_(t.value_) {}
	inline constexpr TypedIntegral(TValue const& val) : value_(val) {}

	inline TypedIntegral<TValue, Tag>& operator = (TValue const& val)
	{
		value_ = val;
		return *this;
	}

	inline TypedIntegral<TValue, Tag>& operator = (TypedIntegral<TValue, Tag> const& val)
	{
		value_ = val.value_;
		return *this;
	}

	inline constexpr explicit operator TValue () const
	{
		return value_;
	}

	inline constexpr bool operator == (TypedIntegral<TValue, Tag> const& val) const
	{
		return value_ == val.value_;
	}

	inline constexpr bool operator == (TValue const& val) const
	{
		return value_ == val;
	}

	inline constexpr bool operator != (TypedIntegral<TValue, Tag> const& val) const
	{
		return value_ != val.value_;
	}

	inline constexpr bool operator != (TValue const& val) const
	{
		return value_ != val;
	}

	inline constexpr bool operator <= (TypedIntegral<TValue, Tag> const& val) const
	{
		return value_ <= val.value_;
	}

	inline constexpr bool operator <= (TValue const& val) const
	{
		return value_ <= val;
	}

	inline constexpr bool operator >= (TypedIntegral<TValue, Tag> const& val) const
	{
		return value_ >= val.value_;
	}

	inline constexpr bool operator >= (TValue const& val) const
	{
		return value_ >= val;
	}

	inline constexpr bool operator < (TypedIntegral<TValue, Tag> const& val) const
	{
		return value_ < val.value_;
	}

	inline constexpr bool operator < (TValue const& val) const
	{
		return value_ < val;
	}

	inline constexpr bool operator > (TypedIntegral<TValue, Tag> const& val) const
	{
		return value_ > val.value_;
	}

	inline constexpr bool operator > (TValue const& val) const
	{
		return value_ > val;
	}

	inline constexpr TValue Value() const
	{
		return value_;
	}

	friend std::ostream& operator << (std::ostream& os, TypedIntegral<TValue, Tag> const& v)
	{
		return os << (int64_t)v.value_;
	}

private:
	TValue value_;
};



inline uint64_t Hash(uint8_t v)
{
	return v;
}

inline uint64_t Hash(uint16_t v)
{
	return v;
}

inline uint64_t Hash(uint32_t v)
{
	return v;
}

inline uint64_t Hash(int32_t v)
{
	return v;
}

inline uint64_t Hash(uint64_t v)
{
	return v;
}

inline uint64_t Hash(int64_t v)
{
	return (uint64_t)v;
}

template <class T>
inline typename std::enable_if_t<std::is_enum_v<T>, uint64_t> Hash(T v)
{
	return Hash(std::underlying_type_t<T>(v));
}

template <class T>
inline typename std::enable_if_t<std::is_pointer_v<T>, uint64_t> Hash(T v)
{
	return Hash(reinterpret_cast<intptr_t>(v));
}

// Return type indicating that Lua return values are pushed to the stack by the function
struct UserReturn
{
	inline UserReturn(int n)
		: num(n)
	{}

	inline operator int() const
	{
		return num;
	}

	int num;
};

// Return type indicating that the return value should be passed to Lua LuaWrite(), i.e. the value should be serialized
template <class T>
struct ByValReturn
{
	inline ByValReturn()
		: Object(nullptr)
	{}
	
	inline ByValReturn(T* obj)
		: Object(obj)
	{}

	inline operator T*() const
	{
		return Object;
	}

	T* Object;
};

// Return type indicating that the return value should be passed to Lua using an object proxy
template <class T>
struct RefReturn
{
	inline RefReturn()
		: Object(nullptr)
	{}
	
	inline RefReturn(T* obj)
		: Object(obj)
	{}

	inline operator T*() const
	{
		return Object;
	}

	T* Object;
};

// Parameter type indicating that the value should be passed from Lua using an object proxy
template <class T>
struct ProxyParam
{
	inline ProxyParam()
		: Object(nullptr)
	{}
	
	inline ProxyParam(T* obj)
		: Object(obj)
	{}

	inline operator T*() const
	{
		return Object;
	}

	inline T* operator ->() const
	{
		return Object;
	}

	T* Object;
};

template <class T>
struct OverrideableProperty
{
    using Type = T;

    T Value;
    bool IsOverridden;
};

enum PropertyOperationResult
{
	Success,
	NoSuchProperty,
	ReadOnly,
	UnsupportedType,
	Unknown
};

struct Event
{
	virtual uint32_t GetEventId() = 0;
};

END_SE()

BEGIN_NS(lua)

// Indicates that a type should be pushed through the polymorphic MakeObjectRef()
// implementation, since there are separate property maps for the different subclasses
template <class T>
struct LuaPolymorphic {
	static constexpr bool IsPolymorphic = false;
};

#define LUA_POLYMORPHIC(cls) \
	template <> \
	struct LuaPolymorphic<cls> { \
		static constexpr bool IsPolymorphic = true; \
		static void MakeRef(lua_State* L, cls* value, LifetimeHandle const& lifetime); \
	};


// Indicates that a type should be pushed through the polymorphic MakeObjectRef()
// implementation, since there are separate property maps for the different subclasses
template <class T>
struct LuaLifetimeInfo {
	static constexpr bool HasInfiniteLifetime = false;
};

#define LUA_INFINITE_LIFETIME(cls) \
	template <> \
	struct LuaLifetimeInfo<cls> { \
		static constexpr bool HasInfiniteLifetime = true; \
	};


END_NS()
