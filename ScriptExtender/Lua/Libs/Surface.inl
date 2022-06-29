#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>Surface</lua_module>
BEGIN_NS(lua::surface)

UserReturn GetTransformRules(lua_State* L)
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

void UpdateTransformInteraction(lua_State* L, int idx, eoc::SurfaceTransformInteractions::Interaction& interaction)
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

void UpdateTransformRules(lua_State* L)
{
	StackCheck _(L, 0);
	auto rules = GetStaticSymbols().eoc__SurfaceTransformActionsFromType;
	if (rules == nullptr) {
		OsiError("Surface transform rules not available!");
		return;
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

			UpdateTransformInteraction(L, interactionIdx, surfaceRules[curInteractionIdx++]);
		}

	}

	*rules = *interactions;
}

SurfaceTemplate* GetTemplate(SurfaceType type)
{
	return GetStaticSymbols().GetSurfaceTemplate(type);
}

void RegisterSurfaceLib()
{
	DECLARE_MODULE(Surface, Both)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetTransformRules)
	MODULE_FUNCTION(UpdateTransformRules)
	MODULE_FUNCTION(GetTemplate)
	END_MODULE()
}

END_NS()
