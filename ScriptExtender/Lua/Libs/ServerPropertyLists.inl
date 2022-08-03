#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>PropertyList</lua_module>
BEGIN_NS(esv::lua::proplist)

void ExecuteSkillPropertiesOnTarget(lua_State* L)
{
	StackCheck _(L, 0);
	auto skillId = get<FixedString>(L, 1);
	auto attacker = ecs::LuaGetCharacter(L, 2);
	auto target = ecs::LuaGetCharacter(L, 3);
	auto position = get<glm::vec3>(L, 4);
	auto propertyContext = get<dse::stats::PropertyContext>(L, 5);
	auto isFromItem = get<bool>(L, 6);

	dse::stats::SkillPrototype* skillProto{ nullptr };
	auto skillProtoMgr = GetStaticSymbols().eoc__SkillPrototypeManager;
	if (skillProtoMgr && *skillProtoMgr) {
		skillProto = (*skillProtoMgr)->Prototypes.try_get(skillId);
	}

	if (!skillProto) {
		LuaError("Couldn't find skill prototype for " << skillId);
		return;
	}

	auto exec = GetStaticSymbols().esv__ExecuteCharacterSetExtraProperties;
	auto skillProperties = skillProto->GetStats()->PropertyLists.try_get(GFS.strSkillProperties);

	if (!skillProperties) {
		LuaError("Skill " << skillId << " has no SkillProperties!");
		return;
	}

	if (!attacker || !target || !exec) {
		return;
	}

	ComponentHandle attackerHandle;
	attacker->GetObjectHandle(attackerHandle);
	ObjectSet<esv::Character*> targets;
	targets.push_back(target);

	exec(skillProperties, attackerHandle.Handle, targets, position, propertyContext, isFromItem,
		skillProto, nullptr, 0.0f, nullptr, false, 2.4f);
}

void ExecuteSkillPropertiesOnPosition(lua_State* L)
{
	StackCheck _(L, 0);
	auto skillId = get<FixedString>(L, 1);
	auto attacker = ecs::LuaGetCharacter(L, 2);
	auto position = get<glm::vec3>(L, 3);
	auto radius = get<float>(L, 4);
	auto propertyContext = get<dse::stats::PropertyContext>(L, 5);
	auto isFromItem = get<bool>(L, 6);

	dse::stats::SkillPrototype* skillProto{ nullptr };
	auto skillProtoMgr = GetStaticSymbols().eoc__SkillPrototypeManager;
	if (skillProtoMgr && *skillProtoMgr) {
		skillProto = (*skillProtoMgr)->Prototypes.try_get(skillId);
	}

	if (!skillProto) {
		LuaError("Couldn't find skill prototype for " << skillId);
		return;
	}

	auto exec = GetStaticSymbols().esv__ExecutePropertyDataOnPositionOnly;
	auto skillProperties = skillProto->GetStats()->PropertyLists.try_get(GFS.strSkillProperties);

	if (!skillProperties) {
		LuaError("Skill " << skillId << " has no SkillProperties!");
		return;
	}

	if (!attacker || !exec) {
		return;
	}

	ComponentHandle attackerHandle;
	attacker->GetObjectHandle(attackerHandle);

	exec(skillProperties, attackerHandle.Handle, &position, radius, propertyContext, isFromItem, skillProto, nullptr, 2.4f);
}

void ExecuteExtraPropertiesOnTarget(FixedString statsEntryName, FixedString propertyName, ProxyParam<Character> attacker, ProxyParam<Character> target,
	glm::vec3 position, dse::stats::PropertyContext propertyContext, bool isFromItem, std::optional<FixedString> skillId)
{
	dse::stats::SkillPrototype* skillProto{ nullptr };
	if (skillId) {
		skillProto = (*GetStaticSymbols().eoc__SkillPrototypeManager)->Prototypes.try_get(*skillId);
		if (!skillProto) {
			LuaError("Couldn't find skill prototype for " << *skillId);
			return;
		}
	}

	auto stat = GetStaticSymbols().GetStats()->Objects.Find(statsEntryName);
	if (!stat) {
		LuaError("Couldn't find stats entry " << statsEntryName);
		return;
	}

	auto extraProperties = stat->PropertyLists.try_get(propertyName);
	if (!extraProperties) {
		LuaError("Stats entry " << statsEntryName << " has no property list named " << propertyName);
		return;
	}

	auto exec = GetStaticSymbols().esv__ExecuteCharacterSetExtraProperties;

	ComponentHandle attackerHandle;
	attacker->GetObjectHandle(attackerHandle);
	ObjectSet<esv::Character*> targets;
	targets.push_back(target.Object);

	exec(extraProperties, attackerHandle.Handle, targets, position, propertyContext, isFromItem,
		skillProto, nullptr, 0.0f, nullptr, false, 2.4f);
}

void ExecuteExtraPropertiesOnPosition(FixedString statsEntryName, FixedString propertyName, ProxyParam<Character> attacker,
	glm::vec3 position, float radius, dse::stats::PropertyContext propertyContext, bool isFromItem, std::optional<FixedString> skillId)
{
	dse::stats::SkillPrototype* skillProto{ nullptr };
	if (skillId) {
		skillProto = (*GetStaticSymbols().eoc__SkillPrototypeManager)->Prototypes.try_get(*skillId);
		if (!skillProto) {
			LuaError("Couldn't find skill prototype for " << *skillId);
			return;
		}
	}

	auto stat = GetStaticSymbols().GetStats()->Objects.Find(statsEntryName);
	if (!stat) {
		LuaError("Couldn't find stats entry " << statsEntryName);
		return;
	}

	auto extraProperties = stat->PropertyLists.try_get(propertyName);
	if (!extraProperties) {
		LuaError("Stats entry " << statsEntryName << " has no property list named " << propertyName);
		return;
	}

	auto exec = GetStaticSymbols().esv__ExecutePropertyDataOnPositionOnly;

	ComponentHandle attackerHandle;
	attacker->GetObjectHandle(attackerHandle);

	exec(extraProperties, attackerHandle.Handle, &position, radius, propertyContext, isFromItem,
		skillProto, nullptr, 2.4f);
}

void RegisterPropertyListLib()
{
	DECLARE_MODULE(PropertyList, Server)
	BEGIN_MODULE()
	MODULE_FUNCTION(ExecuteSkillPropertiesOnTarget)
	MODULE_FUNCTION(ExecuteSkillPropertiesOnPosition)
	MODULE_FUNCTION(ExecuteExtraPropertiesOnTarget)
	MODULE_FUNCTION(ExecuteExtraPropertiesOnPosition)
	END_MODULE()
}

END_NS()
