#include <stdafx.h>
#include <GameDefinitions/Components/Projectile.h>
#include <GameDefinitions/GameObjects/Ai.h>
#include <GameDefinitions/GameObjects/Surface.h>
#include <GameDefinitions/GameObjects/Movement.h>
#include <GameDefinitions/GameObjects/Controllers.h>
#include <GameDefinitions/Components/Scenery.h>
#include <GameDefinitions/Components/Trigger.h>
#include <GameDefinitions/CustomStats.h>
#include <Lua/Server/LuaBindingServer.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <ScriptHelpers.h>
#include "resource.h"

BEGIN_NS(esv::lua::ecs)
void MakeLegacyServerCharacterObjectRef(lua_State* L, esv::Character* value);
Character* LuaGetCharacter(lua_State* L, int index);
Item* LuaGetItem(lua_State* L, int index);
Trigger* LuaGetTrigger(lua_State* L, int index);
IGameObject* LuaGetGameObject(lua_State* L, int idx);
END_NS()

BEGIN_NS(lua)

LifetimeHandle LifetimeFromState(lua_State* L)
{
	return State::FromLua(L)->GetCurrentLifetime();
}

LifetimeHandle GlobalLifetimeFromState(lua_State* L)
{
	return State::FromLua(L)->GetGlobalLifetime();
}

#define MAKE_REF(ty) case ActionDataType::ty: MakeDirectObjectRef(L, lifetime, static_cast<ty##ActionData*>(obj)); return;

void LuaPolymorphic<IActionData>::MakeRef(lua_State* L, IActionData* obj, LifetimeHandle const & lifetime)
{
	switch (obj->Type) {
	MAKE_REF(Teleport)
	MAKE_REF(CreateSurface)
	MAKE_REF(CreatePuddle)
	MAKE_REF(Consume)
	MAKE_REF(Door)
	MAKE_REF(Book)
	MAKE_REF(UseSkill)
	MAKE_REF(SkillBook)
	MAKE_REF(Sit)
	MAKE_REF(Lying)
	MAKE_REF(Identify)
	MAKE_REF(Repair)
	MAKE_REF(Lockpick)
	MAKE_REF(DisarmTrap)
	MAKE_REF(ShowStoryElementUI)
	MAKE_REF(Craft)
	MAKE_REF(PlaySound)
	MAKE_REF(SpawnCharacter)
	MAKE_REF(Constrain)
	MAKE_REF(Recipe)

	case ActionDataType::DestroyParameters:
		MakeDirectObjectRef(L, lifetime, static_cast<DestroyParametersData*>(obj));
		return;

	default:
		MakeDirectObjectRef(L, lifetime, obj);
		return;
	}
}

#undef MAKE_REF


#define MAKE_REF(ty) case SurfaceActionType::ty: MakeDirectObjectRef(L, lifetime, static_cast<esv::ty*>(obj)); return;

void LuaPolymorphic<esv::SurfaceAction>::MakeRef(lua_State* L, esv::SurfaceAction* obj, LifetimeHandle const& lifetime)
{
	switch (obj->VMT->GetTypeId(obj)) {
	MAKE_REF(CreateSurfaceAction)
	MAKE_REF(CreatePuddleAction)
	MAKE_REF(ExtinguishFireAction)
	MAKE_REF(ZoneAction)
	MAKE_REF(TransformSurfaceAction)
	MAKE_REF(ChangeSurfaceOnPathAction)
	MAKE_REF(RectangleSurfaceAction)
	MAKE_REF(PolygonSurfaceAction)
	MAKE_REF(SwapSurfaceAction)

	default:
		OsiError("No property map found for this surface type!");
		MakeDirectObjectRef(L, lifetime, obj);
		return;
	}
}

#undef MAKE_REF


#define MAKE_REF(ty, cls) case ObjectHandleType::ty: MakeDirectObjectRef(L, lifetime, static_cast<cls*>(obj)); return;

void LuaPolymorphic<IGameObject>::MakeRef(lua_State* L, IGameObject* obj, LifetimeHandle const & lifetime)
{
	ComponentHandle handle;
	obj->GetObjectHandle(handle);

	switch ((ObjectHandleType)handle.GetType()) {
	MAKE_REF(ServerCharacter, esv::Character)
	MAKE_REF(ServerItem, esv::Item)
	MAKE_REF(ServerProjectile, esv::Projectile)
	MAKE_REF(ServerEocPointTrigger, esv::EocPointTrigger)
	MAKE_REF(ServerEocAreaTrigger, esv::EocAreaTrigger)
	MAKE_REF(ServerStartTrigger, esv::StartTrigger)
	MAKE_REF(ServerTeleportTrigger, esv::TeleportTrigger)
	MAKE_REF(ServerEventTrigger, esv::EventTrigger)
	MAKE_REF(ServerCrimeAreaTrigger, esv::CrimeAreaTrigger)
	MAKE_REF(ServerCrimeRegionTrigger, esv::CrimeRegionTrigger)
	MAKE_REF(ServerAtmosphereTrigger, esv::AtmosphereTrigger)
	MAKE_REF(ServerAIHintAreaTrigger, esv::AIHintAreaTrigger)
	MAKE_REF(ServerMusicVolumeTrigger, esv::MusicVolumeTrigger)
	MAKE_REF(ServerSecretRegionTrigger, esv::SecretRegionTrigger)
	MAKE_REF(ServerStatsAreaTrigger, esv::StatsAreaTrigger)
	MAKE_REF(ServerSoundVolumeTrigger, esv::SoundVolumeTrigger)
	MAKE_REF(ServerRegionTrigger, esv::RegionTrigger)
	MAKE_REF(ServerExplorationTrigger, esv::ExplorationTrigger)
		
	MAKE_REF(ClientCharacter, ecl::Character)
	MAKE_REF(ClientItem, ecl::Item)
	MAKE_REF(ClientScenery, ecl::Scenery)

	default:
		OsiError("Creating Lua proxy for unknown handle type " << handle.GetType());
		MakeDirectObjectRef(L, lifetime, obj);
		return;
	}
}

#undef MAKE_REF


void LuaPolymorphic<IEoCServerObject>::MakeRef(lua_State* L, IEoCServerObject* obj, LifetimeHandle const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

void LuaPolymorphic<IEoCClientObject>::MakeRef(lua_State* L, IEoCClientObject* obj, LifetimeHandle const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

void LuaPolymorphic<Trigger>::MakeRef(lua_State* L, Trigger* obj, LifetimeHandle const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

void LuaPolymorphic<AreaTrigger>::MakeRef(lua_State* L, AreaTrigger* obj, LifetimeHandle const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

void LuaPolymorphic<esv::AreaTriggerBase>::MakeRef(lua_State* L, esv::AreaTriggerBase* obj, LifetimeHandle const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

void LuaPolymorphic<esv::PointTriggerBase>::MakeRef(lua_State* L, esv::PointTriggerBase* obj, LifetimeHandle const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

#define MAKE_REF(ty, cls) if (*type == GFS.ty) { MakeDirectObjectRef(L, lifetime, static_cast<cls*>(obj)); return; }

void LuaPolymorphic<GameObjectTemplate>::MakeRef(lua_State* L, GameObjectTemplate* obj, LifetimeHandle const& lifetime)
{
	auto type = obj->GetTypeId();

	MAKE_REF(strLevelTemplate, LevelTemplate)
	MAKE_REF(strcharacter, CharacterTemplate)
	MAKE_REF(stritem, ItemTemplate)
	MAKE_REF(strsurface, SurfaceTemplate)
	MAKE_REF(strprojectile, ProjectileTemplate)
	MAKE_REF(strtrigger, TriggerTemplate)

	OsiError("Creating Lua proxy for unknown template type " << *type);
	MakeDirectObjectRef(L, lifetime, obj);
}

#undef MAKE_REF

void LuaPolymorphic<EoCGameObjectTemplate>::MakeRef(lua_State* L, EoCGameObjectTemplate* obj, LifetimeHandle const& lifetime)
{
	LuaPolymorphic<GameObjectTemplate>::MakeRef(L, obj, lifetime);
}

#define MAKE_REF(ty, cls) case StatusType::ty: MakeDirectObjectRef(L, lifetime, static_cast<cls*>(status)); return;

void LuaPolymorphic<esv::Status>::MakeRef(lua_State* L, esv::Status* status, LifetimeHandle const & lifetime)
{
	switch (status->GetStatusId()) {
	MAKE_REF(HIT, esv::StatusHit)
	MAKE_REF(DYING, esv::StatusDying)
	MAKE_REF(HEAL, esv::StatusHeal)
	MAKE_REF(CHARMED, esv::StatusCharmed)
	MAKE_REF(KNOCKED_DOWN, esv::StatusKnockedDown)
	MAKE_REF(SUMMONING, esv::StatusSummoning)
	MAKE_REF(HEALING, esv::StatusHealing)
	MAKE_REF(THROWN, esv::StatusThrown)
	MAKE_REF(TELEPORT_FALLING, esv::StatusTeleportFall)
	MAKE_REF(COMBAT, esv::StatusCombat)
	MAKE_REF(AOO, esv::StatusAoO)
	MAKE_REF(SNEAKING, esv::StatusSneaking)
	MAKE_REF(UNLOCK, esv::StatusUnlock)
	MAKE_REF(BOOST, esv::StatusBoost)
	MAKE_REF(UNSHEATHED, esv::StatusUnsheathed)
	MAKE_REF(STANCE, esv::StatusStance)
	MAKE_REF(SITTING, esv::StatusLying)
	MAKE_REF(LYING, esv::StatusLying)
	MAKE_REF(INFECTIOUS_DISEASED, esv::StatusInfectiousDiseased)
	MAKE_REF(INVISIBLE, esv::StatusInvisible)
	MAKE_REF(ROTATE, esv::StatusRotate)
	MAKE_REF(IDENTIFY, esv::StatusIdentify)
	MAKE_REF(REPAIR, esv::StatusRepair)
	MAKE_REF(MATERIAL, esv::StatusMaterial)
	MAKE_REF(EXPLODE, esv::StatusExplode)
	MAKE_REF(ADRENALINE, esv::StatusAdrenaline)
	MAKE_REF(SHACKLES_OF_PAIN, esv::StatusShacklesOfPain)
	MAKE_REF(SHACKLES_OF_PAIN_CASTER, esv::StatusShacklesOfPainCaster)
	MAKE_REF(DRAIN, esv::StatusDrain)
	MAKE_REF(SPIRIT_VISION, esv::StatusSpiritVision)
	MAKE_REF(SPIRIT, esv::StatusSpirit)
	MAKE_REF(DAMAGE, esv::StatusDamage)
	MAKE_REF(CLIMBING, esv::StatusClimbing)
	MAKE_REF(INCAPACITATED, esv::StatusIncapacitated)
	MAKE_REF(INSURFACE, esv::StatusInSurface)
	MAKE_REF(POLYMORPHED, esv::StatusPolymorphed)
	MAKE_REF(DAMAGE_ON_MOVE, esv::StatusDamageOnMove)
	MAKE_REF(CHALLENGE, esv::StatusChallenge)
	MAKE_REF(HEAL_SHARING, esv::StatusHealSharing)
	MAKE_REF(HEAL_SHARING_CASTER, esv::StatusHealSharingCaster)
	MAKE_REF(ACTIVE_DEFENSE, esv::StatusActiveDefense)
	MAKE_REF(SPARK, esv::StatusSpark)
	MAKE_REF(CONSTRAINED, esv::StatusLying)

	case StatusType::STORY_FROZEN:
	case StatusType::SMELLY:
	case StatusType::CLEAN:
	case StatusType::UNHEALABLE:
	case StatusType::FLANKED:
	case StatusType::INFUSED:
	case StatusType::SOURCE_MUTED:
	case StatusType::DEMONIC_BARGAIN:
	case StatusType::EFFECT:
	case StatusType::TUTORIAL_BED:
		MakeDirectObjectRef(L, lifetime, static_cast<esv::Status*>(status));
		return;

	case StatusType::MUTED:
	case StatusType::CONSUME:
	case StatusType::FEAR:
	case StatusType::BLIND:
	case StatusType::ENCUMBERED:
	case StatusType::LEADERSHIP:
	case StatusType::WIND_WALKER:
	case StatusType::DARK_AVENGER:
	case StatusType::REMORSE:
	case StatusType::DECAYING_TOUCH:
	case StatusType::CHANNELING:
	case StatusType::FORCE_MOVE:
	case StatusType::OVERPOWER:
	case StatusType::COMBUSTION:
	case StatusType::GUARDIAN_ANGEL:
	case StatusType::FLOATING:
	case StatusType::DISARMED:
	case StatusType::EXTRA_TURN:
	case StatusType::PLAY_DEAD:
	case StatusType::DEACTIVATED:
		MakeDirectObjectRef(L, lifetime, static_cast<esv::StatusConsumeBase*>(status));
		return;

	default:
		OsiWarn("No property map available for unknown status type " << (unsigned)status->GetStatusId());
		MakeDirectObjectRef(L, lifetime, static_cast<esv::Status*>(status));
		return;
	}
}

#undef MAKE_REF


#define MAKE_REF(ty, cls) case StatusType::ty: MakeDirectObjectRef(L, lifetime, static_cast<ecl::cls*>(status)); return;

void LuaPolymorphic<ecl::Status>::MakeRef(lua_State* L, ecl::Status* status, LifetimeHandle const & lifetime)
{
	switch (status->GetStatusId()) {
	MAKE_REF(HIT, StatusHit)
	MAKE_REF(DYING, StatusDying)
	MAKE_REF(HEAL, StatusHeal)
	MAKE_REF(CHARMED, StatusCharmed)
	MAKE_REF(KNOCKED_DOWN, StatusKnockedDown)
	MAKE_REF(SUMMONING, StatusSummoning)
	MAKE_REF(HEALING, StatusHealing)
	MAKE_REF(THROWN, StatusThrown)
	MAKE_REF(TELEPORT_FALLING, StatusTeleportFall)
	MAKE_REF(COMBAT, StatusCombat)
	MAKE_REF(AOO, StatusAoO)
	MAKE_REF(SNEAKING, StatusSneaking)
	MAKE_REF(UNLOCK, StatusUnlock)
	MAKE_REF(BOOST, StatusBoost)
	MAKE_REF(UNSHEATHED, StatusUnsheathed)
	MAKE_REF(SITTING, StatusSitting)
	MAKE_REF(LYING, StatusSitting)
	MAKE_REF(ROTATE, StatusRotate)
	MAKE_REF(IDENTIFY, StatusIdentify)
	MAKE_REF(REPAIR, StatusRepair)
	MAKE_REF(MATERIAL, StatusMaterial)
	MAKE_REF(ADRENALINE, StatusAdrenaline)
	MAKE_REF(SPIRIT, StatusSpirit)
	MAKE_REF(DAMAGE, StatusDamage)
	MAKE_REF(CLIMBING, StatusClimbing)
	MAKE_REF(INCAPACITATED, StatusIncapacitated)
	MAKE_REF(POLYMORPHED, StatusPolymorphed)
	MAKE_REF(DAMAGE_ON_MOVE, StatusDamageOnMove)
	MAKE_REF(CHALLENGE, StatusChallenge)
	MAKE_REF(ACTIVE_DEFENSE, StatusActiveDefense)
	MAKE_REF(SPARK, StatusSpark)
	MAKE_REF(CONSTRAINED, StatusSitting)

	case StatusType::STORY_FROZEN:
	case StatusType::SMELLY:
	case StatusType::CLEAN:
	case StatusType::UNHEALABLE:
	case StatusType::FLANKED:
	case StatusType::INFUSED:
	case StatusType::SOURCE_MUTED:
	case StatusType::DEMONIC_BARGAIN:
	case StatusType::EFFECT:
	case StatusType::TUTORIAL_BED:
	case StatusType::EXPLODE:
	case StatusType::DRAIN:
	case StatusType::INSURFACE:
		MakeDirectObjectRef(L, lifetime, static_cast<ecl::Status*>(status));
		return;

	case StatusType::MUTED:
	case StatusType::CONSUME:
	case StatusType::FEAR:
	case StatusType::BLIND:
	case StatusType::ENCUMBERED:
	case StatusType::LEADERSHIP:
	case StatusType::WIND_WALKER:
	case StatusType::DARK_AVENGER:
	case StatusType::REMORSE:
	case StatusType::DECAYING_TOUCH:
	case StatusType::CHANNELING:
	case StatusType::FORCE_MOVE:
	case StatusType::OVERPOWER:
	case StatusType::COMBUSTION:
	case StatusType::GUARDIAN_ANGEL:
	case StatusType::FLOATING:
	case StatusType::DISARMED:
	case StatusType::EXTRA_TURN:
	case StatusType::PLAY_DEAD:
	case StatusType::DEACTIVATED:
	case StatusType::STANCE:
	case StatusType::INFECTIOUS_DISEASED:
	case StatusType::INVISIBLE:
	case StatusType::SHACKLES_OF_PAIN:
	case StatusType::SHACKLES_OF_PAIN_CASTER:
	case StatusType::SPIRIT_VISION:
	case StatusType::HEAL_SHARING:
	case StatusType::HEAL_SHARING_CASTER:
		MakeDirectObjectRef(L, lifetime, static_cast<ecl::StatusConsumeBase*>(status));
		return;

	default:
		MakeDirectObjectRef(L, lifetime, static_cast<ecl::Status*>(status));
		return;
	}
}

#undef MAKE_REF


void LuaPolymorphic<stats::ObjectInstance>::MakeRef(lua_State* L, stats::ObjectInstance* stats, LifetimeHandle const & lifetime)
{
	auto modifierList = stats->GetModifierList();
	if (modifierList && modifierList->Name == GFS.strCharacter) {
		MakeDirectObjectRef(L, lifetime, static_cast<stats::Character*>(stats));
	} else if (modifierList && modifierList->IsItemType()) {
		MakeDirectObjectRef(L, lifetime, static_cast<stats::Item*>(stats));
	} else {
		MakeDirectObjectRef(L, lifetime, stats);
	}
}

void LuaPolymorphic<stats::EquipmentAttributes>::MakeRef(lua_State* L, stats::EquipmentAttributes* stats, LifetimeHandle const & lifetime)
{
	switch (stats->StatsType) {
	case stats::EquipmentStatsType::Weapon: return MakeDirectObjectRef(L, lifetime, static_cast<stats::EquipmentAttributesWeapon*>(stats));
	case stats::EquipmentStatsType::Armor: return MakeDirectObjectRef(L, lifetime, static_cast<stats::EquipmentAttributesArmor*>(stats));
	case stats::EquipmentStatsType::Shield: return MakeDirectObjectRef(L, lifetime, static_cast<stats::EquipmentAttributesShield*>(stats));
	default: return MakeDirectObjectRef(L, lifetime, stats);
	}
}

void LuaPolymorphic<esv::MovementState>::MakeRef(lua_State* L, esv::MovementState* o, LifetimeHandle const & lifetime)
{
	switch (o->GetTypeId()) {
	case esv::MovementStateType::Movement: return MakeDirectObjectRef(L, lifetime, static_cast<esv::MSMovement*>(o));
	case esv::MovementStateType::MoveTo: return MakeDirectObjectRef(L, lifetime, static_cast<esv::MSMoveTo*>(o));
	default: return MakeDirectObjectRef(L, lifetime, o);
	}
}

void LuaPolymorphic<esv::ActionState>::MakeRef(lua_State* L, esv::ActionState* o, LifetimeHandle const & lifetime)
{
	switch (o->GetType()) {
	case esv::ActionStateType::Attack: return MakeDirectObjectRef(L, lifetime, static_cast<esv::ASAttack*>(o));
	case esv::ActionStateType::PrepareSkill: return MakeDirectObjectRef(L, lifetime, static_cast<esv::ASPrepareSkill*>(o));
	case esv::ActionStateType::UseSkill: return MakeDirectObjectRef(L, lifetime, static_cast<esv::ASUseSkill*>(o));
	// TODO - map others
	default: return MakeDirectObjectRef(L, lifetime, o);
	}
}

#define MAKE_REF(ty) case esv::TaskType::ty: return MakeDirectObjectRef(L, lifetime, static_cast<esv::Osiris##ty##Task*>(o));

void LuaPolymorphic<esv::Task>::MakeRef(lua_State* L, esv::Task* o, LifetimeHandle const & lifetime)
{
	switch (o->TaskTypeId) {
	MAKE_REF(MoveToLocation)
	MAKE_REF(MoveToObject)
	MAKE_REF(Flee)
	MAKE_REF(MoveInRange)
	MAKE_REF(TeleportToLocation)
	MAKE_REF(Appear)
	MAKE_REF(Disappear)
	MAKE_REF(FollowNPC)
	MAKE_REF(Wander)
	MAKE_REF(Steer)
	MAKE_REF(PlayAnimation)
	MAKE_REF(Drop)
	MAKE_REF(PickupItem)
	MAKE_REF(UseItem)
	MAKE_REF(MoveItem)
	MAKE_REF(Attack)
	MAKE_REF(Resurrect)
	MAKE_REF(UseSkill)
	MAKE_REF(MoveToAndTalk)
	default: return MakeDirectObjectRef(L, lifetime, o);
	}
}

#undef MAKE_REF

#define MAKE_REF(ty) case GameActionType::ty: return MakeDirectObjectRef(L, lifetime, static_cast<esv::ty*>(o));

void LuaPolymorphic<esv::GameAction>::MakeRef(lua_State* L, esv::GameAction* o, LifetimeHandle const & lifetime)
{
	switch (o->ActionType) {
	MAKE_REF(RainAction)
	MAKE_REF(StormAction)
	MAKE_REF(WallAction)
	MAKE_REF(TornadoAction)
	MAKE_REF(PathAction)
	MAKE_REF(GameObjectMoveAction)
	MAKE_REF(StatusDomeAction)
	default: return MakeDirectObjectRef(L, lifetime, o);
	}
}

#undef MAKE_REF

void LuaPolymorphic<rf::CameraController>::MakeRef(lua_State* L, rf::CameraController* o, LifetimeHandle const & lifetime)
{
	auto const& sym = GetStaticSymbols();
	if (sym.ecl__GameCamera__VMT && *(void**)o == sym.ecl__GameCamera__VMT) {
		return MakeDirectObjectRef(L, lifetime, static_cast<ecl::GameCamera*>(o));
	} else {
		return MakeDirectObjectRef(L, lifetime, o);
	}
}

void LuaPolymorphic<UIObject>::MakeRef(lua_State* L, UIObject* o, LifetimeHandle const & lifetime)
{
	if (o->Type == 3 || o->Type == 4) {
		return MakeDirectObjectRef(L, lifetime, static_cast<ecl::character_creation::UICharacterCreationWizard*>(o));
	} else if (o->Type == 31) {
		return MakeDirectObjectRef(L, lifetime, static_cast<ecl::UICursorInfo*>(o));
	} else {
		return MakeDirectObjectRef(L, lifetime, o);
	}
}

void LuaPolymorphic<ecl::EoCUI>::MakeRef(lua_State* L, ecl::EoCUI* o, LifetimeHandle const & lifetime)
{
	return MakeObjectRef(L, lifetime, static_cast<UIObject*>(o));
}

template <>
esv::Character* ObjectProxyHandleBasedRefImpl<esv::Character>::Get(lua_State* L) const
{
	auto self = esv::GetEntityWorld()->GetComponent<esv::Character>(handle_);
	if (!lifetime_.IsAlive(L)) {
		WarnDeprecated56("An access was made to an esv::Character instance after its lifetime has expired; this behavior is deprecated.");
	}

	return self;
}

END_NS()

namespace dse::esv::lua
{
	using namespace dse::lua;

	#include <Lua/Shared/LevelIteratorFunctions.inl>
}

#include <Lua/Server/ServerEntitySystem.inl>
#include <Lua/Server/ServerCharacter.inl>
#include <Lua/Server/ServerItem.inl>

BEGIN_NS(eoc)

void ItemDefinition::ResetProgression()
{
	LevelGroupIndex = -1;
	RootGroupIndex = -1;
	NameIndex = -1;
	NameCool = 0;
}

END_NS()

namespace dse::esv::lua
{
	using namespace dse::lua;


	char const* const ItemConstructor::MetatableName = "ItemConstructor";

	int ItemConstructorConstructItem(lua_State* L)
	{
		auto self = ItemConstructor::CheckUserData(L, 1);

		auto item = GetStaticSymbols().esv__CreateItemFromParsed(&self->Get(), 0);
		if (item == nullptr) {
			OsiErrorS("Failed to create parsed item.");
			return 0;
		}

		MakeObjectRef(L, item);
		return 1;
	}

	int ItemConstructor::Index(lua_State* L)
	{
		StackCheck _(L, 1);
		if (lua_type(L, 2) == LUA_TSTRING) {
			auto func = get<FixedString>(L, 2);
			if (func == GFS.strConstruct) {
				lua_pushcfunction(L, &ItemConstructorConstructItem);
				return 1;
			}

			OsiError("ItemConstructor has no property named " << func);
			push(L, nullptr);
			return 1;
		} else {
			auto idx = get<int>(L, 2);
			if (idx < 1 || idx > (int)definition_.size()) {
				return luaL_error(L, "Clone set only has %d elements", definition_.size());
			}

			MakeObjectRef(L, &definition_[idx - 1]);
			return 1;
		}
	}


	void ExtensionLibraryServer::Register(lua_State * L)
	{
		ExtensionLibrary::Register(L);

		OsiFunctionNameProxy::RegisterMetatable(L);
		ItemConstructor::RegisterMetatable(L);
		RegisterNameResolverMetatable(L);
		CreateNameResolver(L);
	}

	int CreateItemConstructor(lua_State* L)
	{
		StackCheck _(L, 1);
		auto type = lua_type(L, 1);
		if (type == LUA_TSTRING) {
			auto templateGuid = get<char const*>(L, 1);
			auto constructor = ItemConstructor::New(L);
			if (!script::CreateItemDefinition(templateGuid, constructor->Get())) {
				lua_pop(L, 1);
				push(L, nullptr);
			}
		} else {
			auto item = CheckedGetObject<esv::Item>(L, 1);
			bool recursive{ false };
			if (lua_gettop(L) > 1) {
				recursive = get<bool>(L, 2);
			}

			auto constructor = ItemConstructor::New(L);
			if (!script::ParseItem(item, constructor->Get(), recursive)) {
				lua_pop(L, 1);
				push(L, nullptr);
			}
		}

		return 1;
	}

	int PrepareStatus(lua_State* L)
	{
		StackCheck _(L, 1);

		auto gameObj = ecs::LuaGetGameObject(L, 1);
		auto statusId = get<FixedString>(L, 2);
		float lifeTime = get<float>(L, 3);

		if (!gameObj) {
			OsiError("Attempted to prepare status on nonexistent game object!");
			push(L, nullptr);
			return 1;
		}

		auto statusMachine = static_cast<IEoCServerObject*>(gameObj)->GetStatusMachine();
		if (!statusMachine) {
			push(L, nullptr);
			return 1;
		}

		auto status = gExtender->GetServer().GetStatusHelpers().PrepareStatus(statusMachine, statusId, lifeTime);
		if (!status) {
			push(L, nullptr);
			return 1;
		}

		MakeObjectRef(L, status);
		return 1;
	}

	int ApplyStatus(lua_State* L)
	{
		auto status = CheckedGetObject<esv::Status>(L, 1);
		StatusMachine* statusMachine{ nullptr };

		auto ownerHandle = status->OwnerHandle;
		if (ownerHandle.GetType() == (uint32_t)ObjectHandleType::ServerCharacter) {
			auto character = GetEntityWorld()->GetComponent<Character>(ownerHandle);
			if (character) {
				statusMachine = character->StatusMachine;
			}
		} else if (ownerHandle.GetType() == (uint32_t)ObjectHandleType::ServerItem) {
			auto item = GetEntityWorld()->GetComponent<Item>(ownerHandle);
			if (item) {
				statusMachine = item->StatusMachine;
			}
		}

		if (statusMachine == nullptr) {
			OsiError("No StatusMachine found for this status!");
			return 0;
		}

		auto applyStatus = GetStaticSymbols().esv__StatusMachine__ApplyStatus;
		if (applyStatus == nullptr) {
			OsiErrorS("esv::StatusMachine::ApplyStatus not found!");
			return 0;
		}

		// FIXME - TEMP CAST
		auto statusObj = (esv::Status*)statusMachine->Get(status->StatusHandle);
		if (!statusObj) {
			OsiError("No status found with this handle!");
			return 0;
		}

		if ((statusObj->Flags2 & StatusFlags2::Started) == StatusFlags2::Started) {
			OsiError("Trying to apply status that was already started!");
			return 0;
		}

		applyStatus(statusMachine, statusObj);
		return 0;
	}


	void ExtensionLibraryServer::RegisterLib(lua_State * L)
	{
		static const luaL_Reg extLib[] = {
			{"CreateItemConstructor", CreateItemConstructor},

			{"PrepareStatus", PrepareStatus},
			{"ApplyStatus", ApplyStatus},

			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -

		RegisterSharedMetatables(L);
		gModuleRegistry.ConstructState(L, ModuleRole::Server);
	}



	ServerState* ServerState::FromLua(lua_State* L)
	{
		assert(gExtender->GetServer().IsInServerThread());
		auto self = static_cast<ServerState*>(State::FromLua(L));
		assert(!self->IsClient());
		return self;
	}

	ServerState::ServerState(ExtensionState& state)
		: osiris_(state)
	{}

	ServerState::~ServerState()
	{
		if (gExtender) {
#if !defined(OSI_NO_DEBUGGER)
			auto debugger = gExtender->GetLuaDebugger();
			if (debugger) {
				debugger->ServerStateDeleted();
			}
#endif

			// FIXME - HANDLE IN SERVER LOGIC!
			gExtender->GetServer().Osiris().GetCustomFunctionManager().ClearDynamicEntries();
		}
	}

	bool ServerState::IsClient()
	{
		return false;
	}

	void ServerState::Initialize()
	{
		StackCheck _(L, 0);

		library_.Register(L);

		gExtender->GetServer().GetExtensionState().LuaLoadBuiltinFile("ServerStartup.lua");

		lua_getglobal(L, "Ext"); // stack: Ext
		StatsExtraDataProxy::New(L); // stack: Ext, "ExtraData", ExtraDataProxy
		lua_setfield(L, -2, "ExtraData"); // stack: Ext
		lua_pop(L, 1); // stack: -

		// Ext is not writeable after loading SandboxStartup!
		gExtender->GetServer().GetExtensionState().LuaLoadBuiltinFile("SandboxStartup.lua");

#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gExtender->GetLuaDebugger();
		if (debugger) {
			debugger->ServerStateCreated(this);
		}
#endif
	}


	std::optional<int32_t> ServerState::StatusGetEnterChance(esv::Status* status, bool isEnterCheck)
	{
		StatusGetEnterChanceEvent params{
			.Status = status,
			.IsEnterCheck = isEnterCheck
		};
		ThrowEvent("StatusGetEnterChance", params);
		return params.EnterChance;
	}

	bool ServerState::OnApplyStatus(ComponentHandle const& ownerHandle, esv::Status* status, bool preventStatusApply)
	{
		auto owner = GetEntityWorld()->GetGameObject(ownerHandle);
		BeforeStatusApplyEvent params { 
			.Owner = owner, 
			.Status = status,
			.PreventStatusApply = preventStatusApply
		};
		ThrowEvent("BeforeStatusApply", params);
		return params.PreventStatusApply;
	}

	void ServerState::OnStatusHitEnter(esv::StatusHit* hit, PendingHit* context)
	{
		StatusHitEnterEvent params { 
			.Hit = hit, 
			.Context = context
		};
		ThrowEvent("StatusHitEnter", params);
	}

	void ServerState::OnBeforeStatusDelete(esv::Status* status)
	{
		StatusDeleteEvent params { .Status = status };
		if (ThrowEvent("BeforeStatusDelete", params, true) == EventResult::ActionPrevented) {
			if ((bool)(status->Flags2 & (StatusFlags2::RequestDelete | StatusFlags2::RequestDeleteAtTurnEnd))) {
				status->Flags2 &= ~(StatusFlags2::RequestDelete | StatusFlags2::RequestDeleteAtTurnEnd);
			} else {
				OsiWarn("Couldn't prevent deletion of status (delete not triggered by RequestDelete flag)");
			}
		}
	}

	void ServerState::OnStatusDelete(esv::Status* status)
	{
		StatusDeleteEvent params { .Status = status };
		ThrowEvent("StatusDelete", params);
	}

	bool ServerState::ComputeCharacterHit(stats::Character * target,
		stats::Character *attacker, stats::Item *weapon, stats::DamagePairList *damageList,
		stats::HitType hitType, bool noHitRoll, bool forceReduceDurability, stats::HitDamageInfo *hit,
		stats::PropertyList *skillProperties, stats::HighGroundBonus highGroundFlag, stats::CriticalRoll criticalRoll)
	{
		auto alwaysBackstab = skillProperties != nullptr
			&& skillProperties->Properties.Find(FixedString("AlwaysBackstab")) != nullptr;

		stats::DamagePairList inputDmgList{ *damageList };
		stats::HitDamageInfo tempHit{ *hit };
		ComputeCharacterHitEvent params{
			.Target = target,
			.Attacker = attacker,
			.Weapon = weapon,
			.DamageList = &inputDmgList,
			.HitType = hitType,
			.NoHitRoll = noHitRoll,
			.ForceReduceDurability = forceReduceDurability,
			.Hit = &tempHit,
			.SkillProperties = skillProperties,
			.AlwaysBackstab = alwaysBackstab,
			.HighGround = highGroundFlag,
			.CriticalRoll = criticalRoll,
			.Handled = false
		};
		ThrowEvent("ComputeCharacterHit", params);

		if (params.Handled) {
			*hit = tempHit;
			return true;
		} else {
			return false;
		}
	}

	bool ServerState::OnCharacterApplyDamage(esv::Character* target, stats::HitDamageInfo& hit, ComponentHandle attackerHandle,
			CauseType causeType, glm::vec3& impactDirection, PendingHit* context)
	{
		stats::ObjectInstance* attacker{ nullptr };
		if (attackerHandle) {
			auto attackerChar = GetEntityWorld()->GetComponent<Character>(attackerHandle, false);
			if (attackerChar) {
				attacker = attackerChar->Stats;
			} else {
				auto attackerItem = GetEntityWorld()->GetComponent<Item>(attackerHandle, false);
				if (attackerItem) {
					attacker = attackerItem->Stats;
				} else {
					OsiError("Could not resolve attacker handle: " << std::hex << attackerHandle.Handle);
				}
			}
		}

		BeforeCharacterApplyDamageEvent evt{
			.Target = target, 
			.Attacker = attacker, 
			.Hit = &hit, 
			.Cause = causeType, 
			.ImpactDirection = impactDirection, 
			.Context = context
		};
		ThrowEvent("BeforeCharacterApplyDamage", evt);
		return evt.Handled;
	}


	void ServerState::OnGameStateChanged(GameState fromState, GameState toState)
	{
		GameStateChangedEvent params { 
			.FromState = fromState, 
			.ToState = toState
		};
		ThrowEvent("GameStateChanged", params);
	}


	esv::Item* ServerState::OnGenerateTreasureItem(esv::Item* item)
	{
		TreasureItemGeneratedEvent params { 
			.Item = item, 
			.ResultingItem = nullptr
		};
		ThrowEvent("TreasureItemGenerated", params);

		if (!params.ResultingItem) {
			return item;
		}

		if (params.ResultingItem->ParentInventoryHandle) {
			OsiError("TreasureItemGenerated must return an item that's not already in an inventory");
			return item;
		}

		if (params.ResultingItem->CurrentLevel && params.ResultingItem->CurrentLevel.GetString()[0]) {
			OsiError("TreasureItemGenerated must return an item that's not in the level");
			return item;
		}

		return params.ResultingItem;
	}


	bool ServerState::OnBeforeCraftingExecuteCombination(CraftingStationType craftingStation, ObjectSet<ComponentHandle> const& ingredients,
		esv::Character* character, uint8_t quantity, FixedString const& combinationId)
	{
		BeforeCraftingExecuteCombinationEvent params { 
			.Character = character, 
			.CraftingStation = craftingStation, 
			.CombinationId = combinationId, 
			.Quantity = quantity
		};

		for (auto ingredientHandle : ingredients) {
			auto ingredient = GetEntityWorld()->GetComponent<Item>(ingredientHandle);
			if (ingredient) {
				params.Items.push_back(ingredient);
			}
		}

		ThrowEvent("BeforeCraftingExecuteCombination", params);

		return params.Processed;
	}


	void ServerState::OnAfterCraftingExecuteCombination(CraftingStationType craftingStation, ObjectSet<ComponentHandle> const& ingredients,
		esv::Character* character, uint8_t quantity, FixedString const& combinationId, bool succeeded)
	{
		AfterCraftingExecuteCombinationEvent params { 
			.Character = character, 
			.CraftingStation = craftingStation, 
			.CombinationId = combinationId, 
			.Quantity = quantity, 
			.Succeeded = succeeded
		};

		for (auto ingredientHandle : ingredients) {
			auto ingredient = GetEntityWorld()->GetComponent<Item>(ingredientHandle);
			if (ingredient) {
				params.Items.push_back(ingredient);
			}
		}

		ThrowEvent("AfterCraftingExecuteCombination", params);
	}


	void ServerState::OnBeforeShootProjectile(ShootProjectileHelper* helper)
	{
		BeforeShootProjectileEvent params { .Projectile = helper };
		ThrowEvent("BeforeShootProjectile", params);
	}


	void ServerState::OnShootProjectile(Projectile* projectile)
	{
		ShootProjectileEvent params { .Projectile = projectile };
		ThrowEvent("ShootProjectile", params);
	}

	void ServerState::OnProjectileHit(Projectile* projectile, ComponentHandle const& hitObject, glm::vec3 const& position)
	{
		auto hitObj = GetEntityWorld()->GetGameObject(hitObject);
		ProjectileHitEvent params { 
			.Projectile = projectile, 
			.HitObject = hitObj, 
			.Position = position
		};
		ThrowEvent("ProjectileHit", params);
	}


	void ServerState::OnExecutePropertyDataOnGroundHit(glm::vec3& position, ComponentHandle casterHandle, stats::DamagePairList* damageList)
	{
		GroundHitEvent params { 
			.Position = position, 
			.Caster = GetEntityWorld()->GetGameObject(casterHandle), 
			.DamageList = damageList
		};
		ThrowEvent("GroundHit", params);
	}


	void ServerState::ExecutePropertyDataOnTarget(stats::PropertyExtender* prop, ComponentHandle attackerHandle,
		ComponentHandle targetHandle, glm::vec3 const& impactOrigin, bool isFromItem, stats::SkillPrototype * skillProto,
		stats::HitDamageInfo const* damageInfo)
	{
		auto attacker = GetEntityWorld()->GetGameObject(attackerHandle);
		auto target = GetEntityWorld()->GetGameObject(targetHandle);
		OnExecutePropertyDataOnTargetEvent params { 
			.Property = prop, 
			.Attacker = attacker, 
			.Target = target, 
			.ImpactOrigin = impactOrigin, 
			.IsFromItem = isFromItem, 
			.Skill = skillProto, 
			.Hit = damageInfo
		};
		ThrowEvent("OnExecutePropertyDataOnTarget", params);
	}


	void ServerState::ExecutePropertyDataOnPosition(stats::PropertyExtender* prop, ComponentHandle attackerHandle,
		glm::vec3 const& position, float areaRadius, bool isFromItem, stats::SkillPrototype * skillPrototype,
		stats::HitDamageInfo const* damageInfo)
	{
		auto attacker = GetEntityWorld()->GetGameObject(attackerHandle);
		OnExecutePropertyDataOnPositionEvent params {
			.Property = prop, 
			.Attacker = attacker, 
			.Position = position, 
			.AreaRadius = areaRadius, 
			.IsFromItem = isFromItem, 
			.Skill = skillPrototype, 
			.Hit = damageInfo
		};
		ThrowEvent("OnExecutePropertyDataOnPosition", params);
	}


	void ServerState::OnBeforeSortAiActions(ComponentHandle characterHandle, AiRequest* request)
	{
		AiRequestSortEvent params { 
			.CharacterHandle = characterHandle, 
			.Request = request
		};
		ThrowEvent("OnBeforeSortAiActions", params);
	}

	void ServerState::OnAfterSortAiActions(ComponentHandle characterHandle, AiRequest* request)
	{
		AiRequestSortEvent params{
			.CharacterHandle = characterHandle,
			.Request = request
		};
		ThrowEvent("OnAfterSortAiActions", params);
	}

	void ServerState::OnPeekAiAction(ComponentHandle characterHandle, AiRequest* request, AiActionType actionType, bool isFinished)
	{
		OnPeekAiActionEvent params {
			.CharacterHandle = characterHandle, 
			.Request = request, 
			.ActionType = actionType, 
			.IsFinished = isFinished
		};
		ThrowEvent("OnPeekAiAction", params);
	}


	void ServerState::OnUpdateTurnOrder(TurnManager * self, uint8_t combatId)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictOsiris);

		auto turnMgr = GetEntityWorld()->GetTurnManager();
		auto combat = turnMgr->Combats.try_get_ptr(combatId);
		if (combat == nullptr) {
			OsiError("No combat found with ID " << (unsigned)combatId);
			return;
		}

		CalculateTurnOrderEvent params{
			.Combat = combat
		};
		ThrowEvent("CalculateTurnOrder", params);
	}


	std::optional<STDString> ServerState::GetModPersistentVars(STDString const& modTable)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);
		LifetimeStackPin _p(lifetimeStack_);

		PushExtFunction(L, "_GetModPersistentVars");
		push(L, modTable);

		if (CallWithTraceback(L, 1, 1) != 0) {
			ERR("Ext._GetModPersistentVars failed: %s", lua_tostring(L, -1));
			lua_pop(L, 1);
			return {};
		}

		std::optional<STDString> result;
		if (lua_type(L, -1) == LUA_TSTRING) {
			result = get<char const*>(L, -1);
		}

		lua_pop(L, 1);
		return result;
	}


	void ServerState::RestoreModPersistentVars(STDString const& modTable, STDString const& vars)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);
		LifetimeStackPin _p(lifetimeStack_);

		PushExtFunction(L, "_RestoreModPersistentVars");
		push(L, modTable);
		push(L, vars);

		CheckedCall(L, 2, "Ext.RestoreModPersistentVars");
	}


	void ServerState::OnGameSessionLoading()
	{
		osiris_.GetIdentityAdapterMap().UpdateAdapters();

		State::OnGameSessionLoading();
	}

	void ServerState::StoryFunctionMappingsUpdated()
	{
		auto helpers = library_.GenerateOsiHelpers();
		LoadScript(helpers, "bootstrapper");
	}
}

namespace dse::esv
{

	ExtensionState & ExtensionState::Get()
	{
		return gExtender->GetServer().GetExtensionState();
	}

	ExtensionState::~ExtensionState()
	{
		if (Lua) Lua->Shutdown();
	}

	lua::State * ExtensionState::GetLua()
	{
		if (Lua) {
			return Lua.get();
		} else {
			return nullptr;
		}
	}

	ModManager * ExtensionState::GetModManager()
	{
		return GetModManagerServer();
	}

	void ExtensionState::OnGameSessionLoading()
	{
		dynamicStats_.clear();
		persistentStats_.clear();
		cachedPersistentVars_.clear();
		dse::ExtensionStateBase::OnGameSessionLoading();
	}

	void ExtensionState::Reset()
	{
		ExtensionStateBase::Reset();
		DamageHelpers.Clear();
	}

	void ExtensionState::DoLuaReset()
	{
		if (Lua) Lua->Shutdown();
		Lua.reset();
		Lua = std::make_unique<lua::ServerState>(*this);
		LuaStatePin<ExtensionState, lua::ServerState> pin(*this);
		pin->Initialize();
		pin->StoryFunctionMappingsUpdated();
	}

	void ExtensionState::LuaStartup()
	{
		ExtensionStateBase::LuaStartup();

		LuaServerPin lua(*this);
		auto gameState = GetStaticSymbols().GetServerState();
		if (gameState
			&& (*gameState == esv::GameState::LoadLevel
				|| (*gameState == esv::GameState::LoadModule && WasStatLoadTriggered())
				|| *gameState == esv::GameState::LoadGMCampaign
				|| *gameState == esv::GameState::LoadSession
				|| *gameState == esv::GameState::Sync
				|| *gameState == esv::GameState::Paused
				|| *gameState == esv::GameState::Running
				|| *gameState == esv::GameState::GameMasterPause)) {
			lua->OnModuleResume();
		}
	}

	void ExtensionState::MarkPersistentStat(FixedString const& statId)
	{
		persistentStats_.insert(statId);
	}

	void ExtensionState::UnmarkPersistentStat(FixedString const& statId)
	{
		auto it = persistentStats_.find(statId);
		if (it != persistentStats_.end()) {
			persistentStats_.erase(it);
		}
	}

	void ExtensionState::MarkDynamicStat(FixedString const& statId)
	{
		dynamicStats_.insert(statId);
	}


	std::optional<STDString> ExtensionState::GetModPersistentVars(FixedString const& mod)
	{
		auto modTable = GetModTable(mod);
		if (modTable) {
			LuaServerPin lua(*this);
			if (lua) {
				auto vars = lua->GetModPersistentVars(*modTable);
				if (vars) {
					return vars;
				}
			}
		}

		// If we failed to retrieve vars from the Lua state for some reason, 
		// return the persistent vars we loaded from the savegame instead
		auto cachedIt = cachedPersistentVars_.find(mod);
		if (cachedIt != cachedPersistentVars_.end()) {
			ERR("Persistent variables for mod %s could not be retrieved, saving cached values!", mod.GetString());
			return cachedIt->second;
		} else {
			return {};
		}
	}

	void ExtensionState::RestoreModPersistentVars(FixedString const& mod, STDString const& vars)
	{
		cachedPersistentVars_.insert(std::make_pair(mod, vars));

		auto modTable = GetModTable(mod);
		if (modTable) {
			DEBUG("Restoring persistent vars for mod %s (%ld bytes)", mod.GetString(), vars.size());
			LuaServerPin lua(*this);
			if (lua) {
				lua->RestoreModPersistentVars(*modTable, vars);
			}
		} else {
			ERR("Savegame has persistent variables for mod %s, but it is not loaded or has no ModTable! Variables may be lost on next save!", mod.GetString());
		}
	}

	std::optional<STDString> ExtensionState::GetModTable(FixedString const& mod)
	{
		auto const& configs = GetConfigs();
		auto configIt = configs.find(mod);
		if (configIt != configs.end() && !configIt->second.ModTable.empty()) {
			return configIt->second.ModTable;
		} else {
			return {};
		}
	}

	std::unordered_set<FixedString> ExtensionState::GetPersistentVarMods()
	{
		std::unordered_set<FixedString> names;
		for (auto const& kv : cachedPersistentVars_) {
			names.insert(kv.first);
		}

		for (auto const& config : GetConfigs()) {
			if (config.second.MinimumVersion >= 43 && !config.second.ModTable.empty()) {
				names.insert(config.first);
			}
		}

		return names;
	}

	void ExtensionState::StoryLoaded()
	{
		DEBUG("ExtensionStateServer::StoryLoaded()");
		LuaServerPin lua(*this);
		if (lua) {
			lua->Osiris().StoryLoaded();
		}
	}

	void ExtensionState::StoryFunctionMappingsUpdated()
	{
		LuaServerPin lua(*this);
		if (lua) {
			lua->StoryFunctionMappingsUpdated();
		}
	}

	void ExtensionState::StorySetMerging(bool isMerging)
	{
		LuaServerPin lua(*this);
		if (lua) {
			lua->Osiris().StorySetMerging(isMerging);
		}
	}
}
