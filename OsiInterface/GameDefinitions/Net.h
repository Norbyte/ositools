#pragma once

#include <windows.h>
#include "BaseTypes.h"
#include "Enumerations.h"

namespace dse
{

#pragma pack(push, 1)
	namespace net
	{
		struct Bitstream : ProtectedGameObject<Bitstream>
		{
			uint32_t NumBits;
			uint32_t AllocatedBits;
			uint32_t CurrentOffsetBits;
			uint8_t _Pad[4];
			void* Buf;
			bool Unknown;
			uint8_t _Pad1[7];
		};

		struct BitstreamSerializer : ProtectedGameObject<BitstreamSerializer>
		{
			virtual void WriteBytes(void const* Buf, uint64_t Size) = 0;
			virtual void ReadBytes(void* Buf, uint64_t Size) = 0;

			bool IsWriting;
			uint8_t _Pad[7];
			Bitstream* Bitstream;
		};

		struct MessageVMT
		{
			void* Dtor;
			void* Serialize;
			void* Unknown;
			void* CreateNew;
			void* Reset;
		};

		struct Message
		{
			inline virtual ~Message() {}
			virtual void Serialize(BitstreamSerializer & serializer) = 0;
			virtual void Unknown() = 0;
			virtual Message * CreateNew() = 0;
			virtual void Reset() = 0;

			uint32_t MsgId;
			uint32_t Always4{ 4 };
			uint32_t MsgType{ 1 };
			uint8_t Always0{ 0 };
			uint8_t Unknown1{ 0 };
			uint8_t _Pad1[2];
			uint64_t ReceivedAtTime{ 0 };
			uint32_t ReceiveTimeDelta{ 0 };
			uint32_t Unknown4{ 0 };
		};

		struct MessagePool
		{
			Message* Template;
			uint64_t Unknown[4];
			ObjectSet<Message*> Messages;
			STDString Name;
		};

		struct MessageFactory
		{
			typedef void (* RegisterMessage)(MessageFactory * self, int messageId, Message * msg, 
				unsigned int msgType, const char * messageName);
			typedef Message * (* GetFreeMessage)(MessageFactory * self, int messageId);
			
			void ReservePools(uint32_t minPools);
			
			Array<MessagePool*> MessagePools;
			CRITICAL_SECTION CriticalSection;
		};

		struct MessageContext
		{
			PeerId UserId;
			uint8_t _Pad1[4];
			ObjectSet<uint16_t> PeerIDClassNames;
			ObjectSet<uint32_t> UserIDs;
			Message * Msg;
			uint32_t Unknown1;
			uint8_t Unknown2;
		};

		enum class MessageStatus : int32_t
		{
			Unhandled = 0,
			Handled = 1,
			Unknown2 = 2,
			Unknown3 = 3
		};

		struct Protocol
		{
			inline virtual ~Protocol() {}
			virtual MessageStatus ProcessMsg(void * Unused, MessageContext * Unknown, Message * Msg) = 0;
			virtual void Unknown1() = 0;
			virtual int PreUpdate(void * Unknown) = 0;
			virtual int PostUpdate(void * Unknown) = 0;
			virtual void * OnAddedToHost() = 0;
			virtual void * OnRemovedFromHost() = 0;
			virtual void * Unknown2() = 0;

			struct AbstractPeer * Peer;
		};

		struct AbstractPeer;
		struct AbstractPeerVMT
		{
			typedef void (* SendToPeerProc)(AbstractPeer *, int32_t *, Message *);
			typedef void (* SendToMultiplePeersProc)(AbstractPeer *, ObjectSet<int32_t> *, Message *, int32_t);
			typedef void (* ClientSendProc)(AbstractPeer *, int32_t, Message *);

			uint64_t Unknown[27];
			SendToPeerProc SendToPeer;
			uint64_t Unknown2[3];
			SendToMultiplePeersProc SendToMultiplePeers;
			ClientSendProc ClientSend;
		};

		struct AbstractPeer
		{
			AbstractPeerVMT * VMT;
			void * VMT2;
			uint64_t field_10;
			int field_18;
			int field_1C;
			STDString stdString1;
			uint16_t field_40;
			uint8_t _Pad1[6];
			STDString stdString2;
			uint64_t field_68;
			uint64_t field_70;
			uint64_t FileTransfer[10];
			ObjectSet<PeerId> SessionPeerIds;
			uint64_t Unknown3;
			ObjectSet<PeerId> LevelPeerIds;
			uint64_t Unknown5[18];
#if !defined(OSI_EOCAPP)
			uint64_t FileTransfer2;
#endif
			ObjectSet<PeerId> PeerIdClassNames;
			ObjectSet<void *> DisconnectDelays;
			void * field_1E8;
			MessageFactory * NetMessageFactory;
			uint64_t field_1F8;
			void * NetEventManager;
			uint64_t field_208;
			uint64_t field_210;
			uint64_t field_218;
			char field_220;
			uint8_t _Pad2[7];
			uint64_t field_228;
			uint64_t field_230;
			int field_238;
			uint8_t _Pad3[4];
			uint64_t GameTime;
			uint64_t field_248;
			int field_250;
			uint16_t field_254;
			uint16_t field_256;
			float field_258;
			int field_25C;
			int field_260;
			int field_264;
			STDString stdString3;
			uint16_t field_288;
			uint8_t _Pad4[6];
			STDString stdString4;
			char field_2B0;
			uint8_t _Pad5[3];
			int field_2B4;
			uint64_t field_2B8;
			uint64_t field_2C0;
			uint64_t field_2C8;
			uint64_t field_2D0;
			uint64_t field_2D8;
			uint64_t field_2E0;
			uint64_t field_2E8;
#if !defined(OSI_EOCAPP)
			uint64_t field_2F0;
			STDString stdString5;
#endif

			template <class T>
			T * GetFreeMessage()
			{
				auto getMsg = GetStaticSymbols().net__MessageFactory__GetFreeMessage;
				if (getMsg == nullptr) return nullptr;

				if (NetMessageFactory->MessagePools.Size > T::MessageId) {
					return static_cast<T *>(getMsg(NetMessageFactory, T::MessageId));
				} else {
					ERR("GetFreeMessage(): Message factory not registered for this message type?");
					return nullptr;
				}
			}
		};

		struct Host : public AbstractPeer
		{
			typedef void (* AddProtocol)(Host * self, int protocolId, Protocol * protocol);

			void * VMT3;
			ObjectSet<void *> Protocols;
		};

		struct Client : public AbstractPeer
		{
			typedef void (* AddProtocol)(Client * self, int protocolId, Protocol * protocol);

			void * VMT3;
			PeerId ClientPeerId;
			uint8_t _Pad5[4];
			ObjectSet<void *> Protocols;
		};

		struct GameServer : public Host
		{
			void * EventListenerVMT;
			void * field_348;
			void * EventListenerVMT2;
			void * LobbyManager;
			ObjectSet<PeerId> ConnectedPeerIds;
			uint64_t field_380[13];
			ObjectSet<PeerId> ActivePeerIds;
			ObjectSet<PeerId> UnknownSet1;
			ObjectSet<PeerId> UnknownSet2;
			ObjectSet<PeerId> ConnectedPeerIds2;
			RefMap<PeerId, void *> UserIdToUserInfo;
			RefMap<NetId, PeerId> CharacterNetIdToUserId;
			int Unknown[2];
			void * VoiceProtocol;
		};
	}
#pragma pack(pop)

}
