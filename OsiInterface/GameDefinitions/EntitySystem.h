#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "Net.h"
#include "Module.h"
#include "Stats.h"
#include <GlobalFixedStrings.h>

namespace dse
{

	template <class T, uint32_t TypeIndex>
	struct NetworkObjectFactory : public ObjectFactory<T, TypeIndex>
	{
		Map<FixedString, T *> ObjectMap;
		Map<uint16_t, T *> NetIds;
		Map<uint16_t, void *> FreeNetIdMap;
		Set<uint32_t> Unknown4;
		uint32_t NumFreeNetIDs;
		Array<uint16_t> NetIdSalts;
		uint16_t NextFreeNetIndex;
		bool CanCreateNetIDs;

		T* FindByNetId(NetId netId) const
		{
			if (!netId) return nullptr;

			auto index = netId.GetIndex();
			if (index >= NetIdSalts.Size
				|| netId.GetSalt() != NetIdSalts[index]) {
				return nullptr;
			}

			auto object = NetIds.Find(index);
			if (object != nullptr) {
				return *object;
			} else {
				return nullptr;
			}
		}
	};

	struct ComponentTypeEntry
	{
		// todo
		Component * component;
		uint64_t dummy[31];
	};

	struct ComponentLayout
	{
		struct LayoutEntry
		{
			uint64_t unkn1;
			ComponentHandle Handle;
		};

		Array<LayoutEntry> Entries;
	};

	struct SystemTypeEntry
	{
		void * System;
		int64_t Unkn1;
		uint32_t Unkn2;
		PrimitiveSet<uint64_t> PSet;
	};

	struct EntityEntry
	{
		void * VMT;
		ComponentLayout Layout;
	};

	namespace eoc
	{
		struct CombatComponent;
		struct CustomStatsComponent;
	}
	
	struct BaseComponentProcessingSystem
	{
		void * VMT;
		void * field_8;
	};

	template <class TComponentType>
	struct EntityWorldBase : public ProtectedGameObject<EntityWorldBase<TComponentType>>
	{
		void * VMT;
		Array<EntityEntry *> EntityEntries;
		Array<uint32_t> EntitySalts;
		uint64_t Unkn1[3];
		PrimitiveSet<EntityEntry *> EntityEntries2;
		uint8_t Unkn3;
		uint32_t Unkn4;
		Array<ComponentTypeEntry> Components;
		ObjectSet<void *> KeepAlives; // ObjectSet<ObjectHandleRefMap<ComponentKeepAliveDesc>>
		Array<SystemTypeEntry> SystemTypes;
		Array<void *> EventTypes; // Array<EventTypeEntry>
		void * EntityWorldManager;
		PrimitiveSet<SystemTypeEntry> SystemTypes2;
		ObjectSet<void *> Funcs; // ObjectSet<function>
		RefMap<FixedString, int> RefMap; // ???


		void* GetComponent(TComponentType type, ObjectType handleType, ObjectHandle componentHandle, bool logError = true)
		{
			if (this == nullptr) {
				OsiError("Tried to find component on null EntityWorld!");
				return nullptr;
			}

			if (!componentHandle) {
				return nullptr;
			}

			if ((uint32_t)type >= Components.Size) {
				if (logError) {
					OsiError("Component type index " << (uint32_t)type << " too large!");
				}
				return nullptr;
			}

			auto componentMgr = Components[(uint32_t)type].component;
			if (componentMgr == nullptr) {
				if (logError) {
					OsiError("Component type " << (uint32_t)type << " not bound!");
				}
				return nullptr;
			}

			if (componentHandle.GetType() != (uint32_t)handleType) {
				if (logError) {
					OsiError("Type mismatch! Factory supports " << (unsigned)handleType << ", got " << (unsigned)componentHandle.GetType());
				}
				return nullptr;
			}

			// FIXME - This is somewhat ugly :(
			auto factory = reinterpret_cast<ObjectFactory<void *, 0>*>((std::intptr_t)componentMgr + 8);
			auto index = componentHandle.GetIndex();
			auto salt = componentHandle.GetSalt();
			if (index >= factory->Salts.Size) {
				if (logError) {
					OsiError("Factory for type " << (unsigned)handleType << " only has " << factory->Salts.Size
						<< " objects, requested " << (unsigned)index);
				}
				return nullptr;
			}

			if (salt != factory->Salts[index]) {
				if (logError) {
					OsiError("Salt mismatch for type " << (unsigned)handleType << ", object " << index << ": got "
						<< salt << ", real is " << factory->Salts[index]);
				}
				return nullptr;
			}

			return componentMgr->FindComponentByHandle(&componentHandle);
		}

		void* GetComponent(TComponentType componentType, char const* nameGuid, bool logError = true)
		{
			if (this == nullptr) {
				OsiError("Tried to find component on null EntityWorld!");
				return nullptr;
			}

			if (nameGuid == nullptr) {
				OsiError("Attempted to look up component with null name!");
				return nullptr;
			}

			auto fs = NameGuidToFixedString(nameGuid);
			if (!fs) {
				OsiError("Could not map GUID '" << nameGuid << "' to FixedString");
				return nullptr;
			}

			auto component = Components[(uint32_t)componentType].component->FindComponentByGuid(&fs);
			if (component != nullptr) {
				return component;
			} else {
				if (logError) {
					OsiError("No " << ComponentTypeToName(componentType).Str << " component found with GUID '" << nameGuid << "'");
				}
				return nullptr;
			}
		}

		void* GetComponent(TComponentType type, NetId netId, bool logError = true)
		{
			if (this == nullptr) {
				OsiError("Tried to find component on null EntityWorld!");
				return nullptr;
			}

			if (!netId) {
				return nullptr;
			}

			if ((uint32_t)type >= Components.Size) {
				OsiError("Component type index " << (uint32_t)type << " too large!");
				return nullptr;
			}

			auto componentMgr = Components[(uint32_t)type].component;
			if (componentMgr == nullptr) {
				OsiError("Component type " << (uint32_t)type << " not bound!");
				return nullptr;
			}

			return componentMgr->FindComponentByNetId(netId, true);
		}

		void* GetComponentByEntityHandle(TComponentType type, ObjectHandle entityHandle, bool logError = true)
		{
			if (this == nullptr) {
				OsiError("Tried to find component on null EntityWorld!");
				return nullptr;
			}

			if (entityHandle.GetType() != 0) {
				OsiError("Entity handle has invalid type " << entityHandle.GetType());
				return nullptr;
			}

			auto index = entityHandle.GetIndex();
			if (index >= EntityEntries.Size) {
				if (logError) {
					OsiError("Entity index " << index << " too large!");
				}
				return nullptr;
			}

			auto salt = entityHandle.GetSalt();
			if (salt != EntitySalts[index]) {
				if (logError) {
					OsiError("Salt mismatch on index " << index << "; " << salt << " != " << EntitySalts[index]);
				}
				return nullptr;
			}

			auto entity = EntityEntries[index];
			if ((uint32_t)type >= entity->Layout.Entries.Size) {
				if (logError) {
					OsiError("Entity " << index << " has no component slot for " << (uint32_t)type);
				}
				return nullptr;
			}

			auto const& layoutEntry = entity->Layout.Entries[(uint32_t)type];
			if (!layoutEntry.Handle.IsValid()) {
				if (logError) {
					OsiError("Entity " << index << " has no component bound to slot " << (uint32_t)type);
				}
				return nullptr;
			}

			ObjectHandle componentHandle{ layoutEntry.Handle.Handle };
			auto componentMgr = Components[(uint32_t)type].component;
			return componentMgr->FindComponentByHandle(&componentHandle);
		}

		FixedString ComponentTypeToName(TComponentType type)
		{
			auto label = EnumInfo<TComponentType>::Find(type);
			if (label) {
				return label;
			} else {
				return GFS.strEmpty;
			}
		}
	};

	namespace eoc
	{
		struct Ai;
	}

	struct IGameObjectBase : public ProtectedGameObject<IGameObjectBase>
	{
		virtual ~IGameObjectBase() = 0;
		virtual void HandleTextKeyEvent() = 0;
		virtual uint64_t Ret5() = 0;
		virtual void SetObjectHandle(ObjectHandle Handle) = 0;
		virtual void GetObjectHandle(ObjectHandle& Handle) const = 0;
		virtual void SetGuid(FixedString const& fs) = 0;
		virtual FixedString* GetGuid() const = 0;
		virtual void SetNetID(NetId netId) = 0;
		virtual void GetNetID(NetId& netId) const = 0;
		virtual void SetCurrentTemplate(void* esvTemplate) = 0;
		virtual void* GetCurrentTemplate() const = 0;
		virtual void SetGlobal(bool isGlobal) = 0;
		virtual bool IsGlobal() const = 0;
		virtual uint32_t GetComponentType() = 0;
		virtual void* GetEntityObjectByHandle(ObjectHandle handle) = 0;
		virtual STDWString* GetName() = 0;
		virtual void SetFlags(uint64_t flag) = 0;
		virtual void ClearFlags(uint64_t flag) = 0;
		virtual bool HasFlag(uint64_t flag) const = 0;
		virtual void SetAiColliding(bool colliding) = 0;
		virtual void GetTags(ObjectSet<FixedString> & tags) = 0;
		virtual bool IsTagged(FixedString & tag) = 0;
		virtual Vector3 const* GetTranslate() const = 0;
		virtual glm::mat3 const* GetRotation() const = 0;
		virtual float GetScale() const = 0;
		virtual void SetTranslate(Vector3 const& translate) = 0;
		virtual void SetRotation(glm::mat3 const& rotate) = 0;
		virtual void SetScale(float scale) = 0;
		virtual Vector3 const* GetVelocity() = 0;
		virtual void SetVelocity(Vector3 const& translate) = 0;
		virtual void LoadVisual() = 0;
		virtual void UnloadVisual() = 0;
		virtual void ReloadVisual() = 0;
		virtual void GetVisual() = 0;
		virtual void GetPhysics() = 0;
		virtual void SetPhysics() = 0;
		virtual void LoadPhysics() = 0;
		virtual void UnloadPhysics() = 0;
		virtual void ReloadPhysics() = 0;
		virtual void GetHeight() = 0;
		virtual void GetParentUUID() = 0;
		virtual FixedString* GetCurrentLevel() const = 0;
		virtual void SetCurrentLevel(FixedString const& level) = 0;
	};

	struct IGameObject : public IGameObjectBase
	{
		BaseComponent Base;
		FixedString MyGuid;

		NetId NetID;
	};

	struct IEocClientObject : public IGameObject
	{
		virtual eoc::Ai * GetAi() = 0;
		virtual void LoadAi() = 0;
		virtual void UnloadAi() = 0;
		virtual void Unknown0() = 0;
		virtual void Unknown1() = 0;
		virtual void Unknown2() = 0;
		virtual void Unknown3() = 0;
		virtual FixedString * Unknown4() = 0;
		virtual bool Unknown5() = 0;
		virtual TranslatedString* GetDisplayName(TranslatedString* name) = 0;
		virtual float Unknown6() = 0;
		virtual void SavegameVisit() = 0;
		virtual void SetLight(FixedString *) = 0;
		virtual void * GetLight() = 0;
		virtual void RemoveLight() = 0;
		virtual FixedString * GetPlayerRace(bool returnPolymorph, bool excludeUninitialized) = 0;
		virtual FixedString* GetPlayerOrigin(bool returnPolymorph, bool excludeUninitialized) = 0;
	};

	struct IEoCServerObject : public IGameObject
	{
		virtual void AddPeer() = 0;
		virtual void UNK1() = 0;
		virtual void UNK2() = 0;
		virtual void UNK3() = 0;
		virtual eoc::Ai* GetAi() = 0;
		virtual void LoadAi() = 0;
		virtual void UnloadAi() = 0;
		virtual TranslatedString* GetDisplayName(TranslatedString* name) = 0;
		virtual void SavegameVisit() = 0;
		virtual void GetEntityNetworkId() = 0;
		virtual void SetTemplate() = 0;
		virtual void SetOriginalTemplate_M() = 0;
	};

	template <class TWorld>
	struct EntityManager
	{
		void* VMT;
		RefMap<FixedString, void*> field_8;
		TWorld* EntityWorld;
		FixedString FS1;
	};

	struct Trigger;

	namespace esv
	{
		struct CustomStatDefinitionComponent;
		struct CustomStatSystem;
		struct NetComponent;
		struct Item;
		struct Character;
		struct Inventory;
		struct TurnManager;
		struct Projectile;
		struct EntityWorld;
		struct EntityManager;
		struct CombineManager;
		struct ItemFactory;
		struct CharacterFactory;

		struct ItemConversionHelpers : public ProtectedGameObject<ItemConversionHelpers>
		{
			EntityManager* EntityManager;
			ItemFactory* ItemFactory;
			RefMap<FixedString, ObjectSet<Item *> *> RegisteredItems;
			RefMap<FixedString, ObjectSet<Item *> *> ActivatedItems;
			FixedString CurrentLevel;
			Map<FixedString, ObjectHandle> GlobalItemHandles;
#if !defined(OSI_EOCAPP)
			RefMap<FixedString, void*> DebugItems;
			__int64 field_88;
#endif
		};

		struct CharacterConversionHelpers : public ProtectedGameObject<CharacterConversionHelpers>
		{
			EntityManager* EntityManager;
			CharacterFactory* CharacterFactory;
			RefMap<FixedString, ObjectSet<Character *> *> RegisteredCharacters;
			RefMap<FixedString, ObjectSet<Character *> *> ActivatedCharacters;
		};

		struct TriggerConversionHelpers : public ProtectedGameObject<TriggerConversionHelpers>
		{
			esv::EntityManager* EntityManager;
			void* TriggerFactory;
			RefMap<FixedString, ObjectSet<Trigger*>*> RegisteredTriggers;
		};

		struct ProjectileConversionHelpers : public ProtectedGameObject<ProjectileConversionHelpers>
		{
			esv::EntityManager* EntityManager;
			void* ProjectileFactory;
			RefMap<FixedString, ObjectSet<Projectile *> *> RegisteredProjectiles;
		};


		struct EntityManager : public dse::EntityManager<EntityWorld>
		{
			ItemConversionHelpers ItemConversionHelpers;
			CharacterConversionHelpers CharacterConversionHelpers;
			TriggerConversionHelpers TriggerConversionHelpers;
			ProjectileConversionHelpers ProjectileConversionHelpers;
			void* CharacterManagerSystem;
			void* ItemManagerSystem;
			void* TriggerManagerSystem;
			void* ProjectileManagerSystem;
			void* TurnManagerSystem;
			void* SpiritObjectManager;
			ObjectSet<FixedString> TriggerTypes;
			bool field_150;
			ObjectSet<void*> ComponentPrepares; // <esv::ComponentPrepare>
			ObjectSet<void*> TemplateTraces; // <esv::TemplateTrace>
		};


		struct EntityWorld : public EntityWorldBase<ComponentType>
		{
			inline CustomStatDefinitionComponent* GetCustomStatDefinitionComponent(ObjectHandle componentHandle, bool logError = true)
			{
				auto component = GetComponent(ComponentType::CustomStatDefinition, ObjectType::ServerCustomStatDefinitionComponent, 
					componentHandle, logError);
				if (component != nullptr) {
					return (CustomStatDefinitionComponent*)((uint8_t*)component - 80);
				}
				else {
					return nullptr;
				}
			}

			inline Character* GetCharacterComponentByEntityHandle(ObjectHandle entityHandle, bool logError = true)
			{
				auto ptr = GetComponentByEntityHandle(ComponentType::Character, entityHandle, logError);
				if (ptr != nullptr) {
					return (Character*)((uint8_t*)ptr - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Item* GetItemComponentByEntityHandle(ObjectHandle entityHandle, bool logError = true)
			{
				auto ptr = GetComponentByEntityHandle(ComponentType::Item, entityHandle, logError);
				if (ptr != nullptr) {
					return (Item*)((uint8_t*)ptr - 8);
				}
				else {
					return nullptr;
				}
			}

			inline eoc::CombatComponent* GetCombatComponentByEntityHandle(ObjectHandle entityHandle, bool logError = true)
			{
				return (eoc::CombatComponent*)GetComponentByEntityHandle(ComponentType::Combat, entityHandle, logError);
			}

			inline eoc::CustomStatsComponent* GetCustomStatsComponentByEntityHandle(ObjectHandle entityHandle, bool logError = true)
			{
				return (eoc::CustomStatsComponent*)GetComponentByEntityHandle(ComponentType::CustomStats, entityHandle, logError);
			}

			inline NetComponent* GetNetComponentByEntityHandle(ObjectHandle entityHandle, bool logError = true)
			{
				return (NetComponent*)GetComponentByEntityHandle(ComponentType::Net, entityHandle, logError);
			}

			inline CustomStatSystem* GetCustomStatSystem()
			{
				auto sys = SystemTypes.Buf[(uint32_t)SystemType::CustomStat].System;
				return (CustomStatSystem*)((uint8_t*)sys - 0x18);
			}

			inline TurnManager* GetTurnManager()
			{
				auto const& system = SystemTypes[(unsigned)SystemType::TurnManager];
				return (TurnManager*)((uint8_t*)system.System - 8);
			}

			inline Character* GetCharacter(char const* nameGuid, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Character, nameGuid, logError);
				if (component != nullptr) {
					return (Character*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Character* GetCharacter(ObjectHandle handle, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Character, ObjectType::ServerCharacter, handle, logError);
				if (component != nullptr) {
					return (Character*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Character* GetCharacter(NetId netId, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Character, netId, logError);
				if (component != nullptr) {
					return (Character*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Item* GetItem(char const* nameGuid, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Item, nameGuid, logError);
				if (component != nullptr) {
					return (Item*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Item* GetItem(ObjectHandle handle, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Item, ObjectType::ServerItem, handle, logError);
				if (component != nullptr) {
					return (Item*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Item* GetItem(NetId netId, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Item, netId, logError);
				if (component != nullptr) {
					return (Item*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Projectile* GetProjectile(ObjectHandle handle, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Item, ObjectType::ServerProjectile, handle, logError);
				if (component != nullptr) {
					return (Projectile*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			static constexpr int32_t TriggerOffsets[] = {
				168, // EoCPointTrigger
				208, // EoCAreaTrigger
				168, // StartTrigger
				168, // TeleportTrigger
				208, // EventTrigger
				208, // CrimeAreaTrigger
				208, // CrimeRegionTrigger
				256, // AtmosphereTrigger
				208, // AIHintAreaTrigger
				208, // MusicVolumeTrigger
				208, // SecretRegionTrigger
				208, // StatsAreaTrigger
				296, // SoundVolumeTrigger
				208, // RegionTrigger
				208 // ExplorationTrigger
			};


			inline Trigger* GetTrigger(char const* nameGuid, bool logError = true)
			{
				static constexpr ComponentType triggerTypes[] = {
					ComponentType::EoCPointTrigger,
					ComponentType::EoCAreaTrigger,
					ComponentType::StartTrigger,
					ComponentType::TeleportTrigger,
					ComponentType::EventTrigger,
					ComponentType::CrimeAreaTrigger,
					ComponentType::CrimeRegionTrigger,
					ComponentType::AtmosphereTrigger,
					ComponentType::AIHintAreaTrigger,
					ComponentType::MusicVolumeTrigger,
					ComponentType::SecretRegionTrigger,
					ComponentType::StatsAreaTrigger,
					ComponentType::SoundVolumeTrigger,
					ComponentType::RegionTrigger,
					ComponentType::ExplorationTrigger
				};

				for (auto i = 0; i < std::size(triggerTypes); i++) {
					auto component = GetComponent(triggerTypes[i], nameGuid, false);
					if (component != nullptr) {
						return (Trigger*)((uintptr_t)component - TriggerOffsets[i]);
					}
				}

				return nullptr;
			}

			inline Trigger* GetTrigger(ObjectHandle handle, bool logError = true)
			{
				auto type = (ObjectType)handle.GetType();
				if (type != ObjectType::ServerEocPointTrigger
					&& type != ObjectType::ServerEocAreaTrigger
					&& type != ObjectType::ServerStartTrigger
					&& type != ObjectType::ServerTeleportTrigger
					&& type != ObjectType::ServerEventTrigger
					&& type != ObjectType::ServerCrimeAreaTrigger
					&& type != ObjectType::ServerCrimeRegionTrigger
					&& type != ObjectType::ServerAtmosphereTrigger
					&& type != ObjectType::ServerAIHintAreaTrigger
					&& type != ObjectType::ServerMusicVolumeTrigger
					&& type != ObjectType::ServerSecretRegionTrigger
					&& type != ObjectType::ServerStatsAreaTrigger
					&& type != ObjectType::ServerSoundVolumeTrigger
					&& type != ObjectType::ServerRegionTrigger
					&& type != ObjectType::ServerExplorationTrigger) {
					return nullptr;
				}

				auto typeIdx = (unsigned)type - (unsigned)ObjectType::ServerEocPointTrigger;
				auto componentType = (ComponentType)((unsigned)ComponentType::EoCPointTrigger + typeIdx);

				auto component = GetComponent(componentType, type, handle, logError);
				if (component != nullptr) {
					return (Trigger*)((uintptr_t)component - TriggerOffsets[typeIdx]);
				} else {
					return nullptr;
				}
			}

			IEoCServerObject* GetGameObject(char const* nameGuid, bool logError = true);
			IEoCServerObject* GetGameObject(ObjectHandle handle, bool logError = true);
		};

		struct CharacterFactory : public NetworkObjectFactory<esv::Character, (uint32_t)ObjectType::ServerCharacter>
		{
			void* VMT2;
			void* VMT3;
			Map<FixedString, void*> FSMap_ReloadComponent;
			EntityWorld* Entities;
			uint64_t Unkn8[2];
		};

		struct ItemFactory : public NetworkObjectFactory<esv::Item, (uint32_t)ObjectType::ServerItem>
		{
			void* VMT2;
			void* VMT3;
			Map<FixedString, void*> FSMap_ReloadComponent;
			EntityWorld* Entities;
			uint64_t Unkn8[2];
		};

		struct InventoryFactory : public NetworkObjectFactory<esv::Inventory, (uint32_t)ObjectType::ServerInventory>
		{
			// TODO
		};

		struct GameStateMachine : public ProtectedGameObject<GameStateMachine>
		{
			uint8_t Unknown;
			void * CurrentState;
			GameState State;
			void ** TargetStates;
			uint32_t TargetStateBufSize;
			uint32_t NumTargetStates;
			uint32_t ReadStateIdx;
			uint32_t WriteStateIdx;
		};

		struct EoCServer
		{
			bool Unknown1;
			uint64_t EoC;
			uint64_t GameTime_M;
			ScratchBuffer ScratchBuffer1;
			ScratchBuffer ScratchBuffer2;
			FixedString FS1;
			FixedString FS2;
			FixedString FS3;
			FixedString FSGUID4;
			GameStateMachine * StateMachine;
			net::GameServer * GameServer;
			void * field_88;
			void * GlobalRandom;
			void * ItemCombinationManager;
			CombineManager * CombineManager;
			ModManager * ModManagerServer;
			bool ShutDown;
			void * EntityWorldManager;
			EntityWorld * EntityWorld;
			EntityManager * EntityManager;
			void * ArenaManager;
			void * GameMasterLobbyManager;
			void * LobbyManagerOrigins;
			bool field_E8;
		};

		EntityWorld* GetEntityWorld();
	}

	namespace ecl
	{
		struct Item;
		struct Character;
		struct Inventory;
		struct EntityManager;

		struct ItemConversionHelpers
		{
			EntityManager* EntityManager;
			void* ItemFactory;
			RefMap<FixedString, ObjectSet<Item*>*> RegisteredItemsByLevel;
			RefMap<FixedString, ObjectSet<Item*>*> ActivatedItemsByLevel;
		};


		struct CharacterConversionHelpers
		{
			EntityManager* EntityManager;
			void* CharacterFactory;
			RefMap<FixedString, ObjectSet<Character*>*> RegisteredCharactersByLevel;
			RefMap<FixedString, ObjectSet<Character*>*> ActivatedCharactersByLevel;
		};


		struct TriggerConversionHelpers
		{
			void* VMT;
			ecl::EntityManager* EntityManager;
			void* TriggerFactory;
			RefMap<FixedString, ObjectSet<void*>*> RegisteredTriggersByLevel;
		};

		
		struct ProjectileConversionHelpers
		{
			EntityManager* EntityManager;
			void* ProjectileFactory;
			RefMap<FixedString, ObjectSet<void*>*> RegisteredProjectilesByLevel;
		};


		struct EntityWorld : public EntityWorldBase<ComponentType>
		{
			inline Character* GetCharacter(char const* nameGuid, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Character, nameGuid, logError);
				if (component != nullptr) {
					return (Character*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Character* GetCharacter(ObjectHandle handle, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Character, ObjectType::ClientCharacter, handle, logError);
				if (component != nullptr) {
					return (Character*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Character* GetCharacter(NetId netId, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Character, netId, logError);
				if (component != nullptr) {
					return (Character*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Item* GetItem(char const* nameGuid, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Item, nameGuid, logError);
				if (component != nullptr) {
					return (Item*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Item* GetItem(ObjectHandle handle, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Item, ObjectType::ClientItem, handle, logError);
				if (component != nullptr) {
					return (Item*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline Item* GetItem(NetId netId, bool logError = true)
			{
				auto component = GetComponent(ComponentType::Item, netId, logError);
				if (component != nullptr) {
					return (Item*)((uint8_t*)component - 8);
				}
				else {
					return nullptr;
				}
			}

			inline eoc::CustomStatsComponent* GetCustomStatsComponentByEntityHandle(ObjectHandle entityHandle, bool logError = true)
			{
				return (eoc::CustomStatsComponent*)GetComponentByEntityHandle(ComponentType::CustomStats, entityHandle, logError);
			}
		};


		struct EntityManager : public dse::EntityManager<EntityWorld>
		{
			void* NetEventManagerVMT;
			ItemConversionHelpers ItemConversionHelpers;
			CharacterConversionHelpers CharacterConversionHelpers;
			TriggerConversionHelpers TriggerConversionHelpers;
			uint64_t Unknown[3];
			ProjectileConversionHelpers ProjectileConversionHelpers;
			void* WallManager;
			ObjectSet<FixedString> TriggerTypes;
			ObjectSet<FixedString> OS_FS2;
		};


		struct InventoryFactory : public NetworkObjectFactory<ecl::Inventory, (uint32_t)ObjectType::ClientInventory>
		{
		};

		struct ActivationManager
		{
			struct ActivationGroup
			{
				ObjectSet<Item*> Items;
				ObjectSet<Character*> Characters;
			};

			void* VMT;
			__int64 field_8;
			void* VMT2;
			__int64 field_18;
			RefMap<FixedString, ActivationGroup> ChangedGroups;
			float ActivationRange1;
			float DeactivationRange1;
			float ActivationRange2;
			float DeactivationRange2;
		};


		struct GameStateMachine : public ProtectedGameObject<GameStateMachine>
		{
			uint8_t Unknown;
			void* CurrentState;
			GameState State;
		};

		struct EoCClient : public ProtectedGameObject<EoCClient>
		{
			void* VMT;
			void* GameEventManagerVMT;
			uint64_t field_10;
			void* NetEventManagerVMT;
			uint64_t field_20;
			void* VMT2;
			void* VMT3;
			void* EoC;
			GameStateMachine** GameStateMachine;
			net::Client* GameClient;
			uint64_t field_50;
			void* LobbyLogicManager;
			void* ArenaManager;
			FixedString FS1;
			FixedString LevelName;
			FixedString SomeGUID;
			FixedString FS_CurrentSaveGameGUID;
			bool IsLoading;
			bool IsLoading2;
			PrimitiveSet<int> PrimitiveSetUnkn;
			uint16_t field_B0;
			void* Random;
			void* ItemCombinationManager;
			char field_C8;
			uint64_t ScratchStr[4];
			ScratchBuffer ScratchBuf;
			ModManager* ModManager;
			void* ChatManager;
			STDWString WStr_CurrentHost_M;
			uint64_t SomeObject[16];
			int field_1C0;
			uint64_t field_1C8[2];
			void* EntityWorldManager;
			EntityWorld* EntityWorld;
			EntityManager* EntityManager;
		};

		typedef void (*EoCClient__HandleError)(void* self, STDWString const* message, bool exitGame, STDWString const* a4);
		typedef void (*GameStateThreaded__GameStateWorker__DoWork)(void* self);
		typedef void (*GameStateEventManager__ExecuteGameStateChangedEvent)(void* self, GameState fromState, GameState toState);

		EntityWorld* GetEntityWorld();
	}
}