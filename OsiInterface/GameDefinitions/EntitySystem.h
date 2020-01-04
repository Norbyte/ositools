#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "Stats.h"

namespace osidbg
{

#pragma pack(push, 1)
	struct NetworkObjectFactory : public ObjectFactory
	{
		FixedStringMapBase<void *> CharacterMap;
		uint32_t _Pad;
		uint32_t Unkn4;
		uint32_t _Pad2;
		uint64_t Unkn5;
		uint32_t Unkn6;
		uint32_t _Pad3;
		uint64_t Unkn7[7];
		Array<unsigned short> ShortArray;
		uint64_t Unkn8;
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
		uint8_t _Pad[4];
		void * PrimitiveSetVMT;
		PrimitiveSet<uint64_t> PSet;
		uint8_t Unkn3;
		uint8_t _Pad2[7];
	};

	struct EntityEntry
	{
		void * VMT;
		ComponentLayout Layout;
	};

	namespace esv
	{
		struct CustomStatDefinitionComponent;
		struct CustomStatSystem;
		struct NetComponent;
	}

	namespace eoc
	{
		struct CustomStatsComponent;
	}

	struct EntityWorld : public ProtectedGameObject<EntityWorld>
	{
		void * VMT;
		Array<EntityEntry *> EntityEntries;
		Array<uint32_t> EntitySalts;
		uint64_t Unkn1[4];
		PrimitiveSet<EntityEntry *> EntityEntries2;
		uint64_t Unkn2;
		uint8_t Unkn3;
		uint8_t _Pad3[3];
		uint32_t Unkn4;
		Array<ComponentTypeEntry> Components;
		ObjectSet<void *> KeepAlives; // ObjectSet<ObjectHandleRefMap<ComponentKeepAliveDesc>>
		Array<SystemTypeEntry> SystemTypes;
		Array<void *> EventTypes; // Array<EventTypeEntry>
		void * EntityWorldManager;
		void * SystemTypeEntry_PrimSetVMT;
		PrimitiveSet<SystemTypeEntry> SystemTypes2;
		uint64_t Unkn5;
		ObjectSet<void *> Funcs; // ObjectSet<function>
		FixedStringRefMap<int> RefMap; // ???

		void * GetComponentByEntityHandle(ObjectHandle entityHandle, ComponentType type);
		void * FindComponentByHandle(ObjectHandle componentHandle, ComponentType type);

		inline esv::CustomStatDefinitionComponent * FindCustomStatDefinitionComponentByHandle(ObjectHandle componentHandle)
		{
			auto component = FindComponentByHandle(componentHandle, ComponentType::CustomStatDefinition);
			if (component != nullptr) {
				return (esv::CustomStatDefinitionComponent *)((uint8_t *)component - 80);
			} else {
				return nullptr;
			}
		}

		inline eoc::CustomStatsComponent * GetCustomStatsComponentByEntityHandle(ObjectHandle entityHandle)
		{
			return (eoc::CustomStatsComponent *)GetComponentByEntityHandle(entityHandle, ComponentType::CustomStats);
		}

		inline esv::NetComponent * GetNetComponentByEntityHandle(ObjectHandle entityHandle)
		{
			return (esv::NetComponent *)GetComponentByEntityHandle(entityHandle, ComponentType::Net);
		}

		inline esv::CustomStatSystem * GetCustomStatSystem()
		{
			auto sys = SystemTypes.Buf[(uint32_t)SystemType::CustomStat].System;
			return (esv::CustomStatSystem *)((uint8_t *)sys - 0x18);
		}
	};

	struct CharacterFactory : public NetworkObjectFactory
	{
		void * VMT2;
		void * VMT3;
		FixedStringMapBase<void *> FSMap_ReloadComponent;
		uint32_t _Pad4;
		EntityWorld * Entities;
		uint64_t Unkn8[2];
	};

	struct ItemFactory : public NetworkObjectFactory
	{
		void * VMT2;
		void * VMT3;
		FixedStringMapBase<void *> FSMap_ReloadComponent;
		uint32_t _Pad4;
		EntityWorld * Entities;
		uint64_t Unkn8[2];
	};

	namespace eocnet
	{
		struct Message
		{
			void * VMT;
			uint32_t MessageId;
			uint32_t Always4{ 4 };
			uint32_t MsgType{ 1 };
			uint8_t Always0{ 0 };
			uint8_t Unknown1{ 0 };
			uint8_t _Pad1[2];
			uint64_t Unknown2{ 0 };
			uint32_t Unknown3{ 0 };
			uint32_t Unknown4{ 0 };
		};
	}

	namespace esv
	{
		struct EoCServerObject : public ProtectedGameObject<EoCServerObject>
		{
			virtual ~EoCServerObject() = 0;
			virtual void HandleTextKeyEvent() = 0;
			virtual uint64_t Ret5() = 0;
			virtual void SetObjectHandle(ObjectHandle Handle) = 0;
			virtual void GetObjectHandle(ObjectHandle * Handle) const = 0;
			virtual void SetGuid(FixedString const & fs) = 0;
			virtual FixedString * GetGuid() const = 0;
			virtual void SetNetID(NetId netId) = 0;
			virtual void GetNetID(NetId & netId) const = 0;
			virtual void SetCurrentTemplate(void * esvTemplate) = 0;
			virtual void * GetCurrentTemplate() const = 0;
			virtual void SetGlobal(bool isGlobal) = 0;
			virtual bool IsGlobal() const = 0;
			virtual uint32_t GetComponentType() = 0;
			virtual void * GetEntityObjectByHandle(ObjectHandle handle) = 0;
			virtual STDWString * GetName() = 0;
			virtual void SetFlags(uint64_t flag) = 0;
			virtual void ClearFlags(uint64_t flag) = 0;
			virtual bool HasFlag(uint64_t flag) const = 0;
			virtual void SetAiColliding(bool colliding) = 0;
			virtual void GetTags() = 0;
			virtual void IsTagged() = 0;
			virtual Vector3 const * GetTranslate() const = 0;
			virtual glm::mat3 const * GetRotation() const = 0;
			virtual float GetScale() const = 0;
			virtual void SetTranslate(Vector3 const & translate) = 0;
			virtual void SetRotation(glm::mat3 const & rotate) = 0;
			virtual void SetScale(float scale) = 0;
			virtual Vector3 const * GetVelocity() = 0;
			virtual void SetVelocity(Vector3 const & translate) = 0;
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
			virtual FixedString * GetCurrentLevel() const = 0;
			virtual void SetCurrentLevel(FixedString const & level) = 0;
			virtual void AddPeer() = 0;
			virtual void UNK1() = 0;
			virtual void UNK2() = 0;
			virtual void UNK3() = 0;
			virtual void GetAi() = 0;
			virtual void LoadAi() = 0;
			virtual void UnloadAi() = 0;
			virtual void GetDisplayName() = 0;
			virtual void SavegameVisit() = 0;
			virtual void GetEntityNetworkId() = 0;
			virtual void SetTemplate() = 0;
			virtual void SetOriginalTemplate_M() = 0;

			BaseComponent Base;
			FixedString MyGuid;

			NetId NetID;
			uint32_t _Pad1;
			glm::vec3 WorldPos; // Saved
			uint32_t _Pad2;
			uint64_t Flags; // Saved
			uint32_t U2;
			uint32_t _Pad3;
			FixedString CurrentLevel; // Saved
		};

	}
#pragma pack(pop)
}