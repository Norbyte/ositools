#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	FunctionHandle StatusIteratorEventHandle;

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

					auto osiris = gOsirisProxy->GetDynamicGlobals().OsirisObject;
					gOsirisProxy->GetWrappers().Event.CallOriginal(osiris, (uint32_t)StatusIteratorEventHandle, eventArgs);

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

		bool StatusGetAttributeString(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			if (strcmp(attributeName, "StatsId") == 0) {
				args.Get(3).String = status->StatusId.Str;
			}
			else {
				OsiError("Unknown attribute: " << attributeName);
				return false;
			}

			return true;
		}

		bool StatusGetAttributeGuidString(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			ObjectHandle handle;
			if (strcmp(attributeName, "StatusHandle") == 0) {
				handle = status->StatusHandle;
			}
			else if (strcmp(attributeName, "TargetCI") == 0) {
				handle = status->TargetCIHandle;
			}
			else if (strcmp(attributeName, "StatusSource") == 0) {
				handle = status->StatusSourceHandle;
			}
			else if (strcmp(attributeName, "Obj2") == 0) {
				handle = status->SomeHandle;
			}
			else {
				OsiError("Unknown attribute: " << attributeName);
				return false;
			}

			auto character = FindCharacterByHandle(handle);
			if (character == nullptr) {
				return false;
			}

			args.Get(3).String = character->MyGuid.Str;
			return true;
		}

		bool StatusGetAttributeReal(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			if (strcmp(attributeName, "StartTimer") == 0) {
				args.Get(3).Float = status->StartTimer;
			}
			else if (strcmp(attributeName, "LifeTime") == 0) {
				args.Get(3).Float = status->LifeTime;
			}
			else if (strcmp(attributeName, "CurrentLifeTime") == 0) {
				args.Get(3).Float = status->CurrentLifeTime;
			}
			else if (strcmp(attributeName, "TurnTimer") == 0) {
				args.Get(3).Float = status->TurnTimer;
			}
			else if (strcmp(attributeName, "Strength") == 0) {
				args.Get(3).Float = status->Strength;
			}
			else if (strcmp(attributeName, "StatsMultiplier") == 0) {
				args.Get(3).Float = status->StatsMultiplier;
			}
			else {
				OsiError("Unknown attribute: " << attributeName);
				return false;
			}

			return true;
		}

		bool StatusGetAttributeInt(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			if (strcmp(attributeName, "CanEnterChance") == 0) {
				args.Get(3).Int32 = status->CanEnterChance;
			}
			else if (strcmp(attributeName, "DamageSourceType") == 0) {
				args.Get(3).Int32 = status->DamageSourceType;
			}
			else if (strcmp(attributeName, "KeepAlive") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_KeepAlive) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsOnSourceSurface") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_IsOnSourceSurface) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsFromItem") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_IsFromItem) ? 1 : 0;
			}
			else if (strcmp(attributeName, "Channeled") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_Channeled) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsLifeTimeSet") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_IsLifeTimeSet) ? 1 : 0;
			}
			else if (strcmp(attributeName, "InitiateCombat") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_InitiateCombat) ? 1 : 0;
			}
			else if (strcmp(attributeName, "Influence") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_Influence) ? 1 : 0;
			}
			else if (strcmp(attributeName, "BringIntoCombat") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_BringIntoCombat) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsHostileAct") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_IsHostileAct) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsInvulnerable") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_IsInvulnerable) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsResistingDeath") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_IsResistingDeath) ? 1 : 0;
			}
			else if (strcmp(attributeName, "ForceStatus") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_ForceStatus) ? 1 : 0;
			}
			else if (strcmp(attributeName, "ForceFailStatus") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_ForceFailStatus) ? 1 : 0;
			}
			else if (strcmp(attributeName, "RequestDelete") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_RequestDelete) ? 1 : 0;
			}
			else if (strcmp(attributeName, "RequestDeleteAtTurnEnd") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_RequestDeleteAtTurnEnd) ? 1 : 0;
			}
			else if (strcmp(attributeName, "Started") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_Started) ? 1 : 0;
			}
			else {
				OsiError("Unknown attribute: " << attributeName);
				return false;
			}

			return true;
		}

		void StatusSetAttributeReal(OsiArgumentDesc const & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return;
			}

			auto attributeName = args.Get(2).String;
			auto value = args.Get(3).Float;

			if (value < 0.0f) {
				OsiError("Cannot set a negative value");
				return;
			}

			if (strcmp(attributeName, "LifeTime") == 0) {
				status->LifeTime = value;
				if (status->CurrentLifeTime > status->LifeTime) {
					status->CurrentLifeTime = status->LifeTime;
				}
			}
			else if (strcmp(attributeName, "CurrentLifeTime") == 0) {
				status->CurrentLifeTime = value;
				if (status->LifeTime < status->CurrentLifeTime) {
					status->LifeTime = status->CurrentLifeTime;
				}
			}
			else if (strcmp(attributeName, "Strength") == 0) {
				status->Strength = value;
			}
			else if (strcmp(attributeName, "StatsMultiplier") == 0) {
				status->StatsMultiplier = value;
			}
			else {
				OsiError("Cannot set attribute: " << attributeName);
			}
		}
	}

	void CustomFunctionLibrary::RegisterStatusFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto iterateCharacterStatuses = std::make_unique<CustomCall>(
			"NRD_IterateCharacterStatuses",
			std::vector<CustomFunctionParam>{
				{ "CharacterGuid", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Event", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::IterateCharacterStatuses
		);
		functionMgr.Register(std::move(iterateCharacterStatuses));

		auto getStatusHandle = std::make_unique<CustomQuery>(
			"NRD_StatusGetHandle",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusId", ValueType::String, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::StatusGetHandle
		);
		functionMgr.Register(std::move(getStatusHandle));

		auto getStatusAttributeInt = std::make_unique<CustomQuery>(
			"NRD_StatusGetAttributeInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttributeInt
		);
		functionMgr.Register(std::move(getStatusAttributeInt));

		auto getStatusAttributeReal = std::make_unique<CustomQuery>(
			"NRD_StatusGetAttributeReal",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttributeReal
		);
		functionMgr.Register(std::move(getStatusAttributeReal));

		auto getStatusAttributeString = std::make_unique<CustomQuery>(
			"NRD_StatusGetAttributeString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttributeString
		);
		functionMgr.Register(std::move(getStatusAttributeString));

		auto getStatusAttributeGuidString = std::make_unique<CustomQuery>(
			"NRD_StatusGetAttributeGuidString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::StatusGetAttributeGuidString
		);
		functionMgr.Register(std::move(getStatusAttributeGuidString));

		auto setStatusAttributeReal = std::make_unique<CustomCall>(
			"NRD_StatusSetAttributeReal",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::StatusSetAttributeReal
		);
		functionMgr.Register(std::move(setStatusAttributeReal));

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
	}

}
