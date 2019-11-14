#pragma once

#include <memory>

#pragma pack(push, 1)
struct EoCClientState
{
	uint8_t Unkn[16];
	uint32_t State;
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

class ErrorUtils
{
public:
	ErrorUtils();

	void ShowError(wchar_t const * msg);

private:
	typedef void(*EoCClient__HandleError)(void * self, STDWString *a2, bool a3, STDWString *a4);

	HMODULE eocApp_{ NULL };
	uint8_t const * moduleStart_{ nullptr };
	std::size_t moduleSize_{ 0 };

	eclEoCClient ** EoCClient{ nullptr };
	EoCClient__HandleError EoCClientHandleError{ nullptr };

	bool FindModule();
	void FindErrorFuncs();
	bool CanShowError();

	bool ShowErrorDialog(wchar_t const * msg);
};

extern std::unique_ptr<ErrorUtils> gErrorUtils;
