#include <Extender/Server/StatusHelpers.h>

BEGIN_NS(esv)

void StatusHelpers::PostStartup()
{
	using namespace std::placeholders;
	auto& lib = gExtender->GetEngineHooks();

	lib.esv__StatusMachine__EnterStatus.SetWrapper(
		std::bind(&StatusHelpers::OnStatusMachineEnter, this, _1, _2, _3)
	);
	lib.esv__StatusMachine__Update.SetPreHook(
		std::bind(&StatusHelpers::OnStatusMachineUpdate, this, _1, _2)
	);
	lib.esv__StatusMachine__DeleteStatusByHandle.SetPreHook(
		std::bind(&StatusHelpers::OnStatusMachineDelete, this, _1, _2)
	);
	lib.esv__StatusMachine__DestroyStatus.SetWrapper(
		std::bind(&StatusHelpers::OnStatusMachineDestroy, this, _1, _2, _3)
	);
	lib.esv__StatusMachine__ExitStatus.SetWrapper(
		std::bind(&StatusHelpers::OnStatusMachineExit, this, _1, _2, _3)
	);
	lib.esv__Status__GetEnterChanceHook.SetWrapper(
		std::bind(&StatusHelpers::OnStatusGetEnterChance, this, _1, _2, _3)
	);
	lib.esv__StatusHeal__Enter.SetPreHook(
		std::bind(&StatusHelpers::OnStatusHealEnter, this, _1)
	);
	lib.esv__StatusConsume__Enter.SetWrapper(
		std::bind(&StatusHelpers::OnStatusConsumeEnter, this, _1, _2)
	);
	lib.esv__StatusConsume__Exit.SetWrapper(
		std::bind(&StatusHelpers::OnStatusConsumeExit, this, _1, _2)
	);
}

bool StatusHelpers::OnStatusMachineEnter(StatusMachine::EnterStatusProc* wrapped,
	StatusMachine* self, Status* status)
{
	return wrapped(self, status);
}

void StatusHelpers::OnStatusMachineUpdate(StatusMachine* self, GameTime* time)
{
	auto shouldDelete = GetStaticSymbols().esv__Status__ShouldDelete;
	for (auto status : self->Statuses) {
		if (shouldDelete(status)) {
			LuaServerPin lua(ExtensionState::Get());
			if (lua) {
				lua->OnBeforeStatusDelete(status);
			}
		}
	}
}

void StatusHelpers::OnStatusMachineDelete(StatusMachine* self, ComponentHandle* handle)
{
	auto status = self->GetStatus(*handle);
	LuaServerPin lua(ExtensionState::Get());
	if (lua) {
		lua->OnStatusDelete(status);
	}
}

bool StatusHelpers::OnStatusMachineDestroy(StatusMachine::DestroyStatusProc* wrapped,
	StatusMachine* self, ComponentHandle const& handle)
{
	return wrapped(self, handle);
}

void StatusHelpers::OnStatusMachineExit(StatusMachine::ExitStatusProc* wrapped,
	StatusMachine* self, Status* status)
{
	wrapped(self, status);
}

int32_t StatusHelpers::OnStatusGetEnterChance(Status::GetEnterChanceProc* wrappedGetEnterChance,
	Status * status, bool isEnterCheck)
{
	LuaServerPin lua(ExtensionState::Get());
	if (lua) {
		auto enterChance = lua->StatusGetEnterChance(status, isEnterCheck);
		if (enterChance) {
			return *enterChance;
		}
	}

	return wrappedGetEnterChance(status, isEnterCheck);
}

void StatusHelpers::OnStatusHealEnter(Status * status)
{
	auto statusHeal = static_cast<StatusHeal *>(status);
	gExtender->GetServer().GetOsirisStatusHelpers().ThrowStatusHealEnter(statusHeal);
}

bool WillMadnessCrash(StatusConsume* status)
{
	if (status->StatusId != GFS.strMADNESS) return false;

	auto ch = GetEntityWorld()->GetComponent<Character>(status->OwnerHandle, false);
	// Game may crash if an item with MADNESS has summons
	if (ch == nullptr) return true;

	for (auto const& summonHandle : ch->SummonHandles) {
		auto summon = GetEntityWorld()->GetComponent<Character>(summonHandle, false);
		// Game may crash if any of the summons is already gone,
		// or if the summon is an item
		if (summon == nullptr) return true;
	}

	return false;
}

bool StatusHelpers::OnStatusConsumeEnter(Status::EnterProc* wrappedEnter, Status* status)
{
	if (WillMadnessCrash(static_cast<StatusConsume*>(status))) {
		// Skip all StatusConsume-specific logic if it would crash the game;
		// the "real" consume logic in StatusConsumeBase does not have this bug
		return GetStaticSymbols().esv__StatusConsumeBase__Enter(status);
	} else {
		return wrappedEnter(status);
	}
}

void StatusHelpers::OnStatusConsumeExit(Status::ExitProc* wrappedExit, Status* status)
{
	if (WillMadnessCrash(static_cast<StatusConsume*>(status))) {
		// Skip all StatusConsume-specific logic if it would crash the game;
		// the "real" consume logic in StatusConsumeBase does not have this bug
		return GetStaticSymbols().esv__StatusConsumeBase__Exit(status);
	} else {
		return wrappedExit(status);
	}
}

void StatusHelpers::PreventApply(IGameObject* gameObject, ComponentHandle const& statusHandle, bool preventApply)
{
	ComponentHandle gameObjectHandle;
	gameObject->GetObjectHandle(gameObjectHandle);

	auto status = ExtensionState::Get().PendingStatuses.Find(gameObjectHandle, statusHandle);
	if (status == nullptr) {
		OsiError("No pending status found with handle " << (int64_t)statusHandle);
		return;
	}

	status->PreventApply = preventApply;
}

Status * StatusHelpers::ConstructStatus(StatusMachine * statusMachine, FixedString const& statusId, StatusType type)
{
	Status* status{ nullptr };

	auto createStatus = GetStaticSymbols().esv__StatusMachine__CreateStatus;
	if (createStatus == nullptr) {
		OsiErrorS("esv::StatusMachine::CreateStatus not found!");
		return nullptr;
	}

	if (statusId) {
		status = createStatus(statusMachine, statusId, 0);
	}

	if (status == nullptr) {
		OsiError("Status does not exist: " << statusId);
		return nullptr;
	}

	if (status->GetStatusId() != type) {
		// TODO - dangling status ptr!
		OsiError("Status has incorrect type: " << statusId);
		return nullptr;
	}

	return status;
}


Status* StatusHelpers::PrepareStatus(StatusMachine* statusMachine, FixedString const& statusId, float lifeTime)
{
	auto statusProtoMgr = GetStaticSymbols().eoc__StatusPrototypeManager;
	if (!statusProtoMgr || !*statusProtoMgr) {
		OsiError("Status prototype manager not available!");
		return nullptr;
	}

	auto proto = (*statusProtoMgr)->Prototypes.try_get(statusId);
	if (!proto) {
		OsiError("No status found with ID: " << statusId);
		return nullptr;
	}

	auto statusType = proto->StatusId;
	auto status = ConstructStatus(statusMachine, statusId, statusType);
	if (status == nullptr) {
		return nullptr;
	}

	if (lifeTime == -2.0f) {
		status->Flags0 |= esv::StatusFlags0::KeepAlive;
		status->CurrentLifeTime = 1.0f;
	} else {
		if (lifeTime < 0.0f) {
			lifeTime = -1.0f;
		}

		status->Flags0 |= esv::StatusFlags0::IsLifeTimeSet;
		status->LifeTime = lifeTime;
		status->CurrentLifeTime = lifeTime;
	}

	return status;
}

END_NS()
