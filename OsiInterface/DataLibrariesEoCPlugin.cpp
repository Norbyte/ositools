#include "stdafx.h"
#include "DataLibraries.h"
#include <OsirisProxy.h>
#include <string>
#include <functional>
#include <psapi.h>
#include <GameDefinitions/UI.h>

#if !defined(OSI_EOCAPP)
namespace dse
{
	SymbolMappingData const sSymbolLevelManager = {
		"LevelManager2",
		SymbolMappingData::kCustom, 0,
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
		"E8 XX XX XX XX ", // call    esv__GameActionManager__AddAction
		{},
		{"LevelManager", SymbolMappingTarget::kIndirect, 0, STATIC_SYM(EsvLevelManager)},
		{"esv::GameObjectMoveAction::Setup", SymbolMappingTarget::kIndirect, 57, STATIC_SYM(GameObjectMoveActionSetup)},
		{"esv::GameActionManager::AddAction", SymbolMappingTarget::kIndirect, 68, STATIC_SYM(AddGameAction)}
	};

	SymbolMappingData const sSymbolTornadoAction = {
		"TornadoAction2",
		SymbolMappingData::kCustom, 0,
		"48 8B C4 " // mov     rax, rsp
		"53 " // push    rbx
		"55 ", // push    rbp
		{},
		{"esv::TornadoAction::Setup", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(TornadoActionSetup)}
	};

	SymbolMappingData const sSymbolWallAction = {
		"WallAction2",
		SymbolMappingData::kCustom, 0,
		"48 8B C4 " // mov     rax, rsp
		"48 89 58 18 " // mov     [rax+18h], rbx
		"48 89 70 20 ", // mov     [rax+20h], rsi
		{},
		{"esv::TornadoAction::Setup", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(WallActionCreateWall)}
	};

	SymbolMappingData const sSymbolSummonHelpersSummon = {
		"esv::SummonHelpers::Summon",
		SymbolMappingData::kCustom, 0,
		"48 8D 55 E0 " // lea     rdx, [rbp+0B0h+summonArgs]
		"C6 45 0D 00 " // mov     [rbp+0B0h+summonArgs.MapToAiGrid_M], 0
		"48 8D 4C 24 50 " // lea     rcx, [rsp+1B0h+var_160]
		"E8 XX XX XX XX " // call    esv__SummonHelpers__Summon
		"48 8D 4C 24 50 ", // lea     rcx, [rsp+1B0h+var_160]
		{},
		{"esv::SummonHelpers::Summon", SymbolMappingTarget::kIndirect, 13, STATIC_SYM(SummonHelpersSummon)}
	};

	SymbolMappingData const sSymbolApplyStatus = {
		"ApplyStatus",
		SymbolMappingData::kCustom, 0,
		"C7 43 2C 00 00 00 00 " // mov     dword ptr [rbx+2Ch], 0
		"48 8B CF " // mov     rcx, rdi
		"E8 XX XX XX XX " // call    esv__StatusMachine__ApplyStatus
		"48 8B 7C 24 40 ", // mov     rdi, [rsp+28h+arg_10]
		{},
		{"esv::StatusMachine::ApplyStatus", SymbolMappingTarget::kIndirect, 10, STATIC_SYM(esv__StatusMachine__ApplyStatus)}
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
				GetStaticSymbols().StatusHitVMT = reinterpret_cast<esv::StatusVMT const *>(p - 12 * 8);
				return SymbolMappingResult::Success;
			}
		}

		return SymbolMappingResult::Fail;
	}

	SymbolMappingData const sSymbolStatusHit = {
		"esv::StatusHit::__vftable",
		SymbolMappingData::kCustom, 0,
		"48 8B C4 " // mov     rax, rsp
		"55 " // push    rbp
		"53 ", // push    rbx
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
				GetStaticSymbols().StatusHealVMT = reinterpret_cast<esv::StatusVMT const *>(p - 25 * 8);
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
		if (GetStaticSymbols().ActivateClientSystemsHook == nullptr) {
			GetStaticSymbols().ActivateClientSystemsHook = match;
			return SymbolMappingResult::TryNext;
		} else {
			GetStaticSymbols().ActivateServerSystemsHook = match;
			return SymbolMappingResult::Success;
		}
	}

	SymbolMappingData const sSymbolGetAbility = {
		"GetAbility",
		SymbolMappingData::kCustom, 0,
		"45 33 C9 " // xor     r9d, r9d 
		"45 33 C0 " // xor     r8d, r8d
		"8B D0 " // mov     edx, eax 
		"48 8B CF " // mov     rcx, rdi 
		"E8 XX XX XX XX ", // call    CDivinityStats_Character__GetAbility
		{},
		{"GetAbility", SymbolMappingTarget::kIndirect, 11, CHAR_GETTER_SYM(GetAbility)}
	};

	SymbolMappingData const sSymbolGetTalent = {
		"GetTalent",
		SymbolMappingData::kCustom, 0,
		"45 0F B6 C5 " // movzx   r8d, r13b
		"BA 03 00 00 00 " // mov     edx, 3
		"48 8B CF " // mov     rcx, rdi
		"45 03 FC " // add     r15d, r12d 
		"E8 XX XX XX XX ", // call    CDivinityStats_Character__HasTalent
		{},
		{"GetTalent", SymbolMappingTarget::kIndirect, 15, CHAR_GETTER_SYM(GetTalent)}
	};

	SymbolMappingResult FindCharacterStatGettersEoCApp(uint8_t const * match)
	{
		auto & getters = GetStaticSymbols().CharStatsGetters;

		Pattern p2;
		p2.FromString(
			"49 8B CF " // mov     rcx, r15
			"E8 XX XX XX XX " // call    CDivinityStats_Character__Getxxx
		);

		unsigned ptrIndex = 0;
		p2.Scan(match, 0x240, [&ptrIndex, &getters](const uint8_t * match) -> std::optional<bool> {
			if (ptrIndex < std::size(getters.Ptrs)) {
				auto ptr = AsmResolveInstructionRef(match + 3);
				getters.Ptrs[ptrIndex++] = (void *)ptr;
			}

			return {};
		});

		if (getters.GetBlockChance != nullptr) {
			auto & library = gOsirisProxy->GetLibraryManager();
			library.MapSymbol(sSymbolGetAbility, (uint8_t *)getters.GetDodge, 0x480);
			library.MapSymbol(sSymbolGetTalent, (uint8_t *)getters.GetDodge, 0x480);
		}

		return (getters.GetBlockChance != nullptr) ? SymbolMappingResult::Success : SymbolMappingResult::Fail;
	}

	SymbolMappingData const sSymbolChanceToHitBoost = {
		"GetChanceToHitBoost",
		SymbolMappingData::kCustom, 0,
		"48 0F 4D C2 " // cmovge  rax, rdx
		"33 D2 " // xor     edx, edx
		"8B 18 " // mov     ebx, [rax]
		"E8 XX XX XX XX ", // call    CDivinityStats_Character__GetChanceToHitBoost
		{},
		{"GetChanceToHitBoost", SymbolMappingTarget::kIndirect, 8, CHAR_GETTER_SYM(GetChanceToHitBoost)}
	};

	SymbolMappingData const sSymbolCharacterHitInternal = {
		"CDivinityStats_Character::_HitInternal",
		SymbolMappingData::kCustom, 0,
		"88 44 24 28 " // mov     [rsp+0A8h+var_80], al
		"89 6C 24 20 " // mov     [rsp+0A8h+var_88], ebp
		"E8 XX XX XX XX ", // call    CDivinityStats_Character___HitInternal
		{},
		{"CDivinityStats_Character::_HitInternal", SymbolMappingTarget::kIndirect, 8, STATIC_SYM(CDivinityStats_Character__HitInternal)}
	};

	SymbolMappingData const sSymbolCharacterHit = {
		"esv::Character::Hit",
		SymbolMappingData::kCustom, 0,
		"48 89 44 24 30 " // mov     qword ptr [rsp+150h+a7], rax
		"C6 44 24 28 00 " // mov     byte ptr [rsp+150h+a6], 0
		"C7 44 24 20 05 00 00 00 " // mov     [rsp+150h+a5], 5
		"E8 XX XX XX XX " // call    esv__Character__Hit
		"XX 8B XX B0 01 00 00 " // mov     r13, [r15+1B0h]
		"EB 66 ", // jmp short xxx
		{},
		{"esv::Character::Hit", SymbolMappingTarget::kIndirect, 18, STATIC_SYM(esv__Character__Hit), nullptr, &sSymbolCharacterHitInternal, 0x280}
	};

	SymbolMappingResult FindLibrariesEoCPlugin(uint8_t const * match)
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

			return SymbolMappingResult::TryNext;
		} else {
			return SymbolMappingResult::Fail;
		}
	}

	SymbolMappingResult FindActionMachineSetState(uint8_t const * match)
	{
		if (GetStaticSymbols().ecl__ActionMachine__SetState == nullptr) {
			GetStaticSymbols().ecl__ActionMachine__SetState = (esv::ActionMachine::SetStateProc*)match;
			return SymbolMappingResult::TryNext;
		} else {
			GetStaticSymbols().esv__ActionMachine__SetState = (esv::ActionMachine::SetStateProc*)match;
			return SymbolMappingResult::Success;
		}
	}

	SymbolMappingData const sSymbolSkillPrototypeFormatDescriptionParam = {
		"eoc::SkillPrototype::FormatDescriptionParam2",
		SymbolMappingData::kCustom, 0,
		"40 55 " // push    rbp
		"53 " // push    rbx
		"56 ", // push    rsi
		{},
		{"eoc::SkillPrototype::FormatDescriptionParam2", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(SkillPrototype__FormatDescriptionParam)}
	};

	SymbolMappingData const sSymbolSkillPrototypeGetSkillDamage = {
		"eoc::SkillPrototype::GetSkillDamage2",
		SymbolMappingData::kCustom, 0,
		"44 88 4C 24 20 " // mov     [rsp-8+arg_18], r9b
		"55 " // push    rbp
		"53 ", // push    rbx
		{},
		{"eoc::SkillPrototype::GetSkillDamage2", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(SkillPrototype__GetSkillDamage)}
	};

	SymbolMappingData const sSymbolStatusPrototypeFormatDescriptionParam = {
		"eoc::StatusPrototype::FormatDescriptionParam2",
		SymbolMappingData::kCustom, 0,
		"40 53 " // push    rbx
		"55 " // push    rbp
		"56 ", // push    rsi
		{},
		{"eoc::StatusPrototype::FormatDescriptionParam2", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(StatusPrototype__FormatDescriptionParam)}
	};

	SymbolMappingData const sSymbolMappings[] = {
		{
			"esv::GameActionManager::CreateAction",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_GameAction
			"4C 8B 05 XX XX XX XX " // mov     r8, cs:xxx
			"48 8B CF " // mov     rcx, rdi
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"84 C0 " // test    al, al
			"0F 84 XX XX 00 00 " // jz      xxx
			"49 8B D6 " // mov     rdx, r14
			"48 8D 4D XX " // lea     rcx, [rbp+57h+Memory]
			"E8 XX XX XX XX " // call xxx
			"45 84 E4 " // test    r12b, r12b
			"74 XX " // jz      short xxx
			"49 8B D6 " // mov     rdx, r14
			"48 8D 4D XX " // lea     rcx, [rbp+57h+Memory]
			"48 8B D8 " // mov     rbx, rax
			"E8 XX XX XX XX " // call    xxx
			"4C 8B 03 " // mov     r8, [rbx]
			"49 8B CD " // mov     rcx, r13
			"8B 50 08 " // mov     edx, [rax+8]
			"E8 XX XX XX XX ", // call    esv__GameActionManager__CreateAction
			{SymbolMappingCondition::kFixedString, 0, "GameAction"},
			{"esv::GameActionManager::CreateAction", SymbolMappingTarget::kIndirect, 72, STATIC_SYM(CreateGameAction)}
		},

		{
			"esv::ProjectileHelpers::ShootProjectile",
			SymbolMappingData::kText, 0,
			"48 89 44 24 28 " // mov     [rsp+0F8h+aPosition], rax
			"4C 89 44 24 20 " // mov     [rsp+0F8h+a5], r8
			"E8 XX XX XX XX " // call    ShootProjectileHelperStruct__ctor
			"48 8D 4C 24 70 " // lea     rcx, [rsp+0F8h+shootProjectile]
			"89 BC 24 B8 00 00 00 " // mov     [rsp+0F8h+shootProjectile.CasterLevel], edi
			"E8 XX XX XX XX ", // call    esv__ProjectileHelpers__ShootProjectile
			{},
			{"esv::ProjectileHelpers::ShootProjectile", SymbolMappingTarget::kIndirect, 27, STATIC_SYM(esv__ProjectileHelpers__ShootProjectile)}
		},

		{
			"esv::Projectile::Explode",
			SymbolMappingData::kText, 0,
			"48 8D AC 24 30 FE FF FF " // lea     rbp, [rsp-1D0h]
			"48 81 EC D0 02 00 00 " // sub     rsp, 2D0h
			"44 0F 29 BC 24 30 02 00 00 ", // movaps  [rsp+2F0h+var_C0], xmm15
			{},
			{"esv::Projectile::Explode", SymbolMappingTarget::kAbsolute, -0x0E, STATIC_SYM(esv__Projectile__Explode)}
		},

		{
			"LevelManager",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_ForceMove
			"48 8B C8 " // mov     rcx, rax
			"E8 XX XX XX XX " // call    eoc__SkillPrototype__GetMappedValue
			"85 C0 " // test    eax, eax
			"75 25 " // jnz     short loc_180EE4875
			"4C 8D 46 70 ", // lea     r8, [rsi+70h]
			{SymbolMappingCondition::kFixedString, 4, "ForceMove"},
			{"LevelManager", SymbolMappingTarget::kAbsolute, 0, nullptr, nullptr, &sSymbolLevelManager, 0x100}
		},

		{
			"esv::WallAction::CreateWall",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_GrowTimeout
			"48 8B C8 " // mov     rcx, rax
			"48 8B F0 " // mov     rsi, rax
			"E8 XX XX XX XX " // call    eoc__SkillPrototype__GetMappedValueDiv1k
			"44 0F 28 F8 " // movaps  xmm15, xmm0
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_GrowSpeed
			"F3 44 0F 5E 3D XX XX XX XX ", // divss   xmm15, cs:dword_1819A9AD0
			{SymbolMappingCondition::kFixedString, 4, "GrowTimeout"},
			{"esv::WallAction::CreateWall", SymbolMappingTarget::kAbsolute, -0x100, nullptr, nullptr, &sSymbolWallAction, 0x100}
		},

		{
			"esv::TornadoAction::Setup",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 8D 57 50 " // lea     rdx, [rdi+50h]
			"E8 XX XX XX XX " // call    eoc__SkillPrototypeManager__GetPrototype
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_RandomPoints
			"48 8B C8 " // mov     rcx, rax
			"48 8B F0 " // mov     rsi, rax
			"E8 XX XX XX XX ", // call    eoc__SkillPrototype__GetMappedValue
			{SymbolMappingCondition::kFixedString, 13, "RandomPoints"},
			{"esv::TornadoAction::Setup", SymbolMappingTarget::kAbsolute, -0x100, nullptr, nullptr, &sSymbolTornadoAction, 0x100}
		},

		{
			"esv::SummonHelpers::Summon",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"44 0F 28 94 24 20 01 00 00 " // movaps  xmm10, [rsp+1B0h+var_90]
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_SpawnObject
			"49 8B CE ", // mov     rcx, r14
			{SymbolMappingCondition::kFixedString, 9, "SpawnObject"},
			{"esv::SummonHelpers::Summon", SymbolMappingTarget::kAbsolute, -0x400, nullptr, nullptr, &sSymbolSummonHelpersSummon, 0x400}
		},

		{
			"esv::StatusMachine::CreateStatus",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
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
			"E8 XX XX XX XX ", //  call    esv__StatusMachine__CreateStatus
			{SymbolMappingCondition::kFixedString, 19, "LIFESTEAL"},
			{"esv::StatusMachine::CreateStatus", SymbolMappingTarget::kIndirect, 49, STATIC_SYM(esv__StatusMachine__CreateStatus)},
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
			{"esv::StatusHit::__vftable", SymbolMappingTarget::kAbsolute, -0x600, nullptr, nullptr, &sSymbolStatusHit, 0x600}
		},

		{
			"esv::StatusHeal::__vftable",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"45 33 C9 " // xor     r9d, r9d
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_TargetDependentHeal
			"48 8B CB " // mov     rcx, rbx
			"FF 90 B0 01 00 00 ", // call    qword ptr [rax+1B0h]
			{SymbolMappingCondition::kFixedString, 3, "TargetDependentHeal"},
			{"esv::StatusHeal::__vftable", SymbolMappingTarget::kAbsolute, -0x200, nullptr, nullptr, &sSymbolStatusHeal, 0x200}
		},

		{
			"esv::StatusHit::Setup",
			SymbolMappingData::kText, 0,
			"40 57 " // push    rdi
			"48 83 EC 20 " // sub     rsp, 20h
			"48 89 5C 24 30 " // mov     [rsp+28h+arg_0], rbx
			"48 8B F9 " // mov     rdi, rcx
			"48 8D 99 A0 00 00 00 " // lea     rbx, [rcx+0A0h]
			"48 89 6C 24 38 " // mov     [rsp+28h+arg_8], rbp
			"48 8B CB " // mov     rcx, rbx        ; this
			"48 89 74 24 40 " // mov     [rsp+28h+arg_10], rsi
			"4C 89 74 24 48 " // mov     [rsp+28h+arg_18], r14
			"E8 ", // call    HitDamageInfo__Copy
			{},
			{"esv::StatusHit::Setup", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(esv__StatusHit__Setup)}
		},

		{
			"esv::ParseItem",
			SymbolMappingData::kText, 0,
			"48 8B C8 " // mov     rcx, rax
			"48 89 5C 24 60 " // mov     [rsp+0B8h+var_58.VMT], rbx
			"E8 XX XX XX XX " // call    esv__ParseItem
			"33 D2 " // xor     edx, edx
			"48 8D 4C 24 60 " // lea     rcx, [rsp+0B8h+var_58]
			"E8 XX XX XX XX ", // call    esv__CreateItemFromParsed
			{},
			{"esv::ParseItem", SymbolMappingTarget::kIndirect, 8, STATIC_SYM(ParseItem)},
			{"esv::CreateItemFromParsed", SymbolMappingTarget::kIndirect, 20, STATIC_SYM(CreateItemFromParsed)},
		},

		{
			"UICharacterSheetHook",
			SymbolMappingData::kText, 0,
			"41 0F B6 D5 " // movzx   edx, r13b
			"48 8D 4D 10 " // lea     rcx, [rbp+0D0h+var_C0]
			"FF 15 XX XX XX XX " // call    cs:??0InvokeDataValue@ls@@QEAA@_N@Z
			"48 8B 05 XX XX XX XX " // mov     rax, cs:ecl__gEocClient
			"48 8D 4D B0 " // lea     rcx, [rbp+0D0h+var_120]
			// Replacement: B2 01 90 90 90 90 90 90 90 90
			"44 38 B8 B0 00 00 00 " // cmp     [rax+0B0h], r15b
			"0F 94 C2 ", // setz    dl
			{},
			{"UICharacterSheetHook", SymbolMappingTarget::kAbsolute, 25, STATIC_SYM(UICharacterSheetHook)},
		},

		{
			"ActivateEntitySystemHook",
			SymbolMappingData::kText, 0,
			"48 8B CB " // mov     rcx, rbx
			"E8 XX XX XX XX " // call    xxx
			"48 8B CB " // mov     rcx, rbx
			"E8 XX XX XX XX " // call    xxx
			"48 8B 05 XX XX XX XX " // mov     rax, cs:gGlobalSwitches
			"48 8B 08 " // mov     rcx, [rax]
			"80 B9 03 0C 00 00 01 " // cmp     byte ptr [rcx+0C03h], 1
			// Replacement: 90 90
			"75 XX " // jnz     short xxx
			"48 8B CB " // mov     rcx, rbx
			"E8 XX XX XX XX ", // call    xxx
			{},
			{"ActivateEntitySystemHook", SymbolMappingTarget::kAbsolute, 33, nullptr, &FindActivateEntitySystemEoCApp},
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
			"48 8B 08 " // mov     rcx, [rax]
			"48 8B 05 XX XX XX XX " // mov     rax, cs:qword_182543378
			"49 89 43 98 ", // mov     [r11-68h], rax
			{},
			{"CustomStatUIRollHook", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(CustomStatUIRollHook)},
		},

		{
			"esv::CustomStatsProtocol::ProcessMsg",
			SymbolMappingData::kText, 0,
			"4C 89 4C 24 20 " // mov     [rsp+arg_18], r9
			"53 " // push    rbx
			"41 56 " // push    r14
			"41 57 " // push    r15
			"48 83 EC 40 " // sub     rsp, 40h
			"48 8D 15 XX XX XX XX ", // lea     rdx, "esv::CustomStatsProtocol"
			{SymbolMappingCondition::kString, 14, "esv::CustomStatsProtocol::ProcessMsg"},
			{"esv::CustomStatsProtocol::ProcessMsg", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(EsvCustomStatsProtocolProcessMsg)},
		},

		{
			"ErrorFuncs",
			SymbolMappingData::kText, 0,
			"48 8B 1D XX XX XX XX " // mov     rbx, cs:ecl__gEocClient
			"48 8D 8C 24 90 00 00 00 " // lea     rcx, [rsp+158h+var_C8]
			"41 B1 01 " // mov     r9b, 1
			"45 33 C0 " // xor     r8d, r8d
			"33 D2 " // xor     edx, edx
			"FF 15 XX XX XX XX " // call    cs:?Get@TranslatedString@ls@@QEBAAEBVSTDWString@2@W4EGender@2@0_N@Z 
			"4C 8D 4C 24 20 " // lea     r9, [rsp+158h+var_138]
			"45 33 C0 " // xor     r8d, r8d
			"48 8B D0 " // mov     rdx, rax
			"48 8B CB " // mov     rcx, rbx
			"E8 XX XX XX XX ", // call    ecl__EocClient__HandleError
			{},
			{"ecl::EoCClient", SymbolMappingTarget::kIndirect, 0, STATIC_SYM(EoCClient)},
			{"ecl::EoCClient::HandleError", SymbolMappingTarget::kIndirect, 43, STATIC_SYM(EoCClientHandleError)}
		},

		{
			"eoc::SkillPrototypeManager::Init",
			SymbolMappingData::kText, 0,
			"4C 8D 05 XX XX XX XX " // lea     r8, aSkills     ; "Skills"
			"BA 01 00 00 00 " // mov     edx, 1
			"49 8B CC " // mov     rcx, rsi
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:eoc__gSkillPrototypeManager
			"E8 XX XX XX XX ", // call    eoc__SkillPrototypeManager__Init
			{SymbolMappingCondition::kString, 0, "Skills"},
			{"eoc::SkillPrototypeManager::Init", SymbolMappingTarget::kIndirect, 28, STATIC_SYM(SkillPrototypeManagerInit)},
			{"eoc::SkillPrototypeManager", SymbolMappingTarget::kIndirect, 21, STATIC_SYM(eoc__SkillPrototypeManager)}
		},

		{
			"RPGStats::Load",
			SymbolMappingData::kText, 0,
			"48 8B D6 " // mov     rdx, rsi
			"49 8B CC " // mov     rcx, r12
			"E8 XX XX XX XX " // call    CRPGStatsManager__Load
			"48 8B 3D XX XX XX XX " // mov     rdi, cs:esv__gItemFactory
			"48 8B 1D XX XX XX XX " // mov     rbx, cs:ecl__gItemFactory
			"48 81 C7 20 01 00 00 " // add     rdi, 120h
			"48 8B CF " // mov     rcx, rdi
			"48 89 7D EF ", // mov     [rbp+57h+var_68], rdi
			{},
			{"RPGStats::Load", SymbolMappingTarget::kIndirect, 6, STATIC_SYM(RPGStats__Load)}
		},

		{
			"eoc::StatusPrototypeManager::Init",
			SymbolMappingData::kText, 0,
			"4C 8D 05 XX XX XX XX " // lea     r8, str_Skills  ; "Skills"
			"BA 01 00 00 00 " // mov     edx, 1
			"49 8B CC " // mov     rcx, r12
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"48 8B 0D XX XX XX XX ", // mov     rcx, cs:eoc__gSkillPrototypeManager
			{SymbolMappingCondition::kString, 0, "Statuses"},
			{"eoc::StatusPrototypeManager", SymbolMappingTarget::kIndirect, 21, STATIC_SYM(eoc__StatusPrototypeManager)}
		},

		{
			"ecl::GameStateEventManager::ExecuteGameStateChangedEvent",
			SymbolMappingData::kText, 0,
			"4C 8D 0D XX XX XX XX " // lea r9, aClientStateSwa ; "CLIENT STATE SWAP - from: %s, to: %s\n"
			"48 8B CF " // mov rcx, rdi
			"C7 44 24 30 80 00 00 00 " // mov [rsp+78h+var_48.logLevel], 80h
			"4C 8D 44 24 30 ", // lea r8, [rsp+78h+var_48]
			{SymbolMappingCondition::kString, 0, "CLIENT STATE SWAP - from: %s, to: %s\n"},
			{"ecl::GameStateEventManager::ExecuteGameStateChangedEvent", SymbolMappingTarget::kIndirect, 0x44, STATIC_SYM(ecl__GameStateEventManager__ExecuteGameStateChangedEvent)}
		},

		{
			"esv::GameStateEventManager::ExecuteGameStateChangedEvent",
			SymbolMappingData::kText, 0,
			"4C 8D 0D XX XX XX XX " // lea r9, aClientStateSwa ; "SERVER STATE SWAP - from: %s, to: %s\n"
			"48 89 7C 24 28 " // mov     [rsp+78h+var_50], rdi
			"4C 8D 44 24 30 ", // lea     r8, [rsp+78h+var_48]
			{SymbolMappingCondition::kString, 0, "SERVER STATE SWAP - from: %s, to: %s\n"},
			{"esv::GameStateEventManager::ExecuteGameStateChangedEvent", SymbolMappingTarget::kIndirect, 0x4D, STATIC_SYM(esv__GameStateEventManager__ExecuteGameStateChangedEvent)}
		},

		{
			"esv::GameStateThreaded::GameStateWorker::DoWork",
			SymbolMappingData::kText, 0,
			"48 8D 2D XX XX XX XX ", // lea     rbp, aEsvGamestateth ; "esv::GameStateThreaded::GameStateWorker::DoWork"
			{SymbolMappingCondition::kString, 0, "esv::GameStateThreaded::GameStateWorker::DoWork"},
			{"esv::GameStateThreaded::GameStateWorker::DoWork", SymbolMappingTarget::kAbsolute, -0x44, STATIC_SYM(esv__GameStateThreaded__GameStateWorker__DoWork)}
		},

		{
			"ecl::GameStateThreaded::GameStateWorker::DoWork",
			SymbolMappingData::kText, 0,
			"4C 8D 35 XX XX XX XX ", // lea     r14, aEclGamestateth ; "ecl::GameStateThreaded::GameStateWorker::DoWork"
			{SymbolMappingCondition::kString, 0, "ecl::GameStateThreaded::GameStateWorker::DoWork"},
			{"ecl::GameStateThreaded::GameStateWorker::DoWork", SymbolMappingTarget::kAbsolute, -0x30, STATIC_SYM(ecl__GameStateThreaded__GameStateWorker__DoWork)}
		},

		{
			"net::Host::AddProtocol",
			SymbolMappingData::kText, 0,
			"4C 8B C0 " // mov     r8, rax
			"BA 25 00 00 00 " // mov     edx, 25h ; '%'
			"48 8B CE " // mov     rcx, rsi
			"E8 XX XX XX XX " // call    net__Host__AddProtocol
			"45 33 C9 " // xor     r9d, r9d
			"C6 44 24 20 01 ", // mov     byte ptr [rsp+68h+var_48], 1
			{},
			{"net::Host::AddProtocol", SymbolMappingTarget::kIndirect, 11, STATIC_SYM(net__Host__AddProtocol)}
		},

		{
			"net::MessageFactory::RegisterMessage",
			SymbolMappingData::kText, 0,
			"48 8D 05 XX XX XX XX " // lea     rax, aEocnetLevelloa ; "eocnet::LevelLoadMessage"
			"41 B9 04 00 00 00 " // mov     r9d, 4
			"4C 8B C3 " // mov     r8, rbx
			"48 89 44 24 20 " // mov     [rsp+38h+a5], rax
			"BA FE 00 00 00 " // mov     edx, 0FEh
			"48 8B CE " // mov     rcx, rsi
			"E8 XX XX XX XX ", // call    net__MessageFactory__RegisterMessage
			{SymbolMappingCondition::kString, 0, "eocnet::LevelLoadMessage"},
			{"net::MessageFactory::RegisterMessage", SymbolMappingTarget::kIndirect, 29, STATIC_SYM(net__MessageFactory__RegisterMessage)}
		},

		{
			"net::MessageFactory::GetFreeMessage",
			SymbolMappingData::kText, 0,
			"BA ED 00 00 00 " // mov     edx, 0EDh
			"F3 0F 10 75 28 " // movss   xmm6, dword ptr [rbp+28h]
			"48 8B D8 " // mov     rbx, rax
			"8B 7D 38 " // mov     edi, [rbp+38h]
			"8B 75 48 " // mov     esi, [rbp+48h]
			"4C 8B B9 80 00 00 00 " // mov     r15, [rcx+80h]
			"8B 6D 58 " // mov     ebp, [rbp+58h]
			"49 8B 8F F0 01 00 00 " // mov     rcx, [r15+1F0h]
			"E8 XX XX XX XX ", // call    net__MessageFactory__GetFreeMessage
			{},
			{"net::MessageFactory::GetFreeMessage", SymbolMappingTarget::kIndirect, 36, STATIC_SYM(net__MessageFactory__GetFreeMessage)}
		},

		{
			"CharacterStatsGetters",
			SymbolMappingData::kText, 0,
			"45 84 E4 " // test    r12b, r12b
			"74 09 " // jz      short loc_1810554AD
			"41 8B 87 64 03 00 00 " // mov     eax, [r15+364h]
			"EB 07 " // jmp     short loc_1810554B4
			"41 8B 87 60 03 00 00 " // mov     eax, [r15+360h]
			"41 0F B6 D4 " // movzx   edx, r12b
			"89 47 44 ", // mov     [rdi+44h], eax
			{},
			{"CharacterStatsGetters", SymbolMappingTarget::kAbsolute, 0, nullptr, &FindCharacterStatGettersEoCApp}
		},

		{
			"GetHitChance",
			SymbolMappingData::kText, 0,
			"48 89 5C 24 10 " // mov     [rsp+arg_8], rbx
			"48 89 6C 24 20 " // mov     [rsp+arg_18], rbp
			"56 " // push    rsi
			"57 " // push    rdi
			"41 56 " // push    r14
			"48 83 EC 20 " // sub     rsp, 20h
			"48 8B 99 68 02 00 00 " // mov     rbx, [rcx+268h]
			"4C 8B F2 ", //  mov     r14, rdx
			{},
			{"GetHitChance", SymbolMappingTarget::kAbsolute, 0, CHAR_GETTER_SYM(GetHitChance), nullptr, &sSymbolChanceToHitBoost, 0x200}
		},

		{
			"esv::Character::Hit",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"4C 89 XX 24 18 01 00 00 " // mov     [rsp+150h+var_38], r15
			"E8 XX XX XX XX " // call    eoc__StatusPrototype__GetMappedValue
			"85 C0 " // test    eax, eax
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_DamageItems
			"48 8B CB " // mov     rcx, rbx
			"40 0F 95 C7 ", // setnz   dil
			{SymbolMappingCondition::kFixedString, 15, "DamageItems"},
			{"esv::Character::Hit", SymbolMappingTarget::kAbsolute, 0, nullptr, nullptr, &sSymbolCharacterHit, 0x300}
		},

		{
			"FindLibraries",
			SymbolMappingData::kText, SymbolMappingData::kAllowFail,
			"40 53 " // push rbx
			"48 83 EC 20 " // sub rsp, 20h
			"8B 05 XX XX XX XX " // mov eax, cs:xxx
			"48 8B D9 " // mov rbx, rcx
			"8B D0 " // mov edx, eax
			"FF C0 " // inc eax
			"89 05 XX XX XX XX " // mov cs : xxx, eax
			"85 D2 " // test edx, edx
			"75 18 " // jnz short loc_xxx
			"44 8D 42 XX " // lea r8d, [rdx+XXh]
			"48 8D 15 XX XX XX XX " // lea rdx, xxx
			"48 8D 0D XX XX XX XX ", // lea rcx, xxx
			{},
			{"FindLibraries", SymbolMappingTarget::kAbsolute, 0, nullptr, &FindLibrariesEoCPlugin}
		},

		{
			"esv::ActionMachine::SetState",
			SymbolMappingData::kText, 0,
			"C6 44 01 53 00 " // mov     byte ptr [rcx+rax+53h], 0
			"45 33 C9 " // xor     r9d, r9d
			"48 8B C8 " // mov     rcx, rax
			"C6 44 24 28 01 " // mov     [rsp+78h+var_50], 1
			"45 33 C0 " // xor     r8d, r8d
			"C6 44 24 20 00 " // mov     [rsp+78h+a5], 0
			"40 0F B6 D7 " // movzx   edx, dil
			"E8 XX XX XX XX ", // call    xxx
			{},
			{"esv::ActionMachine::SetState", SymbolMappingTarget::kIndirect, 28, nullptr, &FindActionMachineSetState}
		},

		{
			"eoc::SkillPrototype::FormatDescriptionParam",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"0F 29 BC 24 70 03 00 00 " // movaps  [rsp+3B0h+var_40], xmm7
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"89 5C 24 4C " // mov     [rsp+3B0h+a3], ebx
			"48 8D 15 XX XX XX XX ", // lea     rdx, fs_UseWeaponDamage
			{SymbolMappingCondition::kFixedString, 16, "UseWeaponDamage"},
			{"eoc::SkillPrototype::FormatDescriptionParam", SymbolMappingTarget::kAbsolute, -0x100, nullptr, nullptr, &sSymbolSkillPrototypeFormatDescriptionParam, 0x100}
		},

		{
			"eoc::SkillPrototype::GetSkillDamage",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 0F 44 FE " // cmovz   rdi, rsi
			"41 83 C8 FF " // or      r8d, 0FFFFFFFFh
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_Damage_Multiplier_2
			"49 8B CD ", // mov     rcx, r13
			{SymbolMappingCondition::kFixedString, 8, "Damage Multiplier"},
			{"eoc::SkillPrototype::GetSkillDamage", SymbolMappingTarget::kAbsolute, -0x180, nullptr, nullptr, &sSymbolSkillPrototypeGetSkillDamage, 0x180}
		},

		{
			"eoc::StatusPrototype::FormatDescriptionParam",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 3B 05 XX XX XX XX " // cmp     rax, cs:fs_Damage
			"0F 85 80 00 00 00 " // jnz     loc_xxx
			"48 8D 15 XX XX XX XX " // lea     rdx, fsx_DamageStats 
			"48 8B CF " //  mov     rcx, rdi
			"E8 XX XX XX XX ", // call    eoc__StatusPrototype__GetFixedStringValue
			{SymbolMappingCondition::kFixedString, 13, "DamageStats"},
			{"eoc::StatusPrototype::FormatDescriptionParam", SymbolMappingTarget::kAbsolute, -0x80, nullptr, nullptr, &sSymbolStatusPrototypeFormatDescriptionParam, 0x80}
		},

		{
			"esv::EoCServer",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 8D 15 XX XX XX XX " // lea     rdx, fs_CurrentCellCount
			"48 8B CE " // mov     rcx, rsi
			"FF 90 90 01 00 00 " // call    qword ptr [rax+190h]
			"4C 8B 0D XX XX XX XX ", //  mov     r9, cs:esv__gEoCServer
			{SymbolMappingCondition::kFixedString, 0, "CurrentCellCount"},
			{"esv::EoCServer", SymbolMappingTarget::kIndirect, 16, STATIC_SYM(EoCServer)}
		},

		{
			"esv::TurnManager::UpdateTurnOrder",
			SymbolMappingData::kText, 0,
			"88 54 24 10 " // mov[rsp - 8 + a2], dl
			"55 " // push    rbp
			"41 55 " // push    r13
			"48 8D 6C 24 B1 " //  lea     rbp,[rsp - 4Fh]
			"48 81 EC 88 00 00 00 " //  sub     rsp, 88h
			"3A 15 XX XX XX XX ", //  cmp     dl, cs:xxx
			{},
			{"esv::TurnManager::UpdateTurnOrder", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(esv__TurnManager__UpdateTurnOrder)}
		},

		{
			"GlobalSwitches",
			SymbolMappingData::kText, SymbolMappingData::kDeferred,
			"48 83 EC 20 " // sub     rsp, 20h
			"48 8B 05 XX XX XX XX " // mov     rax, cs:gGlobalSwitches
			"4C 8B F2 " // mov     r14, rdx
			"48 8B F1 " // mov     rsi, rcx
			"48 8D 15 XX XX XX XX ", // lea     rdx, fs_UseLevelCache
			{SymbolMappingCondition::kFixedString, 17, "UseLevelCache"},
			{"GlobalSwitches", SymbolMappingTarget::kIndirect, 4, STATIC_SYM(pGlobalSwitches)}
		},

		{
			"esv::Inventory::Equip",
			SymbolMappingData::kText, 0,
			"48 89 54 24 10 " // mov     [rsp-8+a2], rdx
			"55 56 41 54 41 56 41 57 " // push    rbp, rsi, r12, r14, r15
			"48 8D 6C 24 E9 " // lea     rbp, [rsp-17h]
			"48 81 EC B0 00 00 00 ", // sub     rsp, 0B0h
			{},
			{"esv::Inventory::Equip", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(InventoryEquip)}
		},

		{
			"ecl::EoCUI::EoCUI",
			SymbolMappingData::kText, 0,
			"FF 15 XX XX XX XX " // call    cs:??0UIObject@ls@@QEAA@AEBVPath@1@@Z
			"C7 83 50 01 00 00 00 00 00 00 " // mov     dword ptr [rbx+150h], 0
			"48 8D 05 XX XX XX XX " // lea     rax, ??_7EocUI@ecl@@6B@
			"48 89 03 " // mov     [rbx], rax
			"66 C7 83 54 01 00 00 00 00 " // mov     word ptr [rbx+154h], 0
			"48 8B 05 XX XX XX XX ", // mov     rax, cs:?Unassigned@ObjectHandle@ls@@2V12@B
			{},
			{"ecl::EoCUI::EoCUI", SymbolMappingTarget::kAbsolute, -9, STATIC_SYM(EoCUI__ctor)},
			{"ecl::EoCUI::vftable", SymbolMappingTarget::kIndirect, 16, STATIC_SYM(EoCUI__vftable)},
		},

		{
			"eoc::gSpeakerManager",
			SymbolMappingData::kText, 0,
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:eoc__gSpeakerManager
			"E8 XX XX XX XX " // call    eoc__SpeakerManager__LoadVoiceMetaData
			"49 8B 04 24 " // mov     rax, [r12]
			"4C 8D 05 XX XX XX XX ", // lea     r8, aRootTemplates ; "Root templates"
			{SymbolMappingCondition::kString, 16, "Root templates"},
			{"eoc::gSpeakerManager", SymbolMappingTarget::kIndirect, 0, STATIC_SYM(eoc__SpeakerManager)}
		},

		{
			"esv::OsirisVariableHelper::SavegameVisit",
			SymbolMappingData::kText, 0,
			"40 53 " // push    rbx
			"56 " // push    rsi
			"57 " // push    rdi
			"48 83 EC 30 " // sub     rsp, 30h
			"48 8B FA " // mov     rdi, rdx
			"48 8B D9 " // mov     rbx, rcx
			"48 8D 15 XX XX XX XX ", // lea     rdx, xxx ; "esv::OsirisVariableHelper::SavegameVisit"
			{SymbolMappingCondition::kString, 14, "esv::OsirisVariableHelper::SavegameVisit"},
			{"esv::OsirisVariableHelper::SavegameVisit", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(esv__OsirisVariableHelper__SavegameVisit)}
		},

		{
			"esv::Character::ApplyDamage",
			SymbolMappingData::kText, 0,
			"C7 45 DB 00 00 80 BF " // mov     [rbp+57h+var_7C], 0BF800000h
			"C7 45 DF 00 00 00 00 " // mov     [rbp+57h+var_78], 0
			"4D 8B 00 " // mov     r8, [r8]
			"C6 44 24 28 00 " // mov     [rsp+0B0h+var_88], 0
			"48 89 44 24 20 " // mov     [rsp+0B0h+impactDirection], rax
			"E8 XX XX XX XX ", // call    esv__Character__ApplyDamage
			{},
			{"esv::Character::ApplyDamage", SymbolMappingTarget::kIndirect, 27, STATIC_SYM(esv__Character__ApplyDamage)}
		},

		{
			"ecl::InventoryFactory",
			SymbolMappingData::kText, 0,
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:ecl__gInventoryFactory
			"E8 XX XX XX XX " // call    sub_1804DB830
			"48 8B 03 " // mov     rax, [rbx]
			"4C 8D 05 XX XX XX XX ", // lea     r8, str_Triggers ; "Triggers"
			{SymbolMappingCondition::kString, 15, "Triggers"},
			{"ecl::InventoryFactory", SymbolMappingTarget::kIndirect, 0, STATIC_SYM(EclInventoryFactory)}
		},

		{
			"ecl::LevelManager",
			SymbolMappingData::kText, 0,
			"4C 8D 05 XX XX XX XX " // lea     r8, "NetworkFixedStrings"
			"BA 01 00 00 00 " // mov     edx, 1
			"49 8B CC " // mov     rcx, r12
			"FF 90 80 00 00 00 " // call    qword ptr [rax+80h]
			"48 8B 15 XX XX XX XX " // mov     rdx, cs:ecl__gLevelManager
			"48 8B 0D XX XX XX XX ", // mov     rcx, cs:eoc__gNetworkFixedStrings
			{SymbolMappingCondition::kString, 0, "NetworkFixedStrings"},
			{"esv::LevelManager", SymbolMappingTarget::kIndirect, 21, STATIC_SYM(EclLevelManager)}
		},

		{
			"esv::ItemHelpers::GenerateTreasureItem",
			SymbolMappingData::kText, 0,
			"48 83 EC 50 " // sub     rsp, 50h
			"4C 8B 3D XX XX XX XX " // mov     r15, cs:gRPGStats
			"44 8B E2 " // mov     r12d, edx
			"48 8B F1 " // mov     rsi, rcx
			"48 8D 51 08 " // lea     rdx, [rcx+8]
			"33 ED " // xor     ebp, ebp
			"45 33 F6 " // xor     r14d, r14d
			"49 8B 87 90 00 00 00 ", // mov     rax, [r15+90h]
			{},
			{"esv::ItemHelpers::GenerateTreasureItem", SymbolMappingTarget::kAbsolute, -0x18, STATIC_SYM(esv__ItemHelpers__GenerateTreasureItem)}
		},

		{
			"esv::CombineManager::ExecuteCombination",
			SymbolMappingData::kText, 0,
			"48 8D AC 24 E8 FE FF FF " // lea     rbp, [rsp-118h]
			"48 81 EC 18 02 00 00 " // sub     rsp, 218h
			"48 8B 05 XX XX XX XX " // mov     rax, cs:__security_cookie
			"48 33 C4 " // xor     rax, rsp
			"48 89 85 00 01 00 00 " // mov     [rbp+150h+var_50], rax
			"4C 8B B5 90 01 00 00 ", // mov     r14, [rbp+150h+combinationId]
			{},
			{"esv::CombineManager::ExecuteCombination", SymbolMappingTarget::kAbsolute, -0x0D, STATIC_SYM(esv__CombineManager__ExecuteCombination)}
		},

		{
			"eoc::SurfaceTransformActionsFromType",
			SymbolMappingData::kText, 0,
			"48 63 D9 " // movsxd  rbx, ecx
			"83 FB 4F " // cmp     ebx, 4Fh
			"77 17 " // ja      short xxx
			"48 69 C3 D0 02 00 00 " // imul    rax, rbx, 2D0h
			"48 8D 0D XX XX XX XX ", // lea     rcx, eoc__SurfaceTransformActionsFromType
			{},
			{"eoc::SurfaceTransformActionsFromType", SymbolMappingTarget::kIndirect, 15, STATIC_SYM(eoc__SurfaceTransformActionsFromType)}
		},

		{
			"esv::SurfaceActionFactory",
			SymbolMappingData::kText, 0,
			"BA 04 00 00 00 " // mov     edx, 4
			"48 8B 0D XX XX XX XX " // mov     rcx, cs:esv__gSurfaceActionFactory
			"48 89 44 24 58 " // mov     [rsp+140h+surfaceMgr], rax
			"4D 8B 00 " // mov     r8, [r8]
			"E8 XX XX XX XX ", // call    esv__SurfaceActionFactory__CreateAction
			{},
			{"esv::SurfaceActionFactory", SymbolMappingTarget::kIndirect, 5, STATIC_SYM(esv__SurfaceActionFactory)},
			{"esv::SurfaceActionFactory::CreateAction", SymbolMappingTarget::kIndirect, 20, STATIC_SYM(esv__SurfaceActionFactory__CreateAction)},
		},

		{
			"eoc::SkillPrototype::GetAttackAPCost",
			SymbolMappingData::kText, 0,
			"41 57 " // push    r15
			"48 83 EC 40 " // sub     rsp, 40h
			"8B 79 28 " // mov     edi, [rcx+28h]
			"4D 8B E1 ", // mov     r12, r9
			{},
			{"eoc::SkillPrototype::GetAttackAPCost", SymbolMappingTarget::kAbsolute, -8, STATIC_SYM(SkillPrototype__GetAttackAPCost)}
		},

		{
			"eoc::AiGrid::SearchForCell",
			SymbolMappingData::kText, 0,
			"4C 8B DC " // mov     r11, rsp
			"55 " // push    rbp
			"48 83 EC 70 " // sub     rsp, 70h
			"33 C0 " // xor     eax, eax
			"F3 0F 11 54 24 4C ", // movss   dword ptr [rsp+78h+realXZ+4], xmm2
			{},
			{"eoc::AiGrid::SearchForCell", SymbolMappingTarget::kAbsolute, 0, STATIC_SYM(eoc__AiGrid__SearchForCell)}
		},
	};

	void LibraryManager::MapAllSymbols(bool deferred)
	{
		for (auto i = 0; i < std::size(sSymbolMappings); i++) {
			if ((deferred && (sSymbolMappings[i].Flag & SymbolMappingData::kDeferred))
				|| (!deferred && !(sSymbolMappings[i].Flag & SymbolMappingData::kDeferred))) {
				MapSymbol(sSymbolMappings[i], nullptr, 0);
			}
		}
	}

	bool LibraryManager::FindEoCPlugin(uint8_t const * & start, size_t & size)
	{
		HMODULE hEoCApp = GetModuleHandleW(L"EoCApp.exe");
		if (hEoCApp != NULL) {
			MessageBoxW(NULL, L"This version of the Script Extender can only be used with the editor.", L"Osiris Extender Error", MB_OK | MB_ICONERROR);
			TerminateProcess(GetCurrentProcess(), 1);
		}

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
			Fail("Could not get module info of EoCPlugin.dll");
		}

		start = (uint8_t const *)moduleInfo.lpBaseOfDll;
		size = moduleInfo.SizeOfImage;

		coreLib_ = LoadLibraryW(L"CoreLib.dll");
		if (coreLib_ == NULL) {
			return false;
		}

		if (!GetModuleInformation(GetCurrentProcess(), coreLib_, &moduleInfo, sizeof(moduleInfo))) {
			Fail("Could not get module info of CoreLib.dll");
		}

		coreLibStart_ = (uint8_t const *)moduleInfo.lpBaseOfDll;
		coreLibSize_ = moduleInfo.SizeOfImage;

		gameEngine_ = LoadLibraryW(L"GameEngine.dll");
		if (gameEngine_ == NULL) {
			return false;
		}

		return true;
	}


	void LibraryManager::FindExportsEoCPlugin()
	{
		auto& sym = GetStaticSymbols();
		auto allocProc = GetProcAddress(coreLib_, "?Malloc@GlobalAllocator@ls@@QEAAPEAX_KPEBDH1@Z");
		auto freeProc = GetProcAddress(coreLib_, "?Free@GlobalAllocator@ls@@QEAAXPEAX@Z");

		sym.EoCAlloc = (EoCAllocFunc)allocProc;
		sym.EoCFree = (EoCFreeFunc)freeProc;

		if (allocProc == nullptr || freeProc == nullptr) {
			ERR("Could not find memory management functions");
			CriticalInitFailed = true;
		}

		auto createFixedStringProc = GetProcAddress(coreLib_, "?Create@FixedString@ls@@SA?AV12@PEBD_J@Z");
		sym.CreateFixedString = (ls__FixedString__Create)createFixedStringProc;

		if (GetStaticSymbols().CreateFixedString == nullptr) {
			ERR("LibraryManager::FindExportsEoCPlugin(): Could not find ls::FixedString::Create");
			CriticalInitFailed = true;
		}

		auto getPrefixProc = GetProcAddress(coreLib_, "?GetPrefixForRoot@Path@ls@@CA?AV?$_StringView@DX@2@W4EPathRoot@2@@Z");
		auto fileReaderCtorProc = GetProcAddress(coreLib_, "??0FileReader@ls@@QEAA@AEBVPath@1@W4EType@01@@Z");
		auto fileReaderDtorProc = GetProcAddress(coreLib_, "??1FileReader@ls@@QEAA@XZ");

		sym.GetPrefixForRoot = (ls__Path__GetPrefixForRoot)getPrefixProc;
		sym.FileReaderCtor = (ls__FileReader__FileReader)fileReaderCtorProc;
		sym.FileReaderDtor = (ls__FileReader__Dtor)fileReaderDtorProc;

		if (sym.GetPrefixForRoot == nullptr
			|| sym.FileReaderCtor == nullptr
			|| sym.FileReaderDtor == nullptr) {
			ERR("LibraryManager::FindExportsEoCPlugin(): Could not find filesystem functions");
			CriticalInitFailed = true;
		}

		auto registerUIObjectCreatorProc = GetProcAddress(gameEngine_, "?RegisterUIObjectCreator@UIObjectManager@ls@@QEAAXIPEAUUIObjectFunctor@2@@Z");
		auto createUIObjectProc = GetProcAddress(gameEngine_, "?CreateUIObject@UIObjectManager@ls@@QEAA?AVObjectHandle@2@III_KF@Z");
		auto destroyUIObjectProc = GetProcAddress(gameEngine_, "?DestroyUIObject@UIObjectManager@ls@@QEAA_NAEBVObjectHandle@2@@Z");
		auto getUIObjectManagerProc = GetProcAddress(gameEngine_, "?GetInstance@?$Singleton@VUIObjectManager@ls@@@ls@@SAPEAVUIObjectManager@2@XZ");

		sym.UIObjectManager__RegisterUIObjectCreator = (UIObjectManager::RegisterUIObjectCreatorProc*)registerUIObjectCreatorProc;
		sym.UIObjectManager__CreateUIObject = (UIObjectManager::CreateUIObjectProc*)createUIObjectProc;
		sym.UIObjectManager__DestroyUIObject = (UIObjectManager::DestroyUIObjectProc*)destroyUIObjectProc;
		sym.UIObjectManager__GetInstance = (UIObjectManager::GetInstanceProc*)getUIObjectManagerProc;

		if (sym.UIObjectManager__RegisterUIObjectCreator == nullptr
			|| sym.UIObjectManager__CreateUIObject == nullptr
			|| sym.UIObjectManager__DestroyUIObject == nullptr
			|| sym.UIObjectManager__GetInstance == nullptr) {
			ERR("LibraryManager::FindExportsEoCPlugin(): Could not find UI functions");
			InitFailed = true;
		}

		auto getTranslatedStringRepoProc = GetProcAddress(coreLib_, "?GetInstance@?$Singleton@VTranslatedStringRepository@ls@@@ls@@SAPEAVTranslatedStringRepository@2@XZ");
		auto getTranslatedStringProc = GetProcAddress(coreLib_, "?GetTranslatedString@TranslatedStringRepository@ls@@QEAA?AVTranslatedString@2@AEBVRuntimeStringHandle@2@QEBD@Z");
		auto getTranslatedString2Proc = GetProcAddress(coreLib_, "?Get@TranslatedStringRepository@ls@@QEBAAEBVSTDWString@2@AEBVRuntimeStringHandle@2@AEA_KW4EGender@2@2_N@Z");

		auto getTranslatedStringKeyMgrProc = GetProcAddress(coreLib_, "?GetInstance@?$Singleton@VTranslatedStringKeyManager@ls@@@ls@@SAPEAVTranslatedStringKeyManager@2@XZ");
		auto getTranslatedStringFromKeyProc = GetProcAddress(coreLib_, "?GetTranlatedStringFromKey@TranslatedStringKeyManager@ls@@QEBA?AVTranslatedString@2@AEBVFixedString@2@_N@Z");
		auto unloadOverridesProc = GetProcAddress(coreLib_, "?UnloadOverrides@TranslatedStringRepository@ls@@QEAA?B_NXZ");

		sym.TranslatedStringRepository__GetInstance = (TranslatedStringRepository::GetInstance)getTranslatedStringRepoProc;
		sym.TranslatedStringRepository__Get = (TranslatedStringRepository::Get)getTranslatedString2Proc;
		sym.TranslatedStringRepository__UnloadOverrides = (TranslatedStringRepository::UnloadOverrides)unloadOverridesProc;

		sym.TranslatedStringKeyManager__GetInstance = (TranslatedStringKeyManager::GetInstance)getTranslatedStringKeyMgrProc;
		sym.TranslatedStringKeyManager__GetTranlatedStringFromKey = (TranslatedStringKeyManager::GetTranlatedStringFromKey)getTranslatedStringFromKeyProc;

		if (sym.TranslatedStringRepository__GetInstance == nullptr
			|| sym.TranslatedStringRepository__Get == nullptr
			|| sym.TranslatedStringKeyManager__GetInstance == nullptr
			|| sym.TranslatedStringKeyManager__GetTranlatedStringFromKey == nullptr
			|| sym.TranslatedStringRepository__UnloadOverrides == nullptr) {
			ERR("LibraryManager::FindExportsEoCPlugin(): Could not find TranslatedStringRepository functions");
			InitFailed = true;
		}

		auto collectModsProc = GetProcAddress(gameEngine_, "?CollectAvailableMods@ModManager@ls@@SAXAEAV?$ObjectSet@VModule@ls@@V?$DefaultComparator@VModule@ls@@@2@@2@@Z");
		sym.ModManager__CollectAvailableMods = (ModManager::CollectAvailableMods)collectModsProc;

		if (sym.ModManager__CollectAvailableMods == nullptr) {
			ERR("LibraryManager::FindExportsEoCPlugin(): Could not find CollectAvailableMods");
			InitFailed = true;
		}

		auto buildCheckBlockProc = GetProcAddress(gameEngine_, "?Build@ScriptCheckBlock@ls@@SAPEAVIScriptCheckObject@2@AEBVSTDString@2@PEBV?$ObjectSet@VSTDString@ls@@V?$DefaultComparator@VSTDString@ls@@@2@@2@HH@Z");
		sym.ScriptCheckBlock__Build = (ScriptCheckBlock__Build)buildCheckBlockProc;

		if (sym.ScriptCheckBlock__Build == nullptr) {
			ERR("LibraryManager::FindExportsEoCPlugin(): Could not find ScriptCheckBlock::Build");
			InitFailed = true;
		}
	}

	void LibraryManager::FindServerGlobalsEoCPlugin()
	{
		StaticSymbols::EoCLibraryInfo const * serverLib{ nullptr };

		// 1) Look for the string "esv::CustomStatDefinitionComponent"
		Pattern statComponent;
		statComponent.FromRaw("esv::CustomStatDefinitionComponent");
		uint8_t const * statComponentStr = nullptr;
		statComponent.Scan((uint8_t const *)moduleStart_, moduleSize_, [&statComponentStr](const uint8_t * match) -> std::optional<bool> {
			statComponentStr = match;
			return true;
		}, false);

		if (!statComponentStr) {
			ERR("Could not locate esv::CustomStatDefinitionComponent");
			return;
		}

		// 2) Look for xrefs to esv::CustomStatDefinitionComponent
		Pattern xref;
		xref.FromString(
			"48 8B C8 " // mov rcx, rax
			"48 8D 15 XX XX XX XX " // lea rdx, xxx
			"FF 15 XX XX XX XX " // call cs:xxx
		);

		uint8_t const * statComponentXref = nullptr;
		xref.Scan((uint8_t const *)moduleStart_, moduleSize_, [&statComponentXref, statComponentStr](const uint8_t * match) -> std::optional<bool> {
			int32_t rel = *(int32_t const *)(match + 6);
			uint8_t const * refTo = match + rel + 7 + 3;

			if (refTo == statComponentStr) {
				statComponentXref = match;
				return true;
			} else {
				return {};
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
			"E8 XX XX XX XX " // call ctor
			"48 89 05 XX XX XX XX " // mov cs:xxx, rax
		);

		uint32_t i = 0;
		p.Scan(statComponentXref, 0x1200, [this, &i](const uint8_t * match) -> std::optional<bool> {
			if (i < std::size(GetStaticSymbols().ServerGlobals)) {
				int32_t rel = *(int32_t const *)(match + 16);
				uint8_t const * refTo = match + rel + 7 + 13;
				GetStaticSymbols().ServerGlobals[i++] = (uint8_t const **)refTo;
			}

			return {};
		});

		auto & serverGlobals = GetStaticSymbols().ServerGlobals;
		GetStaticSymbols().EsvInventoryFactory = (esv::InventoryFactory **)serverGlobals[(unsigned)EsvGlobalEoCPlugin::EsvInventoryFactory];
		GetStaticSymbols().EsvSurfaceActionFactory = (esv::SurfaceActionFactory**)serverGlobals[(unsigned)EsvGlobalEoCPlugin::SurfaceActionFactory];

		if (GetStaticSymbols().EsvInventoryFactory == nullptr) {
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
			"E8 XX XX XX XX " // call alloc
			"48 85 C0 " // test rax, rax
			"74 1F " // jz short xxx
			"48 8B C8 " // mov rcx, rax
			"E8 XX XX XX XX " // call ctor
			"48 8D 4C 24 60 " // rcx, [rsp+58h+arg_0]
			"48 89 05 XX XX XX XX " // mov cs:xxx, rax
		);

		p.Scan((uint8_t const *)coreLibStart_, coreLibSize_, [this](const uint8_t * match) -> std::optional<bool> {
			int32_t rel = *(int32_t const *)(match + 35);
			auto refTo = match + rel + 7 + 32;

			GetStaticSymbols().GlobalStrings = (GlobalStringTable const **)refTo;
			return true;
		}, false);

		if (GetStaticSymbols().GlobalStrings == nullptr) {
			ERR("LibraryManager::FindGlobalStringTableCoreLib(): Could not find global string table");
			CriticalInitFailed = true;
		}
	}
}
#endif
