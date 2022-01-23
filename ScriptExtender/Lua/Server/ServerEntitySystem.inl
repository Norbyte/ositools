BEGIN_SE()

using namespace lua;

esv::StatusMachine* IEoCServerObject::GetStatusMachine() const
{
	ComponentHandle handle;
	GetObjectHandle(handle);

	switch ((ObjectType)handle.GetType()) {
	case ObjectType::ServerCharacter:
	{
		auto ch = static_cast<esv::Character const*>(this);
		return ch->StatusMachine;
	}

	case ObjectType::ServerItem:
	{
		auto it = static_cast<esv::Item const*>(this);
		return it->StatusMachine;
	}

	default:
		OsiError("Objects of type " << handle.GetType() << " have no status machine!");
		return nullptr;
	}
}

ObjectSet<FixedString> IGameObject::LuaGetTags()
{
	ObjectSet<FixedString> tags;
	GetTags(tags);
	return tags;
}

bool IGameObject::LuaIsTagged(FixedString const& tag)
{
	return IsTagged(tag);
}

ComponentHandle IGameObject::LuaGetHandle()
{
	ComponentHandle handle;
	GetObjectHandle(handle);
	return handle;
}

EntityHandle IGameObject::LuaGetEntityHandle()
{
	EntityHandle handle;
	GetEntityHandle(handle);
	return handle;
}

glm::vec3 IGameObject::LuaGetTranslate()
{
	return *GetTranslate();
}

void IGameObject::LuaSetTranslate(glm::vec3 const& translate)
{
	SetTranslate(translate);
}

glm::mat3 IGameObject::LuaGetRotate()
{
	return *GetRotation();
}

float IGameObject::LuaGetScale()
{
	return GetScale();
}

void IGameObject::LuaSetScale(float scale)
{
	SetScale(scale);
}

glm::vec3 IGameObject::LuaGetVelocity()
{
	return *GetVelocity();
}

float IGameObject::LuaGetHeight()
{
	return GetHeight();
}

Visual* IGameObject::LuaGetVisual()
{
	return GetVisual();
}

std::optional<STDWString> IEoCServerObject::LuaGetDisplayName()
{
	TranslatedString name;
	GetDisplayName(name);
	return name.Handle.ReferenceString;
}

RefReturn<esv::Status> IEoCServerObject::LuaGetStatus(FixedString const& statusId)
{
	auto statusMachine = GetStatusMachine();
	if (!statusMachine) {
		return {};
	}

	return statusMachine->GetStatus(statusId);
}

RefReturn<esv::Status> IEoCServerObject::LuaGetStatusByType(StatusType type)
{
	auto statusMachine = GetStatusMachine();
	if (!statusMachine) {
		return {};
	}

	return statusMachine->GetStatus(type);
}

RefReturn<esv::Status> IEoCServerObject::LuaGetStatusByHandle(ComponentHandle const& handle)
{
	auto statusMachine = GetStatusMachine();
	if (!statusMachine) {
		return {};
	}

	return statusMachine->GetStatus(handle);
}

ObjectSet<FixedString> IEoCServerObject::LuaGetStatusIds()
{
	ObjectSet<FixedString> statuses;
	auto statusMachine = GetStatusMachine();
	if (statusMachine) {
		for (auto status : statusMachine->Statuses) {
			statuses.push_back(status->StatusId);
		}
	}

	return statuses;
}

UserReturn IEoCServerObject::LuaGetStatuses(lua_State* L)
{
	lua_newtable(L);

	auto statusMachine = GetStatusMachine();
	if (statusMachine) {
		int32_t index = 1;
		for (auto status : statusMachine->Statuses) {
			push(L, index++);
			MakeObjectRef(L, status);
			lua_settable(L, -3);
		}
	}

	return 1;
}

GameObjectTemplate* IEoCServerObject::CreateCacheTemplate()
{
	CacheTemplate(TemplateType::GlobalCacheTemplate, nullptr);
	return GetCurrentTemplate();
}

void IEoCServerObject::LuaTransformTemplate(ProxyParam<GameObjectTemplate> tmpl)
{
	if (tmpl->GetTypeId() != GetCurrentTemplate()->GetTypeId()) {
		OsiError("Attempted to transform object from " << GetCurrentTemplate()->GetTypeId() << " template to " << tmpl->GetTypeId() << " template!");
		return;
	}

	if (tmpl->Id == GetCurrentTemplate()->Id) {
		return;
	}

	TransformTemplate(tmpl);
}

void IEoCServerObject::ForceSyncToPeers()
{
	ClearVisiblePeers();
}

END_SE()
