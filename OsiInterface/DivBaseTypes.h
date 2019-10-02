#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <cassert>
#include <optional>
#include <glm/glm.hpp>

namespace osidbg
{
#pragma pack(push, 1)
	using Vector3 = glm::vec3;

	template <class T>
	struct PrimitiveSet
	{
		T * Buf;
		uint32_t AllocationSize;
		uint32_t ItemCount;
	};

	struct FixedString
	{
		inline FixedString()
			: Str(nullptr)
		{}

		inline FixedString(char const * s)
			: Str(s)
		{}

		inline FixedString(FixedString const & fs)
			: Str(fs.Str)
		{}

		inline bool operator == (FixedString const & fs) const
		{
			return Str == fs.Str;
		}

		inline bool operator != (FixedString const & fs) const
		{
			return Str != fs.Str;
		}

		inline bool operator !() const
		{
			return Str == nullptr;
		}

		inline explicit operator bool() const
		{
			return Str != nullptr;
		}

		char const * Str;
	};

	FixedString ToFixedString(const char * s);

	template <class ValueType>
	struct FixedStringMapBase
	{
		struct Node
		{
			Node * Next;
			FixedString Key;
			ValueType Value;
		};

		uint32_t HashSize;
		uint32_t _Unused;
		Node ** HashTable;
		uint32_t ItemCount;

		ValueType * Find(char const * str) const
		{
			auto fs = ToFixedString(str);
			if (fs) {
				auto item = HashTable[(uint64_t)fs.Str % HashSize];
				while (item != nullptr) {
					if (strcmp(str, item->Key.Str) == 0) {
						return &item->Value;
					}

					item = item->Next;
				}
			}

			return nullptr;
		}

		FixedString * FindByValue(ValueType const & value) const
		{
			for (uint32_t bucket = 0; bucket < HashSize; bucket++) {
				Node * item = HashTable[bucket];
				while (item != nullptr) {
					if (value == item->Value) {
						return &item->Key;
					}

					item = item->Next;
				}
			}

			return nullptr;
		}
	};

	template <class ValueType>
	struct FixedStringRefMap
	{
		struct Node
		{
			Node * Next;
			FixedString Key;
			ValueType Value;
		};

		uint32_t ItemCount;
		uint32_t HashSize;
		Node ** HashTable;

		ValueType * Find(char const * str) const
		{
			auto fs = ToFixedString(str);
			if (fs) {
				auto item = HashTable[(uint64_t)fs.Str % HashSize];
				while (item != nullptr) {
					if (fs.Str == item->Key.Str) {
						return &item->Value;
					}

					item = item->Next;
				}
			}

			return nullptr;
		}
	};

	template <class T>
	struct Set
	{
		T * Buf;
		uint32_t Capacity;
		uint32_t Size;
		uint64_t CapacityIncrementSize;
	};

	template <class T>
	struct ObjectSet : public Set<T>
	{
		virtual ~ObjectSet();
	};

	struct TranslatedString
	{
		void * Unknown;
		char const * String;
		char const * TranslatedStringKey;
	};

	struct STDWString
	{
		union {
			uint16_t Buf[8];
			uint16_t * BufPtr;
		};
		uint64_t Size;
		uint64_t Capacity;
	};

	template <class T>
	struct Array
	{
		void * VMT{ nullptr };
		T * Buf{ nullptr };
		uint32_t Capacity{ 0 };
		uint32_t Size{ 0 };
		uint32_t Unkn[2]{ 0 };

		void Clear()
		{
			Size = 0;
		}

		bool SafeAdd(T const & val)
		{
			if (Size < Capacity) {
				Buf[Size++] = val;
				return true;
			} else {
				return false;
			}
		}

		void Remove(uint32_t index)
		{
			for (auto i = index + 1; i < Size; i++) {
				Buf[index] = Buf[index + 1];
			}

			Size--;
		}
	};

	struct ObjectHandle
	{
		uint64_t Handle;

		inline ObjectHandle()
			: Handle(0)
		{}

		inline ObjectHandle(uint64_t handle)
			: Handle(handle)
		{}

		inline ObjectHandle(int64_t handle)
			: Handle((uint64_t)handle)
		{}

		inline ObjectHandle(uint64_t type, uint64_t index, uint64_t salt)
		{
			if (type >= 0x400 || salt >= 0x400000) {
				Fail("ObjectHandle type/salt overflow.");
			}

			Handle = index | (salt << 32) | (type << 54);
		}

		inline ObjectHandle(ObjectHandle const & oh)
			: Handle(oh.Handle)
		{}

		inline ObjectHandle & operator = (ObjectHandle const & oh)
		{
			Handle = oh.Handle;
			return *this;
		}

		inline bool operator == (ObjectHandle const & oh) const
		{
			return Handle == oh.Handle;
		}

		inline uint32_t GetType() const
		{
			return Handle >> 54;
		}

		inline uint32_t GetSalt() const
		{
			return (Handle >> 32) & 0x3fffff;
		}

		inline uint32_t GetIndex() const
		{
			return (uint32_t)(Handle & 0xffffffff);
		}

		explicit inline operator bool() const
		{
			return Handle != 0;
		}

		inline bool operator !() const
		{
			return Handle == 0;
		}

		explicit inline operator int64_t() const
		{
			return (int64_t)Handle;
		}
	};

	struct ObjectFactory
	{
		void * VMT;
		Array<void *> CharPtrArray;
		Array<unsigned int> IntArray;
		void * unkn;
		uint32_t unkn2[4];
		PrimitiveSet<void *> CharPrimitives;
		uint64_t unkn3[3];
	};

	struct Component
	{
		virtual void VMT00() = 0;
		virtual void VMT08() = 0;
		virtual void VMT10() = 0;
		virtual void VMT18() = 0;
		virtual void * FindComponentByHandle(ObjectHandle const * oh) = 0;
		virtual void VMT28() = 0;
		virtual void * FindComponentByGuid(FixedString const * fs) = 0;
	};

	struct ComponentHandle
	{
		uint64_t TypeId;
		ObjectHandle Handle;
	};

	struct BaseComponent
	{
		void * VMT;
		ObjectHandle Handle;
		ComponentHandle Component;
	};

	struct GlobalStringTable
	{
		static bool UseMurmur;

		struct Entry
		{
			const char * StringPtrs[10];
			Entry * Next;
			uint32_t StringPtrItems;
			uint32_t Unused;

			uint32_t Count() const;
			char const * Get(uint32_t i) const;
		};

		Entry HashTable[65521];

		const char * Find(char const * s, uint64_t length) const;
		static uint32_t Hash(char const * s, uint64_t length);
	};
#pragma pack(pop)

}