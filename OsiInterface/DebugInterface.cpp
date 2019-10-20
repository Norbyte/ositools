#include "stdafx.h"
#include "DebugInterface.h"
#include "NodeHooks.h"
#include "OsirisProxy.h"
#include <sstream>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <WS2tcpip.h>

namespace osidbg
{
	DebugInterface::DebugInterface(uint16_t port)
		: port_(port)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		uint32_t ip;
		inet_pton(AF_INET, "127.0.0.1", &ip);
		socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = ip;
		addr.sin_port = htons(port_);
		if (bind(socket_, (sockaddr *)&addr, sizeof(addr)) != 0) {
			Debug("Could not bind debugger server socket: %d", WSAGetLastError());
			Fail("Debug server start failed");
		}

		if (listen(socket_, 30) != 0) {
			Debug("Could not listen on server socket: %d", WSAGetLastError());
			Fail("Debug server start failed");
		}

		Debug("Debug interface listening on 127.0.0.1:%d; DBG protocol version %d", port_, DebugMessageHandler::ProtocolVersion);
	}

	DebugInterface::~DebugInterface()
	{
		closesocket(socket_);
	}

	void DebugInterface::SetMessageHandler(
		std::function<bool(DebuggerToBackend const *)> messageHandler,
		std::function<void()> connectHandler,
		std::function<void()> disconnectHandler)
	{
		messageHandler_ = messageHandler;
		connectHandler_ = connectHandler;
		disconnectHandler_ = disconnectHandler;
	}

	bool DebugInterface::IsConnected() const
	{
		return clientSocket_ != 0;
	}

	void DebugInterface::Send(BackendToDebugger const & msg)
	{
		if (clientSocket_ == 0) {
			Debug("DebugInterface::Send(): Not connected to debugger frontend");
			return;
		}

		uint32_t size = (uint32_t)msg.ByteSizeLong();
		uint32_t packetSize = size + 4;
		Send((uint8_t *)&packetSize, 4);

		if (size < 0x100) {
			uint8_t buf[0x100];
			if (!msg.SerializeToArray(buf, size)) {
				Fail("Unable to serialize message");
			}

			Send(buf, size);
		}
		else
		{
			uint8_t * buf = new uint8_t[size];
			if (!msg.SerializeToArray(buf, size)) {
				Fail("Unable to serialize message");
			}

			Send(buf, size);
			delete[] buf;
		}
	}

	void DebugInterface::Send(uint8_t * buf, uint32_t length)
	{
		while (length > 0) {
			int sent = send(clientSocket_, (char *)buf, (int)length, 0);
			if (sent <= 0) {
				Debug("Socket send failed: %d, error %d", sent, WSAGetLastError());
				Disconnect();
				return;
			}

			buf += sent;
			length -= sent;
		}
	}

	bool DebugInterface::ProcessMessage(uint8_t * buf, uint32_t length)
	{
		google::protobuf::io::ArrayInputStream ais(buf, length);
		google::protobuf::io::CodedInputStream is(&ais);
		DebuggerToBackend msg;
		if (!msg.ParseFromCodedStream(&is)) {
			Debug("Unable to decode protobuf message from coded stream.");
			return false;
		}

		return messageHandler_(&msg);
	}

	void DebugInterface::Disconnect()
	{
		if (!IsConnected()) return;

		closesocket(clientSocket_);
		clientSocket_ = 0;

		if (disconnectHandler_) {
			disconnectHandler_();
		}
	}

	void DebugInterface::MessageLoop(SOCKET sock)
	{
		receivePos_ = 0;
		for (;;) {
			int len = recv(sock, (char *)&receiveBuf_[receivePos_], sizeof(receiveBuf_) - receivePos_, 0);
			if (len < 0) {
				Debug("Socket recv failed: %d, error %d", len, WSAGetLastError());
				return;
			}

			receivePos_ += len;
			while (receivePos_ >= 4) {
				uint32_t messageLength = *reinterpret_cast<uint32_t *>(&receiveBuf_[0]);

				if (messageLength < 4 || messageLength > sizeof(receiveBuf_)) {
					Debug("DebugInterface::MessageLoop(): Illegal message length: %d", messageLength);
					return;
				}

				if (receivePos_ >= messageLength) {
					if (!ProcessMessage(&receiveBuf_[4], messageLength - 4)) {
						Debug("DebugInterface::MessageLoop(): Message processing failed");
						return;
					}

					memmove(&receiveBuf_[0], &receiveBuf_[messageLength], receivePos_ - messageLength);
					receivePos_ -= messageLength;
				}
				else {
					break;
				}
			}
		}
	}

	void DebugInterface::Run()
	{
		for (;;) {
			sockaddr_in addr;
			int addrlen = sizeof(addr);
			clientSocket_ = accept(socket_, (sockaddr *)&addr, &addrlen);
			Debug("Accepted debug connection.");
			if (connectHandler_) {
				connectHandler_();
			}

			MessageLoop(clientSocket_);
			Disconnect();
		}
	}
}
