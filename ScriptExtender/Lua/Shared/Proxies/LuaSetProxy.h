#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

LifetimeHolder GetCurrentLifetime();

class SetProxyImplBase
{
public:
	inline virtual ~SetProxyImplBase() {};
	virtual TypeInformation const& GetType() const = 0;
	virtual void* GetRaw() = 0;
	virtual bool HasElement(lua_State* L, int luaIndex) = 0;
	virtual bool AddElement(lua_State* L, int luaIndex) = 0;
	virtual bool RemoveElement(lua_State* L, int luaIndex) = 0;
	virtual int Next(lua_State* L, int index) = 0;
	virtual unsigned Length() = 0;
};


class SetProxy : private Userdata<SetProxy>, public Indexable, public NewIndexable,
	public Lengthable, public Iterable, public Stringifiable, public EqualityComparable
{
public:
	static char const * const MetatableName;

	inline SetProxyImplBase* GetImpl()
	{
		return reinterpret_cast<SetProxyImplBase*>(this + 1);
	}

	inline bool IsAlive() const
	{
		return lifetime_.IsAlive();
	}

	template <class T>
	T* Get()
	{
		if (!lifetime_.IsAlive()) {
			return nullptr;
		}
			
		if (GetImpl()->GetType().TypeName == GetTypeInfo<T>().TypeName) {
			return reinterpret_cast<T*>(GetImpl()->GetRaw());
		} else {
			return nullptr;
		}
	}

private:
	LifetimeReference lifetime_;

	SetProxy(LifetimeHolder const& lifetime)
		: lifetime_(lifetime)
	{}

	~SetProxy()
	{
		GetImpl()->~SetProxyImplBase();
	}

protected:
	friend Userdata<SetProxy>;

	int Index(lua_State* L);
	int NewIndex(lua_State* L);
	int Length(lua_State* L);
	int Next(lua_State* L);
	int ToString(lua_State* L);
	bool IsEqual(lua_State* L, SetProxy* other);
};

template <class T>
struct IsSetLike { static constexpr bool Value = false; };

template <class T>
inline void push_set_proxy(lua_State* L, LifetimeHolder const& lifetime, T* v)
{
	SetProxy::Make<T>(L, v, lifetime);
}

template <class T>
inline T* checked_get_set_proxy(lua_State* L, int index)
{
	auto proxy = Userdata<SetProxy>::CheckUserData(L, index);
	auto const& typeName = GetTypeInfo<T>().TypeName;
	if (proxy->GetImpl()->GetType().TypeName == typeName) {
		auto obj = proxy->Get<T>();
		if (obj == nullptr) {
			luaL_error(L, "Argument %d: got Set<%s> whose lifetime has expired", index, typeName.GetString());
			return nullptr;
		} else {
			return obj;
		}
	} else {
		luaL_error(L, "Argument %d: expected Set<%s>, got Set<%s>", index, typeName.GetString(), proxy->GetImpl()->GetType().GetString());
		return nullptr;
	}
}

END_NS()
