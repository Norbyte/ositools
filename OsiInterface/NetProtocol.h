#pragma once

#include <GameDefinitions/Net.h>
#include "ScriptExtensions.pb.h"

namespace osidbg
{
	class ScriptExtenderMessage : public net::Message
	{
	public:
		static constexpr uint32_t MessageId = 328;
		static constexpr uint32_t MaxPayloadLength = 0x1ffff;

		ScriptExtenderMessage();
		~ScriptExtenderMessage() override;

		void Serialize(net::BitstreamSerializer & serializer) override;
		void Unknown() override;
		net::Message * CreateNew() override;
		void Reset() override;

		inline MessageWrapper & GetMessage()
		{
			return message_;
		}

		inline bool IsValid() const
		{
			return valid_;
		}

	private:
		MessageWrapper message_;
		bool valid_{ false };
	};

	class ExtenderProtocol : public net::Protocol
	{
	public:
		~ExtenderProtocol() override;

		bool ProcessMsg(void * Unused, net::MessageContext * Unknown, net::Message * Msg) override;
		void Unknown1() override;
		int PreUpdate(void * Unknown) override;
		int PostUpdate(void * Unknown) override;
		void * OnAddedToHost() override;
		void * OnRemovedFromHost() override;
		void * Unknown2() override;

	protected:
		virtual void ProcessExtenderMessage(MessageWrapper & msg) = 0;
	};

	class ExtenderProtocolClient : public ExtenderProtocol
	{
	protected:
		void ProcessExtenderMessage(MessageWrapper & msg) override;
	};

	class ExtenderProtocolServer : public ExtenderProtocol
	{
	protected:
		void ProcessExtenderMessage(MessageWrapper & msg) override;
	};


	class NetworkManager
	{
	public:
		void ExtendNetworkingClient();
		void ExtendNetworkingServer();

		ScriptExtenderMessage * GetFreeClientMessage();
		ScriptExtenderMessage * GetFreeServerMessage();

	private:
		ExtenderProtocolClient * clientProtocol_{ nullptr };
		ExtenderProtocolServer * serverProtocol_{ nullptr };

		net::Host * GetServer() const;
		net::Client * GetClient() const;
	};
}
