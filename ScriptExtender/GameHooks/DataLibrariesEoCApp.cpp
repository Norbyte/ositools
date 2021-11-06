#include <stdafx.h>
#include <GameHooks/DataLibraries.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Symbols.h>
#include <functional>
#include <psapi.h>

#if defined(OSI_EOCAPP)
namespace dse
{
	void LibraryManager::PreRegisterLibraries(SymbolMappingLoader& loader)
	{
		loader.AddKnownModule("Main");
	}

	void LibraryManager::RegisterLibraries(SymbolMapper& mapper)
	{
		mapper.AddModule("Main", L"EoCApp.exe");
	}

	bool LibraryManager::BindApp()
	{
		HMODULE hEoCPlugin = GetModuleHandleW(L"EoCPlugin.dll");
		if (hEoCPlugin != NULL) {
			MessageBoxW(NULL, L"This version of the Script Extender can only be used ingame.", L"Osiris Extender Error", MB_OK | MB_ICONERROR);
			TerminateProcess(GetCurrentProcess(), 1);
		}

		symbolMapper_.AddModule("Main", L"EoCApp.exe");
		appModule_ = symbolMapper_.Modules().find("Main")->second;

		return true;
	}

	void LibraryManager::FindServerGlobalsEoCApp()
	{
		StaticSymbols::EoCLibraryInfo const * serverLib{ nullptr };
		auto & sym = GetStaticSymbols();
		for (auto const & v : sym.Libraries) {
			if (v.second.refs > 100) {
				serverLib = &v.second;
				break;
			}
		}

		if (serverLib == nullptr) {
			Fail("LibraryManager::FindServerGlobalsEoCApp(): Could not find esv library!");
			return;
		}

		for (auto p = serverLib->initFunc; p < serverLib->initFunc + 0x1000; p++) {
			if (p[0] == 0x48 && p[1] == 0x89 && p[2] == 0x05
				&& p[7] == 0xEB && p[8] == 0x07
				&& p[9] == 0x48 && p[10] == 0x89
				&& p[16] == 0xE8
				&& p[21] == 0xE8
				&& p[26] == 0xE8
				&& p[31] == 0xE8
				&& p[36] == 0xE8) {

				uint8_t const * funcPtr = p + 16;
				for (auto i = 0; i < 36; i++) {
					int32_t rel = *(int32_t *)(funcPtr + 1);
					uint8_t const * registrantFunc = funcPtr + rel + 5;
					sym.ServerRegisterFuncs[i] = registrantFunc;
					funcPtr += 5;
				}

				funcPtr = p + 16 + (36 * 5) + 0x2C;
				for (auto i = 0; i < 14; i++) {
					int32_t rel = *(int32_t *)(funcPtr + 1);
					uint8_t const * registrantFunc = funcPtr + rel + 5;
					sym.ServerRegisterFuncs[i + 36] = registrantFunc;
					funcPtr += 5;
				}

				for (auto i = 0; i < std::size(GetStaticSymbols().ServerRegisterFuncs); i++) {
					uint8_t const * r = GetStaticSymbols().ServerRegisterFuncs[i];
					if (r[0] == 0x48 && r[1] == 0x83 && r[2] == 0xEC /* sub rsp, XXh */
						&& r[4] == 0x48 && r[5] == 0x8B && r[6] == 0x05 /* mov rax, cs:xxx */) {

						int32_t rel = *(int32_t *)(r + 7);
						uint8_t const * registrantObj = r + rel + 4 + 7;
						sym.ServerGlobals[i] = (uint8_t const **)registrantObj;

					}
					else {
						sym.ServerGlobals[i] = nullptr;
					}
				}
			}
		}

		sym.esv__InventoryFactory = (esv::InventoryFactory **)sym.ServerGlobals[(unsigned)EsvGlobalEoCApp::EsvInventoryFactory];
		sym.esv__SurfaceActionFactory = (esv::SurfaceActionFactory**)sym.ServerGlobals[(unsigned)EsvGlobalEoCApp::SurfaceActionFactory];

		if (sym.esv__InventoryFactory == nullptr) {
			CriticalInitFailed = true;
		}
	}

	void LibraryManager::FindEoCGlobalsEoCApp()
	{
		static uint8_t const libInitSig[] = {
			0x48, 0x89, 0x18,       // mov     [rax], rbx
			0x48, 0x89, 0x58, 0x08, // mov     [rax+8], rbx
			0x48, 0x89, 0x58, 0x10, // mov     [rax+10h], rbx
			0x48, 0x8D, 0x40, 0x40, // lea     rax, [rax+40h]
			0x48, 0x89, 0x58, 0xD8, // mov     [rax-28h], rbx
			0x48, 0x89, 0x58, 0xE0  // mov     [rax-20h], rbx
		};

		uint8_t const * globalsInitCode{ nullptr };
		for (auto const & lib : GetStaticSymbols().Libraries) {
			for (auto p = lib.second.initFunc; p < lib.second.initFunc + 0x300; p++) {
				if (p[0] == 0x48 && memcmp(p, libInitSig, sizeof(libInitSig)) == 0) {

					for (auto p2 = p + 0x20; p2 < p + 0x120; p2++) {
						if (p2[0] == 0xE8
							&& p2[5] == 0xE8
							&& p2[10] == 0xE8
							&& p2[15] == 0xE8
							&& p2[20] == 0xE8) {
							globalsInitCode = p2;
							break;
						}
					}
				}
			}
		}

		if (globalsInitCode == nullptr) {
			Fail("LibraryManager::FindEoCGlobalsEoCApp(): Could not find eoc library init function!");
			return;
		}

		for (auto i = 1; i < std::size(GetStaticSymbols().EocRegisterFuncs); i++) {
			int32_t rel = *(int32_t *)(globalsInitCode + 1);
			uint8_t const * registrantFunc = globalsInitCode + rel + 5;
			GetStaticSymbols().EocRegisterFuncs[i] = registrantFunc;
			globalsInitCode += 5;
		}

		for (auto i = 1; i < std::size(GetStaticSymbols().EocRegisterFuncs); i++) {
			uint8_t const * r = GetStaticSymbols().EocRegisterFuncs[i];
			if (r[0] == 0x48 && r[1] == 0x83 && r[2] == 0xEC /* sub rsp, XXh */
				&& r[4] == 0x48 && r[5] == 0x8B && r[6] == 0x05 /* mov rax, cs:xxx */) {

				int32_t rel = *(int32_t *)(r + 7);
				uint8_t const * registrantObj = r + rel + 4 + 7;
				GetStaticSymbols().EocGlobals[i] = (uint8_t const **)registrantObj;

			} else {
				GetStaticSymbols().EocGlobals[i] = nullptr;
			}
		}
	}

	void LibraryManager::FindGlobalStringTableEoCApp()
	{
		static uint8_t const sig1[] = {
			0xB9, 0x88, 0xFA, 0x5F, 0x00, // mov ecx, 5FFA88h
			0xE8, // call xxx
		};

		static uint8_t const sig2[] = {
			0x48, 0x85, 0xC0, //  test rax, rax
			0x74, 0x14, // jz xxx
			0x48, 0x8B, 0xC8, // mov rcx, rax
			0xE8, // call xxx
		};

		static uint8_t const sig3[] = {
			0x48, 0x89, 0x05 // mov xxx, rax
		};

		uint8_t const * p = appModule_.ModuleTextStart;
		uint8_t const * moduleEnd = p + appModule_.ModuleTextSize;

		for (; p < moduleEnd - 100; p++) {
			if (*p == 0xB9
				&& memcmp(p, sig1, sizeof(sig1)) == 0
				&& memcmp(p + 10, sig2, sizeof(sig2)) == 0
				&& memcmp(p + 23, sig3, sizeof(sig3)) == 0) {
				int32_t rel = *(int32_t *)(p + 26);

				GetStaticSymbols().ls__GlobalStrings = (GlobalStringTable const **)(p + rel + 23 + 7);
				break;
			}
		}

		if (GetStaticSymbols().ls__GlobalStrings == nullptr) {
			ERR("LibraryManager::FindGlobalStringTableEoCApp(): Could not find global string table");
			CriticalInitFailed = true;
		}
	}
}
#endif
