#include <Extender/Client/StatusHelpers.h>

BEGIN_NS(ecl)

void StatusHelpers::PostStartup()
{
	auto& lib = gExtender->GetEngineHooks();

	lib.ecl__StatusMachine__ExitStatus.SetWrapper(&StatusHelpers::OnStatusMachineExit, this);
}

void StatusHelpers::OnStatusMachineExit(StatusMachine::ExitStatusProc* wrapped,
	StatusMachine* self, Status* status)
{
	// Work around crash when the character no longer has any stats but a sneak check is
	// being performed on it when exiting statuses
	auto character = GetEntityWorld()->GetComponent<Character>(status->OwnerHandle);
	if (character && character->Stats == nullptr) {
		status->Exit();
		if ((status->Flags & StatusFlags::HasVisuals) == StatusFlags::HasVisuals) {
			status->DestroyVisuals();
		}
	} else {
		wrapped(self, status);
	}
}

END_NS()
