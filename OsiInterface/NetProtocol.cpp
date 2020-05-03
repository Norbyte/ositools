#include <stdafx.h>
#include <NetProtocol.h>
#include <GameDefinitions/Symbols.h>
#include <OsirisProxy.h>
#include <Version.h>
#include <fstream>

namespace dse
{
	namespace net
	{
		void MessageFactory::ReservePools(uint32_t minPools)
		{
			if (MessagePools.Capacity >= minPools) return;

			MessagePools.Reallocate(minPools);
		}
	}


	ExtenderProtocol::~ExtenderProtocol() {}

	net::MessageStatus ExtenderProtocol::ProcessMsg(void * Unused, net::MessageContext * Context, net::Message * Msg)
	{
		if (Msg->MsgId == ScriptExtenderMessage::MessageId) {
			auto msg = static_cast<ScriptExtenderMessage *>(Msg);
			if (msg->IsValid()) {
				ProcessExtenderMessage(*Context, msg->GetMessage());
			}
			return net::MessageStatus::Handled;
		}

		return net::MessageStatus::Unhandled;
	}

	void ExtenderProtocol::Unknown1() {}

	int ExtenderProtocol::PreUpdate(void * Unknown)
	{
		return 0;
	}

	int ExtenderProtocol::PostUpdate(void * Unknown)
	{
		return 0;
	}

	void * ExtenderProtocol::OnAddedToHost()
	{
		return nullptr;
	}

	void * ExtenderProtocol::OnRemovedFromHost()
	{
		return nullptr;
	}

	void * ExtenderProtocol::Unknown2()
	{
		return nullptr;
	}

	void ExtenderProtocolClient::SyncNetworkStrings(MsgS2CSyncNetworkFixedStrings const& msg)
	{
		auto fixedStrings = GetStaticSymbols().NetworkFixedStrings;
		if (fixedStrings == nullptr || *fixedStrings == nullptr) {
			ERR("Cannot sync fixed strings - NetworkFixedStrings is null!");
			return;
		}

		std::vector<STDString> strings;
		auto numStrings = msg.network_string_size();
		strings.reserve(numStrings);
		for (auto i = 0; i < numStrings; i++) {
			auto& str = msg.network_string(i);
			strings.push_back(STDString(str));
		}

		auto state = GetStaticSymbols().GetClientState();
		DEBUG("Got fixedstring list from server");
		gOsirisProxy->NetworkFixedStringSync().SetServerNetworkFixedStrings(strings);

		if (state == ecl::GameState::Running
			|| state == ecl::GameState::PrepareRunning
			|| state == ecl::GameState::Paused) {
			WARN("Attempted to sync NetworkFixedStrings while the game is already running?");
		}
		else if (state == ecl::GameState::SwapLevel
			|| state == ecl::GameState::LoadLevel
			|| state == ecl::GameState::LoadModule
			|| state == ecl::GameState::LoadSession
			|| state == ecl::GameState::LoadGMCampaign) {
			gOsirisProxy->NetworkFixedStringSync().UpdateFromServer();
		}
	}

	void ExtenderProtocolClient::ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg)
	{
		switch (msg.msg_case()) {
		case MessageWrapper::kPostLua:
		{
			auto & postMsg = msg.post_lua();
			ecl::LuaClientPin pin(ecl::ExtensionState::Get());
			if (pin) {
				pin->OnNetMessageReceived(STDString(postMsg.channel_name()), STDString(postMsg.payload()), UserId::Unassigned);
			}
			break;
		}

		case MessageWrapper::kS2CResetLua:
		{
			auto & resetMsg = msg.s2c_reset_lua();
			auto & ext = ecl::ExtensionState::Get();
			ext.LuaReset(resetMsg.bootstrap_scripts());
			ext.OnModuleResume();
			ext.OnGameSessionLoading();
			ext.OnGameSessionLoaded();
			break;
		}

		case MessageWrapper::kS2CSyncStrings:
		{
			SyncNetworkStrings(msg.s2c_sync_strings());
			break;
		}

		case MessageWrapper::kS2CSyncStat:
		{
			auto stats = GetStaticSymbols().GetStats();
			stats->SyncObjectFromServer(msg.s2c_sync_stat());
			break;
		}

		default:
			OsiErrorS("Unknown extension message type received!");
		}
	}

	void ExtenderProtocolServer::ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg)
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
			if (gOsirisProxy->GetConfig().SyncNetworkStrings) {
				gOsirisProxy->NetworkFixedStringSync().OnUpdateRequested(context.UserID);
			}
			break;
		}

		case MessageWrapper::kC2SExtenderHello:
		{
			DEBUG("Got extender support notification from user %d", context.UserID.Id);
			gOsirisProxy->GetNetworkManager().ServerAllowExtenderMessages(context.UserID.GetPeerId());
			break;
		}

		default:
			OsiErrorS("Unknown extension message type received!");
		}
	}


	ScriptExtenderMessage::ScriptExtenderMessage()
	{
		MsgId = MessageId;
		Reset();
	}

	ScriptExtenderMessage::~ScriptExtenderMessage() {}

	void ScriptExtenderMessage::Serialize(net::BitstreamSerializer & serializer)
	{
		auto& msg = GetMessage();
		if (serializer.IsWriting) {
			uint32_t size = (uint32_t)msg.ByteSizeLong();
			if (size <= MaxPayloadLength) {
				serializer.WriteBytes(&size, sizeof(size));
				void * buf = GameAllocRaw(size);
				msg.SerializeToArray(buf, size);
				serializer.WriteBytes(buf, size);
				GameFree(buf);
			} else {
				// Zero length indicates that a packet failed to serialize
				uint32_t dummy = 0;
				serializer.WriteBytes(&dummy, sizeof(dummy));
				OsiError("Tried to write packet of size " << size << ", max size is " << MaxPayloadLength);
			}
		} else {
			uint32_t size = 0;
			valid_ = false;
			serializer.ReadBytes(&size, sizeof(size));
			if (size > MaxPayloadLength) {
				OsiError("Tried to read packet of size " << size << ", max size is " << MaxPayloadLength);
			} else if (size > 0) {
				void * buf = GameAllocRaw(size);
				serializer.ReadBytes(buf, size);
				valid_ = msg.ParseFromArray(buf, size);
				GameFree(buf);
			}
		}
	}

	void ScriptExtenderMessage::Unknown() {}

	net::Message * ScriptExtenderMessage::CreateNew()
	{
		return new ScriptExtenderMessage();
	}

	void ScriptExtenderMessage::Reset()
	{
#if defined(_DEBUG)
		message_ = new MessageWrapper();
#else
		GetMessage().Clear();
#endif
		valid_ = false;
	}


	void NetworkManager::ClientReset()
	{
		clientExtenderSupport_ = false;
	}

	void NetworkManager::ServerReset()
	{
		serverExtenderPeerIds_.clear();
	}

	bool NetworkManager::ClientCanSendExtenderMessages() const
	{
		return clientExtenderSupport_;
	}

	void NetworkManager::ClientAllowExtenderMessages()
	{
		clientExtenderSupport_ = true;
	}

	bool NetworkManager::ServerCanSendExtenderMessages(PeerId peerId) const
	{
		return serverExtenderPeerIds_.find(peerId) != serverExtenderPeerIds_.end();
	}

	void NetworkManager::ServerAllowExtenderMessages(PeerId peerId)
	{
		serverExtenderPeerIds_.insert(peerId);
	}


	void NetworkManager::ExtendNetworkingClient()
	{
		auto client = GetClient();
		if (client != nullptr
			&& client->NetMessageFactory->MessagePools.Size >= (unsigned)ScriptExtenderMessage::MessageId) {
			return;
		}

		if (GetStaticSymbols().net__Host__AddProtocol != nullptr
			&& GetStaticSymbols().net__MessageFactory__RegisterMessage != nullptr
			&& client != nullptr) {
			clientProtocol_ = new ExtenderProtocolClient();
			// Nasty hack to avoid having to lookup net::Client::AddProtocol
			auto gameClientPtr = (net::Host *)((uintptr_t)client + 8);
			GetStaticSymbols().net__Host__AddProtocol(gameClientPtr, 100, clientProtocol_);

			auto extenderMsg = new ScriptExtenderMessage();
			client->NetMessageFactory->ReservePools((unsigned)ScriptExtenderMessage::MessageId + 1);
			GetStaticSymbols().net__MessageFactory__RegisterMessage(client->NetMessageFactory, 
				ScriptExtenderMessage::MessageId, extenderMsg, 4, "ScriptExtenderMessage");
			HookMessages(client->NetMessageFactory);
			DEBUG("Registered custom ecl network protocol");
		} else {
			ERR("Could not register ecl protocol - symbols not mapped");
		}
	}

	void NetworkManager::HookMessages(net::MessageFactory * messageFactory)
	{
		using namespace std::placeholders;

		gOsirisProxy->GetWrappers().InitializeNetworking(messageFactory);

		auto& connectMsg = gOsirisProxy->GetWrappers().eocnet__ClientConnectMessage__Serialize;
		auto& acceptMsg = gOsirisProxy->GetWrappers().eocnet__ClientAcceptMessage__Serialize;
		connectMsg.ClearHooks();
		connectMsg.AddPostHook(std::bind(&NetworkManager::OnClientConnectMessage, this, _1, _2));
		acceptMsg.ClearHooks();
		acceptMsg.AddPostHook(std::bind(&NetworkManager::OnClientAcceptMessage, this, _1, _2));
	}

	void NetworkManager::ExtendNetworkingServer()
	{
		auto server = GetServer();
		if (server != nullptr
			&& server->NetMessageFactory->MessagePools.Size >= (unsigned)ScriptExtenderMessage::MessageId) {
			return;
		}

		if (GetStaticSymbols().net__Host__AddProtocol != nullptr
			&& GetStaticSymbols().net__MessageFactory__RegisterMessage != nullptr
			&& server != nullptr) {
			serverProtocol_ = new ExtenderProtocolServer();
			GetStaticSymbols().net__Host__AddProtocol(server, 100, serverProtocol_);

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
		auto server = GetStaticSymbols().EoCServer;
		if (server != nullptr && *server != nullptr) {
			return (*server)->GameServer;
		} else {
			return nullptr;
		}
	}

	net::Client * NetworkManager::GetClient() const
	{
		auto client = GetStaticSymbols().EoCClient;
		if (client != nullptr && *client != nullptr) {
			return (*client)->GameClient;
		} else {
			return nullptr;
		}
	}

	char const* ExtenderMsgSignature = "EX1";
	uint8_t ExternderNullSignature[3] = { 0, 0, 0 };

	void NetworkManager::OnClientConnectMessage(net::Message* msg, net::BitstreamSerializer* serializer)
	{
	}

	void NetworkManager::OnClientAcceptMessage(net::Message* msg, net::BitstreamSerializer* serializer)
	{
		if (serializer->IsWriting) {
			DEBUG("Appending extender signature to ClientAccept");
			// Client only uses LSB, so we have 3 "free" bytes.
			// We cannot append data to the packet, as client also uses trailer for user IDs
			memcpy(serializer->Bitstream->Buf + 2, ExtenderMsgSignature, 3);
		} else if (memcmp(serializer->Bitstream->Buf + 2, ExternderNullSignature, 3) != 0) {
			if (memcmp(serializer->Bitstream->Buf + 2, ExtenderMsgSignature, 3) == 0) {
				DEBUG("Sending ExtenderHello to server");
				ClientAllowExtenderMessages();
				gOsirisProxy->GetNetworkManager().ExtendNetworkingClient();
				auto helloMsg = GetFreeClientMessage();
				if (helloMsg != nullptr) {
					helloMsg->GetMessage().mutable_c2s_extender_hello();
					ClientSend(helloMsg);
				}
				else {
					OsiErrorS("Could not get free message!");
				}
			} else {
				WARN("Extender signature incorrect in ClientAccept message");
			}
		} else {
			DEBUG("No extender trailer found in ClientAccept message");
		}
	}

	ScriptExtenderMessage * NetworkManager::GetFreeClientMessage()
	{
		// We need to make sure that no extender message is sent if the other party
		// does not have the new message ID-s installed, otherwise the peer will crash
		// while trying to parse the packet.
		if (!clientExtenderSupport_) {
			ERR("Attempted to send extender message to a host that does not understand extender protocol!");
			return nullptr;
		}

		auto client = GetClient();
		if (client != nullptr) {
			return client->GetFreeMessage<ScriptExtenderMessage>();
		} else {
			return nullptr;
		}
	}

	ScriptExtenderMessage * NetworkManager::GetFreeServerMessage(UserId userId)
	{
		if (userId && !ServerCanSendExtenderMessages(userId.GetPeerId())) {
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

	void NetworkManager::ClientSend(ScriptExtenderMessage * msg)
	{
		auto client = GetClient();
		if (client != nullptr) {
			client->VMT->ClientSend(client, client->ClientPeerId, msg);
		}
	}

	void NetworkManager::ServerSend(ScriptExtenderMessage * msg, UserId userId)
	{
		auto server = GetServer();
		if (server != nullptr) {
			server->VMT->SendToPeer(server, &userId.Id, msg);
		}
	}

	void NetworkManager::ServerBroadcast(ScriptExtenderMessage * msg, UserId excludeUserId)
	{
		auto server = GetServer();
		if (server != nullptr) {
			ObjectSet<PeerId> peerIds;
			for (auto peerId : server->ActivePeerIds) {
				if (ServerCanSendExtenderMessages(peerId)) {
					peerIds.Set.Add(peerId);
				} else {
					WARN("Not sending extender message to peer %d as it does not understand extender protocol!", peerId);
				}
			}

			server->VMT->SendToMultiplePeers(server, &peerIds, msg, excludeUserId.Id);
		}
	}

	void NetworkManager::ServerBroadcastToConnectedPeers(ScriptExtenderMessage* msg, UserId excludeUserId)
	{
		auto server = GetServer();
		if (server != nullptr) {
			ObjectSet<PeerId> peerIds;
			peerIds.Set.Reallocate(server->ConnectedPeerIds.Set.Size);
			for (auto peerId : server->ConnectedPeerIds) {
				peerIds.Set.Add(peerId);
			}
			server->VMT->SendToMultiplePeers(server, &peerIds, msg, excludeUserId.Id);
		}
	}


	void NetworkFixedStringSynchronizer::Dump()
	{
		auto nfs = GetStaticSymbols().NetworkFixedStrings;
		if (nfs != nullptr && (*nfs)->Initialized) {
			auto const& strings = (*nfs)->FixedStrSet.Set;

			auto nfsLogPath = gOsirisProxy->MakeLogFilePath(L"NetworkFixedStrings", L"log");
			std::ofstream logOut(nfsLogPath.c_str(), std::ios::out);
			for (auto const& string : strings) {
				logOut << (string ? string.Str : "(NULL)") << std::endl;
			}
			logOut.close();
			DEBUG(L"OsirisProxy::DumpNetworkFixedStrings() - Saved to %s", nfsLogPath.c_str());
		} else {
			ERR("OsirisProxy::DumpNetworkFixedStrings() - Fixed strings not initialized yet");
		}
	}

	void NetworkFixedStringSynchronizer::RequestFromServer()
	{
		if (!gOsirisProxy->GetNetworkManager().ClientCanSendExtenderMessages()) {
			OsiWarnS("Not syncing fixedstrings - host has no extender support");
			return;
		}

		DEBUG("Requesting NetworkFixedStrings from server");
		auto& networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeClientMessage();
		if (msg != nullptr) {
			msg->GetMessage().mutable_c2s_request_strings();
			networkMgr.ClientSend(msg);
		}
		else {
			OsiErrorS("Could not get free message!");
		}
	}

	void NetworkFixedStringSynchronizer::FlushQueuedRequests()
	{
		DEBUG("Flushing NetworkFixedString updates");
		for (auto userId : pendingSyncRequests_) {
			SendUpdateToUser(userId);
		}

		pendingSyncRequests_.clear();
	}

	void NetworkFixedStringSynchronizer::OnUpdateRequested(UserId userId)
	{
		auto gameState = *GetStaticSymbols().GetServerState();
		if (gameState == esv::GameState::LoadSession
			|| gameState == esv::GameState::LoadLevel
			|| gameState == esv::GameState::BuildStory
			|| gameState == esv::GameState::Sync
			|| gameState == esv::GameState::Running) {
			DEBUG("Fulfill requested NetworkFixedString update for user %d", userId.Id);
			SendUpdateToUser(userId);
		} else {
			DEBUG("Queuing requested NetworkFixedString update for user %d", userId.Id);
			pendingSyncRequests_.insert(userId);
		}
	}

	void NetworkFixedStringSynchronizer::SendUpdateToUser(UserId userId)
	{
		auto fixedStrs = GetStaticSymbols().NetworkFixedStrings;
		if (fixedStrs == nullptr || *fixedStrs == nullptr) {
			return;
		}

		DEBUG("Sending NetworkFixedString table to user %d", userId.Id);
		auto& nfs = **fixedStrs;
		auto& networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeServerMessage(userId);
		if (msg != nullptr) {
			auto syncMsg = msg->GetMessage().mutable_s2c_sync_strings();
			auto numStrings = nfs.FixedStrSet.Set.Size;
			for (uint32_t i = 1; i < numStrings; i++) {
				syncMsg->add_network_string(nfs.FixedStrSet[i].Str);
			}

			networkMgr.ServerSend(msg, userId);
		}
		else {
			OsiErrorS("Could not get free message!");
		}
	}

	void NetworkFixedStringSynchronizer::UpdateFromServer()
	{
		auto fixedStrs = GetStaticSymbols().NetworkFixedStrings;
		if (updatedStrings_.empty()
			|| fixedStrs == nullptr
			|| *fixedStrs == nullptr
			|| (*fixedStrs)->FixedStrSet.Set.Size == 0) {
			return;
		}

		DEBUG("Updating NetworkFixedStrings from server");
		auto& fs = **fixedStrs;
		auto numStrings = (uint32_t)updatedStrings_.size();

		auto sizeMin = std::min(fs.FixedStrSet.Set.Size - 1, numStrings);
		uint32_t brokenNum = 0;
		for (uint32_t i = 0; i < sizeMin; i++) {
			auto const& serverString = updatedStrings_[i];
			auto clientString = fs.FixedStrSet[i + 1];
			if (serverString != clientString.Str) {
				ERR("NetworkFixedStrings mismatch - entry %d different! %s vs %s", i, serverString.c_str(), clientString.Str);

				// Find out which string caused the conflict
				conflictingString_.clear();
				for (auto str : updatedStrings_) {
					if (str == clientString.Str) {
						// Client string exists in server map --> server string missing from local NetworkFixedStrings
						conflictingString_ = serverString;
					}
				}

				if (conflictingString_.empty()) {
					// Client string not found on server
					conflictingString_ = clientString.Str;
				}

				notInSync_ = true;
				break;
			}
		}

		if (notInSync_) {
			fs.FixedStrSet.Set.Clear();
			fs.FixedStrToNetIndexMap.Clear();

			fs.FixedStrSet.Set.Reallocate(numStrings + 1);

			fs.FixedStrSet.Set.Add(FixedString{});
			fs.FixedStrToNetIndexMap.Insert(FixedString{}, 1);

			for (uint32_t i = 0; i < numStrings; i++) {
				auto fixedStr = MakeFixedString(updatedStrings_[i].c_str());
				fs.FixedStrSet.Set.Add(fixedStr);
				fs.FixedStrToNetIndexMap.Insert(fixedStr, i + 2);
			}
		}
	}

	void NetworkFixedStringSynchronizer::ClientReset()
	{
		updatedStrings_.clear();
		notInSync_ = false;
		syncWarningShown_ = false;
	}

	void NetworkFixedStringSynchronizer::ClientLoaded()
	{
		if (notInSync_ && !syncWarningShown_) {
			STDWString msg(L"Script Extender has detected a desync issue! Make sure that mod versions are the same on both sides.\r\nFirst mismatching object: ");
			msg += FromUTF8(conflictingString_);
			gOsirisProxy->GetLibraryManager().ShowStartupError(msg, false);
			syncWarningShown_ = true;
		}
	}
}
