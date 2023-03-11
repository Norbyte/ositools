#pragma once

#include <Osiris/Shared/CustomFunctions.h>

BEGIN_NS(esv)

class CustomFunctionLibrary
{
public:
	CustomFunctionLibrary(OsirisExtender & osiris);

	void Register();
	void RegisterHelperFunctions();
	void RegisterMathFunctions();
	void RegisterStatFunctions();
	void RegisterStatusFunctions();
	void RegisterGameActionFunctions();
	void RegisterProjectileFunctions();
	void RegisterHitFunctions();
	void RegisterPlayerFunctions();
	void RegisterItemFunctions();
	void RegisterCharacterFunctions();
	void RegisterCustomStatFunctions();
	void RegisterLuaFunctions();

private:
	OsirisExtender& osiris_;
};

END_NS()
