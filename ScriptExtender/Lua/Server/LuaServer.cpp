#include <stdafx.h>
#include <GameDefinitions/Components/Projectile.h>
#include <GameDefinitions/GameObjects/Ai.h>
#include <GameDefinitions/GameObjects/Surface.h>
#include <GameDefinitions/Components/Trigger.h>
#include <GameDefinitions/CustomStats.h>
#include <Lua/Server/LuaBindingServer.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>
#include <ScriptHelpers.h>
#include "resource.h"

BEGIN_NS(lua)

// FIXME - deprecated; use state from L wherever possible
LifetimeHolder GetCurrentLifetime()
{
	if (gExtender->GetServer().IsInServerThread()) {
		return esv::lua::GetServerLifetime();
	}
	else {
		return ecl::lua::GetClientLifetime();
	}
}

LifetimePool& GetLifetimePool()
{
	if (gExtender->GetServer().IsInServerThread()) {
		return esv::lua::GetServerLifetimePool();
	}
	else {
		return ecl::lua::GetClientLifetimePool();
	}
}

LifetimeHolder LifetimeFromState(lua_State* L)
{
	return State::FromLua(L)->GetCurrentLifetime();
}

LifetimeHolder GlobalLifetimeFromState(lua_State* L)
{
	return State::FromLua(L)->GetGlobalLifetime();
}

#define MAKE_REF(ty) case ActionDataType::ty: ObjectProxy2::MakeRef(L, static_cast<ty##ActionData*>(obj), lifetime); return;

void LuaPolymorphic<IActionData>::MakeRef(lua_State* L, IActionData* obj, LifetimeHolder const & lifetime)
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
		ObjectProxy2::MakeRef(L, static_cast<DestroyParametersData*>(obj), lifetime);
		return;

	default:
		ObjectProxy2::MakeRef(L, obj, lifetime);
		return;
	}
}

#undef MAKE_REF


#define MAKE_REF(ty, cls) case ObjectType::ty: ObjectProxy2::MakeRef(L, static_cast<cls*>(obj), lifetime); return;

void LuaPolymorphic<IGameObject>::MakeRef(lua_State* L, IGameObject* obj, LifetimeHolder const & lifetime)
{
	ComponentHandle handle;
	obj->GetObjectHandle(handle);

	switch ((ObjectType)handle.GetType()) {
	MAKE_REF(ServerCharacter, esv::Character)
	MAKE_REF(ServerItem, esv::Item)
	MAKE_REF(ServerProjectile, esv::Projectile)
	MAKE_REF(ServerAtmosphereTrigger, esv::AtmosphereTrigger)

	// FIXME - triggers need a prototype for each type, since they multiple-inherit from IEoCServerObject
	/*case ObjectType::ServerEocPointTrigger:
	case ObjectType::ServerEocAreaTrigger:
	case ObjectType::ServerStartTrigger:
	case ObjectType::ServerTeleportTrigger:
	case ObjectType::ServerEventTrigger:
	case ObjectType::ServerCrimeAreaTrigger:
	case ObjectType::ServerCrimeRegionTrigger:
	case ObjectType::ServerAIHintAreaTrigger:
	case ObjectType::ServerMusicVolumeTrigger:
	case ObjectType::ServerSecretRegionTrigger:
	case ObjectType::ServerStatsAreaTrigger:
	case ObjectType::ServerSoundVolumeTrigger:
	case ObjectType::ServerRegionTrigger:
	case ObjectType::ServerExplorationTrigger:
		ObjectProxy2::MakeRef(L, static_cast<esv::Trigger*>(obj), lifetime);
		return;*/

	default:
		OsiError("Creating Lua proxy for unknown handle type " << handle.GetType());
		ObjectProxy2::MakeRef(L, obj, lifetime);
		return;
	}
}

#undef MAKE_REF


#define MAKE_REF(ty, cls) case StatusType::ty: ObjectProxy2::MakeRef(L, static_cast<cls*>(status), lifetime); return;

void LuaPolymorphic<IEoCServerObject>::MakeRef(lua_State* L, IEoCServerObject* obj, LifetimeHolder const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

void LuaPolymorphic<IEoCClientObject>::MakeRef(lua_State* L, IEoCClientObject* obj, LifetimeHolder const& lifetime)
{
	LuaPolymorphic<IGameObject>::MakeRef(L, obj, lifetime);
}

void LuaPolymorphic<esv::Status>::MakeRef(lua_State* L, esv::Status* status, LifetimeHolder const & lifetime)
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
		ObjectProxy2::MakeRef(L, static_cast<esv::Status*>(status), lifetime);
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
		ObjectProxy2::MakeRef(L, static_cast<esv::StatusConsumeBase*>(status), lifetime);
		return;

	default:
		OsiWarn("No property map available for unknown status type " << (unsigned)status->GetStatusId());
		ObjectProxy2::MakeRef(L, static_cast<esv::Status*>(status), lifetime);
		return;
	}
}

#undef MAKE_REF

void LuaPolymorphic<stats::ObjectInstance>::MakeRef(lua_State* L, stats::ObjectInstance* stats, LifetimeHolder const & lifetime)
{
	auto modifierList = stats->GetModifierList();
	if (modifierList->Name == GFS.strCharacter) {
		return MakeObjectRef(L, lifetime, static_cast<stats::Character*>(stats));
	} else if (modifierList->Name == GFS.strItem) {
		return MakeObjectRef(L, lifetime, static_cast<stats::Item*>(stats));
	} else {
		return MakeObjectRef(L, lifetime, stats);
	}
}

void LuaPolymorphic<stats::EquipmentAttributes>::MakeRef(lua_State* L, stats::EquipmentAttributes* stats, LifetimeHolder const & lifetime)
{
	switch (stats->StatsType) {
	case stats::EquipmentStatsType::Weapon: return MakeObjectRef(L, lifetime, static_cast<stats::EquipmentAttributesWeapon*>(stats));
	case stats::EquipmentStatsType::Armor: return MakeObjectRef(L, lifetime, static_cast<stats::EquipmentAttributesArmor*>(stats));
	case stats::EquipmentStatsType::Shield: return MakeObjectRef(L, lifetime, static_cast<stats::EquipmentAttributesShield*>(stats));
	default: return MakeObjectRef(L, lifetime, stats);
	}
}

template <>
esv::Character* ObjectProxyHandleBasedRefImpl<esv::Character>::Get() const
{
	auto self = esv::GetEntityWorld()->GetCharacter(handle_);
	if (!lifetime_.IsAlive()) {
		WarnDeprecated56("An access was made to an esv::Character instance after its lifetime has expired; this behavior is deprecated.");
	}

	return self;
}

void MakeLegacyServerCharacterObjectRef(lua_State* L, esv::Character* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<esv::Character>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
	} else {
		push(L, nullptr);
	}
}

END_NS()

namespace dse::esv::lua
{
	using namespace dse::lua;

	LifetimeHolder GetServerLifetime()
	{
		assert(gExtender->GetServer().IsInServerThread());
		return esv::ExtensionState::Get().GetLua()->GetCurrentLifetime();
	}

	LifetimePool& GetServerLifetimePool()
	{
		assert(gExtender->GetServer().IsInServerThread());
		return esv::ExtensionState::Get().GetLua()->GetLifetimePool();
	}

#include <Lua/Shared/LevelIteratorFunctions.inl>

	template <class Predicate>
	void GetTriggersGeneric(lua_State* L, FixedString const& requestedLevel, Predicate pred)
	{
		int index{ 1 };

		lua_newtable(L);
		FixedString levelName = requestedLevel;
		if (!levelName) {
			auto level = GetStaticSymbols().GetCurrentServerLevel();
			if (level == nullptr) {
				OsiError("No current level!");
				return;
			}

			levelName = level->LevelDesc->LevelName;
		}

		auto& helpers = GetEoCServer()->EntityManager->TriggerConversionHelpers;
		auto triggers = helpers.RegisteredTriggers.Find(levelName);
		if (triggers == nullptr) {
			OsiError("No triggers registered for level: " << levelName);
			return;
		}

		// FIXME - re-add when migrated to new proxy
		/*for (auto trigger : **triggers) {
			if (pred(trigger)) {
				auto guid = trigger->GetGuid();
				if (guid && *guid) {
					settable(L, index++, *guid);
				}
			}
		}*/
	}

	int GetAllTriggers(lua_State* L)
	{
		FixedString levelName;
		if (lua_gettop(L) >= 1) {
			levelName = get<FixedString>(L, 1);
		}

		GetTriggersGeneric(L, levelName, [](Trigger*) { return true; });
		return 1;
	}
}

#include <Lua/Server/ServerEntitySystem.inl>
#include <Lua/Server/ServerCharacter.inl>
#include <Lua/Server/ServerItem.inl>

namespace dse::lua
{
	using namespace dse::esv::lua;

	char const* const ObjectProxy<esv::Trigger>::MetatableName = "esv::Trigger";

	esv::Trigger* ObjectProxy<esv::Trigger>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;
		auto trigger = esv::GetEntityWorld()->GetTrigger(handle_);
		if (trigger == nullptr) luaL_error(L, "Trigger handle invalid");
		return static_cast<esv::Trigger*>(trigger);
	}

	int ObjectProxy<esv::Trigger>::Index(lua_State* L)
	{
		auto obj = Get(L);
		if (obj == nullptr) return luaL_error(L, "Trigger object no longer available");

		StackCheck _(L, 1);
		auto prop = get<FixedString>(L, 2);

		// FIXME - re-add when migrated to new proxy
		/*if (prop == GFS.strHandle) {
			ComponentHandle handle;
			obj->GetObjectHandle(handle);
			push(L, handle);
			return 1;
		}

		if (prop == GFS.strUUID) {
			push(L, *obj->GetGuid());
			return 1;
		}*/

		if (prop == GFS.strTriggerData) {
			auto atm = reinterpret_cast<esv::AtmosphereTrigger*>(obj);
			if (!obj->Template || !obj->Template->TriggerTypeData) {
				push(L, nullptr);
			} else if (obj->TriggerType == GFS.strTriggerAtmosphere) {
				auto triggerData = static_cast<AtmosphereTriggerData*>(obj->Template->TriggerTypeData);
				ObjectProxy<AtmosphereTriggerData>::New(L, GetServerLifetime(), triggerData);
			} else {
				LuaError("TriggerData for trigger type '" << obj->TriggerType << "' not supported yet!");
				push(L, nullptr);
			}

			return 1;
		}

		return GenericGetter(L, gTriggerPropertyMap);
	}

	int ObjectProxy<esv::Trigger>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gTriggerPropertyMap);
	}


	char const* const ObjectProxy<AtmosphereTriggerData>::MetatableName = "ls::AtmosphereTriggerData";

	AtmosphereTriggerData* ObjectProxy<AtmosphereTriggerData>::GetPtr(lua_State* L)
	{
		if (obj_ == nullptr) luaL_error(L, "AtmosphereTriggerData object no longer available");
		return obj_;
	}

	int ObjectProxy<AtmosphereTriggerData>::Index(lua_State* L)
	{
		if (obj_ == nullptr) return luaL_error(L, "AtmosphereTriggerData object no longer available");

		StackCheck _(L, 1);
		auto prop = get<FixedString>(L, 2);

		if (prop == GFS.strAtmospheres) {
			return LuaWrite(L, obj_->Atmospheres);
		} else if (prop == GFS.strFadeTime) {
			return LuaWrite(L, obj_->FadeTime);
		} else {
			push(L, nullptr);
			return 1;
		}
	}

	int ObjectProxy<AtmosphereTriggerData>::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		auto prop = get<FixedString>(L, 2);

		if (prop == GFS.strAtmospheres) {
			lua_pushvalue(L, 3);
			LuaRead(L, obj_->Atmospheres);
			lua_pop(L, 1);
		} else if (prop == GFS.strFadeTime) {
			lua_pushvalue(L, 3);
			LuaRead(L, obj_->FadeTime);
			lua_pop(L, 1);
		} else {
			LuaError("Unsupported atmosphere property: " << prop);
		}

		return 0;
	}


	char const* const ObjectProxy<SoundVolumeTriggerData>::MetatableName = "esv::SoundVolumeTriggerData";

	SoundVolumeTriggerData* ObjectProxy<SoundVolumeTriggerData>::GetPtr(lua_State* L)
	{
		if (obj_ == nullptr) luaL_error(L, "SoundVolumeTriggerData object no longer available");
		return obj_;
	}

	int ObjectProxy<SoundVolumeTriggerData>::Index(lua_State* L)
	{
		if (obj_ == nullptr) return luaL_error(L, "SoundVolumeTriggerData object no longer available");

		return GenericGetter(L, gSoundVolumeTriggerDataPropertyMap);
	}

	int ObjectProxy<SoundVolumeTriggerData>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gSoundVolumeTriggerDataPropertyMap);
	}


	char const* const ObjectProxy<eoc::ItemDefinition>::MetatableName = "eoc::ItemDefinition";

	eoc::ItemDefinition* ObjectProxy<eoc::ItemDefinition>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "ItemDefinition object has expired!");
		return nullptr;
	}

	int ItemDefinitionResetProgression(lua_State* L)
	{
		StackCheck _(L, 0);
		auto self = ObjectProxy<eoc::ItemDefinition>::CheckedGet(L, 2);
		self->LevelGroupIndex = -1;
		self->RootGroupIndex = -1;
		self->NameIndex = -1;
		self->NameCool = 0;
		return 0;
	}

	int ObjectProxy<eoc::ItemDefinition>::Index(lua_State* L)
	{
		StackCheck _(L, 1);
		auto prop = get<FixedString>(L, 2);
		if (prop == GFS.strResetProgression) {
			lua_pushcfunction(L, &ItemDefinitionResetProgression);
			return 1;
		}

		if (prop == GFS.strGenerationBoosts) {
			LuaWrite(L, Get(L)->GenerationBoosts);
			return 1;
		}

		if (prop == GFS.strRuneBoosts) {
			LuaWrite(L, Get(L)->RuneBoosts);
			return 1;
		}

		if (prop == GFS.strDeltaMods) {
			LuaWrite(L, Get(L)->DeltaMods);
			return 1;
		}

		return GenericGetter(L, gEoCItemDefinitionPropertyMap);
	}

	int ObjectProxy<eoc::ItemDefinition>::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		auto prop = get<FixedString>(L, 2);
		if (prop == GFS.strGenerationBoosts) {
			lua_pushvalue(L, 3);
			LuaRead(L, Get(L)->GenerationBoosts);
			lua_pop(L, 1);
			return 0;
		}

		if (prop == GFS.strRuneBoosts) {
			lua_pushvalue(L, 3);
			LuaRead(L, Get(L)->RuneBoosts);
			lua_pop(L, 1);
			return 0;
		}

		if (prop == GFS.strDeltaMods) {
			lua_pushvalue(L, 3);
			LuaRead(L, Get(L)->DeltaMods);
			lua_pop(L, 1);
			return 0;
		}

		return GenericSetter(L, gEoCItemDefinitionPropertyMap);
	}


	char const* const ObjectProxy<esv::Surface>::MetatableName = "esv::Surface";

	esv::Surface* ObjectProxy<esv::Surface>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;

		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level) {
			auto surface = level->SurfaceManager->Get(handle_);
			if (surface == nullptr) luaL_error(L, "Surface handle invalid");
			// FIXME - TEMP CAST
			return (esv::Surface*)surface;
		} else {
			return nullptr;
		}
	}

	int ObjectProxy<esv::Surface>::Index(lua_State* L)
	{
		auto surface = Get(L);
		auto prop = get<char const*>(L, 2);

		if (strcmp(prop, "RootTemplate") == 0) {
			auto tmpl = GetStaticSymbols().GetSurfaceTemplate(surface->SurfaceType);
			MakeObjectRef(L, tmpl);
			return 1;
		}

		return GenericGetter(L, gEsvSurfacePropertyMap);
	}

	int ObjectProxy<esv::Surface>::NewIndex(lua_State* L)
	{
		return GenericSetter(L, gEsvSurfacePropertyMap);
	}


	char const* const ObjectProxy<esv::SurfaceAction>::MetatableName = "esv::SurfaceAction";

	esv::SurfaceAction* ObjectProxy<esv::SurfaceAction>::GetPtr(lua_State* L)
	{
		if (obj_) return obj_;
		luaL_error(L, "SurfaceAction object not bound (maybe it was executed already?)");
		return nullptr;
	}

	LegacyPropertyMapBase& GetSurfaceActionPropertyMap(esv::SurfaceAction* action)
	{
		switch (action->VMT->GetTypeId(action)) {
		case SurfaceActionType::CreateSurfaceAction:
			return gEsvCreateSurfaceActionPropertyMap;
		case SurfaceActionType::CreatePuddleAction:
			return gEsvCreatePuddleActionPropertyMap;
		case SurfaceActionType::ExtinguishFireAction:
			return gEsvExtinguishFireActionPropertyMap;
		case SurfaceActionType::ZoneAction:
			return gEsvZoneActionPropertyMap;
		case SurfaceActionType::TransformSurfaceAction:
			return gEsvTransformSurfaceActionPropertyMap;
		case SurfaceActionType::ChangeSurfaceOnPathAction:
			return gEsvChangeSurfaceOnPathActionPropertyMap;
		case SurfaceActionType::RectangleSurfaceAction:
			return gEsvRectangleSurfaceActionPropertyMap;
		case SurfaceActionType::PolygonSurfaceAction:
			return gEsvPolygonSurfaceActionPropertyMap;
		case SurfaceActionType::SwapSurfaceAction:
			return gEsvSwapSurfaceActionPropertyMap;
		default:
			OsiError("No property map found for this surface type!");
			return gEsvSurfaceActionPropertyMap;
		}
	}

	int ObjectProxy<esv::SurfaceAction>::Index(lua_State* L)
	{
		auto action = Get(L);
		if (!action) return 0;

		return GenericGetter(L, GetSurfaceActionPropertyMap(action));
	}

	int ObjectProxy<esv::SurfaceAction>::NewIndex(lua_State* L)
	{
		StackCheck _(L, 0);
		auto action = Get(L);

		auto prop = luaL_checkstring(L, 2);

		if (strcmp(prop, "DamageList") == 0) {
			// FIXME
			luaL_error(L, "DamageList not supported yet!");
			/*
			auto& damageList = DamageList::CheckUserData(L, 3)->Get();
			switch (action->VMT->GetTypeId(action)) {
			case SurfaceActionType::RectangleSurfaceAction:
			{
				auto act = static_cast<esv::RectangleSurfaceAction*>(action);
				act->DamageList.CopyFrom(damageList);
				break;
			}

			case SurfaceActionType::PolygonSurfaceAction:
			{
				auto act = static_cast<esv::PolygonSurfaceAction*>(action);
				act->DamageList.CopyFrom(damageList);
				break;
			}

			case SurfaceActionType::ZoneAction:
			{
				auto act = static_cast<esv::ZoneAction*>(action);
				act->DamageList.CopyFrom(damageList);
				break;
			}

			default:
				OsiError("This surface action type doesn't have a DamageList!");
			}
			*/

			return 0;
		}

		if (strcmp(prop, "Vertices") == 0) {
			if (action->VMT->GetTypeId(action) == SurfaceActionType::PolygonSurfaceAction) {
				auto act = static_cast<esv::PolygonSurfaceAction*>(action);
				act->PolygonVertices.Clear();
				luaL_checktype(L, 3, LUA_TTABLE);
				for (auto idx : iterate(L, 3)) {
					auto vec2 = get<glm::vec2>(L, idx);
					act->PolygonVertices.Add(vec2);
				}
			} else {
				OsiError("Vertices only supported for surface action type PolygonSurfaceAction!");
			}

			return 0;
		}

		auto const& propertyMap = GetSurfaceActionPropertyMap(action);
		return GenericSetter(L, propertyMap);
	}
}

namespace dse::esv::lua
{
	using namespace dse::lua;

	char const * const TurnManagerCombatProxy::MetatableName = "esv::TurnManager::Combat";

	void TurnManagerCombatProxy::PopulateMetatable(lua_State * L)
	{
		lua_newtable(L);

		lua_pushcfunction(L, &GetCurrentTurnOrder);
		lua_setfield(L, -2, "GetCurrentTurnOrder");

		lua_pushcfunction(L, &GetNextTurnOrder);
		lua_setfield(L, -2, "GetNextTurnOrder");

		lua_pushcfunction(L, &UpdateCurrentTurnOrder);
		lua_setfield(L, -2, "UpdateCurrentTurnOrder");

		lua_pushcfunction(L, &UpdateNextTurnOrder);
		lua_setfield(L, -2, "UpdateNextTurnOrder");

		lua_pushcfunction(L, &GetAllTeams);
		lua_setfield(L, -2, "GetAllTeams");

		lua_setfield(L, -2, "__index");
	}

	void CombatTeamListToLua(lua_State * L, CompactSet<esv::TurnManager::CombatTeam *, MSVCMemoryAllocator> const & teams)
	{
		lua_newtable(L);

		for (uint32_t i = 0; i < teams.Size; i++) {
			push(L, i + 1);
			TurnManagerTeamProxy::New(L, teams[i]->TeamId);
			lua_settable(L, -3);
		}
	}

	int TurnManagerCombatProxy::Index(lua_State * L)
	{
		auto combat = Get();
		if (combat == nullptr) return luaL_error(L, "Combat no longer available");

		StackCheck _(L, 1);
		auto prop = luaL_checkstring(L, 2);
		if (strcmp(prop, "CombatId") == 0) {
			push(L, combatId_);
		} else if (strcmp(prop, "LevelName") == 0) {
			push(L, combat->LevelName.Str);
		} else if (strcmp(prop, "IsActive") == 0) {
			push(L, combat->IsActive);
		} else {
			OsiError("Combat has no attribute named " << prop);
			push(L, nullptr);
		}

		return 1;
	}

	int TurnManagerCombatProxy::GetCurrentTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		StackCheck _(L, 1);
		CombatTeamListToLua(L, combat->CurrentRoundTeams);
		return 1;
	}

	int TurnManagerCombatProxy::GetNextTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		StackCheck _(L, 1);
		CombatTeamListToLua(L, combat->NextRoundTeams);
		return 1;
	}

	void UpdateTurnOrder(lua_State * L, uint8_t combatId, int index,
		ObjectSet<esv::TurnManager::CombatTeam *, MSVCMemoryAllocator> & combatTeams,
		ObjectSet<eoc::CombatTeamId> & combatNotifies)
	{
		if (lua_type(L, index) != LUA_TTABLE) luaL_error(L, "UpdateTurnOrder() expects a table of teams as the first argument");

		std::vector<esv::TurnManager::CombatTeam *> teams;
		std::unordered_set<eoc::CombatTeamId> notifies;

		lua_pushnil(L);
		if (index < 0) index--;

		while (lua_next(L, index) != 0) {
			auto luaTeam = TurnManagerTeamProxy::CheckUserData(L, -1);
			if (luaTeam->TeamId().CombatId != combatId) {
				luaL_error(L, "Tried to add team from another combat");
			}

			auto team = luaTeam->Get();
			if (team == nullptr) {
				luaL_error(L, "Tried to add nonexistent team");
			}

			teams.push_back(team);
			notifies.insert(team->TeamId);
			lua_pop(L, 1);
		}

		for (auto team : combatTeams) {
			notifies.insert(team->TeamId);
		}

		for (auto notify : combatNotifies) {
			notifies.insert(notify);
		}

		combatTeams.Size = 0;
		for (auto const & team : teams) {
			combatTeams.Add(team);
		}

		combatNotifies.Size = 0;
		for (auto const & teamId : notifies) {
			combatNotifies.Add(teamId);
		}

		auto protocol = GetTurnBasedProtocol();
		if (protocol != nullptr) {
			// FIXME - filter duplicates
			protocol->UpdatedTurnOrderCombatIds.Add(combatId);
		}
	}

	int TurnManagerCombatProxy::UpdateCurrentTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		StackCheck _(L, 0);
		UpdateTurnOrder(L, self->combatId_, 2, combat->CurrentRoundTeams, combat->CurrentTurnChangeNotificationTeamIds);
		return 0;
	}

	int TurnManagerCombatProxy::UpdateNextTurnOrder(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		StackCheck _(L, 0);
		UpdateTurnOrder(L, self->combatId_, 2, combat->NextRoundTeams, combat->NextTurnChangeNotificationTeamIds);
		return 0;
	}

	int TurnManagerCombatProxy::GetAllTeams(lua_State * L)
	{
		auto self = TurnManagerCombatProxy::CheckUserData(L, 1);
		auto combat = self->Get();
		if (!combat) return 0;

		StackCheck _(L, 1);
		lua_newtable(L);

		uint32_t i = 1;
		for (auto const& team : combat->Teams) {
			push(L, i++);
			TurnManagerTeamProxy::New(L, eoc::CombatTeamId(team.Key));
			lua_settable(L, -3);
		}

		return 1;
	}


	char const * const TurnManagerTeamProxy::MetatableName = "esv::TurnManager::Team";

	int TurnManagerTeamProxy::Index(lua_State * L)
	{
		auto team = Get();
		if (team == nullptr) return luaL_error(L, "Team no longer available");

		StackCheck _(L, 1);
		auto prop = luaL_checkstring(L, 2);
		if (strcmp(prop, "TeamId") == 0) {
			push(L, (uint32_t)team->TeamId);
		} else if (strcmp(prop, "CombatId") == 0) {
			push(L, team->TeamId.CombatId);
		} else if (strcmp(prop, "Initiative") == 0) {
			push(L, (uint32_t)team->Initiative);
		} else if (strcmp(prop, "StillInCombat") == 0) {
			push(L, team->StillInCombat);
			// TODO - fetching CombatGroup?
		} else if (strcmp(prop, "Character") == 0) {
			auto character = team->EntityWrapper.GetCharacter();
			MakeLegacyServerCharacterObjectRef(L, character);
		} else if (strcmp(prop, "Item") == 0) {
			auto item = team->EntityWrapper.GetItem();
			MakeObjectRef(L, item);
		} else {
			OsiError("Combat team has no attribute named " << prop);
			push(L, nullptr);
		}

		return 1;
	}


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

			OsiError("ItemConstructor has no property named " << func.Str);
			push(L, nullptr);
			return 1;
		} else {
			auto idx = get<int>(L, 2);
			if (idx < 1 || idx > (int)definition_.Size) {
				return luaL_error(L, "Clone set only has %d elements", definition_.Size);
			}

			ObjectProxy<eoc::ItemDefinition>::New(L, GetServerLifetime(), &definition_[idx - 1]);
			return 1;
		}
	}


	void ExtensionLibraryServer::Register(lua_State * L)
	{
		ExtensionLibrary::Register(L);

		ObjectProxy<eoc::ItemDefinition>::RegisterMetatable(L);
		ObjectProxy<esv::Trigger>::RegisterMetatable(L);
		ObjectProxy<AtmosphereTriggerData>::RegisterMetatable(L);
		ObjectProxy<SoundVolumeTriggerData>::RegisterMetatable(L);
		ObjectProxy<esv::Surface>::RegisterMetatable(L);
		ObjectProxy<esv::SurfaceAction>::RegisterMetatable(L);

		OsiFunctionNameProxy::RegisterMetatable(L);
		TurnManagerCombatProxy::RegisterMetatable(L);
		TurnManagerTeamProxy::RegisterMetatable(L);
		ItemConstructor::RegisterMetatable(L);
		RegisterNameResolverMetatable(L);
		CreateNameResolver(L);
	}
	
	esv::Character* GetCharacter(lua_State* L, int index)
	{
		esv::Character* character = nullptr;
		switch (lua_type(L, index)) {
		case LUA_TLIGHTUSERDATA:
		{
			auto handle = get<ComponentHandle>(L, index);
			if (handle.GetType() == (uint32_t)ObjectType::ClientCharacter) {
				OsiError("Attempted to resolve client ComponentHandle on the server");
			} else {
				character = GetEntityWorld()->GetCharacter(handle);
			}
			break;
		}

		case LUA_TNUMBER:
		{
			auto value = lua_tointeger(L, index);
			if (value > 0xffffffff) {
				OsiError("Resolving integer object handles is deprecated since v52!")
				ComponentHandle handle{ value };
				if (handle.GetType() == (uint32_t)ObjectType::ClientCharacter) {
					OsiError("Attempted to resolve client ComponentHandle on the server");
				} else {
					character = GetEntityWorld()->GetCharacter(handle);
				}
			} else {
				NetId netId{ (uint32_t)value };
				character = GetEntityWorld()->GetCharacter(netId);
			}
			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, index);
			character = GetEntityWorld()->GetCharacter(guid);
			break;
		}

		default:
			OsiError("Expected character UUID, Handle or NetId; got " << lua_typename(L, lua_type(L, 1)));
			break;
		}

		return character;
	}

	int GetCharacter(lua_State* L)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve character handle in restricted context");
		}

		StackCheck _(L, 1);
		esv::Character* character = GetCharacter(L, 1);
		MakeLegacyServerCharacterObjectRef(L, character);
		return 1;
	}

	int GetItem(lua_State* L)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve item handle in restricted context");
		}

		StackCheck _(L, 1);
		esv::Item* item = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TLIGHTUSERDATA:
		{
			auto handle = get<ComponentHandle>(L, 1);
			item = GetEntityWorld()->GetItem(handle);
			break;
		}

		case LUA_TNUMBER:
		{
			auto value = lua_tointeger(L, 1);
			if (value > 0xffffffff) {
				OsiError("Resolving integer object handles is deprecated since v52!")
				ComponentHandle handle{ value };
				if (handle.GetType() == (uint32_t)ObjectType::ClientItem) {
					OsiError("Attempted to resolve client ComponentHandle on the server");
				} else {
					item = GetEntityWorld()->GetItem(handle);
				}
			} else {
				NetId netId{ (uint32_t)value };
				item = GetEntityWorld()->GetItem(netId);
			}
			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, 1);
			item = GetEntityWorld()->GetItem(guid);
			break;
		}

		default:
			OsiError("Expected item GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
			push(L, nullptr);
			return 1;
		}

		MakeObjectRef(L, item);
		return 1;
	}

	int GetTrigger(lua_State* L)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve trigger handle in restricted context");
		}

		StackCheck _(L, 1);
		Trigger* trigger = nullptr;
		switch (lua_type(L, 1)) {
		case LUA_TLIGHTUSERDATA:
		{
			auto handle = get<ComponentHandle>(L, 1);
			trigger = GetEntityWorld()->GetTrigger(handle);
			break;
		}

		case LUA_TSTRING:
		{
			auto guid = lua_tostring(L, 1);
			trigger = GetEntityWorld()->GetTrigger(guid);
			break;
		}

		default:
			OsiError("Expected trigger GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
			push(L, nullptr);
			return 1;
		}

		if (trigger != nullptr) {
			// FIXME - re-add when migrated to new proxy
			/*ComponentHandle handle;
			trigger->GetObjectHandle(handle);
			ObjectProxy<Trigger>::New(L, handle);*/
			push(L, nullptr);
		} else {
			push(L, nullptr);
		}

		return 1;
	}

	IEoCServerObject* GetGameObjectInternal(ComponentHandle const& handle)
	{
		if (!handle) {
			return nullptr;
		}

		switch ((ObjectType)handle.GetType()) {
		case ObjectType::ServerCharacter:
			return GetEntityWorld()->GetCharacter(handle);

		case ObjectType::ServerItem:
			return GetEntityWorld()->GetItem(handle);

		case ObjectType::ServerProjectile:
			return GetEntityWorld()->GetProjectile(handle);

		// FIXME - re-add when migrated to new proxy
		/*case ObjectType::ServerEocPointTrigger:
		case ObjectType::ServerEocAreaTrigger:
		case ObjectType::ServerStartTrigger:
		case ObjectType::ServerTeleportTrigger:
		case ObjectType::ServerEventTrigger:
		case ObjectType::ServerCrimeAreaTrigger:
		case ObjectType::ServerCrimeRegionTrigger:
		case ObjectType::ServerAtmosphereTrigger:
		case ObjectType::ServerAIHintAreaTrigger:
		case ObjectType::ServerMusicVolumeTrigger:
		case ObjectType::ServerSecretRegionTrigger:
		case ObjectType::ServerStatsAreaTrigger:
		case ObjectType::ServerSoundVolumeTrigger:
		case ObjectType::ServerRegionTrigger:
		case ObjectType::ServerExplorationTrigger:
			return GetEntityWorld()->GetTrigger(handle);*/

		default:
			OsiError("Cannot resolve unsupported server handle type: " << handle.GetType());
			return nullptr;
		}
	}

	IEoCServerObject* GetGameObjectInternal(char const* nameGuid)
	{
		auto character = GetEntityWorld()->GetCharacter(nameGuid, false);
		if (character) {
			return character;
		}

		auto item = GetEntityWorld()->GetItem(nameGuid, false);
		if (item) {
			return item;
		}

		// FIXME - re-add when migrated to new proxy
		/*auto trigger = GetEntityWorld()->GetTrigger(nameGuid, false);
		if (trigger) {
			return trigger;
		}*/

		return nullptr;
	}

	IEoCServerObject* GetGameObjectInternal(lua_State* L, int idx)
	{
		switch (lua_type(L, idx)) {
		case LUA_TLIGHTUSERDATA:
		{
			auto handle = get<ComponentHandle>(L, idx);
			if (handle) {
				return GetGameObjectInternal(handle);
			} else {
				return nullptr;
			}

			break;
		}

		case LUA_TNUMBER:
		{
			OsiError("Resolving integer object handles is deprecated since v52!")
			auto handle = ComponentHandle(lua_tointeger(L, idx));
			if (handle) {
				return GetGameObjectInternal(handle);
			} else {
				return nullptr;
			}

			break;
		}

		case LUA_TSTRING:
		{
			auto guid = get<char const*>(L, idx);
			return GetGameObjectInternal(guid);
		}

		default:
			OsiError("Expected object GUID or handle, got " << lua_typename(L, lua_type(L, idx)));
			return nullptr;
		}
	}

	int GetGameObject(lua_State* L)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve game object handle in restricted context");
		}

		StackCheck _(L, 1);
		auto result = GetGameObjectInternal(L, 1);
		MakeObjectRef(L, result);
		return 1;
	}

	int GetStatus(lua_State* L)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve status handle in restricted context");
		}

		esv::Character* character = GetCharacter(L, 1);
		if (character == nullptr) return 0;

		StackCheck _(L, 1);
		esv::Status* status;
		if (lua_type(L, 2) == LUA_TLIGHTUSERDATA) {
			auto statusHandle = get<ComponentHandle>(L, 2);
			status = character->GetStatus(statusHandle, true);
			if (status != nullptr) {
				MakeObjectRef(L, status);
				return 1;
			}

			OsiError("Character has no status with ComponentHandle 0x" << std::hex << statusHandle.Handle);
		} else {
			auto index = lua_tointeger(L, 2);

			// We need to keep integer status handle support since some extender Osiris events
			// (eg. NRD_OnHit, NRD_OnPrepareHit, etc.) use these handles and Osiris doesn't support lightuserdata
			if (index > 0xffffffff) {
				ComponentHandle statusHandle{ index };
				status = character->GetStatus(statusHandle, true);
				if (status != nullptr) {
					MakeObjectRef(L, status);
					return 1;
				}

				OsiError("Character has no status with ComponentHandle 0x" << std::hex << statusHandle.Handle);
			} else {
				NetId statusNetId{ (uint32_t)index };
				status = character->GetStatus(statusNetId);
				if (status != nullptr) {
					MakeObjectRef(L, status);
					return 1;
				}

				OsiError("Character has no status with NetId 0x" << std::hex << index);
			}
		}

		push(L, nullptr);
		return 1;
	}

	int GetCombat(lua_State* L)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve combat ID in restricted context");
		}

		auto turnMgr = GetEntityWorld()->GetTurnManager();
		if (turnMgr == nullptr) {
			OsiErrorS("esv::TurnManager not available");
			return 0;
		}

		auto combatId = (uint8_t)luaL_checkinteger(L, 1);
		auto combat = turnMgr->Combats.Find(combatId);
		if (combat == nullptr) {
			OsiError("No combat found with ID " << (unsigned)combatId);
			return 0;
		}

		TurnManagerCombatProxy::New(L, combatId);
		return 1;
	}

	int GetSurface(lua_State* L)
	{
		auto lua = State::FromLua(L);
		if (lua->RestrictionFlags & State::RestrictHandleConversion) {
			return luaL_error(L, "Attempted to resolve item handle in restricted context");
		}

		auto handle = get<ComponentHandle>(L, 1);

		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (!level || !level->SurfaceManager) {
			OsiError("Current level not available yet!");
			return 0;
		}

		auto surface = level->SurfaceManager->Get(handle);
		if (surface != nullptr) {
			ObjectProxy<esv::Surface>::New(L, handle);
			return 1;
		} else {
			return 0;
		}
	}

	int GetAiGrid(lua_State* L)
	{
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (!level || !level->AiGrid) {
			OsiError("Current level not available yet!");
			return 0;
		}

		ObjectProxy<eoc::AiGrid>::New(L, GetServerLifetime(), level->AiGrid);
		return 1;
	}

	int GetCurrentLevelData(lua_State* L)
	{
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (!level || !level->LevelDesc) {
			OsiError("Current level not available yet!");
			return 0;
		}

		lua_newtable(L);
		settable(L, "LevelName", level->LevelDesc->LevelName);
		settable(L, "UniqueKey", level->LevelDesc->UniqueKey);
		settable(L, "CustomDisplayLevelName", level->LevelDesc->CustomDisplayLevelName);
		settable(L, "Type", level->LevelDesc->Type);
		return 1;
	}

	int GetSurfaceTransformRules(lua_State* L)
	{
		auto rules = GetStaticSymbols().eoc__SurfaceTransformActionsFromType;
		if (rules == nullptr) {
			OsiError("Surface transform rules not available!");
			return 0;
		}

		StackCheck _(L, 1);
		lua_newtable(L);

		for (auto i = 0; i < std::size(rules->SurfaceTypes); i++) {
			auto const& interactionSet = rules->SurfaceTypes[i].Interactions;

			if ((uint64_t)interactionSet[0].ActionableSurfaces[0] == 0) continue;

			lua_newtable(L);

			auto interactionIdx = 1;
			for (auto const& interaction : interactionSet) {
				if ((uint64_t)interaction.ActionableSurfaces[0] == 0) break;

				push(L, interactionIdx++);
				lua_newtable(L);

				setfield(L, "TransformType", interaction.ActionType);

				lua_newtable(L);
				int surfIdx = 1;
				for (auto const& surface : interaction.ActionableSurfaces) {
					if ((uint64_t)surface == 0) break;

					push(L, surfIdx++);
					push_flags(L, surface);
					lua_settable(L, -3);
				}

				lua_setfield(L, -2, "ActionableSurfaces");

				lua_settable(L, -3);
			}

			lua_setfield(L, -2, EnumInfo<SurfaceType>::Find((SurfaceType)i).Str);
		}

		return 1;
	}

	void UpdateSurfaceTransformInteraction(lua_State* L, int idx, eoc::SurfaceTransformInteractions::Interaction& interaction)
	{
		luaL_checktype(L, idx, LUA_TTABLE);
		auto actionType = checked_getfield<SurfaceTransformActionType>(L, "TransformType", idx);
		interaction.ActionType = actionType;

		lua_getfield(L, idx, "ActionableSurfaces");
		luaL_checktype(L, -1, LUA_TTABLE);
		int curActionIdx = 0;
		for (auto surfaceIdx : iterate(L, -1)) {
			if (curActionIdx >= std::size(interaction.ActionableSurfaces) - 1) {
				luaL_error(L, "At most %d actionable surfaces per transform type are supported", std::size(interaction.ActionableSurfaces) - 1);
			}

			auto surfaceFlags = checked_get_flags<ESurfaceFlag>(L, surfaceIdx);
			interaction.ActionableSurfaces[curActionIdx++] = surfaceFlags;
		}

		lua_pop(L, 1);
	}

	int UpdateSurfaceTransformRules(lua_State* L)
	{
		StackCheck _(L, 0);
		auto rules = GetStaticSymbols().eoc__SurfaceTransformActionsFromType;
		if (rules == nullptr) {
			OsiError("Surface transform rules not available!");
			return 0;
		}

		auto interactions = std::make_unique<eoc::SurfaceTransformInteractions>();
		memset(interactions.get(), 0, sizeof(eoc::SurfaceTransformInteractions));

		luaL_checktype(L, 1, LUA_TTABLE);
		for (auto idx : iterate(L, 1)) {
			auto surfaceType = get<SurfaceType>(L, idx - 1);
			auto& surfaceRules = interactions->SurfaceTypes[(unsigned)surfaceType].Interactions;

			luaL_checktype(L, -1, LUA_TTABLE);
			int curInteractionIdx = 0;
			for (auto interactionIdx : iterate(L, -1)) {
				if (curInteractionIdx >= std::size(surfaceRules) - 1) {
					luaL_error(L, "At most %d rules per surface are supported", std::size(surfaceRules) - 1);
				}

				UpdateSurfaceTransformInteraction(L, interactionIdx, surfaceRules[curInteractionIdx++]);
			}

		}

		*rules = *interactions;
		return 0;
	}

	int CreateSurfaceAction(lua_State* L)
	{
		auto type = get<SurfaceActionType>(L, 1);

		auto const& sym = GetStaticSymbols();
		if (!sym.esv__SurfaceActionFactory || !*sym.esv__SurfaceActionFactory || !sym.esv__SurfaceActionFactory__CreateAction) {
			OsiError("esv::SurfaceActionFactory not mapped!");
			return 0;
		}

		auto action = sym.esv__SurfaceActionFactory__CreateAction(*sym.esv__SurfaceActionFactory, type, 0);
		if (!action) {
			OsiError("Couldn't create surface action for some reason.");
			return 0;
		}

		ObjectProxy<SurfaceAction>::New(L, GetServerLifetime(), action);
		return 1;
	}

	int ExecuteSurfaceAction(lua_State* L)
	{
		StackCheck _(L, 0);
		auto action = ObjectProxy<SurfaceAction>::CheckUserData(L, 1);
		if (!action->Get(L)) {
			OsiError("Attempted to execute surface action more than once!");
			return 0;
		}

		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (!level) {
			OsiError("Attempted to execute surface action while no level is loaded!");
			return 0;
		}

		auto surfaceAction = action->Get(L);
		surfaceAction->Level = level;

		if (surfaceAction->VMT->GetTypeId(surfaceAction) == SurfaceActionType::TransformSurfaceAction) {
			auto transform = reinterpret_cast<TransformSurfaceAction*>(surfaceAction);
			GetStaticSymbols().esv__TransformSurfaceAction__Init(
				transform, transform->SurfaceTransformAction, transform->SurfaceLayer, transform->OriginSurface
			);
		}

		surfaceAction->VMT->Enter(surfaceAction);
		level->SurfaceManager->SurfaceActions.Add(surfaceAction);
		action->Unbind();

		return 0;
	}

	int CancelSurfaceAction(lua_State* L)
	{
		StackCheck _(L, 0);
		auto handle = get<ComponentHandle>(L, 1);
		
		if (handle.GetType() != (uint32_t)ObjectType::ServerSurfaceAction) {
			OsiError("Expected a surface action handle, got type " << handle.GetType());
			return 0;
		}

		auto factory = GetStaticSymbols().esv__SurfaceActionFactory;
		if (!factory || !*factory) {
			OsiError("SurfaceActionFactory not mapped!");
			return 0;
		}

		// FIXME - TEMP CAST
		auto action = (esv::SurfaceAction*)(*factory)->Get(handle);
		if (!action) {
			OsiWarn("No surface action found with handle " << std::hex << handle.Handle << "; maybe it already expired?");
			return 0;
		}

		switch (action->VMT->GetTypeId(action)) {
		case SurfaceActionType::CreateSurfaceAction:
		{
			auto act = static_cast<esv::CreateSurfaceAction*>(action);
			act->CurrentCellCount = act->SurfaceCells.Size;
			break;
		}
		case SurfaceActionType::CreatePuddleAction:
		{
			auto act = static_cast<esv::CreatePuddleAction*>(action);
			act->IsFinished = true;
			break;
		}
		case SurfaceActionType::ExtinguishFireAction:
		{
			auto act = static_cast<esv::ExtinguishFireAction*>(action);
			act->Percentage = 0.0f;
			break;
		}
		case SurfaceActionType::ZoneAction:
		{
			auto act = static_cast<esv::ZoneAction*>(action);
			act->CurrentCellCount = act->SurfaceCells.Size;
			break;
		}
		case SurfaceActionType::ChangeSurfaceOnPathAction:
		{
			auto act = static_cast<esv::ChangeSurfaceOnPathAction*>(action);
			act->IsFinished = true;
			break;
		}
		case SurfaceActionType::RectangleSurfaceAction:
		{
			auto act = static_cast<esv::RectangleSurfaceAction*>(action);
			act->CurrentCellCount = act->SurfaceCells.Size;
			break;
		}
		case SurfaceActionType::PolygonSurfaceAction:
		{
			auto act = static_cast<esv::PolygonSurfaceAction*>(action);
			act->LastSurfaceCellCount = act->SurfaceCells.Size;
			break;
		}
		case SurfaceActionType::SwapSurfaceAction:
		{
			auto act = static_cast<esv::SwapSurfaceAction*>(action);
			act->CurrentCellCount = act->SurfaceCells.Size;
			break;
		}
		case SurfaceActionType::TransformSurfaceAction:
		{
			auto act = static_cast<esv::TransformSurfaceAction*>(action);
			act->Finished = true;
			break;
		}
		default:
			OsiError("CancelSurfaceAction() not implemented for this surface action type!");
		}

		return 0;
	}

	int ExecuteSkillPropertiesOnTarget(lua_State* L)
	{
		StackCheck _(L, 0);
		auto skillId = get<FixedString>(L, 1);
		auto attacker = GetCharacter(L, 2);
		auto target = GetCharacter(L, 3);
		auto position = get<glm::vec3>(L, 4);
		auto propertyContext = get<stats::PropertyContext>(L, 5);
		auto isFromItem = get<bool>(L, 6);

		stats::SkillPrototype* skillProto{ nullptr };
		auto skillProtoMgr = GetStaticSymbols().eoc__SkillPrototypeManager;
		if (skillProtoMgr && *skillProtoMgr) {
			auto proto = (*skillProtoMgr)->Prototypes.Find(skillId);
			if (*proto) {
				skillProto = *proto;
			}
		}

		if (!skillProto) {
			LuaError("Couldn't find skill prototype for " << skillId);
			return 0;
		}

		auto exec = GetStaticSymbols().esv__ExecuteCharacterSetExtraProperties;
		auto skillProperties = skillProto->GetStats()->PropertyLists.Find(GFS.strSkillProperties);

		if (!skillProperties) {
			LuaError("Skill " << skillId << " has no SkillProperties!");
			return 0;
		}

		if (!attacker || !target || !exec) {
			return 0;
		}

		ComponentHandle attackerHandle;
		attacker->GetObjectHandle(attackerHandle);
		ObjectSet<esv::Character*> targets;
		targets.Add(target);

		exec(*skillProperties, attackerHandle.Handle, targets, position, propertyContext, isFromItem,
			skillProto, nullptr, 0.0f, nullptr, false, 2.4f);
		return 0;
	}

	int ExecuteSkillPropertiesOnPosition(lua_State* L)
	{
		StackCheck _(L, 0);
		auto skillId = get<FixedString>(L, 1);
		auto attacker = GetCharacter(L, 2);
		auto position = get<glm::vec3>(L, 3);
		auto radius = get<float>(L, 4);
		auto propertyContext = get<stats::PropertyContext>(L, 5);
		auto isFromItem = get<bool>(L, 6);

		stats::SkillPrototype* skillProto{ nullptr };
		auto skillProtoMgr = GetStaticSymbols().eoc__SkillPrototypeManager;
		if (skillProtoMgr && *skillProtoMgr) {
			auto proto = (*skillProtoMgr)->Prototypes.Find(skillId);
			if (*proto) {
				skillProto = *proto;
			}
		}

		if (!skillProto) {
			LuaError("Couldn't find skill prototype for " << skillId);
			return 0;
		}

		auto exec = GetStaticSymbols().esv__ExecutePropertyDataOnPositionOnly;
		auto skillProperties = skillProto->GetStats()->PropertyLists.Find(GFS.strSkillProperties);

		if (!skillProperties) {
			LuaError("Skill " << skillId << " has no SkillProperties!");
			return 0;
		}

		if (!attacker || !exec) {
			return 0;
		}

		ComponentHandle attackerHandle;
		attacker->GetObjectHandle(attackerHandle);

		exec(*skillProperties, attackerHandle.Handle, &position, radius, propertyContext, isFromItem, skillProto, nullptr, 2.4f);
		return 0;
	}

	int GetGameState(lua_State* L)
	{
		auto state = GetStaticSymbols().GetServerState();
		if (state) {
			push(L, *state);
		} else {
			lua_pushnil(L);
		}

		return 1;
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
			auto item = ObjectProxy2::CheckedGet<esv::Item>(L, 1);
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

		auto gameObj = GetGameObjectInternal(L, 1);
		auto statusId = get<FixedString>(L, 2);
		float lifeTime = get<float>(L, 3);

		if (!gameObj) {
			OsiError("Attempted to prepare status on nonexistent game object!");
			push(L, nullptr);
			return 1;
		}

		auto statusMachine = gameObj->GetStatusMachine();
		if (!statusMachine) {
			push(L, nullptr);
			return 1;
		}

		auto status = gExtender->GetStatusHelpers().PrepareStatus(statusMachine, statusId, lifeTime);
		if (!status) {
			push(L, nullptr);
			return 1;
		}

		MakeObjectRef(L, status);
		return 1;
	}

	int ApplyStatus(lua_State* L)
	{
		auto status = ObjectProxy2::CheckedGet<esv::Status>(L, 1);
		StatusMachine* statusMachine{ nullptr };

		auto ownerHandle = status->OwnerHandle;
		if (ownerHandle.GetType() == (uint32_t)ObjectType::ServerCharacter) {
			auto character = GetEntityWorld()->GetCharacter(ownerHandle);
			if (character) {
				statusMachine = character->StatusMachine;
			}
		} else if (ownerHandle.GetType() == (uint32_t)ObjectType::ServerItem) {
			auto item = GetEntityWorld()->GetItem(ownerHandle);
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
			{"ExecuteSkillPropertiesOnTarget", ExecuteSkillPropertiesOnTarget},
			{"ExecuteSkillPropertiesOnPosition", ExecuteSkillPropertiesOnPosition},

			{"GetSurfaceTransformRules", GetSurfaceTransformRules},
			{"UpdateSurfaceTransformRules", UpdateSurfaceTransformRules},
			{"CreateSurfaceAction", CreateSurfaceAction},
			{"ExecuteSurfaceAction", ExecuteSurfaceAction},
			{"CancelSurfaceAction", CancelSurfaceAction},

			{"GetAllCharacters", GetAllCharacters},
			{"GetCharactersAroundPosition", GetCharactersAroundPosition},
			{"GetAllItems", GetAllItems},
			{"GetItemsAroundPosition", GetItemsAroundPosition},
			{"CreateItemConstructor", CreateItemConstructor},
			{"GetAllTriggers", GetAllTriggers},

			{"PrepareStatus", PrepareStatus},
			{"ApplyStatus", ApplyStatus},

			{"GetCharacter", GetCharacter},
			{"GetItem", GetItem},
			{"GetTrigger", GetTrigger},
			{"GetGameObject", GetGameObject},
			{"GetStatus", GetStatus},
			{"GetCombat", GetCombat},
			{"GetSurface", GetSurface},
			{"GetAiGrid", GetAiGrid},
			{"GetCurrentLevelData", GetCurrentLevelData},
			{"NewDamageList", NewDamageList},
			{"GetSurfaceTemplate", GetSurfaceTemplate},

			{"GetGameState", GetGameState},

			{0,0}
		};

		luaL_newlib(L, extLib); // stack: lib
		lua_setglobal(L, "Ext"); // stack: -

		RegisterSharedLibraries(L);
		RegisterServerLibraries(L);
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

	void ServerState::Initialize()
	{
		StackCheck _(L, 0);

		library_.Register(L);

		gExtender->GetClient().GetExtensionState().LuaLoadBuiltinFile("ServerStartup.lua");

		lua_getglobal(L, "Ext"); // stack: Ext
		StatsExtraDataProxy::New(L); // stack: Ext, "ExtraData", ExtraDataProxy
		lua_setfield(L, -2, "ExtraData"); // stack: Ext
		lua_pop(L, 1); // stack: -

		// Ext is not writeable after loading SandboxStartup!
		gExtender->GetClient().GetExtensionState().LuaLoadBuiltinFile("SandboxStartup.lua");

#if !defined(OSI_NO_DEBUGGER)
		auto debugger = gExtender->GetLuaDebugger();
		if (debugger) {
			debugger->ServerStateCreated(this);
		}
#endif
	}


	std::optional<int32_t> ServerState::StatusGetEnterChance(esv::Status* status, bool isEnterCheck)
	{
		StatusGetEnterChanceEventParams params{ status, isEnterCheck };
		ThrowEvent(*this, "StatusGetEnterChance", params);
		return params.EnterChance;
	}

	void ServerState::OnStatusHitEnter(esv::StatusHit* hit, PendingHit* context)
	{
		StatusHitEnterEventParams params{ hit, context };
		ThrowEvent(*this, "StatusHitEnter", params);
	}

	bool ServerState::ComputeCharacterHit(stats::Character * target,
		stats::Character *attacker, stats::Item *weapon, stats::DamagePairList *damageList,
		stats::HitType hitType, bool noHitRoll, bool forceReduceDurability, stats::HitDamageInfo *hit,
		stats::PropertyList *skillProperties, stats::HighGroundBonus highGroundFlag, stats::CriticalRoll criticalRoll)
	{
		auto alwaysBackstab = skillProperties != nullptr
			&& skillProperties->Properties.Find(FixedString("AlwaysBackstab")) != nullptr;

		stats::HitDamageInfo tempHit;
		ComputeCharacterHitEventParams params{ target, attacker, weapon, damageList, hitType, noHitRoll,
			forceReduceDurability, & tempHit, skillProperties, alwaysBackstab, highGroundFlag, criticalRoll, false };
		ThrowEvent(*this, "ComputeCharacterHit", params);

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
			auto attackerChar = GetEntityWorld()->GetCharacter(attackerHandle, false);
			if (attackerChar) {
				attacker = attackerChar->Stats;
			} else {
				auto attackerItem = GetEntityWorld()->GetItem(attackerHandle, false);
				if (attackerItem) {
					attacker = attackerItem->Stats;
				} else {
					OsiError("Could not resolve attacker handle: " << std::hex << attackerHandle.Handle);
				}
			}
		}

		BeforeCharacterApplyDamageEventParams evt{
			target, attacker, &hit, causeType, impactDirection, context
		};
		ThrowEvent(*this, "BeforeCharacterApplyDamage", evt);
		return evt.Handled;
	}


	void ServerState::OnGameStateChanged(GameState fromState, GameState toState)
	{
		GameStateChangeEventParams params{ fromState, toState };
		ThrowEvent(*this, "GameStateChanged", params);
	}


	esv::Item* ServerState::OnGenerateTreasureItem(esv::Item* item)
	{
		TreasureItemGeneratedEventParams params{ item, nullptr };
		ThrowEvent(*this, "TreasureItemGenerated", params);

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
		BeforeCraftingExecuteCombinationEventParams params{ character, craftingStation, combinationId, quantity };

		for (auto ingredientHandle : ingredients) {
			auto ingredient = GetEntityWorld()->GetItem(ingredientHandle);
			if (ingredient) {
				params.Items.Add(ingredient);
			}
		}

		ThrowEvent(*this, "BeforeCraftingExecuteCombination", params);

		return params.Processed;
	}


	void ServerState::OnAfterCraftingExecuteCombination(CraftingStationType craftingStation, ObjectSet<ComponentHandle> const& ingredients,
		esv::Character* character, uint8_t quantity, FixedString const& combinationId, bool succeeded)
	{
		AfterCraftingExecuteCombinationEventParams params{ character, craftingStation, combinationId, quantity, succeeded };

		for (auto ingredientHandle : ingredients) {
			auto ingredient = GetEntityWorld()->GetItem(ingredientHandle);
			if (ingredient) {
				params.Items.Add(ingredient);
			}
		}

		ThrowEvent(*this, "AfterCraftingExecuteCombination", params);
	}


	void ServerState::OnBeforeShootProjectile(ShootProjectileHelper* helper)
	{
		BeforeShootProjectileEventParams params{ helper };
		ThrowEvent(*this, "BeforeShootProjectile", params);
	}


	void ServerState::OnShootProjectile(Projectile* projectile)
	{
		ShootProjectileEventParams params{ projectile };
		ThrowEvent(*this, "ShootProjectile", params);
	}

	void ServerState::OnProjectileHit(Projectile* projectile, ComponentHandle const& hitObject, glm::vec3 const& position)
	{
		auto hitObj = GetGameObjectInternal(hitObject);
		ProjectileHitEventParams params{ projectile, hitObj, position };
		ThrowEvent(*this, "ProjectileHit", params);
	}


	void ServerState::OnExecutePropertyDataOnGroundHit(glm::vec3& position, ComponentHandle casterHandle, stats::DamagePairList* damageList)
	{
		ExecutePropertyDataOnGroundHitEventParams params{ position, GetGameObjectInternal(casterHandle), damageList };
		ThrowEvent(*this, "GroundHit", params);
	}


	void ServerState::ExecutePropertyDataOnTarget(stats::PropertyExtender* prop, ComponentHandle attackerHandle,
		ComponentHandle targetHandle, glm::vec3 const& impactOrigin, bool isFromItem, stats::SkillPrototype * skillProto,
		stats::HitDamageInfo const* damageInfo)
	{
		auto attacker = GetGameObjectInternal(attackerHandle);
		auto target = GetGameObjectInternal(targetHandle);
		ExecutePropertyDataOnTargetEventParams params{ 
			prop, attacker, target, impactOrigin, isFromItem, skillProto, damageInfo
		};
		ThrowEvent(*this, "OnExecutePropertyDataOnTarget", params);
	}


	void ServerState::ExecutePropertyDataOnPosition(stats::PropertyExtender* prop, ComponentHandle attackerHandle,
		glm::vec3 const& position, float areaRadius, bool isFromItem, stats::SkillPrototype * skillPrototype,
		stats::HitDamageInfo const* damageInfo)
	{
		auto attacker = GetGameObjectInternal(attackerHandle);
		ExecutePropertyDataOnPositionEventParams params{
			prop, attacker, position, areaRadius, isFromItem, skillPrototype, damageInfo
		};
		ThrowEvent(*this, "OnExecutePropertyDataOnPosition", params);
	}


	bool ServerState::OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId)
	{
		// FIXME - not migrated yet!
		return false;

		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictOsiris);

		auto turnMgr = GetEntityWorld()->GetTurnManager();
		if (!turnMgr) {
			OsiErrorS("Couldn't fetch turn manager");
			return false;
		}

		auto combat = turnMgr->Combats.Find(combatId);
		if (combat == nullptr) {
			OsiError("No combat found with ID " << (unsigned)combatId);
			return false;
		}

		PushExtFunction(L, "_CalculateTurnOrder"); // stack: fn

		TurnManagerCombatProxy::New(L, combatId); // stack: fn, combat
		CombatTeamListToLua(L, combat->NextRoundTeams);

		if (CallWithTraceback(L, 2, 1) != 0) { // stack: retval
			LuaError("OnUpdateTurnOrder handler failed: " << lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}

		int isnil = lua_isnil(L, -1);

		bool ok = false;
		if (!isnil) {
			try {
				UpdateTurnOrder(L, combatId, -1, combat->NextRoundTeams, combat->NextTurnChangeNotificationTeamIds);
				ok = true;
			} catch (Exception &) {
				OsiError("UpdateTurnOrder failed");
			}
		}

		lua_pop(L, 1); // stack: -
		return ok;
	}


	std::optional<STDString> ServerState::GetModPersistentVars(STDString const& modTable)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);
		LifetimePin _p(lifetimeStack_);

		PushExtFunction(L, "_GetModPersistentVars");
		push(L, modTable);

		if (CallWithTraceback(L, 1, 1) != 0) {
			ERR("Ext._GetModPersistentVars failed: %s", lua_tostring(L, -1));
			lua_pop(L, 1);
			return {};
		}

		if (lua_type(L, -1) == LUA_TSTRING) {
			return get<char const*>(L, -1);
		}

		return {};
	}


	void ServerState::RestoreModPersistentVars(STDString const& modTable, STDString const& vars)
	{
		StackCheck _(L, 0);
		Restriction restriction(*this, RestrictAll);
		LifetimePin _p(lifetimeStack_);

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
		Lua.reset();
		Lua = std::make_unique<lua::ServerState>(*this);
		Lua->Initialize();
		Lua->StoryFunctionMappingsUpdated();
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
			ERR("Persistent variables for mod %s could not be retrieved, saving cached values!", mod.Str);
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
			DEBUG("Restoring persistent vars for mod %s (%ld bytes)", mod.Str, vars.size());
			LuaServerPin lua(*this);
			if (lua) {
				lua->RestoreModPersistentVars(*modTable, vars);
			}
		} else {
			ERR("Savegame has persistent variables for mod %s, but it is not loaded or has no ModTable! Variables may be lost on next save!", mod.Str);
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
