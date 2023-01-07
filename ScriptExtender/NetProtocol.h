#pragma once

#include <GameDefinitions/Net.h>
#include <Extender/Shared/ScriptExtensions.pb.h>

namespace dse
{
	class ScriptExtenderMessage : public net::Message
	{
	public:
		static constexpr NetMessage MessageId = NetMessage::NETMSG_SCRIPT_EXTENDER;
		static constexpr uint32_t MaxPayloadLength = 0xfffff;

		static constexpr uint32_t VerAddedKickMessage = 1;
		// Version that fixed mod hashes so they ignore path overrides
		static constexpr uint32_t VerCorrectedHashes = 2;
		// Added user variable sync
		static constexpr uint32_t VerUserVariables = 3;
		// Version of protocol, increment each time the protobuf changes
		static constexpr uint32_t ProtoVersion = VerUserVariables;

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

	class ExtenderProtocolBase : public net::Protocol
	{
	public:
		~ExtenderProtocolBase() override;

		net::MessageStatus ProcessMsg(void * Unused, net::MessageContext * Unknown, net::Message * Msg) override;
		void Unknown1() override;
		int PreUpdate(GameTime* Time) override;
		int PostUpdate(GameTime* Time) override;
		void * OnAddedToHost() override;
		void * OnRemovedFromHost() override;
		void * Unknown2() override;

	protected:
		virtual void ProcessExtenderMessage(net::MessageContext& context, MessageWrapper & msg) = 0;
	};
}
