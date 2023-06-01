#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>Template</lua_module>
BEGIN_NS(ecl::lua::tmpl)

Map<FixedString, GameObjectTemplate*>* GetAllRootTemplates()
{
	auto bank = GetStaticSymbols().GetGlobalTemplateBank();
	return &bank->Templates;
}

GameObjectTemplate* GetRootTemplate(FixedString templateId)
{
	auto bank = GetStaticSymbols().GetGlobalTemplateBank();
	return bank->Templates.try_get(templateId);
}

Map<FixedString, GameObjectTemplate*>* GetAllLocalTemplates()
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (level) {
		return &level->LocalTemplateManager->Templates;
	}

	return nullptr;
}

GameObjectTemplate* GetLocalTemplate(FixedString const& templateId)
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (level) {
		return level->LocalTemplateManager->Templates.try_get(templateId);
	}

	return nullptr;
}

Map<FixedString, GameObjectTemplate*>* GetAllCacheTemplates()
{
	auto cache = *GetStaticSymbols().ecl__CacheTemplateManager;
	if (cache) {
		return &cache->Templates;
	}

	return nullptr;
}

GameObjectTemplate* GetCacheTemplate(FixedString const& templateId)
{
	auto cache = *GetStaticSymbols().ecl__CacheTemplateManager;
	if (cache) {
		return cache->Templates.try_get(templateId);
	}

	return nullptr;
}

Map<FixedString, GameObjectTemplate*>* GetAllLocalCacheTemplates()
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (level) {
		return &level->LevelCacheTemplateManager->Templates;
	}

	return nullptr;
}

GameObjectTemplate* GetLocalCacheTemplate(FixedString const& templateId)
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (level) {
		return level->LevelCacheTemplateManager->Templates.try_get(templateId);
	}

	return nullptr;
}

GameObjectTemplate* GetTemplate(FixedString const& templateId)
{
	auto tmpl = GetRootTemplate(templateId);
	if (!tmpl) {
		tmpl = GetLocalTemplate(templateId);
		if (!tmpl) {
			tmpl = GetCacheTemplate(templateId);
			if (!tmpl) {
				tmpl = GetLocalCacheTemplate(templateId);
			}
		}
	}

	return tmpl;
}

void RegisterTemplateLib()
{
	DECLARE_MODULE(Template, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetAllRootTemplates)
	MODULE_FUNCTION(GetRootTemplate)
	MODULE_FUNCTION(GetAllLocalTemplates)
	MODULE_FUNCTION(GetLocalTemplate)
	MODULE_FUNCTION(GetAllCacheTemplates)
	MODULE_FUNCTION(GetCacheTemplate)
	MODULE_FUNCTION(GetAllLocalCacheTemplates)
	MODULE_FUNCTION(GetLocalCacheTemplate)
	MODULE_FUNCTION(GetTemplate)
	END_MODULE()
}

END_NS()
