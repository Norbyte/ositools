#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>Combat</lua_module>
BEGIN_NS(esv)

void TurnManager::Combat::LuaUpdateTurnOrder(lua_State* L, int index,
	ObjectSet<esv::TurnManager::CombatTeam*, MSVCMemoryAllocator>& combatTeams,
	ObjectSet<eoc::CombatTeamId>& combatNotifies)
{
	index = lua_absindex(L, index);
	if (lua_type(L, index) != LUA_TTABLE) luaL_error(L, "UpdateTurnOrder() expects a table of teams as the first argument");

	ObjectSet<esv::TurnManager::CombatTeam*> teams;
	std::unordered_set<eoc::CombatTeamId> notifies;

	lua_pushnil(L);

	while (lua_next(L, index) != 0) {
		auto team = lua::LightObjectProxyByRefMetatable::GetTyped<TurnManager::CombatTeam>(L, lua_absindex(L, -1));
		teams.push_back(team);
		notifies.insert(team->Id);
		lua_pop(L, 1);
	}

	for (auto team : combatTeams) {
		notifies.insert(team->Id);
	}

	for (auto notify : combatNotifies) {
		notifies.insert(notify);
	}

	combatTeams.clear();
	for (auto const& team : teams) {
		combatTeams.push_back(team);
	}

	combatNotifies.clear();
	for (auto const& teamId : notifies) {
		combatNotifies.push_back(teamId);
	}

	auto protocol = GetTurnBasedProtocol();
	if (protocol != nullptr) {
		auto turnManager = GetEntityWorld()->GetTurnManager();
		for (auto const& combat : turnManager->Combats) {
			if (&combat.Value == this) {
				protocol->UpdatedTurnOrderCombatIds.push_back(combat.Key);
				break;
			}
		}
	}
}

void TurnManager::Combat::LuaUpdateCurrentTurnOrder(lua_State* L)
{
	LuaUpdateTurnOrder(L, 2, CurrentRoundTeams, CurrentTurnChangeNotificationTeamIds);
}

void TurnManager::Combat::LuaUpdateNextTurnOrder(lua_State* L)
{
	LuaUpdateTurnOrder(L, 2, NextRoundTeams, NextTurnChangeNotificationTeamIds);
}

END_NS()

/// <lua_module>Combat</lua_module>
BEGIN_NS(esv::lua::combat)

TurnManager* GetTurnManager()
{
	return GetEntityWorld()->GetTurnManager();
}

void RegisterCombatLib()
{
	DECLARE_MODULE(Combat, Server)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetTurnManager)
	END_MODULE()
}

END_NS()
