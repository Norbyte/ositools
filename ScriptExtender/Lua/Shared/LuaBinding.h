#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>

#include <Lua/Shared/Proxies/LuaArrayProxy.h>
#include <Lua/Shared/Proxies/LuaMapProxy.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <Lua/Shared/Proxies/LuaEvent.h>
#include <Lua/Shared/Proxies/LuaEntityProxy.h>
#include <Lua/Shared/Proxies/LuaCppClass.h>
#include <Lua/Shared/Proxies/LuaCppObjectProxy.h>
#include <Lua/Shared/Proxies/LuaCppValue.h>
#include <Lua/Shared/Proxies/LuaEnumValue.h>
#include <Lua/Shared/Proxies/LuaBitfieldValue.h>

#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <Extender/Shared/CustomDamageTypes.h>

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
		StatsEntryProxyRefImpl(LifetimeHandle const& lifetime, stats::Object* obj, std::optional<int> level, bool legacy);

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

	struct DoConsoleCommandEvent : public EventBase
	{
		STDString Command;
	};

	struct TickEvent : public EventBase
	{
		GameTime Time;
	};

	struct NetMessageEvent : public EventBase
	{
		STDString Channel;
		STDString Payload;
		UserId UserID;
	};

	struct GetHitChanceEvent : public EventBase
	{
		stats::Character* Attacker;
		stats::Character* Target;
		std::optional<int32_t> HitChance;
	};

	struct GetSkillDamageEvent : public EventBase
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

	struct GetSkillAPCostEvent : public EventBase
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

	enum class EventResult
	{
		Successful,
		Failed,
		ActionPrevented
	};

	struct LuaInternalState;

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

		State(uint32_t generationId);
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

		inline uint32_t GetGenerationId()
		{
			return generationId_;
		}

		inline LuaInternalState* GetInternalState()
		{
			return internal_;
		}

		inline LifetimeStack& GetStack()
		{
			return lifetimeStack_;
		}

		inline bool StartupDone() const
		{
			return startupDone_;
		}

		LifetimeHandle GetCurrentLifetime();

		inline LifetimeHandle GetGlobalLifetime()
		{
			return globalLifetime_;
		}

		inline LifetimePool& GetLifetimePool()
		{
			return lifetimePool_;
		}

		inline CppMetatableManager& GetMetatableManager()
		{
			return metatableManager_;
		}

		inline CustomDamageTypeCallbackManager& GetCustomDamageTypes()
		{
			return customDamageTypes_;
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
		void OnStatsStructureLoaded();

		std::optional<int> LoadScript(STDString const & script, STDString const & name = "", int globalsIdx = 0);

		std::optional<int32_t> GetHitChance(stats::Character * attacker, stats::Character * target);
		bool GetSkillDamage(stats::SkillPrototype * self, stats::DamagePairList * damageList,
			stats::ObjectInstance *attackerStats, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
			glm::vec3 const& targetPosition, stats::DeathType * pDeathType, int level, bool noRandomization);
		std::optional<std::pair<int, bool>> GetSkillAPCost(stats::SkillPrototype* skill, stats::Character* character, eoc::AiGrid* aiGrid,
			glm::vec3* position, float* radius);
		void OnNetMessageReceived(STDString const & channel, STDString const & payload, UserId userId);

		template <class TEvent>
		EventResult ThrowEvent(char const* eventName, TEvent& evt, bool canPreventAction = false, uint32_t restrictions = 0)
		{
			static_assert(std::is_base_of_v<EventBase, TEvent>, "Event object must be a descendant of EventBase");
			StackCheck _(L, 0);
			LifetimeStackPin _p(GetStack());
			PushInternalFunction(L, "_ThrowEvent");
			LightObjectProxyByRefMetatable::Make(L, &evt, GetCurrentLifetime());
			return DispatchEvent(evt, eventName, canPreventAction, restrictions);
		}

	protected:
		lua_State * L;
		LuaInternalState* internal_{ nullptr };
		bool startupDone_{ false };
		uint32_t generationId_;

		LifetimePool lifetimePool_;
		LifetimeStack lifetimeStack_;
		LifetimeHandle globalLifetime_;

		CppMetatableManager metatableManager_;

		CustomDamageTypeCallbackManager customDamageTypes_;

		void OpenLibs();
		EventResult DispatchEvent(EventBase& evt, char const* eventName, bool canPreventAction, uint32_t restrictions);
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

	void WarnDeprecated56(char const* msg);

	int GetCellInfo(lua_State* L);
}
