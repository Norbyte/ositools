#pragma once

#include <NetProtocol.h>

BEGIN_NS(esv)

class ExtenderProtocol : public ExtenderProtocolBase
{
protected:
	void ProcessExtenderMessage(net::MessageContext& context, MessageWrapper& msg) override;
	int PostUpdate(GameTime* Time) override;

private:
	void ShowPerfWarning(GameTime* Time);

	std::chrono::steady_clock::time_point lastTick_;
	long long lastRealTimeMs_{ 0 };
};

class NetworkManager
{
public:
	void Reset();

	bool CanSendExtenderMessages(PeerId peerId) const;
	std::optional<uint32_t> GetPeerVersion(PeerId peerId) const;
	void AllowExtenderMessages(PeerId peerId, uint32_t version);

	void ExtendNetworking();

	ScriptExtenderMessage * GetFreeMessage(UserId userId);

	void Send(ScriptExtenderMessage * msg, UserId userId);
	void Broadcast(ScriptExtenderMessage * msg, UserId excludeUserId, bool excludeLocalPeer = false);
	void BroadcastToConnectedPeers(ScriptExtenderMessage* msg, UserId excludeUserId, bool excludeLocalPeer = false);

private:
	ExtenderProtocol * protocol_{ nullptr };

	// List of clients that support the extender protocol
	std::unordered_map<PeerId, uint32_t> extenderPeerVersions_;

	net::GameServer * GetServer() const;

	void HookMessages(net::MessageFactory* messageFactory);
};


class NetworkFixedStringSender
{
public:
	inline NetworkFixedStringSender(NetworkManager& network)
		: network_(network)
	{}

	void OnUpdateRequested(UserId userId);
	void FlushQueuedRequests();
	void Dump();

private:
	NetworkManager& network_;
	std::unordered_set<UserId> pendingSyncRequests_;

	void SendUpdateToUser(UserId userId);
};

END_NS()