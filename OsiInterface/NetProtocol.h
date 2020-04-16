#pragma once

#include <GameDefinitions/Net.h>
#include "ScriptExtensions.pb.h"

namespace dse
{
	class ScriptExtenderMessage : public net::Message
	{
	public:
		static constexpr uint32_t MessageId = 400;
		static constexpr uint32_t MaxPayloadLength = 0xfffff;

		ScriptExtenderMessage();
		~ScriptExtenderMessage() override;

		void Serialize(net::BitstreamSerializer & serializer) override;
		void Unknown() override;
		net::Message * CreateNew() override;
		void Reset() override;

		inline MessageWrapper & GetMessage()
		{
#if defined(_DEBUG)
			return *message_;
#else
			return message_;
#endif
		}

		inline bool IsValid() const
		{
			return valid_;
		}

	private:
#if defined(_DEBUG)
		MessageWrapper* message_{ nullptr };
#else
		MessageWrapper message_;
#endif
		bool valid_{ false };
	};

	class ExtenderProtocol : public net::Protocol
	{
	public:
		~ExtenderProtocol() override;

		net::MessageStatus ProcessMsg(void * Unused, net::MessageContext * Unknown, net::Message * Msg) override;
		void Unknown1() override;
		int PreUpdate(void * Unknown) override;
		int PostUpdate(void * Unknown) override;
		void * OnAddedToHost() override;
		void * OnRemovedFromHost() override;
		void * Unknown2() override;

	protected:
		virtual void ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg) = 0;
	};

	class ExtenderProtocolClient : public ExtenderProtocol
	{
	protected:
		void ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg) override;

	private:
		void SyncNetworkStrings(MsgS2CSyncNetworkFixedStrings const& msg);
	};

	class ExtenderProtocolServer : public ExtenderProtocol
	{
	protected:
		void ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg) override;
	};


	class NetworkManager
	{
	public:
		void ClientReset();
		void ServerReset();

		void ClientAllowExtenderMessages();
		void ServerAllowExtenderMessages(PeerId peerId);

		void ExtendNetworkingClient();
		void ExtendNetworkingServer();

		ScriptExtenderMessage * GetFreeClientMessage();
		ScriptExtenderMessage * GetFreeServerMessage(UserId userId);

		void ClientSend(ScriptExtenderMessage * msg);
		void ServerSend(ScriptExtenderMessage * msg, UserId userId);
		void ServerBroadcast(ScriptExtenderMessage * msg, UserId excludeUserId);
		void ServerBroadcastToConnectedPeers(ScriptExtenderMessage* msg, UserId excludeUserId);

	private:
		ExtenderProtocolClient * clientProtocol_{ nullptr };
		ExtenderProtocolServer * serverProtocol_{ nullptr };

		// Indicates that the client can support extender messages to the server
		// (i.e. the server supports the message ID and won't crash)
		bool clientExtenderSupport_{ false };
		// List of clients that support the extender protocol
		std::unordered_set<PeerId> serverExtenderPeerIds_;

		net::GameServer * GetServer() const;
		net::Client * GetClient() const;

		void OnClientConnectMessage(net::Message* msg, net::BitstreamSerializer* serializer);
		void OnClientAcceptMessage(net::Message* msg, net::BitstreamSerializer* serializer);
		void HookMessages(net::MessageFactory* messageFactory);
	};


	class NetworkFixedStringSynchronizer
	{
	public:
		void OnUpdateRequested(UserId userId);
		void RequestFromServer();
		void FlushQueuedRequests();
		void UpdateFromServer();
		void ClientReset();
		void ClientLoaded();
		void Dump();

		inline void SetServerNetworkFixedStrings(std::vector<STDString>& strs)
		{
			updatedStrings_ = strs;
		}

	private:
		std::vector<STDString> updatedStrings_;
		std::unordered_set<UserId> pendingSyncRequests_;
		bool notInSync_{ false };
		bool syncWarningShown_{ false };
		STDString conflictingString_;

		void SendUpdateToUser(UserId userId);
	};
}
