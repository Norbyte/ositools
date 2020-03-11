#include <stdafx.h>
#include <NetProtocol.h>
#include <GameDefinitions/Symbols.h>
#include <OsirisProxy.h>

namespace osidbg
{
	ExtenderProtocol::~ExtenderProtocol() {}

	bool ExtenderProtocol::ProcessMsg(void * Unused, net::MessageContext * Unknown, net::Message * Msg)
	{
		if (Msg->MsgId == ScriptExtenderMessage::MessageId) {
			auto msg = static_cast<ScriptExtenderMessage *>(Msg);
			if (msg->IsValid()) {
				ProcessExtenderMessage(msg->GetMessage());
			}
			return true;
		}

		return false;
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

	void ExtenderProtocolClient::ProcessExtenderMessage(MessageWrapper & msg)
	{
		switch (msg.msg_case()) {
		case MessageWrapper::kPostLuaMessage:
		{
			auto & postMsg = msg.post_lua_message();
			LuaClientPin pin(ExtensionStateClient::Get());
			if (pin) {
				pin->OnNetMessageReceived(postMsg.channel_name(), postMsg.payload());
			}
			break;
		}

		default:
			OsiErrorS("Unknown extension message type received!");
		}
	}

	void ExtenderProtocolServer::ProcessExtenderMessage(MessageWrapper & msg)
	{
		switch (msg.msg_case()) {
		case MessageWrapper::kPostLuaMessage:
		{
			auto & postMsg = msg.post_lua_message();
			LuaServerPin pin(ExtensionStateServer::Get());
			if (pin) {
				pin->OnNetMessageReceived(postMsg.channel_name(), postMsg.payload());
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
	}

	ScriptExtenderMessage::~ScriptExtenderMessage() {}

	void ScriptExtenderMessage::Serialize(net::BitstreamSerializer & serializer)
	{
		if (serializer.IsWriting) {
			uint32_t size = (uint32_t)message_.ByteSizeLong();
			if (size <= MaxPayloadLength) {
				serializer.WriteBytes(&size, sizeof(size));
				void * buf = GameAllocRaw(size);
				message_.SerializeToArray(buf, size);
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
				valid_ = message_.ParseFromArray(buf, size);
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
		message_.Clear();
		valid_ = false;
	}


	void NetworkManager::ExtendNetworkingClient()
	{
		if (!gOsirisProxy->HasFeatureFlag("Net")) return;
		if (clientProtocol_ != nullptr) return;

		auto client = GetClient();
		if (GetStaticSymbols().net__Host__AddProtocol != nullptr
			&& GetStaticSymbols().net__MessageFactory__RegisterMessage != nullptr
			&& client != nullptr) {
			clientProtocol_ = new ExtenderProtocolClient();
			// Nasty hack to avoid having to lookup net::Client::AddProtocol
			auto gameClientPtr = (net::Host *)((uintptr_t)client + 8);
			GetStaticSymbols().net__Host__AddProtocol(gameClientPtr, 100, clientProtocol_);

			auto extenderMsg = new ScriptExtenderMessage();
			GetStaticSymbols().net__MessageFactory__RegisterMessage(client->NetMessageFactory, 
				ScriptExtenderMessage::MessageId, extenderMsg, 4, "ScriptExtenderMessage");
			DEBUG("Registered custom ecl network protocol");
		} else {
			ERR("Could not register ecl protocol - symbols not mapped");
		}
	}

	void NetworkManager::ExtendNetworkingServer()
	{
		if (!gOsirisProxy->HasFeatureFlag("Net")) return;
		if (clientProtocol_ != nullptr) return;

		auto server = GetServer();
		if (GetStaticSymbols().net__Host__AddProtocol != nullptr
			&& GetStaticSymbols().net__MessageFactory__RegisterMessage != nullptr
			&& server != nullptr) {
			serverProtocol_ = new ExtenderProtocolServer();
			GetStaticSymbols().net__Host__AddProtocol(server, 100, serverProtocol_);

			auto extenderMsg = new ScriptExtenderMessage();
			GetStaticSymbols().net__MessageFactory__RegisterMessage(server->NetMessageFactory,
				ScriptExtenderMessage::MessageId, extenderMsg, 4, "ScriptExtenderMessage");
			DEBUG("Registered custom esv network protocol");
		} else {
			ERR("Could not register esv protocol - symbols not mapped");
		}
	}

	net::Host * NetworkManager::GetServer() const
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
}