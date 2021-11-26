#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>

#include <Lua/Shared/Proxies/LuaArrayProxy.h>
#include <Lua/Shared/Proxies/LuaMapProxy.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <Lua/Shared/Proxies/LuaSetProxy.h>
#include <Lua/Shared/Proxies/LuaEvent.h>
// #include <Lua/Shared/LuaEntityProxy.h>

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
	LifetimeHolder GetCurrentLifetime();

	template <class T>
	class ObjectProxy : public Userdata<ObjectProxy<T>>, public Indexable, public NewIndexable, public Pushable<PushPolicy::Unbind>
	{
	public:
		static char const * const MetatableName;

		ObjectProxy(T * obj, LifetimeHolder const& lifetime)
			: obj_(obj), handle_(), lifetime_(lifetime)
		{}

		ObjectProxy(LifetimeHolder const& lifetime, T * obj)
			: obj_(obj), handle_(), lifetime_(lifetime)
		{}

		ObjectProxy(ComponentHandle handle)
			: obj_(nullptr), handle_(handle)
		{}

		void Unbind()
		{
			obj_ = nullptr;
		}

		T* Get(lua_State* L)
		{
			auto ptr = GetPtr(L);
			if (lifetime_ && !lifetime_->IsAlive()) {
				luaL_error(L, "Attempted to access object of type '%s' whose lifetime has expired", MetatableName);
				return nullptr;
			}

			return ptr;
		}

		int Index(lua_State * L);
		int NewIndex(lua_State * L);

		static T* CheckedGet(lua_State* L, int index)
		{
			auto self = ObjectProxy<T>::CheckUserData(L, index);
			auto obj = self->Get(L);
			if (obj != nullptr) {
				return obj;
			} else {
				luaL_error(L, "No object bound to object proxy of type '%s'", MetatableName);
				return nullptr;
			}
		}

	protected:
		T* GetPtr(lua_State* L);

		int GenericGetter(lua_State* L, PropertyMapBase const& propertyMap)
		{
			StackCheck _(L, 1);
			auto obj = Get(L);
			if (!obj) {
				push(L, nullptr);
				return 1;
			}

			auto prop = luaL_checkstring(L, 2);
			auto fetched = LuaPropertyMapGet(L, propertyMap, obj, prop, true);
			if (!fetched) {
				push(L, nullptr);
			}

			return 1;
		}

		int GenericSetter(lua_State* L, PropertyMapBase const& propertyMap)
		{
			StackCheck _(L, 0);
			auto obj = Get(L);
			if (!obj) return 0;

			auto prop = luaL_checkstring(L, 2);
			LuaPropertyMapSet(L, 3, propertyMap, obj, prop, true);

			return 0;
		}

	private:
		T * obj_;
		ComponentHandle handle_;
		std::optional<LifetimeReference> lifetime_;
	};


	
	class StatsEntryProxyRefImpl : public ObjectProxyRefImpl<stats::Object>
	{
	public:
		StatsEntryProxyRefImpl(LifetimeHolder const& lifetime, stats::Object* obj, std::optional<int> level);

		FixedString const& GetTypeName() const override;
		bool GetProperty(lua_State* L, FixedString const& prop) override;
		bool SetProperty(lua_State* L, FixedString const& prop, int index) override;
		int Next(lua_State* L, FixedString const& key) override;

	private:
		std::optional<int> level_;
	};


	class StatsExtraDataProxy : public Userdata<StatsExtraDataProxy>, public Indexable, 
		public NewIndexable, public Pushable<PushPolicy::None>
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

	protected:
		static int Include(lua_State * L);
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
		std::unordered_set<int32_t> OverriddenLevelMaps;

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

		void FinishStartup();
		void LoadBootstrap(STDString const& path, STDString const& modTable);
		virtual void OnGameSessionLoading();
		void OnGameSessionLoaded();
		void OnModuleLoadStarted();
		void OnModuleLoading();
		void OnStatsLoaded();
		void OnModuleResume();
		void OnResetCompleted();

		std::optional<int> LoadScript(STDString const & script, STDString const & name = "", int globalsIdx = 0);

		std::optional<int32_t> GetHitChance(stats::Character * attacker, stats::Character * target);
		bool GetSkillDamage(stats::SkillPrototype * self, stats::DamagePairList * damageList,
			stats::ObjectInstance *attackerStats, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
			glm::vec3 const& targetPosition, stats::DeathType * pDeathType, int level, bool noRandomization);
		std::optional<std::pair<int, bool>> GetSkillAPCost(stats::SkillPrototype* skill, stats::Character* character, eoc::AiGrid* aiGrid,
			glm::vec3* position, float* radius);
		void OnNetMessageReceived(STDString const & channel, STDString const & payload, UserId userId);

		static STDString GetBuiltinLibrary(int resourceId);

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

	template <class TEvent>
	bool ThrowEvent(State& state, char const* eventName, TEvent& evt, bool canPreventAction = false, uint32_t restrictions = 0)
	{
		auto L = state.GetState();
		auto stackSize = lua_gettop(L);

		try {
			StackCheck _(L, 0);
			Restriction restriction(state, restrictions);
			LifetimePin _p(state.GetStack());
			PushInternalFunction(L, "_ThrowEvent");
			EventObject::Make(L, state.GetStack().GetCurrent().pool, eventName, evt, canPreventAction, WriteableEvent{});
			return CheckedCall(L, 1, "_ThrowEvent");
		} catch (Exception &) {
			auto stackRemaining = lua_gettop(L) - stackSize;
			if (stackRemaining > 0) {
				LuaError("Failed to dispatch event '" << eventName << "': " << lua_tostring(L, -1));
				lua_pop(L, stackRemaining);
			} else {
				LuaError("Internal error while dispatching event '" << eventName << "'");
			}

			return false;
		}
	}

	void WarnDeprecated56(char const* msg);

	void RegisterStatsObjects(lua_State* L);

	int GetExtensionVersionWrapper(lua_State* L);
	int GetGameVersionWrapper(lua_State* L);
	int MonotonicTimeWrapper(lua_State* L);
	int OsiPrint(lua_State* L);
	int OsiPrintWarning(lua_State* L);
	int OsiPrintError(lua_State* L);
	int HandleToDoubleWrapper(lua_State* L);
	int DoubleToHandleWrapper(lua_State* L);
	int GetHandleTypeWrapper(lua_State* L);
	int GetCellInfo(lua_State* L);

	int SaveFileWrapper(lua_State* L);
	int LoadFileWrapper(lua_State* L);

	int IsModLoadedWrapper(lua_State* L);
	int GetModLoadOrder(lua_State* L);
	int GetModInfo(lua_State* L);
	int LuaDebugBreakWrapper(lua_State* L);

	int GetStatEntries(lua_State* L);
	int GetStatEntriesLoadedBefore(lua_State* L);
	int GetSkillSet(lua_State* L);
	int UpdateSkillSet(lua_State* L);
	int GetEquipmentSet(lua_State* L);
	int UpdateEquipmentSet(lua_State* L);
	int GetTreasureTable(lua_State* L);
	int UpdateTreasureTable(lua_State* L);
	int GetTreasureCategory(lua_State* L);
	int UpdateTreasureCategory(lua_State* L);
	int GetItemCombo(lua_State* L);
	int UpdateItemCombo(lua_State* L);
	int GetItemComboPreviewData(lua_State* L);
	int UpdateItemComboPreviewData(lua_State* L);
	int GetItemComboProperty(lua_State* L);
	int UpdateItemComboProperty(lua_State* L);
	int GetItemGroup(lua_State* L);
	int GetNameGroup(lua_State* L);

	int StatGetAttribute(lua_State* L);
	int StatSetAttribute(lua_State* L);
	int StatAddCustomDescriptionWrapper(lua_State* L);
	int StatSetLevelScaling(lua_State* L);
	int GetStat(lua_State* L);
	int CreateStat(lua_State* L);
	int SyncStatWrapper(lua_State* L);
	int StatSetPersistenceWrapper(lua_State* L);
	int GetDeltaMod(lua_State* L);
	int UpdateDeltaMod(lua_State* L);

	int EnumIndexToLabel(lua_State* L);
	int EnumLabelToIndex(lua_State* L);
	int NewDamageList(lua_State* L);
	int GetSurfaceTemplate(lua_State* L);

	int IsDeveloperModeWrapper(lua_State* L);
	int GetDifficultyWrapper(lua_State* L);
	int GetGameModeWrapper(lua_State* L);
	int AddPathOverrideWrapper(lua_State* L);
	int GetPathOverrideWrapper(lua_State* L);
	int LuaRandom(lua_State* L);
	int LuaRoundWrapper(lua_State* L);
	int AddVoiceMetaDataWrapper(lua_State* L);
	int GetTranslatedStringWrapper(lua_State* L);
	int GetTranslatedStringFromKeyWrapper(lua_State* L);
	int CreateTranslatedStringKeyWrapper(lua_State* L);
	int CreateTranslatedStringHandleWrapper(lua_State* L);
	int CreateTranslatedStringWrapper(lua_State* L);
	int GenerateIdeHelpersWrapper(lua_State* L);

	int EnableExperimentalPropertyWritesWrapper(lua_State* L);
	int DumpStackWrapper(lua_State* L);
	int ShowErrorAndExitGameWrapper(lua_State* L);
}
