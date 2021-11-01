#include <stdafx.h>
#include <GameDefinitions/Symbols.h>

BEGIN_SE()

void* MSVCMemoryAllocator::Alloc(std::size_t size)
{
	return GetStaticSymbols().CrtAlloc(size);
}

void * GameAllocRaw(std::size_t size)
{
#if defined(OSI_EOCAPP)
	return GetStaticSymbols().EoCAlloc(nullptr, size);
#else
	return GetStaticSymbols().EoCAlloc(nullptr, size, "", 1, "");
#endif
}

void GameFree(void * ptr)
{
	GetStaticSymbols().EoCFree(nullptr, ptr);
}

void* CrtAllocRaw(std::size_t size)
{
	return GetStaticSymbols().CrtAlloc(size);
}

void CrtFree(void* ptr)
{
	GetStaticSymbols().CrtFree(ptr);
}

unsigned int Primes[290] = {
	2, 3, 5, 7, 0x0B, 0x0D, 0x11, 0x13, 0x17, 0x1D, 0x1F, 0x25,
	0x29, 0x2B, 0x2F, 0x35, 0x3B, 0x3D, 0x43, 0x47, 0x49, 0x4F, 0x53,
	0x59, 0x61, 0x65, 0x71, 0x7F, 0x83, 0x95, 0x97, 0x0A3, 0x0AD,
	0x0B5, 0x0BF, 0x0D3, 0x0DF, 0x0E9, 0x0F1, 0x0FB, 0x107, 0x10F,
	0x119, 0x125, 0x133, 0x137, 0x14B, 0x15B, 0x161, 0x16F, 0x175,
	0x17F, 0x18D, 0x191, 0x1A3, 0x1A5, 0x1AF, 0x1BB, 0x1C9, 0x1CD,
	0x1DF, 0x1E7, 0x1EB, 0x1F7, 0x209, 0x21D, 0x22D, 0x233, 0x23B,
	0x24B, 0x251, 0x259, 0x265, 0x277, 0x281, 0x28D, 0x295, 0x2A1,
	0x2AB, 0x2B3, 0x2BD, 0x2CF, 0x2D7, 0x2DD, 0x2E7, 0x2EF, 0x2F9,
	0x305, 0x313, 0x31D, 0x329, 0x32B, 0x335, 0x347, 0x355, 0x35F,
	0x36D, 0x371, 0x38B, 0x38F, 0x3A1, 0x3A9, 0x3AD, 0x3B9, 0x3C7,
	0x3CB, 0x3D7, 0x3DF, 0x3F1, 0x41B, 0x44F, 0x47F, 0x4B1, 0x4EB,
	0x515, 0x5AB, 0x641, 0x679, 0x6AD, 0x6D9, 0x709, 0x79F, 0x7D3,
	0x805, 0x89B, 0x8CB, 0x905, 0x92F, 0x9C7, 0x9F7, 0x0A31, 0x0A61,
	0x0A93, 0x0AC1, 0x0AF1, 0x0B23, 0x0B57, 0x0B89, 0x0BB9, 0x0CB3,
	0x0CE5, 0x0D1F, 0x0D4F, 0x0D81, 0x0E17, 0x0E4B, 0x0E75, 0x0F0B,
	0x0F43, 0x0FD3, 0x1069, 0x109D, 0x1139, 0x1163, 0x119B, 0x122B,
	0x125F, 0x128F, 0x12C1, 0x1357, 0x138B, 0x13BB, 0x13ED, 0x1421,
	0x1459, 0x14E7, 0x151F, 0x15B5, 0x1645, 0x16DB, 0x170F, 0x1741,
	0x1777, 0x17A5, 0x17D5, 0x1807, 0x183B, 0x1871, 0x189D, 0x18D1,
	0x19CF, 0x19FD, 0x1A2D, 0x1AC9, 0x1AFB, 0x1B2F, 0x1B59, 0x1B91,
	0x1BBF, 0x1BEF, 0x1C27, 0x1C55, 0x1C8B, 0x1CB7, 0x1D53, 0x1D87,
	0x1DB3, 0x1E49, 0x1EDD, 0x1F0F, 0x1F49, 0x1F75, 0x1FA5, 0x20A1,
	0x2135, 0x2231, 0x2263, 0x22F7, 0x2329, 0x2363, 0x238F, 0x23BF,
	0x23F3, 0x2429, 0x24BB, 0x254F, 0x2581, 0x267B, 0x26AD, 0x2717,
	0x2905, 0x2AFB, 0x2CEF, 0x2EE7, 0x30D7, 0x32C9, 0x34C9, 0x36B9,
	0x38A7, 0x3AA5, 0x3C97, 0x3E81, 0x4087, 0x4273, 0x4465, 0x465D,
	0x4847, 0x4A39, 0x4C2D, 0x4E2B, 0x501B, 0x5209, 0x53FF, 0x55F3,
	0x57E5, 0x59DB, 0x5BD5, 0x5DC1, 0x5FBD, 0x61B5, 0x63B3, 0x6593,
	0x6785, 0x6983, 0x6B75, 0x6D61, 0x6F61, 0x7151, 0x733D, 0x753B,
	0x772D, 0x7925, 0x7B17, 0x7D03, 0x7EF7, 0x80F5, 0x82DF, 0x84E3,
	0x86C5, 0x88CF, 0x8AB3, 0x8CA7, 0x8EAB, 0x908B, 0x927D, 0x947B,
	0x9665, 0x986B, 0x9A4F, 0x9C49, 0x9E3B, 0x0A033, 0x0A223, 0x0A41D,
	0x0A60D, 0x0A7FB, 0x0A9FD, 0x0ABF1, 0x0ADD5, 0x0AFCF
};

unsigned int GetNearestLowerPrime(unsigned int num)
{
	if (num < 2) return 2;
	if (num >= 45007) return 45007;

	unsigned start = 0;
	unsigned end = (unsigned)std::size(Primes);

	while (start < end) {
		auto mid = (start + end) >> 1;
		auto delta = (int64_t)num - Primes[mid];
		if (delta >= 0) {
			if (delta == 0) {
				return Primes[mid];
			}

			start = mid + 1;
		} else {
			end = (start + end) >> 1;
		}
	}

	return Primes[start];
}

uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed)
{
	uint32_t h = seed;
	if (len > 3) {
		size_t i = len >> 2;
		do {
			uint32_t k;
			memcpy(&k, key, sizeof(uint32_t));
			key += sizeof(uint32_t);
			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			h ^= k;
			h = (h << 13) | (h >> 19);
			h = h * 5 + 0xe6546b64;
		} while (--i);
	}
	if (len & 3) {
		size_t i = len & 3;
		uint32_t k = 0;
		do {
			k <<= 8;
			k |= key[i - 1];
		} while (--i);
		k *= 0xcc9e2d51;
		k = (k << 15) | (k >> 17);
		k *= 0x1b873593;
		h ^= k;
	}
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

uint32_t GlobalStringTable::Entry::Count() const
{
	if (Next) {
		return StringPtrItems + Next->Count();
	}
	else {
		return StringPtrItems;
	}
}

char const * GlobalStringTable::Entry::Get(uint32_t i) const
{
	if (i < StringPtrItems) {
		return StringPtrs[i];
	}
	else {
		return Next->Get(i - StringPtrItems);
	}
}

const char * GlobalStringTable::Find(char const * s, uint64_t length) const
{
	auto const & entry = HashTable[Hash(s, length)];
	auto numItems = entry.Count();
	for (uint32_t i = 0; i < numItems; i++) {
		const char * str = entry.Get(i);
		if (str) {
			auto metadata = reinterpret_cast<FixedString::Metadata*>(const_cast<char*>(str - 0x10));
			if (metadata->Length == length && memcmp(s, str, length) == 0) {
				return str;
			}
		}
	}

	return nullptr;
}

uint32_t GlobalStringTable::Hash(char const * s, uint64_t length)
{
	return murmur3_32((const uint8_t *)s, length, 0) % 0xFFF1;
}

const char* FixedString::FindGlobalString(const char * s)
{
	if (s == nullptr) {
		OsiErrorS("Attempted to look up a null string!");
		return nullptr;
	}

	auto stringTable = GetStaticSymbols().GetGlobalStringTable();
	if (stringTable == nullptr) {
		OsiErrorS("Global string table not available!");
		return nullptr;
	}

	return stringTable->Find(s, strlen(s));
}

FixedString::FixedString(char const* s)
{
	if (s == nullptr) {
		Str = nullptr;
	} else {
		auto str = FindGlobalString(s);
		if (!str) {
			auto createFixedString = GetStaticSymbols().CreateFixedString;
			if (createFixedString != nullptr) {
#if defined(OSI_EOCAPP)
				str = createFixedString(s, -1);
#else
				FixedString fs;
				createFixedString(&fs, s, -1);
				str = fs.Str;
				fs.Str = nullptr;
#endif
			} else {
				OsiErrorS("ls::FixedString::Create not available!");
			}
		}

		Str = str;
		IncRef();
	}
}

bool IsHexChar(char c)
{
	return (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'f');
}

bool IsValidGuidString(const char * s)
{
	auto len = strlen(s);
	if (len < 36) return false;

	auto guidPos = len - 36;
	unsigned const nibbles[5] = { 8, 4, 4, 4, 12 };

	for (auto n = 0; n < 5; n++) {
		for (unsigned i = 0; i < nibbles[n]; i++) {
			if (!IsHexChar(s[guidPos++])) return false;
		}

		if (n != 4 && s[guidPos++] != '-') return false;
	}

	return true;
}

FixedString NameGuidToFixedString(char const * nameGuid)
{
	if (nameGuid == nullptr) {
		return FixedString{};
	}

	auto nameLen = strlen(nameGuid);
	if (!IsValidGuidString(nameGuid)) {
		OsiError("GUID (" << nameGuid << ") malformed!");
		return FixedString{};
	}

	auto guid = nameGuid + nameLen - 36;
	return FixedString(guid);
}

END_SE()