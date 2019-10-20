#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace osidbg
{
	FunctionHandle StatusIteratorEventHandle;
	FunctionHandle HitPrepareEventHandle;
	FunctionHandle HitEventHandle;
	FunctionHandle HealEventHandle;

	namespace func
	{
		void IterateCharacterStatuses(OsiArgumentDesc const & args)
		{
			auto characterGuid = args.Get(0).String;
			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				OsiError("Character " << characterGuid << " does not exist!");
				return;
			}

			auto eventName = args.Get(1).String;
			auto statuses = character->StatusManager;
			if (statuses != nullptr) {
				for (uint32_t index = 0; index < statuses->StatusCount; index++) {
					auto status = statuses->Statuses[index];
					auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
					eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, characterGuid });
					eventArgs->Add(OsiArgumentValue{ ValueType::String, status->StatusId.Str });
					eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

					gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(StatusIteratorEventHandle, eventArgs);

					delete eventArgs;
				}
			}
		}

		EsvStatus * GetStatusHelper(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("Character " << args.Get(0).String << " does not exist!");
				return nullptr;
			}

			ObjectHandle statusHandle{ args.Get(1).Int64 };
			auto status = character->GetStatusByHandle(statusHandle);
			if (status == nullptr) {
				OsiError("No status found with handle " << (int64_t)statusHandle);
				return nullptr;
			}

			return status;
		}

		bool StatusGetHandle(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("Character " << args.Get(0).String << " does not exist!");
				return false;
			}

			if (character->StatusManager == nullptr) {
				OsiError("Character " << args.Get(0).String << " has no StatusManager!");
				return false;
			}

			auto statusId = ToFixedString(args.Get(1).String);
			if (!statusId) {
				// No fixed string with this ID --> invalid status name
				OsiWarn("Status " << args.Get(1).String << " not in string table, possibly invalid status name?");
				return false;
			}

			auto statuses = character->StatusManager;
			for (uint32_t index = 0; index < statuses->StatusCount; index++) {
				auto status = statuses->Statuses[index];
				if (status->StatusId == statusId) {
					args.Get(2).Int64 = (int64_t)status->StatusHandle;
					return true;
				}
			}

			return false;
		}

		template <OsiPropertyMapType Type>
		bool StatusGetAttribute(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			switch (status->GetStatusId()) {
				case StatusType::Hit:
				{
					auto hit = static_cast<EsvStatusHit *>(status);
					if (OsirisPropertyMapGet(gHitDamageInfoPropertyMap, &hit->DamageInfo, args, 2, Type)) {
						return true;
					} else {
						return OsirisPropertyMapGet(gStatusHitPropertyMap, hit, args, 2, Type);
					}
				}

				case StatusType::Heal:
				{
					auto heal = static_cast<EsvStatusHeal *>(status);
					return OsirisPropertyMapGet(gStatusHealPropertyMap, heal, args, 2, Type);
				}

				default:
					return OsirisPropertyMapGet(gStatusPropertyMap, status, args, 2, Type);
			}
		}

		template <OsiPropertyMapType Type>
		void StatusSetAttribute(OsiArgumentDesc const & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return;
			}

			switch (status->GetStatusId()) {
				case StatusType::Hit:
				{
					auto hit = static_cast<EsvStatusHit *>(status);
					if (!OsirisPropertyMapSet(gHitDamageInfoPropertyMap, &hit->DamageInfo, args, 2, Type)) {
						OsirisPropertyMapSet(gStatusHitPropertyMap, hit, args, 2, Type);
					}
				}

				case StatusType::Heal:
				{
					auto heal = static_cast<EsvStatusHeal *>(status);
					OsirisPropertyMapSet(gStatusHealPropertyMap, heal, args, 2, Type);
				}

				default:
					OsirisPropertyMapSet(gStatusPropertyMap, status, args, 2, Type);
			}
		}

		template <class T>
		T * ConstructStatus(EsvStatusManager * statusMachine, char const * statusId, StatusType type)
		{
			auto statusIdFs = ToFixedString(statusId);
			T * status{ nullptr };

			if (statusIdFs) {
				status = (T *)gOsirisProxy->GetLibraryManager().StatusMachineCreateStatus(statusMachine, statusIdFs, 0);
			}

			if (status == nullptr) {
				OsiError("Status does not exist: " << statusId);
				return nullptr;
			}

			if (status->GetStatusId() != type) {
				// TODO - dangling status ptr!
				OsiError("Status has incorrect type: " << statusId);
				return nullptr;
			}

			return status;
		}


		bool ApplyActiveDefense(OsiArgumentDesc & args)
		{
			auto characterGuid = args.Get(0).String;
			auto statusId = args.Get(1).String;
			auto lifeTime = args.Get(2).Float;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				OsiError("Character " << characterGuid << " does not exist!");
				return false;
			}

			auto statusMachine = character->StatusManager;
			if (!statusMachine) {
				OsiError("Character has no StatusMachine!");
				return false;
			}

			auto status = ConstructStatus<EsvStatusActiveDefense>(statusMachine, statusId, StatusType::ActiveDefense);
			if (status == nullptr) {
				return false;
			}
			
			if (lifeTime < 0.0f) {
				status->Flags0 |= SF0_KeepAlive;
				status->CurrentLifeTime = 1.0f;
			} else {
				status->Flags0 |= SF0_IsLifeTimeSet;
				status->LifeTime = lifeTime;
				status->CurrentLifeTime = lifeTime;
			}

			ObjectHandle handle;
			character->GetObjectHandle(&handle);

			status->TargetHandle = handle;
			status->TargetPos = *character->GetTranslate();

			gOsirisProxy->GetLibraryManager().StatusMachineApplyStatus(statusMachine, status);
			args.Get(3).Int64 = (int64_t)status->StatusHandle;
			return true;
		}


		bool ApplyDamageOnMove(OsiArgumentDesc & args)
		{
			auto characterGuid = args.Get(0).String;
			auto statusId = args.Get(1).String;
			auto sourceCharacterGuid = args.Get(2).String;
			auto lifeTime = args.Get(3).Float;
			auto distancePerDamage = args.Get(4).Float;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				OsiError("Character " << characterGuid << " does not exist!");
				return false;
			}

			auto statusMachine = character->StatusManager;
			if (!statusMachine) {
				OsiError("Character has no StatusMachine!");
				return false;
			}

			if (distancePerDamage <= 0.01) {
				OsiError("DistancePerDamage must be a positive value!");
				return false;
			}

			auto status = ConstructStatus<EsvStatusDamageOnMove>(statusMachine, statusId, StatusType::DamageOnMove);
			if (status == nullptr) {
				return false;
			}

			if (lifeTime < 0.0f) {
				status->Flags0 |= SF0_KeepAlive;
				status->CurrentLifeTime = 1.0f;
			} else {
				status->Flags0 |= SF0_IsLifeTimeSet;
				status->LifeTime = lifeTime;
				status->CurrentLifeTime = lifeTime;
			}

			auto sourceCharacter = FindCharacterByNameGuid(sourceCharacterGuid);
			if (sourceCharacter == nullptr) {
				status->StatusSourceHandle = ObjectHandle{};
			} else {
				ObjectHandle sourceHandle;
				sourceCharacter->GetObjectHandle(&sourceHandle);
				status->StatusSourceHandle = sourceHandle;
			}

			status->StartTimer = 0.0f;
			status->StatsMultiplier = 1.0f;
			status->DistancePerDamage = distancePerDamage;

			gOsirisProxy->GetLibraryManager().StatusMachineApplyStatus(statusMachine, status);
			args.Get(5).Int64 = (int64_t)status->StatusHandle;
			return true;
		}
	}


	void CustomFunctionLibrary::OnStatusHitEnter(EsvStatus * status)
	{
		auto statusHit = static_cast<EsvStatusHit *>(status);
		if (statusHit->DamageInfo.EffectFlags & HitFlag::HF_NoEvents) {
			return;
		}

		auto target = FindCharacterByHandle(status->TargetCIHandle);
		if (target == nullptr) {
			OsiError("Status has no target?");
			return;
		}

		char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
		auto source = FindCharacterByHandle(status->StatusSourceHandle);
		if (source != nullptr) {
			sourceGuid = source->GetGuid()->Str;
		}

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, target->GetGuid()->Str });
		eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
		eventArgs->Add(OsiArgumentValue{ (int32_t)statusHit->DamageInfo.TotalDamageDone });
		eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

		gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(HitEventHandle, eventArgs);

		delete eventArgs;
	}


	void CustomFunctionLibrary::OnStatusHealEnter(EsvStatus * status)
	{
		auto statusHeal = static_cast<EsvStatusHeal *>(status);

		auto target = FindCharacterByHandle(status->TargetCIHandle);
		if (target == nullptr) {
			OsiError("Status has no target?");
			return;
		}

		char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
		auto source = FindCharacterByHandle(status->StatusSourceHandle);
		if (source != nullptr) {
			sourceGuid = source->GetGuid()->Str;
		}

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, target->GetGuid()->Str });
		eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
		eventArgs->Add(OsiArgumentValue{ (int32_t)statusHeal->HealAmount });
		eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

		gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(HealEventHandle, eventArgs);

		delete eventArgs;
	}


	void CustomFunctionLibrary::OnCharacterHit(Character__Hit wrappedHit, EsvCharacter * self, CDivinityStats_Character * attackerStats,
		CDivinityStats_Item * itemStats, DamagePairList * damageList, uint32_t hitType, bool rollForDamage,
		HitDamageInfo * damageInfo, int forceReduceDurability, void * skillProperties, HighGroundBonus highGround,
		bool procWindWalker, CriticalRoll criticalRoll)
	{
		if (damageInfo->EffectFlags & HitFlag::HF_NoEvents) {
			wrappedHit(self, attackerStats, itemStats, damageList, hitType, rollForDamage,
				damageInfo, forceReduceDurability, skillProperties, highGround,
				procWindWalker, criticalRoll);
			return;
		}

		char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
		if (attackerStats != nullptr
			&& attackerStats->Character != nullptr) {
			sourceGuid = attackerStats->Character->GetGuid()->Str;
		}

		int32_t totalDamage{ 0 };
		for (int32_t i = 0; i < damageList->Size; i++) {
			totalDamage += damageList->Buf[i].Amount;
		}

		auto helper = gOsirisProxy->GetExtensionState().DamageHelpers.Create();
		helper->Type = DamageHelpers::HT_PrepareHitEvent;
		helper->Target = self;
		if (attackerStats != nullptr) {
			helper->Source = attackerStats->Character;
		}

		helper->CallCharacterHit = true;
		helper->HitType = hitType;
		helper->RollForDamage = rollForDamage;
		helper->ProcWindWalker = procWindWalker;
		helper->HighGround = highGround;
		helper->Critical = criticalRoll;
		helper->ForceReduceDurability = (bool)forceReduceDurability;
		helper->SetExternalDamageInfo(damageInfo, damageList);

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, self->GetGuid()->Str });
		eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
		eventArgs->Add(OsiArgumentValue{ (int32_t)totalDamage });
		eventArgs->Add(OsiArgumentValue{ helper->Handle });

		gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(HitPrepareEventHandle, eventArgs);

		delete eventArgs;

		gOsirisProxy->GetExtensionState().DamageHelpers.Destroy(helper->Handle);

		wrappedHit(self, attackerStats, itemStats, damageList, helper->HitType, helper->RollForDamage,
			damageInfo, helper->ForceReduceDurability, skillProperties, helper->HighGround, 
			helper->ProcWindWalker, helper->Critical);
	}


	void CustomFunctionLibrary::RegisterStatusFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto iterateCharacterStatuses = std::make_unique<CustomCall>(
			"NRD_IterateCharacterStatuses",
			std::vector<CustomFunctionParam>{
				{ "CharacterGuid", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Event", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::IterateCharacterStatuses
		);
		functionMgr.Register(std::move(iterateCharacterStatuses));

		auto getStatusHandle = std::make_unique<CustomQuery>(
			"NRD_StatusGetHandle",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::StatusGetHandle
		);
		functionMgr.Register(std::move(getStatusHandle));

		auto getStatusAttributeInt = std::make_unique<CustomQuery>(
			"NRD_StatusGetInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttribute<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(getStatusAttributeInt));

		auto getStatusAttributeReal = std::make_unique<CustomQuery>(
			"NRD_StatusGetReal",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttribute<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(getStatusAttributeReal));

		auto getStatusAttributeString = std::make_unique<CustomQuery>(
			"NRD_StatusGetString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttribute<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(getStatusAttributeString));

		auto getStatusAttributeGuidString = std::make_unique<CustomQuery>(
			"NRD_StatusGetGuidString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttribute<OsiPropertyMapType::GuidStringHandle>
		);
		functionMgr.Register(std::move(getStatusAttributeGuidString));

		auto setStatusAttributeInt = std::make_unique<CustomCall>(
			"NRD_StatusSetInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::StatusSetAttribute<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(setStatusAttributeInt));

		auto setStatusAttributeReal = std::make_unique<CustomCall>(
			"NRD_StatusSetReal",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::StatusSetAttribute<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(setStatusAttributeReal));

		auto setStatusAttributeString = std::make_unique<CustomCall>(
			"NRD_StatusSetString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::StatusSetAttribute<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(setStatusAttributeString));

		auto setStatusAttributeGuidString = std::make_unique<CustomCall>(
			"NRD_StatusSetGuidString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::In },
			},
			&func::StatusSetAttribute<OsiPropertyMapType::GuidStringHandle>
		);
		functionMgr.Register(std::move(setStatusAttributeGuidString));

		auto setStatusAttributeVector3 = std::make_unique<CustomCall>(
			"NRD_StatusSetVector3",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::StatusSetAttribute<OsiPropertyMapType::Vector3>
		);
		functionMgr.Register(std::move(setStatusAttributeVector3));

		auto statusIteratorEvent = std::make_unique<CustomEvent>(
			"NRD_StatusIteratorEvent",
			std::vector<CustomFunctionParam>{
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
			}
		);
		StatusIteratorEventHandle = functionMgr.Register(std::move(statusIteratorEvent));

		auto applyActiveDefense = std::make_unique<CustomQuery>(
			"NRD_ApplyActiveDefense",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "LifeTime", ValueType::Real, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::ApplyActiveDefense
		);
		functionMgr.Register(std::move(applyActiveDefense));

		auto applyDamageOnMove = std::make_unique<CustomQuery>(
			"NRD_ApplyDamageOnMove",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "SourceCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "LifeTime", ValueType::Real, FunctionArgumentDirection::In },
				{ "DistancePerDamage", ValueType::Real, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::ApplyDamageOnMove
		);
		functionMgr.Register(std::move(applyDamageOnMove));

		auto hitPrepareEvent = std::make_unique<CustomEvent>(
			"NRD_OnPrepareHit",
			std::vector<CustomFunctionParam>{
				{ "Target", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Instigator", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Damage", ValueType::Integer, FunctionArgumentDirection::In },
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
			}
		);
		HitPrepareEventHandle = functionMgr.Register(std::move(hitPrepareEvent));

		auto hitEvent = std::make_unique<CustomEvent>(
			"NRD_OnHit",
			std::vector<CustomFunctionParam>{
				{ "Target", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Instigator", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Damage", ValueType::Integer, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
			}
		);
		HitEventHandle = functionMgr.Register(std::move(hitEvent));

		auto healEvent = std::make_unique<CustomEvent>(
			"NRD_OnHeal",
			std::vector<CustomFunctionParam>{
				{ "Target", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Instigator", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
			}
		);
		HealEventHandle = functionMgr.Register(std::move(healEvent));
	}

}
