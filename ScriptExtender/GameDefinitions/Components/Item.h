#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/Net.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

namespace dse
{

	namespace eoc
	{
		struct ItemDefinition
		{
			uint32_t Version{ 0x36060000 };
			NetId NetID{ NetId::Unassigned };
			NetId ItemNetId{ NetId::Unassigned };
			FixedString UUID;
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7
			FixedString RootTemplate;
			uint32_t RootTemplateType{ 6 };
			// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7"
			FixedString OriginalRootTemplate;
			uint32_t OriginalRootTemplateType{ 6 };
			uint32_t Unkn22[3]{ 0 };
			glm::mat3 WorldRot;
			float Scale_M{ 1.0 };
			NetId InventoryNetID{ NetId::Unassigned };
			NetId InventorySubContainerNetID{ NetId::Unassigned };
			int16_t Slot{ 0 }; // -1 = Not in inventory
			uint32_t Amount{ 1 };
			int32_t GoldValueOverwrite{ -1 }; // -1 = Not overridden
			int32_t WeightValueOverwrite{ -1 }; // -1 = Not overridden
			stats::DamageType DamageTypeOverwrite{ (stats::DamageType)-1 };
			int32_t HP{ -1 };
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
			ObjectSet<FixedString> GenerationBoostSet;
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
			ObjectSet<FixedString> Tags;
			ObjectSet<FixedString> RuneBoostSet;
			ObjectSet<FixedString> DeltaModSet;
			ObjectSet<FixedString> PinnedContainerTags;
			bool IsGlobal{ false };
			bool Active{ false };
			bool HasGeneratedStats{ false };
			bool CanBeUsed{ false };
			bool IsPinnedContainer{ false };
			bool CanBeMoved{ false };
			bool CanBePickedUp{ false };
			bool Invisible{ false };
			uint8_t Flags1[3]{ 0 };
			bool CustomRequirements{ false };
			uint8_t Flags12{ 0 };
			bool Known{ false };
			uint8_t Flags13{ 0 };
#if !defined(OSI_EOCAPP)
			bool UnEquipLocked{ false };
#endif
			bool IsIdentified{ false };
			bool GMFolding{ false };
			bool Floating{ false };
			bool CanUseRemotely{ false };
			uint64_t Unknown2[2]{ 0 };

			void ResetProgression();
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
			static constexpr auto ComponentPoolIndex = EntityComponentIndex::Item;
			static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerItem;

			Status * GetStatus(ComponentHandle handle, bool returnPending, bool returnUnapplied = false) const;
			Status* GetStatus(NetId netId) const;

			glm::vec3 WorldPos; // Saved
			uint32_t _Pad2;
			ItemFlags Flags; // Saved
			float Radius;
			FixedString CurrentLevel; // Saved
			glm::mat3 WorldRot; // Saved
			float Scale; // Saved
			ItemFlags2 Flags2;
			ObjectSet<uint16_t> PeerIDClassNames;
			glm::vec3 WorldVelocity; // Saved
			eoc::Ai* AI;
			ItemTemplate* CurrentTemplate; // Saved
			uint64_t OriginalTemplateType; // Saved
			STDWString CustomDisplayName; // Saved
			STDWString CustomDescription; // Saved
			STDWString CustomBookContent; // Saved
			FixedString StatsId; // Saved
			stats::Item * Stats;
			stats::Object * StatsFromName;
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
			PlanManager* PlanManager; // Saved
			void * VariableManager; // Saved
			StatusMachine * StatusMachine; // Saved
			FixedString VisualResourceID;
			ComponentHandle OwnerHandle; // Saved
			ComponentHandle OriginalOwnerCharacter; // Saved
			void * Sockets; // Saved
			uint64_t U7;
			int32_t ComputedVitality;
			int32_t MaxDurabilityPatchCheck; // Saved
			FixedString Rarity; // Saved
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

			ObjectSet<FixedString> GetInventoryItemGuids();
			ObjectSet<Item*> GetInventoryItems();
			ObjectSet<FixedString> GetNearbyCharacters(float distance);
			CompactObjectSet<FixedString> GetGeneratedBoosts();
			void LuaSetGeneratedBoosts(lua_State* L);
			ObjectSet<FixedString> GetDeltaMods();
			void LuaSetDeltaMods(lua_State* L);

			ItemFlags LuaGetFlags();
			void LuaSetFlags(ItemFlags flags);
			bool LuaHasFlag(ItemFlags flag);
			void LuaSetFlag(ItemFlags flag, bool set);
			UserReturn LuaGetUserVariables(lua_State* L);

			static PropertyOperationResult LuaFallbackGet(lua_State* L, dse::lua::LifetimeHandle const& lifetime, Item* object, FixedString const& prop);
			static PropertyOperationResult LuaFallbackSet(lua_State* L, dse::lua::LifetimeHandle const& lifetime, Item* object, FixedString const& prop, int index);
		};

		typedef void(*ParseItem)(Item * Item, ObjectSet<eoc::ItemDefinition> * ParsedItems, bool CopyNetId, bool CopyContainerContents);
		typedef Item * (*CreateItemFromParsed)(ObjectSet<eoc::ItemDefinition> * ParsedItems, uint32_t Index);

		struct ItemFactory : public NetworkComponentFactory<Item>
		{
			void* VMT2;
			void* VMT3;
			Map<FixedString, void*> FSMap_ReloadComponent;
			EntityWorld* Entities;
			uint64_t Unkn8[2];
		};

		struct ItemMovement
		{
			struct InventoryAddParams
			{
				ComponentHandle OwnerCharacterHandle;
				uint32_t Flags;
				NetId InventoryNetId;
				stats::ItemSlot Slot;
				uint64_t field_18;
				uint64_t field_20;
				uint64_t field_28;
			};

			ComponentHandle ItemHandle;
			ComponentHandle MoverHandle;
			bool Moving;
			void* MovementData;
			bool MovingToInventory;
			bool MovingInWorld;
			bool HeightForced;
			eoc::PathMover PathMover;
			glm::vec3 AiBounds;
			bool WakePhysics;
			bool DoHitTest;
			InventoryAddParams InventoryAdd;
			STDString MoveEventName;
		};

		struct ItemMover
		{
			Map<ComponentHandle, ItemMovement*> Movements;
		};

		struct ItemManager : public BaseComponentProcessingSystem<EntityWorld>
		{
			struct TransformParams
			{
				ComponentHandle ItemHandle;
				TemplateHandle TemplateHandle;
				ItemTransformFlags Flags;
			};

			void* VMT2;
			ItemFactory* Factory;
			ItemMover* Mover;
			ObjectSet<Item*> Items;
			ObjectSet<Item*> ActiveItems;
			ObjectSet<TransformParams> PendingTransforms;
			ObjectSet<TransformParams> NetPendingTransforms;
			ObjectSet<Item*> ItemsUpdatedLastTick;
		};

		struct CombineManager : public ProtectedGameObject<CombineManager>
		{
			using ExecuteCombinationProc = bool (esv::CombineManager* self, CraftingStationType craftingStation, ObjectSet<ComponentHandle>* ingredientHandles, esv::Character* character, uint8_t quantity, char openUI, FixedString* combinationId);

			void* VMT;
			uint64_t field_8;
			ObjectSet<stats::ObjectInstance*> IngredientStats;
			ObjectSet<Item*> Ingredients;
			ObjectSet<ComponentHandle> ObjectHandles;
			ObjectSet<Item*> Items2;
		};

		using ItemHelpers__GenerateTreasureItem = Item* (stats::TreasureObjectInfo* treasureInfo, int level);
	}

	namespace ecl
	{
		struct Item : public IEoCClientReplicatedObject
		{
			static constexpr auto ComponentPoolIndex = EntityComponentIndex::Item;
			static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientItem;

			using SetupDescriptionToFlashProc = void (Item* self, ig::FlashObject* flash, char const* path, 
				uint32_t displayContext, Character* owner, void* iconDrawStruct, int amount);

			glm::vec3 WorldPos;
			uint32_t _Pad2;
			ItemFlags Flags;
			ItemPhysicsFlags PhysicsFlags;
			float GravityTimer;
			float UnknownTimer;
			float FallTimer;
			glm::vec3 WakePosition;
			float AIBoundSize;
			FixedString CurrentLevel;
			glm::mat3 WorldRot;
			float Scale;
			glm::vec3 Velocity;
			PhysicsObject* Physics;
			eoc::Ai* AI;
			ItemTemplate* CurrentTemplate;
			stats::Item* Stats;
			FixedString StatsId;
			stats::Object* StatsFromName;
			ComponentHandle InventoryHandle;
			ComponentHandle InventoryParentHandle;
			int16_t CurrentSlot;
			int Amount;
			int Vitality;
			void* ItemMachine;
			StatusMachine* StatusMachine;
			ComponentHandle InUseByCharacterHandle;
			int InUseByUserId;
			FixedString KeyName;
			int32_t LockLevel;
			int32_t field_B4;
			ComponentHandle OwnerCharacterHandle;
			STDWString* CachedItemDescription;
			STDWString* WString2;
			ComponentHandle OH4;
			void * Sockets;
			__int64 field_E0;
			ObjectSet<FixedString> Tags;
			int field_108;
			bool CachedDescriptionFlags;
			ItemFlags2 Flags2;
			int Level;
			FixedString ItemType;
			int GoldValueOverride;
			int BaseWeightOverwrite;
			void* FlashTooltipArrayPath;
			FixedString ItemColorOverride;
			TranslatedString CustomDisplayName;
			TranslatedString CustomDescription;
			TranslatedString CustomBookContent;
			int GatewayTeleportationsRemaining;
			uint16_t UpdateRefCount;
#if defined(OSI_EOCAPP)
			FixedString Icon;
#endif

			ObjectSet<FixedString> GetInventoryItemGuids();
			std::optional<FixedString> GetOwnerCharacter();
			ObjectSet<FixedString> GetDeltaMods();

			ItemFlags LuaGetFlags();
			void LuaSetFlags(ItemFlags flags);
			bool LuaHasFlag(ItemFlags flag);
			void LuaSetFlag(ItemFlags flag, bool set);
			UserReturn LuaGetUserVariables(lua_State* L);

			static PropertyOperationResult LuaFallbackGet(lua_State* L, dse::lua::LifetimeHandle const& lifetime, Item* object, FixedString const& prop);
			static PropertyOperationResult LuaFallbackSet(lua_State* L, dse::lua::LifetimeHandle const& lifetime, Item* object, FixedString const& prop, int index);
		};

		struct ItemFactory : public NetworkComponentFactory<Item>
		{
			void* VMT2;
			void* VMT3;
			EntityWorld* EntityWorld;
		};
	}
}
