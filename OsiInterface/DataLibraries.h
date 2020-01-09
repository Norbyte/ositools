#pragma once

#include <GameDefinitions/Character.h>
#include <GameDefinitions/CustomStats.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/Osiris.h>
#include <GameDefinitions/Status.h>
#include <GameDefinitions/ActionMachine.h>
#include "Wrappers.h"
#include <optional>

namespace osidbg {

	struct Pattern
	{
		void FromString(std::string const & s);
		void FromRaw(const char * s);
		void Scan(uint8_t const * start, size_t length, std::function<std::optional<bool> (uint8_t const *)> callback, bool multiple = true);

	private:
		struct PatternByte
		{
			uint8_t pattern;
			uint8_t mask;
		};

		std::vector<PatternByte> pattern_;

		bool MatchPattern(uint8_t const * start);
		void ScanPrefix1(uint8_t const * start, uint8_t const * end, std::function<std::optional<bool> (uint8_t const *)> callback, bool multiple);
		void ScanPrefix2(uint8_t const * start, uint8_t const * end, std::function<std::optional<bool> (uint8_t const *)> callback, bool multiple);
		void ScanPrefix4(uint8_t const * start, uint8_t const * end, std::function<std::optional<bool> (uint8_t const *)> callback, bool multiple);
	};

	uint8_t const * AsmCallToAbsoluteAddress(uint8_t const * call);
	uint8_t const * AsmLeaToAbsoluteAddress(uint8_t const * lea);

	struct SymbolMappingCondition
	{
		enum MatchType
		{
			kNone,
			kString, // Match string
			kFixedString, // Match a FixedString reference
		};

		MatchType Type{ kNone };
		int32_t Offset{ 0 };
		char const * String{ nullptr };
	};

	enum class SymbolMappingResult
	{
		Success,
		Fail,
		TryNext
	};

	struct SymbolMappingTarget
	{
		typedef SymbolMappingResult (* HandlerProc)(uint8_t const *);

		enum ActionType
		{
			kNone,
			kAbsolute, // Save absolute value (p + Offset)
			kIndirectCall, // Save AsmCallToAbsolutePtr(p + Offset)
			kIndirectLea // Save AsmLeaToAbsolutePtr(p + Offset)
		};

		char const * Name{ nullptr };
		ActionType Type{ kNone };
		int32_t Offset{ 0 };
		void ** TargetPtr{ nullptr };
		HandlerProc Handler{ nullptr };
		struct SymbolMappingData const * NextSymbol{ nullptr };
		int32_t NextSymbolSeekSize{ 0 };
	};

	struct SymbolMappingData
	{
		enum MatchScope
		{
			kBinary, // Full binary
			kText, // .text segment of binary
			kCustom // Custom scope (specified as scan parameter)
		};

		enum Flags : uint32_t
		{
			kCritical = 1 << 0, // Mapping failure causes a critical error
			kDeferred = 1 << 1, // Perform mapping after fixed string pool was loaded
			kAllowFail = 1 << 2, // Allow mapping to fail without throwing an error
		};

		char const * Name{ nullptr };
		MatchScope Scope{ SymbolMappingData::kText };
		uint32_t Flag{ 0 };
		char const * Matcher;
		SymbolMappingCondition Conditions;
		SymbolMappingTarget Target1;
		SymbolMappingTarget Target2;
		SymbolMappingTarget Target3;
	};

	class LibraryManager
	{
	public:
		bool FindLibraries();
		bool PostStartupFindLibraries();
		void EnableCustomStats();
		void Cleanup();

		void ShowStartupError(std::wstring const & msg, bool wait, bool exitGame);

		bool EvaluateSymbolCondition(SymbolMappingCondition const & cond, uint8_t const * match);
		SymbolMappingResult ExecSymbolMappingAction(SymbolMappingTarget const & target, uint8_t const * match);
		bool MapSymbol(SymbolMappingData const & mapping, uint8_t const * customStart, std::size_t customSize);

		inline uint8_t const * GetModuleStart() const
		{
			return moduleStart_;
			size_t moduleSize_{ 0 };
		}

		inline size_t GetModuleSize() const
		{
			return moduleSize_;
		}

		inline bool CriticalInitializationFailed() const
		{
			return CriticalInitFailed;
		}

		inline bool InitializationFailed() const
		{
			return InitFailed;
		}

		enum class StatusGetEnterChanceTag {};
		WrappableFunction<StatusGetEnterChanceTag, int32_t(esv::Status *, bool)> StatusGetEnterChance;
		
		enum class StatusHealEnterTag {};
		HookableFunction<StatusHealEnterTag, bool (esv::Status *)> StatusHealEnter;

		enum class StatusHitEnterTag {};
		HookableFunction<StatusHitEnterTag, bool (esv::Status *)> StatusHitEnter;

		enum class CharacterHitTag {};
		WrappableFunction<CharacterHitTag, void (esv::Character * , CDivinityStats_Character *, CDivinityStats_Item *, DamagePairList *,
			HitType, bool, HitDamageInfo *, int, void *, HighGroundBonus, bool, CriticalRoll)> CharacterHitHook;

		enum class ApplyStatusTag {};
		WrappableFunction<ApplyStatusTag, void (esv::StatusMachine *, esv::Status *)> ApplyStatusHook;

		enum class ActionMachineSetStateTag {};
		HookableFunction<ActionMachineSetStateTag, bool (esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer)> ActionMachineSetStateHook;

		enum class SkillPrototypeFormatDescriptionParamTag {};
		WrappableFunction<SkillPrototypeFormatDescriptionParamTag, void(SkillPrototype *skillPrototype, CDivinityStats_Character *tgtCharStats,
			eoc::Text *eocText, int paramIndex, __int64 isFromItem, float xmm9_4_0, FixedString * paramText,
			ObjectSet<STDString> * stdStringSet)> SkillPrototypeFormatDescriptionParamHook;

	private:

		void MapAllSymbols(bool deferred);
		void FindTextSegment();

#if defined(OSI_EOCAPP)
		bool FindEoCApp(uint8_t const * & start, size_t & size);
		void FindServerGlobalsEoCApp();
		void FindEoCGlobalsEoCApp();
		void FindGlobalStringTableEoCApp();
#else
		bool FindEoCPlugin(uint8_t const * & start, size_t & size);
		void FindExportsEoCPlugin();
		void FindServerGlobalsEoCPlugin();
		void FindEoCGlobalsEoCPlugin();
		void FindGlobalStringTableCoreLib();
#endif

		bool IsConstStringRef(uint8_t const * ref, char const * str) const;
		bool IsFixedStringRef(uint8_t const * ref, char const * str) const;
		bool CanShowError();

		uint8_t const * moduleStart_{ nullptr };
		size_t moduleSize_{ 0 };
		uint8_t const * moduleTextStart_{ nullptr };
		size_t moduleTextSize_{ 0 };

#if !defined(OSI_EOCAPP)
		HMODULE coreLib_{ NULL };
		uint8_t const * coreLibStart_{ nullptr };
		size_t coreLibSize_{ 0 };
#endif

		bool InitFailed{ false };
		bool CriticalInitFailed{ false };
		bool PostLoaded{ false };
		bool EnabledCustomStats{ false };
		bool EnabledCustomStatsPane{ false };
	};
}
