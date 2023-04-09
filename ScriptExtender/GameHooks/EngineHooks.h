#pragma once

#include <GameHooks/Wrappers.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/GameObjects/AiHelpers.h>

BEGIN_SE()

class EngineHooks
{
public:
#define HOOK_DEFN(name, sym, defn) enum class name##Tag {}; \
	WrappableFunction<name##Tag, defn> name;
#include <GameHooks/EngineHooks.inl>
#undef HOOK_DEFN

	void HookAll();
	void UnhookAll();
};

END_SE()
