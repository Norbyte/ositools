#pragma once

#include <Lua/LuaBinding.h>

namespace osidbg
{
	class LuaExtensionLibraryClient
	{
	public:
		LuaExtensionLibraryClient();

		void Register(lua_State * L);
		void RegisterLib(lua_State * L);

	private:
		static int Require(lua_State * L);
	};


	class LuaStateClient : public LuaState
	{
	public:
		LuaStateClient();

		bool SkillGetDescriptionParam(SkillPrototype * prototype, CDivinityStats_Character * character,
			ObjectSet<STDString> const & paramTexts, std::wstring & replacement);
		bool StatusGetDescriptionParam(StatusPrototype * prototype, CDivinityStats_Character * statusSource,
			CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, std::wstring & replacement);
	private:
		LuaExtensionLibraryClient library_;
	};
}
