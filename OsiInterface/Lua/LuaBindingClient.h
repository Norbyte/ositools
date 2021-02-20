#pragma once

#include <Lua/LuaBinding.h>

namespace dse
{
	struct UIObject;
}

namespace dse::ig
{
	struct InvokeDataValue;
	struct IggyValuePath;
}

namespace dse::ecl::lua
{
	using namespace dse::lua;

	void UIObjectFunctionCallCapture(UIObject* self, const char* function, unsigned int numArgs, ig::InvokeDataValue* args);

	class ExtensionLibraryClient : public ExtensionLibrary
	{
	public:
		void Register(lua_State * L) override;
		void RegisterLib(lua_State * L) override;
	};


	class StatusHandleProxy : public Userdata<StatusHandleProxy>, public Indexable, public NewIndexable, public Pushable<PushPolicy::None>
	{
	public:
		static char const* const MetatableName;

		inline StatusHandleProxy(ObjectHandle character, NetId status)
			: character_(character), statusNetId_(status)
		{}

		inline StatusHandleProxy(ObjectHandle character, ObjectHandle status)
			: character_(character), statusHandle_(status)
		{}

		ecl::Status* Get(lua_State* L);
		int Index(lua_State* L);
		int NewIndex(lua_State* L);

	private:
		ObjectHandle character_;
		ObjectHandle statusHandle_;
		NetId statusNetId_;
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
		static int GetPlayerHandle(lua_State * L);
		static int GetTypeId(lua_State * L);
		static int GetRoot(lua_State* L);
		static int Destroy(lua_State * L);
		static int ExternalInterfaceCall(lua_State * L);
		static int CaptureExternalInterfaceCalls(lua_State * L);
		static int CaptureInvokes(lua_State* L);
	};



	struct UIFlashPath
	{
	public:
		std::vector<ig::IggyValuePath> paths_;

		UIFlashPath();
		UIFlashPath(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path);
		ig::IggyValuePath* Last();
	};

	class UIFlashObject : public Userdata<UIFlashObject>, public Indexable, public NewIndexable
	{
	public:
		static char const* const MetatableName;

		UIFlashObject(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path);
		int Index(lua_State* L);
		int NewIndex(lua_State* L);

	private:
		UIFlashPath path_;
	};


	class UIFlashArray : public Userdata<UIFlashArray>, public Indexable, public NewIndexable, public Lengthable
	{
	public:
		static char const* const MetatableName;

		UIFlashArray(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path);
		int Index(lua_State* L);
		int NewIndex(lua_State* L);
		int Length(lua_State* L);

	private:
		UIFlashPath path_;
	};


	class UIFlashFunction : public Userdata<UIFlashFunction>, public Callable
	{
	public:
		static char const* const MetatableName;

		UIFlashFunction(std::vector<ig::IggyValuePath>& parents, ig::IggyValuePath* path);
		int LuaCall(lua_State* L);

	private:
		UIFlashPath path_;
	};


	class ClientState : public State
	{
	public:
		ClientState();
		~ClientState();

		void OnCreateUIObject(ObjectHandle handle);
		void OnUICall(ObjectHandle uiObjectHandle, const char * func, unsigned int numArgs, ig::InvokeDataValue * args);
		void OnAfterUICall(ObjectHandle uiObjectHandle, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		void OnUIInvoke(ObjectHandle uiObjectHandle, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		void OnAfterUIInvoke(ObjectHandle uiObjectHandle, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		std::optional<STDWString> SkillGetDescriptionParam(SkillPrototype * prototype,
			CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, bool isFromItem);
		std::optional<STDWString> StatusGetDescriptionParam(StatusPrototype * prototype, CRPGStats_ObjectInstance* owner,
			CRPGStats_ObjectInstance* statusSource, ObjectSet<STDString> const & paramTexts);
		void OnGameStateChanged(GameState fromState, GameState toState);
		std::optional<STDString> GetSkillPropertyDescription(CRPGStats_Object_Property_Extender*);
		void OnAppInputEvent(InputEvent const& inputEvent);

		void OnCustomClientUIObjectCreated(char const * name, ObjectHandle handle);
		UIObject * GetUIObject(char const * name);

	private:
		ExtensionLibraryClient library_;
		std::unordered_map<STDString, ObjectHandle> clientUI_;
	};
}
