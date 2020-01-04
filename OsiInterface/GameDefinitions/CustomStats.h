#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"

namespace osidbg
{

#pragma pack(push, 1)
	namespace eocnet
	{
		struct CustomStatDefinitionSyncInfo
		{
			NetId NetId{ NetIdUnassigned };
			uint8_t _Pad1[4];
			uint64_t Unknown1{ 0 };
			FixedString Id;
			STDWString Name;
			STDWString Description;
		};

		struct CustomStatsDefinitionSyncMessage : public Message
		{
			ObjectSet<CustomStatDefinitionSyncInfo> StatDefns;
		};

		struct CustomStatsDefinitionRemoveMessage : public Message
		{
			void * PrimSetVMT;
			ObjectSet<uint32_t> StatDefns; // NetID list
			uint64_t Unkn{ 0 };
		};

		struct CustomStatsSyncInfo
		{
			NetId NetId{ NetIdUnassigned };
			uint8_t _Pad1[4];
			FixedStringMapBase<int> Stats;
			uint8_t _Pad2[4];
		};

		struct CustomStatsSyncMessage : public Message
		{
			ObjectSet<CustomStatsSyncInfo> Stats;
		};
	}

	namespace eoc
	{
		struct CustomStatsComponent : public BaseComponent
		{
			FixedStringMapBase<int> StatValues;
		};

		struct CustomStatDefinitionComponent : public ProtectedGameObject<CustomStatDefinitionComponent>
		{
			void * VMT;
			FixedString Id;
			STDWString Name;
			STDWString Description;
		};
	}

	namespace esv
	{
		struct NetComponent : public ProtectedGameObject<NetComponent>
		{
			BaseComponent Base;
			FixedString FixedStr1;
			NetId NetID;
			uint8_t _Pad[4];
		};

		struct CustomStatDefinitionComponent : public eoc::CustomStatDefinitionComponent
		{
			BaseComponent Base;
			FixedString someStr;
			NetId NetID;
			uint8_t _Pad[4];
			uint64_t Unkn1;
		};

		struct CustomStatSystem : public ProtectedGameObject<CustomStatSystem>
		{
			void * GameEventListenerVMT;
			void * Unkn1;
			void * NetEventListenerVMT;
			void * BaseComponentVMT;
			EntityWorld * EntityWorld;
			ObjectSet<ComponentHandle> CustomStatDefinitionHandles;
			ObjectSet<ComponentHandle> Handles2;
			ObjectSet<ComponentHandle> Handles3;
			ObjectSet<ComponentHandle> Handles4;
			ObjectSet<ComponentHandle> Handles5;
			PrimitiveSet<short> PeerIDClassnames;
			uint64_t Unkn2;
			uint64_t Unkn3;
		};

		typedef int(*CustomStatsProtocol__ProcessMsg)(void * self, void * unkn, void * unkn2, eocnet::Message * msg);
	}
#pragma pack(pop)
}