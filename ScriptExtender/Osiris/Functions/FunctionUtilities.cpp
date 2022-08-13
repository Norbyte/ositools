#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Components/Projectile.h>
#include <GameDefinitions/Components/Scenery.h>
#include <GameDefinitions/Components/Trigger.h>
#include <GameDefinitions/Symbols.h>

BEGIN_SE()

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

END_SE()

BEGIN_NS(esv)

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

Trigger* EntityWorld::GetTrigger(char const* nameGuid, bool logError)
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

Trigger* EntityWorld::GetTrigger(ComponentHandle handle, bool logError)
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

IGameObject* EntityWorld::GetGameObject(char const* nameGuid, bool logError)
{
	if (nameGuid == nullptr) {
		OsiError("Attempted to look up object with null name!");
		return nullptr;
	}

	auto character = GetComponent<Character>(nameGuid, false);
	if (character != nullptr) {
		return character;
	}

	auto item = GetComponent<Item>(nameGuid, false);
	if (item != nullptr) {
		return item;
	}

	auto trigger = GetTrigger(nameGuid, false);
	if (trigger) {
		return trigger;
	}

	if (logError) {
		OsiError("No EoC server object found with GUID '" << nameGuid << "'");
	}

	return nullptr;
}

#define TRY_GET(nm) case ObjectHandleType::Server##nm: \
	return GetComponent<nm>(handle, logError);

IGameObject* EntityWorld::GetGameObject(ComponentHandle handle, bool logError)
{
	if (!handle) {
		return nullptr;
	}

	switch ((ObjectHandleType)handle.GetType()) {
		TRY_GET(Character)
		TRY_GET(Item)
		TRY_GET(Projectile)
		TRY_GET(EocPointTrigger)
		TRY_GET(EocAreaTrigger)
		TRY_GET(StartTrigger)
		TRY_GET(TeleportTrigger)
		TRY_GET(EventTrigger)
		TRY_GET(CrimeAreaTrigger)
		TRY_GET(CrimeRegionTrigger)
		TRY_GET(AtmosphereTrigger)
		TRY_GET(AIHintAreaTrigger)
		TRY_GET(MusicVolumeTrigger)
		TRY_GET(SecretRegionTrigger)
		TRY_GET(StatsAreaTrigger)
		TRY_GET(SoundVolumeTrigger)
		TRY_GET(RegionTrigger)
		TRY_GET(ExplorationTrigger)

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
		return inventoryMgr->Get(handle);
	} else {
		return nullptr;
	}
}

esv::GameAction * FindGameActionByHandle(ComponentHandle const & handle)
{
	auto actionMgr = GetStaticSymbols().GetGameActionManager();

	for (auto action : actionMgr->GameActions) {
		if (action->Handle == handle) {
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

END_NS()

BEGIN_NS(ecl)

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

IEoCClientObject* EntityWorld::GetGameObject(char const* nameGuid, bool logError)
{
	if (!nameGuid) {
		OsiError("Attempted to look up object with null name!");
		return nullptr;
	}

	auto character = GetComponent<Character>(nameGuid, false);
	if (character != nullptr) {
		return character;
	}

	auto item = GetComponent<Item>(nameGuid, false);
	if (item != nullptr) {
		return item;
	}

	auto scenery = GetComponent<Scenery>(nameGuid, false);
	if (scenery != nullptr) {
		return scenery;
	}

	if (logError) {
		OsiError("No EoC client object found with GUID '" << nameGuid << "'");
	}

	return nullptr;
}

IEoCClientObject* EntityWorld::GetGameObject(ComponentHandle handle, bool logError)
{
	if (!handle) {
		return nullptr;
	}

	switch ((ObjectHandleType)handle.GetType()) {
	case ObjectHandleType::ClientCharacter:
		return GetComponent<Character>(handle, logError);

	case ObjectHandleType::ClientItem:
		return GetComponent<Item>(handle, logError);

	case ObjectHandleType::ClientScenery:
		return GetComponent<Scenery>(handle, logError);

	default:
		OsiError("GameObjects with handle type " << handle.GetType() << " not supported!");
		return nullptr;
	}
}

ecl::Inventory* FindInventoryByHandle(ComponentHandle const& handle, bool logError)
{
	auto inventoryMgr = GetStaticSymbols().GetClientInventoryFactory();
	if (inventoryMgr != nullptr) {
		return inventoryMgr->Get(handle);
	} else {
		return nullptr;
	}
}

END_NS()
