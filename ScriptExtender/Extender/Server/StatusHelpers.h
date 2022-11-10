#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Status.h>

BEGIN_NS(esv)

class StatusHelpers
{
public:
	void PostStartup();

	void PreventApply(IGameObject* gameObject, ComponentHandle const& statusHandle, bool preventApply);
	Status* ConstructStatus(StatusMachine* statusMachine, FixedString const& statusId, StatusType type);
	Status* PrepareStatus(StatusMachine* statusMachine, FixedString const& statusId, float lifeTime);

private:
	bool OnStatusMachineEnter(StatusMachine::EnterStatusProc* wrapped, StatusMachine* self, 
		Status* status);
	void OnStatusMachineUpdate(StatusMachine* self, GameTime* time);
	void OnStatusMachineDelete(StatusMachine* self, ComponentHandle* handle);
	bool OnStatusMachineDestroy(StatusMachine::DestroyStatusProc* wrapped, StatusMachine* self, 
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
