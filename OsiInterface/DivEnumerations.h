#pragma once

#include <cstdint>

namespace osidbg
{
	template <class T, class L>
	struct EnumInfoBase
	{
		struct Label
		{
			T Val;
			char const * Name;
		};

		static std::optional<T> Find(char const * name)
		{
			for (auto i = 0; i < std::size(L::Values); i++) {
				if (strcmp(name, L::Values[i].Name) == 0) {
					return L::Values[i].Val;
				}
			}

			return {};
		}

		static std::optional<char const *> Find(T val)
		{
			for (auto i = 0; i < std::size(L::Values); i++) {
				if (val == L::Values[i].Val) {
					return L::Values[i].Name;
				}
			}

			return {};
		}
	};

	template<typename T> struct EnumInfoFakeDep : public std::false_type {};

	template <class T>
	struct EnumInfo
	{
		static_assert(EnumInfoFakeDep<T>::value, "EnumInfo not implemented for this type!");
	};

	#define MAKE_ENUM_INFO(typeName, size) \
		template <> struct EnumInfo<typeName> : public EnumInfoBase<typeName, EnumInfo<typeName>> { \
			static Label const Values[size]; \
		}

	enum class CriticalRoll
	{
		Roll = 0,
		Critical = 1,
		NotCritical = 2
	};

	MAKE_ENUM_INFO(CriticalRoll, 3);

	enum class HighGroundBonus
	{
		Unknown = 0,
		HighGround = 1,
		EvenGround = 2,
		LowGround = 3
	};

	MAKE_ENUM_INFO(HighGroundBonus, 4);

	enum class EquipmentStatsType : uint32_t
	{
		Weapon = 0,
		Armor = 1,
		Shield = 2
	};

	MAKE_ENUM_INFO(EquipmentStatsType, 3);


	enum StatusFlags0 : uint8_t
	{
		SF0_KeepAlive = 1,
		SF0_IsOnSourceSurface = 2,
		SF0_IsFromItem = 4,
		SF0_Channeled = 8,
		SF0_IsLifeTimeSet = 0x10,
		SF0_InitiateCombat = 0x20,
		SF0_Influence = 0x80
	};

	MAKE_ENUM_INFO(StatusFlags0, 7);

	enum StatusFlags1 : uint8_t
	{
		SF1_BringIntoCombat = 1,
		SF1_IsHostileAct = 2,
		SF1_IsInvulnerable = 8,
		SF1_IsResistingDeath = 0x10
	};

	MAKE_ENUM_INFO(StatusFlags1, 4);

	enum StatusFlags2 : uint8_t
	{
		SF2_ForceStatus = 1,
		SF2_ForceFailStatus = 2,
		SF2_RequestDelete = 0x20,
		SF2_RequestDeleteAtTurnEnd = 0x40,
		SF2_Started = 0x80
	};

	MAKE_ENUM_INFO(StatusFlags2, 5);

	enum class StatusType : uint32_t
	{
		Hit = 1,
		Dying = 2,
		Heal = 3,
		Muted = 4,
		Charmed = 5,
		KnockedDown = 6,
		Summoning = 7,
		Healing = 8,
		Thrown = 9,
		TeleportFall = 0x0B,
		Consume = 0x0C,
		Combat = 0x0D,
		AoO = 0x0E,
		StoryFrozen = 0x0F,
		Sneaking = 0x10,
		Unlock = 0x11,
		Fear = 0x12,
		Boost = 0x13,
		Unsheathed = 0x14,
		Stance = 0x15,
		Sitting = 0x16,
		Lying = 0x17,
		Blind = 0x18,
		Smelly = 0x19,
		Clean = 0x1A,
		InfectiousDiseased = 0x1B,
		Invisible = 0x1C,
		Rotate = 0x1D,
		Encumbered = 0x1E,
		Identify = 0x1F,
		Repair = 0x20,
		Material = 0x21,
		Leadership = 0x22,
		Explode = 0x23,
		Adrenaline = 0x24,
		ShacklesOfPain = 0x25,
		ShacklesOfPainCaster = 0x26,
		WindWalker = 0x27,
		DarkAvenger = 0x28,
		Remorse = 0x29,
		DecayingTou0xc = 0x2A,
		Unhealable = 0x2B,
		Flanked = 0x2C,
		Channeling = 0x2D,
		Drain = 0x2E,
		LingeringWounds = 0x2F,
		Infused = 0x30,
		SpiritVision = 0x31,
		Spirit = 0x32,
		Damage = 0x33,
		ForceMove = 0x34,
		Climbing = 0x35,
		Incapacitated = 0x36,
		InSurface = 0x37,
		SourceMuted = 0x38,
		Overpowered = 0x39,
		Combustion = 0x3A,
		Polymorphed = 0x3B,
		DamageOnMove = 0x3C,
		DemonicBargain = 0x3D,
		GuardianAngel = 0x3E,
		Floating = 0x40,
		Challenge = 0x41,
		Disarmed = 0x42,
		HealSharing = 0x43,
		HealSharingCaster = 0x44,
		ExtraTurn = 0x45,
		ActiveDefense = 0x46,
		Spark = 0x47,
		PlayDead = 0x48,
		Constrained = 0x49,
		Effect = 0x4A,
		Deactivated = 0x4B,
		TutorialBed = 0x4C,
	};

	enum class CauseType : uint8_t
	{
		None = 0,
		SurfaceMove = 1,
		SurfaceCreate = 2,
		SurfaceStatus = 3,
		StatusEnter = 4,
		StatusTick = 5,
		Attack = 6,
		Offhand = 7,
		GM = 8
	};

	MAKE_ENUM_INFO(CauseType, 9);

	enum class DeathType : uint8_t
	{
		None = 0,
		Physical = 1,
		Piercing = 2,
		Arrow = 3,
		DoT = 4,
		Incinerate = 5,
		Acid = 6,
		Electrocution = 7,
		FrozenShatter = 8,
		PetrifiedShatter = 9,
		Explode = 10,
		Surrender = 11,
		Hang = 12,
		KnockedDown = 13,
		Lifetime = 14,
		Sulfur = 15,
		Sentinel = 16
	};

	MAKE_ENUM_INFO(DeathType, 17);

	enum class DamageType : uint32_t
	{
		None = 0,
		Physical = 1,
		Piercing = 2,
		Corrosive = 3,
		Magic = 4,
		Chaos = 5,
		Fire = 6,
		Air = 7,
		Water = 8,
		Earth = 9,
		Poison = 10,
		Shadow = 11,
		Custom = 12
	};

	MAKE_ENUM_INFO(DamageType, 13);

	enum class ItemSlot : uint8_t
	{
		Helmet = 0,
		Breast = 1,
		Leggings = 2,
		Weapon = 3,
		Shield = 4,
		Ring = 5,
		Belt = 6,
		Boots = 7,
		Gloves = 8,
		Amulet = 9,
		Ring2 = 10,
		Wings = 11,
		Horns = 12,
		Overhead = 13,
		Sentinel = 14
	};

	MAKE_ENUM_INFO(ItemSlot, 15);

	enum HitFlag : uint32_t
	{
		HF_Hit = 1,
		HF_Blocked = 2,
		HF_Dodged = 4,
		HF_Missed = 8,
		HF_CriticalHit = 0x10,
		HF_AlwaysBackstab = 0x20,
		HF_FromSetHP = 0x40,
		HF_DontCreateBloodSurface = 0x80,
		HF_Reflection = 0x200,
		HF_NoDamageOnOwner = 0x400,
		HF_FromShacklesOfPain = 0x800,
		HF_DamagedMagicArmor = 0x1000,
		HF_DamagedPhysicalArmor = 0x2000,
		HF_DamagedVitality = 0x4000,
		HF_PropagatedFromOwner = 0x10000,
		HF_HitType4 = 0x20000,
		HF_HitType5 = 0x40000,
		HF_ProcWindWalker = 0x80000,
		// Custom flags
		HF_NoEvents = 0x80000000
	};

	MAKE_ENUM_INFO(HitFlag, 16);

	enum EsvCharacterFlags : uint64_t
	{
		CF_HostControl = 0x08,
		CF_OffStage = 0x20,
		CF_CannotBePossessed = 0x80
	};

	enum EsvCharacterFlags2 : uint8_t
	{
		CF_Global = 0x01,
		CF_HasOsirisDialog = 0x02,
		CF_HasDefaultDialog = 0x04,
		CF_TreasureGeneratedForTrader = 0x10,
		CF_Resurrected = 0x20
	};

	enum EsvCharacterFlags3 : uint8_t
	{
		CF_IsPet = 0x01,
		CF_IsSpectating = 0x02,
		CF_NoReptuationEffects = 0x04,
		CF_HasWalkSpeedOverride = 0x08,
		CF_HasRunSpeedOverride = 0x10,
		CF_IsGameMaster = 0x20,
		CF_IsPossessed = 0x40
	};

	enum class GameActionType
	{
		RainAction = 1,
		StormAction = 2,
		WallAction = 4,
		TornadoAction = 6,
		PathAction = 7,
		GameObjectMoveAction = 8,
		StatusDomeAction = 9
	};
}
