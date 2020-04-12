#pragma once

#include <iostream>
#include <string>

namespace std
{
	class thread;
}

enum class DebugMessageType
{
	Debug,
	Info,
	Osiris,
	Warning,
	Error
};

class DebugConsole
{
public:
	void Create();

	void Debug(DebugMessageType type, char const* msg);
	void Debug(DebugMessageType type, wchar_t const* msg);

private:
	bool created_{ false };
	bool inputEnabled_{ false };
	bool consoleRunning_{ false };
	std::thread* consoleThread_{ nullptr };

	void ConsoleThread();
	void SetColor(DebugMessageType type);
};

extern DebugConsole gConsole;

extern HMODULE gThisModule;

std::string ToUTF8(std::wstring_view s);
std::wstring FromUTF8(std::string_view s);

template <typename... Args>
void Debug(DebugMessageType type, wchar_t const * fmt, Args... args)
{
	wchar_t buf[1024];
	int length = swprintf_s(buf, 1024 - 1, fmt, args...);
	buf[length++] = 0;
	gConsole.Debug(type, buf);
}

template <typename... Args>
void Debug(DebugMessageType type, char const * fmt, Args... args)
{
	char buf[1024];
	int length = sprintf_s(buf, 1024 - 1, fmt, args...);
	buf[length++] = 0;
	gConsole.Debug(type, buf);
}

#define DEBUG(msg, ...) ::Debug(DebugMessageType::Debug, msg, __VA_ARGS__)
#define INFO(msg, ...) ::Debug(DebugMessageType::Info, msg, __VA_ARGS__)
#define WARN(msg, ...) ::Debug(DebugMessageType::Warning, msg, __VA_ARGS__)
#define ERR(msg, ...) ::Debug(DebugMessageType::Error, msg, __VA_ARGS__)

[[noreturn]]
void Fail(TCHAR const * reason);

[[noreturn]]
void Fail(char const * reason);


#if !defined(OSI_NO_DEBUG_LOG)
#define OsiError(msg) { \
	std::stringstream ss; \
	ss << __FUNCTION__ "(): " msg; \
	LogOsirisError(ss.str()); \
}

#define OsiWarn(msg) { \
	std::stringstream ss; \
	ss << __FUNCTION__ "(): " msg; \
	LogOsirisWarning(ss.str()); \
}

#define OsiMsg(msg) { \
	std::stringstream ss; \
	ss << msg; \
	LogOsirisMsg(ss.str()); \
}

#define OsiErrorS(msg) LogOsirisError(__FUNCTION__ "(): " msg)
#define OsiWarnS(msg) LogOsirisWarning(__FUNCTION__ "(): " msg)
#define OsiMsgS(msg) LogOsirisMsg(__FUNCTION__ "(): " msg)
#else
#define OsiError(msg) (void)0
#define OsiWarn(msg) (void)0
#define OsiMsg(msg) (void)0
#define OsiErrorS(msg) (void)0
#define OsiWarnS(msg) (void)0
#define OsiMsgS(msg) (void)0
#endif


void LogOsirisError(std::string_view msg);
void LogOsirisWarning(std::string_view msg);
void LogOsirisMsg(std::string_view msg);
