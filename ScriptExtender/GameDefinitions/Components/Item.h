#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Status.h>

namespace dse
{

	namespace eoc
	{
		struct ItemDefinition : public Noncopyable<ItemDefinition>
		{
			uint32_t Version{ 0x36060000 };
			NetId NetID{ -1 };
			NetId ItemNetId{ -1 };
			FixedString FS1;
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7
			FixedString RootTemplate;
			uint32_t Unkn1{ 6 };
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7"
			FixedString OriginalRootTemplate;
			uint32_t Unkn21{ 6 };
			uint32_t Unkn22[3]{ 0 };
			glm::mat3x3 WorldRot;
			float Scale_M{ 1.0 };
			NetId InventoryNetID{ -1 };
			NetId InventorySubContainerNetID{ -1 };
			int16_t Slot{ 0 }; // -1 = Not in inventory
			uint32_t Amount{ 1 };
			int32_t GoldValueOverwrite{ -1 }; // -1 = Not overridden
			int32_t WeightValueOverwrite{ -1 }; // -1 = Not overridden
			DamageType DamageTypeOverwrite{ (DamageType )-1 };
			int32_t SomeOverwrite{ -1 };
			FixedString FS4;
			// eg. "Uncommon"
			FixedString ItemType;
			STDWString UnknownStr;
			STDWString CustomDisplayName;
			STDWString CustomDescription;
			STDWString CustomBookContent;
			// eg. "WPN_Shield"
			FixedString GenerationStatsId;
			// eg. "Uncommon"
			FixedString GenerationItemType;
			uint32_t GenerationRandom{ 0 };
			uint16_t GenerationLevel{ 1 };
			ObjectSet<FixedString> GenerationBoosts;
			int8_t LevelGroupIndex{ -1 };
			int16_t RootGroupIndex{ -1 };
			int8_t NameIndex{ -1 };
			uint8_t NameCool{ 0 };
			uint32_t StatsLevel{ 0 };
			FixedString Key;
			uint32_t LockLevel{ 1 };
			uint32_t Unkn11{ 0 };
			// eg. "WPN_Shield"
			FixedString StatsEntryName;
			uint32_t EquipmentStatsType{ 3 };
			ScratchBuffer PermanentBoostsBuf;
			ScratchBuffer BaseStatsBuf;
			bool HasModifiedSkills{ false };
			FixedString Skills;
			ObjectSet<FixedString> FSSet2;
			ObjectSet<FixedString> RuneBoosts;
			ObjectSet<FixedString> DeltaMods;
			ObjectSet<FixedString> FSSet5;
			uint8_t Flags0[2]{ 0 };
			bool HasGeneratedStats{ false };
			uint8_t Flags1[12]{ 0 };
#if !defined(OSI_EOCAPP)
			uint8_t Flags11{ 0 };
#endif
			bool IsIdentified{ false };
			bool GMFolding{ false };
			uint8_t Flags2{ 0 };
			bool CanUseRemotely{ false };
			uint64_t Unknown2[2]{ 0 };
		};
	}

	namespace esv
	{

		struct ItemGeneration
		{
			FixedString Base; // Saved
			FixedString ItemType; // Saved
			uint32_t Random; // Saved
			uint16_t Level; // Saved
			CompactObjectSet<FixedString> Boosts; // Saved
		};

		struct Item : public IEoCServerObject
		{
			Status * GetStatus(ComponentHandle handle, bool returnPending, bool returnUnapplied = false) const;
			Status* GetStatus(NetId netId) const;

			glm::vec3 WorldPos; // Saved
			uint32_t _Pad2;
			ItemFlags Flags; // Saved
			uint32_t U2;
			FixedString CurrentLevel; // Saved
			glm::mat3 WorldRot; // Saved
			float Scale; // Saved
			ItemFlags2 Flags2;
			PrimitiveSet<uint16_t> PeerIDClassNames;
			glm::vec3 WorldVelocity; // Saved
			eoc::Ai* AI;
			ItemTemplate* CurrentTemplate; // Saved
			uint64_t OriginalTemplateType; // Saved
			STDWString CustomDisplayName; // Saved
			STDWString CustomDescription; // Saved
			STDWString CustomBookContent; // Saved
			FixedString StatsId; // Saved
			CDivinityStats_Item * Stats;
			CRPGStats_Object * StatsFromName;
			ItemGeneration * Generation; // Saved
			ComponentHandle InventoryHandle; // Saved
			ComponentHandle ParentInventoryHandle; // Saved
			uint16_t Slot; // Saved
			uint32_t Amount; // Saved
			uint32_t Vitality; // Saved
			uint32_t Armor; // Saved
			ComponentHandle InUseByCharacterHandle;
			uint32_t UserId;
			uint32_t U50;
			FixedString Key;
			uint32_t LockLevel; // Saved
			void * ItemMachine; // Saved
			void * PlanMachine; // Saved
			void * VariableManager; // Saved
			StatusMachine * StatusMachine; // Saved
			FixedString VisualResourceID;
			ComponentHandle OwnerHandle; // Saved
			ComponentHandle OriginalOwnerCharacter; // Saved
			void * Sockets; // Saved
			uint64_t U7;
			int32_t ComputedVitality;
			int32_t MaxDurabilityPatchCheck; // Saved
			FixedString ItemType; // Saved
			int32_t GoldValueOverwrite; // Saved
			int32_t WeightValueOverwrite; // Saved
			void * Portal;
			ObjectSet<FixedString> Tags; // Saved
			uint64_t TeleportTargetOverride; // Saved
			int32_t TreasureLevel; // Saved // -1 = Not overridden
			int32_t LevelOverride; // Saved
			bool ForceSynch; // Saved
			int32_t TeleportUseCount; // Saved
			FixedString PreviousLevel; // Saved
		};

		typedef void(*ParseItem)(Item * Item, ObjectSet<eoc::ItemDefinition> * ParsedItems, bool CopyNetId, bool CopyContainerContents);
		typedef Item * (*CreateItemFromParsed)(ObjectSet<eoc::ItemDefinition> * ParsedItems, uint32_t Index);


		struct Inventory : public ProtectedGameObject<Inventory>
		{
			typedef void (* Equip)(esv::Inventory * self, uint64_t itemHandle, bool consumeAP, 
				int16_t requestedItemSlot, bool updateInventoryViews, bool checkRequirements, 
				bool updateCurrentVitality, bool useWeaponAnimTypeAndSkills);

			void * VMT;
			FixedString MyGuid;
			NetId NetID;
			PrimitiveSet<uint16_t> PeerIdClassNames;
			ComponentHandle MyHandle;
			uint8_t EquipmentSlots;
			ComponentHandle ParentHandle;
			uint32_t _Pad4;
			uint32_t WeightValueComputed;
			bool IsGlobal;
			bool UnknownFlag;
			ObjectSet<ComponentHandle> ItemsBySlot;
			void * Views;
			ObjectSet<ComponentHandle> UpdateViews;
			uint64_t Unknown2;
			RefMap<FixedString, uint32_t> BuyBackAmounts;
			RefMap<FixedString, uint32_t> TimeItemAddedToInventory;
		};

		struct CombineManager : public ProtectedGameObject<CombineManager>
		{
			using ExecuteCombinationProc = bool (esv::CombineManager* self, CraftingStationType craftingStation, ObjectSet<ComponentHandle>* ingredientHandles, esv::Character* character, uint8_t quantity, char openUI, FixedString* combinationId);

			void* VMT;
			uint64_t field_8;
			ObjectSet<CRPGStats_ObjectInstance*> IngredientStats;
			ObjectSet<Item*> Ingredients;
			ObjectSet<ComponentHandle> ObjectHandles;
			ObjectSet<Item*> Items2;
		};

		using ItemHelpers__GenerateTreasureItem = Item* (RPGStats_Treasure_Object_Info* treasureInfo, int level);
	}

	namespace ecl
	{
		struct Item : public IEocClientObject
		{
			glm::vec3 WorldPos;
			uint32_t _Pad2;
			uint64_t Flags;
			uint8_t Flags2;
			__int64 field_58;
			__int64 field_60;
			__int64 field_68;
			FixedString CurrentLevel;
			glm::mat3 WorldRot;
			float Scale;
			glm::vec3 Velocity;
			void* PhysicsObject;
			void* AiObject;
			ItemTemplate* CurrentTemplate;
			CDivinityStats_Item* Stats;
			FixedString StatsId;
			CRPGStats_Object* StatsFromName;
			ComponentHandle InventoryHandle;
			ComponentHandle InventoryParentHandle;
			int16_t CurrentSlot;
			int Weight;
			int field_80;
			void* ItemMachine;
			StatusMachine* StatusMachine;
			ComponentHandle InUseByCharacterHandle;
			int InUseByUserId;
			FixedString KeyName;
			__int64 field_B0;
			ComponentHandle OH5;
			STDWString* CachedItemDescription;
			STDWString* WString2;
			ComponentHandle OH4;
			void * Sockets;
			__int64 field_E0;
			ObjectSet<FixedString> Tags;
			int field_108;
			bool CachedDescriptionFlags;
			char Flags3;
			int Level;
			FixedString ItemType;
			int GoldValueOverride;
			int BaseWeightOverwrite;
			__int64 field_128;
			FixedString ItemColorOverride;
			TranslatedString CustomDisplayName;
			TranslatedString CustomDescription;
			TranslatedString CustomBookContent;
			int GatewayTeleportationsRemaining;
			uint16_t UpdateRefCount;
		};


		struct Inventory : ProtectedGameObject<Inventory>
		{
			void* VMT;
			FixedString GUID;
			NetId NetID;
			PrimitiveSet<uint16_t> PeerIDClassNames;
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
			RefMap<ComponentHandle, void*>* BuyBackAmounts;
			ObjectSet<ComponentHandle> HandleSet3;
		};
	}
}
