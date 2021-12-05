#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(lua::mod)

bool IsModLoaded(char const* modNameGuid)
{
	auto modUuid = NameGuidToFixedString(modNameGuid);
	if (modUuid) {
		auto modManager = gExtender->GetCurrentExtensionState()->GetModManager();
		for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
			if (mod.Info.ModuleUUID == modUuid) {
				return true;
			}
		}
	}

	return false;
}

ObjectSet<FixedString> GetLoadOrder()
{
	ObjectSet<FixedString> loadOrder;
	auto modManager = gExtender->GetCurrentExtensionState()->GetModManager();

	for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
		loadOrder.Add(mod.Info.ModuleUUID);
	}

	return loadOrder;
}

UserReturn GetModInfo(lua_State* L, char const* modNameGuid)
{
	StackCheck _(L, 1);
	Module const * module{ nullptr };
	auto modUuid = NameGuidToFixedString(modNameGuid);
	if (modUuid) {
		auto modManager = gExtender->GetCurrentExtensionState()->GetModManager();
		for (auto const& mod : modManager->BaseModule.LoadOrderedModules) {
			if (mod.Info.ModuleUUID == modUuid) {
				module = &mod;
				break;
			}
		}
	}

	if (module != nullptr) {
		lua_newtable(L);
		setfield(L, "UUID", module->Info.ModuleUUID);
		setfield(L, "Name", module->Info.Name);
		setfield(L, "Version", module->Info.ModVersion.Ver);
		setfield(L, "PublishVersion", module->Info.PublishVersion.Ver);
		setfield(L, "Directory", module->Info.Directory);
		setfield(L, "Author", module->Info.Author);
		setfield(L, "Description", module->Info.Description);
		setfield(L, "ModuleType", module->Info.ModuleType);
			
		lua_newtable(L);
		auto & dependents = module->DependentModules;
		for (uint32_t i = 0; i < dependents.Size; i++) {
			auto const & mod = dependents[i];
			settable(L, i + 1, mod.Info.ModuleUUID);
		}
		lua_setfield(L, -2, "Dependencies");
	} else {
		push(L, nullptr);
	}

	return 1;
}

Module* GetMod(char const* modNameGuid)
{
	Module const * module{ nullptr };
	auto modUuid = NameGuidToFixedString(modNameGuid);
	if (modUuid) {
		auto modManager = gExtender->GetCurrentExtensionState()->GetModManager();
		for (auto& mod : modManager->BaseModule.LoadOrderedModules) {
			if (mod.Info.ModuleUUID == modUuid) {
				return &mod;
			}
		}
	}

	return nullptr;
}

Module* GetBaseMod()
{
	return &gExtender->GetCurrentExtensionState()->GetModManager()->BaseModule;
}

void RegisterModLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"IsModLoaded", LuaWrapFunction(&IsModLoaded)},
		{"GetLoadOrder", LuaWrapFunction(&GetLoadOrder)},
		{"GetModInfo", LuaWrapFunction(&GetModInfo)},
		{"GetMod", LuaWrapFunction(&GetMod)},
		{"GetBaseMod", LuaWrapFunction(&GetBaseMod)},
		{0,0}
	};

	RegisterLib(L, "Mod", lib);
}

END_NS()
