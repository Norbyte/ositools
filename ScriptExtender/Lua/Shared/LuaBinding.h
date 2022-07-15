#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>

#include <Lua/Shared/Proxies/LuaArrayProxy.h>
#include <Lua/Shared/Proxies/LuaMapProxy.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <Lua/Shared/Proxies/LuaSetProxy.h>
#include <Lua/Shared/Proxies/LuaEvent.h>
#include <Lua/Shared/Proxies/LuaEntityProxy.h>

#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/GameObjects/Status.h>

#include <mutex>
#include <unordered_set>
#include <optional>

namespace dse::lua
{
	LUA_INFINITE_LIFETIME(stats::Object)
	LUA_INFINITE_LIFETIME(GameObjectTemplate)
	LUA_INFINITE_LIFETIME(EoCGameObjectTemplate)
	LUA_INFINITE_LIFETIME(CharacterTemplate)
	LUA_INFINITE_LIFETIME(ItemTemplate)
	LUA_INFINITE_LIFETIME(SurfaceTemplate)
	LUA_INFINITE_LIFETIME(ProjectileTemplate)
	LUA_INFINITE_LIFETIME(TriggerTemplate)

	void PushExtFunction(lua_State * L, char const * func);
	void PushInternalFunction(lua_State* L, char const* func);
	void PushModFunction(lua_State* L, char const* mod, char const* func);
	
	class StatsEntryProxyRefImpl : public ObjectProxyRefImpl<stats::Object>
	{
	public:
		StatsEntryProxyRefImpl(LifetimeHolder const& lifetime, stats::Object* obj, std::optional<int> level, bool legacy);

		FixedString const& GetTypeName() const override;
		bool GetProperty(lua_State* L, FixedString const& prop) override;
		bool SetProperty(lua_State* L, FixedString const& prop, int index) override;
		int Next(lua_State* L, FixedString const& key) override;

	private:
		std::optional<int> level_;
		// Use pre-v55 behavior?
		bool legacy_;
	};


	class StatsExtraDataProxy : public Userdata<StatsExtraDataProxy>, public Indexable, public NewIndexable
	{
	public:
		static char const * const MetatableName;

		int Index(lua_State * L);
		int NewIndex(lua_State * L);
	};

	struct ConsoleEventParams
	{
		STDString Command;
	};

	struct TickEventParams
	{
		GameTime Time;
	};

	struct NetMessageEventParams
	{
		STDString Channel;
		STDString Payload;
		UserId UserID;
	};

	struct GetHitChanceEventParams
	{
		stats::Character* Attacker;
		stats::Character* Target;
		std::optional<int32_t> HitChance;
	};

	struct GetSkillDamageEventParams
	{
		stats::SkillPrototype* Skill;
		stats::ObjectInstance* Attacker;
		bool IsFromItem;
		bool Stealthed;
		glm::vec3 AttackerPosition;
		glm::vec3 TargetPosition;
		int Level;
		bool NoRandomization;
		stats::DamagePairList DamageList;
		std::optional<stats::DeathType> DeathType;
	};

	struct GetSkillAPCostEventParams
	{
		stats::SkillPrototype* Skill;
		stats::Character* Character;
		eoc::AiGrid* AiGrid;
		glm::vec3* Position;
		float* Radius;
		std::optional<int> AP;
		std::optional<bool> ElementalAffinity;
	};

	class ExtensionLibrary
	{
	public:
		virtual void Register(lua_State * L);
		virtual void RegisterLib(lua_State * L) = 0;
	};

	class Exception : public std::runtime_error
	{
	public:
		Exception(std::string const& msg)
			: std::runtime_error("Lua error thrown: " + msg)
		{}
	};

	class State : Noncopyable<State>
	{
	public:
		enum RestrictionFlag
		{
			// Disable Osiris calls
			RestrictOsiris = 1 << 0,
			// Disable handle/guid to object conversion functions (Lua only)
			RestrictHandleConversion = 1 << 1,

			// Permit calls only available during module load state
			ScopeModuleLoad = 1 << 16,
			// Permit calls only available during session load state
			ScopeSessionLoad = 1 << 17,
			// Permit calls only available during module resume state
			ScopeModuleResume = 1 << 18,
			// Permit calls only available during initial module load state (before loading any files)
			ScopeModulePreLoad = 1 << 19,

			RestrictAll = 0x0000ffff,
		};

		uint32_t RestrictionFlags{ 0 };

		State();
		~State();

		State(State const &) = delete;
		State(State &&) = delete;
		State & operator = (State const &) = delete;
		State & operator = (State &&) = delete;
		
		static State* FromLua(lua_State* L);

		inline lua_State * GetState()
		{
			return L;
		}

		inline LifetimeStack& GetStack()
		{
			return lifetimeStack_;
		}

		inline bool StartupDone() const
		{
			return startupDone_;
		}

		LifetimeHolder GetCurrentLifetime();

		inline LifetimeHolder GetGlobalLifetime()
		{
			return globalLifetime_.Get();
		}

		inline LifetimePool& GetLifetimePool()
		{
			return lifetimePool_;
		}

		virtual void Initialize() = 0;
		virtual void Shutdown();
		virtual bool IsClient() = 0;

		void FinishStartup();
		void LoadBootstrap(STDString const& path, STDString const& modTable);
		virtual void OnGameSessionLoading();
		void OnGameSessionLoaded();
		void OnModuleLoadStarted();
		void OnModuleLoading();
		void OnStatsLoaded();
		void OnModuleResume();
		void OnResetCompleted();
		virtual void OnUpdate(GameTime const& time);

		std::optional<int> LoadScript(STDString const & script, STDString const & name = "", int globalsIdx = 0);

		std::optional<int32_t> GetHitChance(stats::Character * attacker, stats::Character * target);
		bool GetSkillDamage(stats::SkillPrototype * self, stats::DamagePairList * damageList,
			stats::ObjectInstance *attackerStats, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
			glm::vec3 const& targetPosition, stats::DeathType * pDeathType, int level, bool noRandomization);
		std::optional<std::pair<int, bool>> GetSkillAPCost(stats::SkillPrototype* skill, stats::Character* character, eoc::AiGrid* aiGrid,
			glm::vec3* position, float* radius);
		void OnNetMessageReceived(STDString const & channel, STDString const & payload, UserId userId);

	protected:
		lua_State * L;
		bool startupDone_{ false };

		LifetimePool lifetimePool_;
		LifetimeStack lifetimeStack_;
		LifetimeReference globalLifetime_;

		void OpenLibs();
	};

	class Restriction
	{
	public:
		inline Restriction(State & state, uint32_t flags)
			: state_(state), oldFlags_(state_.RestrictionFlags)
		{
			state_.RestrictionFlags |= flags;
		}

		inline ~Restriction()
		{
			state_.RestrictionFlags = oldFlags_;
		}

	private:
		State & state_;
		uint32_t oldFlags_;
	};

	enum class EventResult
	{
		Successful,
		Failed,
		ActionPrevented
	};

	template <class TEvent>
	EventResult ThrowEvent(State& state, char const* eventName, TEvent& evt, bool canPreventAction = false, uint32_t restrictions = 0)
	{
		auto L = state.GetState();
		auto stackSize = lua_gettop(L);

		try {
			StackCheck _(L, 0);
			Restriction restriction(state, restrictions);
			LifetimePin _p(state.GetStack());
			PushInternalFunction(L, "_ThrowEvent");
			auto luaEvent = EventObject::Make(L, state.GetCurrentLifetime(), eventName, evt, canPreventAction, WriteableEvent{});
			if (!CheckedCall(L, 1, "_ThrowEvent")) {
				return EventResult::Failed;
			}

			if (luaEvent->IsActionPrevented()) {
				return EventResult::ActionPrevented;
			} else {
				return EventResult::Successful;
			}
		} catch (Exception &) {
			auto stackRemaining = lua_gettop(L) - stackSize;
			if (stackRemaining > 0) {
				LuaError("Failed to dispatch event '" << eventName << "': " << lua_tostring(L, -1));
				lua_pop(L, stackRemaining);
			} else {
				LuaError("Internal error while dispatching event '" << eventName << "'");
			}

			return EventResult::Failed;
		}
	}

	void WarnDeprecated56(char const* msg);

	int GetCellInfo(lua_State* L);
}
