#pragma once

#include <GameDefinitions/Osiris.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse::esv
{
	PropertyMapBase& StatusToPropertyMap(esv::Status* status);

	template <OsiPropertyMapType Type>
	bool StatusGetAttribute(OsiArgumentDesc & args)
	{
		auto status = GetStatusHelper(args);
		if (status == nullptr) {
			return false;
		}

		if constexpr (Type == OsiPropertyMapType::String) {
			if (strcmp(args[2].String, "StatusType") == 0) {
				auto type = EnumInfo<StatusType>::Find(status->GetStatusId());
				if (type) {
					args[3].Set(type.Str);
					return true;
				}
			}
		}

		if (status->GetStatusId() == StatusType::HIT) {
			auto hit = static_cast<esv::StatusHit*>(status);
			if (OsirisPropertyMapGet(gHitDamageInfoPropertyMap, &hit->DamageInfo, args, 2, Type, false)) {
				return true;
			}
		}

		auto& propertyMap = StatusToPropertyMap(status);
		return OsirisPropertyMapGetRaw(propertyMap, status, args, 2, Type);
	}

	template <OsiPropertyMapType Type>
	void StatusSetAttribute(OsiArgumentDesc const & args)
	{
		auto status = GetStatusHelper(args);
		if (status == nullptr) {
			return;
		}

		if (status->GetStatusId() == StatusType::HIT) {
			auto hit = static_cast<esv::StatusHit*>(status);
			if (OsirisPropertyMapSet(gHitDamageInfoPropertyMap, &hit->DamageInfo, args, 2, Type, false)) {
				return;
			}
		}

		auto& propertyMap = StatusToPropertyMap(status);
		OsirisPropertyMapSetRaw(propertyMap, status, args, 2, Type);
	}

	class StatusHelpers
	{
	public:
		esv::StatusMachine* GetStatusMachine(char const* gameObjectGuid);
		esv::Status* GetStatus(char const* gameObjectGuid, ComponentHandle const& statusHandle);
		void PreventApply(IEoCServerObject* gameObject, ComponentHandle const& statusHandle, bool preventApply);

		Status* PrepareStatus(esv::StatusMachine* statusMachine, FixedString const& stautsId, float lifeTime);
		std::optional<ComponentHandle> ApplyActiveDefense(esv::Character* character, FixedString const& stautsId, float lifeTime);
		std::optional<ComponentHandle> ApplyDamageOnMove(esv::Character* character, FixedString const& stautsId, esv::Character* sourceCharacter,
			float lifeTime, float distancePerDamage);

		void ThrowStatusHitEnter(esv::Status* status);
		void ThrowStatusHealEnter(esv::StatusHeal* status);
		void ThrowCharacterHit(esv::Character* self, CDivinityStats_Character* attackerStats,
			CDivinityStats_Item* itemStats, DamagePairList* damageList, HitType hitType, bool noHitRoll,
			HitDamageInfo* damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List* skillProperties, HighGroundBonus highGround,
			bool procWindWalker, CriticalRoll criticalRoll, DamageHelpers& helper);
		void ThrowApplyStatus(esv::StatusMachine* self, esv::Status* status);

	};
}
