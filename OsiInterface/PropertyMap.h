#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <optional>

#include <GameDefinitions/BaseTypes.h>

namespace dse
{
	enum class PropertyType
	{
		kBool,
		kUInt8,
		kInt16,
		kUInt16,
		kInt32,
		kUInt32,
		kInt64,
		kUInt64,
		kFloat,
		kFixedString,
		kFixedStringGuid,
		kStringPtr,
		kStdString,
		kStdWString,
		kObjectHandle,
		kVector3
	};

	enum PropertyFlag
	{
		kPropRead = 1,
		kPropWrite = 2
	};

	template <class T>
	constexpr PropertyType GetPropertyType()
	{
		if constexpr (std::is_same<T, bool>::value) {
			return PropertyType::kBool;
		} else if constexpr (std::is_same<T, uint8_t>::value) {
			return PropertyType::kUInt8;
		} else if constexpr (std::is_same<T, int16_t>::value) {
			return PropertyType::kInt16;
		} else if constexpr (std::is_same<T, uint16_t>::value) {
			return PropertyType::kUInt16;
		} else if constexpr (std::is_same<T, int32_t>::value) {
			return PropertyType::kInt32;
		} else if constexpr (std::is_same<T, uint32_t>::value) {
			return PropertyType::kUInt32;
		} else if constexpr (std::is_same<T, int64_t>::value) {
			return PropertyType::kInt64;
		} else if constexpr (std::is_same<T, uint64_t>::value) {
			return PropertyType::kUInt64;
		} else if constexpr (std::is_same<T, float>::value) {
			return PropertyType::kFloat;
		} else if constexpr (std::is_same<T, FixedString>::value) {
			return PropertyType::kFixedString;
		} else if constexpr (std::is_same<T, char *>::value) {
			return PropertyType::kStringPtr;
		} else if constexpr (std::is_same<T, STDString>::value) {
			return PropertyType::kStdString;
		} else if constexpr (std::is_same<T, STDWString>::value) {
			return PropertyType::kStdWString;
		} else if constexpr (std::is_same<T, ObjectHandle>::value) {
			return PropertyType::kObjectHandle;
		} else if constexpr (std::is_same<T, Vector3>::value) {
			return PropertyType::kVector3;
		} else if constexpr (std::is_same<T, NetId>::value) {
			return PropertyType::kUInt32;
		} else {
			static_assert(false, "Unsupported property type");
		}
	}

	struct PropertyMapBase
	{
		struct PropertyInfo
		{
			PropertyType Type;
			std::uintptr_t Offset;
			uint32_t Flags;

			std::function<bool (void *, int64_t)> SetInt;
			std::function<bool (void *, float)> SetFloat;
			std::function<bool (void *, char const *)> SetString;
			std::function<bool (void *, ObjectHandle)> SetHandle;
			std::function<bool (void *, Vector3)> SetVector3;
			std::function<std::optional<int64_t> (void *)> GetInt;
			std::function<std::optional<float> (void *)> GetFloat;
			std::function<std::optional<char const *> (void *)> GetString;
			std::function<std::optional<ObjectHandle> (void *)> GetHandle;
			std::function<std::optional<Vector3> (void *)> GetVector3;
		};

		struct FlagInfo
		{
			std::string Property;
			uint64_t Mask;
			uint32_t Flags;

			std::function<bool (void *, bool)> Set;
			std::function<std::optional<bool> (void *)> Get;
		};

		std::unordered_map<std::string, PropertyInfo> Properties;
		std::unordered_map<std::string, FlagInfo> Flags;

		PropertyMapBase * Parent{ nullptr };

		virtual void * toParent(void * obj) const = 0;

		PropertyInfo const * findProperty(std::string const & name) const
		{
			PropertyMapBase const * propMap = this;
			do {
				auto prop = propMap->Properties.find(name);
				if (prop != propMap->Properties.end()) {
					return &prop->second;
				}

				propMap = propMap->Parent;
			} while (propMap != nullptr);
			
			return nullptr;
		}

		FlagInfo const * findFlag(std::string const & name) const
		{
			PropertyMapBase const * propMap = this;
			do {
				auto prop = propMap->Flags.find(name);
				if (prop != propMap->Flags.end()) {
					return &prop->second;
				}

				propMap = propMap->Parent;
			} while (propMap != nullptr);

			return nullptr;
		}

		std::optional<int64_t> getInt(void * obj, std::string const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getInt(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get int '" << name << "': Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetInt) {
				return prop->second.GetInt(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get int '" << name << "': Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kBool: return (int64_t)*reinterpret_cast<bool *>(ptr);
			case PropertyType::kUInt8: return (int64_t)*reinterpret_cast<uint8_t *>(ptr);
			case PropertyType::kInt16: return (int64_t)*reinterpret_cast<int16_t *>(ptr);
			case PropertyType::kUInt16: return (int64_t)*reinterpret_cast<uint16_t *>(ptr);
			case PropertyType::kInt32: return (int64_t)*reinterpret_cast<int32_t *>(ptr);
			case PropertyType::kUInt32: return (int64_t)*reinterpret_cast<uint32_t *>(ptr);
			case PropertyType::kInt64: return (int64_t)*reinterpret_cast<int64_t *>(ptr);
			case PropertyType::kUInt64: return (int64_t)*reinterpret_cast<uint64_t *>(ptr);
			case PropertyType::kFloat: return (int64_t)*reinterpret_cast<float *>(ptr);
			default:
				OsiError("Failed to get property '" << name << "': Property is not an int");
				return {};
			}
		}

		std::optional<float> getFloat(void * obj, std::string const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getFloat(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get float '" << name << "': Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetFloat) {
				return prop->second.GetFloat(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get float '" << name << "': Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kFloat: return *reinterpret_cast<float *>(ptr);
			default:
				OsiError("Failed to get property '" << name << "': Property is not a float");
				return {};
			}
		}

		bool setInt(void * obj, std::string const & name, int64_t value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setInt(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set int '" << name << "': Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetInt) {
				return prop->second.SetInt(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set int '" << name << "': Property not writeable");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kBool: *reinterpret_cast<bool *>(ptr) = (bool)value; break;
			case PropertyType::kUInt8: *reinterpret_cast<uint8_t *>(ptr) = (uint8_t)value; break;
			case PropertyType::kInt16: *reinterpret_cast<int16_t *>(ptr) = (int16_t)value; break;
			case PropertyType::kUInt16: *reinterpret_cast<uint16_t *>(ptr) = (uint16_t)value; break;
			case PropertyType::kInt32: *reinterpret_cast<int32_t *>(ptr) = (int32_t)value; break;
			case PropertyType::kUInt32: *reinterpret_cast<uint32_t *>(ptr) = (uint32_t)value; break;
			case PropertyType::kInt64: *reinterpret_cast<int64_t *>(ptr) = (int64_t)value; break;
			case PropertyType::kUInt64: *reinterpret_cast<uint64_t *>(ptr) = (uint64_t)value; break;
			case PropertyType::kFloat: *reinterpret_cast<float *>(ptr) = (float)value; break;
			default:
				OsiError("Failed to set property '" << name << "': Property is not an int");
				return false;
			}

			return true;
		}

		bool setFloat(void * obj, std::string const & name, float value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setFloat(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set float '" << name << "': Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetFloat) {
				return prop->second.SetFloat(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set float '" << name << "': Property not writeable");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kFloat: *reinterpret_cast<float *>(ptr) = value; break;
			default:
				OsiError("Failed to set property '" << name << "': Property is not a float");
				return false;
			}

			return true;
		}

		std::optional<char const *> getString(void * obj, std::string const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getString(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get string '" << name << "': Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetString) {
				return prop->second.GetString(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get string '" << name << "': Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kFixedString:
			case PropertyType::kFixedStringGuid:
			{
				auto p = reinterpret_cast<FixedString *>(ptr)->Str;
				if (p != nullptr) {
					return p;
				} else {
					OsiError("Failed to get FixedString property '" << name << "': String is null!");
					return {};
				}
			}

			case PropertyType::kStringPtr:
			{
				auto p = *reinterpret_cast<char const **>(ptr);
				if (p != nullptr) {
					return p;
				} else {
					OsiError("Failed to get raw string property '" << name << "': String is null!");
					return {};
				}
			}

			case PropertyType::kStdString:
				return gTempStrings.Make(*reinterpret_cast<STDString *>(ptr));

			case PropertyType::kStdWString:
			{
				auto str = reinterpret_cast<STDWString *>(ptr);
				return gTempStrings.Make(ToUTF8(*str));
			}

			case PropertyType::kObjectHandle:
			{
				auto handle = reinterpret_cast<ObjectHandle *>(ptr);
				if (*handle) {
					// FIXME - support for client?
					auto object = esv::GetEntityWorld()->GetGameObject(*handle, false);
					if (object != nullptr) {
						return object->MyGuid.Str;
					} else {
						return {};
					}
				} else {
					return {};
				}
			}

			default:
				OsiError("Failed to get property '" << name << "': Property is not a string");
				return {};
			}
		}

		bool setString(void * obj, std::string const & name, char const * value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setString(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set string '" << name << "': Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetString) {
				return prop->second.SetString(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set string '" << name << "': Property not writeable");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kFixedString:
				{
					auto fs = ToFixedString(value);
					if (!fs) {
						OsiError("Failed to set string '" << name << "': Could not map to FixedString");
						return false;
					} else {
						*reinterpret_cast<FixedString *>(ptr) = fs;
						return true;
					}
				}

			case PropertyType::kFixedStringGuid:
				{
					auto fs = NameGuidToFixedString(value);
					if (!fs) {
						OsiError("Failed to set string '" << name << "': Could not map to FixedString GUID");
						return false;
					} else {
						*reinterpret_cast<FixedString *>(ptr) = fs;
						return true;
					}
				}

			case PropertyType::kStringPtr:
				OsiError("Failed to set property '" << name << "': Updating raw string properties not supported");
				return false;

			case PropertyType::kStdString:
				*reinterpret_cast<STDString *>(ptr) = value;
				return true;

			case PropertyType::kStdWString:
				*reinterpret_cast<STDWString *>(ptr) = FromUTF8(value);
				return true;

			default:
				OsiError("Failed to set property '" << name << "': Property is not a string");
				return false;
			}
		}

		std::optional<ObjectHandle> getHandle(void * obj, std::string const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getHandle(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get handle '" << name << "': Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetHandle) {
				return prop->second.GetHandle(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get handle '" << name << "': Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kObjectHandle) {
				return *reinterpret_cast<ObjectHandle *>(ptr);
			} else {
				OsiError("Failed to get property '" << name << "': Property is not a handle");
				return {};
			}
		}

		bool setHandle(void * obj, std::string const & name, ObjectHandle value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setHandle(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set handle '" << name << "': Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetHandle) {
				return prop->second.SetHandle(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set handle '" << name << "': Property not writeable");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kObjectHandle) {
				*reinterpret_cast<ObjectHandle *>(ptr) = value;
				return true;
			} else {
				OsiError("Failed to set property '" << name << "': Property is not a handle");
				return false;
			}
		}

		std::optional<Vector3> getVector3(void * obj, std::string const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getVector3(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get vector '" << name << "': Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetVector3) {
				return prop->second.GetVector3(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get vector '" << name << "': Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kVector3) {
				return *reinterpret_cast<Vector3 *>(ptr);
			} else {
				OsiError("Failed to get property '" << name << "': Property is not a vector");
				return {};
			}
		}

		bool setVector3(void * obj, std::string const & name, Vector3 const & value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setVector3(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set vector '" << name << "': Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetVector3) {
				return prop->second.SetVector3(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set vector '" << name << "': Property not writeable");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kVector3) {
				*reinterpret_cast<Vector3 *>(ptr) = value;
				return true;
			} else {
				OsiError("Failed to get property '" << name << "': Property is not a vector");
				return false;
			}
		}

		std::optional<bool> getFlag(void * obj, std::string const & name, bool raw, bool throwError) const
		{
			auto flag = Flags.find(name);
			if (flag == Flags.end()) {
				if (Parent != nullptr) {
					return Parent->getFlag(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get flag '" << name << "': Property does not exist");
					}
					return {};
				}
			}

			if (!raw && flag->second.Get) {
				return flag->second.Get(obj);
			}

			if (!raw && !(flag->second.Flags & kPropRead)) {
				OsiError("Failed to get flag '" << name << "': Property not readable");
				return {};
			}

			auto value = getInt(obj, flag->second.Property, true, throwError);
			if (!value) {
				return {};
			}

			return (*value & flag->second.Mask) != 0;
		}

		bool setFlag(void * obj, std::string const & name, bool value, bool raw, bool throwError) const
		{
			auto flag = Flags.find(name);
			if (flag == Flags.end()) {
				if (Parent != nullptr) {
					return Parent->setFlag(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set flag '" << name << "': Property does not exist");
					}
					return false;
				}
			}

			if (!raw && flag->second.Set) {
				return flag->second.Set(obj, value);
			}

			if (!raw && !(flag->second.Flags & kPropWrite)) {
				OsiError("Failed to set flag '" << name << "': Property not writeable");
				return false;
			}

			auto currentValue = getInt(obj, flag->second.Property, true, throwError);
			if (!currentValue) {
				return false;
			}

			if (value) {
				*currentValue |= flag->second.Mask;
			} else {
				*currentValue &= ~flag->second.Mask;
			}

			return setInt(obj, flag->second.Property, *currentValue, true, throwError);
		}
	};

	template <class T>
	struct PropertyMapInterface : public PropertyMapBase
	{
		using ObjType = T;
	};

	template <class T, class TBase>
	struct PropertyMap : public PropertyMapInterface<T>
	{
		using ObjectType = T;
		using BaseType = typename std::conditional<!std::is_same<TBase, void>::value, TBase, T>::type;

		virtual void * toParent(void * obj) const
		{
			auto typedObj = reinterpret_cast<T *>(obj);
			return static_cast<TBase *>(obj);
		}
	};

	template <class TValue>
	typename PropertyMapBase::PropertyInfo & AddProperty(PropertyMapBase & map, std::string const & name, std::uintptr_t offset)
	{
		PropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead | kPropWrite;
		auto it = map.Properties.insert(std::make_pair(name, info));
		return it.first->second;
	}

	template <class TValue>
	typename PropertyMapBase::PropertyInfo & AddPropertyRO(PropertyMapBase & map, std::string const & name, std::uintptr_t offset)
	{
		PropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead;
		auto it = map.Properties.insert(std::make_pair(name, info));
		return it.first->second;
	}

	template <class TEnum>
	typename PropertyMapBase::PropertyInfo & AddPropertyEnum(PropertyMapBase & map, std::string const & name, std::uintptr_t offset)
	{
		using TValue = std::underlying_type<TEnum>::type;
		PropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead | kPropWrite;

		info.GetInt = [offset](void * obj) -> std::optional<int64_t> {
			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(obj) + offset);
			return (int64_t)*ptr;
		};

		info.GetString = [offset](void * obj) -> std::optional<char const *> {
			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(obj) + offset);
			return EnumInfo<TEnum>::Find(*ptr);
		};

		info.SetInt = [offset](void * obj, int64_t val) -> bool {
			auto label = EnumInfo<TEnum>::Find((TEnum)val);
			if (!label) {
				return false;
			}

			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(obj) + offset);
			*ptr = (TEnum)val;
			return true;
		};

		info.SetString = [offset](void * obj, char const * str) -> bool {
			auto enumVal = EnumInfo<TEnum>::Find(str);
			if (!enumVal) {
				return false;
			}

			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(obj) + offset);
			*ptr = *enumVal;
			return true;
		};

		auto it = map.Properties.insert(std::make_pair(name, info));
		return it.first->second;
	}

	template <class TValue, class TEnum>
	void AddPropertyFlags(PropertyMapBase & map, std::string const & name, 
		std::uintptr_t offset, bool canWrite)
	{
		using Enum = EnumInfo<TEnum>;

		PropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = 0;
		map.Properties.insert(std::make_pair(name, info));

		for (auto i = 0; i < std::size(Enum::Values); i++) {
			PropertyMapBase::FlagInfo flag;
			flag.Property = name;
			flag.Flags = kPropRead | (canWrite ? kPropWrite : 0);
			flag.Mask = (int64_t)Enum::Values[i].Val;
			map.Flags.insert(std::make_pair(Enum::Values[i].Name, flag));
		}
	}

	template <class TValue>
	typename void AddPropertyGuidString(PropertyMapBase & map, std::string const & name,
		std::uintptr_t offset, bool canWrite)
	{
		static_assert(std::is_same<TValue, FixedString>::value, "Only FixedString GUID values are supported");
		PropertyMapBase::PropertyInfo info;
		info.Type = PropertyType::kFixedStringGuid;
		info.Offset = offset;
		info.Flags = kPropRead | (canWrite ? kPropWrite : 0);
		map.Properties.insert(std::make_pair(name, info));
	}
}
