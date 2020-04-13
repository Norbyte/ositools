#include <stdafx.h>
#include <NetProtocol.h>
#include <GameDefinitions/Symbols.h>
#include <OsirisProxy.h>
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

		gOsirisProxy->NetworkFixedStringSync().SetServerNetworkFixedStrings(strings);
		gOsirisProxy->NetworkFixedStringSync().UpdateFromServer();
	}

	void ExtenderProtocolClient::ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg)
	{
		switch (msg.msg_case()) {
		case MessageWrapper::kPostLua:
		{
			auto & postMsg = msg.post_lua();
			ecl::LuaClientPin pin(ecl::ExtensionState::Get());
			if (pin) {
				pin->OnNetMessageReceived(STDString(postMsg.channel_name()), STDString(postMsg.payload()));
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
				pin->OnNetMessageReceived(STDString(postMsg.channel_name()), STDString(postMsg.payload()));
			}
			break;
		}

		case MessageWrapper::kC2SRequestStrings:
		{
			if (gOsirisProxy->GetConfig().SyncNetworkStrings) {
				gOsirisProxy->NetworkFixedStringSync().OnUpdateRequested((int32_t)context.UserId);
			}
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


	void NetworkManager::ExtendNetworkingClient()
	{
		auto client = GetClient();
		if (client != nullptr
			&& client->NetMessageFactory->MessagePools.Size >= ScriptExtenderMessage::MessageId) {
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
			client->NetMessageFactory->ReservePools(ScriptExtenderMessage::MessageId + 1);
			GetStaticSymbols().net__MessageFactory__RegisterMessage(client->NetMessageFactory, 
				ScriptExtenderMessage::MessageId, extenderMsg, 4, "ScriptExtenderMessage");
			DEBUG("Registered custom ecl network protocol");
		} else {
			ERR("Could not register ecl protocol - symbols not mapped");
		}
	}

	void NetworkManager::ExtendNetworkingServer()
	{
		auto server = GetServer();
		if (server != nullptr
			&& server->NetMessageFactory->MessagePools.Size >= ScriptExtenderMessage::MessageId) {
			return;
		}

		if (GetStaticSymbols().net__Host__AddProtocol != nullptr
			&& GetStaticSymbols().net__MessageFactory__RegisterMessage != nullptr
			&& server != nullptr) {
			serverProtocol_ = new ExtenderProtocolServer();
			GetStaticSymbols().net__Host__AddProtocol(server, 100, serverProtocol_);

			auto extenderMsg = new ScriptExtenderMessage();
			server->NetMessageFactory->ReservePools(ScriptExtenderMessage::MessageId + 1);
			GetStaticSymbols().net__MessageFactory__RegisterMessage(server->NetMessageFactory,
				ScriptExtenderMessage::MessageId, extenderMsg, 4, "ScriptExtenderMessage");
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

	ScriptExtenderMessage * NetworkManager::GetFreeClientMessage()
	{
		auto client = GetClient();
		if (client != nullptr) {
			return client->GetFreeMessage<ScriptExtenderMessage>();
		} else {
			return nullptr;
		}
	}

	ScriptExtenderMessage * NetworkManager::GetFreeServerMessage()
	{
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

	void NetworkManager::ServerSend(ScriptExtenderMessage * msg, int32_t peerId)
	{
		auto server = GetServer();
		if (server != nullptr) {
			server->VMT->SendToPeer(server, &peerId, msg);
		}
	}

	void NetworkManager::ServerBroadcast(ScriptExtenderMessage * msg, int32_t excludePeerId)
	{
		auto server = GetServer();
		if (server != nullptr) {
			ObjectSet<int32_t> peerIds;
			peerIds.Set.Reallocate(server->ActivePeerIds.Set.Size);
			for (uint32_t i = 0; i < server->ActivePeerIds.Set.Size; i++) {
				peerIds.Set.Add(server->ActivePeerIds[i]);
			}
			server->VMT->SendToMultiplePeers(server, &peerIds, msg, excludePeerId);
		}
	}

	void NetworkManager::ServerBroadcastToConnectedPeers(ScriptExtenderMessage* msg, int32_t excludePeerId)
	{
		auto server = GetServer();
		if (server != nullptr) {
			ObjectSet<int32_t> peerIds;
			peerIds.Set.Reallocate(server->ConnectedPeerIds.Set.Size);
			for (uint32_t i = 0; i < server->ConnectedPeerIds.Set.Size; i++) {
				peerIds.Set.Add(server->ConnectedPeerIds[i]);
			}
			server->VMT->SendToMultiplePeers(server, &peerIds, msg, excludePeerId);
		}
	}


	void NetworkFixedStringSynchronizer::Dump()
	{
		auto nfs = GetStaticSymbols().NetworkFixedStrings;
		if (nfs != nullptr && (*nfs)->Initialized) {
			auto const& strings = (*nfs)->FixedStrSet.Set;

			auto nfsLogPath = gOsirisProxy->MakeLogFilePath(L"NetworkFixedStrings", L"log");
			std::ofstream logOut(nfsLogPath.c_str(), std::ios::out);
			for (uint32_t i = 0; i < strings.Size; i++) {
				auto str = strings[i].Str;
				logOut << (str == nullptr ? "(NULL)" : str) << std::endl;
			}
			logOut.close();
			DEBUG(L"OsirisProxy::DumpNetworkFixedStrings() - Saved to %s", nfsLogPath.c_str());
		} else {
			ERR("OsirisProxy::DumpNetworkFixedStrings() - Fixed strings not initialized yet");
		}
	}

	void NetworkFixedStringSynchronizer::RequestFromServer()
	{
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
		for (auto peerId : pendingSyncRequests_) {
			SendUpdateToPeer(peerId);
		}

		pendingSyncRequests_.clear();
	}

	void NetworkFixedStringSynchronizer::OnUpdateRequested(int32_t peerId)
	{
		auto gameState = *GetStaticSymbols().GetServerState();
		if (gameState == esv::GameState::LoadSession
			|| gameState == esv::GameState::LoadLevel
			|| gameState == esv::GameState::BuildStory
			|| gameState == esv::GameState::Sync
			|| gameState == esv::GameState::Running) {
			DEBUG("Fulfill requested NetworkFixedString update for peer %d", peerId);
			SendUpdateToPeer(peerId);
		} else {
			DEBUG("Queuing requested NetworkFixedString update for peer %d", peerId);
			pendingSyncRequests_.insert(peerId);
		}
	}

	void NetworkFixedStringSynchronizer::SendUpdateToPeer(int32_t peerId)
	{
		auto fixedStrs = GetStaticSymbols().NetworkFixedStrings;
		if (fixedStrs == nullptr || *fixedStrs == nullptr) {
			return;
		}

		DEBUG("Sending NetworkFixedString table to peer %d", peerId);
		auto& nfs = **fixedStrs;
		auto& networkMgr = gOsirisProxy->GetNetworkManager();
		auto msg = networkMgr.GetFreeServerMessage();
		if (msg != nullptr) {
			auto syncMsg = msg->GetMessage().mutable_s2c_sync_strings();
			auto numStrings = nfs.FixedStrSet.Set.Size;
			for (uint32_t i = 1; i < numStrings; i++) {
				syncMsg->add_network_string(nfs.FixedStrSet[i].Str);
			}

			networkMgr.ServerSend(msg, peerId);
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
