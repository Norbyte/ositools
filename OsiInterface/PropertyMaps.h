#pragma once

#include "CustomFunctions.h"
#include "Functions/FunctionLibrary.h"
#include "PropertyMap.h"

namespace dse
{
	extern PropertyMap<esv::Status, void> gStatusPropertyMap;
	extern PropertyMap<esv::StatusConsume, esv::Status> gStatusConsumePropertyMap;
	extern PropertyMap<esv::StatusHit, esv::Status> gStatusHitPropertyMap;
	extern PropertyMap<esv::StatusHeal, esv::Status> gStatusHealPropertyMap;
	extern PropertyMap<esv::StatusHealing, esv::StatusConsume> gStatusHealingPropertyMap;
	extern PropertyMap<HitDamageInfo, void> gHitDamageInfoPropertyMap;
	extern PropertyMap<eoc::ItemDefinition, void> gEoCItemDefinitionPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes, void> gEquipmentAttributesPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes_Weapon, CDivinityStats_Equipment_Attributes> gEquipmentAttributesWeaponPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes_Armor, CDivinityStats_Equipment_Attributes> gEquipmentAttributesArmorPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes_Shield, CDivinityStats_Equipment_Attributes> gEquipmentAttributesShieldPropertyMap;
	extern PropertyMap<CharacterDynamicStat, void> gCharacterDynamicStatPropertyMap;
	extern PropertyMap<CDivinityStats_Character, void> gCharacterStatsPropertyMap;
	extern PropertyMap<CDivinityStats_Item, void> gItemStatsPropertyMap;
	extern PropertyMap<esv::PlayerCustomData, void> gPlayerCustomDataPropertyMap;
	extern PropertyMap<esv::EoCServerObject, void> gEoCServerObjectPropertyMap;
	extern PropertyMap<esv::Character, void> gCharacterPropertyMap;
	extern PropertyMap<esv::Item, void> gItemPropertyMap;
	extern PropertyMap<esv::ASPrepareSkill, void> gASPrepareSkillStatPropertyMap;
	extern PropertyMap<esv::ASUseSkill, void> gASUseSkillStatPropertyMap;

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


	bool OsirisPropertyMapSetRaw(PropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true);
	bool OsirisPropertyMapGetRaw(PropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true);

	template <class T>
	bool OsirisPropertyMapGet(PropertyMapInterface<T> const & propertyMap, T * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true)
	{
		return OsirisPropertyMapGetRaw(propertyMap, obj, args, firstArg, type, throwError);
	}

	template <class T>
	bool OsirisPropertyMapSet(PropertyMapInterface<T> const & propertyMap, T * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true)
	{
		return OsirisPropertyMapSetRaw(propertyMap, obj, args, firstArg, type, throwError);
	}

	bool LuaPropertyMapGet(lua_State * L, PropertyMapBase const & propertyMap, void * obj,
		char const * propertyName, bool throwError);
	bool LuaPropertyMapSet(lua_State * L, int index, PropertyMapBase const & propertyMap,
		void * obj, char const * propertyName, bool throwError);
}