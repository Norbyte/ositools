#include "stdafx.h"
#include "GlobalFixedStrings.h"
#include "ExtensionHelpers.h"

namespace dse
{
	void GlobalFixedStrings::Initialize()
	{
#define FS(val) str##val = MakeFixedString(#val)
#define FS_NAME(name, val) str##name = MakeFixedString(val)
#include "GlobalFixedStrings.inl"
#undef FS
#undef FS_NAME
	}

	GlobalFixedStrings GFS;
}
