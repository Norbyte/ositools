#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	void DamageHelperPool::Clear()
	{
		helpers_.clear();
	}

	DamageHelpers * DamageHelperPool::Create()
	{
		auto id = nextHelperId_++;
		auto helper = std::make_unique<DamageHelpers>();
		helper->Handle = id;
		auto ptr = helper.get();
		helpers_.insert(std::make_pair(id, std::move(helper)));
		return ptr;
	}

	bool DamageHelperPool::Destroy(int64_t handle)
	{
		auto it = helpers_.find(handle);
		if (it == helpers_.end()) {
			return false;
		} else {
			helpers_.erase(it);
			return true;
		}
	}

	DamageHelpers * DamageHelperPool::Get(int64_t handle) const
	{
		auto it = helpers_.find(handle);
		if (it == helpers_.end()) {
			return nullptr;
		} else {
			return it->second.get();
		}
	}


	DamageHelpers::DamageHelpers()
	{
	}

	DamageHelpers::~DamageHelpers()
	{
		if (MyDamageList) {
			delete[] MyDamageList->Buf;
		}
	}


	void DamageHelpers::SetInternalDamageInfo()
	{
		MyDamageInfo = std::make_unique<HitDamageInfo>();
		MyDamageList = std::make_unique<DamagePairList>();
		MyDamageList->Capacity = 7;
		MyDamageList->Buf = new TDamagePair[7];

		Hit = MyDamageInfo.get();
		DamageList = MyDamageList.get();
	}

	void DamageHelpers::SetExternalDamageInfo(HitDamageInfo * damageInfo, DamagePairList * damageList)
	{
		MyDamageInfo.reset();
		MyDamageList.reset();
		Hit = damageInfo;
		DamageList = damageList;
	}

	void DamageHelpers::SetInt(char const * prop, int32_t value)
	{
		if (strcmp(prop, "CallCharacterHit") == 0) {
			if (Type == DamageHelpers::HT_CustomHit) {
				CallCharacterHit = value > 0;
			} else {
				OsiError("Property 'CallCharacterHit' can only be set for custom hits");
			}
		}
		else if (strcmp(prop, "HitType") == 0) {
			HitType = (uint32_t)value;
		}
		else if (strcmp(prop, "RollForDamage") == 0) {
			RollForDamage = value > 0;
		}
		else if (strcmp(prop, "ProcWindWalker") == 0) {
			ProcWindWalker = value > 0;
		} else if (strcmp(prop, "ForceReduceDurability") == 0) {
			ForceReduceDurability = value > 0;
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
			if (Type == DamageHelpers::HT_CustomHit) {
				Strength = value / 100.0f;
			} else {
				OsiError("Property 'Strength' can only be set for custom hits");
			}
		}
		else {
			Hit->SetInt(prop, value);
		}
	}

	void DamageHelpers::SetVector(char const * prop, Vector3 const & value)
	{
		if (Type != DamageHelpers::HT_CustomHit) {
			OsiError("Impact vectors can only be set for custom hits");
			return;
		}

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

	void DamageHelpers::SetFlag(char const * flag)
	{
		Hit->SetFlag(flag);
	}

	void DamageHelpers::SetString(char const * prop, char const * value)
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
			Hit->SetString(prop, value);
		}
	}

	void DamageHelpers::AddDamage(DamageType DamageType, int32_t Amount)
	{
		DamageList->AddDamage(DamageType, Amount);
	}

	EsvStatusHit * DamageHelpers::Execute()
	{
		if (Type != DamageHelpers::HT_CustomHit) {
			OsiError("Called on a DamageHelper that is not a custom hit!");
			return false;
		}

		if (!Target) {
			OsiError("No target!");
			return false;
		}

		if (DamageList->Size == 0) {
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
		damage.Equipment = Hit->Equipment;
		damage.TotalDamageDone = Hit->TotalDamageDone;
		damage.Unknown = Hit->Unknown;
		damage.DeathType = Hit->DeathType;
		damage.DamageType = Hit->DamageType;
		damage.AttackDirection = Hit->AttackDirection;
		damage.ArmorAbsorption = Hit->ArmorAbsorption;
		damage.LifeSteal = Hit->LifeSteal;
		damage.EffectFlags = Hit->EffectFlags;
		damage.HitWithWeapon = Hit->HitWithWeapon;

		if (CallCharacterHit) {
			characterHit(Target, Source->Stats, nullptr, DamageList, HitType, RollForDamage ? 1 : 0, 
				&damage, ForceReduceDurability ? 1 : 0, nullptr, HighGround, ProcWindWalker, Critical);
		} else {
			damage.DamageList.Size = DamageList->Size;
			for (uint32_t i = 0; i < DamageList->Size; i++) {
				damage.DamageList.Buf[i] = DamageList->Buf[i];
				damage.TotalDamageDone += DamageList->Buf[i].Amount;
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
		return hit;
	}

	namespace func
	{
		bool HitPrepare(OsiArgumentDesc & args)
		{
			auto targetGuid = args[0].String;
			auto sourceGuid = args[1].String;
			auto & helperHandle = args[2].Int64;

			auto target = FindCharacterByNameGuid(targetGuid);
			if (target == nullptr) {
				OsiError("Target character '" << targetGuid << "' doesn't exist!");
				return false;
			}

			auto helper = gOsirisProxy->GetExtensionState().DamageHelpers.Create();
			helper->Type = DamageHelpers::HT_CustomHit;
			helper->Target = target;
			helper->Source = FindCharacterByNameGuid(sourceGuid);
			helper->SetInternalDamageInfo();

			helperHandle = helper->Handle;
			return true;
		}

		DamageHelpers * HelperHandleToHelper(int64_t handle)
		{
			auto helper = gOsirisProxy->GetExtensionState().DamageHelpers.Get(handle);
			if (helper == nullptr) {
				OsiError("Damage helper handle " << handle << " doesn't exist!");
			}

			return helper;
		}

		void HitExecute(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			if (helper == nullptr) return;

			helper->Execute();
			gOsirisProxy->GetExtensionState().DamageHelpers.Destroy(helper->Handle);
		}

		bool HitExecuteRetStatus(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto & statusHandle = args[1].Int64;

			if (helper == nullptr) return false;

			auto status = helper->Execute();
			gOsirisProxy->GetExtensionState().DamageHelpers.Destroy(helper->Handle);
			if (status == nullptr) {
				return false;
			} else {
				statusHandle = (int64_t)status->StatusHandle;
				return true;
			}
		}

		void HitSetInt(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			auto value = args[2].Int32;

			if (helper == nullptr) return;

			helper->SetInt(prop, value);
		}

		void HitSetString(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			auto value = args[2].String;

			if (helper == nullptr) return;

			helper->SetString(prop, value);
		}

		void HitSetVector3(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			Vector3 vec = args.GetVector(2);

			if (helper == nullptr) return;

			helper->SetVector(prop, vec);
		}

		void HitSetFlag(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto flag = args[1].String;

			if (helper == nullptr) return;

			helper->SetFlag(flag);
		}

		void HitClearAllDamage(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);

			if (helper == nullptr) return;

			helper->DamageList->Clear();
		}

		void HitClearDamage(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto damageTypeStr = args[1].String;

			if (helper == nullptr) return;

			auto damageType = EnumInfo<DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			auto & dmgList = *helper->DamageList;
			for (uint32_t i = 0; i < dmgList.Size; i++) {
				if (dmgList.Buf[i].DamageType == *damageType) {
					dmgList.Remove(i);
					break;
				}
			}
		}

		bool HitGetDamage(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto damageTypeStr = args[1].String;
			auto & damageAmount = args[2].Int32;

			if (helper == nullptr) return false;

			auto damageType = EnumInfo<DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return false;
			}

			auto & dmgList = *helper->DamageList;
			int32_t amount = 0;
			for (uint32_t i = 0; i < dmgList.Size; i++) {
				if (dmgList.Buf[i].DamageType == *damageType) {
					amount += dmgList.Buf[i].Amount;
				}
			}

			damageAmount = amount;
			return true;
		}

		void HitAddDamage(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto damageTypeStr = args[1].String;
			auto amount = args[2].Int32;

			if (helper == nullptr) return;

			auto damageType = EnumInfo<DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			helper->AddDamage(*damageType, amount);
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
			int32_t amount = 0;
			for (uint32_t i = 0; i < dmgList.Size; i++) {
				if (dmgList.Buf[i].DamageType == *damageType) {
					amount += dmgList.Buf[i].Amount;
				}
			}

			args.Get(3).Int32 = amount;
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

			status->DamageInfo.DamageList.AddDamage(*damageType, amount);
		}
	}

	void CustomFunctionLibrary::RegisterHitFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto hitPrepare = std::make_unique<CustomQuery>(
			"NRD_HitPrepare",
			std::vector<CustomFunctionParam>{
				{ "Target", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Source", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::HitPrepare
		);
		functionMgr.Register(std::move(hitPrepare));

		auto hitExecute = std::make_unique<CustomCall>(
			"NRD_HitExecute",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In }
			},
			&func::HitExecute
		);
		functionMgr.Register(std::move(hitExecute));

		auto hitExecuteEx = std::make_unique<CustomQuery>(
			"NRD_HitQryExecute",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::Out }
			},
			&func::HitExecuteRetStatus
		);
		functionMgr.Register(std::move(hitExecuteEx));

		auto hitSetInt = std::make_unique<CustomCall>(
			"NRD_HitSetInt",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitSetInt
		);
		functionMgr.Register(std::move(hitSetInt));

		auto hitSetString = std::make_unique<CustomCall>(
			"NRD_HitSetString",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitSetString
		);
		functionMgr.Register(std::move(hitSetString));

		auto hitSetVector3 = std::make_unique<CustomCall>(
			"NRD_HitSetVector3",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
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
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Flag", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitSetFlag
		);
		functionMgr.Register(std::move(hitSetFlag));

		auto hitClearAllDamage = std::make_unique<CustomCall>(
			"NRD_HitClearAllDamage",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In }
			},
			&func::HitClearAllDamage
		);
		functionMgr.Register(std::move(hitClearAllDamage));

		auto hitGetDamage = std::make_unique<CustomQuery>(
			"NRD_HitGetDamage",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::HitGetDamage
		);
		functionMgr.Register(std::move(hitGetDamage));

		auto hitClearDamage = std::make_unique<CustomCall>(
			"NRD_HitClearDamage",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitClearDamage
		);
		functionMgr.Register(std::move(hitClearDamage));

		auto hitAddDamage = std::make_unique<CustomCall>(
			"NRD_HitAddDamage",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitAddDamage
		);
		functionMgr.Register(std::move(hitAddDamage));



		auto hitStatusGetInt = std::make_unique<CustomQuery>(
			"NRD_HitStatusGetInt",
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
			"NRD_HitStatusSetInt",
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
			"NRD_HitStatusGetString",
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
			"NRD_HitStatusSetString",
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
			"NRD_HitStatusGetFlag",
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
			"NRD_HitStatusSetFlag",
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
			"NRD_HitStatusClearAllDamage",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In }
			},
			&func::HitStatusClearAllDamage
		);
		functionMgr.Register(std::move(hitStatusClearAllDamage));

		auto hitStatusGetDamage = std::make_unique<CustomQuery>(
			"NRD_HitStatusGetDamage",
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
			"NRD_HitStatusClearDamage",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitStatusClearDamage
		);
		functionMgr.Register(std::move(hitStatusClearDamage));

		auto hitStatusAddDamage = std::make_unique<CustomCall>(
			"NRD_HitStatusAddDamage",
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
