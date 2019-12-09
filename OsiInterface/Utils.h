#pragma once

#include <iostream>
#include <string>

std::string ToUTF8(std::wstring const & s);
std::wstring FromUTF8(std::string const & s);

enum class DebugMessageType
{
	Info,
	Osiris,
	Warning,
	Error
};

template <typename... Args>
void Debug(DebugMessageType type, wchar_t const * fmt, Args... args)
{
	wchar_t buf[1024];
	int length = swprintf_s(buf, 1024 - 1, fmt, args...);
	buf[length++] = 0;
	DebugRaw(type, buf);
}

template <typename... Args>
void Debug(DebugMessageType type, char const * fmt, Args... args)
{
	char buf[1024];
	int length = sprintf_s(buf, 1024 - 1, fmt, args...);
	buf[length++] = 0;
	DebugRaw(type, buf);
}

void DebugRaw(DebugMessageType type, char const * msg);
void DebugRaw(DebugMessageType type, wchar_t const * msg);

#define DEBUG(msg, ...) Debug(DebugMessageType::Info, msg, __VA_ARGS__)
#define WARN(msg, ...) Debug(DebugMessageType::Warning, msg, __VA_ARGS__)
#define ERR(msg, ...) Debug(DebugMessageType::Error, msg, __VA_ARGS__)

[[noreturn]]
void Fail(TCHAR const * reason);

[[noreturn]]
void Fail(char const * reason);

void CreateConsole(HMODULE hModule);
