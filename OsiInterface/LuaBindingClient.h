#pragma once

#include <Lua/LuaBinding.h>

namespace osidbg
{
	class LuaExtensionLibraryClient : public LuaExtensionLibrary
	{
	public:
		void Register(lua_State * L) override;
		void RegisterLib(lua_State * L) override;
	};


	class LuaUIObjectProxy : public LuaUserdata<LuaUIObjectProxy>, public LuaIndexable
	{
	public:
		static char const * const MetatableName;

		static void PopulateMetatable(lua_State * L);

		inline LuaUIObjectProxy(ObjectHandle handle)
			: handle_(handle)
		{}

		struct UIObject * Get();

		int LuaIndex(lua_State * L);

	private:
		ObjectHandle handle_;

		static int SetPosition(lua_State * L);
		static int Resize(lua_State * L);
		static int Show(lua_State * L);
		static int Hide(lua_State * L);

		static int Invoke(lua_State * L);
		static int GotoFrame(lua_State * L);
		static int AddInvokeName(lua_State * L);
		static int GetValue(lua_State * L);
		static int SetValue(lua_State * L);
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
