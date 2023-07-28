#include <stdafx.h>
#include <Extender/Client/NetworkManagerClient.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(ecl)

void ExtenderProtocol::SyncNetworkStrings(MsgS2CSyncNetworkFixedStrings const& msg)
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
	DEBUG("Got FixedString list from server");
	gExtender->GetClient().GetNetworkFixedStrings().SetServerNetworkFixedStrings(strings);

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
		gExtender->GetClient().GetNetworkFixedStrings().UpdateFromServer();
	}
}

void ExtenderProtocol::SyncEntityGuids(MsgUserVars const& msg)
{
	auto state = gExtender->GetCurrentExtensionState();
	for (auto const& guidMap : msg.guids()) {
		switch (guidMap.net_id_type()) {
		case NETID_CHARACTER:
		{
			auto character = GetEntityWorld()->GetComponent<Character>(NetId(guidMap.net_id()));
			if (character != nullptr) {
				SyncEntityGuid(character, FixedString(guidMap.guid()));
			}
			break;
		}

		default:
			ERR("Received entity guid sync for unknown NetID class %d!", guidMap.net_id_type());
			break;
		}
	}
}

void ExtenderProtocol::SyncEntityGuid(Character* character, FixedString const& guid)
{
	if (!character->GetGuid() || !*character->GetGuid()->GetStringOrDefault()) {
		auto pool = GetEntityWorld()->Components[(uint32_t)Character::ComponentPoolIndex].Pool;
		auto factory = reinterpret_cast<NetworkComponentFactory<Character>*>((std::intptr_t)pool + 8);
		factory->Guids.insert(guid, character);
		character->SetGuid(guid);

		WARN("Character %s has no GUID on client. Repaired.", guid.GetStringOrDefault());
	}
}

void ExtenderProtocol::ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg)
{
	switch (msg.msg_case()) {
	case MessageWrapper::kPostLua:
	{
		auto & postMsg = msg.post_lua();
		ecl::LuaClientPin pin(ecl::ExtensionState::Get());
		if (pin) {
			pin->OnNetMessageReceived(STDString(postMsg.channel_name()), STDString(postMsg.payload()), ReservedUserId);
		}
		break;
	}

	case MessageWrapper::kS2CResetLua:
	{
		auto & resetMsg = msg.s2c_reset_lua();
		auto & ext = ecl::ExtensionState::Get();
		ext.AddPostResetCallback([&ext]() {
			ext.OnModuleResume();
			ext.OnGameSessionLoading();
			ext.OnGameSessionLoaded();
			ext.OnResetCompleted();
		});
		ext.LuaReset(resetMsg.bootstrap_scripts());
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

	case MessageWrapper::kS2CKick:
	{
		gExtender->GetLibraryManager().ShowStartupMessage(FromUTF8(msg.s2c_kick().message()), true);
		break;
	}

	case MessageWrapper::kUserVars:
	{
		SyncEntityGuids(msg.user_vars());
		SyncUserVars(msg.user_vars());
		break;
	}

	default:
		OsiErrorS("Unknown extension message type received!");
	}
}

void NetworkManager::Reset()
{
	extenderSupport_ = false;
}

bool NetworkManager::CanSendExtenderMessages() const
{
	return extenderSupport_;
}

void NetworkManager::AllowExtenderMessages()
{
	extenderSupport_ = true;
}


void NetworkManager::ExtendNetworking()
{
	auto client = GetClient();
	if (client != nullptr
		&& client->NetMessageFactory->MessagePools.size() >= (unsigned)ScriptExtenderMessage::MessageId) {
		return;
	}

	if (GetStaticSymbols().net__Host__AddProtocol != nullptr
		&& GetStaticSymbols().net__MessageFactory__RegisterMessage != nullptr
		&& client != nullptr) {
		protocol_ = new ExtenderProtocol();
		// Nasty hack to avoid having to lookup net::Client::AddProtocol
		auto gameClientPtr = (net::Host *)((uintptr_t)client + 8);
		GetStaticSymbols().net__Host__AddProtocol(gameClientPtr, 100, protocol_);

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
	if (wasHooked_) return;

	// FIXME - should separate from Osi!
	gExtender->GetServer().Osiris().GetWrappers().InitializeNetworking(messageFactory);

	auto& connectMsg = gExtender->GetServer().Osiris().GetWrappers().eocnet__ClientConnectMessage__Serialize;
	auto& acceptMsg = gExtender->GetServer().Osiris().GetWrappers().eocnet__ClientAcceptMessage__Serialize;
	connectMsg.SetPostHook(&NetworkManager::OnConnectMessage, this);
	acceptMsg.SetPostHook(&NetworkManager::OnAcceptMessage, this);
	wasHooked_ = true;
}

net::Client * NetworkManager::GetClient() const
{
	auto client = GetStaticSymbols().ecl__EoCClient;
	if (client != nullptr && *client != nullptr) {
		return (*client)->GameClient;
	} else {
		return nullptr;
	}
}

char const* ExtenderMsgSignature = "EX1";
uint8_t ExternderNullSignature[3] = { 0, 0, 0 };

void NetworkManager::OnConnectMessage(net::Message* msg, net::BitstreamSerializer* serializer)
{
}

void NetworkManager::OnAcceptMessage(net::Message* msg, net::BitstreamSerializer* serializer)
{
	if (serializer->IsWriting) {
		DEBUG("Appending extender signature to ClientAccept");
		// Client only uses LSB, so we have 3 "free" bytes.
		// We cannot append data to the packet, as client also uses trailer for user IDs
		memcpy(serializer->Bitstream->Buf + 2, ExtenderMsgSignature, 3);
	} else if (memcmp(serializer->Bitstream->Buf + 2, ExternderNullSignature, 3) != 0) {
		if (memcmp(serializer->Bitstream->Buf + 2, ExtenderMsgSignature, 3) == 0) {
			DEBUG("Sending ExtenderHello to server");
			AllowExtenderMessages();
			ExtendNetworking();
			auto helloMsg = GetFreeMessage();
			if (helloMsg != nullptr) {
				auto hello = helloMsg->GetMessage().mutable_c2s_extender_hello();
				hello->set_version(ScriptExtenderMessage::ProtoVersion);
				Send(helloMsg);
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

ScriptExtenderMessage * NetworkManager::GetFreeMessage()
{
	// We need to make sure that no extender message is sent if the other party
	// does not have the new message ID-s installed, otherwise the peer will crash
	// while trying to parse the packet.
	if (!extenderSupport_) {
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

void NetworkManager::Send(ScriptExtenderMessage * msg)
{
	auto client = GetClient();
	if (client != nullptr) {
		client->VMT->ClientSend(client, client->ClientPeerId, msg);
	}
}


void NetworkFixedStringReceiver::RequestFromServer()
{
	if (!network_.CanSendExtenderMessages()) {
		OsiWarnS("Not syncing fixedstrings - host has no extender support");
		return;
	}

	DEBUG("Requesting NetworkFixedStrings from server");
	auto msg = network_.GetFreeMessage();
	if (msg != nullptr) {
		msg->GetMessage().mutable_c2s_request_strings();
		network_.Send(msg);
	} else {
		OsiErrorS("Could not get free message!");
	}
}

void NetworkFixedStringReceiver::UpdateFromServer()
{
	auto fixedStrs = GetStaticSymbols().NetworkFixedStrings;
	if (updatedStrings_.empty()
		|| fixedStrs == nullptr
		|| *fixedStrs == nullptr
		|| (*fixedStrs)->FixedStrSet.size() == 0) {
		return;
	}

	DEBUG("Updating NetworkFixedStrings from server");
	auto& fs = **fixedStrs;
	auto numStrings = (uint32_t)updatedStrings_.size();

	auto sizeMin = std::min(fs.FixedStrSet.size() - 1, numStrings);
	uint32_t brokenNum = 0;
	for (uint32_t i = 0; i < sizeMin; i++) {
		auto const& serverString = updatedStrings_[i];
		auto clientString = fs.FixedStrSet[i + 1];
		if (serverString != clientString.GetString()) {
			ERR("NetworkFixedStrings mismatch - entry %d different! %s vs %s", i, serverString.c_str(), clientString.GetString());

			// Find out which string caused the conflict
			conflictingString_.clear();
			for (auto str : updatedStrings_) {
				if (str == clientString.GetString()) {
					// Client string exists in server map --> server string missing from local NetworkFixedStrings
					conflictingString_ = serverString;
				}
			}

			if (conflictingString_.empty()) {
				// Client string not found on server
				conflictingString_ = clientString.GetString();
			}

			notInSync_ = true;
			break;
		}
	}

	if (notInSync_) {
		fs.FixedStrSet.clear();
		fs.FixedStrToNetIndexMap.clear();

		fs.FixedStrSet.reallocate(numStrings + 1);

		fs.FixedStrSet.push_back(FixedString{});
		fs.FixedStrToNetIndexMap.insert(FixedString{}, 1);

		for (uint32_t i = 0; i < numStrings; i++) {
			FixedString fixedStr(updatedStrings_[i].c_str());
			fs.FixedStrSet.push_back(fixedStr);
			fs.FixedStrToNetIndexMap.insert(fixedStr, i + 2);
		}
	}
}

void NetworkFixedStringReceiver::ClientReset()
{
	updatedStrings_.clear();
	notInSync_ = false;
	syncWarningShown_ = false;
}

void NetworkFixedStringReceiver::ClientLoaded()
{
	if (notInSync_ && !syncWarningShown_) {
		STDWString msg(L"Script Extender has detected a desync issue! Make sure that mod versions are the same on both sides.\r\nFirst mismatching object: ");
		msg += FromUTF8(conflictingString_);
		gExtender->GetLibraryManager().ShowStartupError(msg, false);
		syncWarningShown_ = true;
	}
}

END_NS()