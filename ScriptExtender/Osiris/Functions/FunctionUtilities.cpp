#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Components/Projectile.h>
#include <GameDefinitions/Symbols.h>

namespace dse
{
	ecl::EoCClient * GetEoCClient()
	{
		auto clientPtr = GetStaticSymbols().ecl__EoCClient;
		if (clientPtr == nullptr || *clientPtr == nullptr) {
			return nullptr;
		} else {
			return *clientPtr;
		}
	}

	ModManager * GetModManagerClient()
	{
		auto client = GetEoCClient();
		if (client == nullptr || client->ModManager == nullptr) {
			return nullptr;
		} else {
			return client->ModManager;
		}
	}

	esv::EoCServer * GetEoCServer()
	{
		auto serverPtr = GetStaticSymbols().esv__EoCServer;
		if (serverPtr == nullptr || *serverPtr == nullptr) {
			return nullptr;
		} else {
			return *serverPtr;
		}
	}

	ModManager * GetModManagerServer()
	{
		auto server = GetEoCServer();
		if (server == nullptr || server->ModManagerServer == nullptr) {
			return nullptr;
		} else {
			return server->ModManagerServer;
		}
	}

	
	void* EntityWorldBase::GetComponent(uint32_t type, ObjectHandleType handleType, ComponentHandle componentHandle, bool logError)
	{
		if (this == nullptr) {
			OsiError("Tried to find component on null EntityWorld!");
			return nullptr;
		}

		if (!componentHandle) {
			return nullptr;
		}

		if ((uint32_t)type >= Components.size()) {
			if (logError) {
				OsiError("Component type index " << (uint32_t)type << " too large!");
			}
			return nullptr;
		}

		auto componentMgr = Components[(uint32_t)type].Pool;
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

	void* EntityWorldBase::GetComponent(uint32_t componentType, char const* nameGuid, bool logError)
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
		auto component = Components[(uint32_t)componentType].Pool->FindComponentByGuid(fs);
		if (component != nullptr) {
			return component;
		} else {
			if (logError) {
				OsiError("No component found with GUID '" << nameGuid << "'");
			}
			return nullptr;
		}
	}

	void* EntityWorldBase::GetComponent(uint32_t type, NetId netId, bool logError)
	{
		if (this == nullptr) {
			OsiError("Tried to find component on null EntityWorld!");
			return nullptr;
		}

		if (!netId) {
			return nullptr;
		}

		if ((uint32_t)type >= Components.size()) {
			OsiError("Component type index " << (uint32_t)type << " too large!");
			return nullptr;
		}

		auto componentMgr = Components[(uint32_t)type].Pool;
		if (componentMgr == nullptr) {
			OsiError("Component type " << (uint32_t)type << " not bound!");
			return nullptr;
		}

		return componentMgr->FindComponentByNetId(netId, true);
	}

	EntityEntry* EntityWorldBase::GetEntity(EntityHandle entityHandle, bool logError)
	{
		if (this == nullptr) {
			OsiError("Tried to find entity on null EntityWorld!");
			return nullptr;
		}

		if (entityHandle.GetType() != 0) {
			OsiError("Entity handle has invalid type " << entityHandle.GetType());
			return nullptr;
		}

		// FIXME - use EntityPool.Get()
		auto index = entityHandle.GetIndex();
		if (index >= EntityPool.Salts.size()) {
			if (logError) {
				OsiError("Tried to fetch entity " << index << "; we only have " << EntityPool.Salts.size() << "!");
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

		return (EntityEntry*)EntityPool.ComponentsByHandleIndex[index];
	}

	void* EntityWorldBase::GetComponentByEntityHandle(uint32_t type, EntityHandle entityHandle, bool logError)
	{
		auto entity = GetEntity(entityHandle, logError);
		if (!entity) return nullptr;

		auto index = entityHandle.GetIndex();
		if ((uint32_t)type >= entity->Layout.Entries.size()) {
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
		auto componentMgr = Components[(uint32_t)type].Pool;
		return componentMgr->FindComponentByHandle(componentHandle);
	}

	namespace esv
	{
		IEoCServerObject* EntityWorld::GetGameObject(char const* nameGuid, bool logError)
		{
			if (nameGuid == nullptr) {
				OsiError("Attempted to look up object with null name!");
				return nullptr;
			}

			auto character = GetCharacter(nameGuid, false);
			if (character != nullptr) {
				return character;
			}

			auto item = GetItem(nameGuid, false);
			if (item != nullptr) {
				return item;
			}

			if (logError) {
				OsiError("No EoC server object found with GUID '" << nameGuid << "'");
			}

			return nullptr;
		}

		IEoCServerObject* EntityWorld::GetGameObject(ComponentHandle handle, bool logError)
		{
			if (!handle) {
				return nullptr;
			}

			switch ((ObjectHandleType)handle.GetType()) {
			case ObjectHandleType::ServerCharacter:
				return GetCharacter(handle, logError);

			case ObjectHandleType::ServerItem:
				return GetItem(handle, logError);

			case ObjectHandleType::ServerProjectile:
				return GetProjectile(handle, logError);

			default:
				OsiError("GameObjects with handle type " << handle.GetType() << " not supported!");
				return nullptr;
			}
		}

		EntityWorld* GetEntityWorld()
		{
			auto server = GetEoCServer();
			if (server == nullptr) {
				OsiErrorS("EoCServer not available!");
				return nullptr;
			} else {
				return server->EntityWorld;
			}
		}

		esv::TurnBasedProtocol * GetTurnBasedProtocol()
		{
			auto server = GetStaticSymbols().esv__EoCServer;
			if (server == nullptr || *server == nullptr) return nullptr;

			auto gameServer = (*server)->GameServer;
			if (gameServer == nullptr) return nullptr;

			if (gameServer->Protocols.size() <= 22) return nullptr;
			return (esv::TurnBasedProtocol *)gameServer->Protocols[22];
		}

		esv::Inventory * FindInventoryByHandle(ComponentHandle const & handle, bool logError)
		{
			auto inventoryMgr = GetStaticSymbols().GetServerInventoryFactory();
			if (inventoryMgr != nullptr) {
				// FIXME - CAST
				return (esv::Inventory*)inventoryMgr->Get(handle);
			} else {
				return nullptr;
			}
		}

		esv::GameAction * FindGameActionByHandle(ComponentHandle const & handle)
		{
			auto actionMgr = GetStaticSymbols().GetGameActionManager();

			for (auto action : actionMgr->GameActions) {
				if (action->MyHandle == handle) {
					return action;
				}
			}

			return nullptr;
		}

		ShootProjectileApiHelper::ShootProjectileApiHelper()
		{
			Helper.Random = (uint8_t)rand();
			Helper.DamageList = nullptr;
			Helper.CasterLevel = -1;
			Helper.HitObject = nullptr;
			Helper.IsTrap = false;
			Helper.UnknownFlag1 = true;
			Helper.StatusClearChance = 0.0f;
			Helper.IsFromItem = false;
			Helper.IsStealthed = false;
			Helper.IgnoreObjects = false;
		}

		void ShootProjectileApiHelper::SetGuidString(FixedString const& prop, char const * value)
		{
			auto obj = GetEntityWorld()->GetGameObject(value);

			if (obj == nullptr) {
				OsiError("GUID '" << value << "' is not a valid item or object");
				return;
			}

			ComponentHandle handle;
			obj->GetObjectHandle(handle);
			glm::vec3 position = reinterpret_cast<esv::Character*>(obj)->WorldPos;

			if (prop == GFS.strSourcePosition) {
				Helper.StartPosition = position;
				HasStartPosition = true;
			}
			else if (prop == GFS.strTargetPosition) {
				Helper.EndPosition = position;
				HasEndPosition = true;
			}
			else if (prop == GFS.strHitObjectPosition) {
				HitObject.Position = position;
				HasHitObjectPosition = true;
			}
			else if (prop == GFS.strCaster) {
				Helper.Caster = handle;
			}
			else if (prop == GFS.strSource) {
				Helper.Source = handle;
			}
			else if (prop == GFS.strTarget) {
				Helper.Target = handle;
			}
			else if (prop == GFS.strHitObject) {
				HitObject.Target = handle;
				HasHitObject = true;
			}
			else {
				OsiError("Unknown object property '" << prop << "'");
			}
		}

		void ShootProjectileApiHelper::SetVector(FixedString const& prop, glm::vec3 const & value)
		{
			if (prop == GFS.strSourcePosition) {
				Helper.StartPosition = value;
				HasStartPosition = true;
			}
			else if (prop == GFS.strTargetPosition) {
				Helper.EndPosition = value;
				HasEndPosition = true;
			}
			else if (prop == GFS.strHitObjectPosition) {
				HitObject.Position = value;
				HasHitObjectPosition = true;
			}
			else {
				OsiError("Unknown vector3 property '" << prop << "'");
			}
		}

		void ShootProjectileApiHelper::AddDamage(stats::DamageType type, int32_t amount)
		{
			DamageList.AddDamage(type, amount);
			HasDamageList = true;
		}

		bool ShootProjectileApiHelper::Shoot()
		{
			if (!HasStartPosition) {
				OsiErrorS("No start position!");
				return false;
			}

			if (!HasEndPosition) {
				OsiErrorS("No end position!");
				return false;
			}

			if (!Helper.SkillId) {
				OsiErrorS("No skill id!");
				return false;
			}

			if (HasHitObject)
			{
				Helper.HitObject = &HitObject;
				if (!HasHitObjectPosition) {
					HitObject.Position = Helper.StartPosition;
				}
			}

			if (HasDamageList) {
				Helper.DamageList = &DamageList;
			}

			auto shoot = GetStaticSymbols().esv__ProjectileHelpers__ShootProjectile;
			if (shoot == nullptr) {
				OsiErrorS("ShootProjectile helper not found!");
				return false;
			}

			auto projectile = shoot(&Helper);
			if (projectile) {
				projectile->AlwaysDamage = AlwaysDamage;
				projectile->CanDeflect = CanDeflect;
				return true;
			} else {
				return false;
			}
		}
	}

	namespace ecl
	{
		EntityWorld* GetEntityWorld()
		{
			auto client = GetEoCClient();
			if (client == nullptr) {
				OsiErrorS("GetEoCClient not available!");
				return nullptr;
			}
			else {
				return client->EntityWorld;
			}
		}

		ecl::Inventory* FindInventoryByHandle(ComponentHandle const& handle, bool logError)
		{
			auto inventoryMgr = GetStaticSymbols().GetClientInventoryFactory();
			// FIXME - cast!
			if (inventoryMgr != nullptr) {
				return (ecl::Inventory*)inventoryMgr->Get(handle);
			}
			else {
				return nullptr;
			}
		}
	}
}
