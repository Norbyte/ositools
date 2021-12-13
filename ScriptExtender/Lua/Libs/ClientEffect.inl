#include <Lua/Shared/LuaMethodHelpers.h>
#include <GameDefinitions/Symbols.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(ecl::lua::effect)

ClientMultiEffect* CreateMultiEffectOnCharacter(lua_State* L, char const* effectText, glm::vec3 const& position, std::optional<ProxyParam<Character>> target, FixedString const& weaponBones, std::optional<ProxyParam<Character>> listenForTextKeys)
{
	auto init = GetStaticSymbols().ecl__MultiEffectHandler__Init;
	auto state = ClientState::FromLua(L);
	auto effect = state->GetEffectSystem().Create();
	init(effect, effectText, position, target ? target->Object : nullptr, nullptr, listenForTextKeys ? listenForTextKeys->Object : nullptr, weaponBones);
	return effect;
}

ClientMultiEffect* CreateMultiEffectOnItem(lua_State* L, char const* effectText, glm::vec3 const& position, std::optional<ProxyParam<Item>> target, FixedString const& weaponBones, std::optional<ProxyParam<Item>> listenForTextKeys)
{
	auto init = GetStaticSymbols().ecl__MultiEffectHandler__Init;
	auto state = ClientState::FromLua(L);
	auto effect = state->GetEffectSystem().Create();
	init(effect, effectText, position, target ? target->Object : nullptr, nullptr, listenForTextKeys ? listenForTextKeys->Object : nullptr, weaponBones);
	return effect;
}

void ClientMultiEffect::Delete(lua_State* L)
{
	auto state = ClientState::FromLua(L);
	state->GetEffectSystem().Destroy(this);
}

void RegisterEffectLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"CreateMultiEffectOnCharacter", LuaWrapFunction(&CreateMultiEffectOnCharacter)},
		{"CreateMultiEffectOnItem", LuaWrapFunction(&CreateMultiEffectOnItem)},
		{0,0}
	};

	RegisterLib(L, "Effect", lib);
}

END_NS()
