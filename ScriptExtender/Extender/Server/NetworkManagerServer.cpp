#include <stdafx.h>
#include <Extender/Server/NetworkManagerServer.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(esv)


void ExtenderProtocol::ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg)
{
	switch (msg.msg_case()) {
	case MessageWrapper::kPostLua:
	{
		auto & postMsg = msg.post_lua();
		esv::LuaServerPin pin(esv::ExtensionState::Get());
		if (pin) {
			pin->OnNetMessageReceived(STDString(postMsg.channel_name()), STDString(postMsg.payload()), context.UserID);
		}
		break;
	}

	case MessageWrapper::kC2SRequestStrings:
	{
		if (gExtender->GetConfig().SyncNetworkStrings) {
			gExtender->GetServer().GetNetworkFixedStrings().OnUpdateRequested(context.UserID);
		}
		break;
	}

	case MessageWrapper::kC2SExtenderHello:
	{
		auto const& hello = msg.c2s_extender_hello();
		// The version is missing in old extender messages, so it'll default to 0.
		DEBUG("Got extender support notification from user %d (version %d)", context.UserID.Id, hello.version());
		gExtender->GetServer().GetNetworkManager().AllowExtenderMessages(context.UserID.GetPeerId(), hello.version());
		break;
	}

	default:
		OsiErrorS("Unknown extension message type received!");
	}
}

void ExtenderProtocol::ShowPerfWarning(GameTime* Time)
{
	auto currentTick = std::chrono::steady_clock::now();
	auto realTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTick - lastTick_).count();

	// Above 200 ms frame times the server clock becomes slower than the real clock
	// (the time between two server frames cannot be greater than ~200ms), which means that
	// animation timings, etc. will be out of sync with the client
	if (Time->DeltaTime >= 0.2f) {
		ERR("Possible desync! Server tick took %.2f ms (frame time exceeded by %.2f ms), wall time %ld ms cur, %ld ms last!",
			Time->DeltaTime * 1000.0f, (Time->DeltaTime - 0.0333f) * 1000.0f, realTimeMs, lastRealTimeMs_);
	}
	else if (Time->DeltaTime > 0.13f) {
		ERR("Server tick took %.2f ms (frame time exceeded by %.2f ms)!",
			Time->DeltaTime * 1000.0f, (Time->DeltaTime - 0.0333f) * 1000.0f);
	}
	else if (Time->DeltaTime > 0.08f) {
		WARN("Server tick took %.2f ms (frame time exceeded by %.2f ms)!",
			Time->DeltaTime * 1000.0f, (Time->DeltaTime - 0.0333f) * 1000.0f);
	}

	lastTick_ = currentTick;
	lastRealTimeMs_ = realTimeMs;
}

int ExtenderProtocol::PostUpdate(GameTime* Time)
{
#if defined(OSI_EOCAPP)
	if (gExtender->GetConfig().ShowPerfWarnings) {
		auto state = GetStaticSymbols().GetServerState();
		if (state && *state == esv::GameState::Running) {
			ShowPerfWarning(Time);
		}
	}
#endif

	return 0;
}

void NetworkManager::Reset()
{
	extenderPeerVersions_.clear();
}

bool NetworkManager::CanSendExtenderMessages(PeerId peerId) const
{
	return extenderPeerVersions_.find(peerId) != extenderPeerVersions_.end();
}

std::optional<uint32_t> NetworkManager::GetPeerVersion(PeerId peerId) const
{
	auto it = extenderPeerVersions_.find(peerId);
	if (it != extenderPeerVersions_.end()) {
		return it->second;
	} else {
		return {};
	}
}

void NetworkManager::AllowExtenderMessages(PeerId peerId, uint32_t version)
{
	extenderPeerVersions_.insert(std::make_pair(peerId, version));
}


void NetworkManager::HookMessages(net::MessageFactory * messageFactory)
{
	// FIXME - SHOULD SEPARATE FROM OSI!
	gExtender->GetServer().Osiris().GetWrappers().InitializeNetworking(messageFactory);
}

void NetworkManager::ExtendNetworking()
{
	auto server = GetServer();
	if (server != nullptr
		&& server->NetMessageFactory->MessagePools.size() >= (unsigned)ScriptExtenderMessage::MessageId) {
		return;
	}

	if (GetStaticSymbols().net__Host__AddProtocol != nullptr
		&& GetStaticSymbols().net__MessageFactory__RegisterMessage != nullptr
		&& server != nullptr) {
		protocol_ = new ExtenderProtocol();
		GetStaticSymbols().net__Host__AddProtocol(server, 100, protocol_);

		auto extenderMsg = new ScriptExtenderMessage();
		server->NetMessageFactory->ReservePools((unsigned)ScriptExtenderMessage::MessageId + 1);
		GetStaticSymbols().net__MessageFactory__RegisterMessage(server->NetMessageFactory,
			ScriptExtenderMessage::MessageId, extenderMsg, 4, "ScriptExtenderMessage");
		HookMessages(server->NetMessageFactory);
		DEBUG("Registered custom esv network protocol");
	} else {
		ERR("Could not register esv protocol - symbols not mapped");
	}
}

net::GameServer * NetworkManager::GetServer() const
{
	auto server = GetStaticSymbols().esv__EoCServer;
	if (server != nullptr && *server != nullptr) {
		return (*server)->GameServer;
	} else {
		return nullptr;
	}
}

char const* ExtenderMsgSignature = "EX1";
uint8_t ExternderNullSignature[3] = { 0, 0, 0 };

ScriptExtenderMessage * NetworkManager::GetFreeMessage(UserId userId)
{
	if (userId && !CanSendExtenderMessages(userId.GetPeerId())) {
		ERR("Attempted to send extender message to user %d that does not understand extender protocol!", userId.Id);
		return nullptr;
	}

	auto server = GetServer();
	if (server != nullptr) {
		return server->GetFreeMessage<ScriptExtenderMessage>();
	} else {
		return nullptr;
	}
}

void NetworkManager::Send(ScriptExtenderMessage * msg, UserId userId)
{
	auto server = GetServer();
	if (server != nullptr) {
		server->VMT->SendToPeer(server, &userId.Id, msg);
	}
}

void NetworkManager::Broadcast(ScriptExtenderMessage * msg, UserId excludeUserId, bool excludeLocalPeer)
{
	auto server = GetServer();
	if (server != nullptr) {
		ObjectSet<PeerId> peerIds;
		for (auto peerId : server->ActivePeerIds) {
			if (CanSendExtenderMessages(peerId)) {
				if (peerId != 1 || !excludeLocalPeer) {
					peerIds.push_back(peerId);
				}
			} else {
				WARN("Not sending extender message to peer %d as it does not understand extender protocol!", peerId);
			}
		}

		server->VMT->SendToMultiplePeers(server, &peerIds, msg, excludeUserId.Id);
	}
}

void NetworkManager::BroadcastToConnectedPeers(ScriptExtenderMessage* msg, UserId excludeUserId, bool excludeLocalPeer)
{
	auto server = GetServer();
	if (server != nullptr) {
		ObjectSet<PeerId> peerIds;
		peerIds.reallocate(server->ConnectedPeerIds.size());
		for (auto peerId : server->ConnectedPeerIds) {
			if (CanSendExtenderMessages(peerId)) {
				if (peerId != 1 || !excludeLocalPeer) {
					peerIds.push_back(peerId);
				}
			} else {
				WARN("Not sending extender message to peer %d as it does not understand extender protocol!", peerId);
			}
		}
		server->VMT->SendToMultiplePeers(server, &peerIds, msg, excludeUserId.Id);
	}
}


void NetworkFixedStringSender::Dump()
{
	auto nfs = GetStaticSymbols().NetworkFixedStrings;
	if (nfs != nullptr && (*nfs)->Initialized) {
		auto const& strings = (*nfs)->FixedStrSet;

#pragma message("Move to logger object")
		auto nfsLogPath = gExtender->MakeLogFilePath(L"NetworkFixedStrings", L"log");
		std::ofstream logOut(nfsLogPath.c_str(), std::ios::out);
		for (auto const& string : strings) {
			logOut << (string ? string.GetString() : "(NULL)") << std::endl;
		}
		logOut.close();
		DEBUG(L"OsirisProxy::DumpNetworkFixedStrings() - Saved to %s", nfsLogPath.c_str());
	}
	else {
		ERR("OsirisProxy::DumpNetworkFixedStrings() - Fixed strings not initialized yet");
	}
}

void NetworkFixedStringSender::FlushQueuedRequests()
{
	DEBUG("Flushing NetworkFixedString updates");
	for (auto userId : pendingSyncRequests_) {
		SendUpdateToUser(userId);
	}

	pendingSyncRequests_.clear();
}

void NetworkFixedStringSender::OnUpdateRequested(UserId userId)
{
	auto gameState = *GetStaticSymbols().GetServerState();
	if (gameState == esv::GameState::LoadSession
		|| gameState == esv::GameState::LoadLevel
		|| gameState == esv::GameState::BuildStory
		|| gameState == esv::GameState::Sync
		|| gameState == esv::GameState::Running) {
		DEBUG("Fulfill requested NetworkFixedString update for user %d", userId.Id);
		SendUpdateToUser(userId);
	}
	else {
		DEBUG("Queuing requested NetworkFixedString update for user %d", userId.Id);
		pendingSyncRequests_.insert(userId);
	}
}

void NetworkFixedStringSender::SendUpdateToUser(UserId userId)
{
	auto fixedStrs = GetStaticSymbols().NetworkFixedStrings;
	if (fixedStrs == nullptr || *fixedStrs == nullptr) {
		return;
	}

	DEBUG("Sending NetworkFixedString table to user %d", userId.Id);
	auto& nfs = **fixedStrs;
	auto msg = network_.GetFreeMessage(userId);
	if (msg != nullptr) {
		auto syncMsg = msg->GetMessage().mutable_s2c_sync_strings();
		auto numStrings = nfs.FixedStrSet.size();
		for (uint32_t i = 1; i < numStrings; i++) {
			syncMsg->add_network_string(nfs.FixedStrSet[i].GetString());
		}

		network_.Send(msg, userId);
	} else {
		OsiErrorS("Could not get free message!");
	}
}

END_NS()
