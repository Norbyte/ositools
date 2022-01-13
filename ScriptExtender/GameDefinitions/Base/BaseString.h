#pragma once

#include <GameDefinitions/Base/BaseTypes.h>
#include <GameDefinitions/Base/BaseMemory.h>

BEGIN_SE()


using STDString = std::basic_string<char, std::char_traits<char>, GameAllocator<char>>;
using STDWString = std::basic_string<wchar_t, std::char_traits<wchar_t>, GameAllocator<wchar_t>>;
using StringView = std::string_view;
using WStringView = std::wstring_view;

STDString ToUTF8(WStringView s);
STDWString FromUTF8(StringView s);

struct FixedString
{
#if defined(OSI_EOCAPP)
	typedef char const* (CreateProc)(char const* str, int length);
#else
	typedef void (CreateProc)(FixedString* self, char const* str, int length);
#endif

	inline FixedString()
		: Str(nullptr)
	{}

	inline explicit FixedString(STDString const& s)
		: FixedString(s.data(), (int)s.size())
	{}

	explicit FixedString(char const* s, int length = -1);

	inline FixedString(FixedString const & fs)
		: Str(fs.Str)
	{
		IncRef();
	}

	inline FixedString(FixedString && fs)
		: Str(fs.Str)
	{
		fs.Str = nullptr;
	}

	inline ~FixedString()
	{
		DecRef();
	}

	inline FixedString & operator = (FixedString const& fs)
	{
		if (fs.Str != Str) {
			DecRef();
			Str = fs.Str;
			IncRef();
		}

		return *this;
	}

	inline FixedString& operator = (FixedString && fs)
	{
		Str = fs.Str;
		if (this != &fs) {
			fs.Str = nullptr;
		}

		return *this;
	}

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

	inline char const* GetString() const
	{
		return Str;
	}

	char const * Str;

	struct Metadata
	{
		std::atomic<int64_t> RefCount;
		uint16_t Length;
		uint16_t HashKey;
	};

private:

	inline void IncRef()
	{
		if (Str) {
			auto meta = (Metadata*)(const_cast<char *>(Str - 0x10));
			meta->RefCount++;
		}
	}

	inline void DecRef()
	{
		if (Str) {
			auto meta = (Metadata*)(const_cast<char*>(Str - 0x10));
			meta->RefCount--;
			// TODO - remove from global pool if refcount reaches 1!
		}
	}

	static const char* FindGlobalString(const char* s);
};

inline uint64_t Hash(FixedString const& s)
{
	return (uint64_t)s.Str;
}

FixedString NameGuidToFixedString(char const* nameGuid);
bool IsValidGuidString(const char* s);
FixedString GenerateGuid();

struct GlobalStringTable : public ProtectedGameObject<GlobalStringTable>
{
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
	void * Buffer{ nullptr };
	uint32_t Capacity{ 0 };
	uint32_t Size{ 0 };
	uint32_t WritePosition{ 0 };
	uint32_t ReadPosition{ 0 };
	uint32_t GrowSize{ 0 };

	ScratchBuffer();
	ScratchBuffer(ScratchBuffer const& o);
	ScratchBuffer(ScratchBuffer && o);
	~ScratchBuffer();

	ScratchBuffer& operator = (ScratchBuffer const& o);
	ScratchBuffer& operator = (ScratchBuffer&& o);
};

struct Path
{
	typedef StringView* (GetPrefixForRootProc)(StringView* path, unsigned int rootType);

	inline Path() {}

	inline Path(STDString const& s) : Name(s) {}

	STDString Name;
#if !defined(OSI_EOCAPP)
	uint32_t Unknown{ 0 };
#endif
};

struct RuntimeStringHandle
{
	RuntimeStringHandle();

	void * VMT{ nullptr };
	FixedString Handle;
	STDString Str;
	STDWString ReferenceString;
};

struct TranslatedString
{
	void* VMT{ nullptr };
	RuntimeStringHandle Handle;
	RuntimeStringHandle ArgumentString;
};

END_SE()


namespace std
{
	template<> struct hash<dse::FixedString>
	{
		typedef dse::FixedString argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& fn) const noexcept
		{
			return std::hash<std::uintptr_t>{}((std::uintptr_t)fn.Str);
		}
	};

	inline ostream& operator << (ostream& out, dse::FixedString const& str)
	{
		if (str) {
			out << str.Str;
		} else {
			out << "(null)";
		}
		return out;
	}
}
