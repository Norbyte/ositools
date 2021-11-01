#include <stdafx.h>
#include <GameDefinitions/GlobalFixedStrings.h>
#include <Extender/Shared/ExtensionHelpers.h>

namespace dse
{
	void GlobalFixedStrings::Initialize()
	{
#define FS(val) str##val = FixedString(#val)
#define FS_NAME(name, val) str##name = FixedString(val)
#include "GlobalFixedStrings.inl"
#undef FS
#undef FS_NAME
	}

	GlobalFixedStrings GFS;
}
