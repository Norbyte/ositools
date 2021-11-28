BEGIN_SE()

using namespace lua;

std::optional<STDWString> IEoCClientObject::LuaGetDisplayName()
{
	TranslatedString name;
	GetDisplayName(name);
	return name.Handle.ReferenceString;
}

ecl::StatusMachine* IEoCClientObject::GetStatusMachine() const
{
	ComponentHandle handle;
	GetObjectHandle(handle);

	switch ((ObjectType)handle.GetType()) {
	case ObjectType::ClientCharacter:
	{
		auto ch = static_cast<ecl::Character const*>(this);
		return ch->StatusMachine;
	}

	case ObjectType::ClientItem:
	{
		auto it = static_cast<ecl::Item const*>(this);
		return it->StatusMachine;
	}

	default:
		OsiError("Objects of type " << handle.GetType() << " have no status machine!");
		return nullptr;
	}
}

RefReturn<ecl::Status> IEoCClientObject::LuaGetStatus(FixedString const& statusId)
{
	auto statusMachine = GetStatusMachine();
	if (!statusMachine) {
		return {};
	}

	return statusMachine->GetStatus(statusId);
}

RefReturn<ecl::Status> IEoCClientObject::LuaGetStatusByType(StatusType type)
{
	auto statusMachine = GetStatusMachine();
	if (!statusMachine) {
		return {};
	}

	return statusMachine->GetStatus(type);
}

ObjectSet<FixedString> IEoCClientObject::LuaGetStatusIds()
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

UserReturn IEoCClientObject::LuaGetStatuses(lua_State* L)
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
