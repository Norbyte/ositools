#pragma once

#include <CustomFunctions.h>
#include <OsirisHelpers.h>

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <mutex>

namespace osidbg
{
	void LuaToOsi(lua_State * L, int i, TypedValue & tv, ValueType osiType);
	TypedValue * LuaToOsi(lua_State * L, int i, ValueType osiType);
	void LuaToOsi(lua_State * L, int i, OsiArgumentValue & arg, ValueType osiType);
	void OsiToLua(lua_State * L, OsiArgumentValue const & arg);
	void OsiToLua(lua_State * L, TypedValue const & tv);

	inline void lua_push(lua_State * L, nullptr_t v)
	{
		lua_pushnil(L);
	}

	inline void lua_push(lua_State * L, bool v)
	{
		lua_pushboolean(L, v ? 1 : 0);
	}

	inline void lua_push(lua_State * L, int32_t v)
	{
		lua_pushinteger(L, (lua_Integer)v);
	}

	inline void lua_push(lua_State * L, uint32_t v)
	{
		lua_pushinteger(L, (lua_Integer)v);
	}

	inline void lua_push(lua_State * L, int64_t v)
	{
		lua_pushinteger(L, v);
	}

	inline void lua_push(lua_State * L, uint64_t v)
	{
		lua_pushinteger(L, (lua_Integer)v);
	}

	inline void lua_push(lua_State * L, double v)
	{
		lua_pushnumber(L, v);
	}

	inline void lua_push(lua_State * L, char const * v)
	{
		lua_pushstring(L, v);
	}

	inline void lua_push(lua_State * L, FixedString v)
	{
		lua_pushstring(L, v.Str);
	}

	inline void lua_push(lua_State * L, std::string const & v)
	{
		lua_pushstring(L, v.c_str());
	}

	inline void lua_push(lua_State * L, std::wstring const & v)
	{
		lua_pushstring(L, ToUTF8(v).c_str());
	}

	inline void lua_push(lua_State * L, STDString const & v)
	{
		lua_pushstring(L, v.GetPtr());
	}

	inline void lua_push(lua_State * L, STDWString const & v)
	{
		lua_pushstring(L, ToUTF8(v.GetPtr()).c_str());
	}

	template <class TKey, class TValue>
	inline void luaL_settable(lua_State * L, TKey const & k, TValue const & v, int index = -3)
	{
		lua_push(L, k);
		lua_push(L, v);
		lua_settable(L, index);
	}

	template <class TValue>
	TValue luaL_get(lua_State * L, int index = -1);

	template <>
	inline bool luaL_get<bool>(lua_State * L, int index)
	{
		return lua_toboolean(L, index) == 1;
	}

	template <>
	inline int32_t luaL_get<int32_t>(lua_State * L, int index)
	{
		return (int32_t)lua_tointeger(L, index);
	}

	template <>
	inline int64_t luaL_get<int64_t>(lua_State * L, int index)
	{
		return lua_tointeger(L, index);
	}

	template <>
	inline double luaL_get<double>(lua_State * L, int index)
	{
		return lua_tonumber(L, index);
	}

	template <>
	inline char const * luaL_get<char const *>(lua_State * L, int index)
	{
		return lua_tostring(L, index);
	}

	template <class TKey, class TValue>
	TValue luaL_gettable(lua_State * L, TKey const & k, int index = -2)
	{
		lua_push(L, k);
		lua_gettable(L, index);
		TValue val = luaL_get<TValue>(L, -1);
		lua_pop(L, 1);
		return val;
	}

	template <class TEnum>
	std::optional<TEnum> lua_toenum(lua_State * L, int index)
	{
		switch (lua_type(L, index)) {
		case LUA_TSTRING:
		{
			auto val = lua_tostring(L, index);
			auto index = EnumInfo<TEnum>::Find(val);
			if (index) {
				return (TEnum)*index;
			} else {
				return {};
			}
			break;
		}

		case LUA_TNUMBER:
		{
			auto val = lua_tointeger(L, index);
			auto index = EnumInfo<TEnum>::Find((TEnum)val);
			if (index) {
				return (TEnum)val;
			} else {
				return {};
			}
			break;
		}

		default:
			return {};
		}
	}

	template <class TEnum>
	TEnum lua_checkenum(lua_State * L, int index)
	{
		switch (lua_type(L, index)) {
		case LUA_TSTRING:
		{
			auto val = lua_tostring(L, index);
			auto index = EnumInfo<TEnum>::Find(val);
			if (index) {
				return (TEnum)*index;
			} else {
				luaL_error(L, "Param %d is not a valid enum label", index);
			}
			break;
		}

		case LUA_TNUMBER:
		{
			auto val = lua_tointeger(L, index);
			auto index = EnumInfo<TEnum>::Find((TEnum)val);
			if (index) {
				return (TEnum)val;
			} else {
				luaL_error(L, "Param %d is not a valid enum label", index);
			}
			break;
		}

		default:
			luaL_error(L, "Param %d must be an integer or string enumeration label", index);
		}

		return (TEnum)0;
	}

	class LuaRegistryEntry
	{
	public:
		LuaRegistryEntry(lua_State * L, int index);
		~LuaRegistryEntry();

		LuaRegistryEntry(LuaRegistryEntry const &) = delete;
		LuaRegistryEntry(LuaRegistryEntry &&);

		LuaRegistryEntry & operator = (LuaRegistryEntry const &) = delete;
		LuaRegistryEntry & operator = (LuaRegistryEntry &&);

		void Push();

	private:
		lua_State * L_;
		int ref_;
	};

	class LuaCallable {};
	class LuaIndexable {};
	class LuaNewIndexable {};

	template <class T>
	class LuaUserdata
	{
	public:
		static T * AsUserData(lua_State * L, int index)
		{
			if (lua_type(L, index) == LUA_TUSERDATA) {
				auto obj = luaL_testudata(L, index, T::MetatableName);
				return reinterpret_cast<T *>(obj);
			} else {
				return nullptr;
			}
		}

		static T * CheckUserData(lua_State * L, int index)
		{
			return reinterpret_cast<T *>(luaL_checkudata(L, index, T::MetatableName));
		}

		template <class... Args>
		static T * New(lua_State * L, Args... args)
		{
			auto obj = reinterpret_cast<T *>(lua_newuserdata(L, sizeof(T)));
			new (obj) T(std::forward<Args>(args)...);
			luaL_setmetatable(L, T::MetatableName);
			return obj;
		}

		static int LuaCallProxy(lua_State * L)
		{
			if constexpr (std::is_base_of_v<LuaCallable, T>) {
				auto self = CheckUserData(L, 1);
				return self->LuaCall(L);
			} else {
				return luaL_error(L, "Not callable!");
			}
		}

		static int LuaIndexProxy(lua_State * L)
		{
			if constexpr (std::is_base_of_v<LuaIndexable, T>) {
				auto self = CheckUserData(L, 1);
				return self->LuaIndex(L);
			} else {
				return luaL_error(L, "Not indexable!");
			}
		}

		static int LuaNewIndexProxy(lua_State * L)
		{
			if constexpr (std::is_base_of_v<LuaNewIndexable, T>) {
				auto self = CheckUserData(L, 1);
				return self->LuaNewIndex(L);
			} else {
				return luaL_error(L, "Not newindexable!");
			}
		}

		static void PopulateMetatable(lua_State * L)
		{
			// Add custom metatable items by overriding this in subclasses
		}

		static void RegisterMetatable(lua_State * L)
		{
			lua_register(L, T::MetatableName, nullptr);
			luaL_newmetatable(L, T::MetatableName); // stack: mt

			if constexpr (std::is_base_of_v<LuaCallable, T>) {
				lua_pushcfunction(L, &LuaCallProxy); // stack: mt, &LuaCall
				lua_setfield(L, -2, "__call"); // mt.__call = &LuaCall; stack: mt
			}

			if constexpr (std::is_base_of_v<LuaIndexable, T>) {
				lua_pushcfunction(L, &LuaIndexProxy); // stack: mt, &LuaIndex
				lua_setfield(L, -2, "__index"); // mt.__index = &LuaIndex; stack: mt
			}

			if constexpr (std::is_base_of_v<LuaNewIndexable, T>) {
				lua_pushcfunction(L, &LuaNewIndexProxy); // stack: mt, &LuaNewIndex
				lua_setfield(L, -2, "__newindex"); // mt.__index = &LuaNewIndex; stack: mt
			}

			T::PopulateMetatable(L);

			lua_pop(L, 1); // stack: -
		}
	};

	class LuaOsiFunction
	{
	public:
		inline bool IsBound() const
		{
			return function_ != nullptr;
		}

		bool Bind(Function const * func, class LuaState & state);
		void Unbind();

		int LuaCall(lua_State * L);

	private:
		Function const * function_{ nullptr };
		AdapterRef adapter_;
		LuaState * state_;

		void OsiCall(lua_State * L);
		void OsiInsert(lua_State * L, bool deleteTuple);
		int OsiQuery(lua_State * L);
		int OsiUserQuery(lua_State * L);
	};

	class LuaOsiFunctionNameProxy : public LuaUserdata<LuaOsiFunctionNameProxy>, public LuaCallable
	{
	public:
		static char const * const MetatableName;
		// Maximum number of OUT params that a query can return.
		// (This setting determines how many function arities we'll check during name lookup)
		static constexpr uint32_t MaxQueryOutParams = 6;

		LuaOsiFunctionNameProxy(std::string const & name, LuaState & state);

		void UnbindAll();
		int LuaCall(lua_State * L);

	private:
		std::string name_;
		std::vector<LuaOsiFunction> functions_;
		LuaState & state_;
		uint32_t generationId_;

		LuaOsiFunction * TryGetFunction(uint32_t arity);
		LuaOsiFunction * CreateFunctionMapping(uint32_t arity, Function const * func);
		Function const * LookupOsiFunction(uint32_t arity);
	};


	class CustomLuaCall : public CustomCallBase
	{
	public:
		inline CustomLuaCall(std::string const & name, std::vector<CustomFunctionParam> params,
			LuaRegistryEntry handler)
			: CustomCallBase(name, std::move(params)), handler_(std::move(handler))
		{}

		virtual bool Call(OsiArgumentDesc const & params) override;

	private:
		LuaRegistryEntry handler_;
	};


	class CustomLuaQuery : public CustomQueryBase
	{
	public:
		inline CustomLuaQuery(std::string const & name, std::vector<CustomFunctionParam> params,
			LuaRegistryEntry handler)
			: CustomQueryBase(name, std::move(params)), handler_(std::move(handler))
		{}

		virtual bool Query(OsiArgumentDesc & params) override;

	private:
		LuaRegistryEntry handler_;
	};

	template <class T>
	class LuaObjectProxy : public LuaUserdata<LuaObjectProxy<T>>, public LuaIndexable, public LuaNewIndexable
	{
	public:
		static char const * const MetatableName;

		LuaObjectProxy(T * obj)
			: obj_(obj)
		{}

		void Unbind()
		{
			obj_ = nullptr;
		}

		int LuaIndex(lua_State * L);
		int LuaNewIndex(lua_State * L);

	private:
		T * obj_;
	};

	class LuaStatsProxy : public LuaUserdata<LuaStatsProxy>, public LuaIndexable, public LuaNewIndexable
	{
	public:
		static char const * const MetatableName;

		LuaStatsProxy(CRPGStats_Object * obj, std::optional<int> level)
			: obj_(obj), level_(level)
		{}

		void Unbind()
		{
			obj_ = nullptr;
		}

		int LuaIndex(lua_State * L);
		int LuaNewIndex(lua_State * L);

	private:
		CRPGStats_Object * obj_;
		std::optional<int> level_;
	};

	class LuaStatsPin
	{
	public:
		inline LuaStatsPin(LuaStatsProxy * proxy)
			: proxy_(proxy)
		{}

		inline ~LuaStatsPin()
		{
			if (proxy_) proxy_->Unbind();
		}

	private:
		LuaStatsProxy * proxy_;
	};

	class LuaSkillPrototypeProxy : public LuaUserdata<LuaSkillPrototypeProxy>, public LuaIndexable
	{
	public:
		static char const * const MetatableName;

		LuaSkillPrototypeProxy(SkillPrototype * obj, std::optional<int> level);

		void Unbind()
		{
			obj_ = nullptr;
		}

		int LuaIndex(lua_State * L);

	private:
		SkillPrototype * obj_;
		CRPGStats_Object * stats_;
		std::optional<int> level_;
	};

	class LuaSkillPrototypePin
	{
	public:
		inline LuaSkillPrototypePin(LuaSkillPrototypeProxy * proxy)
			: proxy_(proxy)
		{}

		inline ~LuaSkillPrototypePin()
		{
			if (proxy_) proxy_->Unbind();
		}

	private:
		LuaSkillPrototypeProxy * proxy_;
	};

	template <class T>
	class LuaGameObjectPin
	{
	public:
		inline LuaGameObjectPin(LuaObjectProxy<T> * proxy)
			: proxy_(proxy)
		{}

		inline ~LuaGameObjectPin()
		{
			if (proxy_) proxy_->Unbind();
		}

	private:
		LuaObjectProxy<T> * proxy_;
	};

	template <class T>
	class LuaHandleProxy : public LuaUserdata<LuaHandleProxy<T>>, public LuaIndexable
	{
	public:
		static char const * const MetatableName;

		LuaHandleProxy(ObjectHandle handle)
			: handle_(handle)
		{}

		int LuaIndex(lua_State * L);
		int LuaNewIndex(lua_State * L);

	private:
		ObjectHandle handle_;
	};

	class LuaStatusHandleProxy : public LuaUserdata<LuaStatusHandleProxy>, public LuaIndexable
	{
	public:
		static char const * const MetatableName;

		inline LuaStatusHandleProxy(ObjectHandle character, ObjectHandle status)
			: character_(character), status_(status)
		{}

		int LuaIndex(lua_State * L);
		int LuaNewIndex(lua_State * L);

	private:
		ObjectHandle character_;
		ObjectHandle status_;
	};

	class LuaStatsExtraDataProxy : public LuaUserdata<LuaStatsExtraDataProxy>, public LuaIndexable
	{
	public:
		static char const * const MetatableName;

		int LuaIndex(lua_State * L);
	};

	class LuaDamageList : public LuaUserdata<LuaDamageList>
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

		static int Add(lua_State * L);
		static int Clear(lua_State * L);
		static int Multiply(lua_State * L);
		static int Merge(lua_State * L);
		static int ConvertDamageType(lua_State * L);
		static int AggregateSameTypeDamages(lua_State * L);
		static int ToTable(lua_State * L);
	};

	class LuaTurnManagerCombatProxy : public LuaUserdata<LuaTurnManagerCombatProxy>
	{
	public:
		static char const * const MetatableName;

		static void PopulateMetatable(lua_State * L);

		inline LuaTurnManagerCombatProxy(uint8_t combatId)
			: combatId_(combatId)
		{}

		inline esv::TurnManager::Combat * Get()
		{
			return GetTurnManager()->Combats.Find(combatId_);
		}

	private:
		uint8_t combatId_;

		static int GetCurrentTurnOrder(lua_State * L);
		static int GetNextTurnOrder(lua_State * L);
		static int UpdateCurrentTurnOrder(lua_State * L);
		static int UpdateNextTurnOrder(lua_State * L);
		static int GetAllTeams(lua_State * L);
		/*static int Clear(lua_State * L);
		static int Multiply(lua_State * L);
		static int Merge(lua_State * L);
		static int ConvertDamageType(lua_State * L);
		static int AggregateSameTypeDamages(lua_State * L);
		static int ToTable(lua_State * L);*/
	};

	class LuaTurnManagerTeamProxy : public LuaUserdata<LuaTurnManagerTeamProxy>, public LuaIndexable
	{
	public:
		static char const * const MetatableName;

		//static void PopulateMetatable(lua_State * L);

		inline LuaTurnManagerTeamProxy(eoc::CombatTeamId teamId)
			: teamId_(teamId)
		{}

		inline eoc::CombatTeamId TeamId() const
		{
			return teamId_;
		}

		inline esv::TurnManager::CombatTeam * Get()
		{
			auto combat = GetTurnManager()->Combats.Find(teamId_.CombatId);
			if (combat) {
				auto team = combat->Teams.Find((uint32_t)teamId_);
				if (team) {
					return *team;
				} else {
					return nullptr;
				}
			} else {
				return nullptr;
			}
		}

		int LuaIndex(lua_State * L);

	private:
		eoc::CombatTeamId teamId_;

		/*static int GetCurrentTurnOrder(lua_State * L);
		static int Clear(lua_State * L);
		static int Multiply(lua_State * L);
		static int Merge(lua_State * L);
		static int ConvertDamageType(lua_State * L);
		static int AggregateSameTypeDamages(lua_State * L);
		static int ToTable(lua_State * L);*/
	};


	class LuaExtensionLibrary
	{
	public:
		LuaExtensionLibrary();

		void Register(lua_State * L);
		void RegisterLib(lua_State * L);
		std::string GenerateOsiHelpers();

	private:
		static char const * const NameResolverMetatableName;

		void RegisterNameResolverMetatable(lua_State * L);
		void CreateNameResolver(lua_State * L);

		static int LuaIndexResolverTable(lua_State * L);

		static int Require(lua_State * L);
		static int NewCall(lua_State * L);
		static int NewQuery(lua_State * L);
		static int NewEvent(lua_State * L);
	};

	inline void OsiReleaseArgument(OsiArgumentDesc & arg)
	{
		arg.NextParam = nullptr;
	}

	inline void OsiReleaseArgument(TypedValue & arg) {}
	inline void OsiReleaseArgument(ListNode<TypedValue *> & arg) {}
	inline void OsiReleaseArgument(ListNode<TupleLL::Item> & arg) {}

	template <class T>
	class OsiArgumentPool
	{
	public:
		OsiArgumentPool()
		{
			argumentPool_.resize(1024);
			usedArguments_ = 0;
		}

		T * AllocateArguments(uint32_t num, uint32_t & tail)
		{
			if (usedArguments_ + num > argumentPool_.size()) {
				throw std::runtime_error("Ran out of argument descriptors");
			}

			tail = usedArguments_;
			auto ptr = argumentPool_.data() + usedArguments_;
			for (uint32_t i = 0; i < num; i++) {
				new (ptr + i) T();
			}

			usedArguments_ += num;
			return ptr;
		}

		void ReleaseArguments(uint32_t tail, uint32_t num)
		{
			if (tail + num != usedArguments_) {
				throw std::runtime_error("Attempted to release arguments out of order");
			}

			for (uint32_t i = 0; i < num; i++) {
				OsiReleaseArgument(argumentPool_[tail + i]);
			}

			usedArguments_ -= num;
		}

	private:
		std::vector<T> argumentPool_;
		uint32_t usedArguments_;
	};

	template <class T>
	class OsiArgumentListPin
	{
	public:
		inline OsiArgumentListPin(OsiArgumentPool<T> & pool, uint32_t numArgs)
			: pool_(pool), numArgs_(numArgs)
		{
			args_ = pool.AllocateArguments(numArgs_, tail_);
		}

		inline ~OsiArgumentListPin()
		{
			pool_.ReleaseArguments(tail_, numArgs_);
		}

		inline T * Args() const
		{
			return args_;
		}

	private:
		OsiArgumentPool<T> & pool_;
		uint32_t numArgs_;
		uint32_t tail_;
		T * args_;
	};

	class LuaException : public std::exception
	{
	public:
		LuaException()
			: std::exception("Lua error thrown")
		{}
	};

	class LuaState
	{
	public:
		enum Restriction
		{
			// Disable Osiris calls
			RestrictOsiris = 1 << 0,
			// Disable handle/guid to object conversion functions (Lua only)
			RestrictHandleConversion = 1 << 1,

			// Permit calls only available during module load state
			ScopeModuleLoad = 1 << 16,
			// Permit calls only available during session load state
			ScopeSessionLoad = 1 << 17,

			RestrictAll = 0x0000ffff
		};

		uint32_t RestrictionFlags{ 0 };

		LuaState();
		~LuaState();

		LuaState(LuaState const &) = delete;
		LuaState(LuaState &&) = delete;
		LuaState & operator = (LuaState const &) = delete;
		LuaState & operator = (LuaState &&) = delete;

		inline lua_State * State()
		{
			return state_;
		}

		inline uint32_t GenerationId() const
		{
			return generationId_;
		}

		inline IdentityAdapterMap & GetIdentityAdapterMap()
		{
			return identityAdapters_;
		}

		inline OsiArgumentPool<OsiArgumentDesc> & GetArgumentDescPool()
		{
			return argDescPool_;
		}

		inline OsiArgumentPool<TypedValue> & GetTypedValuePool()
		{
			return tvPool_;
		}

		inline OsiArgumentPool<ListNode<TypedValue *>> & GetTypedValueNodePool()
		{
			return tvNodePool_;
		}

		inline OsiArgumentPool<ListNode<TupleLL::Item>> & GetTupleNodePool()
		{
			return tupleNodePool_;
		}

		inline bool StartupDone() const
		{
			return startupDone_;
		}

		void FinishStartup();
		void StoryLoaded();
		void StoryFunctionMappingsUpdated();
		void OnGameSessionLoading();
		void OnModuleLoading();

		bool LoadScript(std::string const & script, std::string const & name = "");

		template <class TArg>
		void Call(char const * func, std::vector<TArg> const & args)
		{
			std::lock_guard lock(mutex_);

			auto L = State();
			auto stackSize = lua_gettop(L);

			try {
				lua_getglobal(L, func); // stack: func
				for (auto & arg : args) {
					OsiToLua(L, arg); // stack: func, arg0 ... argn
				}
			
				auto status = CallWithTraceback((int)args.size(), 0);
				if (status != LUA_OK) {
					OsiError("Failed to call function '" << func << "': " << lua_tostring(L, -1));
					// stack: errmsg
					lua_pop(L, 1); // stack: -
				}
			} catch (LuaException &) {
				auto stackRemaining = lua_gettop(L) - stackSize;
				if (stackRemaining > 0) {
					OsiError("Call '" << func << "' failed: " << lua_tostring(L, -1));
					lua_pop(L, stackRemaining);
				} else {
					OsiError("Internal error during call '" << func << "'");
				}
			}
		}

		bool Query(std::string const & name, LuaRegistryEntry * func,
			std::vector<CustomFunctionParam> const & signature, OsiArgumentDesc & params);
		int CallWithTraceback(int narg, int nres);

		std::optional<int32_t> StatusGetEnterChance(esv::Status * status, bool useCharacterStats);
		std::optional<int32_t> GetHitChance(CDivinityStats_Character * attacker, CDivinityStats_Character * target);
		bool SkillGetDescriptionParam(SkillPrototype * prototype, CDivinityStats_Character * character,
			ObjectSet<STDString> const & paramTexts, std::wstring & replacement);
		bool GetSkillDamage(SkillPrototype * self, DamagePairList * damageList,
			CDivinityStats_Character *attackerStats, bool isFromItem, bool stealthed, float * attackerPosition,
			float * targetPosition, DeathType * pDeathType, int level, bool noRandomization);
		bool StatusGetDescriptionParam(StatusPrototype * prototype, CDivinityStats_Character * statusSource, 
			CDivinityStats_Character * character, ObjectSet<STDString> const & paramTexts, std::wstring & replacement);

	private:
		lua_State * state_;
		LuaExtensionLibrary proxy_;
		OsiArgumentPool<OsiArgumentDesc> argDescPool_;
		OsiArgumentPool<TypedValue> tvPool_;
		OsiArgumentPool<ListNode<TypedValue *>> tvNodePool_;
		OsiArgumentPool<ListNode<TupleLL::Item>> tupleNodePool_;
		IdentityAdapterMap identityAdapters_;
		std::recursive_mutex mutex_;
		// ID of current story instance.
		// Used to invalidate function/node pointers in Lua userdata objects
		uint32_t generationId_{ 0 };
		bool startupDone_{ false };

		void OpenLibs();
		bool QueryInternal(std::string const & name, LuaRegistryEntry * func,
			std::vector<CustomFunctionParam> const & signature, OsiArgumentDesc & params);

		static int TracebackHandler(lua_State * L);
		static std::string GetBuiltinLibrary();
	};

	class LuaRestriction
	{
	public:
		inline LuaRestriction(LuaState & state, uint32_t flags)
			: state_(state), oldFlags_(state_.RestrictionFlags)
		{
			state_.RestrictionFlags |= flags;
		}

		inline ~LuaRestriction()
		{
			state_.RestrictionFlags = oldFlags_;
		}

	private:
		LuaState & state_;
		uint32_t oldFlags_;
	};
}
