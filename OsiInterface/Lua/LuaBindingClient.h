#pragma once

#include <Lua/LuaBinding.h>

namespace dse
{
	struct InvokeDataValue;
	struct UIObject;
}

namespace dse::lua
{
	class ExtensionLibraryClient : public ExtensionLibrary
	{
	public:
		void Register(lua_State * L) override;
		void RegisterLib(lua_State * L) override;
	};


	class UIObjectProxy : public Userdata<UIObjectProxy>, public Pushable<PushPolicy::None>
	{
	public:
		static char const * const MetatableName;

		static void PopulateMetatable(lua_State * L);

		inline UIObjectProxy(ObjectHandle handle)
			: handle_(handle)
		{}

		UIObject * Get();

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


	class ClientState : public State
	{
	public:
		ClientState();
		~ClientState();

		void OnUICall(ObjectHandle uiObjectHandle, const char * func, unsigned int numArgs, InvokeDataValue * args);
		std::optional<STDWString> SkillGetDescriptionParam(SkillPrototype * prototype,
			CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts);
		std::optional<STDWString> StatusGetDescriptionParam(StatusPrototype * prototype, CDivinityStats_Character * statusSource,
			CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts);

		void OnClientUIObjectCreated(char const * name, ObjectHandle handle);
		UIObject * GetUIObject(char const * name);

	private:
		ExtensionLibraryClient library_;
		std::unordered_map<STDString, ObjectHandle> clientUI_;
	};
}
