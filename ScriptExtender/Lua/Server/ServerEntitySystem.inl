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

glm::mat3 IGameObject::LuaGetRotate()
{
	return *GetRotation();
}

float IGameObject::LuaGetScale()
{
	return GetScale();
}

glm::vec3 IGameObject::LuaGetVelocity()
{
	return *GetVelocity();
}

float IGameObject::LuaGetHeight()
{
	return GetHeight();
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

ObjectSet<FixedString> IEoCServerObject::LuaGetStatusIds()
{
	ObjectSet<FixedString> statuses;
	auto statusMachine = GetStatusMachine();
	if (statusMachine) {
		for (auto status : statusMachine->Statuses) {
			statuses.Add(status->StatusId);
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

END_SE()
