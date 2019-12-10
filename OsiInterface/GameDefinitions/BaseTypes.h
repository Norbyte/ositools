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
#if defined(OSI_EOCAPP)
	typedef void * (* EoCAllocFunc)(void * self, std::size_t size);
#else
	typedef void * (* EoCAllocFunc)(void * self, std::size_t size, char const * file, int line, char const * function);
#endif
	typedef void (* EoCFreeFunc)(void * self, void * ptr);

	extern EoCAllocFunc EoCAlloc;
	extern EoCFreeFunc EoCFree;

	void * GameAllocRaw(std::size_t size);
	void GameFree(void *);

	template <class T>
	T * GameAlloc()
	{
		auto ptr = reinterpret_cast<T *>(GameAllocRaw(sizeof(T)));
		new (ptr) T();
		return ptr;
	}

	template <class T>
	T * GameAlloc(std::size_t n)
	{
		auto ptr = reinterpret_cast<T *>(GameAllocRaw(sizeof(T) * n));
		for (auto i = 0; i < n; i++) {
			new (ptr + i) T();
		}
		return ptr;
	}

#pragma pack(push, 1)
	using Vector3 = glm::vec3;
	using NetId = uint32_t;

	constexpr NetId NetIdUnassigned = 0xffffffff;

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

		inline explicit FixedString(char const * s)
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

		void Init(uint32_t hashSize)
		{
			HashSize = hashSize;
			HashTable = GameAlloc<Node *>(hashSize);
			ItemCount = 0;
			memset(HashTable, 0, sizeof(Node *) * hashSize);
		}

		bool Add(FixedString key, ValueType const & value)
		{
			if (!key) return false;

			auto * item = &HashTable[(uint64_t)key.Str % HashSize];
			while (*item != nullptr) {
				if (strcmp(key.Str, (*item)->Key.Str) == 0) {
					(*item)->Value = value;
					return true;
				}

				item = &(*item)->Next;
			}

			auto node = GameAlloc<Node>();
			node->Next = nullptr;
			node->Key = key;
			node->Value = value;
			*item = node;
			return true;
		}

		ValueType * Find(FixedString fs) const
		{
			if (!fs) return nullptr;

			auto item = HashTable[(uint64_t)fs.Str % HashSize];
			while (item != nullptr) {
				if (strcmp(fs.Str, item->Key.Str) == 0) {
					return &item->Value;
				}

				item = item->Next;
			}

			return nullptr;
		}

		ValueType * Find(char const * str) const
		{
			auto fs = ToFixedString(str);
			return Find(fs);
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

		template <class Visitor>
		void Iterate(Visitor visitor)
		{
			for (uint32_t bucket = 0; bucket < HashSize; bucket++) {
				Node * item = HashTable[bucket];
				while (item != nullptr) {
					visitor(item->Key, item->Value);
					item = item->Next;
				}
			}
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

		ValueType * Find(FixedString fs) const
		{
			if (!fs) return nullptr;

			auto item = HashTable[(uint64_t)fs.Str % HashSize];
			while (item != nullptr) {
				if (fs.Str == item->Key.Str) {
					return &item->Value;
				}

				item = item->Next;
			}

			return nullptr;
		}

		ValueType * Find(char const * str) const
		{
			auto fs = ToFixedString(str);
			return Find(fs);
		}
	};

	template <class T>
	struct CompactSet
	{
		T * Buf{ nullptr };
		uint32_t Capacity{ 0 };
		uint32_t Size{ 0 };
	};

	template <class T>
	struct Set : public CompactSet<T>
	{
		uint64_t CapacityIncrementSize{ 0 };
	};

	template <class T>
	struct ObjectSet
	{
		void * VMT{ nullptr };
		Set<T> Set;
	};

	template <class T>
	struct CompactObjectSet
	{
		void * VMT{ nullptr };
		CompactSet<T> Set;
	};

	template <unsigned TDWords>
	struct BitArray
	{
		uint32_t Bits[TDWords];

		inline bool Set(uint32_t index)
		{
			if (index <= 0 || index > (TDWords * 32)) {
				return false;
			}

			Bits[(index - 1) >> 5] |= (1 << ((index - 1) & 0x1f));
			return true;
		}

		inline bool Clear(uint32_t index)
		{
			if (index <= 0 || index > (TDWords * 32)) {
				return false;
			}

			Bits[(index - 1) >> 5] &= ~(1 << ((index - 1) & 0x1f));
			return true;
		}

		inline bool IsSet(uint32_t index)
		{
			if (index <= 0 || index > (TDWords * 32)) {
				return false;
			}

			return (Bits[(index - 1) >> 5] & (1 << ((index - 1) & 0x1f))) != 0;
		}
	};

	struct STDWString
	{
		union {
			wchar_t Buf[8];
			wchar_t * BufPtr{ nullptr };
		};
		uint64_t Size{ 0 };
		uint64_t Capacity{ 7 };

		inline wchar_t const * GetPtr() const
		{
			if (Size > 7) {
				return BufPtr;
			} else {
				return Buf;
			}
		}

		void Set(std::string const & s);
		void Set(std::wstring const & s);
	};

	struct StringView
	{
		char * Buf;
		uint64_t Size;
	};

	struct STDString
	{
		union {
			char Buf[16];
			char * BufPtr{ nullptr };
		};
		uint64_t Size{ 0 };
		uint64_t Capacity{ 15 };

		inline char const * GetPtr() const
		{
			if (Size > 15) {
				return BufPtr;
			} else {
				return Buf;
			}
		}

		void Set(std::string const & s);

		inline bool operator < (STDString const & other) const
		{
			return _stricmp(GetPtr(), other.GetPtr()) < 0;
		}
	};

	struct Path
	{
		STDString Name;
		uint32_t Unknown{ 0 };
		uint32_t _Pad;
	};

	struct RuntimeStringHandle
	{
		void * VMT;
		FixedString FS;
		STDString Str;
		STDWString WStr;
	};

	struct TranslatedString
	{
		void * VMT;
		RuntimeStringHandle Str1;
		RuntimeStringHandle Str2;
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
		virtual void Destroy() = 0;
		virtual void DestroyComponent() = 0;
		virtual void CreateComponent() = 0;
		virtual void ForceCreateComponent() = 0;
		virtual void * FindComponentByHandle(ObjectHandle const * oh) = 0;
		virtual void TryFindComponentByHandle() = 0;
		virtual void * FindComponentByGuid(FixedString const * fs) = 0;
		virtual void MoveComponentByGuid() = 0;
		virtual void GetComponentByNetId() = 0;
		virtual void UNKN() = 0;
		virtual void GetComponentByIndex() = 0;
		virtual void GetFreeHandle() = 0;
		virtual void IsFreeIndex() = 0;
		virtual void IsReservedIndex() = 0;
		virtual void ReserveIndex() = 0;
		virtual void UnreserveIndex() = 0;
	};

	struct ComponentHandle
	{
		int64_t TypeId;
		ObjectHandle Handle;

		inline bool IsValid() const
		{
			return TypeId != -1 && !!Handle;
		}
	};

	struct BaseComponent
	{
		void * VMT;
		ObjectHandle EntityObjectHandle;
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

	struct ScratchBuffer
	{
		void * Buffer;
		uint32_t Unkn;
		uint32_t Size;
		uint32_t WritePosition;
		uint32_t ReadPosition;
		uint32_t GrowSize;
	};
#pragma pack(pop)
}


namespace std
{
	template<> struct hash<osidbg::FixedString>
	{
		typedef osidbg::FixedString argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<std::uintptr_t>{}((std::uintptr_t)fn.Str);
		}
	};

	template<> struct hash<osidbg::ObjectHandle>
	{
		typedef osidbg::ObjectHandle argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<uint64_t>{}(fn.Handle);
		}
	};
}
