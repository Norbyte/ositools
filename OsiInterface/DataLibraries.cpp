#include "stdafx.h"
#include "DataLibraries.h"
#include <string>
#include <functional>
#include <psapi.h>

namespace osidbg
{
	void InitPropertyMaps();

	decltype(LibraryManager::StatusHealEnter) * decltype(LibraryManager::StatusHealEnter)::gHook;
	decltype(LibraryManager::StatusHitEnter) * decltype(LibraryManager::StatusHitEnter)::gHook;
	decltype(LibraryManager::CharacterHitHook) * decltype(LibraryManager::CharacterHitHook)::gHook;
	decltype(LibraryManager::ApplyStatusHook) * decltype(LibraryManager::ApplyStatusHook)::gHook;

	bool GlobalStringTable::UseMurmur = false;

	uint8_t CharToByte(char c)
	{
		if (c >= '0' && c <= '9') {
			return c - '0';
		}
		else if (c >= 'A' && c <= 'F') {
			return c - 'A' + 0x0A;
		}
		else if (c >= 'a' && c <= 'f') {
			return c - 'a' + 0x0A;
		}
		else {
			Fail("Invalid hexadecimal character");
		}
	}

	uint8_t HexByteToByte(char c1, char c2)
	{
		uint8_t hi = CharToByte(c1);
		uint8_t lo = CharToByte(c2);
		return (hi << 4) | lo;
	}

	void Pattern::FromString(std::string const & s)
	{
		if (s.size() % 3) Fail("Invalid pattern length");
		auto len = s.size() / 3;
		if (!len) Fail("Zero-length patterns not allowed");

		pattern_.reserve(len);

		char const * c = s.data();
		for (auto i = 0; i < len; i++) {
			PatternByte b;
			if (c[2] != ' ') Fail("Bytes must be separated by space");
			if (c[0] == 'X' && c[1] == 'X') {
				b.pattern = 0;
				b.mask = 0;
			}
			else {
				b.pattern = HexByteToByte(c[0], c[1]);
				b.mask = 0xff;
			}

			pattern_.push_back(b);
			c += 3;
		}

		if (pattern_[0].mask != 0xff) Fail("First byte of pattern must be an exact match");
	}

	void Pattern::FromRaw(const char * s)
	{
		auto len = strlen(s) + 1;
		pattern_.resize(len);
		for (auto i = 0; i < len; i++) {
			pattern_[i].pattern = (uint8_t)s[i];
			pattern_[i].mask = 0xFF;
		}
	}

	bool Pattern::MatchPattern(uint8_t const * start)
	{
		auto p = start;
		for (auto const & pattern : pattern_) {
			if ((*p++ & pattern.mask) != pattern.pattern) {
				return false;
			}
		}

		return true;
	}

	void Pattern::ScanPrefix1(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback, bool multiple)
	{
		uint8_t initial = pattern_[0].pattern;

		for (auto p = start; p < end; p++) {
			if (*p == initial) {
				if (MatchPattern(p)) {
					callback(p);
					if (!multiple) return;
				}
			}
		}
	}

	void Pattern::ScanPrefix2(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback, bool multiple)
	{
		uint16_t initial = pattern_[0].pattern
			| (pattern_[1].pattern << 8);

		for (auto p = start; p < end; p++) {
			if (*reinterpret_cast<uint16_t const *>(p) == initial) {
				if (MatchPattern(p)) {
					callback(p);
					if (!multiple) return;
				}
			}
		}
	}

	void Pattern::ScanPrefix4(uint8_t const * start, uint8_t const * end, std::function<void(uint8_t const *)> callback, bool multiple)
	{
		uint32_t initial = pattern_[0].pattern
			| (pattern_[1].pattern << 8)
			| (pattern_[2].pattern << 16)
			| (pattern_[3].pattern << 24);

		for (auto p = start; p < end; p++) {
			if (*reinterpret_cast<uint32_t const *>(p) == initial) {
				if (MatchPattern(p)) {
					callback(p);
					if (!multiple) return;
				}
			}
		}
	}

	void Pattern::Scan(uint8_t const * start, size_t length, std::function<void(uint8_t const *)> callback, bool multiple)
	{
		// Check prefix length
		auto prefixLength = 0;
		for (auto i = 0; i < pattern_.size(); i++) {
			if (pattern_[i].mask == 0xff) {
				prefixLength++;
			}
			else {
				break;
			}
		}

		auto end = start + length - pattern_.size();
		if (prefixLength >= 4) {
			ScanPrefix4(start, end, callback, multiple);
		} else if (prefixLength >= 2) {
			ScanPrefix2(start, end, callback, multiple);
		} else {
			ScanPrefix1(start, end, callback, multiple);
		}
	}

	bool LibraryManager::IsFixedStringRef(uint8_t const * ref, char const * str) const
	{
		if (ref >= moduleStart_ && ref < moduleStart_ + moduleSize_) {
			auto fsx = (FixedString const *)ref;
			if (*fsx && strcmp(fsx->Str, str) == 0) {
				return true;
			}
		}

		return false;
	}

	uint8_t const * AsmCallToAbsoluteAddress(uint8_t const * call)
	{
		if (call[0] != 0xE8) {
			Debug("AsmCallToAbsoluteAddress(): Not a call @ %p", call);
			return nullptr;
		}

		int32_t rel = *(int32_t const *)(call + 1);
		return call + rel + 5;
	}

	uint8_t const * AsmLeaToAbsoluteAddress(uint8_t const * lea)
	{
		if ((lea[0] != 0x48 && lea[0] != 0x4C) || (lea[1] != 0x8D && lea[1] != 0x8B)) {
			Debug("AsmLeaToAbsoluteAddress(): Not a LEA @ %p", lea);
			return nullptr;
		}

		int32_t rel = *(int32_t const *)(lea + 3);
		return lea + rel + 7;
	}

	bool LibraryManager::FindLibraries()
	{
#if defined(OSI_EOCAPP)
		if (FindEoCApp(moduleStart_, moduleSize_)) {
			FindMemoryManagerEoCApp();
			FindLibrariesEoCApp();
			FindServerGlobalsEoCApp();
			FindEoCGlobalsEoCApp();
			FindGlobalStringTableEoCApp();
			FindNetworkFixedStringsEoCApp();
			FindErrorFuncsEoCApp();
			FindFileSystemEoCApp();
			FindGameStateFuncsEoCApp();
			return !CriticalInitFailed;
		}
#else
		if (FindEoCPlugin(moduleStart_, moduleSize_)) {
			FindMemoryManagerEoCPlugin();
			FindLibrariesEoCPlugin();
			FindServerGlobalsEoCPlugin();
			FindEoCGlobalsEoCPlugin();
			FindGlobalStringTableCoreLib();
			FindErrorFuncsEoCPlugin();
			FindFileSystemCoreLib();
			FindGameStateFuncsEoCPlugin();
			return !CriticalInitFailed;
		}
#endif
		else {
			Debug("LibraryManager::FindLibraries(): Unable to determine application type.");
			return false;
		}
	}

	bool LibraryManager::PostStartupFindLibraries()
	{
		if (PostLoaded) {
			return !CriticalInitFailed;
		}

		auto initStart = std::chrono::high_resolution_clock::now();

#if defined(OSI_EOCAPP)
		FindGameActionManagerEoCApp();
		FindGameActionsEoCApp();
		FindStatusMachineEoCApp();
		FindHitFuncsEoCApp();
		FindItemFuncsEoCApp();
		FindStatusTypesEoCApp();
		FindCustomStatsEoCApp();
		FindCharacterStatFuncsEoCApp();
#else
		FindGameActionManagerEoCPlugin();
		FindGameActionsEoCPlugin();
		FindStatusMachineEoCPlugin();
		FindHitFuncsEoCPlugin();
		FindItemFuncsEoCPlugin();
		FindStatusTypesEoCPlugin();
		FindCustomStatsEoCPlugin();
		FindCharacterStatFuncsEoCPlugin();
#endif

		if (!CriticalInitFailed) {
			EnableCustomStats();
			InitPropertyMaps();

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			if (StatusHitVMT != nullptr) {
				StatusHitEnter.Wrap(StatusHitVMT->Enter);
			}

			if (StatusHealVMT != nullptr) {
				StatusHealEnter.Wrap(StatusHealVMT->Enter);
			}

			if (CharacterHit != nullptr) {
				CharacterHitHook.Wrap(CharacterHit);
			}

			if (StatusMachineApplyStatus != nullptr) {
				ApplyStatusHook.Wrap(StatusMachineApplyStatus);
			}

			gCharacterStatsGetters.WrapAll();

			DetourTransactionCommit();
		}

		auto initEnd = std::chrono::high_resolution_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(initEnd - initStart).count();
		Debug("LibraryManager::PostStartupFindLibraries() took %d ms", ms);

		PostLoaded = true;
		return !CriticalInitFailed;
	}

	void LibraryManager::Cleanup()
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		StatusHitEnter.Unwrap();
		StatusHealEnter.Unwrap();
		CharacterHitHook.Unwrap();
		ApplyStatusHook.Unwrap();

		DetourTransactionCommit();
	}

	void LibraryManager::ShowStartupError(std::wstring const & msg, bool wait, bool exitGame)
	{
		if (EoCClient == nullptr
			|| EoCClientHandleError == nullptr
			|| EoCAlloc == nullptr) {
			return;
		}

		if (wait) {
			std::thread messageThread([this, msg, exitGame]() {
				unsigned retries{ 0 };
				while (!CanShowError() && retries < 600) {
					Sleep(100);
					retries++;
				}

				STDWString str;
				str.Set(msg);
				EoCClientHandleError(*EoCClient, &str, exitGame, &str);
			});
			messageThread.detach();
		} else {
			STDWString str;
			str.Set(msg);
			EoCClientHandleError(*EoCClient, &str, exitGame, &str);
		}
	}

	bool LibraryManager::CanShowError()
	{
		if (EoCClient == nullptr
			|| *EoCClient == nullptr
			|| (*EoCClient)->GameStateMachine == nullptr
			|| *(*EoCClient)->GameStateMachine == nullptr
			|| EoCClientHandleError == nullptr) {
			return false;
		}

		auto state = (*(*EoCClient)->GameStateMachine)->State;
		return state == GameState::Running
			|| state == GameState::Paused
			|| state == GameState::GameMasterPause
			|| state == GameState::Menu
			|| state == GameState::Lobby;
	}

	class WriteAnchor
	{
	public:
		WriteAnchor(uint8_t const * ptr, std::size_t size)
			: ptr_(const_cast<uint8_t *>(ptr)),
			size_(size)
		{
			BOOL succeeded = VirtualProtect((LPVOID)ptr_, size_, PAGE_READWRITE, &oldProtect_);
			if (!succeeded) Fail("VirtualProtect() failed");
		}

		~WriteAnchor()
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

	void LibraryManager::EnableCustomStats()
	{
		if (UICharacterSheetHook == nullptr
			|| ActivateClientSystemsHook == nullptr
			|| ActivateServerSystemsHook == nullptr
			|| CustomStatUIRollHook == nullptr) {
			Debug("LibraryManager::EnableCustomStats(): Hooks not available");
			return;
		}

		{
			uint8_t const replacement[] = {
#if defined(OSI_EOCAPP)
				0xc6, 0x45, 0xf8, 0x01
#else
				0xB2, 0x01, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
#endif
			};

			WriteAnchor code(UICharacterSheetHook, sizeof(replacement));
			memcpy(code.ptr(), replacement, sizeof(replacement));
		}

		{
			uint8_t const replacement[] = { 0x90, 0x90 };
			WriteAnchor code(ActivateClientSystemsHook, sizeof(replacement));
			memcpy(code.ptr(), replacement, sizeof(replacement));
		}

		{
			uint8_t const replacement[] = { 0x90, 0x90 };
			WriteAnchor code(ActivateServerSystemsHook, sizeof(replacement));
			memcpy(code.ptr(), replacement, sizeof(replacement));
		}

		{
			uint8_t const replacement[] = { 0xC3 };
			WriteAnchor code(CustomStatUIRollHook, sizeof(replacement));
			memcpy(code.ptr(), replacement, sizeof(replacement));
		}
	}
}
