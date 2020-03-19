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


	class LuaUIObjectProxy : public LuaUserdata<LuaUIObjectProxy>
	{
	public:
		static char const * const MetatableName;

		static void PopulateMetatable(lua_State * L);

		inline LuaUIObjectProxy(ObjectHandle handle)
			: handle_(handle)
		{}

		struct UIObject * Get();

	private:
		ObjectHandle handle_;

		static int SetPosition(lua_State * L);
		static int Resize(lua_State * L);
		static int Show(lua_State * L);
		static int Hide(lua_State * L);

		static int Invoke(lua_State * L);
		static int GotoFrame(lua_State * L);
		static int GetValue(lua_State * L);
		static int SetValue(lua_State * L);
		static int GetHandle(lua_State * L);
		static int Destroy(lua_State * L);
	};


	class LuaStateClient : public LuaState
	{
	public:
		LuaStateClient();
		~LuaStateClient();

		void OnUICall(ObjectHandle uiObjectHandle, const char * func, unsigned int numArgs, struct InvokeDataValue * args);
		bool SkillGetDescriptionParam(SkillPrototype * prototype, CDivinityStats_Character * character,
			ObjectSet<STDString> const & paramTexts, std::wstring & replacement);
		bool StatusGetDescriptionParam(StatusPrototype * prototype, CDivinityStats_Character * statusSource,
			CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, std::wstring & replacement);

		void OnClientUIObjectCreated(char const * name, ObjectHandle handle);
		UIObject * GetUIObject(char const * name);

	private:
		LuaExtensionLibraryClient library_;
		std::unordered_map<std::string, ObjectHandle> clientUI_;
	};
}
