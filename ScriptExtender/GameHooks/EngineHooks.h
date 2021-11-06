#pragma once

#include <GameHooks/Wrappers.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/GameObjects/Status.h>

BEGIN_SE()

class EngineHooks
{
public:
#define HOOK_DEFN(name, sym, defn, hookType) enum class name##Tag {}; \
hookType<name##Tag, defn> name;
#include <GameHooks/EngineHooks.inl>
#undef HOOK_DEFN

	void HookAll();
	void UnhookAll();
};

END_SE()
