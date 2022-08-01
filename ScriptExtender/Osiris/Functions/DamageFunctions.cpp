#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse::esv
{
	esv::Status* GetStatusHelper(OsiArgumentDesc const& args);

	void DamageHelperPool::Clear()
	{
		helpers_.clear();
	}

	DamageHelpers * DamageHelperPool::Create()
	{
		auto id = nextHelperId_++;
		auto helper = std::make_unique<DamageHelpers>();
		helper->Handle = ComponentHandle(DamageHelpers::HitHandleTypeId, id, 0);
		auto ptr = helper.get();
		helpers_.insert(std::make_pair(id, std::move(helper)));
		return ptr;
	}

	bool DamageHelperPool::Destroy(ComponentHandle handle)
	{
		if (handle.GetType() != DamageHelpers::HitHandleTypeId) {
			return false;
		}

		auto it = helpers_.find(handle.GetIndex());
		if (it == helpers_.end()) {
			return false;
		} else {
			helpers_.erase(it);
			return true;
		}
	}

	DamageHelpers * DamageHelperPool::Get(ComponentHandle handle) const
	{
		if (handle.GetType() != DamageHelpers::HitHandleTypeId) {
			return nullptr;
		}

		auto it = helpers_.find(handle.GetIndex());
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
	}


	void DamageHelpers::SetInternalDamageInfo()
	{
		damageInfo_ = std::make_unique<stats::HitDamageInfo>();
		damageList_ = std::make_unique<stats::DamagePairList>();

		Hit = damageInfo_.get();
		DamageList = damageList_.get();
	}

	void DamageHelpers::SetExternalDamageInfo(stats::HitDamageInfo * damageInfo, stats::DamagePairList * damageList)
	{
		damageInfo_.reset();
		damageList_.reset();
		Hit = damageInfo;
		DamageList = damageList;
	}

	void DamageHelpers::SetVector(FixedString const& prop, glm::vec3 const & value)
	{
		if (Type != DamageHelpers::HT_CustomHit) {
			OsiErrorS("Impact vectors can only be set for custom hits");
			return;
		}

		if (prop == GFS.strImpactPosition) {
			ImpactPosition = value;
			HasImpactPosition = true;
		}
		else if (prop == GFS.strImpactOrigin) {
			ImpactOrigin = value;
			HasImpactOrigin = true;
		}
		else if (prop == GFS.strImpactDirection) {
			ImpactDirection = value;
			HasImpactDirection = true;
		}
		else {
			OsiError("Unknown vector3 property '" << prop << "'");
		}
	}

	void DamageHelpers::AddDamage(stats::DamageType damageType, int32_t Amount)
	{
		DamageList->AddDamage(damageType, Amount);
	}

	esv::StatusMachine * GetStatusMachine(char const * gameObjectGuid);

	esv::StatusHit * DamageHelpers::Execute()
	{
		if (!Target) {
			OsiErrorS("No target!");
			return nullptr;
		}

		if (DamageList->size() == 0) {
			stats::TDamagePair dummy;
			dummy.Amount = 0;
			dummy.DamageType = stats::DamageType::Physical;
			DamageList->push_back(dummy);
		}

		auto characterHit = GetStaticSymbols().esv__Character__Hit;
		if (SimulateHit && characterHit == nullptr) {
			OsiErrorS("esv::Character::Hit not found!");
			return nullptr;
		}

		auto statusMachine = GetStatusMachine(Target->GetGuid()->GetString());
		if (!statusMachine) {
			OsiErrorS("Target has no StatusMachine!");
			return nullptr;
		}

		auto createStatus = GetStaticSymbols().esv__StatusMachine__CreateStatus;
		auto applyStatus = GetStaticSymbols().esv__StatusMachine__ApplyStatus;
		if (createStatus == nullptr || applyStatus == nullptr) {
			OsiErrorS("esv::StatusMachine::CreateStatus not found!");
			return nullptr;
		}

		FixedString fsHit("HIT");
		auto hit = (esv::StatusHit *)createStatus(statusMachine, fsHit, 0);

		hit->StartTimer = 0.0f;
		hit->HitByHandle = ComponentHandle{};
		hit->HitWithHandle = ComponentHandle{};
		hit->WeaponHandle = ComponentHandle{};
		hit->HitReason = HitReason;
		hit->Flags0 &= ~esv::StatusFlags0::IsFromItem; // Clear IsFromItem

		if (SkillId) {
			hit->SkillId = SkillId;
		} else {
			hit->SkillId = FixedString("");
		}

		hit->Strength = Strength;
		hit->DamageSourceType = DamageSourceType;
		hit->StatusSourceHandle = ComponentHandle{};

		if (Source != nullptr) {
			ComponentHandle sourceHandle;
			Source->GetObjectHandle(sourceHandle);
			hit->HitByHandle = sourceHandle;
			hit->StatusSourceHandle = sourceHandle;
		}

		auto & damage = hit->Hit;
		damage.Equipment = Hit->Equipment;
		damage.TotalDamageDone = Hit->TotalDamageDone;
		damage.DamageDealt = Hit->DamageDealt;
		damage.DeathType = Hit->DeathType;
		damage.DamageType = Hit->DamageType;
		damage.AttackDirection = Hit->AttackDirection;
		damage.ArmorAbsorption = Hit->ArmorAbsorption;
		damage.LifeSteal = Hit->LifeSteal;
		damage.EffectFlags = Hit->EffectFlags;
		damage.HitWithWeapon = Hit->HitWithWeapon;

		if (SimulateHit) {
			auto targetCharacter = GetEntityWorld()->GetComponent<Character>(Target->GetGuid()->GetString());
			if (targetCharacter == nullptr) {
				OsiErrorS("Attempt to hit an item with SimulateHit flag ?!");
			} else {
				characterHit(targetCharacter, Source ? Source->Stats : nullptr, nullptr, DamageList, HitType, NoHitRoll,
					&damage, ForceReduceDurability ? 1 : 0, nullptr, HighGround, ProcWindWalker, CriticalRoll);
			}

		} else {
			damage.DamageList = *DamageList;
			for (auto const& dmg : *DamageList) {
				damage.TotalDamageDone += dmg.Amount;
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
			auto & helperHandle = args[2];

			auto target = GetEntityWorld()->GetGameObject(targetGuid);
			if (target == nullptr) {
				OsiError("Target '" << targetGuid << "' doesn't exist!");
				return false;
			}

			auto helper = gExtender->GetServer().GetExtensionState().DamageHelpers.Create();
			helper->Type = DamageHelpers::HT_CustomHit;
			helper->Target = target;
			helper->Source = GetEntityWorld()->GetComponent<Character>(sourceGuid);
			helper->SetInternalDamageInfo();

			helperHandle.Set((int64_t)helper->Handle.Handle);
			return true;
		}

		DamageHelpers * HelperHandleToHelper(int64_t handleInt)
		{
			ComponentHandle handle{ handleInt };
			if (handle.GetType() != DamageHelpers::HitHandleTypeId) {
				OsiError("Attempted to use handle of type " << handle.GetType() << " in a hit function.");
				OsiError("For HIT statuses and handles received from NRD_OnHit use the NRD_StatusGet... functions instead!");
				return nullptr;
			}

			auto helper = gExtender->GetServer().GetExtensionState().DamageHelpers.Get(handle);
			if (helper == nullptr) {
				OsiError("Damage helper handle 0x" << std::hex << handle.Handle << " doesn't exist!");
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
			gExtender->GetServer().GetExtensionState().DamageHelpers.Destroy(helper->Handle);
		}

		bool HitExecuteRetStatus(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto & statusHandle = args[1];

			if (helper == nullptr) return false;

			if (helper->Type != DamageHelpers::HT_CustomHit) {
				OsiError("Called on a DamageHelper that is not a custom hit!");
				return false;
			}

			auto status = helper->Execute();
			gExtender->GetServer().GetExtensionState().DamageHelpers.Destroy(helper->Handle);
			if (status == nullptr) {
				return false;
			} else {
				statusHandle.Set((int64_t)status->StatusHandle);
				return true;
			}
		}

		template <OsiPropertyMapType Type>
		void HitSet(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			if (helper == nullptr) return;

			if (!OsirisPropertyMapSet(gDamageHelpersPropertyMap, helper, args, 1, Type, false)) {
				OsirisPropertyMapSet(gHitDamageInfoPropertyMap, helper->Hit, args, 1, Type);
			}
		}

		template <OsiPropertyMapType Type>
		bool HitGet(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			if (helper == nullptr) return false;

			if (OsirisPropertyMapGet(gDamageHelpersPropertyMap, helper, args, 1, Type, false)) {
				return true;
			} else {
				return OsirisPropertyMapGet(gHitDamageInfoPropertyMap, helper->Hit, args, 1, Type);
			}
		}

		void HitSetVector3(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto prop = args[1].String;
			glm::vec3 vec = args.GetVector(2);

			if (helper == nullptr) return;

			helper->SetVector(FixedString(prop), vec);
		}

		void HitClearAllDamage(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);

			if (helper == nullptr) return;

			helper->DamageList->clear();
		}

		void HitClearDamage(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto damageTypeStr = args[1].String;

			if (helper == nullptr) return;

			auto damageType = EnumInfo<stats::DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			auto & dmgList = *helper->DamageList;
			for (uint32_t i = 0; i < dmgList.size(); i++) {
				if (dmgList[i].DamageType == *damageType) {
					dmgList.remove(i);
					break;
				}
			}
		}

		bool HitGetDamage(OsiArgumentDesc & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto damageTypeStr = args[1].String;
			auto & damageAmount = args[2];

			if (helper == nullptr) return false;

			auto damageType = EnumInfo<stats::DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return false;
			}

			int32_t amount = 0;
			for (auto const& dmg : *helper->DamageList) {
				if (dmg.DamageType == *damageType) {
					amount += dmg.Amount;
				}
			}

			damageAmount.Set(amount);
			return true;
		}

		void HitAddDamage(OsiArgumentDesc const & args)
		{
			auto helper = HelperHandleToHelper(args[0].Int64);
			auto damageTypeStr = args[1].String;
			auto amount = args[2].Int32;

			if (helper == nullptr) return;

			auto damageType = EnumInfo<stats::DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			helper->AddDamage(*damageType, amount);
		}

		esv::StatusHit * HitStatusGet(OsiArgumentDesc const & args)
		{
			auto statusHandle = args[1].Int64;

			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				OsiError("No status found with this handle: " << statusHandle);
				return nullptr;
			}

			if (status->GetStatusId() != StatusType::HIT) {
				OsiError("Status with handle " << statusHandle << " is not a HIT!");
				return nullptr;
			}

			return static_cast<esv::StatusHit *>(status);
		}

		void HitStatusClearAllDamage(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			status->Hit.ClearDamage();
		}

		void HitStatusClearDamage(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto damageTypeStr = args[2].String;
			auto damageType = EnumInfo<stats::DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			status->Hit.ClearDamage(*damageType);
		}

		bool HitStatusGetDamage(OsiArgumentDesc & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return false;

			auto damageTypeStr = args[2].String;
			auto damageType = EnumInfo<stats::DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return false;
			}

			auto & dmgList = status->Hit.DamageList;
			int32_t amount = 0;
			for (auto const& dmg : status->Hit.DamageList) {
				if (dmg.DamageType == *damageType) {
					amount += dmg.Amount;
				}
			}

			args[3].Set(amount);
			return true;
		}

		void HitStatusAddDamage(OsiArgumentDesc const & args)
		{
			auto status = HitStatusGet(args);
			if (status == nullptr) return;

			auto damageTypeStr = args[2].String;
			auto amount = args[3].Int32;

			auto damageType = EnumInfo<stats::DamageType>::Find(damageTypeStr);
			if (!damageType) {
				OsiError("Not a valid DamageType: " << damageTypeStr);
				return;
			}

			status->Hit.AddDamage(*damageType, amount);
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
			&func::HitGet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(hitGetInt));

		auto hitSetInt = std::make_unique<CustomCall>(
			"NRD_HitSetInt",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::HitSet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(hitSetInt));

		auto hitGetString = std::make_unique<CustomQuery>(
			"NRD_HitGetString",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::HitGet<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(hitGetString));

		auto hitSetString = std::make_unique<CustomCall>(
			"NRD_HitSetString",
			std::vector<CustomFunctionParam>{
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::HitSet<OsiPropertyMapType::String>
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
