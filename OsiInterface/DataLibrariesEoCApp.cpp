#include "stdafx.h"
#include "DataLibraries.h"
#include "OsirisProxy.h"
#include <GameDefinitions/Symbols.h>
#include <functional>
#include <psapi.h>

#if defined(OSI_EOCAPP)
namespace osidbg
{
	SymbolMappingData const sSymbolLevelManager = {
		"LevelManager2",
		SymbolMappingData::kCustom, 0,
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
		"E8 XX XX XX XX ", // call    esv__GameActionManager__AddAction
		{},
		{"LevelManager", SymbolMappingTarget::kIndirectLea, 0, (void **)&gStaticSymbols.LevelManager},
		{"esv::GameObjectMoveAction::Setup", SymbolMappingTarget::kIndirectCall, 73, (void **)&gStaticSymbols.GameObjectMoveActionSetup},
		{"esv::GameActionManager::AddAction", SymbolMappingTarget::kIndirectCall, 95, (void **)&gStaticSymbols.AddGameAction}
	};

	SymbolMappingData const sSymbolTornadoAction = {
		"TornadoAction2",
		SymbolMappingData::kCustom, 0,
		"48 8B C4 " // mov     rax, rsp
		"53 ", // push    rbx
		{},
		{"esv::TornadoAction::Setup", SymbolMappingTarget::kAbsolute, 0, (void **)&gStaticSymbols.TornadoActionSetup}
	};

	SymbolMappingData const sSymbolApplyStatus = {
		"ApplyStatus",
		SymbolMappingData::kCustom, 0,
		"C7 43 2C 00 00 00 00 " // mov     dword ptr [rbx+2Ch], 0
		"48 8B CF " // mov     rcx, rdi
		"E8 XX XX XX XX " // call    esv__StatusMachine__ApplyStatus
		"48 8B 7C 24 40 ", // mov     rdi, [rsp+28h+arg_10]
		{},
		{"esv::StatusMachine::ApplyStatus", SymbolMappingTarget::kIndirectCall, 10, (void **)&gStaticSymbols.StatusMachineApplyStatus}
	};

	SymbolMappingResult FindStatusHitEoCApp2(uint8_t const * match)
	{
		auto & library = gOsirisProxy->GetLibraryManager();
		auto moduleStart = library.GetModuleStart();
		auto moduleSize = library.GetModuleSize();

		// Look for this function ptr
		auto ptr = (uint64_t)match;
		for (auto p = moduleStart; p < moduleStart + moduleSize; p += 8) {
			if (*reinterpret_cast<uint64_t const *>(p) == ptr) {
				gStaticSymbols.StatusHitVMT = reinterpret_cast<esv::StatusVMT const *>(p - 12 * 8);
				return SymbolMappingResult::Success;
			}
		}

		return SymbolMappingResult::Fail;
	}

	SymbolMappingData const sSymbolStatusHit = {
		"esv::StatusHit::__vftable",
		SymbolMappingData::kCustom, 0,
		"40 55 " // push    rbp
		"41 54 " // push    r12
		"41 55 " // push    r13
		"41 57 ", // push    r15
		{},
		{"esv::StatusHit::__vftable", SymbolMappingTarget::kAbsolute, 0, nullptr, &FindStatusHitEoCApp2}
	};

	SymbolMappingResult FindStatusHealEoCApp2(uint8_t const * match)
	{
		auto & library = gOsirisProxy->GetLibraryManager();
		auto moduleStart = library.GetModuleStart();
		auto moduleSize = library.GetModuleSize();

		// Look for this function ptr
		auto ptr = (uint64_t)match;
		for (auto p = moduleStart; p < moduleStart + moduleSize; p += 8) {
			if (*reinterpret_cast<uint64_t const *>(p) == ptr) {
				gStaticSymbols.StatusHealVMT = reinterpret_cast<esv::StatusVMT const *>(p - 25 * 8);
				return SymbolMappingResult::Success;
			}
		}

		return SymbolMappingResult::Fail;
	}

	SymbolMappingData const sSymbolStatusHeal = {
		"esv::StatusHeal::__vftable",
		SymbolMappingData::kCustom, 0,
		"48 89 5C 24 10 " // mov     [rsp-8+arg_8], rbx
		"48 89 74 24 18 ", // mov     [rsp-8+arg_10], rsi
		{},
		{"esv::StatusHeal::__vftable", SymbolMappingTarget::kAbsolute, 0, nullptr, &FindStatusHealEoCApp2}
	};

	SymbolMappingResult FindActivateEntitySystemEoCApp(uint8_t const * match)
	{
		if (gStaticSymbols.ActivateClientSystemsHook == nullptr) {
			gStaticSymbols.ActivateClientSystemsHook = match;
			return SymbolMappingResult::TryNext;
		} else {
			gStaticSymbols.ActivateServerSystemsHook = match;
			return SymbolMappingResult::Success;
		}
	}

	SymbolMappingData const sSymbolGetAbility = {
		"GetAbility",
		SymbolMappingData::kCustom, 0,
		"45 33 C9 " // xor     r9d, r9d 
		"45 33 C0 " // xor     r8d, r8d
		"8B D0 " // mov     edx, eax 
		"48 8B CB " // mov     rcx, rbx 
		"E8 XX XX XX XX ", // call    CDivinityStats_Character__GetAbility
		{},
		{"GetAbility", SymbolMappingTarget::kIndirectCall, 11, (void **)&gCharacterStatsGetters.GetAbility}
	};

	SymbolMappingData const sSymbolGetTalent = {
		"GetTalent",
		SymbolMappingData::kCustom, 0,
		"45 0F B6 C7 " // movzx   r8d, r15b
		"BA 03 00 00 00 " // mov     edx, 3
		"48 8B CB " // mov     rcx, rbx
		"03 F7 " // add     esi, edi
		"E8 XX XX XX XX ", // call    CDivinityStats_Character__HasTalent
		{},
		{"GetTalent", SymbolMappingTarget::kIndirectCall, 14, (void **)&gCharacterStatsGetters.GetTalent}
	};

	SymbolMappingResult FindCharacterStatGettersEoCApp(uint8_t const * match)
	{
		unsigned ptrIndex = 0;
		for (auto p = match; p < match + 0x240; p++) {
			auto insn = *reinterpret_cast<uint32_t const *>(p);
			if (insn == 0xE8CE8B48 /* 48 8B CE E8 -- mov  rcx, rsi; call */
				|| insn == 0xE8084389 /* 89 43 08 E8 -- mov [rbx+8], eax; call */) {
				if (ptrIndex < std::size(gCharacterStatsGetters.Ptrs)) {
					auto ptr = AsmCallToAbsoluteAddress(p + 3);
					gCharacterStatsGetters.Ptrs[ptrIndex++] = (void *)ptr;
				}
			}
		}

		if (gCharacterStatsGetters.GetUnknown != nullptr) {
			auto & library = gOsirisProxy->GetLibraryManager();
			library.MapSymbol(sSymbolGetAbility, (uint8_t *)gCharacterStatsGetters.GetDodge, 0x480);
			library.MapSymbol(sSymbolGetTalent, (uint8_t *)gCharacterStatsGetters.GetDodge, 0x480);
		}

		return (gCharacterStatsGetters.GetUnknown != nullptr) ? SymbolMappingResult::Success : SymbolMappingResult::Fail;
	}

	SymbolMappingData const sSymbolChanceToHitBoost = {
		"GetChanceToHitBoost",
			SymbolMappingData::kCustom, 0,
			"48 0F 4D C2 " // cmovge  rax, rdx
			"8B 18 " // mov     ebx, [rax]
			"E8 XX XX XX XX ", // call    CDivinityStats_Character__GetChanceToHitBoost
		{},
		{"GetChanceToHitBoost", SymbolMappingTarget::kIndirectCall, 6, (void **)&gCharacterStatsGetters.GetChanceToHitBoost}
	};

	SymbolMappingData const sSymbolCharacterHit = {
		"esv::Character::CharacterHit",
		SymbolMappingData::kCustom, 0,
		"C7 44 24 20 05 00 00 00 " // mov     dword ptr [rsp+140h+var_120], 5
		"44 89 65 50 " // mov     [rbp+40h+arg_0], r12d
		"E8 XX XX XX XX " // call    esv__Character__Hit
		"4C 8B AE A0 01 00 00 " // mov     r13, [rsi+1A0h]
		"EB 5F ", // jmp short xxx
		{},
		{"esv::Character::CharacterHit", SymbolMappingTarget::kIndirectCall, 12, (void **)&gStaticSymbols.CharacterHit}
	};

	SymbolMappingResult FindLibrariesEoCApp(uint8_t const * match)
	{
		auto & lib = gStaticSymbols.Libraries;

		auto initFunc = AsmLeaToAbsoluteAddress(match + 18);
		auto freeFunc = AsmLeaToAbsoluteAddress(match + 25);
		if (initFunc != nullptr && freeFunc != nullptr) {
			auto it = gStaticSymbols.Libraries.find(initFunc);
			if (it != lib.end()) {
				it->second.refs++;
			} else {
				lib.insert(std::pair<uint8_t const *, StaticSymbols::EoCLibraryInfo>(initFunc, { initFunc, freeFunc, 1 }));
			}

			return SymbolMappingResult::TryNext;
		} else {
			return SymbolMappingResult::Fail;
		}
	}

	SymbolMappingResult FindActionMachineSetState(uint8_t const * match)
	{
		if (gStaticSymbols.EclActionMachine__SetState == nullptr) {
			gStaticSymbols.EclActionMachine__SetState = (esv::ActionMachine::SetStateProc)match;
			return SymbolMappingResult::TryNext;
		} else {
			gStaticSymbols.EsvActionMachine__SetState = (esv::ActionMachine::SetStateProc)match;
			return SymbolMappingResult::Success;
		}
	}

	SymbolMappingData const sSymbolSkillPrototypeFormatDescriptionParam = {
		"eoc::SkillPrototype::FormatDescriptionParam2",
		SymbolMappingData::kCustom, 0,
		"4C 8B DC " // mov     r11, rsp
		"55 " // push    rbp
		"53 ", // push    rbx
		{},
		{"eoc::SkillPrototype::FormatDescriptionParam2", SymbolMappingTarget::kAbsolute, 0, (void **)&gStaticSymbols.SkillPrototypeFormatDescriptionParam}
	};

	SymbolMappingData const sSymbolSkillPrototypeGetSkillDamage = {
		"eoc::SkillPrototype::GetSkillDamage2",
		SymbolMappingData::kCustom, 0,
		"44 88 4C 24 20 " // mov     byte ptr [rsp+arg_18], r9b
		"48 89 54 24 10 ", // mov     [rsp+damageList], rdx
		{},
		{"eoc::SkillPrototype::GetSkillDamage2", SymbolMappingTarget::kAbsolute, 0, (void **)&gStaticSymbols.SkillPrototypeGetSkillDamage}
	};

	SymbolMappingData const sSymbolStatusPrototypeFormatDescriptionParam = {
		"eoc::StatusPrototype::FormatDescriptionParam2",
		SymbolMappingData::kCustom, 0,
		"4C 8B DC " // mov     r11, rsp
		"55 " // push    rbp
		"56 ", // push    rsi
		{},
		{"eoc::StatusPrototype::FormatDescriptionParam2", SymbolMappingTarget::kAbsolute, 0, (void **)&gStaticSymbols.StatusPrototypeFormatDescriptionParam}
	};

	SymbolMappingData const sSymbolMappings[] = {
		{
			"EoCMemoryMgr", 
			SymbolMappingData::kText, SymbolMappingData::kCritical,
			"89 41 10 " // mov     [rcx+10h], eax
			"E8 XX XX XX XX " // call    ls__GlobalAllocator__Free
			"48 8B 47 08 " // mov     rax, [rdi+8]
			"BA 28 01 00 00 " // mov     edx, 128h
			"48 89 46 08 " // mov     [rsi+8], rax
			"48 C7 47 08 00 00 00 00 " // mov     qword ptr [rdi+8], 0
			"C7 07 00 00 00 00 " // mov     dword ptr [rdi], 0
			"E8 XX XX XX XX " // call    ls__GlobalAllocator__Malloc
			"33 D2 ", // xor     edx, edx 
			{}, // Unconditional
			{"EoCAlloc", SymbolMappingTarget::kIndirectCall, 35, (void **)&gStaticSymbols.EoCAlloc},
			{"EoCFree", SymbolMappingTarget::kIndirectCall, 3, (void **)&gStaticSymbols.EoCFree}
		},

		{
			"ls::FixedString::Create",
			SymbolMappingData::kText, SymbolMappingData::kCritical,
			"48 8B F0 " // mov     rsi, rax
			"48 85 C0 " // test    rax, rax
			"0F XX XX XX 00 00 " // jz      loc_xxx
			"83 CA FF " // or      edx, 0FFFFFFFFh
			"4C 89 74 XX 60 " // mov     [rsp+0DB0h+var_D50], r14
			"48 8D 0D XX XX XX XX " // lea     rcx, str_Damage ; "Damage"
			"E8 XX XX XX XX " // call    ls__FixedString__Create
			"83 CA FF ", // or      edx, 0FFFFFFFFh
			{SymbolMappingCondition::kString, 20, "Damage"},
			{"ls::FixedString::Create", SymbolMappingTarget::kIndirectCall, 27, (void **)&gStaticSymbols.CreateFixedString}
		},

		{
			"ls::FileReader::FileReader",
			SymbolMappingData::kText, SymbolMappingData::kCritical,
			"F3 0F 7F 45 D7 " // movdqu  [rbp+4Fh+var_78], xmm0
			"E8 XX XX XX XX " // call    xxx
			"45 33 C0 " // xor     r8d, r8d
			"89 75 17 " // mov     [rbp+4Fh+var_38], esi
			"48 8B D7 " // mov     rdx, rdi
			"48 89 75 1F " // mov     [rbp+4Fh+var_30], rsi
			"48 8D 4D CF " // lea     rcx, [rbp+4Fh+var_80]
			"E8 XX XX XX XX " // call    ls__FileReader__FileReader
			"48 8D 55 CF " // lea     rdx, [rbp+4Fh+var_80]
			"48 8D 4C 24 30 " // lea     rcx, [rsp+110h+var_E0]
			"E8 XX XX XX XX " // call    sub_1411CF840
			"48 8D 4D CF " // lea     rcx, [rbp+4Fh+var_80]
			"E8 XX XX XX XX ", // call    ls__FileReader__dtor
			{}, // Unconditional
			{"ls::FileReader::FileReader", SymbolMappingTarget::kIndirectCall, 27, (void **)&gStaticSymbols.FileReaderCtor},
			{"ls::FileReader::~FileReader", SymbolMappingTarget::kIndirectCall, 50, (void **)&gStaticSymbols.FileReaderDtor}
		},

		// TODO - find FileReaderDtor

		{
			"PathRoots",
			SymbolMappingData::kText, SymbolMappingData::kCritical,
			"48 83 EC 20 " // sub     rsp, 20h
			"0F B7 C2 " // movzx   eax, dx
			"48 8B D9 " // mov     rbx, rcx
			"48 8D 0D XX XX XX XX " // lea     rcx, ls__Path__s_Roots
			"49 8B F8 " // mov     rdi, r8
			"48 8D 34 C1 ", // lea     rsi, [rcx+rax*8]
			{}, // Unconditional
			{"PathRoots", SymbolMappingTarget::kIndirectLea, 10, (void **)&gStaticSymbols.PathRoots}
		},

		{
			"esv::GameActionManager::CreateAction",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
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
			"E8 XX XX XX XX ", // call    esv__GameActionManager__CreateAction
			{SymbolMappingCondition::kFixedString, 0, "GameAction"},
			{"esv::GameActionManager::CreateAction", SymbolMappingTarget::kIndirectCall, 38, (void **)&gStaticSymbols.CreateGameAction}
		},

		{
			"esv::ProjectileHelpers::ShootProjectile",
			SymbolMappingData::kText, 0,
			"C6 45 AE 01 " // mov     [rbp+50h+var_A2], 1
			"F6 80 08 02 00 00 0C " // test    byte ptr [rax+208h], 0Ch
			"0F 95 45 AD " // setnz   [rbp+50h+var_A3]
			"E8 XX XX XX XX ", // call    esv__ProjectileHelpers__ShootProjectile
			{},
			{"esv::ProjectileHelpers::ShootProjectile", SymbolMappingTarget::kIndirectCall, 15, (void **)&gStaticSymbols.ShootProjectile}
		},

		{
			"LevelManager",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"0F 84 XX 00 00 00 " // jz      xxx
			"4C 8B 00 " // mov     r8, [rax]
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_ForceMove
			"48 8B C8 " // mov     rcx, rax
			"41 FF 50 28 " // call    qword ptr [r8+28h]
			"4D 8B 06 " // mov     r8, [r14]
			"49 8B CE ", // mov     rcx, r14
			{SymbolMappingCondition::kFixedString, 9, "ForceMove"},
			{"LevelManager", SymbolMappingTarget::kAbsolute, 0, nullptr, nullptr, &sSymbolLevelManager, 0x100}
		},

		{
			"TornadoAction",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"EB XX " // jmp     short xxx
			"48 8B 00 " // mov     rax, [rax]
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_RandomPoints
			"48 8B XX " // mov     rcx, rbx
			"FF 50 28 " // call    qword ptr [rax+28h]
			"4C 8B XX ", // mov     r8, [rbx]
			{SymbolMappingCondition::kFixedString, 5, "RandomPoints"},
			{"TornadoAction", SymbolMappingTarget::kAbsolute, -0x200, nullptr, nullptr, &sSymbolTornadoAction, 0x200}
		},

		{
			"esv::StatusMachine::CreateStatus",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
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
			"E8 XX XX XX XX ", //  call    esv__StatusMachine__CreateStatus
			{SymbolMappingCondition::kFixedString, 23, "LIFESTEAL"},
			{"esv::StatusMachine::CreateStatus", SymbolMappingTarget::kIndirectCall, 50, (void **)&gStaticSymbols.StatusMachineCreateStatus},
			{"esv::StatusMachine::ApplyStatus", SymbolMappingTarget::kAbsolute, 55, nullptr, nullptr, &sSymbolApplyStatus, 0x100},
		},

		{
			"esv::StatusHit::__vftable",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"4C 8D 0D XX XX XX XX " // lea     r9, fsx_Dummy_BodyFX
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_RS3_FX_GP_Status_Retaliation_Beam_01
			"E8 XX XX XX XX " // call    esv__EffectFactory__CreateEffectWrapper
			"48 8B D8 ", // mov     rbx, rax
			{SymbolMappingCondition::kFixedString, 7, "RS3_FX_GP_Status_Retaliation_Beam_01"},
			{"esv::StatusHit::__vftable", SymbolMappingTarget::kAbsolute, -0xa00, nullptr, nullptr, &sSymbolStatusHit, 0xa00}
		},

		{
			"esv::StatusHeal::__vftable",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"45 33 C9 " // xor     r9d, r9d
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_TargetDependentHeal
			"48 8B CB " // mov     rcx, rbx
			"FF 90 B0 01 00 00 ", // call    qword ptr [rax+1B0h]
			{SymbolMappingCondition::kFixedString, 3, "TargetDependentHeal"},
			{"esv::StatusHeal::__vftable", SymbolMappingTarget::kAbsolute, -0x200, nullptr, nullptr, &sSymbolStatusHeal, 0xa00}
		},

		{
			"esv::ParseItem",
			SymbolMappingData::kText, 0,
			"45 0F B6 C1 " // movzx   r8d, r9b
			"48 8B CF " // mov     rcx, rdi
			"E8 XX XX XX XX " // call    esv__ParseItem
			"33 D2 " // xor     edx, edx
			"48 8D 4C 24 38 " // lea     rcx, [rsp+78h+var_40]
			"E8 XX XX XX XX ", // call    esv__CreateItemFromParsed
			{},
			{"esv::ParseItem", SymbolMappingTarget::kIndirectCall, 7, (void **)&gStaticSymbols.ParseItem},
			{"esv::CreateItemFromParsed", SymbolMappingTarget::kIndirectCall, 19, (void **)&gStaticSymbols.CreateItemFromParsed},
		},

		{
			"UICharacterSheetHook",
			SymbolMappingData::kText, 0,
			"48 89 5D 60 " // mov     [rbp+0D0h+var_70], rbx
			"0F B6 88 B0 00 00 00 " // movzx   ecx, byte ptr [rax+0B0h]
			"41 3A CF " // cmp     cl, r15b
			"F2 0F 11 75 08 " // movsd   [rbp+0D0h+var_C8], xmm6
			"48 8B 4E 18 " // mov     rcx, [rsi+18h]
			"C7 45 B0 03 00 00 00 " // mov     [rbp+0D0h+var_120], 3
			// Replacement: c6 45 f8 01             mov    BYTE PTR [rbp-0x8],0x1
			"0F 94 45 F8 ", // setz    [rbp+0D0h+var_D8]
			{},
			{"UICharacterSheetHook", SymbolMappingTarget::kAbsolute, 30, (void **)&gStaticSymbols.UICharacterSheetHook},
		},

		{
			"ActivateEntitySystemHook",
			SymbolMappingData::kText, 0,
			"48 03 93 D0 00 00 00 " // add     rdx, [rbx+0D0h]
			"E8 XX XX XX XX " // call    ecs__EntityWorld__ActivateSystemInternal
			"48 8B 05 XX XX XX XX " // mov     rax, cs:ls__GlobalSwitches
			"80 B8 F3 0B 00 00 01 " // cmp     byte ptr [rax+0BF3h], 1
			// Replacement: 90 90
			"75 XX " // jnz     short loc_1415A124C
			"48 6B 15 XX XX XX XX 38 " // imul    rdx, cs:qword_1428F14F0, 38h
			"48 8B CB ", // mov     rcx, rbx
			{},
			{"ActivateEntitySystemHook", SymbolMappingTarget::kAbsolute, 26, nullptr, &FindActivateEntitySystemEoCApp},
		},

		{
			"CustomStatUIRollHook",
			SymbolMappingData::kText, 0,
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
			"48 8D 05 ", // lea     rax, ecl__CustomStatsProtocolSomething__vftable
			{},
			{"CustomStatUIRollHook", SymbolMappingTarget::kAbsolute, 0, (void **)&gStaticSymbols.CustomStatUIRollHook},
		},

		{
			"esv::CustomStatsProtocol::ProcessMsg",
			SymbolMappingData::kText, 0,
			// Replacement: C3 (retn)
			"4C 89 4C 24 20 " // mov     [rsp+arg_18], r9
			"53 " // push    rbx
			"57 " // push    rdi
			"41 56 " // push    r14
			"48 83 EC 30 " // sub     rsp, 30h
			"41 8B 51 08 " // mov     edx, [r9+8]
			"49 8B F9 " // mov     rdi, r9
			"4D 8B F0 " // mov     r14, r8
			"81 FA 3B 01 00 00 ", // cmp     edx, 13Bh
			{},
			{"esv::CustomStatsProtocol::ProcessMsg", SymbolMappingTarget::kAbsolute, 0, (void **)&gStaticSymbols.EsvCustomStatsProtocolProcessMsg},
		},

		{
			"NetworkFixedStrings",
			SymbolMappingData::kText, 0,
			"4C 8D 05 XX XX XX XX " // lea     r8, aNetworkfixedst ; "NetworkFixedStrings"
			"BA 01 00 00 00 " // mov     edx, 1
			"48 8B CF " // mov     rcx, rdi
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"48 8B 15 XX XX XX XX " // mov     rdx, cs:qword_14297F520
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:eoc__gNetworkFixedStrings
			"48 83 C2 28 " // add     rdx, 28h
			"E8 XX XX XX ", // call    eoc__NetworkFixedStrings__RegisterAll
			{SymbolMappingCondition::kString, 0, "NetworkFixedStrings"},
			{"NetworkFixedStrings", SymbolMappingTarget::kIndirectLea, 28, (void **)&gStaticSymbols.NetworkFixedStrings},
			{"InitNetworkFixedStrings", SymbolMappingTarget::kIndirectCall, 39, (void **)&gStaticSymbols.InitNetworkFixedStrings}
		},

		{
			"ErrorFuncs",
			SymbolMappingData::kText, 0,
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:ecl__gEocClient
			"4C 8D 8D 08 04 00 00 " // lea     r9, [rbp+8F8h+var_4F0]
			"41 B0 01 " // mov     r8b, 1
			"48 8D 95 E8 03 00 00 " // lea     rdx, [rbp+8F8h+var_510]
			"E8 XX XX XX XX " // call    ecl__EocClient__HandleError
			"48 8D 8D E8 03 00 00 ", // lea     rdx, [rbp+8F8h+var_510]
			{},
			{"ecl::EoCClient", SymbolMappingTarget::kIndirectLea, 0, (void **)&gStaticSymbols.EoCClient},
			{"ecl::EoCClient::HandleError", SymbolMappingTarget::kIndirectCall, 24, (void **)&gStaticSymbols.EoCClientHandleError}
		},

		{
			"eoc::SkillPrototypeManager::Init",
			SymbolMappingData::kText, 0,
			"4C 8D 05 XX XX XX XX " // lea     r8, str_Skills     ; "Skills"
			"BA 01 00 00 00 " // mov     edx, 1
			"48 8B CF " // mov     rcx, rdi
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:eoc__gSkillPrototypeManager
			"E8 XX XX XX XX ", // call    eoc__SkillPrototypeManager__Init
			{SymbolMappingCondition::kString, 0, "Skills"},
			{"eoc::SkillPrototypeManager::Init", SymbolMappingTarget::kIndirectCall, 28, (void **)&gStaticSymbols.SkillPrototypeManagerInit}
		},

		{
			"GameStateChangedEvent",
			SymbolMappingData::kText, 0,
			"4C 8D 0D XX XX XX XX " // lea r9, aClientStateSwa ; "CLIENT STATE SWAP - from: %s, to: %s\n"
			"4C 89 44 24 28 " // mov     [rsp+68h+var_40], r8
			"33 D2 " // xor     edx, edx
			"4C 8D 44 24 30 ", // lea     r8, [rsp+68h+var_38]
			{SymbolMappingCondition::kString, 0, "CLIENT STATE SWAP - from: %s, to: %s\n"},
			{"GameStateChangedEvent", SymbolMappingTarget::kIndirectCall, 0x2A, (void **)&gStaticSymbols.GameStateChangedEvent}
		},

		{
			"CharacterStatsGetters",
			SymbolMappingData::kText, 0,
			"40 84 ED " // test    bpl, bpl
			"74 08 " // jz      short loc_141C7A428
			"8B 86 44 03 00 00 " // mov     eax, [rsi+344h]
			"EB 06 " // jmp     short loc_141C7A42E
			"8B 86 40 03 00 00 " // mov     eax, [rsi+340h]
			"40 0F B6 D5 " // movzx   edx, bpl
			"89 43 44 ", // mov     [rbx+44h], eax
			{},
			{"CharacterStatsGetters", SymbolMappingTarget::kAbsolute, 0, nullptr, &FindCharacterStatGettersEoCApp}
		},

		{
			"GetHitChance",
			SymbolMappingData::kText, 0,
			"40 56 " // push    rsi
			"57 " // push    rdi
			"48 83 EC 28 " // sub     rsp, 28h
			"45 33 C0 " // xor     r8d, r8d
			"48 8B F2 " // mov     rsi, rdx
			"48 8B F9 " // mov     rdi, rcx
			"41 8D 50 78 " // lea     edx, [r8+78h]
			"E8 XX XX XX XX ", // call    CDivinityStats_Character__HasTalent
			{},
			{"GetHitChance", SymbolMappingTarget::kAbsolute, 0, (void **)&gCharacterStatsGetters.GetHitChance, nullptr, &sSymbolChanceToHitBoost, 0x200}
		},

		{
			"esv::Character::Hit",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"74 74 " // jz      short loc_141A3EED4
			"48 8B 00 " // mov     rax, [rax]
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_DamageItems
			"48 8B CE " // mov     rcx, rsi
			"FF 50 28 ", // call    qword ptr [rax+28h]
			{SymbolMappingCondition::kFixedString, 5, "DamageItems"},
			{"esv::Character::Hit", SymbolMappingTarget::kAbsolute, 0, nullptr, nullptr, &sSymbolCharacterHit, 0x400}
		},

		{
			"FindLibrariesEoCApp",
			SymbolMappingData::kText, SymbolMappingData::kAllowFail,
			"8B C8 " // mov ecx, eax
			"FF C0 " // inc eax
			"89 05 XX XX XX XX " // mov cs:xxx, eax
			"85 C9 " // test ecx, ecx
			"75 17 " // jnz short loc_xxx
			"44 8D 41 XX " // lea r8d, [rdx+XXh]
			"48 8D 0D XX XX XX XX " // lea rcx, xxx
			"48 8D 15 XX XX XX XX ", // lea rdx, xxx
			{},
			{"FindLibrariesEoCApp", SymbolMappingTarget::kAbsolute, 0, nullptr, &FindLibrariesEoCApp}
		},

		{
			"esv::ActionMachine::UpdateSyncState",
			SymbolMappingData::kText, 0,
			"C6 44 24 28 01 " // mov     [rsp+58h+var_30], 1
			"45 33 C9 " // xor     r9d, r9d
			"45 33 C0 " // xor     r8d, r8d
			"C6 44 24 20 00 " // mov     [rsp+58h+var_38], 0
			"41 0F B6 D2 " // movzx   edx, r10b
			"41 C6 44 3B 53 00 " // mov     byte ptr [r11+rdi+53h], 0
			"48 8B CF " // mov     rcx, rdi
			"E8 XX XX XX XX ", // call    xxx
			{},
			{"esv::ActionMachine::UpdateSyncState", SymbolMappingTarget::kIndirectCall, 29, nullptr, &FindActionMachineSetState}
		},

		{
			"eoc::SkillPrototype::FormatDescriptionParam",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 83 CF FF " // or      rdi, 0FFFFFFFFFFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_Damage_Multiplier
			"44 8B C7 " // mov     r8d, edi
			"48 8B CB " // mov     rcx, rbx
			"E8 XX XX XX XX " // call    sub_141C979B0
			"8B C8 ", // mov     ecx, eax
			{SymbolMappingCondition::kFixedString, 4, "Damage Multiplier"},
			{"eoc::SkillPrototype::FormatDescriptionParam", SymbolMappingTarget::kAbsolute, -0x200, nullptr, nullptr, &sSymbolSkillPrototypeFormatDescriptionParam, 0x200}
		},

		{
			"eoc::SkillPrototype::GetSkillDamage",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"0F 84 4D 06 00 00 " // jz      loc_xxx
			"4C 8B 00 " // mov     r8, [rax]
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_Damage_Multiplier
			"48 8B C8 ", // mov     rcx, rax
			{SymbolMappingCondition::kFixedString, 9, "Damage Multiplier"},
			{"eoc::SkillPrototype::GetSkillDamage", SymbolMappingTarget::kAbsolute, -0x200, nullptr, nullptr, &sSymbolSkillPrototypeGetSkillDamage, 0x200}
		},

		{
			"eoc::StatusPrototype::FormatDescriptionParam",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"49 89 7B C8 " // mov     [r11-38h], rdi
			"0F 85 B5 00 00 00 " // jnz     loc_xxx
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_DamageStats
			"49 8B CE ", // mov     rcx, r14
			{SymbolMappingCondition::kFixedString, 10, "DamageStats"},
			{"eoc::StatusPrototype::FormatDescriptionParam", SymbolMappingTarget::kAbsolute, -0x100, nullptr, nullptr, &sSymbolStatusPrototypeFormatDescriptionParam, 0x100}
		},

		{
			"esv::EoCServer",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_CurrentCellCount
			"48 8B CB " // mov     rcx, rbx
			"FF 90 90 01 00 00 " // call    qword ptr [rax+190h]
			"48 8B 03 " // mov     rax, [rbx]
			"48 8B CB " // mov     rcx, rbx
			"4C 8B 3D XX XX XX XX ", //  mov     rdi, cs:esv__gEoCServer
			{SymbolMappingCondition::kFixedString, 0, "CurrentCellCount"},
			{"esv::EoCServer", SymbolMappingTarget::kIndirectLea, 22, (void **)&gStaticSymbols.EoCServer}
		},
	};

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

	void LibraryManager::MapAllSymbols(bool deferred)
	{
		for (auto i = 0; i < std::size(sSymbolMappings); i++) {
			if ((deferred && (sSymbolMappings[i].Flag & SymbolMappingData::kDeferred))
				|| (!deferred && !(sSymbolMappings[i].Flag & SymbolMappingData::kDeferred))) {
				MapSymbol(sSymbolMappings[i], nullptr, 0);
			}
		}
	}

	void LibraryManager::FindServerGlobalsEoCApp()
	{
		StaticSymbols::EoCLibraryInfo const * serverLib{ nullptr };
		for (auto const & v : gStaticSymbols.Libraries) {
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
					gStaticSymbols.ServerRegisterFuncs[i] = registrantFunc;
					funcPtr += 5;
				}

				funcPtr = p + 16 + (36 * 5) + 0x2C;
				for (auto i = 0; i < 14; i++) {
					int32_t rel = *(int32_t *)(funcPtr + 1);
					uint8_t const * registrantFunc = funcPtr + rel + 5;
					gStaticSymbols.ServerRegisterFuncs[i + 36] = registrantFunc;
					funcPtr += 5;
				}

				for (auto i = 0; i < std::size(gStaticSymbols.ServerRegisterFuncs); i++) {
					uint8_t const * r = gStaticSymbols.ServerRegisterFuncs[i];
					if (r[0] == 0x48 && r[1] == 0x83 && r[2] == 0xEC /* sub rsp, XXh */
						&& r[4] == 0x48 && r[5] == 0x8B && r[6] == 0x05 /* mov rax, cs:xxx */) {

						int32_t rel = *(int32_t *)(r + 7);
						uint8_t const * registrantObj = r + rel + 4 + 7;
						gStaticSymbols.ServerGlobals[i] = (uint8_t const **)registrantObj;

					}
					else {
						gStaticSymbols.ServerGlobals[i] = nullptr;
						DEBUG("LibraryManager::FindServerGlobalsEoCApp(): Could not extract global from func @ %p", r);
					}
				}
			}
		}

		gStaticSymbols.EsvCharacterFactory = (CharacterFactory **)gStaticSymbols.ServerGlobals[(unsigned)EsvGlobalEoCApp::EsvCharacterFactory];
		gStaticSymbols.EsvItemFactory = (ItemFactory **)gStaticSymbols.ServerGlobals[(unsigned)EsvGlobalEoCApp::EsvItemFactory];

		if (gStaticSymbols.EsvCharacterFactory == nullptr || gStaticSymbols.EsvItemFactory == nullptr) {
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
		for (auto const & lib : gStaticSymbols.Libraries) {
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

		for (auto i = 1; i < std::size(gStaticSymbols.EocRegisterFuncs); i++) {
			int32_t rel = *(int32_t *)(globalsInitCode + 1);
			uint8_t const * registrantFunc = globalsInitCode + rel + 5;
			gStaticSymbols.EocRegisterFuncs[i] = registrantFunc;
			globalsInitCode += 5;
		}

		for (auto i = 1; i < std::size(gStaticSymbols.EocRegisterFuncs); i++) {
			uint8_t const * r = gStaticSymbols.EocRegisterFuncs[i];
			if (r[0] == 0x48 && r[1] == 0x83 && r[2] == 0xEC /* sub rsp, XXh */
				&& r[4] == 0x48 && r[5] == 0x8B && r[6] == 0x05 /* mov rax, cs:xxx */) {

				int32_t rel = *(int32_t *)(r + 7);
				uint8_t const * registrantObj = r + rel + 4 + 7;
				gStaticSymbols.EocGlobals[i] = (uint8_t const **)registrantObj;

			} else {
				gStaticSymbols.EocGlobals[i] = nullptr;
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

				gStaticSymbols.GlobalStrings = (GlobalStringTable const **)(p + rel + 23 + 7);
				GlobalStringTable::UseMurmur = true;
				break;
			}
		}

		if (gStaticSymbols.GlobalStrings == nullptr) {
			ERR("LibraryManager::FindGlobalStringTableEoCApp(): Could not find global string table");
			CriticalInitFailed = true;
		}
	}
}
#endif
