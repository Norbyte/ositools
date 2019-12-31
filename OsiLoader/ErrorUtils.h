#pragma once

#include <memory>
#include <optional>
#include <vector>

#pragma pack(push, 1)
enum class GameState : uint32_t
{
	Unknown = 0,
	Init,
	InitMenu,
	InitNetwork,
	InitConnection,
	Idle,
	LoadMenu,
	Menu,
	Exit,
	SwapLevel,
	LoadLevel = 10,
	LoadModule,
	LoadSession,
	LoadGMCampaign,
	UnloadLevel,
	UnloadModule,
	UnloadSession,
	Paused,
	PrepareRunning,
	Running,
	Disconnect = 20,
	Join,
	StartLoading,
	StopLoading,
	StartServer,
	Movie,
	Installation,
	GameMasterPause,
	ModReceiving,
	Lobby = 30,
	BuildStory
};

struct EoCClientState
{
	uint8_t Unkn[16];
	GameState State;
};

struct eclEoCClient
{
	uint8_t Unkn[64];
	EoCClientState ** State;
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
};
#pragma pack(pop)

struct THREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
};

class ErrorUtils
{
public:
	ErrorUtils();

	void ShowError(wchar_t const * msg) const;
	std::optional<GameState> GetState() const;

	void SuspendClientThread() const;
	void ResumeClientThread() const;

private:
	typedef void(*EoCClient__HandleError)(void * self, STDWString * message, bool exitGame, STDWString *a4);

	struct ThreadInfo
	{
		DWORD ThreadId;
		std::string Name;
	};

	HMODULE eocApp_{ NULL };
	uint8_t const * moduleStart_{ nullptr };
	std::size_t moduleSize_{ 0 };
	std::vector<ThreadInfo> threads_;

	eclEoCClient ** EoCClient{ nullptr };
	EoCClient__HandleError EoCClientHandleError{ nullptr };

	bool FindModule();
	void FindErrorFuncs();
	bool CanShowError() const;

	bool ShowErrorDialog(wchar_t const * msg) const;
	void ClientHandleError(wchar_t const * msg, bool exitGame) const;

	ThreadInfo const * FindClientThread() const;
	static LONG NTAPI ThreadNameCaptureFilter(_EXCEPTION_POINTERS *ExceptionInfo);
};

extern std::unique_ptr<ErrorUtils> gErrorUtils;
