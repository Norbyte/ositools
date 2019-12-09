#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

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

	bool DamageHelpers::GetInt(char const * prop, int32_t & value)
	{
		if (strcmp(prop, "SimulateHit") == 0) {
			value = SimulateHit ? 1 : 0;
		} else if (strcmp(prop, "HitType") == 0) {
			value = (int32_t)HitType;
		} else if (strcmp(prop, "RollForDamage") == 0) {
			value = RollForDamage ? 1 : 0;
		} else if (strcmp(prop, "ProcWindWalker") == 0) {
			value = ProcWindWalker ? 1 : 0;
		} else if (strcmp(prop, "ForceReduceDurability") == 0) {
			value = ForceReduceDurability ? 1 : 0;
		} else if (strcmp(prop, "HighGround") == 0) {
			value = (int32_t)HighGround;
		} else if (strcmp(prop, "CriticalRoll") == 0) {
			value = (int32_t)Critical;
		} else if (strcmp(prop, "HitReason") == 0) {
			value = (int32_t)HitReason;
		} else if (strcmp(prop, "DamageSourceType") == 0) {
			value = (int32_t)DamageSourceType;
		} else if (strcmp(prop, "Strength") == 0) {
			value = (int32_t)(Strength * 100.0f);
		} else {
			auto & propertyMap = gHitDamageInfoPropertyMap;
			auto flag = propertyMap.getFlag(Hit, prop, false, false);
			if (flag) {
				value = *flag ? 1 : 0;
			} else {
				auto val = propertyMap.getInt(Hit, prop, false, true);
				if (val) {
					value = (int32_t)*val;
				} else {
					return false;
				}
			}
		}

		return true;
	}

	bool DamageHelpers::GetString(char const * prop, char const * & value)
	{
		if (strcmp(prop, "HitType") == 0) {
			auto val = EnumInfo<osidbg::HitType>::Find(HitType);
			if (val) {
				value = *val;
			}
			return val.has_value();
		} else if (strcmp(prop, "HighGround") == 0) {
			auto val = EnumInfo<HighGroundBonus>::Find(HighGround);
			if (val) {
				value = *val;
			}
			return val.has_value();
		} else if (strcmp(prop, "CriticalRoll") == 0) {
			auto val = EnumInfo<CriticalRoll>::Find(Critical);
			if (val) {
				value = *val;
			}
			return val.has_value();
		} else if (strcmp(prop, "DamageSourceType") == 0) {
			auto val = EnumInfo<CauseType>::Find(DamageSourceType);
			if (val) {
				value = *val;
			}
			return val.has_value();
		} else {
			auto & propertyMap = gHitDamageInfoPropertyMap;
			auto val = propertyMap.getString(Hit, prop, false, true);
			if (val) {
				value = *val;
				return true;
			} else {
				return false;
			}
		}
	}

	void DamageHelpers::SetInt(char const * prop, int32_t value)
	{
		if (strcmp(prop, "SimulateHit") == 0) {
			if (Type == DamageHelpers::HT_CustomHit) {
				SimulateHit = value > 0;
			} else {
				OsiErrorS("Property 'SimulateHit' can only be set for custom hits");
			}
		}
		else if (strcmp(prop, "HitType") == 0) {
			auto val = EnumInfo<osidbg::HitType>::Find((osidbg::HitType)value);
			if (val) {
				HitType = (osidbg::HitType)value;
			} else {
				OsiError("Invalid value for enum 'HitType': " << value);
			}
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
			auto val = EnumInfo<HighGroundBonus>::Find((HighGroundBonus)value);
			if (val) {
				HighGround = (HighGroundBonus)value;
			} else {
				OsiError("Invalid value for enum 'HighGround': " << value);
			}
		}
		else if (strcmp(prop, "CriticalRoll") == 0) {
			auto val = EnumInfo<CriticalRoll>::Find((CriticalRoll)value);
			if (val) {
				Critical = (CriticalRoll)value;
			} else {
				OsiError("Invalid value for enum 'CriticalRoll': " << value);
			}
		}
		else if (strcmp(prop, "HitReason") == 0) {
			// FIXME enum + filter
			HitReason = (uint32_t)value;
		}
		else if (strcmp(prop, "DamageSourceType") == 0) {
			auto val = EnumInfo<CauseType>::Find((CauseType)value);
			if (val) {
				DamageSourceType = (CauseType)value;
			} else {
				OsiError("Invalid value for enum 'DamageSourceType': " << value);
			}
		}
		else if (strcmp(prop, "Strength") == 0) {
			if (Type == DamageHelpers::HT_CustomHit) {
				Strength = value / 100.0f;
			} else {
				OsiErrorS("Property 'Strength' can only be set for custom hits");
			}
		}
		else {
			auto & propertyMap = gHitDamageInfoPropertyMap;
			if (!propertyMap.setFlag(Hit, prop, value != 0, false, false)) {
				propertyMap.setInt(Hit, prop, value, false, true);
			}
		}
	}

	void DamageHelpers::SetVector(char const * prop, Vector3 const & value)
	{
		if (Type != DamageHelpers::HT_CustomHit) {
			OsiErrorS("Impact vectors can only be set for custom hits");
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

	void DamageHelpers::SetString(char const * prop, char const * value)
	{
		auto fs = ToFixedString(value);
		if (!fs) {
			OsiError("Could not map value '" << value << "' to FixedString");
			return;
		}

		if (strcmp(prop, "SkillId") == 0) {
			SkillId = fs;
		} else if (strcmp(prop, "HitType") == 0) {
			auto val = EnumInfo<osidbg::HitType>::Find(value);
			if (val) {
				HitType = *val;
			} else {
				OsiError("Invalid value for enum 'HitType': " << value);
			}
		} else if (strcmp(prop, "HighGround") == 0) {
			auto val = EnumInfo<HighGroundBonus>::Find(value);
			if (val) {
				HighGround = *val;
			} else {
				OsiError("Invalid value for enum 'HighGround': " << value);
			}
		} else if (strcmp(prop, "CriticalRoll") == 0) {
			auto val = EnumInfo<CriticalRoll>::Find(value);
			if (val) {
				Critical = *val;
			} else {
				OsiError("Invalid value for enum 'CriticalRoll': " << value);
			}
		} else if (strcmp(prop, "DamageSourceType") == 0) {
			auto val = EnumInfo<CauseType>::Find(value);
			if (val) {
				DamageSourceType = *val;
			} else {
				OsiError("Invalid value for enum 'DamageSourceType': " << value);
			}
		} else {
			auto & propertyMap = gHitDamageInfoPropertyMap;
			propertyMap.setString(Hit, prop, value, false, true);
		}
	}

	void DamageHelpers::AddDamage(DamageType DamageType, int32_t Amount)
	{
		DamageList->AddDamage(DamageType, Amount);
	}

	esv::StatusMachine * GetStatusMachine(char const * gameObjectGuid);

	esv::StatusHit * DamageHelpers::Execute()
	{
		if (!Target) {
			OsiErrorS("No target!");
			return false;
		}

		if (DamageList->Size == 0) {
			OsiErrorS("At least one damage type should be added!");
			return false;
		}

		auto characterHit = gOsirisProxy->GetLibraryManager().CharacterHit;
		if (SimulateHit && characterHit == nullptr) {
			OsiErrorS("esv::Character::Hit not found!");
			return false;
		}

		auto statusMachine = GetStatusMachine(Target->GetGuid()->Str);
		if (!statusMachine) {
			OsiErrorS("Target has no StatusMachine!");
			return false;
		}

		auto createStatus = gOsirisProxy->GetLibraryManager().StatusMachineCreateStatus;
		auto applyStatus = gOsirisProxy->GetLibraryManager().StatusMachineApplyStatus;
		if (createStatus == nullptr || applyStatus == nullptr) {
			OsiErrorS("esv::StatusMachine::CreateStatus not found!");
			return false;
		}

		auto fsHit = ToFixedString("HIT");
		auto hit = (esv::StatusHit *)createStatus(statusMachine, fsHit, 0);

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

		if (SimulateHit) {
			auto targetCharacter = FindCharacterByNameGuid(Target->GetGuid()->Str);
			if (targetCharacter == nullptr) {
				OsiErrorS("Attempt to hit an item with SimulateHit flag ?!");
			} else {
				characterHit(targetCharacter, Source->Stats, nullptr, DamageList, HitType, RollForDamage ? 1 : 0,
					&damage, ForceReduceDurability ? 1 : 0, nullptr, HighGround, ProcWindWalker, Critical);
			}

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

		applyStatus(statusMachine, hit);
		return hit;
	}

	namespace func
	{
		bool HitPrepare(OsiArgumentDesc & args)
		{
			auto targetGuid = args[0].String;
			auto sourceGuid = args[1].String;
			auto & helperHandle = args[2].Int64;

			auto target = FindGameObjectByNameGuid(targetGuid);
			if (target == nullptr) {
				OsiError("Target '" << targetGuid << "' doesn't exist!");
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

			if (helper->Type != DamageHelpers::HT_CustomHit) {
				OsiError("Called on a DamageHelper that is not a custom hit!");
				return;
			}

			helper->Execute();
			gOsirisProxy->GetExtensionState().DamageHelpers.Destroy(helper->Handle);
		}

		bool HitExecuteRetStatus(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto & statusHandle = args[1].Int64;

			if (helper == nullptr) return false;

			if (helper->Type != DamageHelpers::HT_CustomHit) {
				OsiError("Called on a DamageHelper that is not a custom hit!");
				return false;
			}

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

		bool HitGetInt(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			auto & value = args[2].Int32;

			if (helper == nullptr) return false;

			return helper->GetInt(prop, value);
		}

		void HitSetString(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			auto value = args[2].String;

			if (helper == nullptr) return;

			helper->SetString(prop, value);
		}

		bool HitGetString(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			auto & value = args[2].String;

			if (helper == nullptr) return false;

			return helper->GetString(prop, value);
		}

		void HitSetVector3(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			Vector3 vec = args.GetVector(2);

			if (helper == nullptr) return;

			helper->SetVector(prop, vec);
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

		esv::Status * GetStatusHelper(OsiArgumentDesc const & args);

		esv::StatusHit * HitStatusGet(OsiArgumentDesc const & args)
		{
			auto statusHandle = args[1].Int64;

			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				OsiError("No status found with this handle: " << statusHandle);
				return nullptr;
			}

			if (status->GetStatusId() != StatusType::Hit) {
				OsiError("Status with handle " << statusHandle << " is not a HIT!");
				return nullptr;
			}

			return static_cast<esv::StatusHit *>(status);
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

			auto damageTypeStr = args[2].String;
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

			auto damageTypeStr = args[2].String;
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

			args[3].Int32 = amount;
			return true;
		}

		void HitStatusAddDamage(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto damageTypeStr = args[2].String;
			auto amount = args[3].Int32;

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

		auto hitGetInt = std::make_unique<CustomQuery>(
			"NRD_HitGetInt",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::HitGetInt
		);
		functionMgr.Register(std::move(hitGetInt));

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

		auto hitGetString = std::make_unique<CustomQuery>(
			"NRD_HitGetString",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::HitGetString
		);
		functionMgr.Register(std::move(hitGetString));

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


		auto hitStatusClearAllDamage = std::make_unique<CustomCall>(
			"NRD_HitStatusClearAllDamage",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In }
			},
			&func::HitStatusClearAllDamage
		);
		functionMgr.Register(std::move(hitStatusClearAllDamage));

		auto hitStatusGetDamage = std::make_unique<CustomQuery>(
			"NRD_HitStatusGetDamage",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
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
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitStatusClearDamage
		);
		functionMgr.Register(std::move(hitStatusClearDamage));

		auto hitStatusAddDamage = std::make_unique<CustomCall>(
			"NRD_HitStatusAddDamage",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitStatusAddDamage
		);
		functionMgr.Register(std::move(hitStatusAddDamage));

	}

}
