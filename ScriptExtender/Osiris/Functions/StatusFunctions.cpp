#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse::esv
{
	FunctionHandle StatusIteratorEventHandle;
	FunctionHandle StatusAttemptEventHandle;
	FunctionHandle HitPrepareEventHandle;
	FunctionHandle HitEventHandle;
	FunctionHandle HealEventHandle;
	FunctionHandle ActionStateEnterHandle;
	FunctionHandle ActionStateExitHandle;

	esv::StatusMachine * GetStatusMachine(char const * gameObjectGuid)
	{
		auto character = GetEntityWorld()->GetComponent<Character>(gameObjectGuid, false);
		if (character != nullptr) {
			return character->StatusMachine;
		}

		auto item = GetEntityWorld()->GetComponent<Item>(gameObjectGuid, false);
		if (item != nullptr) {
			return item->StatusMachine;
		}

		OsiError("Character or item " << gameObjectGuid << " does not exist!");
		return nullptr;
	}

	namespace func
	{
		void IterateStatuses(OsiArgumentDesc const & args)
		{
			auto gameObjectGuid = args[0].String;
			auto eventName = args[1].String;

			auto statusMachine = GetStatusMachine(gameObjectGuid);
			if (statusMachine == nullptr) return;

			for (auto const status : statusMachine->Statuses) {
				auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
				eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, gameObjectGuid });
				eventArgs->Add(OsiArgumentValue{ ValueType::String, status->StatusId });
				eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

				gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(StatusIteratorEventHandle, eventArgs);

				delete eventArgs;
			}
		}

		bool ObjectHasStatusType(OsiArgumentDesc& args)
		{
			auto gameObjectGuid = args[0].String;
			auto statusType = args[1].String;
			auto& hasStatus = args[2];

			auto statusMachine = GetStatusMachine(gameObjectGuid);
			if (statusMachine == nullptr) return false;
			
			auto typeId = EnumInfo<StatusType>::Find(FixedString(statusType));
			if (!typeId) {
				OsiError("Status type unknown: " << statusType);
				return false;
			}

			hasStatus.Set(0);
			for (auto const status : statusMachine->Statuses) {
				if (status->GetStatusId() == *typeId) {
					hasStatus.Set(1);
					break;
				}
			}

			return true;
		}

		bool StatusGetHandle(OsiArgumentDesc & args)
		{
			auto gameObjectGuid = args[0].String;
			auto statusId = args[1].String;

			auto statusMachine = GetStatusMachine(gameObjectGuid);
			if (statusMachine == nullptr) return false;

			FixedString statusIdFS(args[1].String);

			for (auto const status : statusMachine->Statuses) {
				if (status->StatusId == statusIdFS) {
					args[2].Set((int64_t)status->StatusHandle);
					return true;
				}
			}

			return false;
		}

		void StatusPreventApply(OsiArgumentDesc const & args)
		{
			auto gameObject = GetEntityWorld()->GetGameObject(args[0].String);
			auto statusHandle = ComponentHandle{ args[1].Int64 };
			auto preventApply = args[2].Int32;

			if (gameObject == nullptr) {
				OsiError("Game object " << args[0].String << " does not exist!");
				return;
			}

			gExtender->GetServer().GetStatusHelpers().PreventApply(gameObject, statusHandle, preventApply != 0);
		}


		bool ApplyActiveDefense(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto statusId = args[1].String;
			auto lifeTime = args[2].Float;

			auto character = GetEntityWorld()->GetComponent<Character>(characterGuid);
			if (character == nullptr) {
				OsiError("Character " << characterGuid << " does not exist!");
				return false;
			}

			auto statusHandle = gExtender->GetServer().GetOsirisStatusHelpers().ApplyActiveDefense(character, FixedString(statusId), lifeTime);
			if (statusHandle) {
				args[3].Set((int64_t)*statusHandle);
				return true;
			} else {
				return false;
			}
		}


		bool ApplyDamageOnMove(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto statusId = args[1].String;
			auto sourceCharacterGuid = args[2].String;
			auto lifeTime = args[3].Float;
			auto distancePerDamage = args[4].Float;

			auto character = GetEntityWorld()->GetComponent<Character>(characterGuid);
			if (character == nullptr) {
				OsiError("Character " << characterGuid << " does not exist!");
				return false;
			}

			auto sourceCharacter = GetEntityWorld()->GetComponent<Character>(sourceCharacterGuid);
			auto statusHandle = gExtender->GetServer().GetOsirisStatusHelpers().ApplyDamageOnMove(character, FixedString(statusId), sourceCharacter, lifeTime, distancePerDamage);
			if (statusHandle) {
				args[5].Set((int64_t)*statusHandle);
				return true;
			} else {
				return false;
			}
		}



		template <OsiPropertyMapType Type>
		bool ActionStateGet(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;

			auto character = GetEntityWorld()->GetComponent<Character>(characterGuid);
			if (character == nullptr
				|| character->ActionMachine == nullptr
				|| character->ActionMachine->Layers[0].State == nullptr) {
				return false;
			}

			auto state = character->ActionMachine->Layers[0].State;

			if (state->GetType() == ActionStateType::UseSkill) {
				auto useSkill = static_cast<ASUseSkill*>(state);
				if (useSkill->Skill != nullptr) {
					return OsirisPropertyMapGetRaw(gSkillStatePropertyMap, useSkill->Skill, args, 1, Type);
				} else {
					OsiError("No skill bound to ASUseSkill!");
					return false;
				}
			}

			auto propertyMap = state->GetPropertyMap();
			if (propertyMap != nullptr) {
				return OsirisPropertyMapGetRaw(*propertyMap, state, args, 1, Type, true);
			} else {
				OsiError("No property map available for action type " << (unsigned)state->GetType() << " yet!");
				return false;
			}
		}

		bool CharacterGetCurrentAction(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto & action = args[1];

			auto character = GetEntityWorld()->GetComponent<Character>(characterGuid);
			if (character == nullptr
				|| character->ActionMachine == nullptr) {
				return false;
			}

			auto actionState = character->ActionMachine->Layers[0].State;

			if (actionState == nullptr) {
				action.Set("");
				return true;
			}

			auto typeName = actionState->GetTypeName();
			if (typeName == nullptr) return false;
			action.Set(typeName);
			return true;
		}
	}

	esv::Status * GetStatusHelper(OsiArgumentDesc const & args)
	{
		auto gameObjectGuid = args[0].String;
		ComponentHandle statusHandle{ args[1].Int64 };

		if (statusHandle.GetType() == DamageHelpers::HitHandleTypeId) {
			OsiError("Attempted to use a hit handle in a status function.");
			OsiError("For handles received from NRD_OnPrepareHit/NRD_PrepareHit use the NRD_HitGet... functions instead!");
			return nullptr;
		}

		esv::Status * status{ nullptr };
		auto character = GetEntityWorld()->GetComponent<Character>(gameObjectGuid, false);
		if (character != nullptr) {
			status = character->GetStatus(statusHandle, true);
		} else {
			auto item = GetEntityWorld()->GetComponent<Item>(gameObjectGuid, false);
			if (item != nullptr) {
				status = item->GetStatus(statusHandle, true);
			} else {
				OsiError("Character or item " << gameObjectGuid << " does not exist!");
				return nullptr;
			}
		}

		if (status == nullptr) {
			OsiError("No status found with handle " << (int64_t)statusHandle);
			return nullptr;
		}

		return status;
	}


	void CustomFunctionLibrary::RegisterStatusFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto iterateCharacterStatuses = std::make_unique<CustomCall>(
			"NRD_IterateStatuses",
			std::vector<CustomFunctionParam>{
				{ "ObjectGuid", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Event", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::IterateStatuses
		);
		functionMgr.Register(std::move(iterateCharacterStatuses));

		auto hasStatusType = std::make_unique<CustomQuery>(
			"NRD_ObjectHasStatusType",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusType", ValueType::String, FunctionArgumentDirection::In },
				{ "HasStatus", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::ObjectHasStatusType
		);
		functionMgr.Register(std::move(hasStatusType));

		auto getStatusHandle = std::make_unique<CustomQuery>(
			"NRD_StatusGetHandle",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::StatusGetHandle
		);
		functionMgr.Register(std::move(getStatusHandle));

		auto getStatusAttributeInt = std::make_unique<CustomQuery>(
			"NRD_StatusGetInt",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&StatusGetAttribute<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(getStatusAttributeInt));

		auto getStatusAttributeReal = std::make_unique<CustomQuery>(
			"NRD_StatusGetReal",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&StatusGetAttribute<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(getStatusAttributeReal));

		auto getStatusAttributeString = std::make_unique<CustomQuery>(
			"NRD_StatusGetString",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&StatusGetAttribute<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(getStatusAttributeString));

		auto getStatusAttributeGuidString = std::make_unique<CustomQuery>(
			"NRD_StatusGetGuidString",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&StatusGetAttribute<OsiPropertyMapType::GuidStringHandle>
		);
		functionMgr.Register(std::move(getStatusAttributeGuidString));

		auto setStatusAttributeInt = std::make_unique<CustomCall>(
			"NRD_StatusSetInt",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&StatusSetAttribute<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(setStatusAttributeInt));

		auto setStatusAttributeReal = std::make_unique<CustomCall>(
			"NRD_StatusSetReal",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::In },
			},
			&StatusSetAttribute<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(setStatusAttributeReal));

		auto setStatusAttributeString = std::make_unique<CustomCall>(
			"NRD_StatusSetString",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In },
			},
			&StatusSetAttribute<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(setStatusAttributeString));

		auto setStatusAttributeGuidString = std::make_unique<CustomCall>(
			"NRD_StatusSetGuidString",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::In },
			},
			&StatusSetAttribute<OsiPropertyMapType::GuidStringHandle>
		);
		functionMgr.Register(std::move(setStatusAttributeGuidString));

		auto setStatusAttributeVector3 = std::make_unique<CustomCall>(
			"NRD_StatusSetVector3",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
			},
			&StatusSetAttribute<OsiPropertyMapType::Vector3>
		);
		functionMgr.Register(std::move(setStatusAttributeVector3));

		auto statusPreventApply = std::make_unique<CustomCall>(
			"NRD_StatusPreventApply",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "PreventApply", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::StatusPreventApply
		);
		functionMgr.Register(std::move(statusPreventApply));

		auto statusIteratorEvent = std::make_unique<CustomEvent>(
			"NRD_StatusIteratorEvent",
			std::vector<CustomFunctionParam>{
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
			}
		);
		StatusIteratorEventHandle = functionMgr.Register(std::move(statusIteratorEvent));

		auto statusAttemptEvent = std::make_unique<CustomEvent>(
			"NRD_OnStatusAttempt",
			std::vector<CustomFunctionParam>{
				{ "Target", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Instigator", ValueType::GuidString, FunctionArgumentDirection::In },
			}
		);
		StatusAttemptEventHandle = functionMgr.Register(std::move(statusAttemptEvent));

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
				{ "Target", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Instigator", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Damage", ValueType::Integer, FunctionArgumentDirection::In },
				{ "HitHandle", ValueType::Integer64, FunctionArgumentDirection::In },
			}
		);
		HitPrepareEventHandle = functionMgr.Register(std::move(hitPrepareEvent));

		auto hitEvent = std::make_unique<CustomEvent>(
			"NRD_OnHit",
			std::vector<CustomFunctionParam>{
				{ "Target", ValueType::GuidString, FunctionArgumentDirection::In },
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


		auto actionStateEnterEvent = std::make_unique<CustomEvent>(
			"NRD_OnActionStateEnter",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Action", ValueType::String, FunctionArgumentDirection::In }
			}
		);
		ActionStateEnterHandle = functionMgr.Register(std::move(actionStateEnterEvent));

		auto actionStateExitEvent = std::make_unique<CustomEvent>(
			"NRD_OnActionStateExit",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Action", ValueType::String, FunctionArgumentDirection::In }
			}
		);
		ActionStateExitHandle = functionMgr.Register(std::move(actionStateExitEvent));

		auto characterGetCurrentAction = std::make_unique<CustomQuery>(
			"NRD_CharacterGetCurrentAction",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Action", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::CharacterGetCurrentAction
		);
		functionMgr.Register(std::move(characterGetCurrentAction));

		auto actionStateGetInt = std::make_unique<CustomQuery>(
			"NRD_ActionStateGetInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::ActionStateGet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(actionStateGetInt));

		auto actionStateGetReal = std::make_unique<CustomQuery>(
			"NRD_ActionStateGetReal",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::ActionStateGet<OsiPropertyMapType::Real>
		);
		functionMgr.Register(std::move(actionStateGetReal));

		auto actionStateGetString = std::make_unique<CustomQuery>(
			"NRD_ActionStateGetString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::ActionStateGet<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(actionStateGetString));

		auto actionStateGetGuidString = std::make_unique<CustomQuery>(
			"NRD_ActionStateGetGuidString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::Out }
			},
			&func::ActionStateGet<OsiPropertyMapType::GuidStringHandle>
		);
		functionMgr.Register(std::move(actionStateGetGuidString));
	}

}
