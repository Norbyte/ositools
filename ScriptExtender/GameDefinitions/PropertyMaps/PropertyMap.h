#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <optional>

#include <GameDefinitions/Base/Base.h>

namespace dse
{
	enum class LegacyPropertyType
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
		kDynamicFixedString,
		kFixedStringGuid,
		kStringPtr,
		kStdString,
		kStdWString,
		kTranslatedString,
		kObjectHandle,
		kVector3
	};

	enum PropertyFlag
	{
		kPropRead = 1,
		kPropWrite = 2
	};

	template <class T>
	constexpr LegacyPropertyType GetPropertyType()
	{
		if constexpr (std::is_same<T, bool>::value) {
			return LegacyPropertyType::kBool;
		} else if constexpr (std::is_same<T, uint8_t>::value) {
			return LegacyPropertyType::kUInt8;
		} else if constexpr (std::is_same<T, int16_t>::value) {
			return LegacyPropertyType::kInt16;
		} else if constexpr (std::is_same<T, uint16_t>::value) {
			return LegacyPropertyType::kUInt16;
		} else if constexpr (std::is_same<T, int32_t>::value) {
			return LegacyPropertyType::kInt32;
		} else if constexpr (std::is_same<T, uint32_t>::value) {
			return LegacyPropertyType::kUInt32;
		} else if constexpr (std::is_same<T, int64_t>::value) {
			return LegacyPropertyType::kInt64;
		} else if constexpr (std::is_same<T, uint64_t>::value) {
			return LegacyPropertyType::kUInt64;
		} else if constexpr (std::is_same<T, float>::value) {
			return LegacyPropertyType::kFloat;
		} else if constexpr (std::is_same<T, FixedString>::value) {
			return LegacyPropertyType::kFixedString;
		} else if constexpr (std::is_same<T, char *>::value) {
			return LegacyPropertyType::kStringPtr;
		} else if constexpr (std::is_same<T, STDString>::value) {
			return LegacyPropertyType::kStdString;
		} else if constexpr (std::is_same<T, STDWString>::value) {
			return LegacyPropertyType::kStdWString;
		} else if constexpr (std::is_same<T, TranslatedString>::value) {
			return LegacyPropertyType::kTranslatedString;
		} else if constexpr (std::is_same<T, ComponentHandle>::value) {
			return LegacyPropertyType::kObjectHandle;
		} else if constexpr (std::is_same<T, glm::vec3>::value) {
			return LegacyPropertyType::kVector3;
		} else if constexpr (std::is_same<T, NetId>::value) {
			return LegacyPropertyType::kUInt32;
		} else {
			static_assert(false, "Unsupported property type");
		}
	}

	struct LegacyPropertyMapBase
	{
		struct PropertyInfo
		{
			LegacyPropertyType Type;
			std::uintptr_t Offset;
			uint32_t Flags;

			std::function<bool (void *, int64_t)> SetInt;
			std::function<bool (void *, float)> SetFloat;
			std::function<bool (void *, char const *)> SetString;
			std::function<bool (void *, ComponentHandle)> SetHandle;
			std::function<bool (void *, glm::vec3)> SetVector3;
			std::function<std::optional<int64_t> (void *)> GetInt;
			std::function<std::optional<float> (void *)> GetFloat;
			std::function<std::optional<char const *> (void *)> GetString;
			std::function<std::optional<ComponentHandle> (void *)> GetHandle;
			std::function<std::optional<glm::vec3> (void *)> GetVector3;
		};

		struct FlagInfo
		{
			FixedString Property;
			uint64_t Mask;
			uint32_t Flags;

			std::function<bool (void *, bool)> Set;
			std::function<std::optional<bool> (void *)> Get;
		};

		STDString Name;
		std::unordered_map<FixedString, PropertyInfo> Properties;
		std::unordered_map<FixedString, FlagInfo> Flags;

		LegacyPropertyMapBase * Parent{ nullptr };

		virtual void * toParent(void * obj) const = 0;

		PropertyInfo const * findProperty(FixedString const& name) const
		{
			LegacyPropertyMapBase const * propMap = this;
			do {
				auto prop = propMap->Properties.find(name);
				if (prop != propMap->Properties.end()) {
					return &prop->second;
				}

				propMap = propMap->Parent;
			} while (propMap != nullptr);
			
			return nullptr;
		}

		FlagInfo const * findFlag(FixedString const& name) const
		{
			LegacyPropertyMapBase const * propMap = this;
			do {
				auto prop = propMap->Flags.find(name);
				if (prop != propMap->Flags.end()) {
					return &prop->second;
				}

				propMap = propMap->Parent;
			} while (propMap != nullptr);

			return nullptr;
		}

		std::optional<int64_t> getInt(void * obj, FixedString const& name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getInt(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get int property '" << name << "' of [" << Name << "]: Property does not exist!");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetInt) {
				return prop->second.GetInt(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get int property '" << name << "' of [" << Name << "]: Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case LegacyPropertyType::kBool: return (int64_t)*reinterpret_cast<bool *>(ptr);
			case LegacyPropertyType::kUInt8: return (int64_t)*reinterpret_cast<uint8_t *>(ptr);
			case LegacyPropertyType::kInt16: return (int64_t)*reinterpret_cast<int16_t *>(ptr);
			case LegacyPropertyType::kUInt16: return (int64_t)*reinterpret_cast<uint16_t *>(ptr);
			case LegacyPropertyType::kInt32: return (int64_t)*reinterpret_cast<int32_t *>(ptr);
			case LegacyPropertyType::kUInt32: return (int64_t)*reinterpret_cast<uint32_t *>(ptr);
			case LegacyPropertyType::kInt64: return (int64_t)*reinterpret_cast<int64_t *>(ptr);
			case LegacyPropertyType::kUInt64: return (int64_t)*reinterpret_cast<uint64_t *>(ptr);
			case LegacyPropertyType::kFloat: return (int64_t)*reinterpret_cast<float *>(ptr);
			default:
				OsiError("Failed to get property '" << name << "' of [" << Name << "]: Property is not an int");
				return {};
			}
		}

		std::optional<float> getFloat(void * obj, FixedString const& name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getFloat(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get float property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetFloat) {
				return prop->second.GetFloat(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get float property '" << name << "' of [" << Name << "]: Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case LegacyPropertyType::kFloat: return *reinterpret_cast<float *>(ptr);
			default:
				OsiError("Failed to get property '" << name << "' of [" << Name << "]: Property is not a float");
				return {};
			}
		}

		bool setInt(void * obj, FixedString const & name, int64_t value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setInt(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set int property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetInt) {
				return prop->second.SetInt(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set int property '" << name << "' of [" << Name << "]: Property not writeable.");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case LegacyPropertyType::kBool: *reinterpret_cast<bool *>(ptr) = (bool)value; break;
			case LegacyPropertyType::kUInt8: *reinterpret_cast<uint8_t *>(ptr) = (uint8_t)value; break;
			case LegacyPropertyType::kInt16: *reinterpret_cast<int16_t *>(ptr) = (int16_t)value; break;
			case LegacyPropertyType::kUInt16: *reinterpret_cast<uint16_t *>(ptr) = (uint16_t)value; break;
			case LegacyPropertyType::kInt32: *reinterpret_cast<int32_t *>(ptr) = (int32_t)value; break;
			case LegacyPropertyType::kUInt32: *reinterpret_cast<uint32_t *>(ptr) = (uint32_t)value; break;
			case LegacyPropertyType::kInt64: *reinterpret_cast<int64_t *>(ptr) = (int64_t)value; break;
			case LegacyPropertyType::kUInt64: *reinterpret_cast<uint64_t *>(ptr) = (uint64_t)value; break;
			case LegacyPropertyType::kFloat: *reinterpret_cast<float *>(ptr) = (float)value; break;
			default:
				OsiError("Failed to set property '" << name << "' of [" << Name << "]: Property is not an int");
				return false;
			}

			return true;
		}

		bool setFloat(void * obj, FixedString const & name, float value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setFloat(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set float property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetFloat) {
				return prop->second.SetFloat(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set float property '" << name << "' of [" << Name << "]: Property not writeable; contact Norbyte on Discord if you have a legitimate use case for doing this.");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case LegacyPropertyType::kFloat: *reinterpret_cast<float *>(ptr) = value; break;
			default:
				OsiError("Failed to set property '" << name << "' of [" << Name << "]: Property is not a float");
				return false;
			}

			return true;
		}

		std::optional<char const *> getString(void * obj, FixedString const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getString(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get string property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetString) {
				return prop->second.GetString(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get string property '" << name << "' of [" << Name << "]: Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case LegacyPropertyType::kFixedString:
			case LegacyPropertyType::kDynamicFixedString:
			case LegacyPropertyType::kFixedStringGuid:
			{
				auto p = reinterpret_cast<FixedString *>(ptr)->Str;
				if (p != nullptr) {
					return p;
				} else {
					return "";
				}
			}

			case LegacyPropertyType::kStringPtr:
			{
				auto p = *reinterpret_cast<char const **>(ptr);
				if (p != nullptr) {
					return p;
				} else {
					return "";
				}
			}

			case LegacyPropertyType::kStdString:
				return gTempStrings.Make(*reinterpret_cast<STDString *>(ptr));

			case LegacyPropertyType::kStdWString:
			{
				auto str = reinterpret_cast<STDWString *>(ptr);
				return gTempStrings.Make(ToUTF8(*str));
			}

			case LegacyPropertyType::kTranslatedString:
			{
				auto str = reinterpret_cast<TranslatedString*>(ptr);
				return gTempStrings.Make(ToUTF8(str->Handle.ReferenceString));
			}

			case LegacyPropertyType::kObjectHandle:
			{
				auto handle = reinterpret_cast<ComponentHandle *>(ptr);
				if (*handle) {
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
				OsiError("Failed to get property '" << name << "' of [" << Name << "]: Property is not a string");
				return {};
			}
		}

		bool setString(void * obj, FixedString const & name, char const * value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setString(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set string property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetString) {
				return prop->second.SetString(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set string property '" << name << "' of [" << Name << "]: Property not writeable; contact Norbyte on Discord if you have a legitimate use case for doing this.");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case LegacyPropertyType::kFixedString:
			case LegacyPropertyType::kDynamicFixedString:
				*reinterpret_cast<FixedString*>(ptr) = FixedString(value);
				return true;

			case LegacyPropertyType::kFixedStringGuid:
				{
					auto fs = NameGuidToFixedString(value);
					if (!fs) {
						OsiError("Failed to set string property '" << name << "' of [" << Name << "]: Not a valid GUID string!");
						return false;
					} else {
						*reinterpret_cast<FixedString *>(ptr) = fs;
						return true;
					}
				}

			case LegacyPropertyType::kStringPtr:
				OsiError("Failed to set property '" << name << "' of [" << Name << "]: Updating raw string properties not supported");
				return false;

			case LegacyPropertyType::kStdString:
				*reinterpret_cast<STDString *>(ptr) = value;
				return true;

			case LegacyPropertyType::kStdWString:
				*reinterpret_cast<STDWString *>(ptr) = FromUTF8(value);
				return true;

			case LegacyPropertyType::kTranslatedString:
				OsiError("Failed to set property '" << name << "' of [" << Name << "]: Updating TranslatedString properties not supported");
				return false;

			default:
				OsiError("Failed to set property '" << name << "' of [" << Name << "]: Property is not a string");
				return false;
			}
		}

		std::optional<ComponentHandle> getHandle(void * obj, FixedString const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getHandle(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get handle property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetHandle) {
				return prop->second.GetHandle(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get handle property '" << name << "' of [" << Name << "]: Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == LegacyPropertyType::kObjectHandle) {
				return *reinterpret_cast<ComponentHandle *>(ptr);
			} else {
				OsiError("Failed to get property '" << name << "' of [" << Name << "]: Property is not a handle");
				return {};
			}
		}

		bool setHandle(void * obj, FixedString const & name, ComponentHandle value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setHandle(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set handle property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetHandle) {
				return prop->second.SetHandle(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set handle property '" << name << "' of [" << Name << "]: Property not writeable; contact Norbyte on Discord if you have a legitimate use case for doing this.");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == LegacyPropertyType::kObjectHandle) {
				*reinterpret_cast<ComponentHandle *>(ptr) = value;
				return true;
			} else {
				OsiError("Failed to set property '" << name << "' of [" << Name << "]: Property is not a handle");
				return false;
			}
		}

		std::optional<glm::vec3> getVector3(void * obj, FixedString const & name, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getVector3(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get vector property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return {};
				}
			}

			if (!raw && prop->second.GetVector3) {
				return prop->second.GetVector3(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				OsiError("Failed to get vector property '" << name << "' of [" << Name << "]: Property not readable");
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == LegacyPropertyType::kVector3) {
				return *reinterpret_cast<glm::vec3 *>(ptr);
			} else {
				OsiError("Failed to get property '" << name << "' of [" << Name << "]: Property is not a vector");
				return {};
			}
		}

		bool setVector3(void * obj, FixedString const & name, glm::vec3 const & value, bool raw, bool throwError) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setVector3(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set vector property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return false;
				}
			}

			if (!raw && prop->second.SetVector3) {
				return prop->second.SetVector3(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				OsiError("Failed to set vector property '" << name << "' of [" << Name << "]: Property not writeable; contact Norbyte on Discord if you have a legitimate use case for doing this.");
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == LegacyPropertyType::kVector3) {
				*reinterpret_cast<glm::vec3 *>(ptr) = value;
				return true;
			} else {
				OsiError("Failed to get property '" << name << "' of [" << Name << "]: Property is not a vector");
				return false;
			}
		}

		std::optional<bool> getFlag(void * obj, FixedString const & name, bool raw, bool throwError) const
		{
			auto flag = Flags.find(name);
			if (flag == Flags.end()) {
				if (Parent != nullptr) {
					return Parent->getFlag(toParent(obj), name, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to get flag property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return {};
				}
			}

			if (!raw && flag->second.Get) {
				return flag->second.Get(obj);
			}

			if (!raw && !(flag->second.Flags & kPropRead)) {
				OsiError("Failed to get flag property '" << name << "' of [" << Name << "]: Property not readable");
				return {};
			}

			auto value = getInt(obj, flag->second.Property, true, throwError);
			if (!value) {
				return {};
			}

			return (*value & flag->second.Mask) != 0;
		}

		bool setFlag(void * obj, FixedString const & name, bool value, bool raw, bool throwError) const
		{
			auto flag = Flags.find(name);
			if (flag == Flags.end()) {
				if (Parent != nullptr) {
					return Parent->setFlag(toParent(obj), name, value, raw, throwError);
				} else {
					if (throwError) {
						OsiError("Failed to set flag property '" << name << "' of [" << Name << "]: Property does not exist");
					}
					return false;
				}
			}

			if (!raw && flag->second.Set) {
				return flag->second.Set(obj, value);
			}

			if (!raw && !(flag->second.Flags & kPropWrite)) {
				OsiError("Failed to set flag property '" << name << "' of [" << Name << "]: Property not writeable; contact Norbyte on Discord if you have a legitimate use case for doing this.");
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
	struct LegacyPropertyMapInterface : public LegacyPropertyMapBase
	{
		using ObjType = T;
	};

	template <class T, class TBase>
	struct LegacyPropertyMap : public LegacyPropertyMapInterface<T>
	{
		using ObjectType = T;
		using BaseType = typename std::conditional<!std::is_same<TBase, void>::value, TBase, T>::type;

		virtual void * toParent(void * obj) const
		{
			auto typedObj = reinterpret_cast<T *>(obj);
			return static_cast<TBase *>(obj);
		}
	};

	inline LegacyPropertyMapBase::PropertyInfo& AddPropertyInternal(LegacyPropertyMapBase& map, char const* name, LegacyPropertyMapBase::PropertyInfo const& info)
	{
#if !defined(NDEBUG)
		if (map.Properties.find(FixedString(name)) != map.Properties.end()
			|| map.Flags.find(FixedString(name)) != map.Flags.end()) {
			throw std::runtime_error("Tried to add duplicate property!");
		}
#endif

		auto it = map.Properties.insert(std::make_pair(FixedString(name), info));
		return it.first->second;
	}

	template <class TValue>
	typename LegacyPropertyMapBase::PropertyInfo & AddProperty(LegacyPropertyMapBase & map, char const* name, std::uintptr_t offset)
	{
		LegacyPropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead | kPropWrite;
		return AddPropertyInternal(map, name, info);
	}

	template <class TValue>
	typename LegacyPropertyMapBase::PropertyInfo & AddPropertyRO(LegacyPropertyMapBase & map, char const* name, std::uintptr_t offset)
	{
		LegacyPropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead;
		return AddPropertyInternal(map, name, info);
	}

	template <class TEnum>
	typename LegacyPropertyMapBase::PropertyInfo & AddPropertyEnum(LegacyPropertyMapBase & map, char const* name, std::uintptr_t offset, bool writeable)
	{
		using TValue = std::underlying_type<TEnum>::type;
		LegacyPropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead | (writeable ? kPropWrite : 0);

		info.GetInt = [offset](void * obj) -> std::optional<int64_t> {
			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(obj) + offset);
			return (int64_t)*ptr;
		};

		info.GetString = [offset](void * obj) -> std::optional<char const *> {
			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(obj) + offset);
			auto val = EnumInfo<TEnum>::Find(*ptr);
			if (val) {
				return val.Str;
			} else {
				return {};
			}
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

		return AddPropertyInternal(map, name, info);
	}

	template <class TValue, class TEnum>
	void AddPropertyFlags(LegacyPropertyMapBase & map, char const* name,
		std::uintptr_t offset, bool canWrite)
	{
		using Enum = EnumInfo<TEnum>;

		FixedString fieldName(name);
		LegacyPropertyMapBase::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = 0;
		AddPropertyInternal(map, name, info);

		for (auto const& val : Enum::Values) {
			LegacyPropertyMapBase::FlagInfo flag;
			flag.Property = fieldName;
			flag.Flags = kPropRead | (canWrite ? kPropWrite : 0);
			flag.Mask = (int64_t)val.Value;

#if !defined(NDEBUG)
			if (map.Properties.find(val.Key) != map.Properties.end()
				|| map.Flags.find(val.Key) != map.Flags.end()) {
				throw std::runtime_error("Tried to add duplicate property!");
			}
#endif

			map.Flags.insert(std::make_pair(val.Key, flag));
		}
	}

	template <class TValue>
	typename void AddPropertyGuidString(LegacyPropertyMapBase & map, char const* name,
		std::uintptr_t offset, bool canWrite)
	{
		static_assert(std::is_same<TValue, FixedString>::value, "Only FixedString GUID values are supported");
		LegacyPropertyMapBase::PropertyInfo info;
		info.Type = LegacyPropertyType::kFixedStringGuid;
		info.Offset = offset;
		info.Flags = kPropRead | (canWrite ? kPropWrite : 0);
		AddPropertyInternal(map, name, info);
	}

	template <class TValue>
	typename void AddPropertyDynamicFixedString(LegacyPropertyMapBase& map, char const* name,
		std::uintptr_t offset, bool canWrite)
	{
		static_assert(std::is_same<TValue, FixedString>::value, "Only FixedString values are supported");
		LegacyPropertyMapBase::PropertyInfo info;
		info.Type = LegacyPropertyType::kDynamicFixedString;
		info.Offset = offset;
		info.Flags = kPropRead | (canWrite ? kPropWrite : 0);
		AddPropertyInternal(map, name, info);
	}
}
