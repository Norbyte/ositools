#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Stats.h"
#include "Status.h"

namespace osidbg
{

#pragma pack(push, 1)
	namespace eoc
	{
		struct ItemDefinition : public ProtectedGameObject<ItemDefinition>
		{
			uint32_t Unknown;
			NetId NetID;
			NetId ItemNetId;
			uint8_t _Pad0[4];
			FixedString FS1;
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7
			FixedString RootTemplate;
			uint32_t Unkn1;
			uint8_t _Pad1[4];
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7"
			FixedString OriginalRootTemplate;
			uint32_t Unkn2[4];
			uint32_t Unkn3;
			float Unkn3Flt;
			uint32_t Unkn4[2];
			float Unkn4Flt;
			uint32_t Unkn41[4];
			uint32_t Amount;
			NetId InventoryNetID;
			NetId InventorySubContainerNetID;
			int16_t Slot; // -1 = Not in inventory
			uint8_t _Pad3[2];
			uint32_t Unkn6;
			int32_t GoldValueOverwrite; // -1 = Not overridden
			int32_t WeightValueOverwrite; // -1 = Not overridden
			uint32_t DamageTypeOverwrite;
			uint32_t SomeOverwrite;
			FixedString FS4;
			// eg. "Uncommon"
			FixedString ItemType;
			STDWString CustomDisplayName;
			STDWString CustomDescription;
			STDWString CustomBookContent;
			STDWString GenerationBase;
			// eg. "WPN_Shield"
			FixedString GenerationStatsId;
			// eg. "Uncommon"
			FixedString GenerationItemType;
			uint32_t GenerationRandom;
			uint16_t GenerationLevel;
			uint8_t _Pad4[2];
			ObjectSet<FixedString> GenerationBoosts;
			uint8_t LevelGroupIndex;
			uint16_t RootGroupIndex;
			uint8_t NameIndex;
			uint8_t NameCool;
			uint8_t _Pad5[3];
			uint32_t Unkn;
			uint8_t _Pad6[4];
			FixedString StatsName;
			uint32_t Level;
			uint32_t Unkn11;
			// eg. "WPN_Shield"
			FixedString StatsEntryName;
			uint32_t EquipmentStatsType;
			uint8_t _Pad7[4];
			ScratchBuffer PermanentBoostsBuf;
			uint8_t _Pad8[4];
			ScratchBuffer BaseStatsBuf;
			uint8_t _Pad9[4];
			bool HasModifiedSkills;
			uint8_t _Pad10[7];
			FixedString Skills;
			ObjectSet<FixedString> FSSet2;
			ObjectSet<FixedString> RuneBoostNames;
			uint8_t Flags0[2];
			uint8_t HasItemGeneration_M;
			uint8_t Flags1[12];
			uint8_t IsIdentified;
			uint8_t Flags2[3];
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

		struct Item : public EoCServerObject
		{
			Status * GetStatusByHandle(ObjectHandle handle, bool returnPending) const;

			glm::mat3 WorldRot; // Saved
			float Scale; // Saved
			uint8_t Flags3;
			uint8_t _Pad4[7];
			PrimitiveSet<uint16_t> PeerIDClassNames;
			uint64_t U3[2];
			glm::vec3 WorldVelocity; // Saved
			uint32_t _Pad5;
			uint64_t U4;
			FixedString CurrentTemplate; // Saved
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
			uint64_t U5[2];
			uint32_t LockLevel; // Saved
			uint32_t _Pad7;
			void * ItemMachine; // Saved
			void * PlanMachine; // Saved
			void * VariableManager; // Saved
			StatusMachine * StatusMachine; // Saved
			void * U6;
			ObjectHandle OwnerHandle; // Saved
			ObjectHandle OriginalOwnerCharacter; // Saved
			void * Sockets; // Saved
			uint64_t U7;
			int32_t ComputedVitality;
			int32_t MaxDurabilityPatchCheck; // Saved
			FixedString ItemType; // Saved
			int32_t GoldValueOverwrite; // Saved
			int32_t WeightValueOverwrite; // Saved
			uint64_t U8;
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

	}
#pragma pack(pop)
}
