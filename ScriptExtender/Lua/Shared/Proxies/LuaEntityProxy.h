#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/EntitySystem.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

class EntityProxyImplBase
{
public:
	virtual ~EntityProxyImplBase();
	virtual bool IsAlive(EntityHandle const& handle) = 0;
	virtual int GetComponents(lua_State* L, EntityHandle const& handle) = 0;
	virtual int HasComponent(lua_State* L, EntityHandle const& handle) = 0;
	virtual int GetComponent(lua_State* L, EntityHandle const& handle) = 0;
};

void MakeComponentHandleProxy(lua_State* L, ComponentHandle const& handle, esv::EntityWorld* entityWorld);
void MakeComponentHandleProxy(lua_State* L, ComponentHandle const& handle, ecl::EntityWorld* entityWorld);

template <class TEntityWorld>
class EntityProxyImpl : public EntityProxyImplBase
{
public:
	EntityProxyImpl(TEntityWorld* entityWorld)
		: entityWorld_(entityWorld)
	{}

	~EntityProxyImpl() override
	{}

	bool IsAlive(EntityHandle const& handle) override
	{
		return entityWorld_->GetEntity(handle, false) != nullptr;
	}

	int GetComponents(lua_State* L, EntityHandle const& handle) override
	{
		lua_newtable(L);

		auto entity = entityWorld_->GetEntity(handle, false);
		if (entity == nullptr) {
			return 1;
		}

		for (unsigned i = 0; i < entity->Layout.Entries.size(); i++) {
			auto const& componentHandle = entity->Layout.Entries[i].Handle;
			if (componentHandle.IsValid()) {
				auto name = EnumInfo<typename TEntityWorld::EntityComponentIndex>::Find((typename TEntityWorld::EntityComponentIndex)i);

				if (name) {
					push(L, name);
					MakeComponentHandleProxy(L, componentHandle.Handle, entityWorld_);
					lua_settable(L, -3);
				}
			}
		}

		return 1;
	}

	int HasComponent(lua_State* L, EntityHandle const& handle) override
	{
		auto entity = entityWorld_->GetEntity(handle, false);
		if (entity == nullptr) {
			push(L, false);
			return 1;
		}

		auto componentIdx = (uint32_t)get<TEntityWorld::EntityComponentIndex>(L, 2);
		auto valid = entity->Layout.Entries.size() > componentIdx
			&& entity->Layout.Entries[componentIdx].Handle.IsValid();

		push(L, valid);
		return 1;
	}

	int GetComponent(lua_State* L, EntityHandle const& handle) override
	{
		auto entity = entityWorld_->GetEntity(handle, false);
		if (entity == nullptr) {
			push(L, false);
			return 1;
		}

		auto componentIdx = (uint32_t)get<TEntityWorld::EntityComponentIndex>(L, 2);
		auto valid = entity->Layout.Entries.size() > componentIdx
			&& entity->Layout.Entries[componentIdx].Handle.IsValid();

		if (valid) {
			push(L, entity->Layout.Entries[componentIdx].Handle.Handle);
		} else {
			push(L, nullptr);
		}

		return 1;
	}

private:
	TEntityWorld* entityWorld_;
};

class EntityProxy : public Userdata<EntityProxy>, public Indexable, public Stringifiable,
	public EqualityComparable
{
public:
	static char const* const MetatableName;

	template <class TEntityWorld>
	inline static EntityProxyImpl<TEntityWorld>* Make(lua_State* L, EntityHandle const& handle, TEntityWorld* entityWorld)
	{
		auto self = NewWithExtraData(L, sizeof(EntityProxyImpl<TEntityWorld>), handle);
		return new (self->GetImpl()) EntityProxyImpl<TEntityWorld>(entityWorld);
	}

	EntityProxy(EntityHandle const& handle);

	static int HasComponent(lua_State* L);
	static int GetComponent(lua_State* L);
	static int GetAllComponents(lua_State* L);
	static int IsAlive(lua_State* L);

	int Index(lua_State* L);
	int ToString(lua_State* L);
	bool IsEqual(lua_State* L, EntityProxy* other);

	inline EntityHandle const& GetHandle() const
	{
		return handle_;
	}

	inline EntityProxyImplBase* GetImpl()
	{
		return reinterpret_cast<EntityProxyImplBase*>(this + 1);
	}

private:
	EntityHandle handle_;
};


class ComponentHandleProxyImplBase
{
public:
	virtual ~ComponentHandleProxyImplBase();
	virtual int GetComponent(lua_State* L, ComponentHandle const& handle) = 0;
};

template <class TEntityWorld>
class ComponentHandleProxyImpl : public ComponentHandleProxyImplBase
{
public:
	ComponentHandleProxyImpl(TEntityWorld* entityWorld)
		: entityWorld_(entityWorld)
	{}

	~ComponentHandleProxyImpl() override
	{}

	int GetComponent(lua_State* L, ComponentHandle const& handle) override;

private:
	TEntityWorld* entityWorld_;
};

class ComponentHandleProxy : public Userdata<ComponentHandleProxy>, public Indexable, public Stringifiable,
	public EqualityComparable
{
public:
	static char const* const MetatableName;

	template <class TEntityWorld>
	inline static ComponentHandleProxyImpl<TEntityWorld>* Make(lua_State* L, ComponentHandle const& handle, TEntityWorld* entityWorld)
	{
		auto self = NewWithExtraData(L, sizeof(ComponentHandleProxyImpl<TEntityWorld>), handle);
		return new (self->GetImpl()) ComponentHandleProxyImpl<TEntityWorld>(entityWorld);
	}

	ComponentHandleProxy(ComponentHandle const& handle);

	static int GetComponent(lua_State* L);

	int Index(lua_State* L);
	int ToString(lua_State* L);
	bool IsEqual(lua_State* L, ComponentHandleProxy* other);

	inline ComponentHandle const& Handle() const
	{
		return handle_;
	}

	inline ComponentHandleProxyImplBase* GetImpl()
	{
		return reinterpret_cast<ComponentHandleProxyImplBase*>(this + 1);
	}

private:
	ComponentHandle handle_;
};


inline EntityHandle do_get(lua_State* L, int index, Overload<EntityHandle>)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	return EntityProxy::CheckUserData(L, index)->GetHandle();
}


END_NS()
