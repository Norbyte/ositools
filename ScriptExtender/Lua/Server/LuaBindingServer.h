#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <Lua/Server/LuaOsiris.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/Osiris.h>
#include <GameDefinitions/TurnManager.h>
#include <Osiris/Shared/CustomFunctions.h>
#include <Extender/Shared/ExtensionHelpers.h>
#include <Osiris/Shared/OsirisHelpers.h>

BEGIN_NS(lua)

LUA_POLYMORPHIC(IActionData)
LUA_POLYMORPHIC(IGameObject)
LUA_POLYMORPHIC(IEoCServerObject)
LUA_POLYMORPHIC(IEoCClientObject)
LUA_POLYMORPHIC(GameObjectTemplate)
LUA_POLYMORPHIC(EoCGameObjectTemplate)
LUA_POLYMORPHIC(esv::SurfaceAction)
LUA_POLYMORPHIC(esv::Status)
LUA_POLYMORPHIC(ecl::Status)
LUA_POLYMORPHIC(stats::ObjectInstance)
LUA_POLYMORPHIC(stats::EquipmentAttributes)

void RegisterSharedLibraries(lua_State* L);

END_NS()

BEGIN_NS(esv::lua)

using namespace dse::lua;

LifetimeHolder GetServerLifetime();
LifetimePool& GetServerLifetimePool();
void RegisterServerLibraries(lua_State* L);


class TurnManagerCombatProxy : public Userdata<TurnManagerCombatProxy>, public Indexable
{
public:
	static char const * const MetatableName;

	static void PopulateMetatable(lua_State * L);

	inline TurnManagerCombatProxy(uint8_t combatId)
		: combatId_(combatId)
	{}

	inline esv::TurnManager::Combat * Get()
	{
		return GetEntityWorld()->GetTurnManager()->Combats.Find(combatId_);
	}

	int Index(lua_State * L);

private:
	uint8_t combatId_;

	static int GetCurrentTurnOrder(lua_State * L);
	static int GetNextTurnOrder(lua_State * L);
	static int UpdateCurrentTurnOrder(lua_State * L);
	static int UpdateNextTurnOrder(lua_State * L);
	static int GetAllTeams(lua_State * L);
};

class TurnManagerTeamProxy : public Userdata<TurnManagerTeamProxy>, public Indexable
{
public:
	static char const * const MetatableName;

	inline TurnManagerTeamProxy(eoc::CombatTeamId teamId)
		: teamId_(teamId)
	{}

	inline eoc::CombatTeamId TeamId() const
	{
		return teamId_;
	}

	inline esv::TurnManager::CombatTeam * Get()
	{
		auto combat = GetEntityWorld()->GetTurnManager()->Combats.Find(teamId_.CombatId);
		if (combat) {
			auto team = combat->Teams.Find((uint32_t)teamId_);
			if (team) {
				return *team;
			} else {
				return nullptr;
			}
		} else {
			return nullptr;
		}
	}

	int Index(lua_State * L);

private:
	eoc::CombatTeamId teamId_;
};


class ItemConstructor : public Userdata<ItemConstructor>, public Indexable
{
public:
	static char const* const MetatableName;

	inline ItemConstructor()
	{}

	inline ObjectSet<eoc::ItemDefinition>& Get()
	{
		return definition_;
	}

	int Index(lua_State* L);

private:
	ObjectSet<eoc::ItemDefinition> definition_;
};


class ExtensionLibraryServer : public ExtensionLibrary
{
public:
	void Register(lua_State * L) override;
	void RegisterLib(lua_State * L) override;
	STDString GenerateOsiHelpers();

private:
	static char const * const NameResolverMetatableName;

	void RegisterNameResolverMetatable(lua_State * L);
	void CreateNameResolver(lua_State * L);

	static int LuaIndexResolverTable(lua_State * L);
};


struct GameStateChangeEventParams
{
	GameState FromState;
	GameState ToState;
};

struct StatusGetEnterChanceEventParams
{
	esv::Status* Status;
	bool IsEnterCheck;
	std::optional<int32_t> EnterChance;
};

struct BeforeStatusApplyEventParams
{
	IEoCServerObject* Owner;
	esv::Status* Status;
	bool PreventStatusApply;
};

struct StatusHitEnterEventParams
{
	esv::StatusHit* Hit;
	PendingHit* Context;
};

struct ComputeCharacterHitEventParams
{
	stats::Character* Target;
	stats::Character* Attacker;
	stats::Item* Weapon;
	stats::DamagePairList* DamageList;
	stats::HitType HitType;
	bool NoHitRoll;
	bool ForceReduceDurability;
	stats::HitDamageInfo* Hit;
	stats::PropertyList* SkillProperties;
	bool AlwaysBackstab;
	stats::HighGroundBonus HighGround;
	stats::CriticalRoll CriticalRoll;
	bool Handled{ false };
};

struct BeforeCharacterApplyDamageEventParams
{
	esv::Character* Target;
	stats::ObjectInstance* Attacker;
	stats::HitDamageInfo* Hit;
	CauseType Cause;
	glm::vec3 ImpactDirection;
	PendingHit* Context;
	bool Handled{ false };
};

struct TreasureItemGeneratedEventParams
{
	esv::Item* Item;
	esv::Item* ResultingItem{ nullptr };
};

struct BeforeCraftingExecuteCombinationEventParams
{
	esv::Character* Character;
	CraftingStationType CraftingStation;
	FixedString CombinationId;
	uint8_t Quantity;
	ObjectSet<esv::Item*> Items;
	bool Processed{ false };
};

struct AfterCraftingExecuteCombinationEventParams
{
	esv::Character* Character;
	CraftingStationType CraftingStation;
	FixedString CombinationId;
	uint8_t Quantity;
	bool Succeeded;
	ObjectSet<esv::Item*> Items;
};

struct BeforeShootProjectileEventParams
{
	ShootProjectileHelper* Projectile;
};

struct ShootProjectileEventParams
{
	Projectile* Projectile;
};

struct ProjectileHitEventParams
{
	Projectile* Projectile;
	IEoCServerObject* HitObject;
	glm::vec3 Position;
};

struct ExecutePropertyDataOnGroundHitEventParams
{
	glm::vec3 Position;
	IEoCServerObject* Caster;
	stats::DamagePairList* DamageList;
};

struct ExecutePropertyDataOnTargetEventParams
{
	stats::PropertyExtender* Property;
	IEoCServerObject* Attacker;
	IEoCServerObject* Target;
	glm::vec3 ImpactOrigin;
	bool IsFromItem;
	stats::SkillPrototype* Skill;
	stats::HitDamageInfo const* Hit;
};

struct ExecutePropertyDataOnPositionEventParams
{
	stats::PropertyExtender* Property;
	IEoCServerObject* Attacker;
	glm::vec3 Position;
	float AreaRadius;
	bool IsFromItem;
	stats::SkillPrototype* Skill;
	stats::HitDamageInfo const* Hit;
};

class ServerState : public State
{
public:
	static ServerState* FromLua(lua_State* L);

	ServerState(ExtensionState& state);
	~ServerState();

	void Initialize() override;
	bool IsClient() override;

	inline OsirisBinding& Osiris()
	{
		return osiris_;
	}

	void OnGameSessionLoading() override;
	void StoryFunctionMappingsUpdated();

	template <class TArg>
	void Call(char const* mod, char const* func, Vector<TArg> const & args)
	{
		auto L = GetState();
		LifetimePin _(GetStack());
		lua_checkstack(L, (int)args.size() + 1);
		auto stackSize = lua_gettop(L);

		try {
			if (mod != nullptr) {
				PushModFunction(L, mod, func); // stack: func
			} else {
				lua_getglobal(L, func); // stack: func
			}

			for (auto & arg : args) {
				OsiToLua(L, arg); // stack: func, arg0 ... argn
			}

			auto status = CallWithTraceback(L, (int)args.size(), 0);
			if (status != LUA_OK) {
				LuaError("Failed to call function '" << func << "': " << lua_tostring(L, -1));
				// stack: errmsg
				lua_pop(L, 1); // stack: -
			}
		} catch (Exception &) {
			auto stackRemaining = lua_gettop(L) - stackSize;
			if (stackRemaining > 0) {
				if (mod != nullptr) {
					LuaError("Call to mod function '" << mod << "'.'" << func << "' failed: " << lua_tostring(L, -1));
				} else {
					LuaError("Call to mod function '" << func << "' failed: " << lua_tostring(L, -1));
				}
				lua_pop(L, stackRemaining);
			} else {
				if (mod != nullptr) {
					LuaError("Internal error during call to mod function '" << mod << "'.'" << func << "'");
				} else {
					LuaError("Internal error during call to mod function '" << func << "'");
				}
			}
		}
	}

	std::optional<int32_t> StatusGetEnterChance(esv::Status * status, bool isEnterCheck);
	bool OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId);
	bool OnApplyStatus(ComponentHandle const& ownerHandle, esv::Status* status, bool preventStatusApply);
	void OnStatusHitEnter(esv::StatusHit* hit, PendingHit* context);
	bool ComputeCharacterHit(stats::Character * self,
		stats::Character *attackerStats, stats::Item *item, stats::DamagePairList *damageList, stats::HitType hitType, bool noHitRoll,
		bool forceReduceDurability, stats::HitDamageInfo *damageInfo, stats::PropertyList *skillProperties,
		stats::HighGroundBonus highGroundFlag, stats::CriticalRoll criticalRoll);
	bool OnCharacterApplyDamage(esv::Character* target, stats::HitDamageInfo& hit, ComponentHandle attackerHandle,
		CauseType causeType, glm::vec3& impactDirection, PendingHit* context);
	void OnGameStateChanged(GameState fromState, GameState toState);
	esv::Item* OnGenerateTreasureItem(esv::Item* item);
	bool OnBeforeCraftingExecuteCombination(CraftingStationType craftingStation, ObjectSet<ComponentHandle> const& ingredients,
		esv::Character* character, uint8_t quantity, FixedString const& combinationId);
	void OnAfterCraftingExecuteCombination(CraftingStationType craftingStation, ObjectSet<ComponentHandle> const& ingredients,
		esv::Character* character, uint8_t quantity, FixedString const& combinationId, bool succeeded);
	void OnBeforeShootProjectile(ShootProjectileHelper* helper);
	void OnShootProjectile(Projectile* projectile);
	void OnProjectileHit(Projectile* projectile, ComponentHandle const& hitObject, glm::vec3 const& position);
	void OnExecutePropertyDataOnGroundHit(glm::vec3& position, ComponentHandle casterHandle, stats::DamagePairList* damageList);

	void ExecutePropertyDataOnTarget(stats::PropertyExtender* prop, ComponentHandle attackerHandle,
		ComponentHandle target, glm::vec3 const& impactOrigin, bool isFromItem, stats::SkillPrototype* skillProto,
		stats::HitDamageInfo const* damageInfo);
	void ExecutePropertyDataOnPosition(stats::PropertyExtender* prop, ComponentHandle attackerHandle,
		glm::vec3 const& position, float areaRadius, bool isFromItem, stats::SkillPrototype* skillPrototype,
		stats::HitDamageInfo const* damageInfo);

	std::optional<STDString> GetModPersistentVars(STDString const& modTable);
	void RestoreModPersistentVars(STDString const& modTable, STDString const& vars);

	bool Query(char const* mod, char const* name, RegistryEntry* func,
		std::vector<CustomFunctionParam> const& signature, OsiArgumentDesc& params);

private:
	ExtensionLibraryServer library_;
	OsirisBinding osiris_;

	bool QueryInternal(char const* mod, char const* name, RegistryEntry* func,
		std::vector<CustomFunctionParam> const& signature, OsiArgumentDesc& params);
};

END_NS()
