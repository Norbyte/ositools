#pragma once

#include "CustomFunctions.h"
#include "Functions/FunctionLibrary.h"
#include "PropertyMap.h"

namespace osidbg
{
	extern PropertyMap<EsvStatus, void> gStatusPropertyMap;
	extern PropertyMap<EsvStatusConsume, EsvStatus> gStatusConsumePropertyMap;
	extern PropertyMap<EsvStatusHit, EsvStatus> gStatusHitPropertyMap;
	extern PropertyMap<EsvStatusHeal, EsvStatus> gStatusHealPropertyMap;
	extern PropertyMap<EsvStatusHealing, EsvStatusConsume> gStatusHealingPropertyMap;
	extern PropertyMap<HitDamageInfo, void> gHitDamageInfoPropertyMap;
	extern PropertyMap<EoCItemDefinition, void> gEoCItemDefinitionPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes, void> gEquipmentAttributesPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes_Weapon, CDivinityStats_Equipment_Attributes> gEquipmentAttributesWeaponPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes_Armor, CDivinityStats_Equipment_Attributes> gEquipmentAttributesArmorPropertyMap;
	extern PropertyMap<CDivinityStats_Equipment_Attributes_Shield, CDivinityStats_Equipment_Attributes> gEquipmentAttributesShieldPropertyMap;
	extern PropertyMap<CharacterDynamicStat, void> gCharacterDynamicStatPropertyMap;
	extern PropertyMap<CDivinityStats_Character, void> gCharacterStatsPropertyMap;

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

	template <class T>
	bool OsirisPropertyMapGet(PropertyMapInterface<T> const & propertyMap, T * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true)
	{
		auto propertyName = args.Get(firstArg).String;

		if (obj == nullptr) {
			OsiError("Attempted to get property '" << propertyName << "' of null object!");
			return false;
		}

		switch (type) {
			case OsiPropertyMapType::Integer:
			{
				auto val = propertyMap.getInt(obj, propertyName, false, false);
				if (val) {
					args.Get(firstArg + 1).Int32 = (int32_t)*val;
				} else {
					auto boolval = propertyMap.getFlag(obj, propertyName, false, throwError);
					if (boolval) {
						args.Get(firstArg + 1).Int32 = *boolval ? 1 : 0;
					} else {
						return false;
					}
				}

				return true;
			}

			case OsiPropertyMapType::Integer64:
			{
				auto val = propertyMap.getInt(obj, propertyName, false, throwError);
				if (!val) {
					return false;
				}

				args.Get(firstArg + 1).Int64 = (int64_t)*val;
				return true;
			}

			case OsiPropertyMapType::Real:
			{
				auto val = propertyMap.getFloat(obj, propertyName, false, throwError);
				if (!val) {
					return false;
				}

				args.Get(firstArg + 1).Float = *val;
				return true;
			}

			case OsiPropertyMapType::String:
			{
				auto val = propertyMap.getString(obj, propertyName, false, throwError);
				if (!val) {
					return false;
				}

				args.Get(firstArg + 1).String = *val;
				return true;
			}

			case OsiPropertyMapType::GuidStringHandle:
			{
				auto val = propertyMap.getHandle(obj, propertyName, false, throwError);
				if (!val) {
					return false;
				}

				if (!*val) {
					args.Get(firstArg + 1).String = ToFixedString("NULL_00000000-0000-0000-0000-000000000000").Str;
					return true;
				}

				auto character = FindCharacterByHandle(*val);
				if (character != nullptr) {
					args.Get(firstArg + 1).String = character->GetGuid()->Str;
					return true;
				}

				auto item = FindItemByHandle(*val);
				if (item != nullptr) {
					args.Get(firstArg + 1).String = item->GetGuid()->Str;
					return true;
				}

				OsiError("Failed to get property '" << propertyName << "': Could not map handle to game object: " << (int64_t)*val);
				return false;
			}

			case OsiPropertyMapType::Vector3:
			{
				auto val = propertyMap.getVector3(obj, propertyName, false, throwError);
				if (!val) {
					return false;
				}

				args.Get(firstArg + 1).Float = val->x;
				args.Get(firstArg + 2).Float = val->y;
				args.Get(firstArg + 3).Float = val->z;
				return true;
			}

			default:
				OsiError("Failed to get property '" << propertyName << "': Unknown Osi property type!");
				return false;
		}
	}

	template <class T>
	bool OsirisPropertyMapSet(PropertyMapInterface<T> const & propertyMap, T * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError = true)
	{
		auto propertyName = args.Get(firstArg).String;
		if (obj == nullptr) {
			OsiError("Attempted to set property '" << propertyName << "' of null object!");
			return false;
		}

		switch (type) {
		case OsiPropertyMapType::Integer:
		{
			auto val = args.Get(firstArg + 1).Int32;

			if (!propertyMap.setFlag(obj, propertyName, val != 0, false, false)) {
				return propertyMap.setInt(obj, propertyName, val, false, throwError);
			} else {
				return true;
			}
		}

		case OsiPropertyMapType::Integer64:
		{
			auto val = args.Get(firstArg + 1).Int64;
			return propertyMap.setInt(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::Real:
		{
			auto val = args.Get(firstArg + 1).Float;
			return propertyMap.setFloat(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::String:
		{
			auto val = args.Get(firstArg + 1).String;
			return propertyMap.setString(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::GuidStringHandle:
		{
			auto guid = args.Get(firstArg + 1).String;
			if (guid == nullptr 
				|| !strlen(guid) 
				|| NameGuidToFixedString(guid) == ToFixedString("00000000-0000-0000-0000-000000000000").Str) {
				return propertyMap.setHandle(obj, propertyName, ObjectHandle{}, false, throwError);
			}

			auto character = FindCharacterByNameGuid(guid);
			if (character != nullptr) {
				ObjectHandle handle;
				character->GetObjectHandle(&handle);
				return propertyMap.setHandle(obj, propertyName, handle, false, throwError);
			}

			auto item = FindItemByNameGuid(guid);
			if (item != nullptr) {
				ObjectHandle handle;
				item->GetObjectHandle(&handle);
				return propertyMap.setHandle(obj, propertyName, handle, false, throwError);
			}

			OsiError("Failed to set property '" << propertyName << "': Could not map GUID to game object: " << guid);
			return false;
		}

		case OsiPropertyMapType::Vector3:
		{
			auto x = args.Get(firstArg + 1).Float;
			auto y = args.Get(firstArg + 2).Float;
			auto z = args.Get(firstArg + 3).Float;
			Vector3 vec(x, y, z);
			return propertyMap.setVector3(obj, propertyName, vec, false, throwError);
		}

		default:
			OsiError("Failed to set property '" << propertyName << "': Unknown Osi property type!");
			return false;
		}
	}
}