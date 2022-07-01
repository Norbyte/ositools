#pragma once

#include <GameHooks/Wrappers.h>
#include <optional>
#include <unordered_set>

namespace tinyxml2 {
	class XMLDocument;
	class XMLElement;
}

BEGIN_SE()

class WriteAnchor
{
public:
	inline WriteAnchor(uint8_t const * ptr, std::size_t size)
		: ptr_(const_cast<uint8_t *>(ptr)),
		size_(size)
	{
		BOOL succeeded = VirtualProtect((LPVOID)ptr_, size_, PAGE_READWRITE, &oldProtect_);
		if (!succeeded) Fail("VirtualProtect() failed");
	}

	inline ~WriteAnchor()
	{
		BOOL succeeded = VirtualProtect((LPVOID)ptr_, size_, oldProtect_, &oldProtect_);
		if (!succeeded) Fail("VirtualProtect() failed");
	}

	inline uint8_t * ptr()
	{
		return ptr_;
	}

private:
	uint8_t * ptr_;
	std::size_t size_;
	DWORD oldProtect_;
};

struct Pattern
{
	enum class ScanAction
	{
		Continue,
		Finish
	};

	bool FromString(std::string_view s);
	void FromRaw(const char * s);
	void Scan(uint8_t const * start, size_t length, std::function<ScanAction (uint8_t const *)> callback) const;
	std::optional<uint32_t> GetAnchor(char const* anchor) const;

private:
	struct PatternByte
	{
		uint8_t pattern;
		uint8_t mask;
	};

	std::vector<PatternByte> pattern_;
	std::unordered_map<std::string, uint32_t> anchors_;

	bool MatchPattern(uint8_t const * start) const;
	void ScanPrefix1(uint8_t const * start, uint8_t const * end, std::function<ScanAction (uint8_t const *)> callback) const;
	void ScanPrefix2(uint8_t const * start, uint8_t const * end, std::function<ScanAction (uint8_t const *)> callback) const;
	void ScanPrefix4(uint8_t const * start, uint8_t const * end, std::function<ScanAction (uint8_t const *)> callback) const;
};

uint8_t const * AsmResolveInstructionRef(uint8_t const * code);

struct StaticSymbolRef
{
	std::ptrdiff_t Offset;
	void ** TargetPtr;

	inline StaticSymbolRef()
		: Offset(-1), TargetPtr(nullptr)
	{}

	inline StaticSymbolRef(void ** ptr)
		: Offset(-1), TargetPtr(ptr)
	{}

	explicit inline StaticSymbolRef(std::ptrdiff_t offset)
		: Offset(offset), TargetPtr(nullptr)
	{}

	void ** Get() const;

	inline operator bool() const
	{
		return Offset != -1 || TargetPtr != nullptr;
	}
};

#define STATIC_SYM(name) StaticSymbolRef(offsetof(StaticSymbols, name))
#define CHAR_GETTER_SYM(name) StaticSymbolRef(offsetof(StaticSymbols, CharStatsGetters) + offsetof(CharacterStatsGetters, name))

struct SymbolMappings
{
	enum class MatchType
	{
		kNone,
		kString, // Match string
		kWString, // Match UTF-16LE string
		kFixedString, // Match a FixedString reference
		kFixedStringIndirect // Match a pointer to a FixedString reference
	};

	struct Condition
	{
		MatchType Type{ MatchType::kNone };
		int32_t Offset{ 0 };
		std::string String;
		std::wstring WString;
	};

	enum ActionType
	{
		kNone,
		kAbsolute, // Save absolute value (p + Offset)
		kIndirect // Save AsmResolveIndirectRef(p + Offset)
	};

	struct Target
	{
		std::string Name;
		ActionType Type{ ActionType::kNone };
		int32_t Offset{ 0 };
		StaticSymbolRef TargetRef;
		std::string NextSymbol;
		int32_t NextSymbolSeekSize{ 0 };
		std::string EngineCallback;
	};

	enum class SymbolVersion
	{
		None,
		Below,
		AboveOrEqual
	};

	enum class MatchScope
	{
		kBinary, // Full binary
		kText, // .text segment of binary
		kCustom // Custom scope (specified as scan parameter)
	};

	struct Mapping
	{
		struct VersionRequirement
		{
			SymbolVersion Type{ SymbolVersion::None };
			uint32_t Revision{ 0 };
		};

		enum Flags : uint32_t
		{
			kCritical = 1 << 0, // Mapping failure causes a critical error
			kDeferred = 1 << 1, // Perform mapping after fixed string pool was loaded
			kAllowFail = 1 << 2, // Allow mapping to fail without throwing an error
		};

		std::string Name;
		std::string Module;
		MatchScope Scope{ MatchScope::kText };
		uint32_t Flag{ 0 };
		Pattern Pattern;
		std::vector<Condition> Conditions;
		std::vector<Target> Targets;
		VersionRequirement Version;
	};

	struct DllImport
	{
		std::string Symbol;
		std::string Module;
		std::string Proc;
		StaticSymbolRef TargetRef;
	};

	struct StaticSymbol
	{
		int Offset{ 0 };
		bool Bound{ false };
	};

	std::unordered_map<std::string, Mapping> Mappings;
	std::unordered_map<std::string, DllImport> DllImports;
	std::unordered_map<std::string, StaticSymbol> StaticSymbols;
};

class SymbolMappingLoader
{
public:
	inline SymbolMappingLoader(SymbolMappings& mappings)
		: mappings_(mappings)
	{}

	void AddKnownModule(std::string const& name);
	bool LoadBuiltinMappings();
	bool LoadMappings(tinyxml2::XMLDocument* doc);

private:
	SymbolMappings& mappings_;
	std::unordered_set<std::string> knownModules_;

	bool LoadMappingsNode(tinyxml2::XMLElement* mappingsNode);
	bool LoadMapping(tinyxml2::XMLElement* mapping, SymbolMappings::Mapping& sym);
	bool LoadDllImport(tinyxml2::XMLElement* mapping, SymbolMappings::DllImport& imp);
	bool LoadTarget(tinyxml2::XMLElement* ele, Pattern const& pattern, SymbolMappings::Target& target);
	bool LoadCondition(tinyxml2::XMLElement* ele, Pattern const& pattern, SymbolMappings::Condition& condition);
};

class SymbolMapper
{
public:
	enum class MappingResult
	{
		Success,
		Fail,
		TryNext
	};

	struct ModuleInfo
	{
		uint8_t const* ModuleStart{ nullptr };
		size_t ModuleSize{ 0 };
		uint8_t const* ModuleTextStart{ nullptr };
		size_t ModuleTextSize{ 0 };
	};

	inline SymbolMapper(SymbolMappings& mappings)
		: mappings_(mappings)
	{}

	bool AddModule(std::string const& name, std::wstring const& modName);
	void AddEngineCallback(std::string const& name, std::function<MappingResult (uint8_t const *)> const& cb);
	void MapAllSymbols(bool deferred);
	bool MapSymbol(std::string const& mappingName, uint8_t const* customStart, std::size_t customSize);
	bool MapSymbol(SymbolMappings::Mapping const& mapping, uint8_t const* customStart, std::size_t customSize);
	bool MapDllImport(SymbolMappings::DllImport const& imp);

	inline bool HasFailedCriticalMappings() const
	{
		return hasFailedCriticalMappings_;
	}

	inline bool HasFailedMappings() const
	{
		return hasFailedMappings_;
	}

	inline std::unordered_map<std::string, ModuleInfo> const& Modules() const
	{
		return modules_;
	}

private:
	SymbolMappings& mappings_;
	std::unordered_map<std::string, ModuleInfo> modules_;
	std::unordered_map<std::string, std::function<MappingResult(uint8_t const*)>> engineCallbacks_;
	uint32_t gameRevision_;
	bool hasFailedMappings_{ false };
	bool hasFailedCriticalMappings_{ false };

	bool IsValidModulePtr(uint8_t const* ref) const;
	bool IsConstStringRef(uint8_t const* ref, char const* str) const;
	bool IsConstWStringRef(uint8_t const* ref, wchar_t const* str) const;
	bool IsFixedStringRef(uint8_t const* ref, char const* str) const;
	bool IsIndirectFixedStringRef(uint8_t const* ref, char const* str) const;

	bool EvaluateSymbolCondition(SymbolMappings::Condition const& cond, uint8_t const* match);
	MappingResult ExecSymbolMappingAction(SymbolMappings::Target const& target, uint8_t const* match);
};

END_SE()
