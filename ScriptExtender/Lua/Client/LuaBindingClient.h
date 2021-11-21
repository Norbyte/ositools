#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <GameDefinitions/UI.h>
#include <span>

namespace dse
{
	struct UIObject;

	void UIObjectFunctionCallCapture(UIObject* self, const char* function, unsigned int numArgs, ig::InvokeDataValue* args);
}

namespace dse::ig
{
	struct InvokeDataValue;
	struct IggyValuePath;
}

namespace dse::ecl::lua
{
	using namespace dse::lua;

	LifetimeHolder GetClientLifetime();
	LifetimePool& GetClientLifetimePool();

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

		inline StatusHandleProxy(ComponentHandle character, NetId status)
			: character_(character), statusNetId_(status)
		{}

		inline StatusHandleProxy(ComponentHandle character, ComponentHandle status)
			: character_(character), statusHandle_(status)
		{}

		ecl::Status* Get(lua_State* L);
		int Index(lua_State* L);
		int NewIndex(lua_State* L);

	private:
		ComponentHandle character_;
		ComponentHandle statusHandle_;
		NetId statusNetId_;
	};



	struct UIFlashPath
	{
	public:
		std::vector<ig::IggyValuePath> paths_;

		UIFlashPath();
		UIFlashPath(std::vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
		ig::IggyValuePath* Last();
	};

	class UIFlashObject : public Userdata<UIFlashObject>, public Indexable, public NewIndexable
	{
	public:
		static char const* const MetatableName;

		UIFlashObject(std::vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
		int Index(lua_State* L);
		int NewIndex(lua_State* L);

	private:
		UIFlashPath path_;
	};


	class UIFlashArray : public Userdata<UIFlashArray>, public Indexable, public NewIndexable, public Lengthable
	{
	public:
		static char const* const MetatableName;

		UIFlashArray(std::vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
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

		UIFlashFunction(std::vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
		int LuaCall(lua_State* L);

	private:
		UIFlashPath path_;
	};

	struct GameStateChangeEventParams
	{
		GameState FromState;
		GameState ToState;
	};

	struct UIObjectCreatedEventParams
	{
		UIObject* UI;
	};

	struct UICallEventParams
	{
		UIObject* UI;
		char const* Function;
		char const* When;
		std::span<ig::InvokeDataValue> Args;
	};

	struct SkillGetDescriptionEventParams
	{
		SkillPrototype* Skill;
		CDivinityStats_Character* Character;
		ObjectSet<STDString> Params;
		bool IsFromItem;
		STDString Description;
	};

	struct StatusGetDescriptionEventParams
	{
		StatusPrototype* Status;
		CRPGStats_ObjectInstance* Owner;
		CRPGStats_ObjectInstance* StatusSource;
		ObjectSet<STDString> Params;
		STDString Description;
	};

	struct GetSkillPropertyDescriptionEventParams
	{
		CRPGStats_Object_Property_Extender* Property;
		STDString Description;
	};

	struct InputEventParams
	{
		InputEvent* Event;
	};

	class ClientState : public State
	{
	public:
		ClientState();
		~ClientState();

		void OnCreateUIObject(ComponentHandle handle);
		void OnUICall(UIObject* ui, const char * func, unsigned int numArgs, ig::InvokeDataValue * args);
		void OnAfterUICall(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		void OnUIInvoke(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		void OnAfterUIInvoke(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		std::optional<STDWString> SkillGetDescriptionParam(SkillPrototype * prototype,
			CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, bool isFromItem);
		std::optional<STDWString> StatusGetDescriptionParam(StatusPrototype * prototype, CRPGStats_ObjectInstance* owner,
			CRPGStats_ObjectInstance* statusSource, ObjectSet<STDString> const & paramTexts);
		void OnGameStateChanged(GameState fromState, GameState toState);
		std::optional<STDString> GetSkillPropertyDescription(CRPGStats_Object_Property_Extender*);
		void OnAppInputEvent(InputEvent const& inputEvent);

		void OnCustomClientUIObjectCreated(char const * name, ComponentHandle handle);
		UIObject * GetUIObject(char const * name);

	private:
		ExtensionLibraryClient library_;
		std::unordered_map<STDString, ComponentHandle> clientUI_;
	};
}
