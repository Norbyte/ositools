#pragma once

#include <Osiris/Shared/CustomFunctions.h>
#include <Osiris/Functions/FunctionLibrary.h>
#include <GameDefinitions/PropertyMaps/PropertyMap.h>
#include <GameDefinitions/GameObjects/Surface.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

namespace dse
{
	extern LegacyPropertyMap<esv::Status, void> gStatusPropertyMap;
	extern LegacyPropertyMap<esv::StatusConsumeBase, esv::Status> gStatusConsumeBasePropertyMap;
	extern LegacyPropertyMap<esv::StatusHit, esv::Status> gStatusHitPropertyMap;
	extern LegacyPropertyMap<esv::StatusDying, esv::Status> gStatusDyingPropertyMap;
	extern LegacyPropertyMap<esv::StatusHeal, esv::Status> gStatusHealPropertyMap;
	extern LegacyPropertyMap<esv::StatusCharmed, esv::Status> gStatusCharmedPropertyMap;
	extern LegacyPropertyMap<esv::StatusKnockedDown, esv::Status> gStatusKnockedDownPropertyMap;
	extern LegacyPropertyMap<esv::StatusSummoning, esv::Status> gStatusSummoningPropertyMap;
	extern LegacyPropertyMap<esv::StatusHealing, esv::StatusConsume> gStatusHealingPropertyMap;
	extern LegacyPropertyMap<esv::StatusThrown, esv::Status> gStatusThrownPropertyMap;
	extern LegacyPropertyMap<esv::StatusTeleportFall, esv::Status> gStatusTeleportFallPropertyMap;
	extern LegacyPropertyMap<esv::StatusCombat, esv::Status> gStatusCombatPropertyMap;
	extern LegacyPropertyMap<esv::StatusAoO, esv::Status> gStatusAoOPropertyMap;
	extern LegacyPropertyMap<esv::StatusSneaking, esv::Status> gStatusSneakingPropertyMap;
	extern LegacyPropertyMap<esv::StatusUnlock, esv::Status> gStatusUnlockPropertyMap;
	extern LegacyPropertyMap<esv::StatusBoost, esv::Status> gStatusBoostPropertyMap;
	extern LegacyPropertyMap<esv::StatusUnsheathed, esv::Status> gStatusUnsheathedPropertyMap;
	extern LegacyPropertyMap<esv::StatusStance, esv::StatusConsumeBase> gStatusStancePropertyMap;
	extern LegacyPropertyMap<esv::StatusLying, esv::Status> gStatusLyingPropertyMap;
	extern LegacyPropertyMap<esv::StatusInfectiousDiseased, esv::StatusConsumeBase> gStatusInfectiousDiseasedPropertyMap;
	extern LegacyPropertyMap<esv::StatusInvisible, esv::StatusConsumeBase> gStatusInvisiblePropertyMap;
	extern LegacyPropertyMap<esv::StatusRotate, esv::Status> gStatusRotatePropertyMap;
	extern LegacyPropertyMap<esv::StatusIdentify, esv::Status> gStatusIdentifyPropertyMap;
	extern LegacyPropertyMap<esv::StatusRepair, esv::Status> gStatusRepairPropertyMap;
	extern LegacyPropertyMap<esv::StatusMaterial, esv::Status> gStatusMaterialPropertyMap;
	extern LegacyPropertyMap<esv::StatusExplode, esv::Status> gStatusExplodePropertyMap;
	extern LegacyPropertyMap<esv::StatusAdrenaline, esv::StatusConsumeBase> gStatusAdrenalinePropertyMap;
	extern LegacyPropertyMap<esv::StatusShacklesOfPain, esv::StatusConsumeBase> gStatusShacklesOfPainPropertyMap;
	extern LegacyPropertyMap<esv::StatusShacklesOfPainCaster, esv::StatusConsumeBase> gStatusShacklesOfPainCasterPropertyMap;
	extern LegacyPropertyMap<esv::StatusDrain, esv::Status> gStatusDrainPropertyMap;
	extern LegacyPropertyMap<esv::StatusSpiritVision, esv::StatusConsumeBase> gStatusSpiritVisionPropertyMap;
	extern LegacyPropertyMap<esv::StatusSpirit, esv::Status> gStatusSpiritPropertyMap;
	extern LegacyPropertyMap<esv::StatusDamage, esv::StatusConsumeBase> gStatusDamagePropertyMap;
	extern LegacyPropertyMap<esv::StatusClimbing, esv::Status> gStatusClimbingPropertyMap;
	extern LegacyPropertyMap<esv::StatusIncapacitated, esv::StatusConsumeBase> gStatusIncapacitatedPropertyMap;
	extern LegacyPropertyMap<esv::StatusInSurface, esv::Status> gStatusInSurfacePropertyMap;
	extern LegacyPropertyMap<esv::StatusPolymorphed, esv::StatusConsumeBase> gStatusPolymorphedPropertyMap;
	extern LegacyPropertyMap<esv::StatusDamageOnMove, esv::StatusDamage> gStatusDamageOnMovePropertyMap;
	extern LegacyPropertyMap<esv::StatusChallenge, esv::StatusConsumeBase> gStatusChallengePropertyMap;
	extern LegacyPropertyMap<esv::StatusHealSharing, esv::StatusConsumeBase> gStatusHealSharingPropertyMap;
	extern LegacyPropertyMap<esv::StatusHealSharingCaster, esv::StatusConsumeBase> gStatusHealSharingCasterPropertyMap;
	extern LegacyPropertyMap<esv::StatusActiveDefense, esv::StatusConsume> gStatusActiveDefensePropertyMap;
	extern LegacyPropertyMap<esv::StatusSpark, esv::StatusConsume> gStatusSparkPropertyMap;

	extern LegacyPropertyMap<stats::HitDamageInfo, void> gHitDamageInfoPropertyMap;
	extern LegacyPropertyMap<esv::DamageHelpers, void> gDamageHelpersPropertyMap;
	extern LegacyPropertyMap<esv::ShootProjectileHelper, void> gShootProjectileHelperPropertyMap;
	extern LegacyPropertyMap<eoc::ItemDefinition, void> gEoCItemDefinitionPropertyMap;
	extern LegacyPropertyMap<stats::EquipmentAttributes, void> gEquipmentAttributesPropertyMap;
	extern LegacyPropertyMap<stats::EquipmentAttributesWeapon, stats::EquipmentAttributes> gEquipmentAttributesWeaponPropertyMap;
	extern LegacyPropertyMap<stats::EquipmentAttributesArmor, stats::EquipmentAttributes> gEquipmentAttributesArmorPropertyMap;
	extern LegacyPropertyMap<stats::EquipmentAttributesShield, stats::EquipmentAttributes> gEquipmentAttributesShieldPropertyMap;
	extern LegacyPropertyMap<stats::CharacterDynamicStat, void> gCharacterDynamicStatPropertyMap;
	extern LegacyPropertyMap<stats::Character, void> gCharacterStatsPropertyMap;
	extern LegacyPropertyMap<stats::Item, void> gItemStatsPropertyMap;
	extern LegacyPropertyMap<eoc::PlayerCustomData, void> gPlayerCustomDataPropertyMap;
	extern LegacyPropertyMap<esv::Character, void> gCharacterPropertyMap;
	extern LegacyPropertyMap<esv::Item, void> gItemPropertyMap;
	extern LegacyPropertyMap<esv::Projectile, void> gProjectilePropertyMap;
	extern LegacyPropertyMap<esv::Surface, void> gEsvSurfacePropertyMap;
	extern LegacyPropertyMap<Trigger, void> gTriggerPropertyMap;
	extern LegacyPropertyMap<SoundVolumeTriggerData, void> gSoundVolumeTriggerDataPropertyMap;

	extern LegacyPropertyMap<esv::SurfaceAction, void> gEsvSurfaceActionPropertyMap;
	extern LegacyPropertyMap<esv::TransformSurfaceAction, esv::SurfaceAction> gEsvTransformSurfaceActionPropertyMap;
	extern LegacyPropertyMap<esv::CreateSurfaceActionBase, esv::SurfaceAction> gEsvCreateSurfaceActionBasePropertyMap;
	extern LegacyPropertyMap<esv::CreateSurfaceAction, esv::CreateSurfaceActionBase> gEsvCreateSurfaceActionPropertyMap;
	extern LegacyPropertyMap<esv::ChangeSurfaceOnPathAction, esv::CreateSurfaceActionBase> gEsvChangeSurfaceOnPathActionPropertyMap;
	extern LegacyPropertyMap<esv::CreatePuddleAction, esv::CreateSurfaceActionBase> gEsvCreatePuddleActionPropertyMap;
	extern LegacyPropertyMap<esv::ExtinguishFireAction, esv::CreateSurfaceActionBase> gEsvExtinguishFireActionPropertyMap;
	extern LegacyPropertyMap<esv::RectangleSurfaceAction, esv::CreateSurfaceActionBase> gEsvRectangleSurfaceActionPropertyMap;
	extern LegacyPropertyMap<esv::PolygonSurfaceAction, esv::CreateSurfaceActionBase> gEsvPolygonSurfaceActionPropertyMap;
	extern LegacyPropertyMap<esv::SwapSurfaceAction, esv::CreateSurfaceActionBase> gEsvSwapSurfaceActionPropertyMap;
	extern LegacyPropertyMap<esv::ZoneAction, esv::CreateSurfaceActionBase> gEsvZoneActionPropertyMap;


	extern LegacyPropertyMap<esv::ASAttack, void> gASAttackPropertyMap;
	extern LegacyPropertyMap<esv::ASPrepareSkill, void> gASPrepareSkillPropertyMap;
	extern LegacyPropertyMap<esv::SkillState, void> gSkillStatePropertyMap;

	extern LegacyPropertyMap<ecl::Character, void> gEclCharacterPropertyMap;
	extern LegacyPropertyMap<ecl::Item, void> gEclItemPropertyMap;
	extern LegacyPropertyMap<ecl::Status, void> gEclStatusPropertyMap;

	extern LegacyPropertyMap<GameObjectTemplate, void> gGameObjectTemplatePropertyMap;
	extern LegacyPropertyMap<EoCGameObjectTemplate, GameObjectTemplate> gEoCGameObjectTemplatePropertyMap;
	extern LegacyPropertyMap<CharacterTemplate, EoCGameObjectTemplate> gCharacterTemplatePropertyMap;
	extern LegacyPropertyMap<ItemTemplate, EoCGameObjectTemplate> gItemTemplatePropertyMap;

	void InitPropertyMaps();

	enum class OsiPropertyMapType
	{
		Integer,
		Integer64,
		Real,
		String,
		GuidStringHandle,
		Vector3
	};


	bool OsirisPropertyMapSetRaw(LegacyPropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true);
	bool OsirisPropertyMapGetRaw(LegacyPropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true);

	template <class T>
	bool OsirisPropertyMapGet(LegacyPropertyMapInterface<T> const & propertyMap, T * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true)
	{
		return OsirisPropertyMapGetRaw(propertyMap, obj, args, firstArg, type, throwError);
	}

	template <class T>
	bool OsirisPropertyMapSet(LegacyPropertyMapInterface<T> const & propertyMap, T * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true)
	{
		return OsirisPropertyMapSetRaw(propertyMap, obj, args, firstArg, type, throwError);
	}

	bool LuaPropertyMapGet(lua_State * L, LegacyPropertyMapBase const & propertyMap, void * obj,
		char const * propertyName, bool throwError);
	bool LuaPropertyMapGet(lua_State* L, LegacyPropertyMapBase const& propertyMap, void* obj,
		FixedString const& propertyName, bool throwError);
	bool LuaPropertyMapSet(lua_State * L, int index, LegacyPropertyMapBase const & propertyMap,
		void * obj, char const * propertyName, bool throwError);
}
