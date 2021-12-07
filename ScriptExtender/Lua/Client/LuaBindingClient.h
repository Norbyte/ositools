#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <GameDefinitions/UI.h>
#include <Lua/Client/ClientUI.h>
#include <span>

namespace dse::ecl::lua
{
	using namespace dse::lua;

	LifetimeHolder GetClientLifetime();
	LifetimePool& GetClientLifetimePool();
	void RegisterClientLibraries(lua_State* L);

	class ExtensionLibraryClient : public ExtensionLibrary
	{
	public:
		void Register(lua_State * L) override;
		void RegisterLib(lua_State * L) override;
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
		stats::SkillPrototype* Skill;
		stats::Character* Character;
		ObjectSet<STDString> Params;
		bool IsFromItem;
		STDString Description;
	};

	struct StatusGetDescriptionEventParams
	{
		stats::StatusPrototype* Status;
		stats::ObjectInstance* Owner;
		stats::ObjectInstance* StatusSource;
		ObjectSet<STDString> Params;
		STDString Description;
	};

	struct SkillGetPropertyDescriptionEventParams
	{
		stats::PropertyExtender* Property;
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

		CustomDrawHelper& GetCustomDrawHelper()
		{
			return customDrawHelper_;
		}

		void Initialize() override;

		void OnCreateUIObject(ComponentHandle handle);
		void OnUICall(UIObject* ui, const char * func, unsigned int numArgs, ig::InvokeDataValue * args);
		void OnAfterUICall(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		void OnUIInvoke(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		void OnAfterUIInvoke(UIObject* ui, const char* func, unsigned int numArgs, ig::InvokeDataValue* args);
		std::optional<STDWString> SkillGetDescriptionParam(stats::SkillPrototype * prototype,
			stats::Character * character, ObjectSet<STDString> const & paramTexts, bool isFromItem);
		std::optional<STDWString> StatusGetDescriptionParam(stats::StatusPrototype * prototype, stats::ObjectInstance* owner,
			stats::ObjectInstance* statusSource, ObjectSet<STDString> const & paramTexts);
		void OnGameStateChanged(GameState fromState, GameState toState);
		std::optional<STDString> GetSkillPropertyDescription(stats::PropertyExtender*);
		void OnAppInputEvent(InputEvent const& inputEvent);

		void OnCustomClientUIObjectCreated(char const * name, ComponentHandle handle);
		UIObject * GetUIObject(char const * name);

	private:
		ExtensionLibraryClient library_;
		std::unordered_map<STDString, ComponentHandle> clientUI_;
		CustomDrawHelper customDrawHelper_;
	};
}
