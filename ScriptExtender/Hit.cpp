#include <stdafx.h>
#include <Hit.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

// #define DEBUG_HIT DEBUG
// #define WARN_HIT WARN
#define DEBUG_HIT(...)
#define WARN_HIT(...)

namespace dse::esv
{
	extern FunctionHandle StatusAttemptEventHandle;
	extern FunctionHandle HitPrepareEventHandle;
	extern FunctionHandle HitEventHandle;


	PendingHit* PendingHitManager::OnCharacterHit(esv::Character* character, stats::Character* attacker,
		stats::Item* weapon, stats::DamagePairList* damageList, stats::HitType hitType, bool noHitRoll,
		stats::HitDamageInfo* damageInfo, int forceReduceDurability, stats::HighGroundBonus highGround,
		bool procWindWalker, stats::CriticalRoll criticalRoll)
	{
		auto it = characterHitMap_.find(damageInfo);
		if (it != characterHitMap_.end()) {
			if (it->second->CapturedStatusSetup || it->second->CapturedStatusEnter) {
				WARN_HIT("PendingHitManager::OnCharacterHit(Hit=%p): Hit pointer reuse on %d", damageInfo, it->second->Id);
				it->second->CharacterHitPointer = nullptr;
			} else {
				WARN_HIT("PendingHitManager::OnCharacterHit(Hit=%p): Found duplicate hit with ID %d, deleting", damageInfo, it->second->Id);
				DeleteHit(it->second);
			}
		}

		auto hit = std::make_unique<PendingHit>();
		hit->Id = nextHitId_++;

		DEBUG_HIT("PendingHitManager::OnCharacterHit(Hit=%p): Constructing new hit %d", damageInfo, hit->Id);

		ComponentHandle targetHandle, attackerHandle;
		character->GetObjectHandle(targetHandle);
		hit->TargetHandle = targetHandle;

		if (attacker != nullptr && attacker->GameObject != nullptr) {
			attacker->GameObject->GetObjectHandle(attackerHandle);
			hit->AttackerHandle = attackerHandle;
		}

		hit->CapturedCharacterHit = true;
		hit->WeaponStats = weapon;
		hit->CharacterHitPointer = damageInfo;
		hit->CharacterHitDamageList = *damageList;
		hit->CharacterHit.CopyFrom(*damageInfo);
		hit->HitType = hitType;
		hit->NoHitRoll = noHitRoll;
		hit->ForceReduceDurability = forceReduceDurability;
		hit->HighGround = highGround;
		hit->ProcWindWalker = procWindWalker;
		hit->CriticalRoll = criticalRoll;

		auto pHit = hit.get();
		hits_.insert(std::make_pair(hit->Id, std::move(hit)));
		characterHitMap_.insert(std::make_pair(damageInfo, pHit));
		return pHit;
	}

	PendingHit* PendingHitManager::OnStatusHitSetup(esv::StatusHit* status, stats::HitDamageInfo* damageInfo)
	{
		PendingHit* pHit;
		auto it = characterHitMap_.find(damageInfo);
		if (it != characterHitMap_.end()) {
			pHit = it->second;
			DEBUG_HIT("PendingHitManager::OnStatusHitSetup(S=%p, Hit=%p): Mapped to existing %d", status, damageInfo, pHit->Id);
		} else {
			auto hit = std::make_unique<PendingHit>();
			hit->Id = nextHitId_++;

			pHit = hit.get();
			hits_.insert(std::make_pair(hit->Id, std::move(hit)));
			WARN_HIT("PendingHitManager::OnStatusHitSetup(S=%p, Hit=%p): Create new %d", status, damageInfo, pHit->Id);
		}

		pHit->CapturedStatusSetup = true;
		pHit->Status = status;
		hitStatusMap_.insert(std::make_pair(status, pHit));

		// We no longer need to keep character hit mappings
		if (pHit->CapturedCharacterHit && pHit->CharacterHitPointer != nullptr) {
			auto it = characterHitMap_.find(pHit->CharacterHitPointer);
			if (it != characterHitMap_.end() && it->second == pHit) {
				characterHitMap_.erase(it);
			}

			pHit->CharacterHitPointer = nullptr;
		}

		return pHit;
	}

	PendingHit* PendingHitManager::OnApplyHit(esv::StatusMachine* self, esv::StatusHit* status)
	{
		PendingHit* pHit;
		auto it = hitStatusMap_.find(status);
		if (it != hitStatusMap_.end()) {
			pHit = it->second;
			DEBUG_HIT("PendingHitManager::OnApplyHit(S=%p): Mapped to existing %d", status, &status->Hit, pHit->Id);
		} else {
			auto hit = std::make_unique<PendingHit>();
			hit->Id = nextHitId_++;

			pHit = hit.get();
			hits_.insert(std::make_pair(hit->Id, std::move(hit)));
			WARN_HIT("PendingHitManager::OnStatusHitEnter(S=%p): Create new %d", status, &status->Hit, pHit->Id);
		}

		pHit->CapturedStatusApply = true;
		pHit->TargetHandle = self->OwnerObjectHandle;
		pHit->Status = status;
		hitStatusDamageMap_.insert(std::make_pair(&status->Hit, pHit));

		return pHit;
	}

	PendingHit* PendingHitManager::OnStatusHitEnter(esv::StatusHit* status)
	{
		PendingHit* pHit;
		auto it = hitStatusMap_.find(status);
		if (it != hitStatusMap_.end()) {
			pHit = it->second;
			DEBUG_HIT("PendingHitManager::OnStatusHitEnter(S=%p, Hit=%p): Mapped to existing %d", status, &status->Hit, pHit->Id);
		} else {
			auto hit = std::make_unique<PendingHit>();
			hit->Id = nextHitId_++;

			pHit = hit.get();
			hits_.insert(std::make_pair(hit->Id, std::move(hit)));
			WARN_HIT("PendingHitManager::OnStatusHitEnter(S=%p, Hit=%p): Create new %d", status, &status->Hit, pHit->Id);
		}

		pHit->CapturedStatusEnter = true;
		pHit->Status = status;
		hitStatusDamageMap_.insert(std::make_pair(&status->Hit, pHit));

		return pHit;
	}

	void PendingHitManager::OnStatusHitDestroy(esv::StatusHit* status)
	{
		auto it = hitStatusMap_.find(status);
		if (it != hitStatusMap_.end()) {
			DEBUG_HIT("PendingHitManager::OnStatusHitEnter(S=%p): Deleting hit %d", status, it->second->Id);
			DeleteHit(it->second);
		} else {
			WARN_HIT("PendingHitManager::OnStatusHitEnter(S=%p): Hit not tracked!", status);
		}

	}

	PendingHit* PendingHitManager::OnCharacterApplyDamage(stats::HitDamageInfo* hit)
	{
		auto it = hitStatusDamageMap_.find(hit);
		if (it != hitStatusDamageMap_.end()) {
			DEBUG_HIT("PendingHitManager::OnCharacterApplyDamage(Hit=%p): Mapped to existing %d", hit, it->second->Id);
			return it->second;
		} else {
			DEBUG_HIT("PendingHitManager::OnCharacterApplyDamage(Hit=%p): No context record found!", hit);
			return nullptr;
		}
	}

	void PendingHitManager::DeleteHit(PendingHit* hit)
	{
		if (hit->CapturedStatusEnter) {
			auto it = hitStatusDamageMap_.find(&hit->Status->Hit);
			if (it != hitStatusDamageMap_.end()) {
				hitStatusDamageMap_.erase(it);
			}
		}

		if (hit->CapturedStatusSetup) {
			auto it = hitStatusMap_.find(hit->Status);
			if (it != hitStatusMap_.end()) {
				hitStatusMap_.erase(it);
			}
		}

		if (hit->CapturedCharacterHit && hit->CharacterHitPointer != nullptr) {
			auto it = characterHitMap_.find(hit->CharacterHitPointer);
			if (it != characterHitMap_.end() && it->second == hit) {
				characterHitMap_.erase(it);
			}
		}

		auto it = hits_.find(hit->Id);
		if (it != hits_.end()) {
			// Hit deleted here by unique_ptr dtor
			hits_.erase(it);
		}
	}
	

	HitProxy::HitProxy(dse::ScriptExtender& osiris)
		: osiris_(osiris)
	{}


	void HitProxy::PostStartup()
	{
		if (PostLoaded) {
			return;
		}

		auto& hooks = osiris_.GetEngineHooks();
		hooks.esv__StatusHit__Setup.SetPreHook(&HitProxy::OnStatusHitSetup, this);
		hooks.esv__StatusHit__Enter.SetPreHook(&HitProxy::OnStatusHitEnter, this);
		hooks.esv__Character_Hit.SetWrapper(&HitProxy::OnCharacterHit, this);
		hooks.CDivinityStats_Character__HitInternal.SetWrapper(&HitProxy::OnCharacterHitInternal, this);
		hooks.esv__Character_ApplyDamageHook.SetWrapper(&HitProxy::OnCharacterApplyDamage, this);
		hooks.esv__StatusMachine__ApplyStatus.SetWrapper(&HitProxy::OnApplyStatus, this);
		hooks.SkillPrototype__GetSkillDamage.SetWrapper(&HitProxy::OnGetSkillDamage, this);

		PostLoaded = true;
	}


	void HitProxy::OnStatusHitSetup(esv::StatusHit* status, stats::HitDamageInfo* hit)
	{
		gExtender->GetServer().GetExtensionState().PendingHits.OnStatusHitSetup(status, hit);
	}


	void HitProxy::OnStatusHitEnter(esv::Status* status)
	{
		auto hit = static_cast<esv::StatusHit*>(status);
		auto context = gExtender->GetServer().GetExtensionState().PendingHits.OnStatusHitEnter(hit);

		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			lua->OnStatusHitEnter(hit, context);
		}

		gExtender->GetServer().GetOsirisStatusHelpers().ThrowStatusHitEnter(status);
	}


	void HitProxy::OnCharacterHit(esv::Character::HitProc* wrappedHit, esv::Character* self, stats::Character* attackerStats,
		stats::Item* itemStats, stats::DamagePairList* damageList, stats::HitType hitType, bool noHitRoll,
		stats::HitDamageInfo* damageInfo, int forceReduceDurability, stats::PropertyList* skillProperties, stats::HighGroundBonus highGround,
		bool procWindWalker, stats::CriticalRoll criticalRoll)
	{
		auto helper = gExtender->GetServer().GetExtensionState().DamageHelpers.Create();
		helper->Type = DamageHelpers::HT_PrepareHitEvent;
		helper->Target = self;
		if (attackerStats != nullptr) {
			helper->Source = static_cast<esv::Character*>(attackerStats->GameObject);
		}

		helper->SimulateHit = true;
		helper->HitType = hitType;
		helper->NoHitRoll = noHitRoll;
		helper->ProcWindWalker = procWindWalker;
		helper->HighGround = highGround;
		helper->CriticalRoll = criticalRoll;
		helper->ForceReduceDurability = (bool)forceReduceDurability;
		helper->SetExternalDamageInfo(damageInfo, damageList);

		gExtender->GetServer().GetOsirisStatusHelpers().ThrowCharacterHit(self, attackerStats, itemStats, damageList, hitType, noHitRoll,
			damageInfo, forceReduceDurability, skillProperties, highGround, procWindWalker, criticalRoll, *helper);

		wrappedHit(self, attackerStats, itemStats, damageList, helper->HitType, helper->NoHitRoll,
			damageInfo, helper->ForceReduceDurability, skillProperties, helper->HighGround,
			helper->ProcWindWalker, helper->CriticalRoll);

		gExtender->GetServer().GetExtensionState().PendingHits.OnCharacterHit(self, attackerStats, itemStats,
			damageList, hitType, noHitRoll, damageInfo, forceReduceDurability, highGround, procWindWalker, criticalRoll);

		gExtender->GetServer().GetExtensionState().DamageHelpers.Destroy(helper->Handle);
	}

	void HitProxy::OnCharacterHitInternal(stats::Character::HitInternalProc next, stats::Character* self,
		stats::Character* attackerStats, stats::Item* item, stats::DamagePairList* damageList, stats::HitType hitType, bool noHitRoll,
		bool forceReduceDurability, stats::HitDamageInfo* damageInfo, stats::PropertyList* skillProperties,
		stats::HighGroundBonus highGroundFlag, stats::CriticalRoll criticalRoll)
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


	void HitProxy::OnCharacterApplyDamage(esv::Character::ApplyDamageProc* next, esv::Character* self, stats::HitDamageInfo& hit,
		uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection)
	{
		auto context = gExtender->GetServer().GetExtensionState().PendingHits.OnCharacterApplyDamage(&hit);

		stats::HitDamageInfo luaHit = hit;

		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			if (lua->OnCharacterApplyDamage(self, luaHit, ComponentHandle(attackerHandle), causeType, impactDirection, context)) {
				return;
			}
		}

		next(self, luaHit, attackerHandle, causeType, impactDirection);
		// ApplyDamage() sets these two flags so we need to copy them
		hit.EffectFlags = luaHit.EffectFlags;
		hit.DamageDealt = luaHit.DamageDealt;
	}

	void HitProxy::OnApplyStatus(esv::StatusMachine::ApplyStatusProc* wrappedApply, esv::StatusMachine* self, esv::Status* status)
	{
		// Don't throw events for inactive status machines, as those will get swallowed
		// by Osiris during loading anyway.
		if (!self->IsStatusMachineActive) {
			wrappedApply(self, status);
			return;
		}

		ExtensionState::Get().PendingStatuses.Add(status);

		gExtender->GetServer().GetOsirisStatusHelpers().ThrowApplyStatus(self, status);

		bool previousPreventApplyState = self->PreventStatusApply;
		ComponentHandle targetHandle;
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

		LuaServerPin lua(ExtensionState::Get());
		if (lua) {
			if (lua->OnApplyStatus(self->OwnerObjectHandle, status, self->PreventStatusApply)) {
				self->PreventStatusApply = true;
			}
		}

		wrappedApply(self, status);

		self->PreventStatusApply = previousPreventApplyState;
		ExtensionState::Get().PendingStatuses.Remove(status);
	}


	void HitProxy::OnGetSkillDamage(stats::SkillPrototype::GetSkillDamageProc* next, stats::SkillPrototype* self, stats::DamagePairList* damageList,
		stats::ObjectInstance* attackerStats, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
		glm::vec3 const& targetPosition, stats::DeathType* pDeathType, int level, bool noRandomization)
	{
		LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
		if (lua) {
			if (lua->GetSkillDamage(self, damageList, attackerStats, isFromItem, stealthed, attackerPosition, targetPosition, pDeathType, level, noRandomization)) {
				return;
			}
		}

		next(self, damageList, attackerStats, isFromItem, stealthed, attackerPosition, targetPosition, pDeathType, level, noRandomization);
	}
}
