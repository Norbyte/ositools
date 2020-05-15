#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Stats.h"
#include "Status.h"

namespace dse
{

#pragma pack(push, 1)
	namespace eoc
	{
		struct ItemDefinition : public Noncopyable<ItemDefinition>
		{
			uint32_t Version{ 0x36060000 };
			NetId NetID{ -1 };
			NetId ItemNetId{ -1 };
			uint8_t _Pad0[4];
			FixedString FS1;
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7
			FixedString RootTemplate;
			uint32_t Unkn1{ 6 };
			uint8_t _Pad1[4];
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7"
			FixedString OriginalRootTemplate;
			uint32_t Unkn21{ 6 };
			uint32_t Unkn22[3]{ 0 };
			glm::mat3x3 WorldRot;
			float Scale_M{ 1.0 };
			NetId InventoryNetID{ -1 };
			NetId InventorySubContainerNetID{ -1 };
			int16_t Slot{ 0 }; // -1 = Not in inventory
			uint8_t _Pad3[2]{ 0 };
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
			uint8_t _Pad4[2];
			ObjectSet<FixedString> GenerationBoosts;
			int8_t LevelGroupIndex{ -1 };
			int16_t RootGroupIndex{ -1 };
			int8_t NameIndex{ -1 };
			uint8_t NameCool{ 0 };
			uint8_t _Pad5[3];
			uint32_t StatsLevel{ 0 };
			uint8_t _Pad6[4];
			FixedString Key;
			uint32_t LockLevel{ 1 };
			uint32_t Unkn11{ 0 };
			// eg. "WPN_Shield"
			FixedString StatsEntryName;
			uint32_t EquipmentStatsType{ 3 };
			uint8_t _Pad7[4];
			ScratchBuffer PermanentBoostsBuf;
			uint8_t _Pad8[4];
			ScratchBuffer BaseStatsBuf;
			uint8_t _Pad9[4];
			bool HasModifiedSkills{ false };
			uint8_t _Pad10[7];
			FixedString Skills;
			ObjectSet<FixedString> FSSet2;
			ObjectSet<FixedString> RuneBoosts;
			ObjectSet<FixedString> DeltaMods;
			ObjectSet<FixedString> FSSet5;
			uint8_t Flags0[2]{ 0 };
			bool HasGeneratedStats{ false };
			uint8_t Flags1[13]{ 0 };
			bool IsIdentified{ false };
			bool GMFolding{ false };
			uint8_t Flags2{ 0 };
			bool CanUseRemotely{ false };
			uint8_t _Pad11[4]{ 0 };
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
			uint16_t _Pad0;
			CompactObjectSet<FixedString> Boosts; // Saved
		};

		struct Item : public IEoCServerObject
		{
			Status * GetStatus(ObjectHandle handle, bool returnPending) const;
			Status* GetStatus(NetId netId) const;

			glm::vec3 WorldPos; // Saved
			uint32_t _Pad2;
			uint64_t Flags; // Saved
			uint32_t U2;
			uint32_t _Pad3;
			FixedString CurrentLevel; // Saved
			glm::mat3 WorldRot; // Saved
			float Scale; // Saved
			uint8_t Flags3;
			uint8_t _Pad4[7];
			PrimitiveSet<uint16_t> PeerIDClassNames;
			uint64_t U3[2];
			glm::vec3 WorldVelocity; // Saved
			uint32_t _Pad5;
			eoc::Ai* AI;
			void* CurrentTemplate; // Saved
			uint64_t OriginalTemplateType; // Saved
			STDWString CustomDisplayName; // Saved
			STDWString CustomDescription; // Saved
			STDWString CustomBookContent; // Saved
			FixedString StatsId; // Saved
			CDivinityStats_Item * StatsDynamic;
			CRPGStats_Object * StatsFromName;
			ItemGeneration * Generation; // Saved
			ObjectHandle InventoryHandle; // Saved
			ObjectHandle ParentInventoryHandle; // Saved
			uint16_t Slot; // Saved
			uint16_t _Pad6;
			uint32_t Amount; // Saved
			uint32_t Vitality; // Saved
			uint32_t Armor; // Saved
			ObjectHandle InUseByCharacterHandle;
			uint32_t UserId;
			uint32_t U50;
			FixedString Key;
			uint32_t LockLevel; // Saved
			uint32_t _Pad7;
			void * ItemMachine; // Saved
			void * PlanMachine; // Saved
			void * VariableManager; // Saved
			StatusMachine * StatusMachine; // Saved
			FixedString VisualResourceID;
			ObjectHandle OwnerHandle; // Saved
			ObjectHandle OriginalOwnerCharacter; // Saved
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
			uint8_t _Pad8[3];
			int32_t TeleportUseCount; // Saved
			FixedString PreviousLevel; // Saved
		};

		typedef void(*ParseItem)(Item * Item, ObjectSet<eoc::ItemDefinition> * ParsedItems, bool CopyNetId, bool CopyContainerContents);
		typedef Item * (*CreateItemFromParsed)(ObjectSet<eoc::ItemDefinition> * ParsedItems, uint32_t Index);


		struct Inventory
		{
			typedef void (* Equip)(esv::Inventory * self, uint64_t itemHandle, bool consumeAP, 
				int16_t requestedItemSlot, bool updateInventoryViews, bool checkRequirements, 
				bool updateCurrentVitality, bool useWeaponAnimTypeAndSkills);

			void * VMT;
			FixedString MyGuid;
			NetId NetID;
			uint8_t _Pad1[4];
			PrimitiveSet<uint16_t> PeerIdClassNames;
			uint64_t _Pad2[2];
			ObjectHandle MyHandle;
			uint8_t EquipmentSlots;
			uint8_t _Pad3[7];
			ObjectHandle ParentHandle;
			uint32_t _Pad4;
			uint32_t WeightValueComputed;
			bool IsGlobal;
			bool UnknownFlag;
			uint8_t _Pad5[6];
			ObjectSet<ObjectHandle> ItemsBySlot;
			void * Views;
			ObjectSet<ObjectHandle> UpdateViews;
			uint64_t Unknown2;
			RefMap<FixedString, uint32_t> BuyBackAmounts;
			RefMap<FixedString, uint32_t> TimeItemAddedToInventory;
		};
	}

	namespace ecl
	{
		struct Item : public IEocClientObject
		{
			glm::vec3 WorldPos;
			uint32_t _Pad2;
			uint64_t Flags;
			uint8_t Flags2;
			uint8_t _Pad3[7];
			__int64 field_58;
			__int64 field_60;
			__int64 field_68;
			FixedString CurrentLevel;
			glm::mat3 WorldRot;
			float Scale;
			glm::vec3 Velocity;
			uint8_t gap34[4];
			void* PhysicsObject;
			void* AiObject;
			void* CurrentTemplate;
			CDivinityStats_Item* Stats;
			FixedString StatsId;
			CRPGStats_Object* StatsFromName;
			ObjectHandle InventoryHandle;
			ObjectHandle InventoryParentHandle;
			int field_78;
			int Weight;
			int field_80;
			uint8_t gap84[4];
			void* ItemMachine;
			StatusMachine* StatusMachine;
			ObjectHandle InUseByCharacterHandle;
			int InUseByUserId;
			uint8_t gapA4[4];
			FixedString KeyName;
			__int64 field_B0;
			ObjectHandle OH5;
			STDWString* CachedItemDescription;
			STDWString* WString2;
			ObjectHandle OH4;
			void * Sockets;
			__int64 field_E0;
			ObjectSet<FixedString> Tags;
			int field_108;
			bool CachedDescriptionFlags;
			char Flags3;
			uint8_t gap10E[2];
			int Level;
			uint8_t gap114[4];
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

	}
#pragma pack(pop)
}
