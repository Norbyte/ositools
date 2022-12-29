#pragma once

struct lua_State;

BEGIN_NS(lua)

template <class T, class ...Trait>
class Traits
{
public:
	template <class ...Args>
	inline Traits(Args&&... args) : value_(std::forward<Args>(args)...) {}

	inline Traits(T const& v) : value_(v) {}
	inline Traits(Traits<T, Trait...> const& v) : value_(*v) {}
	inline Traits(T&& v) : value_(std::move(v)) {}
	inline Traits(Traits<T, Trait...>&& v) : value_(std::move(*v)) {}

	inline Traits& operator = (T const& v)
	{
		value_ = v;
		return *this;
	}

	inline Traits& operator = (Traits<T, Trait...> const& v)
	{
		value_ = *v;
		return *this;
	}

	inline Traits& operator = (T&& v)
	{
		value_ = v;
		return *this;
	}

	inline Traits& operator = (Traits<T, Trait...>&& v)
	{
		value_ = std::move(*v);
		return *this;
	}

	inline T& operator * ()
	{
		return value_;
	}

	inline T const& operator * () const
	{
		return value_;
	}

	inline T* operator -> ()
	{
		return &value_;
	}

	inline T const* operator -> () const
	{
		return &value_;
	}

private:
	T value_;
};

enum class CallableTrait {};

template <class Trait>
void ValidateTrait(lua_State* L, int index);

template <>
void ValidateTrait<CallableTrait>(lua_State* L, int index);

END_NS()
