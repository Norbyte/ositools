#include <Lua/Shared/LuaMethodHelpers.h>
#include <Lua/Libs/ClientCharacterTask.inl>

/// <lua_module>Behavior</lua_module>
BEGIN_NS(ecl::lua::behavior)

void AddGlobalSkillConstructor(lua_State* L, Ref ctor)
{
	ClientState::FromLua(L)->GetCustomSkillManager().AddConstructor(L, ctor);
}

void AddSkillTypeConstructor(lua_State* L, SkillType type, Ref ctor)
{
	ClientState::FromLua(L)->GetCustomSkillManager().AddConstructor(type, L, ctor);
}

void AddSkillIdConstructor(lua_State* L, FixedString id, Ref ctor)
{
	ClientState::FromLua(L)->GetCustomSkillManager().AddConstructor(id, L, ctor);
}

void RegisterBehaviorLib()
{
	DECLARE_MODULE(Behavior, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(RegisterCharacterTask)
	MODULE_FUNCTION(AttachCharacterTask)
	END_MODULE()

	DECLARE_SUBMODULE(Behavior, Skill, Client)
	BEGIN_MODULE()
	MODULE_NAMED_FUNCTION("AddGlobal", AddGlobalSkillConstructor)
	MODULE_NAMED_FUNCTION("AddByType", AddSkillTypeConstructor)
	MODULE_NAMED_FUNCTION("AddById", AddSkillIdConstructor)
	END_MODULE()
}

END_NS()
