#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	struct DamageHelpers
	{
		EsvCharacter * Source{ nullptr };
		EsvCharacter * Target{ nullptr };

		HitDamageInfo Hit;
		bool CallCharacterHit{ false };
		// 0, 1, 4, 5, (hitType & 0xFFFFFFFC) spec values
		uint32_t HitType{ 0 };
		bool RollForDamage{ false };
		bool ProcWindWalker{ false };
		// TODO - SkillProperties
		HighGroundBonus HighGround{ HighGroundBonus::Unknown };
		CriticalRoll Critical{ CriticalRoll::Roll };

		// TODO - WeaponHandle, PropertyList, PropertyContext
		uint32_t HitReason{ 6 };
		FixedString SkillId;
		Vector3 ImpactPosition;
		bool HasImpactPosition{ false };
		Vector3 ImpactOrigin;
		bool HasImpactOrigin{ false };
		Vector3 ImpactDirection;
		bool HasImpactDirection{ false };

		CauseType DamageSourceType{ CauseType::None };
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
			else if (strcmp(prop, "ProcWindWalker") == 0) {
				ProcWindWalker = value > 0;
			}
			else if (strcmp(prop, "HighGround") == 0) {
				if (value < 0 || value > (int32_t)HighGroundBonus::HighGround) {
					OsiError("Value '" << value << "' is not a value HighGround flag");
				} else {
					HighGround = (HighGroundBonus)value;
				}
			}
			else if (strcmp(prop, "CriticalRoll") == 0) {
				if (value < 0 || value > (int32_t)CriticalRoll::NotCritical) {
					OsiError(" Value '" << value << "' is not a value CriticalRoll flag");
				} else {
					Critical = (CriticalRoll)value;
				}
			}
			else if (strcmp(prop, "HitReason") == 0) {
				HitReason = (uint32_t)value;
			}
			else if (strcmp(prop, "DamageSourceType") == 0) {
				// FIXME - filter 
				DamageSourceType = (CauseType)value;
			}
			else if (strcmp(prop, "Strength") == 0) {
				Strength = value / 100.0f;
			}
			else {
				Hit.SetInt(prop, value);
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
				OsiError("Unknown vector3 property '" << prop << "'");
			}
		}

		void SetFlag(char const * flag)
		{
			Hit.SetFlag(flag);
		}

		void SetString(char const * prop, char const * value)
		{
			auto fs = ToFixedString(value);
			if (!fs) {
				OsiError("Could not map value '" << value << "' to FixedString");
				return;
			}

			if (strcmp(prop, "SkillId") == 0) {
				SkillId = fs;
			}
			else {
				Hit.SetString(prop, value);
			}
		}

		void AddDamage(DamageType DamageType, uint32_t Amount)
		{
			TDamagePair dmg;
			dmg.DamageType = DamageType;
			dmg.Amount = Amount;
			if (!DamageList.SafeAdd(dmg)) {
				OsiError("Exceeded damage list capacity");
			}
		}

		bool Execute()
		{
			if (!Target) {
				OsiError("No target!");
				return false;
			}

			if (DamageList.Size == 0) {
				OsiError("At least one damage type should be added!");
				return false;
			}

			auto characterHit = gOsirisProxy->GetLibraryManager().CharacterHit;
			if (CallCharacterHit && characterHit == nullptr) {
				OsiError("esv::Character::Hit not found!");
				return false;
			}

			auto statusMachine = Target->StatusManager;
			if (!statusMachine) {
				OsiError("Target has no StatusMachine!");
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
					&damage, 0, nullptr, HighGround, ProcWindWalker, Critical);
			} else {
				damage.DamageList.Size = DamageList.Size;
				for (uint32_t i = 0; i < DamageList.Size; i++) {
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
				OsiWarn("Destroying active DamageHelper?");
			}

			auto targetGuid = args.Get(0).String;
			auto target = FindCharacterByNameGuid(targetGuid);
			if (target == nullptr) {
				OsiError("Target character '" << targetGuid << "' doesn't exist!");
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
				OsiError("No damage prepared!");
			}
		}

		void HitSetInt(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).Int32;

			if (!DamageHelper) {
				OsiError("Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetInt(prop, value);
		}

		void HitSetString(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).String;

			if (!DamageHelper) {
				OsiError("Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetString(prop, value);
		}

		void HitSetVector3(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			Vector3 vec = args.GetVector(1);

			if (!DamageHelper) {
				OsiError("Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetVector(prop, vec);
		}

		void HitSetFlag(OsiArgumentDesc const & args)
		{
			auto flag = args.Get(0).String;

			if (!DamageHelper) {
				OsiError("Called when not preparing a hit!");
				return;
			}

			DamageHelper->SetFlag(flag);
		}

		void HitAddDamage(OsiArgumentDesc const & args)
		{
			auto damageType = (uint32_t)args.Get(0).Int32;
			auto amount = (uint32_t)args.Get(1).Int32;

			if (!DamageHelper) {
				OsiError("Called when not preparing a hit!");
				return;
			}

			DamageHelper->AddDamage((DamageType)damageType, amount);
		}



		EsvStatusHit * HitStatusGet(OsiArgumentDesc const & args)
		{
			auto characterGuid = args.Get(0).String;
			auto statusHandle = args.Get(1).Int64;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				return nullptr;
			}

			auto status = character->GetStatusByHandle(ObjectHandle{ statusHandle });
			if (status == nullptr) {
				OsiError("No status found with this handle: " << statusHandle);
				return nullptr;
			}

			if (status->GetStatusId() != StatusType::Hit) {
				OsiError("Status with handle " << statusHandle << " is not a HIT!");
				return nullptr;
			}

			return static_cast<EsvStatusHit *>(status);
		}

		bool HitStatusGetInt(OsiArgumentDesc & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return false;

			auto prop = args.Get(2).String;
			auto value = status->DamageInfo.GetInt(prop);
			if (value) {
				args.Get(3).Int32 = *value;
				return true;
			} else {
				return false;
			}
		}

		void HitStatusSetInt(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto prop = args.Get(2).String;
			auto value = args.Get(3).Int32;
			status->DamageInfo.SetInt(prop, value);
		}

		bool HitStatusGetString(OsiArgumentDesc & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return false;

			auto prop = args.Get(2).String;
			auto value = status->DamageInfo.GetString(prop);
			if (value) {
				args.Get(3).String = *value;
				return true;
			} else {
				return false;
			}
		}

		void HitStatusSetString(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto prop = args.Get(2).String;
			auto value = args.Get(3).String;
			status->DamageInfo.SetString(prop, value);
		}

		bool HitStatusGetFlag(OsiArgumentDesc & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return false;

			auto prop = args.Get(2).String;
			auto flag = status->DamageInfo.HasFlag(prop);
			if (flag) {
				args.Get(3).Int32 = (*flag) ? 1 : 0;
				return true;
			} else {
				return false;
			}
		}

		void HitStatusSetFlag(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto prop = args.Get(2).String;
			auto value = args.Get(3).Int32;
			if (value) {
				status->DamageInfo.SetFlag(prop);
			} else {
				status->DamageInfo.ClearFlag(prop);
			}
		}

		void HitStatusClearAllDamage(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			status->DamageInfo.DamageList.Clear();
		}

		void HitStatusClearDamage(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto damageTypeStr = args.Get(2).String;
			auto damageType = EnumInfo<DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			auto & dmgList = status->DamageInfo.DamageList;
			for (uint32_t i = 0; i < dmgList.Size; i++) {
				if (dmgList.Buf[i].DamageType == *damageType) {
					dmgList.Remove(i);
					break;
				}
			}
		}

		bool HitStatusGetDamage(OsiArgumentDesc & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return false;

			auto damageTypeStr = args.Get(2).String;
			auto damageType = EnumInfo<DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return false;
			}

			auto & dmgList = status->DamageInfo.DamageList;
			uint32_t amount = 0;
			for (uint32_t i = 0; i < dmgList.Size; i++) {
				if (dmgList.Buf[i].DamageType == *damageType) {
					amount += dmgList.Buf[i].Amount;
				}
			}

			args.Get(3).Int32 = (int32_t)amount;
			return true;
		}

		void HitStatusAddDamage(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto damageTypeStr = args.Get(2).String;
			auto amount = args.Get(3).Int32;

			auto damageType = EnumInfo<DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			TDamagePair dmg;
			dmg.DamageType = *damageType;
			dmg.Amount = (uint32_t)amount;
			if (!status->DamageInfo.DamageList.SafeAdd(dmg)) {
				OsiError("DamageList capacity exceeded!");
			}
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



		auto hitStatusGetInt = std::make_unique<CustomQuery>(
			"NRD_HitGetInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::HitStatusGetInt
		);
		functionMgr.Register(std::move(hitStatusGetInt));

		auto hitStatusSetInt = std::make_unique<CustomCall>(
			"NRD_HitSetInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitStatusSetInt
		);
		functionMgr.Register(std::move(hitStatusSetInt));

		auto hitStatusGetString = std::make_unique<CustomQuery>(
			"NRD_HitGetString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::HitStatusGetString
		);
		functionMgr.Register(std::move(hitStatusGetString));

		auto hitStatusSetString = std::make_unique<CustomCall>(
			"NRD_HitSetString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitStatusSetString
		);
		functionMgr.Register(std::move(hitStatusSetString));

		auto hitStatusGetFlag = std::make_unique<CustomQuery>(
			"NRD_HitGetFlag",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Flag", ValueType::String, FunctionArgumentDirection::In },
				{ "IsSet", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::HitStatusGetFlag
		);
		functionMgr.Register(std::move(hitStatusGetFlag));

		auto hitStatusSetFlag = std::make_unique<CustomCall>(
			"NRD_HitSetFlag",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Flag", ValueType::String, FunctionArgumentDirection::In },
				{ "IsSet", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitStatusSetFlag
		);
		functionMgr.Register(std::move(hitStatusSetFlag));

		auto hitStatusClearAllDamage = std::make_unique<CustomCall>(
			"NRD_HitClearAllDamage",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In }
			},
			&func::HitStatusClearAllDamage
		);
		functionMgr.Register(std::move(hitStatusClearAllDamage));

		auto hitStatusGetDamage = std::make_unique<CustomQuery>(
			"NRD_HitGetDamage",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::HitStatusGetDamage
		);
		functionMgr.Register(std::move(hitStatusGetDamage));

		auto hitStatusClearDamage = std::make_unique<CustomCall>(
			"NRD_HitClearDamage",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitStatusClearDamage
		);
		functionMgr.Register(std::move(hitStatusClearDamage));

		auto hitStatusAddDamage = std::make_unique<CustomCall>(
			"NRD_HitAddDamage",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitStatusAddDamage
		);
		functionMgr.Register(std::move(hitStatusAddDamage));

	}

}
