#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>

namespace dse
{
	struct ITriggerData : public ProtectedGameObject<ITriggerData>
	{
		virtual ~ITriggerData() = 0;
		virtual int GetSyncBehaviour() = 0;
		virtual ITriggerData* Clone() = 0;
		virtual bool Visit(ObjectVisitor* visitor) = 0;
	};

	struct SoundVolumeTriggerData : public ITriggerData
	{
		FixedString AmbientSound;
		float Occlusion;
		uint8_t AuxBus1;
		uint8_t AuxBus2;
		uint8_t AuxBus3;
		uint8_t AuxBus4;
	};

	struct AtmosphereTriggerData : public ITriggerData
	{
		ObjectSet<FixedString, GameMemoryAllocator, true> Atmospheres;
		float FadeTime;
	};

	struct Trigger : public IGameObjectBase
	{
		ComponentHandle Handle; // Defunct, use GetObjectHandle() instead
		FixedString UUID; // Defunct, use GetGuid() instead
		TriggerTemplate* Template;
		uint16_t field_20;
		uint16_t field_22;
		uint16_t SyncFlags;
		uint16_t field_26;
		glm::vec3 Translate;
		FixedString TriggerType;
		bool IsGlobal;
		glm::mat3 Rotate;
		glm::mat3 Rotate2;
		float field_8C;
		void* Visual;
		FixedString Level;
		void* SyncDirtyListener;
	};

	struct AreaTrigger : public Trigger
	{
		void* field_A8;
		ObjectSet<ComponentHandle> field_B0;
		// void* field_D0; - Unused?
	};

	struct AtmosphereTrigger : public AreaTrigger
	{
		PrimitiveSmallSet<FixedString> AtmosphereResourceIds;
		void* field_E8;
		FixedString CurrentAtmosphere;
		float FadeTime;
	};
}

namespace dse::esv
{
	struct EsvTrigger : public dse::Trigger
	{};

	struct AtmosphereTrigger : public dse::AtmosphereTrigger
	{
		BaseComponent Base;
		FixedString MyGuid;
		NetId NetID;
	};
}
