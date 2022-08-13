#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GlobalFixedStrings.h>

BEGIN_SE()

struct BaseComponent
{
	void* VMT;
	EntityHandle Entity;
	ComponentHandleWithType Component;

	template <class T>
	T* ToComponent()
	{
		if (this == nullptr) return nullptr;

		if constexpr (std::is_base_of_v<BaseComponent, T>) {
			return static_cast<T*>(this);
		} else {
			return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(this) - offsetof(T, Base));
		}
	}

	template <class T>
	T const* ToComponent() const
	{
		if (this == nullptr) return nullptr;

		if constexpr (std::is_base_of_v<BaseComponent, T>) {
			return static_cast<T const*>(this);
		} else {
			return reinterpret_cast<T const*>(reinterpret_cast<std::uintptr_t>(this) - offsetof(T, Base));
		}
	}
};

struct IObjectFactory : public Noncopyable<IObjectFactory>
{
	virtual ComponentHandle * ReevaluateHandle(ComponentHandle & handle) = 0;
	virtual ComponentHandle GetFreeHandle() = 0;
	virtual bool IsFreeIndex(uint32_t index) = 0;
	virtual bool IsReservedIndex(uint32_t index) = 0;
	virtual void Reserve(uint32_t index) = 0;
	virtual void Unreserve(uint32_t index) = 0;
	virtual ~IObjectFactory();
	virtual void Destroy() = 0;
	virtual void OnDestroyObject(void* object) = 0;
};

struct ObjectFactoryNullLocker : public IObjectFactory
{};

struct ObjectFactoryRWLocker : public IObjectFactory
{
	SRWLOCK Lock;
};

template <class TComponent, class TLocker = ObjectFactoryNullLocker>
struct ComponentFactory : public TLocker
{
	static constexpr ObjectHandleType ObjectTypeIndex = TComponent::ObjectTypeIndex;
	static constexpr uintptr_t ReservedFlag = 0x4000000000000000ull;

	Array<TComponent*> ComponentsByHandleIndex;
	Array<uint32_t> Salts;
	Queue<uint32_t> FreeHandleQueue;
	ObjectSet<TComponent*> Components;
	bool NeedsFreeHandleQueueRebuild{false};
	uint32_t MaximumSize{ 0 };

	ComponentFactory(uint32_t size, uint32_t maximumSize)
	{
		assert(size <= maximumSize);
		MaximumSize = maximumSize;
		Grow(size);
	}

	~ComponentFactory()
	{}

	TComponent* Create()
	{
		auto handle = GetFreeHandle();
		auto component = GameAlloc<TComponent>();
		component->SetHandle(handle);
		Components.push_back(component);
		ComponentsByHandleIndex[handle.GetIndex()] = component;
		return component;
	}

	bool Destroy(ComponentHandle handle)
	{
		auto component = Get(handle);
		if (!component) {
			OsiError("Couldn't find object to destroy: " << std::hex << handle.Handle);
			return false;
		}

		Components.remove(component);
		ReleaseHandle(handle);
		return true;
	}

	TComponent* Get(ComponentHandle handle) const
	{
		if (!handle || handle.GetType() != (uint32_t)ObjectTypeIndex) {
			return nullptr;
		}

		auto index = handle.GetIndex();
		if (index >= ComponentsByHandleIndex.size()) {
			return nullptr;
		}

		if (Salts[index] != handle.GetSalt()) {
			return nullptr;
		}

		return ComponentsByHandleIndex[index];
	}

	ComponentHandle* ReevaluateHandle(ComponentHandle& handle) override
	{
		if (!Get(handle)) {
			handle = ComponentHandle{};
		}

		return &handle;
	}

	ComponentHandle GetFreeHandle() override
	{
		if (NeedsFreeHandleQueueRebuild) {
			RebuildFreeHandleQueue();
		}

		if (FreeHandleQueue.empty()) {
			Grow();
		}

		auto index = FreeHandleQueue.pop();
		auto salt = Salts[index];
		return ComponentHandle((uint32_t)ObjectTypeIndex, index, salt);
	}

	bool IsFreeIndex(uint32_t index) override
	{
		return index < ComponentsByHandleIndex.size()
			&& ComponentsByHandleIndex[index] == nullptr;
	}

	bool IsReservedIndex(uint32_t index) override
	{
		return index < ComponentsByHandleIndex.size()
			&& ComponentsByHandleIndex[index] == (TComponent*)ReservedFlag;
	}

	void Reserve(uint32_t index) override
	{
		assert(index < ComponentsByHandleIndex.size());
		assert(ComponentsByHandleIndex[index] == nullptr);
		ComponentsByHandleIndex[index] = (TComponent*)ReservedFlag;
		NeedsFreeHandleQueueRebuild = true;
	}

	void Unreserve(uint32_t index) override
	{
		assert(index < ComponentsByHandleIndex.size());
		assert(ComponentsByHandleIndex[index] == (TComponent*)ReservedFlag);
		ComponentsByHandleIndex[index] = nullptr;
		NeedsFreeHandleQueueRebuild = true;
	}

	void Destroy() override
	{
		throw std::runtime_error("Unsupported!");
	}

	void OnDestroyObject(void* object) override
	{
	}

private:
	void ReleaseHandle(ComponentHandle handle)
	{
		if (NeedsFreeHandleQueueRebuild) {
			RebuildFreeHandleQueue();
		}

		auto index = handle.GetIndex();
		Salts[index] = (Salts[index] + 1) % 0x400000;
		FreeHandleQueue.push(index);
		ComponentsByHandleIndex[index] = nullptr;
	}

	void Grow()
	{
		Grow(ComponentsByHandleIndex.size() + ComponentsByHandleIndex.grow_size());
	}

	void Grow(uint32_t newSize)
	{
		auto curSize = ComponentsByHandleIndex.size();
		ComponentsByHandleIndex.resize(newSize);
		Salts.resize(newSize);

		for (auto i = curSize; i < newSize; i++) {
			ComponentsByHandleIndex[i] = nullptr;
			Salts[i] = 1;
			FreeHandleQueue.push(i);
		}
	}

	void RebuildFreeHandleQueue()
	{
		FreeHandleQueue.clear();
		for (uint32_t i = 0; i < ComponentsByHandleIndex.size(); i++) {
			if (!ComponentsByHandleIndex[i]) {
				FreeHandleQueue.push(i);
			}
		}

		NeedsFreeHandleQueueRebuild = false;
	}
};

struct IComponentPool : public ProtectedGameObject<IComponentPool>
{
	virtual void Destroy() = 0;
	virtual void DestroyComponent() = 0;
	virtual void CreateComponent() = 0;
	virtual void ForceCreateComponent() = 0; // ForceComponentDefault
	virtual BaseComponent* FindComponentByHandle(ComponentHandle const & oh) = 0;
	virtual BaseComponent* TryFindComponentByHandle(ComponentHandle const & oh) = 0;
	virtual BaseComponent* FindComponentByGuid(FixedString const & fs) = 0;
	virtual bool MoveComponentByGuid(FixedString const & fs, void* component) = 0;
	virtual BaseComponent* FindComponentByNetId(NetId const & netId, bool checkSalt = true) = 0;
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
	Map<FixedString, TComponent*> Guids;
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
		if (index >= NetIdSalts.size()
			|| netId.GetSalt() != NetIdSalts[index]) {
			return nullptr;
		}

		return NetIds.try_get(index);
	}

	TComponent* FindByGuid(FixedString const& guid) const
	{
		if (!guid) return nullptr;
		return Guids.try_get(guid);
	}
};

struct ComponentTypeEntry
{
	IComponentPool* Pool;
	int64_t TypeId;
	ObjectSet<uint64_t> DependencyComponentIds;
	ObjectSet<uint64_t> RegisteredSystemIds;
	ObjectSet<void*> Listeners;
	bool field_70;
	uint64_t field_78[17];
};

struct ComponentLayout
{
	struct LayoutEntry
	{
		uint8_t Flags;
		ComponentHandleWithType Handle;
	};

	Array<LayoutEntry> Entries;
};

struct SystemTypeEntry
{
	void * System;
	int64_t Unkn1;
	uint32_t Unkn2;
	ObjectSet<uint64_t> PSet;
};

struct EntityEntry
{
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;

	ComponentHandle Handle;
	ComponentLayout Layout;
};
	
template <class TEntityWorld>
struct BaseComponentProcessingSystem
{
	void * VMT;
	TEntityWorld* EntityWorld;
};

struct EntityWorldData : public ProtectedGameObject<EntityWorldData>
{
	ComponentFactory<EntityEntry> EntityPool;
	Array<ComponentTypeEntry> Components;
	ObjectSet<void*> KeepAlives; // ObjectSet<ObjectHandleRefMap<ComponentKeepAliveDesc>>
	Array<SystemTypeEntry> SystemTypes;
	Array<void*> EventTypes; // Array<EventTypeEntry>
	void* EntityWorldManager;
	ObjectSet<SystemTypeEntry> SystemTypes2;
	ObjectSet<void*> Funcs; // ObjectSet<function>
	RefMap<FixedString, int> RefMap; // ???
	bool RegisterPhaseEnded;
	uint64_t NextComponentIndex;
	uint64_t ReservedEntityHandleList;
	uint64_t ReservedComponentHandleList;
	uint64_t Unknown2[2];


	EntityEntry* GetEntity(EntityHandle entityHandle, bool logError = true)
	{
		if (this == nullptr) {
			OsiError("Tried to find entity on null EntityWorld!");
			return nullptr;
		}

		auto entity = EntityPool.Get(ComponentHandle{ entityHandle.Handle });
		if (entity != nullptr) {
			return entity;
		} else {
			if (logError) {
				OsiError("Couldn't find entity with handle " << entityHandle.Handle);
			}
			return nullptr;
		}
	}
};

template <class TEntityComponentIndex>
struct EntityWorldBase : public EntityWorldData
{
	// Handle type index, registered statically during game startup
	enum class HandleTypeIndexTag {};
	using HandleTypeIndex = TypedIntegral<int32_t, HandleTypeIndexTag>;
	// Component type index, registered statically during game startup
	enum class ComponentTypeIndexTag {};
	using ComponentTypeIndex = TypedIntegral<int32_t, ComponentTypeIndexTag>;
	using EntityComponentIndex = TEntityComponentIndex;

	BaseComponent* GetBaseComponent(TEntityComponentIndex componentPoolIdx, ObjectHandleType handleType, ComponentHandle componentHandle, bool logError = true)
	{
		if (this == nullptr) {
			OsiError("Tried to find component on null EntityWorld!");
			return nullptr;
		}

		if (!componentHandle) {
			return nullptr;
		}

		if (componentHandle.GetType() != (uint32_t)handleType) {
			if (logError) {
				OsiError("Type mismatch! Factory supports " << (unsigned)handleType << ", got " << (unsigned)componentHandle.GetType());
			}
			return nullptr;
		}

		// FIXME - This is somewhat ugly :(
		auto componentMgr = Components[(uint32_t)componentPoolIdx].Pool;
		auto factory = reinterpret_cast<ComponentFactory<EntityEntry>*>((std::intptr_t)componentMgr + 8);
		auto index = componentHandle.GetIndex();
		auto salt = componentHandle.GetSalt();
		if (index >= factory->Salts.size()) {
			if (logError) {
				OsiError("Factory for type " << (unsigned)handleType << " only has " << factory->Salts.size()
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

		return componentMgr->FindComponentByHandle(componentHandle);
	}

	BaseComponent* GetBaseComponent(TEntityComponentIndex componentPoolIdx, char const* nameGuid, bool logError = true)
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
		auto component = Components[(uint32_t)componentPoolIdx].Pool->FindComponentByGuid(fs);
		if (component != nullptr) {
			return component;
		} else {
			if (logError) {
				OsiError("No component found with GUID '" << nameGuid << "'");
			}
			return nullptr;
		}
	}

	BaseComponent* GetBaseComponent(TEntityComponentIndex componentPoolIdx, NetId netId, bool logError = true)
	{
		if (this == nullptr) {
			OsiError("Tried to find component on null EntityWorld!");
			return nullptr;
		}

		if (!netId) {
			return nullptr;
		}

		auto component = Components[(uint32_t)componentPoolIdx].Pool->FindComponentByNetId(netId, true);
		if (component != nullptr) {
			return component;
		} else {
			if (logError) {
				OsiError("No component found with NetID " << netId.Id);
			}
			return nullptr;
		}
	}

	BaseComponent* GetBaseComponent(TEntityComponentIndex componentPoolIdx, EntityHandle entityHandle, bool logError = true)
	{
		auto entity = GetEntity(entityHandle, logError);
		if (!entity) return nullptr;

		auto index = entityHandle.GetIndex();
		if ((uint32_t)componentPoolIdx >= entity->Layout.Entries.size()) {
			if (logError) {
				OsiError("Entity " << index << " has no component slot for " << (uint32_t)componentPoolIdx);
			}
			return nullptr;
		}

		auto const& layoutEntry = entity->Layout.Entries[(uint32_t)componentPoolIdx];
		if (!layoutEntry.Handle.IsValid()) {
			if (logError) {
				OsiError("Entity " << index << " has no component bound to slot " << (uint32_t)componentPoolIdx);
			}
			return nullptr;
		}

		ComponentHandle componentHandle{ layoutEntry.Handle.Handle };
		auto componentMgr = Components[(uint32_t)componentPoolIdx].Pool;
		return componentMgr->FindComponentByHandle(componentHandle);
	}

	template <class T>
	T* GetComponent(ComponentHandle componentHandle, bool logError = true)
	{
		return GetBaseComponent(T::ComponentPoolIndex, T::ObjectTypeIndex, componentHandle, logError)->ToComponent<T>();
	}

	template <class T>
	T* GetComponent(char const* nameGuid, bool logError = true)
	{
		return GetBaseComponent(T::ComponentPoolIndex, nameGuid, logError)->ToComponent<T>();
	}

	template <class T>
	T* GetComponent(NetId netId, bool logError = true)
	{
		return GetBaseComponent(T::ComponentPoolIndex, netId, logError)->ToComponent<T>();
	}

	template <class T>
	T* GetComponent(EntityHandle entityHandle, bool logError = true)
	{
		return GetBaseComponent(T::ComponentPoolIndex, entityHandle, logError)->ToComponent<T>();
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
	virtual Visual* GetVisual() = 0;
	virtual PhysicsObject* GetPhysics() = 0;
	virtual void SetPhysics(PhysicsObject* physics) = 0;
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
	void LuaSetTranslate(glm::vec3 const& translate);
	glm::mat3 LuaGetRotate();
	void LuaSetRotate(glm::mat3 const& rotate);
	float LuaGetScale();
	void LuaSetScale(float);
	glm::vec3 LuaGetVelocity();
	float LuaGetHeight();
	Visual* LuaGetVisual();
};

struct IEoCClientObject : public IGameObject
{
	virtual eoc::Ai * GetAi() = 0;
	virtual void LoadAi() = 0;
	virtual void UnloadAi() = 0;
	virtual bool IsFadeable() = 0;
	virtual bool GetFadeIn() = 0;
	virtual bool Unknown2() = 0;
	virtual float GetOpacity() = 0;
	virtual FixedString * GetFadeGroup() = 0;
	virtual bool IsSeeThrough() = 0;
	virtual float GetHeight2() = 0;
	virtual TranslatedString* GetDisplayName(TranslatedString& name) = 0;
	virtual float GetPathRadius() = 0;
	virtual void SavegameVisit() = 0;
	virtual void SetLight(FixedString *) = 0;
	virtual void * GetLight() = 0;
	virtual void RemoveLight() = 0;
	virtual FixedString * GetPlayerRace(bool returnPolymorph, bool excludeUninitialized) = 0;
	virtual FixedString* GetPlayerOrigin(bool returnPolymorph, bool excludeUninitialized) = 0;

	std::optional<STDWString> LuaGetDisplayName();
	ecl::StatusMachine* GetStatusMachine() const;
	RefReturn<ecl::Status> LuaGetStatus(FixedString const& statusId);
	RefReturn<ecl::Status> LuaGetStatusByType(StatusType type);
	RefReturn<ecl::Status> LuaGetStatusByHandle(ComponentHandle const& handle);
	ObjectSet<FixedString> LuaGetStatusIds();
	UserReturn LuaGetStatuses(lua_State* L);

	BaseComponent Base;
};

struct IEoCClientReplicatedObject : public IEoCClientObject
{
	FixedString MyGuid;
	NetId NetID;
};

struct IEoCServerObject : public IGameObject
{
	virtual void AddVisibleToPeer(PeerId const& peer) = 0;
	virtual void RemoveVisibleToPeer(PeerId const& peer) = 0;
	virtual void ClearVisiblePeers() = 0;
	virtual bool IsVisibleToPeer(PeerId const& peer) = 0;
	virtual eoc::Ai* GetAi() = 0;
	virtual void LoadAi() = 0;
	virtual void UnloadAi() = 0;
	virtual TranslatedString* GetDisplayName(TranslatedString& name) = 0;
	virtual bool SavegameVisit(ObjectVisitor& visitor) = 0;
	virtual NetId GetEntityNetworkId(NetId& netId) = 0;
	virtual void TransformTemplate(GameObjectTemplate* tmpl) = 0;
	virtual void CacheTemplate(TemplateType templateType, LevelTemplate* levelTemplate) = 0;

	std::optional<STDWString> LuaGetDisplayName();
	esv::StatusMachine* GetStatusMachine() const;
	RefReturn<esv::Status> LuaGetStatus(FixedString const& statusId);
	RefReturn<esv::Status> LuaGetStatusByType(StatusType type);
	RefReturn<esv::Status> LuaGetStatusByHandle(ComponentHandle const& handle);
	ObjectSet<FixedString> LuaGetStatusIds();
	UserReturn LuaGetStatuses(lua_State* L);
	GameObjectTemplate* CreateCacheTemplate();
	void LuaTransformTemplate(ProxyParam<GameObjectTemplate> tmpl);
	void ForceSyncToPeers();

	BaseComponent Base;
	FixedString MyGuid;
	NetId NetID;
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


	struct EntityWorld : public EntityWorldBase<EntityComponentIndex>
	{
		inline CustomStatSystem* GetCustomStatSystem()
		{
			auto sys = SystemTypes[(uint32_t)SystemType::CustomStat].System;
			return (CustomStatSystem*)((uint8_t*)sys - 0x18);
		}

		inline TurnManager* GetTurnManager()
		{
			auto const& system = SystemTypes[(unsigned)SystemType::TurnManager];
			return (TurnManager*)((uint8_t*)system.System - 8);
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
			static constexpr EntityComponentIndex triggerTypes[] = {
				EntityComponentIndex::EoCPointTrigger,
				EntityComponentIndex::EoCAreaTrigger,
				EntityComponentIndex::StartTrigger,
				EntityComponentIndex::TeleportTrigger,
				EntityComponentIndex::EventTrigger,
				EntityComponentIndex::CrimeAreaTrigger,
				EntityComponentIndex::CrimeRegionTrigger,
				EntityComponentIndex::AtmosphereTrigger,
				EntityComponentIndex::AIHintAreaTrigger,
				EntityComponentIndex::MusicVolumeTrigger,
				EntityComponentIndex::SecretRegionTrigger,
				EntityComponentIndex::StatsAreaTrigger,
				EntityComponentIndex::SoundVolumeTrigger,
				EntityComponentIndex::RegionTrigger,
				EntityComponentIndex::ExplorationTrigger
			};

			for (auto i = 0; i < std::size(triggerTypes); i++) {
				auto component = GetBaseComponent(triggerTypes[i], nameGuid, false);
				if (component != nullptr) {
					return (Trigger*)((uintptr_t)component - TriggerOffsets[i]);
				}
			}

			return nullptr;
		}

		inline Trigger* GetTrigger(ComponentHandle handle, bool logError = true)
		{
			auto type = (ObjectHandleType)handle.GetType();
			if (type != ObjectHandleType::ServerEocPointTrigger
				&& type != ObjectHandleType::ServerEocAreaTrigger
				&& type != ObjectHandleType::ServerStartTrigger
				&& type != ObjectHandleType::ServerTeleportTrigger
				&& type != ObjectHandleType::ServerEventTrigger
				&& type != ObjectHandleType::ServerCrimeAreaTrigger
				&& type != ObjectHandleType::ServerCrimeRegionTrigger
				&& type != ObjectHandleType::ServerAtmosphereTrigger
				&& type != ObjectHandleType::ServerAIHintAreaTrigger
				&& type != ObjectHandleType::ServerMusicVolumeTrigger
				&& type != ObjectHandleType::ServerSecretRegionTrigger
				&& type != ObjectHandleType::ServerStatsAreaTrigger
				&& type != ObjectHandleType::ServerSoundVolumeTrigger
				&& type != ObjectHandleType::ServerRegionTrigger
				&& type != ObjectHandleType::ServerExplorationTrigger) {
				return nullptr;
			}

			auto typeIdx = (unsigned)type - (unsigned)ObjectHandleType::ServerEocPointTrigger;
			auto componentType = (EntityComponentIndex)((unsigned)EntityComponentIndex::EoCPointTrigger + typeIdx);

			auto component = GetBaseComponent(componentType, type, handle, logError);
			if (component != nullptr) {
				return (Trigger*)((uintptr_t)component - TriggerOffsets[typeIdx]);
			} else {
				return nullptr;
			}
		}

		IGameObject* GetGameObject(char const* nameGuid, bool logError = true);
		IGameObject* GetGameObject(ComponentHandle handle, bool logError = true);
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


	struct EntityWorld : public EntityWorldBase<EntityComponentIndex>
	{
	public:
		IEoCClientObject* GetGameObject(char const* nameGuid, bool logError = true);
		IEoCClientObject* GetGameObject(ComponentHandle handle, bool logError = true);
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
		using HandleErrorProc = void (EoCClient* self, STDWString const* message, bool exitGame, STDWString const* a4);

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
		ObjectSet<int> PrimitiveSetUnkn;
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

	typedef void (*GameStateThreaded__GameStateWorker__DoWork)(void* self);
	typedef void (*GameStateMachine__Update)(void* self, GameTime* time);
	typedef void (*GameStateEventManager__ExecuteGameStateChangedEvent)(void* self, GameState fromState, GameState toState);
	typedef void (*GameStateLoad__IncLocalProgress)(void* self, int progress, char const* state);

	EntityWorld* GetEntityWorld();
}

END_SE()
