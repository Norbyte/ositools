#include <Lua/Shared/Proxies/LuaEntityProxy.h>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaBinding.h>

#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Inventory.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/Components/Projectile.h>
#include <GameDefinitions/GameObjects/Effect.h>
#include <GameDefinitions/GameObjects/Surface.h>
#include <GameDefinitions/CustomStats.h>
#include <GameDefinitions/TurnManager.h>

BEGIN_NS(lua)

EntityProxyImplBase::~EntityProxyImplBase() {}

char const* const EntityProxy::MetatableName = "EntityProxy";

EntityProxy::EntityProxy(EntityHandle const& handle)
	: handle_(handle)
{}

int EntityProxy::HasComponent(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<EntityProxy*>(L, 1);
	return self->GetImpl()->HasComponent(L, self->handle_);
}

int EntityProxy::GetComponent(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<EntityProxy*>(L, 1);
	return self->GetImpl()->GetComponent(L, self->handle_);
}

int EntityProxy::GetAllComponents(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<EntityProxy*>(L, 1);
	return self->GetImpl()->GetComponents(L, self->handle_);
}

int EntityProxy::IsAlive(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<EntityProxy*>(L, 1);
	auto alive = self->GetImpl()->IsAlive(self->handle_);
	push(L, alive);
	return 1;
}

int EntityProxy::Index(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<EntityProxy*>(L, 1);
	auto key = get<FixedString>(L, 2);

	if (key == GFS.strHasComponent) {
		push(L, &EntityProxy::HasComponent);
		return 1;
	}

	if (key == GFS.strGetComponent) {
		push(L, &EntityProxy::GetComponent);
		return 1;
	}

	if (key == GFS.strGetComponents) {
		push(L, &EntityProxy::GetAllComponents);
		return 1;
	}

	if (key == GFS.strHandle) {
		push(L, ComponentHandle{ self->handle_.Handle });
		return 1;
	}

	if (key == GFS.strEntityType) {
		push(L, self->handle_.GetType());
		return 1;
	}

	if (key == GFS.strSalt) {
		push(L, self->handle_.GetSalt());
		return 1;
	}

	if (key == GFS.strIndex) {
		push(L, self->handle_.GetIndex());
		return 1;
	}

	if (key == GFS.strIsAlive) {
		push(L, &EntityProxy::IsAlive);
		return 1;
	}

	push(L, nullptr);
	return 1;
}

int EntityProxy::ToString(lua_State* L)
{
	StackCheck _(L, 1);
	char entityName[100];
	sprintf_s(entityName, "Entity (%016llx)", handle_.Handle);
	push(L, entityName);
	return 1;
}

bool EntityProxy::IsEqual(lua_State* L, EntityProxy* other)
{
	return handle_ == other->handle_;
}



void MakeComponentHandleProxy(lua_State* L, ComponentHandle const& handle, esv::EntityWorld* entityWorld)
{
	ComponentHandleProxy::Make(L, handle, entityWorld);
}

void MakeComponentHandleProxy(lua_State* L, ComponentHandle const& handle, ecl::EntityWorld* entityWorld)
{
	ComponentHandleProxy::Make(L, handle, entityWorld);
}


ComponentHandleProxyImplBase::~ComponentHandleProxyImplBase() {}

template <class T, class TEntityWorld>
bool ConstructComponentProxyHelper(lua_State* L, TEntityWorld* entityWorld, ComponentHandle const& handle)
{
	auto component = entityWorld->GetComponent<T>(handle);
	if (component != nullptr) {
		ObjectProxy2::MakeRef(L, component, lua::State::FromLua(L)->GetCurrentLifetime());
		return true;
	} else {
		push(L, nullptr);
		return false;
	}
}

#define P_CLS(cls) case dse::cls::ObjectTypeIndex: return ConstructComponentProxyHelper<dse::cls>(L, entityWorld, handle)

bool ConstructComponentProxy(lua_State* L, EntityWorldBase<dse::esv::EntityComponentIndex>* entityWorld, ComponentHandle const& handle)
{
	switch ((ObjectHandleType)handle.GetType())
	{
	// Components from ECS
	P_CLS(esv::Character);
	P_CLS(esv::Item);
	P_CLS(esv::Projectile);
	P_CLS(esv::CombatComponent);

	case esv::Inventory::ObjectTypeIndex:
	{
		auto inventory = GetStaticSymbols().GetServerInventoryFactory()->Get(handle);
		if (inventory) {
			ObjectProxy2::MakeRef(L, inventory, lua::State::FromLua(L)->GetCurrentLifetime());
			return true;
		} else {
			push(L, nullptr);
			return false;
		}
	}

	case esv::InventoryView::ObjectTypeIndex:
	{
		auto inventoryView = GetStaticSymbols().GetServerInventoryViewFactory()->Get(handle);
		if (inventoryView) {
			ObjectProxy2::MakeRef(L, inventoryView, lua::State::FromLua(L)->GetCurrentLifetime());
			return true;
		} else {
			push(L, nullptr);
			return false;
		}
	}

	case esv::SurfaceAction::ObjectTypeIndex:
	{
		auto factory = *GetStaticSymbols().esv__SurfaceActionFactory;
		auto action = factory->Get(handle);
		if (action) {
			ObjectProxy2::MakeRef(L, action, lua::State::FromLua(L)->GetCurrentLifetime());
			return true;
		} else {
			push(L, nullptr);
			return false;
		}
	}
	default:
		OsiError("No user-visible class exists for handles of type " << handle.GetType());
		push(L, nullptr);
		return false;
	}
}

bool ConstructComponentProxy(lua_State* L, EntityWorldBase<dse::ecl::EntityComponentIndex>* entityWorld, ComponentHandle const& handle)
{
	switch ((ObjectHandleType)handle.GetType())
	{
	// Components from ECS
	P_CLS(ecl::Character);
	P_CLS(ecl::Item);

	case esv::Inventory::ObjectTypeIndex:
	{
		auto inventory = GetStaticSymbols().GetClientInventoryFactory()->Get(handle);
		if (inventory) {
			ObjectProxy2::MakeRef(L, inventory, lua::State::FromLua(L)->GetCurrentLifetime());
			return true;
		} else {
			push(L, nullptr);
			return false;
		}
	}

	case esv::InventoryView::ObjectTypeIndex:
	{
		auto inventoryView = GetStaticSymbols().GetClientInventoryViewFactory()->Get(handle);
		if (inventoryView) {
			ObjectProxy2::MakeRef(L, inventoryView, lua::State::FromLua(L)->GetCurrentLifetime());
			return true;
		} else {
			push(L, nullptr);
			return false;
		}
	}

	// resolve from EffectFactory!
	// P_CLS(ecl::Effect);
	// P_CLS(Visual);
	default:
		OsiError("No user-visible class exists for handles of type " << handle.GetType());
		push(L, nullptr);
		return false;
	}
}

#undef P_CLS

int ComponentHandleProxyImpl<esv::EntityWorld>::GetComponent(lua_State* L, ComponentHandle const& handle)
{
	ConstructComponentProxy(L, entityWorld_, handle);
	return 1;
}

int ComponentHandleProxyImpl<ecl::EntityWorld>::GetComponent(lua_State* L, ComponentHandle const& handle)
{
	ConstructComponentProxy(L, entityWorld_, handle);
	return 1;
}


char const* const ComponentHandleProxy::MetatableName = "ComponentHandleProxy";

ComponentHandleProxy::ComponentHandleProxy(ComponentHandle const& handle)
	: handle_(handle)
{}

int ComponentHandleProxy::GetComponent(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ComponentHandleProxy*>(L, 1);
	return self->GetImpl()->GetComponent(L, self->handle_);
}

int ComponentHandleProxy::Index(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ComponentHandleProxy*>(L, 1);
	auto key = get<FixedString>(L, 2);

	if (key == GFS.strGet) {
		push(L, &ComponentHandleProxy::GetComponent);
		return 1;
	}

	if (key == GFS.strTypeId) {
		push(L, self->handle_.GetType());
		return 1;
	}

	if (key == GFS.strType) {
		auto typeName = EnumInfo<ObjectHandleType>::Find((ObjectHandleType)self->handle_.GetType());
		push(L, typeName);
		return 1;
	}

	if (key == GFS.strSalt) {
		push(L, self->handle_.GetSalt());
		return 1;
	}

	if (key == GFS.strIndex) {
		push(L, self->handle_.GetIndex());
		return 1;
	}

	push(L, nullptr);
	return 1;
}

int ComponentHandleProxy::ToString(lua_State* L)
{
	StackCheck _(L, 1);

	char entityName[200];
	auto componentName = EnumInfo<ObjectHandleType>::Find((ObjectHandleType)handle_.GetType());
	if (componentName) {
		_snprintf_s(entityName, std::size(entityName) - 1, "%s Object (%016llx)", componentName.GetString(), handle_.Handle);
	} else {
		_snprintf_s(entityName, std::size(entityName) - 1, "Object (%016llx)", handle_.Handle);
	}

	push(L, entityName);
	return 1;
}

bool ComponentHandleProxy::IsEqual(lua_State* L, ComponentHandleProxy* other)
{
	return handle_ == other->handle_;
}


void RegisterEntityProxy(lua_State* L)
{
	EntityProxy::RegisterMetatable(L);
	ComponentHandleProxy::RegisterMetatable(L);
}

END_NS()
