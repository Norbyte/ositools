#pragma once

#include <cstdint>
#include <WinSock2.h>
#include "osidebug.pb.h"

namespace osidbg
{
	class DebugInterface
	{
	public:
		DebugInterface();
		~DebugInterface();

		void SetMessageHandler(
			std::function<bool(DebuggerToBackend const *)> messageHandler,
			std::function<void()> connectHandler,
			std::function<void()> disconnectHandler
		);
		bool IsConnected() const;
		void Send(BackendToDebugger const & msg);
		void Run();

	private:
		void Disconnect();
		void Send(uint8_t * buf, uint32_t length);
		bool ProcessMessage(uint8_t * buf, uint32_t length);
		void MessageLoop(SOCKET sock);

		SOCKET socket_;
		SOCKET clientSocket_{ 0 };
		uint8_t receiveBuf_[0x10000];
		uint32_t receivePos_;
		std::function<bool (DebuggerToBackend const *)> messageHandler_;
		std::function<void ()> connectHandler_;
		std::function<void ()> disconnectHandler_;
	};
}
