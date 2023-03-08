#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Status.h>

BEGIN_NS(esv)

struct EntityStatusHandle
{
	inline bool operator == (EntityStatusHandle const& o) const
	{
		return Owner == o.Owner && Status == o.Status;
	}

	ComponentHandle Owner;
	ComponentHandle Status;
};

END_NS()


namespace std
{
	template<> struct hash<dse::esv::EntityStatusHandle>
	{
		typedef dse::esv::EntityStatusHandle argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& h) const noexcept
		{
			return std::hash<uint64_t>{}(h.Owner.Handle + h.Status.Handle);
		}
	};
}


BEGIN_NS(esv)

class StatusHelpers
{
public:
	void PostStartup();

	void PreventApply(IGameObject* gameObject, ComponentHandle const& statusHandle, bool preventApply);
	Status* ConstructStatus(StatusMachine* statusMachine, FixedString const& statusId, StatusType type);
	Status* PrepareStatus(StatusMachine* statusMachine, FixedString const& statusId, float lifeTime);

private:
	struct StatusApplyData
	{
		Status* Status;
		bool PendingDelete{false};
	};

	// Statuses that are being applied - i.e. we're inside StatusMachine::DoEnter()
	std::unordered_map<EntityStatusHandle, StatusApplyData> pendingApply_;

	bool OnStatusMachineEnter(StatusMachine::EnterStatusProc* wrapped, StatusMachine* self, 
		Status* status);
	void OnStatusMachineUpdate(StatusMachine* self, GameTime* time);
	bool OnStatusMachineDeleteStatusByHandle(StatusMachine::DeleteStatusByHandleProc* wrapped, StatusMachine* self,
		ComponentHandle const& handle);
	void OnStatusMachineExit(StatusMachine::ExitStatusProc* wrapped, StatusMachine* self, 
		Status* status);
	int32_t OnStatusGetEnterChance(Status::GetEnterChanceProc* wrappedGetEnterChance,
		Status* status, bool isEnterCheck);
	void OnStatusHealEnter(Status* status);
	bool OnStatusConsumeEnter(Status::EnterProc* wrappedEnter, Status* status);
	void OnStatusConsumeExit(Status::ExitProc* wrappedExit, Status* status);
};

END_NS()
