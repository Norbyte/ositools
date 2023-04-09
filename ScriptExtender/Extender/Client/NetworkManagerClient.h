#pragma once

#include <NetProtocol.h>

BEGIN_NS(ecl)

class ExtenderProtocol : public ExtenderProtocolBase
{
protected:
	void ProcessExtenderMessage(net::MessageContext& context, MessageWrapper& msg) override;

private:
	void SyncNetworkStrings(MsgS2CSyncNetworkFixedStrings const& msg);
};

class NetworkManager
{
public:
	void Reset();

	bool CanSendExtenderMessages() const;
	void AllowExtenderMessages();

	void ExtendNetworking();

	ScriptExtenderMessage* GetFreeMessage();

	void Send(ScriptExtenderMessage* msg);

private:
	ExtenderProtocol* protocol_{ nullptr };

	// Indicates that the client can support extender messages to the server
	// (i.e. the server supports the message ID and won't crash)
	bool extenderSupport_{ false };
	bool wasHooked_{ false };

	net::Client* GetClient() const;

	void OnConnectMessage(net::Message* msg, net::BitstreamSerializer* serializer);
	void OnAcceptMessage(net::Message* msg, net::BitstreamSerializer* serializer);
	void HookMessages(net::MessageFactory* messageFactory);
};


class NetworkFixedStringReceiver
{
public:
	inline NetworkFixedStringReceiver(NetworkManager& network)
		: network_(network)
	{}

	void RequestFromServer();
	void UpdateFromServer();
	void ClientReset();
	void ClientLoaded();

	inline void SetServerNetworkFixedStrings(std::vector<STDString>& strs)
	{
		updatedStrings_ = strs;
	}

private:
	NetworkManager& network_;
	std::vector<STDString> updatedStrings_;
	bool notInSync_{ false };
	bool syncWarningShown_{ false };
	STDString conflictingString_;
};

END_NS()
