#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>
#include <optional>

#include "DivBaseTypes.h"

namespace osidbg
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
		} else if constexpr (std::is_same<T, ObjectHandle>::value) {
			return PropertyType::kObjectHandle;
		} else if constexpr (std::is_same<T, Vector3>::value) {
			return PropertyType::kVector3;
		} else {
			static_assert(false, "Unsupported property type");
		}
	}

	template <class T>
	struct PropertyMapInterface
	{
		virtual std::optional<int64_t> getInt(T * obj, std::string const & name, bool raw) const = 0;
		virtual std::optional<float> getFloat(T * obj, std::string const & name, bool raw) const = 0;
		virtual bool setInt(T * obj, std::string const & name, int64_t value, bool raw) const = 0;
		virtual bool setFloat(T * obj, std::string const & name, float value, bool raw) const = 0;
		virtual std::optional<char const *> getString(T * obj, std::string const & name, bool raw) const = 0;
		virtual bool setString(T * obj, std::string const & name, char const * value, bool raw) const = 0;
		virtual std::optional<ObjectHandle> getHandle(T * obj, std::string const & name, bool raw) const = 0;
		virtual bool setHandle(T * obj, std::string const & name, ObjectHandle value, bool raw) const = 0;
		virtual std::optional<Vector3> getVector3(T * obj, std::string const & name, bool raw) const = 0;
		virtual bool setVector3(T * obj, std::string const & name, Vector3 const & value, bool raw) const = 0;
		virtual std::optional<bool> getFlag(T * obj, std::string const & name, bool raw) const = 0;
		virtual bool setFlag(T * obj, std::string const & name, bool value, bool raw) const = 0;
	};

	template <class T, class TBase>
	struct PropertyMap : public PropertyMapInterface<T>
	{
		using ObjType = T;
		using BaseType = typename std::conditional<!std::is_same<TBase, void>::value, TBase, T>::type;

		struct PropertyInfo
		{
			PropertyType Type;
			std::uintptr_t Offset;
			uint32_t Flags;

			std::function<bool (T *, int64_t)> SetInt;
			std::function<bool (T *, float)> SetFloat;
			std::function<bool (T *, char const *)> SetString;
			std::function<bool (T *, ObjectHandle)> SetHandle;
			std::function<bool (T *, Vector3)> SetVector3;
			std::function<std::optional<int64_t> (T *)> GetInt;
			std::function<std::optional<float> (T *)> GetFloat;
			std::function<std::optional<char const *> (T *)> GetString;
			std::function<std::optional<ObjectHandle> (T *)> GetHandle;
			std::function<std::optional<Vector3> (T *)> GetVector3;
		};

		struct FlagInfo
		{
			std::string Property;
			uint64_t Mask;
			uint32_t Flags;

			std::function<bool (T *, bool)> Set;
			std::function<std::optional<bool> (T *)> Get;
		};

		std::unordered_map<std::string, PropertyInfo> Properties;
		std::unordered_map<std::string, FlagInfo> Flags;

		PropertyMapInterface<BaseType> * Parent{ nullptr };

		std::optional<int64_t> getInt(T * obj, std::string const & name, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getInt(static_cast<BaseType *>(obj), name, raw);
				} else {
					return {};
				}
			}

			if (!raw && prop->second.GetInt) {
				return prop->second.GetInt(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
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
			default: return {};
			}
		}

		std::optional<float> getFloat(T * obj, std::string const & name, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getFloat(static_cast<BaseType *>(obj), name, raw);
				} else {
					return {};
				}
			}

			if (!raw && prop->second.GetFloat) {
				return prop->second.GetFloat(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kFloat: return *reinterpret_cast<float *>(ptr);
			default: return {};
			}
		}

		bool setInt(T * obj, std::string const & name, int64_t value, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setInt(static_cast<BaseType *>(obj), name, value, raw);
				} else {
					return false;
				}
			}

			if (!raw && prop->second.SetInt) {
				return prop->second.SetInt(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
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
			default: return false;
			}

			return true;
		}

		bool setFloat(T * obj, std::string const & name, float value, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setFloat(static_cast<BaseType *>(obj), name, value, raw);
				} else {
					return false;
				}
			}

			if (!raw && prop->second.SetFloat) {
				return prop->second.SetFloat(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			switch (prop->second.Type) {
			case PropertyType::kFloat: *reinterpret_cast<float *>(ptr) = value; break;
			default: return false;
			}

			return true;
		}

		std::optional<char const *> getString(T * obj, std::string const & name, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getString(static_cast<BaseType *>(obj), name, raw);
				} else {
					return {};
				}
			}

			if (!raw && prop->second.GetString) {
				return prop->second.GetString(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kFixedString) {
				return reinterpret_cast<FixedString *>(ptr)->Str;
			} else {
				return {};
			}
		}

		bool setString(T * obj, std::string const & name, char const * value, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setString(static_cast<BaseType *>(obj), name, value, raw);
				} else {
					return false;
				}
			}

			if (!raw && prop->second.SetString) {
				return prop->second.SetString(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kFixedString) {
				auto fs = ToFixedString(value);
				if (!fs) {
					return false;
				} else {
					*reinterpret_cast<FixedString *>(ptr) = fs;
					return true;
				}
			} else {
				return false;
			}
		}

		std::optional<ObjectHandle> getHandle(T * obj, std::string const & name, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getHandle(static_cast<BaseType *>(obj), name, raw);
				} else {
					return {};
				}
			}

			if (!raw && prop->second.GetHandle) {
				return prop->second.GetHandle(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kObjectHandle) {
				return *reinterpret_cast<ObjectHandle *>(ptr);
			} else {
				return {};
			}
		}

		bool setHandle(T * obj, std::string const & name, ObjectHandle value, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setHandle(static_cast<BaseType *>(obj), name, value, raw);
				} else {
					return false;
				}
			}

			if (!raw && prop->second.SetHandle) {
				return prop->second.SetHandle(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kObjectHandle) {
				*reinterpret_cast<ObjectHandle *>(ptr) = value;
				return true;
			} else {
				return false;
			}
		}

		std::optional<Vector3> getVector3(T * obj, std::string const & name, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->getVector3(static_cast<BaseType *>(obj), name, raw);
				} else {
					return {};
				}
			}

			if (!raw && prop->second.GetVector3) {
				return prop->second.GetVector3(obj);
			}

			if (!raw && !(prop->second.Flags & kPropRead)) {
				return {};
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kVector3) {
				return *reinterpret_cast<Vector3 *>(ptr);
			} else {
				return {};
			}
		}

		bool setVector3(T * obj, std::string const & name, Vector3 const & value, bool raw) const
		{
			auto prop = Properties.find(name);
			if (prop == Properties.end()) {
				if (Parent != nullptr) {
					return Parent->setVector3(static_cast<BaseType *>(obj), name, value, raw);
				} else {
					return false;
				}
			}

			if (!raw && prop->second.SetVector3) {
				return prop->second.SetVector3(obj, value);
			}

			if (!raw && !(prop->second.Flags & kPropWrite)) {
				return false;
			}

			auto ptr = reinterpret_cast<std::uintptr_t>(obj) + prop->second.Offset;
			if (prop->second.Type == PropertyType::kVector3) {
				*reinterpret_cast<Vector3 *>(ptr) = value;
				return true;
			} else {
				return false;
			}
		}

		std::optional<bool> getFlag(T * obj, std::string const & name, bool raw) const
		{
			auto flag = Flags.find(name);
			if (flag == Flags.end()) {
				if (Parent != nullptr) {
					return Parent->getFlag(static_cast<BaseType *>(obj), name, raw);
				} else {
					return {};
				}
			}

			if (!raw && flag->second.Get) {
				return flag->second.Get(obj);
			}

			if (!raw && !(flag->second.Flags & kPropRead)) {
				return {};
			}

			auto value = getInt(obj, flag->second.Property, true);
			if (!value) {
				return {};
			}

			return (*value & flag->second.Mask) != 0;
		}

		bool setFlag(T * obj, std::string const & name, bool value, bool raw) const
		{
			auto flag = Flags.find(name);
			if (flag == Flags.end()) {
				if (Parent != nullptr) {
					return Parent->setFlag(static_cast<BaseType *>(obj), name, value, raw);
				} else {
					return false;
				}
			}

			if (!raw && flag->second.Set) {
				return flag->second.Set(obj, value);
			}

			if (!raw && !(flag->second.Flags & kPropWrite)) {
				return false;
			}

			auto currentValue = getInt(obj, flag->second.Property, true);
			if (!currentValue) {
				return false;
			}

			if (value) {
				*currentValue |= flag->second.Mask;
			} else {
				*currentValue &= ~flag->second.Mask;
			}

			return setInt(obj, flag->second.Property, *currentValue, true);
		}
	};

	template <class TValue, class T, class TBase>
	typename PropertyMap<T, TBase>::PropertyInfo & AddProperty(PropertyMap<T, TBase> & map, std::string const & name, std::uintptr_t offset)
	{
		PropertyMap<T, TBase>::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead | kPropWrite;
		auto it = map.Properties.insert(std::make_pair(name, info));
		return it.first->second;
	}

	template <class TValue, class T, class TBase>
	typename PropertyMap<T, TBase>::PropertyInfo & AddPropertyRO(PropertyMap<T, TBase> & map, std::string const & name, std::uintptr_t offset)
	{
		PropertyMap<T, TBase>::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead;
		auto it = map.Properties.insert(std::make_pair(name, info));
		return it.first->second;
	}

	template <class TEnum, class T, class TBase>
	typename PropertyMap<T, TBase>::PropertyInfo & AddPropertyEnum(PropertyMap<T, TBase> & map, std::string const & name, std::uintptr_t offset)
	{
		using TValue = std::underlying_type<TEnum>::type;
		PropertyMap<T, TBase>::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = kPropRead | kPropWrite;

		info.GetInt = [offset](T * o) -> std::optional<int64_t> {
			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(o) + offset);
			return (int64_t)*ptr;
		};

		info.GetString = [offset](T * o) -> std::optional<char const *> {
			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(o) + offset);
			return EnumInfo<TEnum>::Find(*ptr);
		};

		info.SetInt = [offset](T * o, int64_t val) -> bool {
			auto label = EnumInfo<TEnum>::Find((TEnum)val);
			if (!label) {
				return false;
			}

			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(o) + offset);
			*ptr = (TEnum)val;
			return true;
		};

		info.SetString = [offset](T * o, char const * str) -> bool {
			auto enumVal = EnumInfo<TEnum>::Find(str);
			if (!enumVal) {
				return false;
			}

			auto ptr = reinterpret_cast<TEnum *>(reinterpret_cast<std::uintptr_t>(o) + offset);
			*ptr = *enumVal;
			return true;
		};

		auto it = map.Properties.insert(std::make_pair(name, info));
		return it.first->second;
	}

	template <class TValue, class TEnum, class T, class TBase>
	void AddPropertyFlags(PropertyMap<T, TBase> & map, std::string const & name, std::uintptr_t offset)
	{
		using Enum = EnumInfo<TEnum>;

		PropertyMap<T, TBase>::PropertyInfo info;
		info.Type = GetPropertyType<TValue>();
		info.Offset = offset;
		info.Flags = 0;
		map.Properties.insert(std::make_pair(name, info));

		for (auto i = 0; i < std::size(Enum::Values); i++) {
			PropertyMap<T, TBase>::FlagInfo flag;
			flag.Property = name;
			flag.Flags = kPropRead | kPropWrite;
			flag.Mask = (int64_t)Enum::Values[i].Val;
			map.Flags.insert(std::make_pair(Enum::Values[i].Name, flag));
		}
	}
}
