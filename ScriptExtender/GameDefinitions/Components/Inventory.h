#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Components/Item.h>

namespace dse
{
	namespace esv
	{
		struct Inventory : public ProtectedGameObject<Inventory>
		{
			static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerInventory;

			typedef void (* Equip)(esv::Inventory * self, uint64_t itemHandle, bool consumeAP, 
				int16_t requestedItemSlot, bool updateInventoryViews, bool checkRequirements, 
				bool updateCurrentVitality, bool useWeaponAnimTypeAndSkills);

			void * VMT;
			FixedString GUID; // Unused?
			NetId NetID;
			ObjectSet<uint16_t> PeerIdClassNames;
			ComponentHandle Handle;
			uint8_t EquipmentSlots;
			ComponentHandle ParentHandle;
			int32_t CachedGoldAmount;
			int32_t CachedWeight;
			bool IsGlobal;
			bool UnknownFlag;
			ObjectSet<ComponentHandle> ItemsBySlot;
			RefMap<int32_t, ComponentHandle> * Views;
			ObjectSet<ComponentHandle> UpdateViews;
			uint64_t Unknown2;
			RefMap<ComponentHandle, uint32_t> BuyBackAmounts;
			RefMap<ComponentHandle, uint32_t> TimeItemAddedToInventory;
			ObjectSet<ComponentHandle> PinnedContainers;
		};

		struct InventoryFactory : public NetworkComponentFactory<Inventory>
		{
			// TODO
		};

		struct InventoryView
		{
			static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerInventoryView;

			void* VMT;
			FixedString GUID;
			NetId NetID;
			ComponentHandle Handle;
			ComponentHandle Owner;
			uint32_t ParentType;
			ObjectSet<ComponentHandle> Parents;
			uint32_t ViewId;
			ObjectSet<ComponentHandle> Items;
			Map<ComponentHandle, int32_t> ItemIndices;
			ObjectSet<ComponentHandle> PinnedContainerTags;
			ObjectSet<PeerId> PeerIds;
			ObjectSet<PeerId> PeerIds2;
		};

		struct InventoryViewFactory : public NetworkComponentFactory<InventoryView>
		{
		};

	}

	namespace ecl
	{
		struct Inventory : ProtectedGameObject<Inventory>
		{
			static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientInventory;

			struct BuyBackAmount
			{
				int32_t Amount;
				int32_t Price;
				ComponentHandle Character;
			};

			void* VMT;
			FixedString GUID;
			NetId NetID;
			ObjectSet<uint16_t> PeerIDClassNames;
			ComponentHandle OwnerCharacterHandleUI;
			uint8_t field_40;
			uint8_t field_41;
			uint8_t EquipmentSlots;
			ComponentHandle ParentHandle;
			int field_50;
			int field_54;
			uint8_t Flags;
			uint8_t field_59;
			ObjectSet<ComponentHandle> ItemsBySlot;
			RefMap<int, void*>* Views; // <int, InventoryView*>
			ObjectSet<ComponentHandle> UpdateViews;
			RefMap<int, void*>* OfferedAmounts;
			RefMap<ComponentHandle, BuyBackAmount>* BuyBackAmounts;
			ObjectSet<ComponentHandle> PinnedContainers;
		};

		struct InventoryFactory : public NetworkComponentFactory<Inventory>
		{
		};

		struct InventoryView
		{
			static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientInventoryView;

			void* VMT;
			FixedString GUID_Unused;
			NetId NetID;
			ComponentHandle Handle;
			NetId ParentNetId_M;
			ComponentHandle OH1;
			ObjectSet<NetId> ParentInventories;
			ObjectSet<ComponentHandle> ItemHandles;
			Map<NetId, int> ItemNetIdToIndex;
		};

		struct InventoryViewFactory : public NetworkComponentFactory<InventoryView>
		{
		};

		struct InventoryProtocol : public net::Protocol
		{
			using PostUpdateProc = int (InventoryProtocol* self);

			struct InventoryViewItemUpdate
			{
				NetId InventoryNetId;
				NetId ItemNetId;
				int Index;
			};


			ObjectSet<InventoryViewItemUpdate> ItemUpdates;
			net::Message* LockStateSyncMsg;
		};
	}
}
