#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Status.h>

BEGIN_NS(ecl)

class StatusHelpers
{
public:
	void PostStartup();

	void OnStatusMachineExit(StatusMachine::ExitStatusProc* wrapped, StatusMachine* self, Status* status);
};

END_NS()
