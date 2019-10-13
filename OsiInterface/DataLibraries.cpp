#include "stdafx.h"
#include "DataLibraries.h"
#include "PropertyMaps.h"
#include <string>
#include <functional>
#include <psapi.h>

namespace osidbg
{
	decltype(LibraryManager::StatusHealEnter) * decltype(LibraryManager::StatusHealEnter)::gHook;
	decltype(LibraryManager::StatusHitEnter) * decltype(LibraryManager::StatusHitEnter)::gHook;

	bool GlobalStringTable::UseMurmur = false;

	uint8_t CharToByte(char c)
	{
		if (c >= '0' && c <= '9') {
			return c - '0';
		}
		else if (c >= 'A' && c <= 'F') {
			return c - 'A' + 0x0A;
		}
		else if (c >= 'a' && c <= 'f') {
			return c - 'a' + 0x0A;
		}
		else {
			Fail("Invalid hexadecimal character");
		}
	}

	uint8_t HexByteToByte(char c1, char c2)
	{
		uint8_t hi = CharToByte(c1);
		uint8_t lo = CharToByte(c2);
		return (hi << 4) | lo;
	}

	struct Pattern
	{
		struct PatternByte
		{
			uint8_t pattern;
			uint8_t mask;
		};

		std::vector<PatternByte> pattern_;

		void FromString(std::string const & s)
		{
			if (s.size() % 3) Fail("Invalid pattern length");
			auto len = s.size() / 3;
			if (!len) Fail("Zero-length patterns not allowed");

			pattern_.reserve(len);

			char const * c = s.data();
			for (auto i = 0; i < len; i++) {
				PatternByte b;
				if (c[2] != ' ') Fail("Bytes must be separated by space");
				if (c[0] == 'X' && c[1] == 'X') {
					b.pattern = 0;
					b.mask = 0;
				}
				else {
					b.pattern = HexByteToByte(c[0], c[1]);
					b.mask = 0xff;
				}

				pattern_.push_back(b);
				c += 3;
			}

			if (pattern_[0].mask != 0xff) Fail("First byte of pattern must be an exact match");
		}

		void FromRaw(const char * s)
		{
			auto len = strlen(s) + 1;
			pattern_.resize(len);
			for (auto i = 0; i < len; i++) {
				pattern_[i].pattern = (uint8_t)s[i];
				pattern_[i].mask = 0xFF;
			}
		}

		bool MatchPattern(uint8_t const * start)
		{
			auto p = start;
			for (auto const & pattern : pattern_) {
				if ((*p++ & pattern.mask) != pattern.pattern) {
					return false;
				}
			}

			return true;
		}

		void ScanPrefix1(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback)
		{
			uint8_t initial = pattern_[0].pattern;

			for (auto p = start; p < end; p++) {
				if (*p == initial) {
					if (MatchPattern(p)) {
						callback(p);
					}
				}
			}
		}

		void ScanPrefix2(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback)
		{
			uint16_t initial = pattern_[0].pattern
				| (pattern_[1].pattern << 8);

			for (auto p = start; p < end; p++) {
				if (*reinterpret_cast<uint16_t const *>(p) == initial) {
					if (MatchPattern(p)) {
						callback(p);
					}
				}
			}
		}

		void ScanPrefix4(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback)
		{
			uint32_t initial = pattern_[0].pattern
				| (pattern_[1].pattern << 8)
				| (pattern_[2].pattern << 16)
				| (pattern_[3].pattern << 24);

			for (auto p = start; p < end; p++) {
				if (*reinterpret_cast<uint32_t const *>(p) == initial) {
					if (MatchPattern(p)) {
						callback(p);
					}
				}
			}
		}

		void Scan(uint8_t const * start, size_t length, std::function<void(uint8_t const *)> callback)
		{
			// Check prefix length
			auto prefixLength = 0;
			for (auto i = 0; i < pattern_.size(); i++) {
				if (pattern_[i].mask == 0xff) {
					prefixLength++;
				}
				else {
					break;
				}
			}

			auto end = start + length - pattern_.size();
			if (prefixLength >= 4) {
				ScanPrefix4(start, end, callback);
			} else if (prefixLength >= 2) {
				ScanPrefix2(start, end, callback);
			} else {
				ScanPrefix1(start, end, callback);
			}
		}
	};

	bool LibraryManager::IsFixedStringRef(uint8_t const * ref, char const * str) const
	{
		if (ref >= moduleStart_ && ref < moduleStart_ + moduleSize_) {
			auto fsx = (FixedString const *)ref;
			if (*fsx && strcmp(fsx->Str, str) == 0) {
				return true;
			}
		}

		return false;
	}

	void LibraryManager::FindLibrariesEoCPlugin()
	{
		uint8_t const prologue0[] = {
			0x40, 0x53, // push rbx
			0x48, 0x83, 0xEC, 0x20,  // sub rsp, 20h
			0x8B, 0x05 // mov eax, cs:xxx
		};

		uint8_t const prologue1[] = {
			0x48, 0x8B, 0xD9, // mov rbx, rcx
			0x8B, 0xD0, // mov edx, eax
			0xFF, 0xC0, // inc eax
			0x89, 0x05 // mov cs:xxx, eax
		};

		uint8_t const prologue2[] = {
			0x85, 0xD2, // test edx, edx
			0x75, 0x18, // jnz short loc_xxx
			0x44, 0x8D, 0x42, // lea r8d, [rdx+XXh]
		};

		uint8_t const prologue3[] = {
			0x48, 0x8D, 0x15 // lea rdx, xxx
		};

		uint8_t const * p = (uint8_t const *)moduleStart_;
		uint8_t const * moduleEnd = p + moduleSize_;

		for (; p < moduleEnd - 100; p++) {
			if (*p == 0x40
				&& memcmp(p, prologue0, sizeof(prologue0)) == 0
				&& memcmp(p + 0x0C, prologue1, sizeof(prologue1)) == 0
				&& memcmp(p + 0x19, prologue2, sizeof(prologue2)) == 0
				&& memcmp(p + 0x21, prologue3, sizeof(prologue3)) == 0) {
				int32_t rel1 = *(int32_t *)(p + 0x24);
				int32_t rel2 = *(int32_t *)(p + 0x2B);

				uint8_t const * freeFunc = p + rel1 + 0x21 + 7;
				uint8_t const * initFunc = p + rel2 + 0x28 + 7;

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
		Debug("LibraryManager::FindLibrariesEoCPlugin(): Found libraries:");
		for (auto const & v : libraries_) {
			Debug("\t(Init %p; Dtor %p, Refs %d)!", v.second.initFunc, v.second.freeFunc, v.second.refs);
		}
#endif
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
		Debug("LibraryManager::FindLibrariesEoCApp(): Found libraries:");
		for (auto const & v : libraries_) {
			Debug("\t(Init %p; Dtor %p, Refs %d)!", v.second.initFunc, v.second.freeFunc, v.second.refs);
		}
#endif
	}

	void LibraryManager::FindServerGlobalsEoCPlugin()
	{
		EoCLibraryInfo const * serverLib{ nullptr };

		// 1) Look for the string "esv::CustomStatDefinitionComponent"
		Pattern statComponent;
		statComponent.FromRaw("esv::CustomStatDefinitionComponent");
		uint8_t const * statComponentStr = nullptr;
		statComponent.Scan((uint8_t const *)moduleStart_, moduleSize_, [&statComponentStr](const uint8_t * match) {
			statComponentStr = match;
		});

		if (!statComponentStr) Fail("Could not locate esv::CustomStatDefinitionComponent");

		// 2) Look for xrefs to esv::CustomStatDefinitionComponent
		Pattern xref;
		xref.FromString(
			"48 8B C8 " // mov rcx, rax
			"48 8D 15 XX XX XX XX " // lea rdx, xxx
			"FF 15 XX XX XX XX " // call cs:xxx
		);

		uint8_t const * statComponentXref = nullptr;
		xref.Scan((uint8_t const *)moduleStart_, moduleSize_, [&statComponentXref, statComponentStr](const uint8_t * match) {
			int32_t rel = *(int32_t const *)(match + 6);
			uint8_t const * refTo = match + rel + 7 + 3;

			if (refTo == statComponentStr) {
				statComponentXref = match;
			}
		});

		if (!statComponentXref) Fail("Could not locate registration xref for esv::CustomStatDefinitionComponent");

		Pattern p;
		p.FromString(
			"48 85 C0 " // test rax, rax
			"74 11 " // jz  xxx
			"48 8B C8 " // mov rcx, rax
			"E8 XX XX XX XX " // call ctor
			"48 89 05 XX XX XX XX " // mov cs:xxx, rax
		);

		uint32_t i = 0;
		p.Scan(statComponentXref, 0x1200, [this, &i](const uint8_t * match) {
			if (i < std::size(serverGlobals_)) {
				int32_t rel = *(int32_t const *)(match + 16);
				uint8_t const * refTo = match + rel + 7 + 13;
				serverGlobals_[i++] = (uint8_t const **)refTo;
			}
		});

		EsvCharacterFactory = (CharacterFactory **)serverGlobals_[(unsigned)EsvGlobalEoCPlugin::EsvCharacterFactory];
		EsvItemFactory = (ItemFactory **)serverGlobals_[(unsigned)EsvGlobalEoCPlugin::EsvItemFactory];

		// FIXME - HACK
		ShootProjectile = (ProjectileHelpers_ShootProjectile)((uint8_t const *)moduleStart_ + 0xC13440 + 0x1000);
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
			Fail(L"LibraryManager::FindServerGlobalsEoCApp(): Could not find esv library!");
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
						Debug(L"LibraryManager::FindServerGlobalsEoCApp(): Could not extract global from func @ %p", r);
					}
				}
			}
		}

		EsvCharacterFactory = (CharacterFactory **)serverGlobals_[(unsigned)EsvGlobalEoCApp::EsvCharacterFactory];
		EsvItemFactory = (ItemFactory **)serverGlobals_[(unsigned)EsvGlobalEoCApp::EsvItemFactory];
	}

	void LibraryManager::FindEoCGlobalsEoCPlugin()
	{
		uint8_t const * globalsInitCode{ nullptr };
		size_t nextGlobalIndex = 0;
		for (auto const & lib : libraries_) {
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
							eocGlobals_[nextGlobalIndex++] = (uint8_t const **)globalPtr;
							if (nextGlobalIndex >= std::size(eocGlobals_)) {
								break;
							}
						}
					}

					break;
				}

				if (nextGlobalIndex >= std::size(eocGlobals_)) {
					break;
				}
			}
		}

		if (nextGlobalIndex != std::size(eocGlobals_)) {
			Fail(L"LibraryManager::FindEoCGlobalsEoCApp(): Could not find all eoc globals!");
			return;
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
			Fail(L"LibraryManager::FindEoCGlobalsEoCApp(): Could not find eoc library init function!");
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
				Debug(L"LibraryManager::FindEoCGlobalsEoCApp(): Could not extract global from func @ %p", r);
			}
		}
	}

	void LibraryManager::FindGlobalStringTableCoreLib()
	{
		Pattern p;
		p.FromString(
			"B9 88 FA 5F 00 " // mov ecx, 5FFA88h
			"41 8D 51 01 " // lea edx, [r9+1]
			"E8 XX XX XX XX " // call alloc
			"48 85 C0 " // test rax, rax
			"74 1F " // jz short xxx
			"48 8B C8 " // mov rcx, rax
			"E8 XX XX XX XX " // call ctor
			"48 8D 4C 24 60 " // rcx, [rsp+58h+arg_0]
			"48 89 05 XX XX XX XX " // mov cs:xxx, rax
		);

		p.Scan((uint8_t const *)coreLibStart_, coreLibSize_, [this](const uint8_t * match) {
			int32_t rel = *(int32_t const *)(match + 35);
			auto refTo = match + rel + 7 + 32;

			GlobalStrings = (GlobalStringTable const **)refTo;
			GlobalStringTable::UseMurmur = true;
		});

		if (GlobalStrings == nullptr) {
			Debug(L"LibraryManager::FindGlobalStringTableCoreLib(): Could not find global string table");
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
			Debug(L"LibraryManager::FindGlobalStringTableEoCApp(): Could not find global string table");
		}
	}

	uint8_t const * AsmCallToAbsoluteAddress(uint8_t const * call)
	{
		if (call[0] != 0xE8) {
			Debug("AsmCallToAbsoluteAddress(): Not a call @ %p", call);
			return nullptr;
		}

		int32_t rel = *(int32_t const *)(call + 1);
		return call + rel + 5;
	}

	uint8_t const * AsmLeaToAbsoluteAddress(uint8_t const * lea)
	{
		if (lea[0] != 0x48 || (lea[1] != 0x8D && lea[1] != 0x8B)) {
			Debug("AsmLeaToAbsoluteAddress(): Not a LEA @ %p", lea);
			return nullptr;
		}

		int32_t rel = *(int32_t const *)(lea + 3);
		return lea + rel + 7;
	}

	void LibraryManager::FindGameActionManagerEoCPlugin()
	{
		Pattern p;
		p.FromString(
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_GameAction
			"4C 8B 05 XX XX XX XX " // mov     r8, cs:xxx
			"48 8B CF " // mov     rcx, rdi
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"84 C0 " // test    al, al
			"0F 84 XX 00 00 00 " // jz      xxx
			"49 8B D6 " // mov     rdx, r14
			"48 8D 4D FF " // lea     rcx, [rbp+57h+Memory]
			"E8 XX XX XX XX " // call xxx
			"45 84 E4 " // test    r12b, r12b
			"74 XX " // jz      short xxx
			"49 8B D6 " // mov     rdx, r14
			"48 8D 4D FF " // lea     rcx, [rbp+57h+Memory]
			"48 8B D8 " // mov     rbx, rax
			"E8 XX XX XX XX " // call    xxx
			"4C 8B 03 " // mov     r8, [rbx]
			"49 8B CD " // mov     rcx, r13
			"8B 50 08 " // mov     edx, [rax+8]
			"E8 XX XX XX XX " // call    esv__GameActionManager__CreateAction
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match);
			if (IsFixedStringRef(fsx, "GameAction")) {
				auto actionAddr = AsmCallToAbsoluteAddress(match + 72);
				CreateGameAction = (GameActionManager__CreateAction)actionAddr;
			}
		});

		if (CreateGameAction == nullptr) {
			Debug(L"LibraryManager::FindGameActionManagerEoCPlugin(): Could not find GameActionManager::CreateAction");
		}

		Pattern p2;
		p2.FromString(
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_ForceMove
			"48 8B C8 " // mov     rcx, rax
			"E8 XX XX XX XX " // call    eoc__SkillPrototype__GetMappedValue
			"85 C0 " // test    eax, eax
			"75 25 " // jnz     short loc_180EE4875
			"4C 8D 46 70 " // lea     r8, [rsi+70h]
		);

		Pattern p3;
		p3.FromString(
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:ls__gServerLevelAllocator
			"48 89 7C 24 50 " // mov     [rsp+38h+arg_10], rdi
			"E8 XX XX XX XX " // call    esv__LevelManager__GetGameActionManager
			"4C 8B 05 XX XX XX XX " // mov     r8, cs:xxx
			"BA 08 00 00 00 " //  mov     edx, 8
			"48 8B C8 " // mov     rcx, rax
			"48 8B F8 " // mov     rdi, rax
			"4D 8B 00 " // mov     r8, [r8]
			"E8 XX XX XX XX " // call    esv__GameActionManager__CreateAction
			"4C 8D 46 70 " // lea     r8, [rsi+70h]
			"48 8B C8 " // mov     rcx, rax
			"48 8D 56 68 " // lea     rdx, [rsi+68h]
			"48 8B D8 " // mov     rbx, rax
			"E8 XX XX XX XX " // call esv__GameObjectMoveAction__Setup
			"48 8B D3 " // mov     rdx, rbx
			"48 8B CF " // mov     rcx, rdi
			"E8 XX XX XX XX " // call    esv__GameActionManager__AddAction
		);

		p2.Scan(moduleStart_, moduleSize_, [this, &p3](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 4);
			if (IsFixedStringRef(fsx, "ForceMove")) {
				p3.Scan(match, 0x100, [this](const uint8_t * match2) {
					LevelManager = (void **)AsmLeaToAbsoluteAddress(match2);

					auto moveSetupAddr = AsmCallToAbsoluteAddress(match2 + 57);
					GameObjectMoveActionSetup = (GameObjectMoveAction__Setup)moveSetupAddr;

					auto addActionAddr = AsmCallToAbsoluteAddress(match2 + 68);
					AddGameAction = (GameActionManager__AddAction)addActionAddr;
				});
			}
		});

		if (LevelManager == nullptr || AddGameAction == nullptr) {
			Debug(L"LibraryManager::FindGameActionManagerEoCApp(): Could not find esv::LevelManager");
		}
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
				CreateGameAction = (GameActionManager__CreateAction)actionAddr;
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
					CreateGameAction = (GameActionManager__CreateAction)actionAddr;
				}
			});

			if (CreateGameAction == nullptr) {
				Debug(L"LibraryManager::FindGameActionManagerEoCApp(): Could not find GameActionManager::CreateAction");
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
					GameObjectMoveActionSetup = (GameObjectMoveAction__Setup)moveSetupAddr;

					auto addActionAddr = AsmCallToAbsoluteAddress(match2 + 84);
					AddGameAction = (GameActionManager__AddAction)addActionAddr;
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
						GameObjectMoveActionSetup = (GameObjectMoveAction__Setup)moveSetupAddr;

						auto addActionAddr = AsmCallToAbsoluteAddress(match2 + 95);
						AddGameAction = (GameActionManager__AddAction)addActionAddr;
					});
				}
			}
		});

		if (LevelManager == nullptr || AddGameAction == nullptr) {
			Debug(L"LibraryManager::FindGameActionManagerEoCApp(): Could not find esv::LevelManager");
		}
	}

	void LibraryManager::FindGameActionsEoCPlugin()
	{
		Pattern p;
		p.FromString(
			"48 8D 57 50 " // lea     rdx, [rdi+50h]
			"E8 XX XX XX XX " // call    eoc__SkillPrototypeManager__GetPrototype
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_RandomPoints
			"48 8B C8 " // mov     rcx, rax
			"48 8B F0 " // mov     rsi, rax
			"E8 XX XX XX XX " // call    eoc__SkillPrototype__GetMappedValue
		);

		Pattern p2;
		p2.FromString(
			"48 8B C4 " // mov     rax, rsp
			"53 " // push    rbx
			"55 " // push    rbp
		);

		p.Scan(moduleStart_, moduleSize_, [this, &p2](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 13);
			if (IsFixedStringRef(fsx, "RandomPoints")) {
				p2.Scan(match - 0x100, 0x100, [this](const uint8_t * match2) {
					TornadoActionSetup = (TornadoAction__Setup)match2;
				});
			}
		});

		if (TornadoActionSetup == nullptr) {
			Debug(L"LibraryManager::FindGameActionsEoCPlugin(): Could not find TornadoAction");
		}


		Pattern p3;
		p3.FromString(
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_GrowTimeout
			"48 8B C8 " // mov     rcx, rax
			"48 8B F0 " // mov     rsi, rax
			"E8 XX XX XX XX " // call    eoc__SkillPrototype__GetMappedValueDiv1k
			"44 0F 28 F8 " // movaps  xmm15, xmm0
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_GrowSpeed
			"F3 44 0F 5E 3D XX XX XX XX " // divss   xmm15, cs:dword_1819A9AD0
		);

		Pattern p4;
		p4.FromString(
			"48 8B C4 " // mov     rax, rsp
			"48 89 58 18 " // mov     [rax+18h], rbx
			"48 89 70 20 " // mov     [rax+20h], rsi
		);

		p3.Scan(moduleStart_, moduleSize_, [this, &p4](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 4);
			auto fsx2 = AsmLeaToAbsoluteAddress(match + 26);
			if (IsFixedStringRef(fsx, "GrowTimeout")
				&& IsFixedStringRef(fsx2, "GrowSpeed")) {
				p4.Scan(match - 0x100, 0x100, [this](const uint8_t * match2) {
					WallActionCreateWall = (TornadoAction__Setup)match2;
				});
			}
		});

		if (WallActionCreateWall == nullptr) {
			Debug(L"LibraryManager::FindGameActionsEoCPlugin(): Could not find WallAction");
		}


		Pattern p5;
		p5.FromString(
			"44 0F 28 94 24 20 01 00 00 " // movaps  xmm10, [rsp+1B0h+var_90]
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_SpawnObject
			"49 8B CE " // mov     rcx, r14
		);

		Pattern p6;
		p6.FromString(
			"48 8D 55 E0 " // lea     rdx, [rbp+0B0h+summonArgs]
			"C6 45 0D 00 " // mov     [rbp+0B0h+summonArgs.MapToAiGrid_M], 0
			"48 8D 4C 24 50 " // lea     rcx, [rsp+1B0h+var_160]
			"E8 XX XX XX XX " // call    esv__SummonHelpers__Summon
			"48 8D 4C 24 50 " // lea     rcx, [rsp+1B0h+var_160]
		);

		p5.Scan(moduleStart_, moduleSize_, [this, &p6](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 9);
			if (IsFixedStringRef(fsx, "SpawnObject")) {
				p6.Scan(match - 0x400, 0x400, [this](const uint8_t * match2) {
					auto summon = AsmCallToAbsoluteAddress(match2 + 13);
					SummonHelpersSummon = (SummonHelpers__Summon)summon;
				});
			}
		});

		if (SummonHelpersSummon == nullptr) {
			Debug(L"LibraryManager::FindGameActionsEoCPlugin(): Could not find SummonHelpers::Summon");
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
					TornadoActionSetup = (TornadoAction__Setup)match2;
				});
			}
		});

		if (TornadoActionSetup == nullptr) {
			Debug(L"LibraryManager::FindGameActionsEoCApp(): Could not find TornadoAction");
		}

		// FIXME: WallAction, SummonHelpers::Summon
	}

	void LibraryManager::FindStatusMachineEoCPlugin()
	{
		Pattern p;
		p.FromString(
			"83 7A 1C 00 " // cmp     dword ptr [rdx+1Ch], 0
			"48 8B F2 " // mov     rsi, rdx
			"4C 8B F1 " // mov     r14, rcx
			"7E 7E " // jle     short xxx
			"4C 8B 05 XX XX XX XX " // mov     r8, cs:?Unassigned@ObjectHandle@ls@@2V12@B
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_LIFESTEAL
			"48 89 5C 24 30 " //  mov     [rsp+28h+arg_0], rbx
			"48 89 7C 24 40 " //  mov     [rsp+28h+arg_10], rdi
			"48 8B B9 B0 01 00 00 " //  mov     rdi, [rcx+1B0h]
			"4D 8B 00 " //  mov     r8, [r8]
			"48 8B CF " //  mov     rcx, rdi 
			"E8 XX XX XX XX " //  call    esv__StatusMachine__CreateStatus
		);

		uint8_t const * lastMatch;
		p.Scan(moduleStart_, moduleSize_, [this, &lastMatch](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 19);
			if (IsFixedStringRef(fsx, "LIFESTEAL")) {
				auto actionAddr = AsmCallToAbsoluteAddress(match + 49);
				lastMatch = match + 55;
				StatusMachineCreateStatus = (StatusMachine__CreateStatus)actionAddr;
			}
		});

		if (StatusMachineCreateStatus == nullptr) {
			Debug(L"LibraryManager::FindStatusMachineEoCPlugin(): Could not find StatusMachine::CreateStatus");
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
			StatusMachineApplyStatus = (StatusMachine__ApplyStatus)actionAddr;
		});

		if (StatusMachineApplyStatus == nullptr) {
			Debug(L"LibraryManager::FindStatusMachineEoCPlugin(): Could not find StatusMachine::ApplyStatus");
		}
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
				StatusMachineCreateStatus = (StatusMachine__CreateStatus)actionAddr;
			}
		});

		if (StatusMachineCreateStatus == nullptr) {
			Debug(L"LibraryManager::FindStatusMachineEoCApp(): Could not find StatusMachine::CreateStatus");
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
			StatusMachineApplyStatus = (StatusMachine__ApplyStatus)actionAddr;
		});

		if (StatusMachineApplyStatus == nullptr) {
			Debug(L"LibraryManager::FindStatusMachineEoCApp(): Could not find StatusMachine::ApplyStatus");
		}
	}

	void LibraryManager::FindStatusTypesEoCPlugin()
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
							StatusHealVMT = reinterpret_cast<EsvStatusVMT const *>(p - 25 * 8);
						}
					}
				});
			}
		});

		if (StatusHealVMT == nullptr) {
			Debug(L"LibraryManager::FindStatusTypesEoCPlugin(): Could not find esv::StatusHeal");
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
			"48 8B C4 " // mov     rax, rsp
			"55 " // push    rbp
			"53 " // push    rbx
		);

		p3.Scan(moduleStart_, moduleSize_, [this, &p4](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 7);
			if (IsFixedStringRef(fsx, "RS3_FX_GP_Status_Retaliation_Beam_01")) {
				p4.Scan(match - 0x600, 0x600, [this](const uint8_t * match) {
					// Look for this function ptr
					auto ptr = (uint64_t)match;
					for (auto p = moduleStart_; p < moduleStart_ + moduleSize_; p += 8) {
						if (*reinterpret_cast<uint64_t const *>(p) == ptr) {
							StatusHitVMT = reinterpret_cast<EsvStatusVMT const *>(p - 12 * 8);
						}
					}
				});
			}
		});

		if (StatusHitVMT == nullptr) {
			Debug(L"LibraryManager::FindStatusTypesEoCPlugin(): Could not find esv::StatusHit");
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
							StatusHealVMT = reinterpret_cast<EsvStatusVMT const *>(p - 25 * 8);
						}
					}
				});
			}
		});

		if (StatusHealVMT == nullptr) {
			Debug(L"LibraryManager::FindStatusTypesEoCApp(): Could not find esv::StatusHeal");
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
							StatusHitVMT = reinterpret_cast<EsvStatusVMT const *>(p - 12 * 8);
						}
					}
				});
			}
		});

		if (StatusHitVMT == nullptr) {
			Debug(L"LibraryManager::FindStatusTypesEoCApp(): Could not find esv::StatusHit");
		}
	}


	void LibraryManager::FindHitFuncsEoCPlugin()
	{
		Pattern p;
		p.FromString(
			"4C 89 XX 24 18 01 00 00 " // mov     [rsp+150h+var_38], r15
			"E8 XX XX XX XX " // call    eoc__StatusPrototype__GetMappedValue
			"85 C0 " // test    eax, eax
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_DamageItems
			"48 8B CB " // mov     rcx, rbx
			"40 0F 95 C7 " // setnz   dil
		);

		Pattern p2;
		p2.FromString(
			"48 89 44 24 30 " // mov     qword ptr [rsp+150h+a7], rax
			"C6 44 24 28 00 " // mov     byte ptr [rsp+150h+a6], 0
			"C7 44 24 20 05 00 00 00 " // mov     [rsp+150h+a5], 5
			"E8 XX XX XX XX " // call    esv__Character__Hit
			"XX 8B XX B0 01 00 00 " // mov     r13, [r15+1B0h]
			"EB 66 " // jmp short xxx
		);

		p.Scan(moduleStart_, moduleSize_, [this, &p2](const uint8_t * match) {
			auto fsx = AsmLeaToAbsoluteAddress(match + 15);
			if (IsFixedStringRef(fsx, "DamageItems")) {
				p2.Scan(match, 0x300, [this](const uint8_t * match) {
					auto actionAddr = AsmCallToAbsoluteAddress(match + 18);
					CharacterHit = (Character__Hit)actionAddr;
				});
			}
		});

		if (CharacterHit == nullptr) {
			Debug(L"LibraryManager::FindHitFuncsEoCPlugin(): Could not find Character::Hit");
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
					CharacterHit = (Character__Hit)actionAddr;
				});
			}
		});

		if (CharacterHit == nullptr) {
			Debug(L"LibraryManager::FindHitFuncsEoCApp(): Could not find Character::Hit");
		}
	}

	void LibraryManager::FindItemFuncsEoCPlugin()
	{
		Pattern p;
		p.FromString(
			"48 8B C8 " // mov     rcx, rax
			"48 89 5C 24 60 " // mov     [rsp+0B8h+var_58.VMT], rbx
			"E8 XX XX XX XX " // call    esv__ParseItem
			"33 D2 " // xor     edx, edx
			"48 8D 4C 24 60 " // lea     rcx, [rsp+0B8h+var_58]
			"E8 XX XX XX XX " // call    esv__CreateItemFromParsed
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto parseAddr = AsmCallToAbsoluteAddress(match + 8);
			ParseItem = (esv__ParseItem)parseAddr;
			auto createAddr = AsmCallToAbsoluteAddress(match + 20);
			CreateItemFromParsed = (esv__CreateItemFromParsed)createAddr;
		});

		if (ParseItem == nullptr || CreateItemFromParsed == nullptr) {
			Debug(L"LibraryManager::FindItemFuncsEoCPlugin(): Could not find esv::CreateItemFromParsed");
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
			"48 8D 4C 24 60 " // lea     rcx, [rsp+0B8h+var_58]
			"E8 XX XX XX XX " // call    esv__CreateItemFromParsed
		);

		p.Scan(moduleStart_, moduleSize_, [this](const uint8_t * match) {
			auto parseAddr = AsmCallToAbsoluteAddress(match + 8);
			ParseItem = (esv__ParseItem)parseAddr;
			auto createAddr = AsmCallToAbsoluteAddress(match + 20);
			CreateItemFromParsed = (esv__CreateItemFromParsed)createAddr;
		});

		if (ParseItem == nullptr || CreateItemFromParsed == nullptr) {
			Debug(L"LibraryManager::FindItemFuncsEoCApp(): Could not find esv::CreateItemFromParsed");
		}
	}

	bool LibraryManager::FindLibraries()
	{
		if (FindEoCPlugin(moduleStart_, moduleSize_)) {
			FindLibrariesEoCPlugin();
			FindServerGlobalsEoCPlugin();
			FindEoCGlobalsEoCPlugin();
			FindGlobalStringTableCoreLib();
			return true;
		}
		else if (FindEoCApp(moduleStart_, moduleSize_)) {
			coreLibStart_ = nullptr;
			coreLibSize_ = 0;
			FindLibrariesEoCApp();
			FindServerGlobalsEoCApp();
			FindEoCGlobalsEoCApp();
			FindGlobalStringTableEoCApp();
			return true;
		}
		else {
			Debug(L"LibraryManager::FindLibraries(): Unable to determine application type.");
			return false;
		}
	}

	void LibraryManager::PostStartupFindLibraries()
	{
		if (PostLoaded) {
			return;
		}

		if (coreLibStart_ != nullptr) {
			FindGameActionManagerEoCPlugin();
			FindGameActionsEoCPlugin();
			FindStatusMachineEoCPlugin();
			FindHitFuncsEoCPlugin();
			FindItemFuncsEoCPlugin();
			FindStatusTypesEoCPlugin();
		}
		else {
			FindGameActionManagerEoCApp();
			FindGameActionsEoCApp();
			FindStatusMachineEoCApp();
			FindHitFuncsEoCApp();
			FindItemFuncsEoCApp();
			FindStatusTypesEoCApp();
		}

		InitPropertyMaps();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		if (StatusHitVMT != nullptr) {
			StatusHitEnter.Wrap(StatusHitVMT->Enter);
		}

		if (StatusHealVMT != nullptr) {
			StatusHealEnter.Wrap(StatusHealVMT->Enter);
		}

		DetourTransactionCommit();

		PostLoaded = true;
	}

	void LibraryManager::Cleanup()
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		StatusHitEnter.Unwrap();
		StatusHealEnter.Unwrap();

		DetourTransactionCommit();
	}

	bool LibraryManager::FindEoCPlugin(uint8_t const * & start, size_t & size)
	{
		HMODULE hDivinityEngine = GetModuleHandleW(L"DivinityEngine2.exe");
		if (hDivinityEngine == NULL) {
			return false;
		}

		HMODULE hEoCPlugin = LoadLibraryW(L"EoCPlugin.dll");
		if (hEoCPlugin == NULL) {
			return false;
		}

		MODULEINFO moduleInfo;
		if (!GetModuleInformation(GetCurrentProcess(), hEoCPlugin, &moduleInfo, sizeof(moduleInfo))) {
			Fail(L"Could not get module info of EoCPlugin.dll");
		}

		start = (uint8_t const *)moduleInfo.lpBaseOfDll;
		size = moduleInfo.SizeOfImage;

		HMODULE hCoreLib = LoadLibraryW(L"CoreLib.dll");
		if (hCoreLib == NULL) {
			return false;
		}

		if (!GetModuleInformation(GetCurrentProcess(), hCoreLib, &moduleInfo, sizeof(moduleInfo))) {
			Fail(L"Could not get module info of CoreLib.dll");
		}

		coreLibStart_ = (uint8_t const *)moduleInfo.lpBaseOfDll;
		coreLibSize_ = moduleInfo.SizeOfImage;

		return true;
	}

	bool LibraryManager::FindEoCApp(uint8_t const * & start, size_t & size)
	{
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
}
