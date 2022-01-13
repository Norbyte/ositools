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

			switch ((ObjectType)handle.GetType()) {
			case ObjectType::ServerCharacter:
				return GetCharacter(handle, logError);

			case ObjectType::ServerItem:
				return GetItem(handle, logError);

			case ObjectType::ServerProjectile:
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
