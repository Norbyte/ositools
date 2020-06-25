#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace dse::esv
{
	FunctionHandle StatusIteratorEventHandle;
	FunctionHandle StatusAttemptEventHandle;
	FunctionHandle HitPrepareEventHandle;
	FunctionHandle HitEventHandle;
	FunctionHandle HealEventHandle;
	FunctionHandle ActionStateEnterHandle;

	esv::StatusMachine * GetStatusMachine(char const * gameObjectGuid)
	{
		auto character = GetEntityWorld()->GetCharacter(gameObjectGuid, false);
		if (character != nullptr) {
			return character->StatusMachine;
		}

		auto item = GetEntityWorld()->GetItem(gameObjectGuid, false);
		if (item != nullptr) {
			return item->StatusMachine;
		}

		OsiError("Character or item " << gameObjectGuid << " does not exist!");
		return nullptr;
	}

	PropertyMapBase & StatusToPropertyMap(esv::Status * status)
	{
		switch (status->GetStatusId()) {
		case StatusType::HIT:
			return gStatusHitPropertyMap;

		case StatusType::HEAL:
			return gStatusHealPropertyMap;

		case StatusType::HEALING:
			return gStatusHealingPropertyMap;

		case StatusType::CONSUME:
		case StatusType::DARK_AVENGER:
		case StatusType::DECAYING_TOUCH:
		case StatusType::REMORSE:
		case StatusType::WIND_WALKER:
		case StatusType::FLOATING:
		case StatusType::INCAPACITATED:
		case StatusType::POLYMORPHED:
		case StatusType::SHACKLES_OF_PAIN:
		case StatusType::SHACKLES_OF_PAIN_CASTER:
		case StatusType::CHARMED:
		case StatusType::EXTRA_TURN:
		case StatusType::DAMAGE:
		case StatusType::BLIND:
		case StatusType::ENCUMBERED:
		case StatusType::FEAR:
		case StatusType::INFECTIOUS_DISEASED:
		case StatusType::INVISIBLE:
		case StatusType::MUTED:
		case StatusType::STANCE:
		case StatusType::LEADERSHIP:
		case StatusType::ADRENALINE:
		case StatusType::LINGERING_WOUNDS:
		case StatusType::SPIRIT_VISION:
		case StatusType::OVERPOWER:
		case StatusType::COMBUSTION:
		case StatusType::GUARDIAN_ANGEL:
		case StatusType::CHALLENGE:
		case StatusType::DISARMED:
		case StatusType::HEAL_SHARING:
		case StatusType::HEAL_SHARING_CASTER:
		case StatusType::ACTIVE_DEFENSE:
		case StatusType::SPARK:
		case StatusType::PLAY_DEAD:
		case StatusType::DEACTIVATED:
			return gStatusConsumePropertyMap;

		default:
			return gStatusPropertyMap;
		}
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
				eventArgs->Add(OsiArgumentValue{ ValueType::String, status->StatusId.Str });
				eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

				gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(StatusIteratorEventHandle, eventArgs);

				delete eventArgs;
			}
		}

		esv::Status * GetStatusHelper(OsiArgumentDesc const & args)
		{
			auto gameObjectGuid = args[0].String;
			ObjectHandle statusHandle{ args[1].Int64 };

			if (statusHandle.GetType() == DamageHelpers::HitHandleTypeId) {
				OsiError("Attempted to use a hit handle in a status function.");
				OsiError("For handles received from NRD_OnPrepareHit/NRD_PrepareHit use the NRD_HitGet... functions instead!");
				return nullptr;
			}

			esv::Status * status{ nullptr };
			auto character = GetEntityWorld()->GetCharacter(gameObjectGuid, false);
			if (character != nullptr) {
				status = character->GetStatus(statusHandle, true);
			} else {
				auto item = GetEntityWorld()->GetItem(gameObjectGuid, false);
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

		bool ObjectHasStatusType(OsiArgumentDesc& args)
		{
			auto gameObjectGuid = args[0].String;
			auto statusType = args[1].String;
			auto& hasStatus = args[2];

			auto statusMachine = GetStatusMachine(gameObjectGuid);
			if (statusMachine == nullptr) return false;
			
			auto typeId = EnumInfo<StatusType>::Find(statusType);
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

			auto statusIdFS = ToFixedString(args[1].String);
			if (!statusIdFS) {
				// No fixed string with this ID --> invalid status name
				OsiWarn("Status " << args[1].String << " not in string table, possibly invalid status name?");
				return false;
			}

			for (auto const status : statusMachine->Statuses) {
				if (status->StatusId == statusIdFS) {
					args[2].Set((int64_t)status->StatusHandle);
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
				case StatusType::HIT:
				{
					auto hit = static_cast<esv::StatusHit *>(status);
					if (OsirisPropertyMapGet(gHitDamageInfoPropertyMap, &hit->DamageInfo, args, 2, Type, false)) {
						return true;
					} else {
						return OsirisPropertyMapGet(gStatusHitPropertyMap, hit, args, 2, Type);
					}
				}

				case StatusType::HEAL:
				{
					auto heal = static_cast<esv::StatusHeal *>(status);
					return OsirisPropertyMapGet(gStatusHealPropertyMap, heal, args, 2, Type);
				}

				case StatusType::HEALING:
				{
					auto healing = static_cast<esv::StatusHealing *>(status);
					return OsirisPropertyMapGet(gStatusHealingPropertyMap, healing, args, 2, Type);
				}

				case StatusType::CONSUME:
				case StatusType::DARK_AVENGER:
				case StatusType::DECAYING_TOUCH:
				case StatusType::REMORSE:
				case StatusType::WIND_WALKER:
				case StatusType::FLOATING:
				case StatusType::INCAPACITATED:
				case StatusType::POLYMORPHED:
				case StatusType::SHACKLES_OF_PAIN:
				case StatusType::SHACKLES_OF_PAIN_CASTER:
				case StatusType::CHARMED:
				case StatusType::EXTRA_TURN:
				case StatusType::DAMAGE:
				case StatusType::BLIND:
				case StatusType::ENCUMBERED:
				case StatusType::FEAR:
				case StatusType::INFECTIOUS_DISEASED:
				case StatusType::INVISIBLE:
				case StatusType::MUTED:
				case StatusType::STANCE:
				case StatusType::LEADERSHIP:
				case StatusType::ADRENALINE:
				case StatusType::LINGERING_WOUNDS:
				case StatusType::SPIRIT_VISION:
				case StatusType::OVERPOWER:
				case StatusType::COMBUSTION:
				case StatusType::GUARDIAN_ANGEL:
				case StatusType::CHALLENGE:
				case StatusType::DISARMED:
				case StatusType::HEAL_SHARING:
				case StatusType::HEAL_SHARING_CASTER:
				case StatusType::ACTIVE_DEFENSE:
				case StatusType::SPARK:
				case StatusType::PLAY_DEAD:
				case StatusType::DEACTIVATED:
				{
					auto consume = static_cast<esv::StatusConsume *>(status);
					return OsirisPropertyMapGet(gStatusConsumePropertyMap, consume, args, 2, Type);
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
				case StatusType::HIT:
				{
					auto hit = static_cast<esv::StatusHit *>(status);
					if (!OsirisPropertyMapSet(gHitDamageInfoPropertyMap, &hit->DamageInfo, args, 2, Type, false)) {
						OsirisPropertyMapSet(gStatusHitPropertyMap, hit, args, 2, Type);
					}
					break;
				}

				case StatusType::HEAL:
				{
					auto heal = static_cast<esv::StatusHeal *>(status);
					OsirisPropertyMapSet(gStatusHealPropertyMap, heal, args, 2, Type);
					break;
				}

				case StatusType::HEALING:
				{
					auto healing = static_cast<esv::StatusHealing *>(status);
					OsirisPropertyMapSet(gStatusHealingPropertyMap, healing, args, 2, Type);
					break;
				}

				case StatusType::CONSUME:
				case StatusType::DARK_AVENGER:
				case StatusType::DECAYING_TOUCH:
				case StatusType::REMORSE:
				case StatusType::WIND_WALKER:
				case StatusType::FLOATING:
				case StatusType::INCAPACITATED:
				case StatusType::POLYMORPHED:
				case StatusType::SHACKLES_OF_PAIN:
				case StatusType::SHACKLES_OF_PAIN_CASTER:
				case StatusType::CHARMED:
				case StatusType::EXTRA_TURN:
				case StatusType::DAMAGE:
				case StatusType::BLIND:
				case StatusType::ENCUMBERED:
				case StatusType::FEAR:
				case StatusType::INFECTIOUS_DISEASED:
				case StatusType::INVISIBLE:
				case StatusType::MUTED:
				case StatusType::STANCE:
				case StatusType::LEADERSHIP:
				case StatusType::ADRENALINE:
				case StatusType::LINGERING_WOUNDS:
				case StatusType::SPIRIT_VISION:
				case StatusType::OVERPOWER:
				case StatusType::COMBUSTION:
				case StatusType::GUARDIAN_ANGEL:
				case StatusType::CHALLENGE:
				case StatusType::DISARMED:
				case StatusType::HEAL_SHARING:
				case StatusType::HEAL_SHARING_CASTER:
				case StatusType::ACTIVE_DEFENSE:
				case StatusType::SPARK:
				case StatusType::PLAY_DEAD:
				case StatusType::DEACTIVATED:
				{
					auto consume = static_cast<esv::StatusConsume *>(status);
					OsirisPropertyMapSet(gStatusConsumePropertyMap, consume, args, 2, Type);
					break;
				}

				default:
					OsirisPropertyMapSet(gStatusPropertyMap, status, args, 2, Type);
					break;
			}
		}

		void StatusPreventApply(OsiArgumentDesc const & args)
		{
			auto gameObject = GetEntityWorld()->GetGameObject(args[0].String);
			auto statusHandle = ObjectHandle{ args[1].Int64 };
			auto preventApply = args[2].Int32;

			if (gameObject == nullptr) {
				OsiError("Game object " << args[0].String << " does not exist!");
				return;
			}

			ObjectHandle gameObjectHandle;
			gameObject->GetObjectHandle(gameObjectHandle);

			auto status = ExtensionState::Get().PendingStatuses.Find(gameObjectHandle, statusHandle);
			if (status == nullptr) {
				OsiError("No pending status found with handle " << (int64_t)statusHandle);
				return;
			}

			status->PreventApply = (preventApply != 0);
		}

		template <class T>
		T * ConstructStatus(esv::StatusMachine * statusMachine, char const * statusId, StatusType type)
		{
			auto statusIdFs = ToFixedString(statusId);
			T * status{ nullptr };

			auto createStatus = GetStaticSymbols().StatusMachineCreateStatus;
			if (createStatus == nullptr) {
				OsiErrorS("esv::StatusMachine::CreateStatus not found!");
				return nullptr;
			}

			if (statusIdFs) {
				status = (T *)createStatus(statusMachine, statusIdFs, 0);
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
			auto characterGuid = args[0].String;
			auto statusId = args[1].String;
			auto lifeTime = args[2].Float;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) {
				OsiError("Character " << characterGuid << " does not exist!");
				return false;
			}

			auto statusMachine = character->StatusMachine;
			if (!statusMachine) {
				OsiErrorS("Character has no StatusMachine!");
				return false;
			}

			auto applyStatus = GetStaticSymbols().StatusMachineApplyStatus;
			if (applyStatus == nullptr) {
				OsiErrorS("esv::StatusMachine::ApplyStatus not found!");
				return false;
			}

			auto status = ConstructStatus<esv::StatusActiveDefense>(statusMachine, statusId, StatusType::ACTIVE_DEFENSE);
			if (status == nullptr) {
				return false;
			}
			
			if (lifeTime < 0.0f) {
				status->Flags0 |= esv::StatusFlags0::KeepAlive;
				status->CurrentLifeTime = 1.0f;
			} else {
				status->Flags0 |= esv::StatusFlags0::IsLifeTimeSet;
				status->LifeTime = lifeTime;
				status->CurrentLifeTime = lifeTime;
			}

			ObjectHandle handle;
			character->GetObjectHandle(handle);

			status->TargetHandle = handle;
			status->TargetPos = *character->GetTranslate();

			applyStatus(statusMachine, status);
			args[3].Set((int64_t)status->StatusHandle);
			return true;
		}


		bool ApplyDamageOnMove(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto statusId = args[1].String;
			auto sourceCharacterGuid = args[2].String;
			auto lifeTime = args[3].Float;
			auto distancePerDamage = args[4].Float;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) {
				OsiError("Character " << characterGuid << " does not exist!");
				return false;
			}

			auto statusMachine = character->StatusMachine;
			if (!statusMachine) {
				OsiErrorS("Character has no StatusMachine!");
				return false;
			}

			if (distancePerDamage <= 0.01) {
				OsiErrorS("DistancePerDamage must be a positive value!");
				return false;
			}

			auto applyStatus = GetStaticSymbols().StatusMachineApplyStatus;
			if (applyStatus == nullptr) {
				OsiErrorS("esv::StatusMachine::ApplyStatus not found!");
				return false;
			}

			auto status = ConstructStatus<esv::StatusDamageOnMove>(statusMachine, statusId, StatusType::DAMAGE_ON_MOVE);
			if (status == nullptr) {
				return false;
			}

			if (lifeTime < 0.0f) {
				status->Flags0 |= esv::StatusFlags0::KeepAlive;
				status->CurrentLifeTime = 1.0f;
			} else {
				status->Flags0 |= esv::StatusFlags0::IsLifeTimeSet;
				status->LifeTime = lifeTime;
				status->CurrentLifeTime = lifeTime;
			}

			auto sourceCharacter = GetEntityWorld()->GetCharacter(sourceCharacterGuid);
			if (sourceCharacter == nullptr) {
				status->StatusSourceHandle = ObjectHandle{};
			} else {
				ObjectHandle sourceHandle;
				sourceCharacter->GetObjectHandle(sourceHandle);
				status->StatusSourceHandle = sourceHandle;
			}

			status->StartTimer = 0.0f;
			status->StatsMultiplier = 1.0f;
			status->DistancePerDamage = distancePerDamage;

			applyStatus(statusMachine, status);
			args[5].Set((int64_t)status->StatusHandle);
			return true;
		}



		template <OsiPropertyMapType Type>
		bool ActionStateGet(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr
				|| character->ActionMachine == nullptr
				|| character->ActionMachine->Layers[0].State == nullptr) {
				return false;
			}

			auto state = character->ActionMachine->Layers[0].State;
			auto propertyMap = state->GetPropertyMap();

			if (state->GetType() == ActionStateType::ASUseSkill) {
				auto useSkill = static_cast<ASUseSkill*>(state);
				if (useSkill->Skill != nullptr) {
					if (OsirisPropertyMapGetRaw(gSkillStatePropertyMap, useSkill->Skill, args, 1, Type, false)) {
						return true;
					}
				}
			}

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

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
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


	int32_t CustomFunctionLibrary::OnStatusGetEnterChance(esv::Status__GetEnterChance wrappedGetEnterChance,
		esv::Status * status, bool isEnterCheck)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			auto enterChance = lua->StatusGetEnterChance(status, isEnterCheck);
			if (enterChance) {
				return *enterChance;
			}
		}

		return wrappedGetEnterChance(status, isEnterCheck);
	}

	int32_t CustomFunctionLibrary::OnGetHitChance(CDivinityStats_Character__GetHitChance * wrappedGetHitChance,
		CDivinityStats_Character * attacker, CDivinityStats_Character * target)
	{
		LuaVirtualPin lua(gOsirisProxy->GetCurrentExtensionState());
		if (lua) {
			auto hitChance = lua->GetHitChance(attacker, target);
			if (hitChance) {
				return *hitChance;
			}
		}

		return wrappedGetHitChance(attacker, target);
	}

	void CustomFunctionLibrary::ThrowStatusHitEnter(esv::Status * status)
	{
		auto statusHit = static_cast<esv::StatusHit *>(status);
		if ((bool)(statusHit->DamageInfo.EffectFlags & HitFlag::NoEvents)) {
			return;
		}

		auto target = GetEntityWorld()->GetGameObject(status->TargetHandle);
		if (target == nullptr) {
			OsiErrorS("Status has no target?");
			return;
		}

		char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
		auto source = GetEntityWorld()->GetCharacter(status->StatusSourceHandle);
		if (source != nullptr) {
			sourceGuid = source->GetGuid()->Str;
		}

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, target->GetGuid()->Str });
		eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
		eventArgs->Add(OsiArgumentValue{ (int32_t)statusHit->DamageInfo.TotalDamage });
		eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

		gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(HitEventHandle, eventArgs);

		delete eventArgs;

		if (statusHit->DamageInfo.DamageList.Size == 0) {
			TDamagePair dummy;
			dummy.Amount = 0;
			dummy.DamageType = DamageType::Physical;
			statusHit->DamageInfo.DamageList.SafeAdd(dummy);
		}
	}


	void CustomFunctionLibrary::OnStatusHealEnter(esv::Status * status)
	{
		auto statusHeal = static_cast<esv::StatusHeal *>(status);

		auto target = GetEntityWorld()->GetCharacter(status->TargetHandle);
		if (target == nullptr) {
			OsiErrorS("Status has no target?");
			return;
		}

		char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
		auto source = GetEntityWorld()->GetCharacter(status->StatusSourceHandle);
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


	void CustomFunctionLibrary::ThrowCharacterHit(esv::Character * self, CDivinityStats_Character * attackerStats,
		CDivinityStats_Item * itemStats, DamagePairList * damageList, HitType hitType, bool noHitRoll,
		HitDamageInfo * damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List * skillProperties, HighGroundBonus highGround,
		bool procWindWalker, CriticalRoll criticalRoll, DamageHelpers& helper)
	{
		if ((bool)(damageInfo->EffectFlags & HitFlag::NoEvents)) {
			return;
		}

		char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
		if (attackerStats != nullptr
			&& attackerStats->Character != nullptr) {
			sourceGuid = attackerStats->Character->GetGuid()->Str;
		}

		int32_t totalDamage{ 0 };
		for (auto const& dmg : *damageList) {
			totalDamage += dmg.Amount;
		}

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, self->GetGuid()->Str });
		eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
		eventArgs->Add(OsiArgumentValue{ (int32_t)totalDamage });
		eventArgs->Add(OsiArgumentValue{ (int64_t)helper.Handle.Handle });

		gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(HitPrepareEventHandle, eventArgs);

		delete eventArgs;
	}


	void CustomFunctionLibrary::ThrowApplyStatus(esv::StatusMachine * self, esv::Status * status)
	{
		char const * targetGuid{ nullptr };
		auto target = GetEntityWorld()->GetGameObject(self->OwnerObjectHandle);
		if (target != nullptr) {
			targetGuid = target->GetGuid()->Str;
		} else {
			OsiErrorS("Can't throw ApplyStatus event - target handle could not be resolved.");
		}

		if (targetGuid == nullptr) {
			return;
		}

		char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
		if (status->StatusSourceHandle) {
			auto source = GetEntityWorld()->GetGameObject(status->StatusSourceHandle);
			if (source != nullptr) {
				sourceGuid = source->GetGuid()->Str;
			}
		}

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, targetGuid });
		eventArgs->Add(OsiArgumentValue{ ValueType::String, status->StatusId.Str });
		eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });
		eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });

		gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(StatusAttemptEventHandle, eventArgs);

		delete eventArgs;
	}

	void CustomFunctionLibrary::OnActionMachineSetState(esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer, bool succeeded)
	{
		if (!succeeded || actionState == nullptr || !setLayer) return;

		auto character = GetEntityWorld()->GetCharacter(self->CharacterHandle);
		if (character == nullptr) {
			OsiErrorS("ActionMachine has no character?");
			return;
		}

		auto type = actionState->GetType();
		auto typeName = actionState->GetTypeName();
		// Avoid event spam from idle/anim states
		if (type == esv::ActionStateType::ASIdle
			|| type == esv::ActionStateType::ASAnimation
			|| typeName == nullptr) {
			return;
		}

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, character->GetGuid()->Str });
		eventArgs->Add(OsiArgumentValue{ ValueType::String, typeName });

		gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(ActionStateEnterHandle, eventArgs);

		delete eventArgs;
	}

	void CustomFunctionLibrary::OnSkillFormatDescriptionParam(SkillPrototype::FormatDescriptionParam next, SkillPrototype *skillPrototype,
		CDivinityStats_Character *tgtCharStats, eoc::Text *eocText, int paramIndex, bool isFromItem,
		float xmm9_4_0, FixedString * paramText, ObjectSet<STDString> * paramTexts)
	{
		// When fetching subproperties (recursively), paramTexts will be null.
		// We won't post these to Lua since the Lua scripts already processed the original (unwrapped) query
		if (paramTexts != nullptr) {
			ecl::LuaClientPin lua(ecl::ExtensionState::Get());
			if (lua) {
				auto replacement = lua->SkillGetDescriptionParam(skillPrototype, tgtCharStats, *paramTexts, isFromItem);
				if (replacement) {
					eocText->ReplaceParam(paramIndex, *replacement);
					return;
				}
			}
		}

		next(skillPrototype, tgtCharStats, eocText, paramIndex, isFromItem, xmm9_4_0, paramText, paramTexts);
	}

	void CustomFunctionLibrary::OnStatusFormatDescriptionParam(StatusPrototype::FormatDescriptionParam next, StatusPrototype *prototype,
		CRPGStats_ObjectInstance* owner, CRPGStats_ObjectInstance* statusSource, float multiplier,
		eoc::Text * text, int paramIndex, FixedString * param, ObjectSet<STDString> * paramSet)
	{
		ecl::LuaClientPin lua(ecl::ExtensionState::Get());
		if (lua) {
			auto replacement = lua->StatusGetDescriptionParam(prototype, owner, statusSource, *paramSet);
			if (replacement) {
				text->ReplaceParam(paramIndex, *replacement);
				return;
			}
		}

		next(prototype, owner, statusSource, multiplier, text, paramIndex, param, paramSet);
	}

	void CustomFunctionLibrary::OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			lua->OnUpdateTurnOrder(self, combatId);
		}
	}


	void CustomFunctionLibrary::OnCreateUIObject(UIObjectManager* self, ObjectHandle* handle, unsigned int layerIndex,
		unsigned int creatorId, int flags, uint64_t resourceFlags, uint16_t playerId, ObjectHandle* result)
	{
		ecl::LuaClientPin lua(ecl::ExtensionState::Get());
		if (lua) {
			lua->OnCreateUIObject(*result);
		}
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
			&func::StatusGetAttribute<OsiPropertyMapType::Integer>
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
			&func::StatusGetAttribute<OsiPropertyMapType::Real>
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
			&func::StatusGetAttribute<OsiPropertyMapType::String>
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
			&func::StatusGetAttribute<OsiPropertyMapType::GuidStringHandle>
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
			&func::StatusSetAttribute<OsiPropertyMapType::Integer>
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
			&func::StatusSetAttribute<OsiPropertyMapType::Real>
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
			&func::StatusSetAttribute<OsiPropertyMapType::String>
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
			&func::StatusSetAttribute<OsiPropertyMapType::GuidStringHandle>
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
			&func::StatusSetAttribute<OsiPropertyMapType::Vector3>
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
