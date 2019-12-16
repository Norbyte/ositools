#include "stdafx.h"
#include "DataLibraries.h"
#include <string>
#include <functional>
#include <psapi.h>

#if defined(OSI_EOCAPP)
namespace osidbg
{
	bool LibraryManager::FindEoCApp(uint8_t const * & start, size_t & size)
	{
		HMODULE hEoCPlugin = GetModuleHandleW(L"EoCPlugin.dll");
		if (hEoCPlugin != NULL) {
			MessageBoxW(NULL, L"This version of the Osiris Extender can only be used ingame.", L"Osiris Extender Error", MB_OK | MB_ICONERROR);
			TerminateProcess(GetCurrentProcess(), 1);
		}

		HMODULE hEoCApp = GetModuleHandleW(L"EoCApp.exe");
		if (hEoCApp == NULL) {
			return false;
		}

		MODULEINFO moduleInfo;
		if (!GetModuleInformation(GetCurrentProcess(), hEoCApp, &moduleInfo, sizeof(moduleInfo))) {
			Fail(L"Could not get module info of EoCApp.exe");
		}

		start = (uint8_t const *)moduleInfo.lpBaseOfDll;
		size = moduleInfo.SizeOfImage;
		return true;
	}


	void LibraryManager::FindMemoryManagerEoCApp()
	{
		Pattern p;
		p.FromString(
			"89 41 10 " // mov     [rcx+10h], eax
			"E8 XX XX XX XX " // call    ls__GlobalAllocator__Free
			"48 8B 47 08 " // mov     rax, [rdi+8]
			"BA 28 01 00 00 " // mov     edx, 128h
			"48 89 46 08 " // mov     [rsi+8], rax
			"48 C7 47 08 00 00 00 00 " // mov     qword ptr [rdi+8], 0
			"C7 07 00 00 00 00 " // mov     dword ptr [rdi], 0
			"E8 XX XX XX XX " // call    ls__GlobalAllocator__Malloc
			"33 D2 " // xor     edx, edx 
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			EoCAlloc = (EoCAllocFunc)AsmCallToAbsoluteAddress(match + 35);
			EoCFree = (EoCFreeFunc)AsmCallToAbsoluteAddress(match + 3);
		});

		if (EoCAlloc == nullptr || EoCFree == nullptr) {
			ERR("Could not find memory management functions");
			CriticalInitFailed = true;
		}
	}


	void LibraryManager::FindLibrariesEoCApp()
	{
		uint8_t const prologue1[] = {
			0x8B, 0xC8, // mov ecx, eax
			0xFF, 0xC0, // inc eax
			0x89, 0x05 // mov cs:xxx, eax
		};

		uint8_t const prologue2[] = {
			0x85, 0xC9, // test ecx, ecx
			0x75, 0x17, // jnz short loc_xxx
			0x44, 0x8D, 0x41 // lea r8d, [rdx+XXh]
		};

		uint8_t const prologue3[] = {
			0x48, 0x8D, 0x0D // lea rcx, xxx
		};

		uint8_t const * p = (uint8_t const *)moduleStart_;
		uint8_t const * moduleEnd = p + moduleSize_;

		for (; p < moduleEnd - 100; p++) {
			if (*p == 0x8B
				// && memcmp(p, prologue0, sizeof(prologue0)) == 0
				&& memcmp(p + 0x06, prologue1, sizeof(prologue1)) == 0
				&& memcmp(p + 0x10, prologue2, sizeof(prologue2)) == 0
				&& memcmp(p + 0x18, prologue3, sizeof(prologue3)) == 0) {
				int32_t rel1 = *(int32_t *)(p + 0x1B);
				int32_t rel2 = *(int32_t *)(p + 0x22);

				uint8_t const * initFunc = p + rel1 + 0x18 + 7;
				uint8_t const * freeFunc = p + rel2 + 0x1F + 7;

				auto it = libraries_.find(initFunc);
				if (it != libraries_.end()) {
					it->second.refs++;
				}
				else {
					libraries_.insert(std::pair<uint8_t const *, EoCLibraryInfo>(initFunc, { initFunc, freeFunc, 1 }));
				}
			}
		}

#if 0
		DEBUG("LibraryManager::FindLibrariesEoCApp(): Found libraries:");
		for (auto const & v : libraries_) {
			DEBUG("\t(Init %p; Dtor %p, Refs %d)!", v.second.initFunc, v.second.freeFunc, v.second.refs);
		}
#endif
	}

	void LibraryManager::FindServerGlobalsEoCApp()
	{
		EoCLibraryInfo const * serverLib{ nullptr };
		for (auto const & v : libraries_) {
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
					serverRegisterFuncs_[i] = registrantFunc;
					funcPtr += 5;
				}

				funcPtr = p + 16 + (36 * 5) + 0x2C;
				for (auto i = 0; i < 14; i++) {
					int32_t rel = *(int32_t *)(funcPtr + 1);
					uint8_t const * registrantFunc = funcPtr + rel + 5;
					serverRegisterFuncs_[i + 36] = registrantFunc;
					funcPtr += 5;
				}

				for (auto i = 0; i < std::size(serverRegisterFuncs_); i++) {
					uint8_t const * r = serverRegisterFuncs_[i];
					if (r[0] == 0x48 && r[1] == 0x83 && r[2] == 0xEC /* sub rsp, XXh */
						&& r[4] == 0x48 && r[5] == 0x8B && r[6] == 0x05 /* mov rax, cs:xxx */) {

						int32_t rel = *(int32_t *)(r + 7);
						uint8_t const * registrantObj = r + rel + 4 + 7;
						serverGlobals_[i] = (uint8_t const **)registrantObj;

					}
					else {
						serverGlobals_[i] = nullptr;
						DEBUG("LibraryManager::FindServerGlobalsEoCApp(): Could not extract global from func @ %p", r);
					}
				}
			}
		}

		EsvCharacterFactory = (CharacterFactory **)serverGlobals_[(unsigned)EsvGlobalEoCApp::EsvCharacterFactory];
		EsvItemFactory = (ItemFactory **)serverGlobals_[(unsigned)EsvGlobalEoCApp::EsvItemFactory];

		if (EsvCharacterFactory == nullptr || EsvItemFactory == nullptr) {
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
		for (auto const & lib : libraries_) {
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

		for (auto i = 1; i < std::size(eocRegisterFuncs_); i++) {
			int32_t rel = *(int32_t *)(globalsInitCode + 1);
			uint8_t const * registrantFunc = globalsInitCode + rel + 5;
			eocRegisterFuncs_[i] = registrantFunc;
			globalsInitCode += 5;
		}

		for (auto i = 1; i < std::size(eocRegisterFuncs_); i++) {
			uint8_t const * r = eocRegisterFuncs_[i];
			if (r[0] == 0x48 && r[1] == 0x83 && r[2] == 0xEC /* sub rsp, XXh */
				&& r[4] == 0x48 && r[5] == 0x8B && r[6] == 0x05 /* mov rax, cs:xxx */) {

				int32_t rel = *(int32_t *)(r + 7);
				uint8_t const * registrantObj = r + rel + 4 + 7;
				eocGlobals_[i] = (uint8_t const **)registrantObj;

			}
			else {
				eocGlobals_[i] = nullptr;
				DEBUG("LibraryManager::FindEoCGlobalsEoCApp(): Could not extract global from func @ %p", r);
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

		uint8_t const * p = (uint8_t const *)moduleStart_;
		uint8_t const * moduleEnd = p + moduleSize_;

		for (; p < moduleEnd - 100; p++) {
			if (*p == 0xB9
				&& memcmp(p, sig1, sizeof(sig1)) == 0
				&& memcmp(p + 10, sig2, sizeof(sig2)) == 0
				&& memcmp(p + 23, sig3, sizeof(sig3)) == 0) {
				int32_t rel = *(int32_t *)(p + 26);

				GlobalStrings = (GlobalStringTable const **)(p + rel + 23 + 7);
				GlobalStringTable::UseMurmur = true;
				break;
			}
		}

		if (GlobalStrings == nullptr) {
			ERR("LibraryManager::FindGlobalStringTableEoCApp(): Could not find global string table");
			CriticalInitFailed = true;
		}

		Pattern p2;
		p2.FromString(
			"48 8B F0 " // mov     rsi, rax
			"48 85 C0 " // test    rax, rax
			"0F XX XX XX 00 00 " // jz      loc_xxx
			"83 CA FF " // or      edx, 0FFFFFFFFh
			"4C 89 74 XX 60 " // mov     [rsp+0DB0h+var_D50], r14
			"48 8D 0D XX XX XX XX " // lea     rcx, str_Damage ; "Damage"
			"E8 XX XX XX XX " // call    ls__FixedString__Create
			"83 CA FF " // or      edx, 0FFFFFFFFh
		);

		p2.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto damage = (char const *)AsmLeaToAbsoluteAddress(match + 20);
			if (damage && strcmp(damage, "Damage") == 0) {
				auto func = AsmCallToAbsoluteAddress(match + 27);
				CreateFixedString = (ls__FixedString__Create)func;
			}
		}, false);


		if (CreateFixedString == nullptr) {
			ERR("LibraryManager::FindGlobalStringTableEoCApp(): Could not find ls::FixedString::Create");
			CriticalInitFailed = true;
		}
	}

	void LibraryManager::FindFileSystemEoCApp()
	{
		Pattern p;
		p.FromString(
			"4C 89 6D 98 " // mov     [rbp+180h+var_1E8], r13
			"44 89 6D A0 " // mov     [rbp+180h+var_1E0], r13d
			"44 89 6D A8 " // mov     [rbp+180h+var_1D8], r13d
			"4C 89 6D B0 " // mov     [rbp+180h+var_1D0], r13
			"C6 44 24 60 00 " // mov     [rsp+280h+var_220], 0
			"E8 XX XX XX XX " // call    ls__FileReader__FileReader
			"48 8D 54 24 50 " // lea     rdx, [rsp+280h+Src]
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto fileReaderProc = AsmCallToAbsoluteAddress(match + 21);
			FileReaderCtor = (ls__FileReader__FileReader)fileReaderProc;
		}, false);

		Pattern p2;
		p2.FromString(
			"48 83 EC 20 " // sub     rsp, 20h
			"0F B7 C2 " // movzx   eax, dx
			"48 8B D9 " // mov     rbx, rcx
			"48 8D 0D XX XX XX XX " // lea     rcx, ls__Path__s_Roots
			"49 8B F8 " // mov     rdi, r8
			"48 8D 34 C1 " // lea     rsi, [rcx+rax*8]
		);

		p2.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto roots = AsmLeaToAbsoluteAddress(match + 10);
			PathRoots = (STDString **)roots;
		}, false);

		// TODO - find FileReaderDtor
		if (PathRoots == nullptr || FileReaderCtor == nullptr) {
			ERR("LibraryManager::FindFileSystemEoCApp(): Could not find filesystem functions");
			CriticalInitFailed = true;
		}
	}

	std::string LibraryManager::ToPath(std::string const & path, PathRootType root) const
	{
		if (PathRoots == nullptr) {
			ERR("LibraryManager::ToPath(): Path root API not available!");
			return "";
		}

		auto rootPath = PathRoots[(unsigned)root];

		std::string absolutePath = rootPath->GetPtr();
		absolutePath += "/" + path;
		return absolutePath;
	}

	FileReaderPin LibraryManager::MakeFileReader(std::string const & path, PathRootType root) const
	{
		if (PathRoots == nullptr || FileReaderCtor == nullptr) {
			ERR("LibraryManager::MakeFileReader(): File reader API not available!");
			return FileReaderPin(nullptr);
		}

		auto absolutePath = ToPath(path, root);

		Path lsPath;
		lsPath.Name.Set(absolutePath);

		auto reader = new FileReader();
		FileReaderCtor(reader, &lsPath, 2);
		return FileReaderPin(reader);
	}

	void LibraryManager::FindGameActionManagerEoCApp()
	{
		Pattern p;
		p.FromString(
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_GameAction
			"XX 8B XX " // mov     xx, xx
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"84 C0 " // test    al, al
			"0F 84 XX XX 00 00 " // jz      xxx
			"84 DB " // test    bl, bl
			"74 XX " // jz      short loc_xxx
			"4C 8B 06 " // mov     r8, [rsi]
			"49 8B XX " // mov     rcx, xx
			"8B 56 08 " // mov     edx, [rsi+8]
			"E8 XX XX XX XX " // call    esv__GameActionManager__CreateAction
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match);
			if (IsFixedStringRef(fsx, "GameAction")) {
				auto actionAddr = AsmCallToAbsoluteAddress(match + 37);
				CreateGameAction = (esv::GameActionManager__CreateAction)actionAddr;
			}
		});

		if (CreateGameAction == nullptr) {
			Pattern alternate;
			alternate.FromString(
				"48 8D 15 XX XX XX XX " // lea     rdx, fsx_GameAction
				"XX 8B XX " // mov     xx, xx
				"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
				"84 C0 " // test    al, al
				"0F 84 XX XX 00 00 " // jz      xxx
				"45 84 FF " // test    r15b, r15b
				"74 XX " // jz      short loc_xxx
				"4C 8B 06 " // mov     r8, [rsi]
				"49 8B XX " // mov     rcx, xx
				"8B 56 08 " // mov     edx, [rsi+8]
				"E8 XX XX XX XX " // call    esv__GameActionManager__CreateAction
			);

			alternate.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
				auto fsx = AsmLeaToAbsoluteAddress(match);
				if (IsFixedStringRef(fsx, "GameAction")) {
					auto actionAddr = AsmCallToAbsoluteAddress(match + 38);
					CreateGameAction = (esv::GameActionManager__CreateAction)actionAddr;
				}
			});

			if (CreateGameAction == nullptr) {
				ERR("LibraryManager::FindGameActionManagerEoCApp(): Could not find GameActionManager::CreateAction");
				InitFailed = true;
			}
		}

		Pattern p2;
		p2.FromString(
			"0F 84 XX 00 00 00 " // jz      xxx
			"4C 8B 00 " // mov     r8, [rax]
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_ForceMove
			"48 8B C8 " // mov     rcx, rax
			"41 FF 50 28 " // call    qword ptr [r8+28h]
			"4D 8B 06 " // mov     r8, [r14]
			"49 8B CE " // mov     rcx, r14
		);

		Pattern p3;
		p3.FromString(
			"48 8B 05 XX XX XX XX " // mov     rax, cs:ls__gServerLevelAllocator
			"4C 8D 44 24 60 " // lea     r8, [rsp+58h+arg_0]
			"48 8D 54 24 70 " // lea     rdx, [rsp+58h+arg_10]
			"C7 44 24 60 FF FF FF FF " // mov     dword ptr [rsp+58h+arg_0], 0FFFFFFFFh
			"48 8B 88 80 00 00 00 " //  mov     rcx, [rax+80h]
			"48 8B 05 XX XX XX XX " // mov     rax, cs:ls__ObjectHandle__Unassigned
			"48 89 44 24 70 " // mov     [rsp+58h+arg_10], rax
			"48 8B B9 D0 00 00 00 " // mov     rdi, [rcx+0D0h]
			"48 8B CF " // mov     rcx, rdi
			"E8 XX XX XX XX " // call    xxx
			"4C 8D 46 70 " // lea     r8, [rsi+70h]
			"48 8B C8 " // mov     rcx, rax
			"48 8D 56 68 " // lea     rdx, [rsi+68h]
			"48 8B D8 " // mov     rbx, rax
			"E8 XX XX XX XX " // call    esv__GameObjectMoveAction__Setup
			"48 8B D3 " // mov     rdx, rbx
			"48 8B CF " // mov     rcx, rdi
			"E8 XX XX XX XX " // call    esv__GameActionManager__AddAction
		);

		p2.Scan(moduleStart_, moduleSize_, [this, &p3](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 9);
			if (IsFixedStringRef(fsx, "ForceMove")) {
				p3.Scan(match, 0x100, [this](const uint8_t * match2) {
					LevelManager = (void **)AsmLeaToAbsoluteAddress(match2);

					auto moveSetupAddr = AsmCallToAbsoluteAddress(match2 + 73);
					GameObjectMoveActionSetup = (esv::GameObjectMoveAction__Setup)moveSetupAddr;

					auto addActionAddr = AsmCallToAbsoluteAddress(match2 + 84);
					AddGameAction = (esv::GameActionManager__AddAction)addActionAddr;
				});

				if (LevelManager == nullptr || AddGameAction == nullptr) {
					Pattern alternate;
					alternate.FromString(
						"48 8B 05 XX XX XX XX " // mov     rax, cs:ls__gServerLevelAllocator
						"4C 8D 44 24 60 " // lea     r8, [rsp+58h+arg_0]
						"48 8D 54 24 70 " // lea     rdx, [rsp+58h+arg_10]
						"C7 44 24 60 FF FF FF FF " // mov     dword ptr [rsp+58h+arg_0], 0FFFFFFFFh
						"48 8B 88 80 00 00 00 " //  mov     rcx, [rax+80h]
						"48 8B 05 XX XX XX XX " // mov     rax, cs:ls__ObjectHandle__Unassigned
						"48 89 44 24 70 " // mov     [rsp+58h+arg_10], rax
						"48 8B B9 D0 00 00 00 " // mov     rdi, [rcx+0D0h]
						"48 8B CF " // mov     rcx, rdi
						"E8 XX XX XX XX " // call    xxx
						"4C 8D 46 70 " // lea     r8, [rsi+70h]
						"48 8B C8 " // mov     rcx, rax
						"48 8D 56 68 " // lea     rdx, [rsi+68h]
						"48 8B D8 " // mov     rbx, rax
						"E8 XX XX XX XX " // call    esv__GameObjectMoveAction__Setup
						"48 8D 8F 30 01 00 00 " // lea     rcx, [rdi+130h]
						"48 89 5C 24 60 " // mov     [rsp+58h+arg_0], rbx
						"48 8D 54 24 60 " // lea     rdx, [rsp+58h+arg_0]
						"E8 XX XX XX XX " // call    esv__GameActionManager__AddAction
					);

					alternate.Scan(match, 0x100, [this](const uint8_t * match2) {
						LevelManager = (void **)AsmLeaToAbsoluteAddress(match2);

						auto moveSetupAddr = AsmCallToAbsoluteAddress(match2 + 73);
						GameObjectMoveActionSetup = (esv::GameObjectMoveAction__Setup)moveSetupAddr;

						auto addActionAddr = AsmCallToAbsoluteAddress(match2 + 95);
						AddGameAction = (esv::GameActionManager__AddAction)addActionAddr;
					});
				}
			}
		});

		if (LevelManager == nullptr || AddGameAction == nullptr) {
			ERR("LibraryManager::FindGameActionManagerEoCApp(): Could not find esv::LevelManager");
			InitFailed = true;
		}
	}

	void LibraryManager::FindGameActionsEoCApp()
	{
		Pattern p;
		p.FromString(
			"EB XX " // jmp     short xxx
			"48 8B 00 " // mov     rax, [rax]
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_RandomPoints
			"48 8B XX " // mov     rcx, rbx
			"FF 50 28 " // call    qword ptr [rax+28h]
			"4C 8B XX " // mov     r8, [rbx]
		);

		Pattern p2;
		p2.FromString(
			"48 8B C4 " // mov     rax, rsp
			"53 " // push    rbx
		);

		p.Scan(moduleStart_, moduleSize_, [this, &p2](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 5);
			if (IsFixedStringRef(fsx, "RandomPoints")) {
				p2.Scan(match - 0x200, 0x200, [this](const uint8_t * match2) {
					TornadoActionSetup = (esv::TornadoAction__Setup)match2;
				});
			}
		});

		if (TornadoActionSetup == nullptr) {
			ERR("LibraryManager::FindGameActionsEoCApp(): Could not find TornadoAction");
			InitFailed = true;
		}

		// FIXME: WallAction, SummonHelpers::Summon
	}

	void LibraryManager::FindStatusMachineEoCApp()
	{
		Pattern p;
		p.FromString(
			"83 7A 1C 00 " // cmp     dword ptr [rdx+1Ch], 0
			"48 8B F2 " // mov     rsi, rdx
			"4C 8B F1 " // mov     r14, rcx
			"0F 8E 8C 00 00 00 " // jle     short xxx
			"4C 8B 05 XX XX XX XX " // mov     r8, cs:?Unassigned@ObjectHandle@ls@@2V12@B
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_LIFESTEAL
			"48 89 5C 24 30 " //  mov     [rsp+28h+arg_0], rbx
			"48 89 7C 24 40 " //  mov     [rsp+28h+arg_10], rdi
			"48 8B B9 A0 01 00 00 " //  mov     rdi, [rcx+1A0h]
			"48 8B CF " //  mov     rcx, rdi 
			"E8 XX XX XX XX " //  call    esv__StatusMachine__CreateStatus
		);

		uint8_t const * lastMatch;
		p.Scan(moduleStart_, moduleSize_, [this, &lastMatch](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 23);
			if (IsFixedStringRef(fsx, "LIFESTEAL")) {
				auto actionAddr = AsmCallToAbsoluteAddress(match + 50);
				lastMatch = match + 55;
				StatusMachineCreateStatus = (esv::StatusMachine__CreateStatus)actionAddr;
			}
		});

		if (StatusMachineCreateStatus == nullptr) {
			ERR("LibraryManager::FindStatusMachineEoCApp(): Could not find StatusMachine::CreateStatus");
			InitFailed = true;
		}

		Pattern p2;
		p2.FromString(
			"C7 43 2C 00 00 00 00 " // mov     dword ptr [rbx+2Ch], 0
			"48 8B CF " // mov     rcx, rdi
			"E8 XX XX XX XX " // call    esv__StatusMachine__ApplyStatus
			"48 8B 7C 24 40 " // mov     rdi, [rsp+28h+arg_10]
		);

		p2.Scan(lastMatch, 0x100, [this](const uint8_t * match) {
			auto actionAddr = AsmCallToAbsoluteAddress(match + 10);
			StatusMachineApplyStatus = (esv::StatusMachine__ApplyStatus)actionAddr;
		});

		if (StatusMachineApplyStatus == nullptr) {
			ERR("LibraryManager::FindStatusMachineEoCApp(): Could not find StatusMachine::ApplyStatus");
			InitFailed = true;
		}
	}

	void LibraryManager::FindStatusTypesEoCApp()
	{
		Pattern p;
		p.FromString(
			"45 33 C9 " // xor     r9d, r9d
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_TargetDependentHeal
			"48 8B CB " // mov     rcx, rbx
			"FF 90 B0 01 00 00 " // call    qword ptr [rax+1B0h]
		);

		Pattern p2;
		p2.FromString(
			"48 89 5C 24 10 " // mov     [rsp-8+arg_8], rbx
			"48 89 74 24 18 " // mov     [rsp-8+arg_10], rsi
		);

		p.Scan(moduleStart_, moduleSize_, [this, &p2](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 3);
			if (IsFixedStringRef(fsx, "TargetDependentHeal")) {
				p2.Scan(match - 0x200, 0x200, [this](const uint8_t * match) {
					// Look for this function ptr
					auto ptr = (uint64_t)match;
					for (auto p = moduleStart_; p < moduleStart_ + moduleSize_; p += 8) {
						if (*reinterpret_cast<uint64_t const *>(p) == ptr) {
							StatusHealVMT = reinterpret_cast<esv::StatusVMT const *>(p - 25 * 8);
						}
					}
				});
			}
		});

		if (StatusHealVMT == nullptr) {
			ERR("LibraryManager::FindStatusTypesEoCApp(): Could not find esv::StatusHeal");
			InitFailed = true;
		}

		Pattern p3;
		p3.FromString(
			"4C 8D 0D XX XX XX XX " // lea     r9, fsx_Dummy_BodyFX
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_RS3_FX_GP_Status_Retaliation_Beam_01
			"E8 XX XX XX XX " // call    esv__EffectFactory__CreateEffectWrapper
			"48 8B D8 " // mov     rbx, rax
		);

		Pattern p4;
		p4.FromString(
			"40 55 " // push    rbp
			"41 54 " // push    r12
			"41 55 " // push    r13
			"41 57 " // push    r15
		);

		p3.Scan(moduleStart_, moduleSize_, [this, &p4](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 7);
			if (IsFixedStringRef(fsx, "RS3_FX_GP_Status_Retaliation_Beam_01")) {
				p4.Scan(match - 0xa00, 0xa00, [this](const uint8_t * match) {
					// Look for this function ptr
					auto ptr = (uint64_t)match;
					for (auto p = moduleStart_; p < moduleStart_ + moduleSize_; p += 8) {
						if (*reinterpret_cast<uint64_t const *>(p) == ptr) {
							StatusHitVMT = reinterpret_cast<esv::StatusVMT const *>(p - 12 * 8);
						}
					}
				});
			}
		});

		if (StatusHitVMT == nullptr) {
			ERR("LibraryManager::FindStatusTypesEoCApp(): Could not find esv::StatusHit");
			InitFailed = true;
		}
	}

	void LibraryManager::FindHitFuncsEoCApp()
	{
		Pattern p;
		p.FromString(
			"74 74 " // jz      short loc_141A3EED4
			"48 8B 00 " // mov     rax, [rax]
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_DamageItems
			"48 8B CE " // mov     rcx, rsi
			"FF 50 28 " // call    qword ptr [rax+28h]
		);

		Pattern p2;
		p2.FromString(
			"C7 44 24 20 05 00 00 00 " // mov     dword ptr [rsp+140h+var_120], 5
			"44 89 65 50 " // mov     [rbp+40h+arg_0], r12d
			"E8 XX XX XX XX " // call    esv__Character__Hit
			"4C 8B AE A0 01 00 00 " // mov     r13, [rsi+1A0h]
			"EB 5F " // jmp short xxx
		);

		p.Scan(moduleStart_, moduleSize_, [this, &p2](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 5);
			if (IsFixedStringRef(fsx, "DamageItems")) {
				p2.Scan(match, 0x400, [this](const uint8_t * match) {
					auto actionAddr = AsmCallToAbsoluteAddress(match + 12);
					CharacterHit = (esv::Character__Hit)actionAddr;
				});
			}
		});

		if (CharacterHit == nullptr) {
			ERR("LibraryManager::FindHitFuncsEoCApp(): Could not find Character::Hit");
			InitFailed = true;
		}
	}

	void LibraryManager::FindItemFuncsEoCApp()
	{
		Pattern p;
		p.FromString(
			"45 0F B6 C1 " // movzx   r8d, r9b
			"48 8B CF " // mov     rcx, rdi
			"E8 XX XX XX XX " // call    esv__ParseItem
			"33 D2 " // xor     edx, edx
			"48 8D 4C 24 38 " // lea     rcx, [rsp+78h+var_40]
			"E8 XX XX XX XX " // call    esv__CreateItemFromParsed
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto parseAddr = AsmCallToAbsoluteAddress(match + 7);
			ParseItem = (esv::ParseItem)parseAddr;
			auto createAddr = AsmCallToAbsoluteAddress(match + 19);
			CreateItemFromParsed = (esv::CreateItemFromParsed)createAddr;
		}, false);

		if (ParseItem == nullptr || CreateItemFromParsed == nullptr) {
			ERR("LibraryManager::FindItemFuncsEoCApp(): Could not find esv::CreateItemFromParsed");
			InitFailed = true;
		}
	}


	void LibraryManager::FindCustomStatsEoCApp()
	{
		Pattern p;
		p.FromString(
			"48 89 5D 60 " // mov     [rbp+0D0h+var_70], rbx
			"0F B6 88 B0 00 00 00 " // movzx   ecx, byte ptr [rax+0B0h]
			"41 3A CF " // cmp     cl, r15b
			"F2 0F 11 75 08 " // movsd   [rbp+0D0h+var_C8], xmm6
			"48 8B 4E 18 " // mov     rcx, [rsi+18h]
			"C7 45 B0 03 00 00 00 " // mov     [rbp+0D0h+var_120], 3
			// Replacement: c6 45 f8 01             mov    BYTE PTR [rbp-0x8],0x1
			"0F 94 45 F8 " // setz    [rbp+0D0h+var_D8]
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			UICharacterSheetHook = match + 30;
		}, false);

		Pattern p2;
		p2.FromString(
			"48 03 93 D0 00 00 00 " // add     rdx, [rbx+0D0h]
			"E8 XX XX XX XX " // call    ecs__EntityWorld__ActivateSystemInternal
			"48 8B 05 XX XX XX XX " // mov     rax, cs:ls__GlobalSwitches
			"80 B8 F3 0B 00 00 01 " // cmp     byte ptr [rax+0BF3h], 1
			// Replacement: 90 90
			"75 XX " // jnz     short loc_1415A124C
			"48 6B 15 XX XX XX XX 38 " // imul    rdx, cs:qword_1428F14F0, 38h
			"48 8B CB " // mov     rcx, rbx
		);

		p2.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			if (ActivateClientSystemsHook == nullptr) {
				ActivateClientSystemsHook = match + 26;
			} else {
				ActivateServerSystemsHook = match + 26;
			}
		});

		Pattern p3;
		p3.FromString(
			// Replacement: C3 (retn)
			"4C 8B DC " // mov     r11, rsp
			"48 81 EC 88 00 00 00 " // sub     rsp, 88h
			"48 8B 05 XX XX XX XX " // mov     rax, cs:__security_cookie
			"48 33 C4 " // xor     rax, rsp
			"48 89 44 24 70 " // mov     [rsp+88h+var_18], rax
			"48 8B 05 XX XX XX XX " // mov     rax, cs:?s_Ptr@PlayerManager@ls@@1PEAV12@EA
			"49 8D 53 A8 " // lea     rdx, [r11-58h]
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:qword_142940598
			"49 89 43 98 " // mov     [r11-68h], rax
			"48 8D 05 " // lea     rax, ecl__CustomStatsProtocolSomething__vftable
		);

		p3.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			CustomStatUIRollHook = match;
		});

		Pattern p4;
		p4.FromString(
			"4C 89 4C 24 20 " // mov     [rsp+arg_18], r9
			"53 " // push    rbx
			"57 " // push    rdi
			"41 56 " // push    r14
			"48 83 EC 30 " // sub     rsp, 30h
			"41 8B 51 08 " // mov     edx, [r9+8]
			"49 8B F9 " // mov     rdi, r9
			"4D 8B F0 " // mov     r14, r8
			"81 FA 3B 01 00 00 " // cmp     edx, 13Bh
		);

		p4.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			EsvCustomStatsProtocolProcessMsg = (esv::CustomStatsProtocol__ProcessMsg)match;
		});

		if (UICharacterSheetHook == nullptr
			|| ActivateServerSystemsHook == nullptr
			|| ActivateClientSystemsHook == nullptr
			|| CustomStatUIRollHook == nullptr
			|| EsvCustomStatsProtocolProcessMsg == nullptr) {
			ERR("LibraryManager::FindCustomStatsEoCPlugin(): Could not find all hooks");
			InitFailed = true;
		}
	}


	void LibraryManager::FindNetworkFixedStringsEoCApp()
	{
		Pattern p;
		p.FromString(
			"4C 8D 05 XX XX XX XX " // lea     r8, aNetworkfixedst ; "NetworkFixedStrings"
			"BA 01 00 00 00 " // mov     edx, 1
			"48 8B CF " // mov     rcx, rdi
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"48 8B 15 XX XX XX XX " // mov     rdx, cs:qword_14297F520
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:eoc__gNetworkFixedStrings
			"48 83 C2 28 " // add     rdx, 28h
			"E8 XX XX XX " // call    eoc__NetworkFixedStrings__RegisterAll
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto nameAddr = AsmLeaToAbsoluteAddress(match);
			if (strcmp((const char *)nameAddr, "NetworkFixedStrings") == 0) {
				auto addr = AsmLeaToAbsoluteAddress(match + 28);
				NetworkFixedStrings = (eoc::NetworkFixedStrings **)addr;
				InitNetworkFixedStrings = (void *)AsmCallToAbsoluteAddress(match + 39);
			}
		}, false);

		if (NetworkFixedStrings == nullptr) {
			ERR("LibraryManager::FindNetworkFixedStringsEoCApp(): Could not find eoc::NetworkFixedStrings");
			InitFailed = true;
		}
	}


	void LibraryManager::FindErrorFuncsEoCApp()
	{
		Pattern p;
		p.FromString(
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:ecl__gEocClient
			"4C 8D 8D 08 04 00 00 " // lea     r9, [rbp+8F8h+var_4F0]
			"41 B0 01 " // mov     r8b, 1
			"48 8D 95 E8 03 00 00 " // lea     rdx, [rbp+8F8h+var_510]
			"E8 XX XX XX XX " // call    ecl__EocClient__HandleError
			"48 8D 8D E8 03 00 00 " // lea     rdx, [rbp+8F8h+var_510]
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			EoCClient = (ecl::EoCClient **)AsmLeaToAbsoluteAddress(match);
			EoCClientHandleError = (ecl::EoCClient__HandleError)AsmCallToAbsoluteAddress(match + 24);
		}, false);

		if (EoCClient == nullptr || EoCClientHandleError == nullptr) {
			ERR("LibraryManager::FindErrorFuncsEoCApp(): Could not find ecl::EoCClient::HandleError");
			InitFailed = true;
		}
	}


	void LibraryManager::FindGameStateFuncsEoCApp()
	{
		Pattern p;
		p.FromString(
			"48 89 5C 24 10 " // mov     [rsp-8+arg_8], rbx
			"48 89 74 24 18 " // mov     [rsp-8+arg_10], rsi
			"48 89 7C 24 20 " // mov     [rsp-8+arg_18], rdi
			"55 41 54 41 55 41 56 41 57 " // push    rbp, r12, r13, r14, r15
			"48 8D AC 24 70 FC FF FF " // lea     rbp, [rsp-390h]
			"48 81 EC 90 04 00 00 " // sub     rsp, 490h
			"48 8B 05 XX XX XX XX " // mov     rax, cs:__security_cookie
			"48 33 C4 " // xor     rax, rsp
			"48 89 85 80 03 00 00 " // mov     [rbp+3B0h+var_30], rax
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			GameStateLoadModuleDo = (ecl::GameStateLoadModule__Do)match;
		}, false);

		if (GameStateLoadModuleDo == nullptr) {
			ERR("LibraryManager::FindGameStateFuncsEoCApp(): Could not find ecl::GameStateLoadModule::Do");
			CriticalInitFailed = true;
		}


		Pattern p3;
		p3.FromString(
			"4C 8D 05 XX XX XX XX " // lea     r8, str_Skills     ; "Skills"
			"BA 01 00 00 00 " // mov     edx, 1
			"48 8B CF " // mov     rcx, rdi
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:eoc__gSkillPrototypeManager
			"E8 XX XX XX XX " // call    eoc__SkillPrototypeManager__Init
		);

		p3.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto txtSkills = AsmLeaToAbsoluteAddress(match);
			if (txtSkills != nullptr && strcmp((char const *)txtSkills, "Skills") == 0) {
				auto func = AsmCallToAbsoluteAddress(match + 28);
				SkillPrototypeManagerInit = (eoc__SkillPrototypeManager__Init)func;
			}
		});

		if (SkillPrototypeManagerInit == nullptr) {
			ERR("LibraryManager::FindGameStateFuncsEoCPlugin(): Could not find eoc::SkillPrototypeManager::Init");
			CriticalInitFailed = true;
		}


		Pattern p4;
		p4.FromString(
			"4C 8D 0D XX XX XX XX " // lea r9, aClientStateSwa ; "CLIENT STATE SWAP - from: %s, to: %s\n"
			"4C 89 44 24 28 " // mov     [rsp+68h+var_40], r8
			"33 D2 " // xor     edx, edx
			"4C 8D 44 24 30 " // lea     r8, [rsp+68h+var_38]
		);

		p4.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto txtSwap = AsmLeaToAbsoluteAddress(match);
			if (txtSwap != nullptr && strcmp((char const *)txtSwap, "CLIENT STATE SWAP - from: %s, to: %s\n") == 0) {
				auto func = AsmCallToAbsoluteAddress(match + 0x2A);
				GameStateChangedEvent = (ecl::GameStateEventManager__ExecuteGameStateChangedEvent)func;
			}
		});

		if (GameStateChangedEvent == nullptr) {
			ERR("LibraryManager::FindGameStateFuncsEoCPlugin(): Could not find ecl::GameStateEventManager::ExecuteGameStateChangedEvent");
			CriticalInitFailed = true;
		}
	}


	void LibraryManager::FindCharacterStatFuncsEoCApp()
	{
		memset(&gCharacterStatsGetters.Ptrs, 0, sizeof(gCharacterStatsGetters.Ptrs));

		Pattern p;
		p.FromString(
			"40 84 ED " // test    bpl, bpl
			"74 08 " // jz      short loc_141C7A428
			"8B 86 44 03 00 00 " // mov     eax, [rsi+344h]
			"EB 06 " // jmp     short loc_141C7A42E
			"8B 86 40 03 00 00 " // mov     eax, [rsi+340h]
			"40 0F B6 D5 " // movzx   edx, bpl
			"89 43 44 " // mov     [rbx+44h], eax
		);

		uint8_t const * gettersStart{ nullptr };
		p.Scan(moduleStart_, moduleSize_, [&gettersStart](const uint8_t * match) {
			gettersStart = match;
		}, false);

		if (gettersStart != nullptr) {
			unsigned ptrIndex = 0;
			for (auto p = gettersStart; p < gettersStart + 0x240; p++) {
				auto insn = *reinterpret_cast<uint32_t const *>(p);
				if (insn == 0xE8CE8B48 /* 48 8B CE E8 -- mov  rcx, rsi; call */
					|| insn == 0xE8084389 /* 89 43 08 E8 -- mov [rbx+8], eax; call */) {
					if (ptrIndex < std::size(gCharacterStatsGetters.Ptrs)) {
						auto ptr = AsmCallToAbsoluteAddress(p + 3);
						gCharacterStatsGetters.Ptrs[ptrIndex++] = (void *)ptr;
					}
				}
			}
		}

		if (gCharacterStatsGetters.GetUnknown == nullptr) {
			ERR("LibraryManager::FindCharacterStatFuncsEoCApp(): Could not find all stat getters");
			InitFailed = true;
			return;
		}

		Pattern p2;
		p2.FromString(
			"48 89 5C 24 10 " // mov     [rsp+arg_8], rbx
			"55 " // push    rbp
			"56 " // push    rsi
			"57 " // push    rdi
			"48 83 EC 20 " // sub     rsp, 20h
			"48 8B F2 " // mov     rsi, rdx
			"48 8B F9 " // mov     rdi, rcx
			"B2 01 " // mov     dl, 1 
			"E8 XX XX XX XX " // call    CDivinityStats_Character__GetWeaponStats
			"8B 88 E4 01 00 00 " // mov     ecx, [rax+1E4h]
		);

		p2.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			gCharacterStatsGetters.GetHitChance = (CDivinityStats_Character__GetHitChance *)match;
		}, false);

		if (gCharacterStatsGetters.GetHitChance == nullptr) {
			// Alternative pattern for Gift Bag 3
			Pattern p2b;
			p2b.FromString(
				"40 56 " // push    rsi
				"57 " // push    rdi
				"48 83 EC 28 " // sub     rsp, 28h
				"45 33 C0 " // xor     r8d, r8d
				"48 8B F2 " // mov     rsi, rdx
				"48 8B F9 " // mov     rdi, rcx
				"41 8D 50 78 " // lea     edx, [r8+78h]
				"E8 XX XX XX XX " // call    CDivinityStats_Character__HasTalent
			);

			p2b.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
				gCharacterStatsGetters.GetHitChance = (CDivinityStats_Character__GetHitChance *)match;
			}, false);
		}

		if (gCharacterStatsGetters.GetHitChance == nullptr) {
			ERR("LibraryManager::FindCharacterStatFuncsEoCPlugin(): Could not find CDivinityStats_Character::CalculateHitChance");
			InitFailed = true;
			return;
		}

		Pattern p3;
		p3.FromString(
			"48 0F 4D C2 " // cmovge  rax, rdx
			"8B 18 " // mov     ebx, [rax]
			"E8 XX XX XX XX " // call    CDivinityStats_Character__GetChanceToHitBoost
		);

		p3.Scan((uint8_t const *)gCharacterStatsGetters.GetHitChance, 0x200, [this](const uint8_t * match) {
			auto ptr = AsmCallToAbsoluteAddress(match + 6);
			gCharacterStatsGetters.GetChanceToHitBoost = (CDivinityStats_Character__GetStat *)ptr;
		}, false);

		if (gCharacterStatsGetters.GetChanceToHitBoost == nullptr) {
			ERR("LibraryManager::FindCharacterStatFuncsEoCPlugin(): Could not find CDivinityStats_Character::GetChanceToHitBoost");
			InitFailed = true;
		}

		if (gCharacterStatsGetters.GetDodge != nullptr) {
			Pattern pa;
			pa.FromString(
				"45 33 C9 " // xor     r9d, r9d 
				"45 33 C0 " // xor     r8d, r8d
				"8B D0 " // mov     edx, eax 
				"48 8B CB " // mov     rcx, rbx 
				"E8 XX XX XX XX " // call    CDivinityStats_Character__GetAbility
			);

			pa.Scan((uint8_t *)gCharacterStatsGetters.GetDodge, 0x480, [this](const uint8_t * match) {
				auto func = AsmCallToAbsoluteAddress(match + 11);
				gCharacterStatsGetters.GetAbility = (CDivinityStats_Character__GetAbility *)func;
			}, false);

			if (gCharacterStatsGetters.GetAbility == nullptr) {
				ERR("LibraryManager::FindCharacterStatFuncsEoCPlugin(): Could not find CDivinityStats_Character::GetAbility");
				InitFailed = true;
			}

			Pattern pt;
			pt.FromString(
				"45 0F B6 C7 " // movzx   r8d, r15b
				"BA 03 00 00 00 " // mov     edx, 3
				"48 8B CB " // mov     rcx, rbx
				"03 F7 " // add     esi, edi
				"E8 XX XX XX XX " // call    CDivinityStats_Character__HasTalent
			);

			pt.Scan((uint8_t *)gCharacterStatsGetters.GetDodge, 0x480, [this](const uint8_t * match) {
				auto func = AsmCallToAbsoluteAddress(match + 14);
				gCharacterStatsGetters.GetTalent = (CDivinityStats_Character__GetTalent *)func;
			}, false);

			if (gCharacterStatsGetters.GetTalent == nullptr) {
				ERR("LibraryManager::FindCharacterStatFuncsEoCPlugin(): Could not find CDivinityStats_Character::GetTalent");
				InitFailed = true;
			}
		}
	}
}
#endif
