#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/Osiris.h>
#include <GameDefinitions/TurnManager.h>
#include <Osiris/Shared/CustomFunctions.h>
#include <Extender/Shared/ExtensionHelpers.h>
#include <Osiris/Shared/OsirisHelpers.h>

namespace dse::esv
{
	struct PendingHit;
	class ExtensionState;

	struct OsirisHookSignature
	{
		enum HookType
		{
			BeforeTrigger,
			AfterTrigger,
			BeforeDeleteTrigger,
			AfterDeleteTrigger
		};

		STDString name;
		uint32_t arity;
		HookType type;

		inline bool operator == (OsirisHookSignature const& o) const
		{
			return name == o.name
				&& arity == o.arity
				&& type == o.type;
		}
	};

}

namespace std
{
	template<> struct hash<dse::esv::OsirisHookSignature>
	{
		typedef dse::esv::OsirisHookSignature argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const& sig) const noexcept
		{
			return std::hash<dse::STDString>{}(sig.name) ^ std::hash<uint32_t>{}(sig.arity | ((uint32_t)sig.type << 6));
		}
	};
}

namespace dse::lua
{
	void RegisterSharedLibraries(lua_State* L);
}

namespace dse::esv::lua
{
	using namespace dse::lua;

	LifetimeHolder GetServerLifetime();
	LifetimePool& GetServerLifetimePool();

	void LuaToOsi(lua_State * L, int i, TypedValue & tv, ValueType osiType, bool allowNil = false);
	TypedValue * LuaToOsi(lua_State * L, int i, ValueType osiType, bool allowNil = false);
	void LuaToOsi(lua_State * L, int i, OsiArgumentValue & arg, ValueType osiType, bool allowNil = false);
	void OsiToLua(lua_State * L, OsiArgumentValue const & arg);
	void OsiToLua(lua_State * L, TypedValue const & tv);
	Function const* LookupOsiFunction(STDString const& name, uint32_t arity);

	class OsiFunction
	{
	public:
		inline bool IsBound() const
		{
			return function_ != nullptr;
		}

		inline bool IsDB() const
		{
			return IsBound() && function_->Type == FunctionType::Database;
		}

		bool Bind(Function const * func, class ServerState & state);
		void Unbind();

		int LuaCall(lua_State * L);
		int LuaGet(lua_State * L);
		int LuaDelete(lua_State * L);

	private:
		Function const * function_{ nullptr };
		AdapterRef adapter_;
		ServerState * state_;

		void OsiCall(lua_State * L);
		void OsiInsert(lua_State * L, bool deleteTuple);
		int OsiQuery(lua_State * L);
		int OsiUserQuery(lua_State * L);

		bool MatchTuple(lua_State * L, int firstIndex, TupleVec const & tuple);
		void ConstructTuple(lua_State * L, TupleVec const & tuple);
	};

	class OsiFunctionNameProxy : public Userdata<OsiFunctionNameProxy>, public Callable
	{
	public:
		static char const * const MetatableName;
		// Maximum number of OUT params that a query can return.
		// (This setting determines how many function arities we'll check during name lookup)
		static constexpr uint32_t MaxQueryOutParams = 6;

		static void PopulateMetatable(lua_State * L);

		OsiFunctionNameProxy(STDString const & name, ServerState & state);

		void UnbindAll();
		int LuaCall(lua_State * L);

	private:
		STDString name_;
		std::vector<OsiFunction> functions_;
		ServerState & state_;
		uint32_t generationId_;

		static int LuaGet(lua_State * L);
		static int LuaDelete(lua_State * L);
		bool BeforeCall(lua_State * L);
		OsiFunction * TryGetFunction(uint32_t arity);
		OsiFunction * CreateFunctionMapping(uint32_t arity, Function const * func);
	};


	class CustomLuaCall : public CustomCallBase
	{
	public:
		inline CustomLuaCall(STDString const & name, std::vector<CustomFunctionParam> params,
			RegistryEntry handler)
			: CustomCallBase(name, std::move(params)), handler_(std::move(handler))
		{}

		virtual bool Call(OsiArgumentDesc const & params) override;

	private:
		RegistryEntry handler_;
	};


	class CustomLuaQuery : public CustomQueryBase
	{
	public:
		inline CustomLuaQuery(STDString const & name, std::vector<CustomFunctionParam> params,
			RegistryEntry handler)
			: CustomQueryBase(name, std::move(params)), handler_(std::move(handler))
		{}

		virtual bool Query(OsiArgumentDesc & params) override;

	private:
		RegistryEntry handler_;
	};


	class StatusHandleProxy : public Userdata<StatusHandleProxy>, public Indexable, public NewIndexable, public Pushable<PushPolicy::None>
	{
	public:
		static char const * const MetatableName;

		inline StatusHandleProxy(ComponentHandle owner, ComponentHandle status, bool isUnapplied = false)
			: owner_(owner), statusHandle_(status), isUnapplied_(isUnapplied)
		{}

		inline StatusHandleProxy(ComponentHandle owner, NetId status)
			: owner_(owner), statusNetId_(status), isUnapplied_(false)
		{}

		int Index(lua_State * L);
		int NewIndex(lua_State * L);
		esv::Status* Get(lua_State* L);

		inline ComponentHandle OwnerHandle() const
		{
			return owner_;
		}

		inline ComponentHandle StatusHandle() const
		{
			return statusHandle_;
		}

	private:
		ComponentHandle owner_;
		ComponentHandle statusHandle_;
		NetId statusNetId_;
		bool isUnapplied_;
	};


	class TurnManagerCombatProxy : public Userdata<TurnManagerCombatProxy>, public Indexable, public Pushable<PushPolicy::None>
	{
	public:
		static char const * const MetatableName;

		static void PopulateMetatable(lua_State * L);

		inline TurnManagerCombatProxy(uint8_t combatId)
			: combatId_(combatId)
		{}

		inline esv::TurnManager::Combat * Get()
		{
			return GetEntityWorld()->GetTurnManager()->Combats.Find(combatId_);
		}

		int Index(lua_State * L);

	private:
		uint8_t combatId_;

		static int GetCurrentTurnOrder(lua_State * L);
		static int GetNextTurnOrder(lua_State * L);
		static int UpdateCurrentTurnOrder(lua_State * L);
		static int UpdateNextTurnOrder(lua_State * L);
		static int GetAllTeams(lua_State * L);
	};

	class TurnManagerTeamProxy : public Userdata<TurnManagerTeamProxy>, public Indexable, public Pushable<PushPolicy::None>
	{
	public:
		static char const * const MetatableName;

		inline TurnManagerTeamProxy(eoc::CombatTeamId teamId)
			: teamId_(teamId)
		{}

		inline eoc::CombatTeamId TeamId() const
		{
			return teamId_;
		}

		inline esv::TurnManager::CombatTeam * Get()
		{
			auto combat = GetEntityWorld()->GetTurnManager()->Combats.Find(teamId_.CombatId);
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

		int Index(lua_State * L);

	private:
		eoc::CombatTeamId teamId_;
	};


	class ItemConstructor : public Userdata<ItemConstructor>, public Indexable, public Pushable<PushPolicy::None>
	{
	public:
		static char const* const MetatableName;

		inline ItemConstructor()
		{}

		inline ObjectSet<eoc::ItemDefinition>& Get()
		{
			return definition_;
		}

		int Index(lua_State* L);

	private:
		ObjectSet<eoc::ItemDefinition> definition_;
	};


	class ExtensionLibraryServer : public ExtensionLibrary
	{
	public:
		void Register(lua_State * L) override;
		void RegisterLib(lua_State * L) override;
		STDString GenerateOsiHelpers();

	private:
		static char const * const NameResolverMetatableName;

		void RegisterNameResolverMetatable(lua_State * L);
		void CreateNameResolver(lua_State * L);

		static int LuaIndexResolverTable(lua_State * L);

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


	class ServerState;
	class OsirisCallbackManager : Noncopyable<OsirisCallbackManager>
	{
	public:
		OsirisCallbackManager(ExtensionState& state);
		~OsirisCallbackManager();

		void Subscribe(STDString const& name, uint32_t arity, OsirisHookSignature::HookType type, RegistryEntry handler);
		void StoryLoaded();
		void StorySetMerging(bool isMerging);

	private:
		static constexpr uint64_t AfterTriggerNodeRef = 0x8000000000000000ull;
		static constexpr uint64_t DeleteTriggerNodeRef = 0x4000000000000000ull;

		ExtensionState& state_;
		std::vector<RegistryEntry> subscribers_;
		std::unordered_multimap<OsirisHookSignature, std::size_t> nameSubscriberRefs_;
		std::unordered_multimap<uint64_t, std::size_t> nodeSubscriberRefs_;
		bool storyLoaded_{ false };
		bool osirisHooked_{ false };
		// Are we currently merging Osiris files (story)?
		// If so, then we won't trigger events for calls/inserts that might occur during the merge,
		// as those are not real inserts but byproducts of the merge process.
		bool merging_{ false };

		void RegisterNodeHandler(OsirisHookSignature const& sig, std::size_t handlerId);
		void HookOsiris();

		void InsertPreHook(Node* node, TuplePtrLL* tuple, bool deleted);
		void InsertPostHook(Node* node, TuplePtrLL* tuple, bool deleted);
		void CallQueryPreHook(Node* node, OsiArgumentDesc* args);
		void CallQueryPostHook(Node* node, OsiArgumentDesc* args, bool succeeded);
		void RunHandlers(uint64_t nodeRef, TuplePtrLL* tuple) const;
		void RunHandler(ServerState& lua, RegistryEntry const& func, TuplePtrLL* tuple) const;
		void RunHandlers(uint64_t nodeRef, OsiArgumentDesc* tuple) const;
		void RunHandler(ServerState& lua, RegistryEntry const& func, OsiArgumentDesc* tuple) const;
	};

	struct GameStateChangeEventParams
	{
		GameState FromState;
		GameState ToState;
	};

	class ServerState : public State
	{
	public:
		ServerState(ExtensionState& state);
		~ServerState();

		ServerState(ServerState const &) = delete;
		ServerState(ServerState &&) = delete;
		ServerState & operator = (ServerState const &) = delete;
		ServerState & operator = (ServerState &&) = delete;

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

		inline OsirisCallbackManager& GetOsirisCallbacks()
		{
			return osirisCallbacks_;
		}

		void OnGameSessionLoading() override;

		void StoryLoaded();
		void StoryFunctionMappingsUpdated();
		void StorySetMerging(bool isMerging);

		template <class TArg>
		void Call(char const* mod, char const* func, std::vector<TArg> const & args)
		{
			auto L = GetState();
			LifetimePin _(GetStack());
			lua_checkstack(L, (int)args.size() + 1);
			auto stackSize = lua_gettop(L);

			try {
				if (mod != nullptr) {
					PushModFunction(L, mod, func); // stack: func
				} else {
					lua_getglobal(L, func); // stack: func
				}

				for (auto & arg : args) {
					OsiToLua(L, arg); // stack: func, arg0 ... argn
				}

				auto status = CallWithTraceback(L, (int)args.size(), 0);
				if (status != LUA_OK) {
					LuaError("Failed to call function '" << func << "': " << lua_tostring(L, -1));
					// stack: errmsg
					lua_pop(L, 1); // stack: -
				}
			} catch (Exception &) {
				auto stackRemaining = lua_gettop(L) - stackSize;
				if (stackRemaining > 0) {
					if (mod != nullptr) {
						LuaError("Call to mod function '" << mod << "'.'" << func << "' failed: " << lua_tostring(L, -1));
					} else {
						LuaError("Call to mod function '" << func << "' failed: " << lua_tostring(L, -1));
					}
					lua_pop(L, stackRemaining);
				} else {
					if (mod != nullptr) {
						LuaError("Internal error during call to mod function '" << mod << "'.'" << func << "'");
					} else {
						LuaError("Internal error during call to mod function '" << func << "'");
					}
				}
			}
		}

		bool Query(char const* mod, char const* name, RegistryEntry * func,
			std::vector<CustomFunctionParam> const & signature, OsiArgumentDesc & params);

		std::optional<int32_t> StatusGetEnterChance(esv::Status * status, bool isEnterCheck);
		bool OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId);
		void OnStatusHitEnter(esv::StatusHit* hit, PendingHit* context);
		bool ComputeCharacterHit(CDivinityStats_Character * self,
			CDivinityStats_Character *attackerStats, CDivinityStats_Item *item, DamagePairList *damageList, HitType hitType, bool noHitRoll,
			bool forceReduceDurability, HitDamageInfo *damageInfo, CRPGStats_Object_Property_List *skillProperties,
			HighGroundBonus highGroundFlag, CriticalRoll criticalRoll);
		bool OnCharacterApplyDamage(esv::Character* target, HitDamageInfo& hit, ComponentHandle attackerHandle,
			CauseType causeType, glm::vec3& impactDirection, PendingHit* context);
		void OnGameStateChanged(GameState fromState, GameState toState);
		esv::Item* OnGenerateTreasureItem(esv::Item* item);
		bool OnBeforeCraftingExecuteCombination(CraftingStationType craftingStation, ObjectSet<ComponentHandle> const& ingredients,
			esv::Character* character, uint8_t quantity, FixedString const& combinationId);
		void OnAfterCraftingExecuteCombination(CraftingStationType craftingStation, ObjectSet<ComponentHandle> const& ingredients,
			esv::Character* character, uint8_t quantity, FixedString const& combinationId, bool succeeded);
		void OnBeforeShootProjectile(ShootProjectileHelper* helper);
		void OnShootProjectile(Projectile* projectile);
		void OnProjectileHit(Projectile* projectile, ComponentHandle const& hitObject, glm::vec3 const& position);
		void OnExecutePropertyDataOnGroundHit(glm::vec3& position, ComponentHandle casterHandle, DamagePairList* damageList);

		void ExecutePropertyDataOnTarget(CRPGStats_Object_Property_Extender* prop, ComponentHandle attackerHandle,
			ComponentHandle target, glm::vec3 const& impactOrigin, bool isFromItem, SkillPrototype* skillProto,
			HitDamageInfo const* damageInfo);
		void ExecutePropertyDataOnPosition(CRPGStats_Object_Property_Extender* prop, ComponentHandle attackerHandle, 
			glm::vec3 const& position, float areaRadius, bool isFromItem, SkillPrototype* skillPrototype,
			HitDamageInfo const* damageInfo);

		std::optional<STDString> GetModPersistentVars(STDString const& modTable);
		void RestoreModPersistentVars(STDString const& modTable, STDString const& vars);

	private:
		ExtensionLibraryServer library_;
		OsiArgumentPool<OsiArgumentDesc> argDescPool_;
		OsiArgumentPool<TypedValue> tvPool_;
		OsiArgumentPool<ListNode<TypedValue *>> tvNodePool_;
		OsiArgumentPool<ListNode<TupleLL::Item>> tupleNodePool_;
		IdentityAdapterMap identityAdapters_;
		// ID of current story instance.
		// Used to invalidate function/node pointers in Lua userdata objects
		uint32_t generationId_{ 0 };
		OsirisCallbackManager osirisCallbacks_;

		bool QueryInternal(char const* mod, char const* name, RegistryEntry * func,
			std::vector<CustomFunctionParam> const & signature, OsiArgumentDesc & params);
	};
}
