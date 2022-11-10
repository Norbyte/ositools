#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <Extender/Server/OsirisStatusHelpers.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

BEGIN_NS(esv)

extern FunctionHandle StatusAttemptEventHandle;
extern FunctionHandle HitPrepareEventHandle;
extern FunctionHandle HitEventHandle;
extern FunctionHandle HealEventHandle;

LegacyPropertyMapBase & StatusToPropertyMap(esv::Status * status)
{
	switch (status->GetStatusId()) {
	case StatusType::HIT: return gStatusHitPropertyMap;
	case StatusType::DYING: return gStatusDyingPropertyMap;
	case StatusType::HEAL: return gStatusHealPropertyMap;
	case StatusType::CHARMED: return gStatusCharmedPropertyMap;
	case StatusType::KNOCKED_DOWN: return gStatusKnockedDownPropertyMap;
	case StatusType::SUMMONING: return gStatusSummoningPropertyMap;
	case StatusType::HEALING: return gStatusHealingPropertyMap;
	case StatusType::THROWN: return gStatusThrownPropertyMap;
	case StatusType::TELEPORT_FALLING: return gStatusTeleportFallPropertyMap;
	case StatusType::COMBAT: return gStatusCombatPropertyMap;
	case StatusType::AOO: return gStatusAoOPropertyMap;
	case StatusType::SNEAKING: return gStatusSneakingPropertyMap;
	case StatusType::UNLOCK: return gStatusUnlockPropertyMap;
	case StatusType::BOOST: return gStatusBoostPropertyMap;
	case StatusType::UNSHEATHED: return gStatusUnsheathedPropertyMap;
	case StatusType::STANCE: return gStatusStancePropertyMap;
	case StatusType::SITTING: return gStatusLyingPropertyMap;
	case StatusType::LYING: return gStatusLyingPropertyMap;
	case StatusType::INFECTIOUS_DISEASED: return gStatusInfectiousDiseasedPropertyMap;
	case StatusType::INVISIBLE: return gStatusInvisiblePropertyMap;
	case StatusType::ROTATE: return gStatusRotatePropertyMap;
	case StatusType::IDENTIFY: return gStatusIdentifyPropertyMap;
	case StatusType::REPAIR: return gStatusRepairPropertyMap;
	case StatusType::MATERIAL: return gStatusMaterialPropertyMap;
	case StatusType::EXPLODE: return gStatusExplodePropertyMap;
	case StatusType::ADRENALINE: return gStatusAdrenalinePropertyMap;
	case StatusType::SHACKLES_OF_PAIN: return gStatusShacklesOfPainPropertyMap;
	case StatusType::SHACKLES_OF_PAIN_CASTER: return gStatusShacklesOfPainCasterPropertyMap;
	case StatusType::DRAIN: return gStatusDrainPropertyMap;
	case StatusType::SPIRIT_VISION: return gStatusSpiritVisionPropertyMap;
	case StatusType::SPIRIT: return gStatusSpiritPropertyMap;
	case StatusType::DAMAGE: return gStatusDamagePropertyMap;
	case StatusType::CLIMBING: return gStatusClimbingPropertyMap;
	case StatusType::INCAPACITATED: return gStatusIncapacitatedPropertyMap;
	case StatusType::INSURFACE: return gStatusInSurfacePropertyMap;
	case StatusType::POLYMORPHED: return gStatusPolymorphedPropertyMap;
	case StatusType::DAMAGE_ON_MOVE: return gStatusDamageOnMovePropertyMap;
	case StatusType::CHALLENGE: return gStatusChallengePropertyMap;
	case StatusType::HEAL_SHARING: return gStatusHealSharingPropertyMap;
	case StatusType::HEAL_SHARING_CASTER: return gStatusHealSharingCasterPropertyMap;
	case StatusType::ACTIVE_DEFENSE: return gStatusActiveDefensePropertyMap;
	case StatusType::SPARK: return gStatusSparkPropertyMap;
	case StatusType::CONSTRAINED: return gStatusLyingPropertyMap;

	case StatusType::STORY_FROZEN:
	case StatusType::SMELLY:
	case StatusType::CLEAN:
	case StatusType::UNHEALABLE:
	case StatusType::FLANKED:
	case StatusType::INFUSED:
	case StatusType::SOURCE_MUTED:
	case StatusType::DEMONIC_BARGAIN:
	case StatusType::EFFECT:
	case StatusType::TUTORIAL_BED:
		return gStatusPropertyMap;

	case StatusType::MUTED:
	case StatusType::CONSUME:
	case StatusType::FEAR:
	case StatusType::BLIND:
	case StatusType::ENCUMBERED:
	case StatusType::LEADERSHIP:
	case StatusType::WIND_WALKER:
	case StatusType::DARK_AVENGER:
	case StatusType::REMORSE:
	case StatusType::DECAYING_TOUCH:
	case StatusType::CHANNELING:
	case StatusType::FORCE_MOVE:
	case StatusType::OVERPOWER:
	case StatusType::COMBUSTION:
	case StatusType::GUARDIAN_ANGEL:
	case StatusType::FLOATING:
	case StatusType::DISARMED:
	case StatusType::EXTRA_TURN:
	case StatusType::PLAY_DEAD:
	case StatusType::DEACTIVATED:
		return gStatusConsumeBasePropertyMap;

	default:
		OsiWarn("No property map available for unknown status type " << (unsigned)status->GetStatusId());
		return gStatusPropertyMap;
	}
}

esv::Status * OsirisStatusHelpers::GetStatus(char const* gameObjectGuid, ComponentHandle const& statusHandle)
{
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

std::optional<ComponentHandle> OsirisStatusHelpers::ApplyActiveDefense(esv::Character* character, FixedString const& statusId, float lifeTime)
{
	auto statusMachine = character->StatusMachine;
	if (!statusMachine) {
		OsiErrorS("Character has no StatusMachine!");
		return {};
	}

	auto applyStatus = GetStaticSymbols().esv__StatusMachine__ApplyStatus;
	if (applyStatus == nullptr) {
		OsiErrorS("esv::StatusMachine::ApplyStatus not found!");
		return {};
	}

	auto status = (esv::StatusActiveDefense *)gExtender->GetServer().GetStatusHelpers().ConstructStatus(statusMachine, statusId, StatusType::ACTIVE_DEFENSE);
	if (status == nullptr) {
		return {};
	}

	status->Flags0 |= esv::StatusFlags0::IsLifeTimeSet;
	status->LifeTime = lifeTime;
	status->CurrentLifeTime = lifeTime;

	ComponentHandle handle;
	character->GetObjectHandle(handle);

	status->StatusTargetHandle = handle;
	status->TargetPos = *character->GetTranslate();

	applyStatus(statusMachine, status);
	return status->StatusHandle;
}

std::optional<ComponentHandle> OsirisStatusHelpers::ApplyDamageOnMove(esv::Character* character, FixedString const& statusId, esv::Character* sourceCharacter,
	float lifeTime, float distancePerDamage)
{
	auto statusMachine = character->StatusMachine;
	if (!statusMachine) {
		OsiErrorS("Character has no StatusMachine!");
		return {};
	}

	if (distancePerDamage <= 0.01) {
		OsiErrorS("DistancePerDamage must be a positive value!");
		return {};
	}

	auto applyStatus = GetStaticSymbols().esv__StatusMachine__ApplyStatus;
	if (applyStatus == nullptr) {
		OsiErrorS("esv::StatusMachine::ApplyStatus not found!");
		return {};
	}

	auto status = (esv::StatusDamageOnMove *)gExtender->GetServer().GetStatusHelpers().ConstructStatus(statusMachine, statusId, StatusType::DAMAGE_ON_MOVE);
	if (status == nullptr) {
		return {};
	}

	status->Flags0 |= esv::StatusFlags0::IsLifeTimeSet;
	status->LifeTime = lifeTime;
	status->CurrentLifeTime = lifeTime;

	if (sourceCharacter == nullptr) {
		status->StatusSourceHandle = ComponentHandle{};
	} else {
		ComponentHandle sourceHandle;
		sourceCharacter->GetObjectHandle(sourceHandle);
		status->StatusSourceHandle = sourceHandle;
	}

	status->StartTimer = 0.0f;
	status->StatsMultiplier = 1.0f;
	status->DistancePerDamage = distancePerDamage;

	applyStatus(statusMachine, status);
	return status->StatusHandle;
}

void OsirisStatusHelpers::ThrowStatusHitEnter(esv::Status * status)
{
	auto statusHit = static_cast<esv::StatusHit *>(status);
	if ((bool)(statusHit->Hit.EffectFlags & stats::HitFlag::NoEvents)) {
		return;
	}

	auto target = GetEntityWorld()->GetGameObject(status->OwnerHandle);
	if (target == nullptr) {
		OsiErrorS("Status has no target?");
		return;
	}

	char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
	auto source = GetEntityWorld()->GetGameObject(status->StatusSourceHandle);
	if (source != nullptr) {
		sourceGuid = source->GetGuid()->GetString();
	}

	auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, *target->GetGuid() });
	eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
	eventArgs->Add(OsiArgumentValue{ (int32_t)statusHit->Hit.TotalDamageDone });
	eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

	gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(HitEventHandle, eventArgs);

	delete eventArgs;

	if (statusHit->Hit.DamageList.size() == 0) {
		stats::TDamagePair dummy;
		dummy.Amount = 0;
		dummy.DamageType = stats::DamageType::Physical;
		statusHit->Hit.DamageList.push_back(dummy);
	}
}


void OsirisStatusHelpers::ThrowStatusHealEnter(esv::StatusHeal* status)
{
	auto target = GetEntityWorld()->GetComponent<Character>(status->OwnerHandle);
	if (target == nullptr) {
		OsiErrorS("Status has no target?");
		return;
	}

	char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
	auto source = GetEntityWorld()->GetGameObject(status->StatusSourceHandle);
	if (source != nullptr) {
		sourceGuid = source->GetGuid()->GetString();
	}

	auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, *target->GetGuid() });
	eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
	eventArgs->Add(OsiArgumentValue{ (int32_t)status->HealAmount });
	eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });

	gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(HealEventHandle, eventArgs);

	delete eventArgs;
}


void OsirisStatusHelpers::ThrowCharacterHit(esv::Character * self, stats::Character * attackerStats,
	stats::Item * itemStats, stats::DamagePairList * damageList, stats::HitType hitType, bool noHitRoll,
	stats::HitDamageInfo * damageInfo, int forceReduceDurability, stats::PropertyList * skillProperties, stats::HighGroundBonus highGround,
	bool procWindWalker, stats::CriticalRoll criticalRoll, DamageHelpers& helper)
{
	if ((bool)(damageInfo->EffectFlags & stats::HitFlag::NoEvents)) {
		return;
	}

	char const * sourceGuid = "NULL_00000000-0000-0000-0000-000000000000";
	if (attackerStats != nullptr
		&& attackerStats->GameObject != nullptr) {
		sourceGuid = attackerStats->GameObject->GetGuid()->GetString();
	}

	int32_t totalDamage{ 0 };
	for (auto const& dmg : *damageList) {
		totalDamage += dmg.Amount;
	}

	auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, *self->GetGuid() });
	eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });
	eventArgs->Add(OsiArgumentValue{ (int32_t)totalDamage });
	eventArgs->Add(OsiArgumentValue{ (int64_t)helper.Handle.Handle });

	gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(HitPrepareEventHandle, eventArgs);

	delete eventArgs;
}


void OsirisStatusHelpers::ThrowApplyStatus(esv::StatusMachine * self, esv::Status * status)
{
	char const * targetGuid{ nullptr };
	auto target = GetEntityWorld()->GetGameObject(self->OwnerObjectHandle);
	if (target != nullptr) {
		targetGuid = target->GetGuid()->GetString();
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
			sourceGuid = source->GetGuid()->GetString();
		}
	}

	auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, targetGuid });
	eventArgs->Add(OsiArgumentValue{ ValueType::String, status->StatusId });
	eventArgs->Add(OsiArgumentValue{ (int64_t)status->StatusHandle });
	eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, sourceGuid });

	// FIXME - clean up Osi events
	gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(StatusAttemptEventHandle, eventArgs);

	delete eventArgs;
}

END_NS()
