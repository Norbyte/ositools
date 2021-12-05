#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(esv::lua::stats)

std::optional<FixedString> CreateCustomStat(char const* name, char const* description)
{
	return CustomStatHelpers::CreateStat(name, description);
}

UserReturn GetCustomStatById(lua_State* L, char const* statId)
{
	StackCheck _(L, 1);

	auto statDefn = CustomStatHelpers::FindStatDefinitionById(statId);
	if (statDefn) {
		lua_newtable(L);
		settable(L, "Id", statDefn->Id);
		settable(L, "Name", statDefn->Name);
		settable(L, "Description", statDefn->Description);
	} else {
		push(L, nullptr);
	}

	return 1;
}

UserReturn GetCustomStatByName(lua_State* L, char const* statName)
{
	StackCheck _(L, 1);

	auto statDefn = CustomStatHelpers::FindStatDefinitionByName(statName);
	if (statDefn) {
		lua_newtable(L);
		settable(L, "Id", statDefn->Id);
		settable(L, "Name", statDefn->Name);
		settable(L, "Description", statDefn->Description);
	} else {
		push(L, nullptr);
	}

	return 1;
}

UserReturn GetAllCustomStats(lua_State* L)
{
	StackCheck _(L, 1);

	auto entityWorld = GetEntityWorld();
	auto statSystem = entityWorld->GetCustomStatSystem();

	lua_newtable(L);
	int idx{ 1 };
	for (auto const& defn : statSystem->CreatedDefinitions) {
		auto statDefn = entityWorld->GetCustomStatDefinitionComponent(defn.Handle);
		if (statDefn != nullptr) {
			settable(L, idx++, statDefn->Id);
		}
	}

	for (auto const& defn : statSystem->InSyncDefinitions) {
		auto statDefn = entityWorld->GetCustomStatDefinitionComponent(defn.Handle);
		if (statDefn != nullptr) {
			settable(L, idx++, statDefn->Id);
		}
	}

	return 1;
}

void RegisterCustomStatLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"Create", LuaWrapFunction(&CreateCustomStat)},
		{"GetById", LuaWrapFunction(&GetCustomStatById)},
		{"GetByName", LuaWrapFunction(&GetCustomStatByName)},
		{"GetAll", LuaWrapFunction(&GetAllCustomStats)},
		{0,0}
	};

	RegisterLib(L, "CustomStat", lib);
}

END_NS()
