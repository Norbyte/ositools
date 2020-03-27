#pragma once

#if !defined(OSI_NO_DEBUGGER)

#include <cstdint>
#include <WinSock2.h>
#include "osidebug.pb.h"

namespace dse
{
	class DebugInterface
	{
	public:
		DebugInterface(uint16_t port);
		~DebugInterface();

		void SetMessageHandler(
			std::function<bool(DebuggerToBackend const *)> messageHandler,
			std::function<void()> connectHandler,
			std::function<void()> disconnectHandler
		);
		bool IsConnected() const;
		void Send(BackendToDebugger const & msg);
		void Run();
		void Disconnect();

	private:
		void Send(uint8_t * buf, uint32_t length);
		bool ProcessMessage(uint8_t * buf, uint32_t length);
		void MessageLoop(SOCKET sock);

		uint16_t port_;
		SOCKET socket_;
		SOCKET clientSocket_{ 0 };
		uint8_t receiveBuf_[0x10000];
		uint32_t receivePos_;
		std::function<bool (DebuggerToBackend const *)> messageHandler_;
		std::function<void ()> connectHandler_;
		std::function<void ()> disconnectHandler_;
	};
}

#endif
