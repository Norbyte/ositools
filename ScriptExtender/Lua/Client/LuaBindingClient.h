#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <GameDefinitions/UI.h>
#include <Lua/Client/ClientUI.h>
#include <GameDefinitions/GameObjects/Effect.h>
#include <span>

BEGIN_NS(ecl::lua::visual)

struct ClientMultiVisual : public MultiEffectHandler
{
public:
	void ParseFromStats(char const* effect, std::optional<char const*> weaponBones);
	Visual* AddVisual(lua_State* L, FixedString const& visualId);
	void Delete(lua_State* L);

	ObjectSet<ComponentHandle> AttachedVisuals;
	ComponentHandle Handle;
};

class VisualSystem
{
public:
	~VisualSystem();

	ClientMultiVisual* Get(ComponentHandle handle);
	ClientMultiVisual* Create();
	void Destroy(ClientMultiVisual* effect);
	void Update();

	void RequestDeleteVisual(ComponentHandle handle);

private:
	Map<ComponentHandle, GameUniquePtr<ClientMultiVisual>> visuals_;
	ObjectSet<ComponentHandle> pendingVisualDeletes_;
};

END_NS()

namespace dse::ecl::lua
{
	using namespace dse::lua;

	class ExtensionLibraryClient : public ExtensionLibrary
	{
	public:
		void Register(lua_State * L) override;
		void RegisterLib(lua_State * L) override;
	};

	struct GameStateChangedEvent : public EventBase
	{
		GameState FromState;
		GameState ToState;
	};

	struct UIObjectCreatedEvent : public EventBase
	{
		UIObject* UI;
	};

	struct UICallEvent : public EventBase
	{
		UIObject* UI;
		STDString Function;
		std::span<ig::InvokeDataValue> Args;
		char const* When;
	};

	struct SkillGetDescriptionParamEvent : public EventBase
	{
		stats::SkillPrototype* Skill;
		stats::Character* Character;
		ObjectSet<STDString> Params;
		bool IsFromItem;
		STDString Description;
	};

	struct StatusGetDescriptionParamEvent : public EventBase
	{
		stats::StatusPrototype* Status;
		stats::ObjectInstance* Owner;
		stats::ObjectInstance* StatusSource;
		ObjectSet<STDString> Params;
		STDString Description;
	};

	struct SkillGetPropertyDescriptionEvent : public EventBase
	{
		stats::PropertyExtender* Property;
		STDString Description;
	};

	struct InputEvent : public EventBase
	{
		dse::InputEvent* Event;
	};

	struct RawInputEvent : public EventBase
	{
		InjectInputData const* Input;
	};

	class ClientState : public State
	{
	public:
		static ClientState* FromLua(lua_State* L);

		ClientState();
		~ClientState();

		visual::VisualSystem& GetVisualSystem()
		{
			return visualSystem_;
		}

		CustomDrawHelper& GetCustomDrawHelper()
		{
			return customDrawHelper_;
		}

		void Initialize() override;
		bool IsClient() override;
		void OnUpdate(GameTime const& time) override;

		void OnCreateUIObject(ComponentHandle handle);
		EventResult OnUICall(UICallEvent& params);
		void OnAfterUICall(UICallEvent& params);
		EventResult OnUIInvoke(UICallEvent& params);
		void OnAfterUIInvoke(UICallEvent& params);
		std::optional<STDWString> SkillGetDescriptionParam(stats::SkillPrototype * prototype,
			stats::Character * character, ObjectSet<STDString> const & paramTexts, bool isFromItem);
		std::optional<STDWString> StatusGetDescriptionParam(stats::StatusPrototype * prototype, stats::ObjectInstance* owner,
			stats::ObjectInstance* statusSource, ObjectSet<STDString> const & paramTexts);
		void OnGameStateChanged(GameState fromState, GameState toState);
		std::optional<STDString> GetSkillPropertyDescription(stats::PropertyExtender*);
		void OnAppInputEvent(dse::InputEvent const& inputEvent);
		void OnRawInputEvent(InjectInputData const& inputEvent);

		void OnCustomClientUIObjectCreated(char const * name, ComponentHandle handle);
		UIObject * GetUIObject(char const * name);

	private:
		ExtensionLibraryClient library_;
		std::unordered_map<STDString, ComponentHandle> clientUI_;
		CustomDrawHelper customDrawHelper_;
		visual::VisualSystem visualSystem_;
	};
}
