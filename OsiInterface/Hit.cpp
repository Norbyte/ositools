#include <stdafx.h>
#include <Hit.h>
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace dse::esv
{
	extern FunctionHandle StatusAttemptEventHandle;
	extern FunctionHandle HitPrepareEventHandle;
	extern FunctionHandle HitEventHandle;
	

	HitProxy::HitProxy(OsirisProxy& osiris)
		: osiris_(osiris)
	{}


	void HitProxy::PostStartup()
	{
		if (!gOsirisProxy->HasFeatureFlag("OsirisExtensions")) {
			return;
		}

		if (PostLoaded) {
			return;
		}

		using namespace std::placeholders;

		osiris_.GetLibraryManager().StatusHitEnter.SetPreHook(
			std::bind(&HitProxy::OnStatusHitEnter, this, _1)
		);
		osiris_.GetLibraryManager().CharacterHitHook.SetWrapper(
			std::bind(&HitProxy::OnCharacterHit, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)
		);
		osiris_.GetLibraryManager().CharacterHitInternalHook.SetWrapper(
			std::bind(&HitProxy::OnCharacterHitInternal, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)
		);
		osiris_.GetLibraryManager().CharacterApplyDamageHook.SetWrapper(
			std::bind(&HitProxy::OnCharacterApplyDamage, this, _1, _2, _3, _4, _5, _6)
		);
		osiris_.GetLibraryManager().ApplyStatusHook.SetWrapper(
			std::bind(&HitProxy::OnApplyStatus, this, _1, _2, _3)
		);
		osiris_.GetLibraryManager().SkillPrototypeGetSkillDamageHook.SetWrapper(
			std::bind(&HitProxy::OnGetSkillDamage, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11)
		);

		PostLoaded = true;
	}


	void HitProxy::OnStatusHitEnter(esv::Status* status)
	{
		gOsirisProxy->GetFunctionLibrary().ThrowStatusHitEnter(status);
	}


	void HitProxy::OnCharacterHit(esv::Character::HitProc wrappedHit, esv::Character* self, CDivinityStats_Character* attackerStats,
		CDivinityStats_Item* itemStats, DamagePairList* damageList, HitType hitType, bool noHitRoll,
		HitDamageInfo* damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List* skillProperties, HighGroundBonus highGround,
		bool procWindWalker, CriticalRoll criticalRoll)
	{
		auto helper = gOsirisProxy->GetServerExtensionState().DamageHelpers.Create();
		helper->Type = DamageHelpers::HT_PrepareHitEvent;
		helper->Target = self;
		if (attackerStats != nullptr) {
			helper->Source = static_cast<esv::Character*>(attackerStats->Character);
		}

		// TODO - allow disabling SimulateHit & not call the original func?
		helper->SimulateHit = true;
		helper->HitType = hitType;
		helper->NoHitRoll = noHitRoll;
		helper->ProcWindWalker = procWindWalker;
		helper->HighGround = highGround;
		helper->CriticalRoll = criticalRoll;
		helper->ForceReduceDurability = (bool)forceReduceDurability;
		helper->SetExternalDamageInfo(damageInfo, damageList);

		gOsirisProxy->GetFunctionLibrary().ThrowCharacterHit(self, attackerStats, itemStats, damageList, hitType, noHitRoll,
			damageInfo, forceReduceDurability, skillProperties, highGround, procWindWalker, criticalRoll, *helper);

		wrappedHit(self, attackerStats, itemStats, damageList, helper->HitType, helper->NoHitRoll,
			damageInfo, helper->ForceReduceDurability, skillProperties, helper->HighGround,
			helper->ProcWindWalker, helper->CriticalRoll);

		gOsirisProxy->GetServerExtensionState().DamageHelpers.Destroy(helper->Handle);
	}

	void HitProxy::OnCharacterHitInternal(CDivinityStats_Character::HitInternalProc next, CDivinityStats_Character* self,
		CDivinityStats_Character* attackerStats, CDivinityStats_Item* item, DamagePairList* damageList, HitType hitType, bool noHitRoll,
		bool forceReduceDurability, HitDamageInfo* damageInfo, CRPGStats_Object_Property_List* skillProperties,
		HighGroundBonus highGroundFlag, CriticalRoll criticalRoll)
	{
		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			if (lua->ComputeCharacterHit(self, attackerStats, item, damageList, hitType, noHitRoll, forceReduceDurability, damageInfo,
				skillProperties, highGroundFlag, criticalRoll)) {
				return;
			}
		}

		next(self, attackerStats, item, damageList, hitType, noHitRoll, forceReduceDurability, damageInfo,
			skillProperties, highGroundFlag, criticalRoll);
	}


	void HitProxy::OnCharacterApplyDamage(esv::Character::ApplyDamageProc next, esv::Character* self, HitDamageInfo& hit,
		uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection)
	{
		HitDamageInfo luaHit = hit;

		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			if (lua->OnCharacterApplyDamage(self, luaHit, ObjectHandle(attackerHandle), causeType, impactDirection)) {
				return;
			}
		}

		next(self, luaHit, attackerHandle, causeType, impactDirection);
	}

	void HitProxy::OnApplyStatus(esv::StatusMachine__ApplyStatus wrappedApply, esv::StatusMachine* self, esv::Status* status)
	{
		// Don't throw events for inactive status machines, as those will get swallowed
		// by Osiris during loading anyway.
		if (!self->IsStatusMachineActive) {
			wrappedApply(self, status);
			return;
		}

		ExtensionState::Get().PendingStatuses.Add(status);

		gOsirisProxy->GetFunctionLibrary().ThrowApplyStatus(self, status);

		bool previousPreventApplyState = self->PreventStatusApply;
		ObjectHandle targetHandle;
		auto target = GetEntityWorld()->GetGameObject(self->OwnerObjectHandle);
		if (target != nullptr) {
			target->GetObjectHandle(targetHandle);

			auto pendingStatus = ExtensionState::Get().PendingStatuses.Find(targetHandle, status->StatusHandle);
			if (pendingStatus != nullptr) {
				self->PreventStatusApply = pendingStatus->PreventApply;
			} else {
				OsiErrorS("Status not found in pending status list during ApplyStatus ?!");
			}
		}

		wrappedApply(self, status);

		self->PreventStatusApply = previousPreventApplyState;
		ExtensionState::Get().PendingStatuses.Remove(status);
	}


	void HitProxy::OnGetSkillDamage(SkillPrototype::GetSkillDamage next, SkillPrototype* self, DamagePairList* damageList,
		CRPGStats_ObjectInstance* attackerStats, bool isFromItem, bool stealthed, float* attackerPosition,
		float* targetPosition, DeathType* pDeathType, int level, bool noRandomization)
	{
		LuaVirtualPin lua(gOsirisProxy->GetCurrentExtensionState());
		if (lua) {
			if (lua->GetSkillDamage(self, damageList, attackerStats, isFromItem, stealthed, attackerPosition, targetPosition, pDeathType, level, noRandomization)) {
				return;
			}
		}

		next(self, damageList, attackerStats, isFromItem, stealthed, attackerPosition, targetPosition, pDeathType, level, noRandomization);
	}
}
