#include <stdafx.h>
#include <NetProtocol.h>
#include <GameDefinitions/Symbols.h>
#include <Extender/ScriptExtender.h>
#include <Version.h>
#include <fstream>

BEGIN_SE()

namespace net
{
	Message* AbstractPeer::DoGetFreeMessage(NetMessage messageId)
	{
		auto getMsg = GetStaticSymbols().net__MessageFactory__GetFreeMessage;
		if (getMsg == nullptr) return nullptr;

		if (NetMessageFactory->MessagePools.Size > (unsigned)messageId) {
			return getMsg(NetMessageFactory, messageId);
		} else {
			ERR("GetFreeMessage(): Message factory not registered for this message type?");
			return nullptr;
		}
	}

	void MessageFactory::ReservePools(uint32_t minPools)
	{
		if (MessagePools.Capacity >= minPools) return;

		MessagePools.Reallocate(minPools);
	}
}


ExtenderProtocolBase::~ExtenderProtocolBase() {}

net::MessageStatus ExtenderProtocolBase::ProcessMsg(void * Unused, net::MessageContext * Context, net::Message * Msg)
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

void ExtenderProtocolBase::Unknown1() {}

int ExtenderProtocolBase::PreUpdate(GameTime* Time)
{
	return 0;
}

int ExtenderProtocolBase::PostUpdate(GameTime* Time)
{
	return 0;
}

void * ExtenderProtocolBase::OnAddedToHost()
{
	return nullptr;
}

void * ExtenderProtocolBase::OnRemovedFromHost()
{
	return nullptr;
}

void * ExtenderProtocolBase::Unknown2()
{
	return nullptr;
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

END_SE()
