#include <stdafx.h>
#include <GameHooks/DataLibraries.h>
#include <Extender/ScriptExtender.h>
#include <string>
#include <functional>
#include <psapi.h>
#include <GameDefinitions/UI.h>

#if !defined(OSI_EOCAPP)
BEGIN_SE()

SymbolMapper::MappingResult FindCharacterStatGetters(uint8_t const * match)
{
	unsigned ptrIndex = 0;
	auto & getters = GetStaticSymbols().CharStatsGetters;
	for (auto p = match; p < match + 0x240; p++) {
		auto insn = *reinterpret_cast<uint32_t const *>(p);
		if (insn == 0xE8CF8B49 /* 49 8B CF E8 -- mov rcx, r15; call */) {
			if (ptrIndex < std::size(getters.Ptrs)) {
				auto ptr = AsmResolveInstructionRef(p + 3);
				getters.Ptrs[ptrIndex++] = (void *)ptr;
			}
		}
	}

	if (getters.GetBlockChance != nullptr) {
		auto& mapper = gExtender->GetLibraryManager().Mapper();
		mapper.MapSymbol("CharacterStatGetters::GetAbility", (uint8_t *)getters.GetDodge, 0x480);
		mapper.MapSymbol("CharacterStatGetters::GetTalent", (uint8_t *)getters.GetDodge, 0x480);
	}

	return (getters.GetBlockChance != nullptr) ? SymbolMapper::MappingResult::Success : SymbolMapper::MappingResult::Fail;
}

SymbolMapper::MappingResult FindLibrariesEoCPlugin(uint8_t const * match)
{
	auto & lib = GetStaticSymbols().Libraries;

	auto freeFunc = AsmResolveInstructionRef(match + 0x21);
	auto initFunc = AsmResolveInstructionRef(match + 0x28);
	if (initFunc != nullptr && freeFunc != nullptr) {
		auto it = GetStaticSymbols().Libraries.find(initFunc);
		if (it != lib.end()) {
			it->second.refs++;
		} else {
			lib.insert(std::pair<uint8_t const *, StaticSymbols::EoCLibraryInfo>(initFunc, { initFunc, freeFunc, 1 }));
		}

		return SymbolMapper::MappingResult::TryNext;
	} else {
		return SymbolMapper::MappingResult::Fail;
	}
}

SymbolMapper::MappingResult FindActivateEntitySystem(uint8_t const* match)
{
	if (GetStaticSymbols().ActivateClientSystemsHook == nullptr) {
		GetStaticSymbols().ActivateClientSystemsHook = match;
		return SymbolMapper::MappingResult::TryNext;
	} else {
		GetStaticSymbols().ActivateServerSystemsHook = match;
		return SymbolMapper::MappingResult::Success;
	}
}

SymbolMapper::MappingResult FindActionMachineSetState(uint8_t const * match)
{
	if (GetStaticSymbols().ecl__ActionMachine__SetState == nullptr) {
		GetStaticSymbols().ecl__ActionMachine__SetState = (esv::ActionMachine::SetStateProc*)match;
		return SymbolMapper::MappingResult::TryNext;
	} else {
		GetStaticSymbols().esv__ActionMachine__SetState = (esv::ActionMachine::SetStateProc*)match;
		return SymbolMapper::MappingResult::Success;
	}
}

SymbolMapper::MappingResult FindActionMachineResetState(uint8_t const* match)
{
	if (GetStaticSymbols().ecl__ActionMachine__ResetState == nullptr) {
		GetStaticSymbols().ecl__ActionMachine__ResetState = (esv::ActionMachine::ResetStateProc*)match;
		return SymbolMapper::MappingResult::TryNext;
	}
	else {
		GetStaticSymbols().esv__ActionMachine__ResetState = (esv::ActionMachine::ResetStateProc*)match;
		return SymbolMapper::MappingResult::Success;
	}
}
	
SymbolMapper::MappingResult FindStatusHeal(uint8_t const * match)
{
	auto const& mainMod = gExtender->GetLibraryManager().Mapper().Modules().find("Main");
	auto moduleStart = mainMod->second.ModuleStart;
	auto moduleSize = mainMod->second.ModuleSize;

	// Look for this function ptr
	auto ptr = (uint64_t)match;
	for (auto p = moduleStart; p < moduleStart + moduleSize; p += 8) {
		if (*reinterpret_cast<uint64_t const *>(p) == ptr) {
			GetStaticSymbols().esv__StatusHeal__VMT = reinterpret_cast<esv::StatusVMT const *>(p - 25 * 8);
			return SymbolMapper::MappingResult::Success;
		}
	}

	return SymbolMapper::MappingResult::Fail;
}

SymbolMapper::MappingResult FindStatusHit(uint8_t const * match)
{
	auto const & mainMod = gExtender->GetLibraryManager().Mapper().Modules().find("Main");
	auto moduleStart = mainMod->second.ModuleStart;
	auto moduleSize = mainMod->second.ModuleSize;

	// Look for this function ptr
	auto ptr = (uint64_t)match;
	for (auto p = moduleStart; p < moduleStart + moduleSize; p += 8) {
		if (*reinterpret_cast<uint64_t const *>(p) == ptr) {
			GetStaticSymbols().esv__StatusHit__VMT = reinterpret_cast<esv::StatusVMT const *>(p - 12 * 8);
			return SymbolMapper::MappingResult::Success;
		}
	}

	return SymbolMapper::MappingResult::Fail;
}

void LibraryManager::PreRegisterLibraries(SymbolMappingLoader& loader)
{
	loader.AddKnownModule("Main");
	loader.AddKnownModule("GameEngine");
	loader.AddKnownModule("CoreLib");
}

void LibraryManager::RegisterLibraries(SymbolMapper& mapper)
{
	mapper.AddModule("Main", L"EoCPlugin.dll");
	mapper.AddModule("GameEngine", L"GameEngine.dll");
	mapper.AddModule("CoreLib", L"CoreLib.dll");
	mapper.AddEngineCallback("FindLibraries", &FindLibrariesEoCPlugin);
	mapper.AddEngineCallback("FindActivateEntitySystem", &FindActivateEntitySystem);
	mapper.AddEngineCallback("FindCharacterStatGetters", &FindCharacterStatGetters);
	mapper.AddEngineCallback("FindActionMachineSetState", &FindActionMachineSetState);
	mapper.AddEngineCallback("FindActionMachineResetState", &FindActionMachineResetState);
	mapper.AddEngineCallback("FindStatusHeal", &FindStatusHeal);
	mapper.AddEngineCallback("FindStatusHit", &FindStatusHit);
}

bool LibraryManager::BindApp()
{
	HMODULE hEoCApp = GetModuleHandleW(L"EoCApp.exe");
	if (hEoCApp != NULL) {
		MessageBoxW(NULL, L"This version of the Script Extender can only be used in the editor.", L"Osiris Extender Error", MB_OK | MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 1);
	}

	HMODULE hDivinityEngine = GetModuleHandleW(L"DivinityEngine2.exe");
	if (hDivinityEngine == NULL) {
		return false;
	}

	symbolMapper_.AddModule("Main", L"EoCPlugin.dll");
	symbolMapper_.AddModule("GameEngine", L"GameEngine.dll");
	symbolMapper_.AddModule("CoreLib", L"CoreLib.dll");
	appModule_ = symbolMapper_.Modules().find("Main")->second;
	gameEngineModule_ = symbolMapper_.Modules().find("GameEngine")->second;
	coreLibModule_ = symbolMapper_.Modules().find("CoreLib")->second;

	return true;
}

void LibraryManager::FindServerGlobalsEoCPlugin()
{
	StaticSymbols::EoCLibraryInfo const * serverLib{ nullptr };

	// 1) Look for the string "esv::CustomStatDefinitionComponent"
	Pattern statComponent;
	statComponent.FromRaw("esv::CustomStatDefinitionComponent");
	uint8_t const * statComponentStr = nullptr;
	statComponent.Scan(appModule_.ModuleStart, appModule_.ModuleSize, [&statComponentStr](const uint8_t * match) -> Pattern::ScanAction {
		statComponentStr = match;
		return Pattern::ScanAction::Finish;
	});

	if (!statComponentStr) {
		ERR("Could not locate esv::CustomStatDefinitionComponent");
		return;
	}

	// 2) Look for xrefs to esv::CustomStatDefinitionComponent
	Pattern xref;
	xref.FromString(
		"48 8B C8 " // mov rcx, rax
		"48 8D 15 ?? ?? ?? ?? " // lea rdx, xxx
		"FF 15 ?? ?? ?? ?? " // call cs:xxx
	);

	uint8_t const * statComponentXref = nullptr;
	xref.Scan(appModule_.ModuleStart, appModule_.ModuleSize, [&statComponentXref, statComponentStr](const uint8_t * match) -> Pattern::ScanAction {
		int32_t rel = *(int32_t const *)(match + 6);
		uint8_t const * refTo = match + rel + 7 + 3;

		if (refTo == statComponentStr) {
			statComponentXref = match;
			return Pattern::ScanAction::Finish;
		} else {
			return Pattern::ScanAction::Continue;
		}
	});

	if (!statComponentXref) {
		ERR("Could not locate registration xref for esv::CustomStatDefinitionComponent");
		return;
	}

	Pattern p;
	p.FromString(
		"48 85 C0 " // test rax, rax
		"74 11 " // jz  xxx
		"48 8B C8 " // mov rcx, rax
		"E8 ?? ?? ?? ?? " // call ctor
		"48 89 05 ?? ?? ?? ?? " // mov cs:xxx, rax
	);

	uint32_t i = 0;
	p.Scan(statComponentXref, 0x1200, [this, &i](const uint8_t * match) -> Pattern::ScanAction {
		if (i < std::size(GetStaticSymbols().ServerGlobals)) {
			int32_t rel = *(int32_t const *)(match + 16);
			uint8_t const * refTo = match + rel + 7 + 13;
			GetStaticSymbols().ServerGlobals[i++] = (uint8_t const **)refTo;
		}

		return Pattern::ScanAction::Continue;
	});

	auto & serverGlobals = GetStaticSymbols().ServerGlobals;
	GetStaticSymbols().esv__InventoryFactory = (esv::InventoryFactory **)serverGlobals[(unsigned)EsvGlobalEoCPlugin::EsvInventoryFactory];
	GetStaticSymbols().esv__InventoryViewFactory = (esv::InventoryViewFactory **)serverGlobals[(unsigned)EsvGlobalEoCPlugin::EsvInventoryViewFactory];
	GetStaticSymbols().esv__SurfaceActionFactory = (esv::SurfaceActionFactory**)serverGlobals[(unsigned)EsvGlobalEoCPlugin::SurfaceActionFactory];

	if (GetStaticSymbols().esv__InventoryFactory == nullptr) {
		CriticalInitFailed = true;
	}
}

void LibraryManager::FindEoCGlobalsEoCPlugin()
{
	uint8_t const * globalsInitCode{ nullptr };
	size_t nextGlobalIndex = 0;
	for (auto const & lib : GetStaticSymbols().Libraries) {
		for (auto p = lib.second.initFunc; p < lib.second.initFunc + 0x300; p++) {
			if (p[0] == 0xE8
				&& p[5] == 0xE8
				&& p[10] == 0xE8
				&& p[15] == 0xE8
				&& p[20] == 0xE8
				&& p[25] == 0xE8
				&& p[30] == 0x48 && p[31] == 0x39 && p[32] == 0x1D) { /* cmp cs:xxx, rbx */

				for (auto p2 = p + 0x30; p2 < p + 0x240; p2++) {
					if (p2[0] == 0xE8 /* call xxx */
						&& p2[5] == 0x48 && p2[6] == 0x89 && p2[7] == 0x05 /* mov cs:xxx, rax */
						&& p2[12] == 0xEB /* jmp xxx */) {

						int32_t rel = *(int32_t *)(p2 + 8);
						uint8_t const * globalPtr = p2 + rel + 5 + 7;
						GetStaticSymbols().EocGlobals[nextGlobalIndex++] = (uint8_t const **)globalPtr;
						if (nextGlobalIndex >= std::size(GetStaticSymbols().EocGlobals)) {
							break;
						}
					}
				}

				break;
			}

			if (nextGlobalIndex >= std::size(GetStaticSymbols().EocGlobals)) {
				break;
			}
		}
	}

	if (nextGlobalIndex != std::size(GetStaticSymbols().EocGlobals)) {
		ERR("LibraryManager::FindEoCGlobalsEoCPlugin(): Could not find all eoc globals!");
		CriticalInitFailed = true;
	}
}

void LibraryManager::FindGlobalStringTableCoreLib()
{
	Pattern p;
	p.FromString(
		"B9 88 FA 5F 00 " // mov ecx, 5FFA88h
		"41 8D 51 01 " // lea edx, [r9+1]
		"E8 ?? ?? ?? ?? " // call alloc
		"48 85 C0 " // test rax, rax
		"74 1F " // jz short xxx
		"48 8B C8 " // mov rcx, rax
		"E8 ?? ?? ?? ?? " // call ctor
		"48 8D 4C 24 60 " // rcx, [rsp+58h+arg_0]
		"48 89 05 ?? ?? ?? ?? " // mov cs:xxx, rax
	);

	p.Scan(coreLibModule_.ModuleStart, coreLibModule_.ModuleSize, [this](const uint8_t * match) -> Pattern::ScanAction {
		int32_t rel = *(int32_t const *)(match + 35);
		auto refTo = match + rel + 7 + 32;

		GetStaticSymbols().ls__GlobalStrings = (GlobalStringTable const **)refTo;
		return Pattern::ScanAction::Finish;
	});

	if (GetStaticSymbols().ls__GlobalStrings == nullptr) {
		ERR("LibraryManager::FindGlobalStringTableCoreLib(): Could not find global string table");
		CriticalInitFailed = true;
	}
}

END_SE()
#endif
