#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>Template</lua_module>
BEGIN_NS(esv::lua::tmpl)

Map<FixedString, GameObjectTemplate*>* GetAllRootTemplates()
{
	auto bank = GetStaticSymbols().GetGlobalTemplateBank();
	return &bank->Templates;
}

GameObjectTemplate* GetRootTemplate(FixedString const& templateId)
{
	auto bank = GetStaticSymbols().GetGlobalTemplateBank();
	return bank->Templates.try_get(templateId);
}

Map<FixedString, GameObjectTemplate*>* GetAllCacheTemplates()
{
	auto cache = *GetStaticSymbols().esv__CacheTemplateManager;
	if (cache) {
		return &cache->Templates;
	}

	return nullptr;
}

Map<FixedString, GameObjectTemplate*>* GetAllLocalTemplates()
{
	auto level = GetStaticSymbols().GetCurrentServerLevel();
	if (level) {
		return &level->LocalTemplateManager->Templates;
	}

	return nullptr;
}

GameObjectTemplate* GetLocalTemplate(FixedString const& templateId)
{
	auto level = GetStaticSymbols().GetCurrentServerLevel();
	if (level) {
		return level->LocalTemplateManager->Templates.try_get(templateId);
	}

	return nullptr;
}

GameObjectTemplate* GetCacheTemplate(FixedString const& templateId)
{
	auto cache = *GetStaticSymbols().esv__CacheTemplateManager;
	if (cache) {
		return cache->Templates.try_get(templateId);
	}

	return nullptr;
}

Map<FixedString, GameObjectTemplate*>* GetAllLocalCacheTemplates()
{
	auto level = GetStaticSymbols().GetCurrentServerLevel();
	if (level) {
		return &level->LevelCacheTemplateManager->Templates;
	}

	return nullptr;
}

GameObjectTemplate* GetLocalCacheTemplate(FixedString const& templateId)
{
	auto level = GetStaticSymbols().GetCurrentServerLevel();
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

GameObjectTemplate* CreateCacheTemplate(FixedString const& templateId)
{
	auto tmpl = GetTemplate(templateId);
	if (!tmpl) {
		OsiError("Couldn't find template to cache: " << templateId);
		return nullptr;
	}

	if (tmpl->Handle.Type() != (uint32_t)TemplateType::RootTemplate) {
		OsiError("Cannot cache template that is not global: " << templateId);
		return nullptr;
	}

	auto baseCopy = tmpl->Clone();
	baseCopy->Id = GenerateGuid();
	baseCopy->RootTemplate = tmpl->Id;
	baseCopy->Flags.Value |= GameObjectTemplateFlags::IsCustom;
	baseCopy->Flags.IsOverridden = true;

	auto cache = *GetStaticSymbols().esv__CacheTemplateManager;
	auto cacheFunc = GetStaticSymbols().ls__CacheTemplateManagerBase__CacheTemplate;

	auto cachedTmpl = cacheFunc(cache, baseCopy);
	if (cache->NewTemplates.empty()) {
		OsiError("CacheTemplateManager did not add new template: " << cachedTmpl->Id);
	}

	return cachedTmpl;
}

void RegisterTemplateLib()
{
	DECLARE_MODULE(Template, Server)
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
	MODULE_FUNCTION(CreateCacheTemplate)
	END_MODULE()
}

END_NS()
