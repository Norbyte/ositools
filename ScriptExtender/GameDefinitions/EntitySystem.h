#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GlobalFixedStrings.h>

BEGIN_SE()

struct BaseComponent
{
	void* VMT;
	// FIXME - this is an EntityHandle
	ComponentHandle EntityObjectHandle;
	ComponentHandleWithType Component;
};

template <class TComponent>
struct ComponentFactory : public ProtectedGameObject<ComponentFactory<TComponent>>
{
	static constexpr ObjectType ObjectTypeIndex = TComponent::ObjectTypeIndex;

	/*virtual ComponentHandle * ReevaluateHandle(ComponentHandle & handle) = 0;
	virtual ComponentHandle * GetFreeHandle(ComponentHandle & handle) = 0;
	virtual bool IsFreeIndex(uint32_t index) = 0;
	virtual bool IsReservedIndex(uint32_t index) = 0;
	virtual uint64_t ReserveIndex(uint32_t index) = 0;
	virtual uint64_t UnreserveIndex(uint32_t index) = 0;
	virtual void Destroy() = 0;*/

	void * VMT;
	Array<TComponent*> ComponentsByHandleIndex;
	Array<uint32_t> Salts;
	Set<uint32_t> FreeSlotIndices;
	ObjectSet<TComponent*> Components;
	bool HasReservedIndices;
	uint32_t MaximumSize;

	TComponent* Get(ComponentHandle handle) const
	{
		if (!handle || handle.GetType() != (uint32_t)ObjectTypeIndex) {
			return nullptr;
		}

		auto index = handle.GetIndex();
		if (index >= ComponentsByHandleIndex.Size) {
			return nullptr;
		}

		if (Salts[index] != handle.GetSalt()) {
			return nullptr;
		}

		return ComponentsByHandleIndex[index];
	}
};

struct Component : public ProtectedGameObject<Component>
{
	virtual void Destroy() = 0;
	virtual void DestroyComponent() = 0;
	virtual void CreateComponent() = 0;
	virtual void ForceCreateComponent() = 0; // ForceComponentDefault
	virtual void * FindComponentByHandle(ComponentHandle const * oh) = 0;
	virtual void * TryFindComponentByHandle(ComponentHandle const* oh) = 0;
	virtual void * FindComponentByGuid(FixedString const * fs) = 0;
	virtual bool MoveComponentByGuid(FixedString const* fs, void* component) = 0;
	virtual void * FindComponentByNetId(NetId const & netId, bool checkSalt = true) = 0;
	virtual void UNKN() = 0;
	virtual void * GetComponentByIndex(uint64_t index) = 0;
	virtual void GetFreeHandle() = 0;
	virtual void IsFreeIndex() = 0;
	virtual void IsReservedIndex() = 0;
	virtual void ReserveIndex() = 0;
	virtual void UnreserveIndex() = 0;
};

template <class TComponent>
struct NetworkComponentFactory : public ComponentFactory<TComponent>
{
	Map<FixedString, TComponent*> ObjectMap;
	Map<uint16_t, TComponent*> NetIds;
	Map<uint16_t, TComponent*> FreeNetIdMap;
	Set<uint32_t> FreeNetIds;
	uint32_t NumFreeNetIDs;
	Array<uint16_t> NetIdSalts;
	uint16_t NextFreeNetIndex;
	bool CanCreateNetIDs;

	TComponent* FindByNetId(NetId netId) const
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
		ComponentHandleWithType Handle;
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
	static constexpr auto ObjectTypeIndex = ObjectType::Unknown;

	void * VMT;
	ComponentLayout Layout;
};
	
struct BaseComponentProcessingSystem
{
	void * VMT;
	void * field_8;
};

struct EntityWorldBase : public ProtectedGameObject<EntityWorldBase>
{
	// Handle type index, registered statically during game startup
	enum class HandleTypeIndexTag {};
	using HandleTypeIndex = TypedIntegral<int32_t, HandleTypeIndexTag>;
	// Component type index, registered statically during game startup
	enum class ComponentTypeIndexTag {};
	using ComponentTypeIndex = TypedIntegral<int32_t, ComponentTypeIndexTag>;

	ComponentFactory<EntityEntry> EntityPool;
	Array<ComponentTypeEntry> Components;
	ObjectSet<void *> KeepAlives; // ObjectSet<ObjectHandleRefMap<ComponentKeepAliveDesc>>
	Array<SystemTypeEntry> SystemTypes;
	Array<void *> EventTypes; // Array<EventTypeEntry>
	void * EntityWorldManager;
	PrimitiveSet<SystemTypeEntry> SystemTypes2;
	ObjectSet<void *> Funcs; // ObjectSet<function>
	RefMap<FixedString, int> RefMap; // ???
	bool RegisterPhaseEnded;
	uint64_t NextComponentIndex;
	uint64_t ReservedEntityHandleList;
	uint64_t ReservedComponentHandleList;
	uint64_t Unknown2[2];


	void* GetComponent(uint32_t type, ObjectType handleType, ComponentHandle componentHandle, bool logError = true)
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
		auto factory = reinterpret_cast<ComponentFactory<EntityEntry>*>((std::intptr_t)componentMgr + 8);
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

	void* GetComponent(uint32_t componentType, char const* nameGuid, bool logError = true)
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

	void* GetComponent(uint32_t type, NetId netId, bool logError = true)
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

	void* GetComponentByEntityHandle(uint32_t type, ComponentHandle entityHandle, bool logError = true)
	{
		if (this == nullptr) {
			OsiError("Tried to find component on null EntityWorld!");
			return nullptr;
		}

		if (entityHandle.GetType() != 0) {
			OsiError("Entity handle has invalid type " << entityHandle.GetType());
			return nullptr;
		}

		// FIXME - use EntityPool.Get()
		auto index = entityHandle.GetIndex();
		if (index >= EntityPool.Components.Size) {
			if (logError) {
				OsiError("Entity index " << index << " too large!");
			}
			return nullptr;
		}

		auto salt = entityHandle.GetSalt();
		if (salt != EntityPool.Salts[index]) {
			if (logError) {
				OsiError("Salt mismatch on index " << index << "; " << salt << " != " << EntityPool.Salts[index]);
			}
			return nullptr;
		}

		auto entity = (EntityEntry*)EntityPool.ComponentsByHandleIndex[index];
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

		ComponentHandle componentHandle{ layoutEntry.Handle.Handle };
		auto componentMgr = Components[(uint32_t)type].component;
		return componentMgr->FindComponentByHandle(&componentHandle);
	}

	FixedString ComponentTypeToName(uint32_t type)
	{
		return GFS.strEmpty;
		// FIXME!
		/*auto label = EnumInfo<TComponentType>::Find(type);
		if (label) {
			return label;
		} else {
			return GFS.strEmpty;
		}*/
	}
};


struct IGameObject : public ProtectedGameObject<IGameObject>
{
	virtual ~IGameObject() = 0;
	virtual void HandleTextKeyEvent() = 0;
	virtual uint64_t GetRTTI() = 0;
	virtual void SetObjectHandle(ComponentHandle Handle) = 0;
	virtual void GetObjectHandle(ComponentHandle& Handle) const = 0;
	virtual void SetGuid(FixedString const& fs) = 0;
	virtual FixedString* GetGuid() const = 0;
	virtual void SetNetID(NetId netId) = 0;
	virtual void GetNetID(NetId& netId) const = 0;
	virtual void SetCurrentTemplate(void* esvTemplate) = 0;
	virtual GameObjectTemplate* GetCurrentTemplate() const = 0;
	virtual void SetGlobal(bool isGlobal) = 0;
	virtual bool IsGlobal() const = 0;
	virtual uint32_t GetComponentType() = 0;
	virtual EntityHandle* GetEntityHandle(EntityHandle& handle) = 0;
	virtual STDWString* GetName() = 0;
	virtual void SetFlags(uint64_t flag) = 0;
	virtual void ClearFlags(uint64_t flag) = 0;
	virtual bool HasFlag(uint64_t flag) const = 0;
	virtual void SetAiColliding(bool colliding) = 0;
	virtual void GetTags(ObjectSet<FixedString> & tags) = 0;
	virtual bool IsTagged(FixedString const & tag) = 0;
	virtual glm::vec3 const* GetTranslate() const = 0;
	virtual glm::mat3 const* GetRotation() const = 0;
	virtual float GetScale() const = 0;
	virtual void SetTranslate(glm::vec3 const& translate) = 0;
	virtual void SetRotation(glm::mat3 const& rotate) = 0;
	virtual void SetScale(float scale) = 0;
	virtual glm::vec3 const* GetVelocity() = 0;
	virtual void SetVelocity(glm::vec3 const& translate) = 0;
	virtual void LoadVisual() = 0;
	virtual void UnloadVisual() = 0;
	virtual void ReloadVisual() = 0;
	virtual void GetVisual() = 0;
	virtual void GetPhysics() = 0;
	virtual void SetPhysics() = 0;
	virtual void LoadPhysics() = 0;
	virtual void UnloadPhysics() = 0;
	virtual void ReloadPhysics() = 0;
	virtual float GetHeight() = 0;
	virtual void GetParentUUID() = 0;
	virtual FixedString* GetCurrentLevel() const = 0;
	virtual void SetCurrentLevel(FixedString const& level) = 0;

	ObjectSet<FixedString> LuaGetTags();
	bool LuaIsTagged(FixedString const& tag);

	ComponentHandle LuaGetHandle();
	EntityHandle LuaGetEntityHandle();
	glm::vec3 LuaGetTranslate();
	glm::mat3 LuaGetRotate();
	float LuaGetScale();
	glm::vec3 LuaGetVelocity();
	float LuaGetHeight();

	BaseComponent Base;
	FixedString MyGuid;

	NetId NetID;
};

struct IEoCClientObject : public IGameObject
{
	virtual eoc::Ai * GetAi() = 0;
	virtual void LoadAi() = 0;
	virtual void UnloadAi() = 0;
	virtual bool Unknown0() = 0;
	virtual bool Unknown1() = 0;
	virtual bool Unknown2() = 0;
	virtual float Unknown3() = 0;
	virtual FixedString * Unknown4() = 0;
	virtual bool Unknown5() = 0;
	virtual float GetHeight2() = 0;
	virtual TranslatedString* GetDisplayName(TranslatedString& name) = 0;
	virtual float Unknown6() = 0;
	virtual void SavegameVisit() = 0;
	virtual void SetLight(FixedString *) = 0;
	virtual void * GetLight() = 0;
	virtual void RemoveLight() = 0;
	virtual FixedString * GetPlayerRace(bool returnPolymorph, bool excludeUninitialized) = 0;
	virtual FixedString* GetPlayerOrigin(bool returnPolymorph, bool excludeUninitialized) = 0;

	ecl::StatusMachine* GetStatusMachine() const;
	RefReturn<ecl::Status> LuaGetStatus(FixedString const& statusId);
	RefReturn<ecl::Status> LuaGetStatusByType(StatusType type);
	ObjectSet<FixedString> LuaGetStatusIds();
	UserReturn LuaGetStatuses(lua_State* L);
};

struct IEoCServerObject : public IGameObject
{
	virtual void AddPeer(PeerId const& peer) = 0;
	virtual void RemovePeer(PeerId const& peer) = 0;
	virtual void ClearPeers() = 0;
	virtual bool HasPeer(PeerId const& peer) = 0;
	virtual eoc::Ai* GetAi() = 0;
	virtual void LoadAi() = 0;
	virtual void UnloadAi() = 0;
	virtual TranslatedString* GetDisplayName(TranslatedString& name) = 0;
	virtual bool SavegameVisit(ObjectVisitor& visitor) = 0;
	virtual NetId GetEntityNetworkId(NetId& netId) = 0;
	virtual void SetTemplate(GameObjectTemplate* tmpl) = 0;
	virtual void CacheTemplate(int templateType, Level* level) = 0;

	std::optional<STDWString> LuaGetDisplayName();
	esv::StatusMachine* GetStatusMachine() const;
	RefReturn<esv::Status> LuaGetStatus(FixedString const& statusId);
	RefReturn<esv::Status> LuaGetStatusByType(StatusType type);
	ObjectSet<FixedString> LuaGetStatusIds();
	UserReturn LuaGetStatuses(lua_State* L);
};

template <class TWorld>
struct EntityManager
{
	void* VMT;
	RefMap<FixedString, void*> field_8;
	TWorld* EntityWorld;
	FixedString FS1;
};


namespace esv
{
	struct ItemConversionHelpers : public ProtectedGameObject<ItemConversionHelpers>
	{
		EntityManager* EntityManager;
		ItemFactory* ItemFactory;
		RefMap<FixedString, ObjectSet<Item *> *> RegisteredItems;
		RefMap<FixedString, ObjectSet<Item *> *> ActivatedItems;
		FixedString CurrentLevel;
		Map<FixedString, ComponentHandle> GlobalItemHandles;
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


	struct EntityWorld : public EntityWorldBase
	{
		inline CustomStatDefinitionComponent* GetCustomStatDefinitionComponent(ComponentHandle componentHandle, bool logError = true)
		{
			// FIXME - remove typecasts after conversion is complete
			auto component = GetComponent((uint32_t)ComponentType::CustomStatDefinition, ObjectType::ServerCustomStatDefinitionComponent, 
				componentHandle, logError);
			if (component != nullptr) {
				return (CustomStatDefinitionComponent*)((uint8_t*)component - 80);
			}
			else {
				return nullptr;
			}
		}

		inline Character* GetCharacterComponentByEntityHandle(ComponentHandle entityHandle, bool logError = true)
		{
			auto ptr = GetComponentByEntityHandle((uint32_t)ComponentType::Character, entityHandle, logError);
			if (ptr != nullptr) {
				return (Character*)((uint8_t*)ptr - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Item* GetItemComponentByEntityHandle(ComponentHandle entityHandle, bool logError = true)
		{
			auto ptr = GetComponentByEntityHandle((uint32_t)ComponentType::Item, entityHandle, logError);
			if (ptr != nullptr) {
				return (Item*)((uint8_t*)ptr - 8);
			}
			else {
				return nullptr;
			}
		}

		inline eoc::CombatComponent* GetCombatComponentByEntityHandle(ComponentHandle entityHandle, bool logError = true)
		{
			return (eoc::CombatComponent*)GetComponentByEntityHandle((uint32_t)ComponentType::Combat, entityHandle, logError);
		}

		inline eoc::CustomStatsComponent* GetCustomStatsComponentByEntityHandle(ComponentHandle entityHandle, bool logError = true)
		{
			return (eoc::CustomStatsComponent*)GetComponentByEntityHandle((uint32_t)ComponentType::CustomStats, entityHandle, logError);
		}

		inline NetComponent* GetNetComponentByEntityHandle(ComponentHandle entityHandle, bool logError = true)
		{
			return (NetComponent*)GetComponentByEntityHandle((uint32_t)ComponentType::Net, entityHandle, logError);
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
			auto component = GetComponent((uint32_t)ComponentType::Character, nameGuid, logError);
			if (component != nullptr) {
				return (Character*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Character* GetCharacter(ComponentHandle handle, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Character, ObjectType::ServerCharacter, handle, logError);
			if (component != nullptr) {
				return (Character*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Character* GetCharacter(NetId netId, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Character, netId, logError);
			if (component != nullptr) {
				return (Character*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Item* GetItem(char const* nameGuid, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Item, nameGuid, logError);
			if (component != nullptr) {
				return (Item*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Item* GetItem(ComponentHandle handle, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Item, ObjectType::ServerItem, handle, logError);
			if (component != nullptr) {
				return (Item*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Item* GetItem(NetId netId, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Item, netId, logError);
			if (component != nullptr) {
				return (Item*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Projectile* GetProjectile(ComponentHandle handle, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Item, ObjectType::ServerProjectile, handle, logError);
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
				auto component = GetComponent((uint32_t)triggerTypes[i], nameGuid, false);
				if (component != nullptr) {
					return (Trigger*)((uintptr_t)component - TriggerOffsets[i]);
				}
			}

			return nullptr;
		}

		inline Trigger* GetTrigger(ComponentHandle handle, bool logError = true)
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

			auto component = GetComponent((uint32_t)componentType, type, handle, logError);
			if (component != nullptr) {
				return (Trigger*)((uintptr_t)component - TriggerOffsets[typeIdx]);
			} else {
				return nullptr;
			}
		}

		IEoCServerObject* GetGameObject(char const* nameGuid, bool logError = true);
		IEoCServerObject* GetGameObject(ComponentHandle handle, bool logError = true);
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


	struct EntityWorld : public EntityWorldBase
	{
		inline Character* GetCharacter(char const* nameGuid, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Character, nameGuid, logError);
			if (component != nullptr) {
				return (Character*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Character* GetCharacter(ComponentHandle handle, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Character, ObjectType::ClientCharacter, handle, logError);
			if (component != nullptr) {
				return (Character*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Character* GetCharacter(NetId netId, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Character, netId, logError);
			if (component != nullptr) {
				return (Character*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Item* GetItem(char const* nameGuid, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Item, nameGuid, logError);
			if (component != nullptr) {
				return (Item*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Item* GetItem(ComponentHandle handle, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Item, ObjectType::ClientItem, handle, logError);
			if (component != nullptr) {
				return (Item*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline Item* GetItem(NetId netId, bool logError = true)
		{
			auto component = GetComponent((uint32_t)ComponentType::Item, netId, logError);
			if (component != nullptr) {
				return (Item*)((uint8_t*)component - 8);
			}
			else {
				return nullptr;
			}
		}

		inline eoc::CustomStatsComponent* GetCustomStatsComponentByEntityHandle(ComponentHandle entityHandle, bool logError = true)
		{
			return (eoc::CustomStatsComponent*)GetComponentByEntityHandle((uint32_t)ComponentType::CustomStats, entityHandle, logError);
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
	typedef void (*GameStateMachine__Update)(void* self, GameTime* time);
	typedef void (*GameStateEventManager__ExecuteGameStateChangedEvent)(void* self, GameState fromState, GameState toState);

	EntityWorld* GetEntityWorld();
}

END_SE()
