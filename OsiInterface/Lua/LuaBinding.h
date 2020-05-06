#pragma once

#include <GameDefinitions/Character.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/Status.h>
#include <Lua/LuaHelpers.h>

#include <mutex>
#include <unordered_set>
#include <optional>


namespace dse::lua
{
	void PushExtFunction(lua_State * L, char const * func);
	void PushModFunction(lua_State* L, char const* mod, char const* func);

	template <class T>
	class ObjectProxy : public Userdata<ObjectProxy<T>>, public Indexable, public NewIndexable, public Pushable<PushPolicy::Unbind>
	{
	public:
		static char const * const MetatableName;

		ObjectProxy(T * obj)
			: obj_(obj), handle_()
		{}

		ObjectProxy(ObjectHandle handle)
			: obj_(nullptr), handle_(handle)
		{}

		void Unbind()
		{
			obj_ = nullptr;
		}

		int Index(lua_State * L);
		int NewIndex(lua_State * L);
		T* Get(lua_State* L);

	private:
		T * obj_;
		ObjectHandle handle_;
	};


	class StatsProxy : public Userdata<StatsProxy>, public Indexable, public NewIndexable, public Pushable<PushPolicy::Unbind>
	{
	public:
		static char const * const MetatableName;

		StatsProxy(CRPGStats_Object * obj, std::optional<int> level)
			: obj_(obj), level_(level)
		{}

		void Unbind()
		{
			obj_ = nullptr;
		}

		int Index(lua_State * L);
		int NewIndex(lua_State * L);

	private:
		CRPGStats_Object * obj_;
		std::optional<int> level_;
	};


	class SkillPrototypeProxy : public Userdata<SkillPrototypeProxy>, public Indexable, public Pushable<PushPolicy::Unbind>
	{
	public:
		static char const * const MetatableName;

		SkillPrototypeProxy(SkillPrototype * obj, std::optional<int> level);

		void Unbind()
		{
			obj_ = nullptr;
		}

		int Index(lua_State * L);

	private:
		SkillPrototype * obj_;
		CRPGStats_Object * stats_;
		std::optional<int> level_;
	};


	class ItemOrCharacterPushPin
	{
	public:
		ItemOrCharacterPushPin(lua_State * L, CRPGStats_Object * obj);
		~ItemOrCharacterPushPin();

	private:
		ObjectProxy<CDivinityStats_Character> * character_{ nullptr };
		ObjectProxy<CDivinityStats_Item> * item_{ nullptr };
		StatsProxy * object_{ nullptr };
	};


	class StatsExtraDataProxy : public Userdata<StatsExtraDataProxy>, public Indexable, public Pushable<PushPolicy::None>
	{
	public:
		static char const * const MetatableName;

		int Index(lua_State * L);
	};


	class DamageList : public Userdata<DamageList>, public Pushable<PushPolicy::None>
	{
	public:
		static char const * const MetatableName;

		static void PopulateMetatable(lua_State * L);

		inline DamagePairList & Get()
		{
			return damages_;
		}

	private:
		DamagePairList damages_;

		static int GetByType(lua_State * L);
		static int Add(lua_State * L);
		static int Clear(lua_State * L);
		static int Multiply(lua_State * L);
		static int Merge(lua_State * L);
		static int ConvertDamageType(lua_State * L);
		static int AggregateSameTypeDamages(lua_State * L);
		static int ToTable(lua_State * L);
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

	class State
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

		inline lua_State * GetState()
		{
			return L;
		}

		inline bool StartupDone() const
		{
			return startupDone_;
		}

		inline std::recursive_mutex & GetMutex()
		{
			return mutex_;
		}

		void FinishStartup();
		void LoadBootstrap(STDString const& path, STDString const& modTable);
		virtual void OnGameSessionLoading();
		void OnGameSessionLoaded();
		void OnModuleLoadStarted();
		void OnModuleLoading();
		void OnModuleResume();

		template <class... Ret, class... Args>
		auto CallExt(char const * func, uint32_t restrictions, ReturnType<Ret...>, Args... args)
		{
			std::lock_guard lock(mutex_);
			Restriction restriction(*this, restrictions);
			PushExtFunction(L, func);
			auto _{ PushArguments(L, std::tuple{args...}) };
			return CheckedCall<Ret...>(L, sizeof...(args), func);
		}

		std::optional<int> LoadScript(STDString const & script, STDString const & name = "", int globalsIdx = 0);

		std::optional<int32_t> GetHitChance(CDivinityStats_Character * attacker, CDivinityStats_Character * target);
		bool GetSkillDamage(SkillPrototype * self, DamagePairList * damageList,
			CRPGStats_ObjectInstance *attackerStats, bool isFromItem, bool stealthed, float * attackerPosition,
			float * targetPosition, DeathType * pDeathType, int level, bool noRandomization);
		void OnNetMessageReceived(STDString const & channel, STDString const & payload, UserId userId);

	protected:
		lua_State * L;
		std::recursive_mutex mutex_;
		bool startupDone_{ false };

		void OpenLibs();

		static STDString GetBuiltinLibrary(int resourceId);
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


	int GetExtensionVersion(lua_State* L);
	int MonotonicTime(lua_State* L);
	int OsiPrint(lua_State* L);
	int OsiPrintWarning(lua_State* L);
	int OsiPrintError(lua_State* L);
	int HandleToDouble(lua_State* L);
	int SaveFile(lua_State* L);
	int LoadFile(lua_State* L);
	int JsonParse(lua_State* L);
	int JsonStringify(lua_State* L);
	int IsModLoaded(lua_State* L);
	int GetModLoadOrder(lua_State* L);
	int GetModInfo(lua_State* L);
	int GetStatEntries(lua_State* L);
	int GetStatEntriesLoadedBefore(lua_State* L);
	int GetSkillSet(lua_State* L);
	int GetEquipmentSet(lua_State* L);
	int StatGetAttribute(lua_State* L);
	int StatSetAttribute(lua_State* L);
	int StatAddCustomDescription(lua_State* L);
	int StatSetLevelScaling(lua_State* L);
	int GetStat(lua_State* L);
	int CreateStat(lua_State* L);
	int SyncStat(lua_State* L);
	int GetDeltaMod(lua_State* L);
	int UpdateDeltaMod(lua_State* L);
	int NewDamageList(lua_State* L);
	int IsDeveloperMode(lua_State* L);
	int AddPathOverride(lua_State* L);
	int LuaRandom(lua_State* L);
	int LuaRound(lua_State* L);
	int AddVoiceMetaData(lua_State* L);
	int GetTranslatedString(lua_State* L);
	int GetTranslatedStringFromKey(lua_State* L);
	int GenerateIdeHelpers(lua_State* L);
}
