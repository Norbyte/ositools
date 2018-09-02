#pragma once

#include "OsirisProxy.h"
#include <WinSock2.h>

osidbg::WrappedFunction<int(SOCKET, LPWSAMSG, DWORD, LPDWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE)> WrappedWSASendMsg;
osidbg::WrappedFunction<int(SOCKET, LPWSABUF, DWORD, LPDWORD, DWORD, LPWSAOVERLAPPED, LPWSAOVERLAPPED_COMPLETION_ROUTINE)> WrappedWSASend;
osidbg::WrappedFunction<int(SOCKET, const char *, int, int)> WrappedSend;
osidbg::WrappedFunction<int(SOCKET, const char *, int, int, const struct sockaddr *, int)> WrappedSendto;
osidbg::WrappedFunction<SOCKET(int, int, int)> WrappedSocket;

SOCKET WSAAPI SocketWrapper(
	_In_ int af,
	_In_ int type,
	_In_ int protocol
)
{
	Debug(L"socket");
	return WrappedSocket(af, type, protocol);
}

int SendWrapper(
	_In_       SOCKET s,
	_In_ const char   *buf,
	_In_       int    len,
	_In_       int    flags
)
{
	Debug(L"send");
	return WrappedSend(s, buf, len, flags);
}

int SendtoWrapper(
	_In_       SOCKET                s,
	_In_ const char                  *buf,
	_In_       int                   len,
	_In_       int                   flags,
	_In_       const struct sockaddr *to,
	_In_       int                   tolen
)
{
	Debug(L"sendto");
	return WrappedSendto(s, buf, len, flags, to, tolen);
}

int WSASendWrapper(
	_In_  SOCKET                             s,
	_In_  LPWSABUF                           lpBuffers,
	_In_  DWORD                              dwBufferCount,
	_Out_ LPDWORD                            lpNumberOfBytesSent,
	_In_  DWORD                              dwFlags,
	_In_  LPWSAOVERLAPPED                    lpOverlapped,
	_In_  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	Debug(L"WSASend");
	return WrappedWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

int WSASendMsgWrapper(
	_In_  SOCKET                             s,
	_In_  LPWSAMSG                           lpMsg,
	_In_  DWORD                              dwFlags,
	_Out_ LPDWORD                            lpNumberOfBytesSent,
	_In_  LPWSAOVERLAPPED                    lpOverlapped,
	_In_  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	Debug(L"WSASendMsg");
	return WrappedWSASendMsg(s, lpMsg, dwFlags, lpNumberOfBytesSent, lpOverlapped, lpCompletionRoutine);
}

void RunSocketProxy()
{
/*	HMODULE hWS = GetModuleHandleW(L"ws2_32.dll");

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	auto pWSASendMsg = GetProcAddress(hWS, "WSASendMsg");
	WrappedWSASendMsg.Wrap(pWSASendMsg, &WSASendMsgWrapper);

	auto pWSASend = GetProcAddress(hWS, "WSASend");
	WrappedWSASend.Wrap(pWSASend, &WSASendWrapper);

	auto pSendto = GetProcAddress(hWS, "sendto");
	WrappedSendto.Wrap(pSendto, &SendtoWrapper);

	auto pSend = GetProcAddress(hWS, "send");
	WrappedSend.Wrap(pSend, &SendWrapper);

	auto pSocket = GetProcAddress(hWS, "socket");
	WrappedSocket.Wrap(pSocket, &SocketWrapper);
	DetourTransactionCommit();*/
}