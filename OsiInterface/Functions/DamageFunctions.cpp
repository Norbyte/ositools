#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	struct DamageHelpers
	{
		enum CriticalRoll
		{
			CR_Roll = 0,
			CR_Critical = 1,
			CR_NotCritical = 2
		};

		enum HighGroundBonus
		{
			HG_Unknown = 0,
			HG_HighGround = 1,
			HG_EvenGround = 2,
			HG_LowGround = 3
		};

		EsvCharacter * Source{ nullptr };
		EsvCharacter * Target{ nullptr };

		HitDamageInfo Hit;
		bool CallCharacterHit{ false };
		// 0, 1, 4, 5, (hitType & 0xFFFFFFFC) spec values
		uint32_t HitType{ 0 };
		bool RollForDamage{ false };
		bool ReduceDurability{ false };
		// TODO - SkillProperties
		HighGroundBonus HighGround{ HG_Unknown };
		CriticalRoll Critical{ CR_Roll };

		// TODO - WeaponHandle, PropertyList, PropertyContext
		uint32_t HitReason{ 6 };
		FixedString SkillId;
		Vector3 ImpactPosition;
		bool HasImpactPosition{ false };
		Vector3 ImpactOrigin;
		bool HasImpactOrigin{ false };
		Vector3 ImpactDirection;
		bool HasImpactDirection{ false };

		uint32_t DamageSourceType{ 0 };
		float Strength{ 0.0f };

		Array<TDamagePair> DamageList;

		DamageHelpers()
		{
			DamageList.Capacity = 7;
			DamageList.Buf = new TDamagePair[7];
		}

		~DamageHelpers()
		{
			delete[] DamageList.Buf;
		}


		void SetInt(char const * prop, int32_t value)
		{
			if (strcmp(prop, "CallCharacterHit") == 0) {
				CallCharacterHit = value > 0;
			}
			else if (strcmp(prop, "HitType") == 0) {
				HitType = (uint32_t)value;
			}
			else if (strcmp(prop, "RollForDamage") == 0) {
				RollForDamage = value > 0;
			}
			else if (strcmp(prop, "ReduceDurability") == 0) {
				ReduceDurability = value > 0;
			}
			else if (strcmp(prop, "HighGround") == 0) {
				if (value < 0 || value > HG_HighGround) {
					OsiError("DamageHelpers::SetInt(): Value '" << value << "' is not a value HighGround flag");
				} else {
					HighGround = (HighGroundBonus)value;
				}
			}
			else if (strcmp(prop, "CriticalRoll") == 0) {
				if (value < 0 || value > CR_NotCritical) {
					OsiError("DamageHelpers::SetInt(): Value '" << value << "' is not a value CriticalRoll flag");
				} else {
					Critical = (CriticalRoll)value;
				}
			}
			else if (strcmp(prop, "Equipment") == 0) {
				Hit.Equipment = (uint32_t)value;
			}
			else if (strcmp(prop, "DeathType") == 0) {
				Hit.DeathType = (uint32_t)value;
			}
			else if (strcmp(prop, "DamageType") == 0) {
				Hit.DamageType = (uint32_t)value;
			}
			else if (strcmp(prop, "AttackDirection") == 0) {
				Hit.AttackDirection = (uint32_t)value;
			}
			else if (strcmp(prop, "ArmorAbsorption") == 0) {
				Hit.ArmorAbsorption = (uint32_t)value;
			}
			else if (strcmp(prop, "LifeSteal") == 0) {
				Hit.LifeSteal = (uint32_t)value;
			}
			else if (strcmp(prop, "EffectFlags") == 0) {
				Hit.EffectFlags = (uint32_t)value;
			}
			else if (strcmp(prop, "HitWithWeapon") == 0) {
				Hit.HitWithWeapon = value > 0;
			}
			else if (strcmp(prop, "HitReason") == 0) {
				HitReason = (uint32_t)value;
			}
			else if (strcmp(prop, "DamageSourceType") == 0) {
				DamageSourceType = (uint32_t)value;
			}
			else if (strcmp(prop, "Strength") == 0) {
				Strength = value / 100.0f;
			}
			else {
				OsiError("DamageHelpers::SetInt(): Unknown int property: '" << prop << "'");
			}
		}

		void SetVector(char const * prop, Vector3 const & value)
		{
			if (strcmp(prop, "ImpactPosition") == 0) {
				ImpactPosition = value;
				HasImpactPosition = true;
			}
			else if (strcmp(prop, "ImpactOrigin") == 0) {
				ImpactOrigin = value;
				HasImpactOrigin = true;
			}
			else if (strcmp(prop, "ImpactDirection") == 0) {
				ImpactDirection = value;
				HasImpactDirection = true;
			}
			else {
				OsiError("DamageHelpers::SetVector(): Unknown vector3 property '" << prop << "'");
			}
		}

		void SetFlag(char const * flag)
		{
			if (strcmp(flag, "Hit") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_Hit;
			}
			else if (strcmp(flag, "Blocked") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_Blocked;
			}
			else if (strcmp(flag, "Dodged") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_Dodged;
			}
			else if (strcmp(flag, "Missed") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_Missed;
			}
			else if (strcmp(flag, "CriticalHit") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_CriticalHit;
			}
			else if (strcmp(flag, "AlwaysBackstab") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_AlwaysBackstab;
			}
			else if (strcmp(flag, "FromSetHP") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_FromSetHP;
			}
			else if (strcmp(flag, "DontCreateBloodSurface") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_DontCreateBloodSurface;
			}
			else if (strcmp(flag, "Reflection") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_Reflection;
			}
			else if (strcmp(flag, "NoDamageOnOwner") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_NoDamageOnOwner;
			}
			else if (strcmp(flag, "FromShacklesOfPain") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_FromShacklesOfPain;
			}
			else if (strcmp(flag, "DamagedMagicArmor") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_DamagedMagicArmor;
			}
			else if (strcmp(flag, "DamagedPhysicalArmor") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_DamagedPhysicalArmor;
			}
			else if (strcmp(flag, "DamagedVitality") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_DamagedVitality;
			}
			else if (strcmp(flag, "PropagatedFromOwner") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_PropagatedFromOwner;
			}
			else if (strcmp(flag, "ProcWindWalker") == 0) {
				Hit.EffectFlags |= HitDamageInfo::EF_ProcWindWalker;
			}
			else {
				OsiError("DamageHelpers::SetFlag(): Unknown flag: '" << flag << "'");
			}
		}

		void SetString(char const * prop, char const * value)
		{
			auto fs = ToFixedString(value);
			if (!fs) {
				OsiError("DamageHelpers::SetString(): Could not map value '" << value << "' to FixedString");
				return;
			}

			if (strcmp(prop, "SkillId") == 0) {
				SkillId = fs;
			}
			else {
				OsiError("DamageHelpers::SetString(): Unknown string property '" << prop << "'");
			}
		}

		void AddDamage(uint32_t DamageType, uint32_t Amount)
		{
			if (DamageList.Size == DamageList.Capacity) {
				OsiError("DamageHelpers::AddDamage(): Exceeded damage list capacity");
				return;
			}

			auto & dmg = DamageList.Buf[DamageList.Size++];
			dmg.DamageType = DamageType;
			dmg.Amount = Amount;
		}

		bool Execute()
		{
			if (!Target) {
				OsiError("DamageHelpers::Execute(): No target!");
				return false;
			}

			if (DamageList.Size == 0) {
				OsiError("DamageHelpers::Execute(): At least one damage type should be added!");
				return false;
			}

			auto characterHit = gOsirisProxy->GetLibraryManager().CharacterHit;
			if (CallCharacterHit && characterHit == nullptr) {
				OsiError("DamageHelpers::Execute(): esv::Character::Hit not found!");
				return false;
			}

			auto statusMachine = Target->StatusManager;
			if (!statusMachine) {
				OsiError("DamageHelpers::Execute(): Target has no StatusMachine!");
				return false;
			}

			auto fsHit = ToFixedString("HIT");
			auto hit = (EsvStatusHit *)gOsirisProxy->GetLibraryManager().StatusMachineCreateStatus(statusMachine, fsHit, 0);

			hit->StartTimer = 0.0f;
			hit->HitByHandle = ObjectHandle{};
			hit->HitWithHandle = ObjectHandle{}; // FIXME?
			hit->WeaponHandle = ObjectHandle{}; // FIXME?
			hit->HitReason = HitReason;
			hit->Flags0 &= 0xFB; // Clear IsFromItem

			if (SkillId) {
				hit->SkillId = SkillId;
			} else {
				hit->SkillId = ToFixedString("");
			}

			hit->Strength = Strength;
			hit->DamageSourceType = DamageSourceType;
			hit->StatusSourceHandle = ObjectHandle{};

			if (Source != nullptr) {
				ObjectHandle sourceHandle;
				Source->GetObjectHandle(&sourceHandle);
				hit->HitByHandle = sourceHandle;
				hit->StatusSourceHandle = sourceHandle;
			}

			auto & damage = hit->DamageInfo;
			damage.Equipment = Hit.Equipment;
			damage.TotalDamageDone = Hit.TotalDamageDone;
			damage.Unknown = Hit.Unknown;
			damage.DeathType = Hit.DeathType;
			damage.DamageType = Hit.DamageType;
			damage.AttackDirection = Hit.AttackDirection;
			damage.ArmorAbsorption = Hit.ArmorAbsorption;
			damage.LifeSteal = Hit.LifeSteal;
			damage.EffectFlags = Hit.EffectFlags;
			damage.HitWithWeapon = Hit.HitWithWeapon;

			if (CallCharacterHit) {
				characterHit(Target, Source->Stats, nullptr, &DamageList, HitType, RollForDamage ? 1 : 0, 
					&damage, 0, nullptr, HighGround, ReduceDurability ? 1 : 0, Critical);
			} else {
				damage.DamageList.Size = DamageList.Size;
				for (auto i = 0; i < DamageList.Size; i++) {
					damage.DamageList.Buf[i] = DamageList.Buf[i];
					damage.TotalDamageDone += DamageList.Buf[i].Amount;
				}
			}

			if (HasImpactPosition) {
				hit->ImpactPosition = ImpactPosition;
			} else {
				hit->ImpactPosition = *Target->GetTranslate();
			}

			if (HasImpactOrigin) {
				hit->ImpactOrigin = ImpactOrigin;
			} else if (Source != nullptr) {
				hit->ImpactOrigin = *Source->GetTranslate();
			} else {
				hit->ImpactOrigin = *Target->GetTranslate();
			}

			if (HasImpactDirection) {
				hit->ImpactDirection = ImpactDirection;
			} else {
				auto dir = (hit->ImpactPosition - hit->ImpactOrigin);
				if (glm::length(dir) > 0.0001f) {
					dir = glm::normalize(dir);
				}

				hit->ImpactDirection = dir;
			}

			gOsirisProxy->GetLibraryManager().StatusMachineApplyStatus(statusMachine, hit);
			return true;
		}
	};

	namespace func
	{
		std::unique_ptr<DamageHelpers> DamageHelper;

		void HitPrepare(OsiArgumentDesc const & args)
		{
			if (DamageHelper) {
				OsiWarn("HitPrepare(): Destroying active DamageHelper?");
			}

			auto targetGuid = args.Get(0).String;
			auto target = FindCharacterByNameGuid(targetGuid);
			if (target == nullptr) {
				OsiError("HitPrepare(): Target character '" << targetGuid << "' doesn't exist!");
				return;
			}

			DamageHelper = std::make_unique<DamageHelpers>();
			DamageHelper->Target = target;
			DamageHelper->Source = FindCharacterByNameGuid(args.Get(1).String);
		}

		void HitExecute(OsiArgumentDesc const & args)
		{
			if (DamageHelper != nullptr)
			{
				DamageHelper->Execute();
				DamageHelper.reset();
			}
			else
			{
				OsiError("HitExecute(): No damage prepared!");
			}
		}

		void HitSetInt(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).Int32;

			if (!DamageHelper) {
				OsiError("HitSetInt(): Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetInt(prop, value);
		}

		void HitSetString(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).String;

			if (!DamageHelper) {
				OsiError("HitSetString(): Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetString(prop, value);
		}

		void HitSetVector3(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			Vector3 vec;
			vec.x = args.Get(1).Float;
			vec.y = args.Get(2).Float;
			vec.z = args.Get(3).Float;

			if (!DamageHelper) {
				OsiError("HitSetVector3(): Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetVector(prop, vec);
		}

		void HitSetFlag(OsiArgumentDesc const & args)
		{
			auto flag = args.Get(0).String;

			if (!DamageHelper) {
				OsiError("HitSetFlag(): Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetFlag(flag);
		}

		void HitAddDamage(OsiArgumentDesc const & args)
		{
			auto damageType = (uint32_t)args.Get(0).Int32;
			auto amount = (uint32_t)args.Get(1).Int32;

			if (!DamageHelper) {
				OsiError("HitAddDamage(): Called when not preparing a hit!");
				return;
			}

			DamageHelper->AddDamage(damageType, amount);
		}
	}

	void CustomFunctionLibrary::RegisterHitFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto hitPrepare = std::make_unique<CustomCall>(
			"NRD_HitPrepare",
			std::vector<CustomFunctionParam>{
				{ "Target", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Source", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::HitPrepare
		);
		functionMgr.Register(std::move(hitPrepare));

		auto hitExecute = std::make_unique<CustomCall>(
			"NRD_HitExecute",
			std::vector<CustomFunctionParam>{},
			&func::HitExecute
		);
		functionMgr.Register(std::move(hitExecute));

		auto hitSetInt = std::make_unique<CustomCall>(
			"NRD_HitSetInt",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitSetInt
		);
		functionMgr.Register(std::move(hitSetInt));

		auto hitSetString = std::make_unique<CustomCall>(
			"NRD_HitSetString",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitSetString
		);
		functionMgr.Register(std::move(hitSetString));

		auto hitSetVector3 = std::make_unique<CustomCall>(
			"NRD_HitSetVector3",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In }
			},
			&func::HitSetVector3
		);
		functionMgr.Register(std::move(hitSetVector3));

		auto hitSetFlag = std::make_unique<CustomCall>(
			"NRD_HitSetFlag",
			std::vector<CustomFunctionParam>{
				{ "Flag", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitSetFlag
		);
		functionMgr.Register(std::move(hitSetFlag));

		auto hitAddDamage = std::make_unique<CustomCall>(
			"NRD_HitAddDamage",
			std::vector<CustomFunctionParam>{
				{ "DamageType", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitAddDamage
		);
		functionMgr.Register(std::move(hitAddDamage));
	}

}
