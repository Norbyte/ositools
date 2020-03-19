#pragma once

#include <GameDefinitions/Character.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/Status.h>

#include <cstdint>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <mutex>
#include <unordered_set>
#include <optional>

namespace osidbg
{
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
		LuaRegistryEntry();
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

	class LuaItemOrCharacterPushPin
	{
	public:
		LuaItemOrCharacterPushPin(lua_State * L, CRPGStats_Object * obj);
		~LuaItemOrCharacterPushPin();

	private:
		LuaObjectProxy<CDivinityStats_Character> * character_{ nullptr };
		LuaObjectProxy<CDivinityStats_Item> * item_{ nullptr };
		LuaStatsProxy * object_{ nullptr };
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

	class LuaExtensionLibrary
	{
	public:
		virtual void Register(lua_State * L);
		virtual void RegisterLib(lua_State * L) = 0;

	protected:
		static int Require(lua_State * L);
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
			// Permit calls only available during module resume state
			ScopeModuleResume = 1 << 18,

			// Flags to use for code that only runs on the server
			RestrictAllServer = 0x0000ffff & ~RestrictHandleConversion,
			// Flags to use for code that runs on both server & client
			RestrictAllClient = 0x0000ffff,
		};

		uint32_t RestrictionFlags{ 0 };
		std::unordered_set<int32_t> OverriddenLevelMaps;

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

		inline bool StartupDone() const
		{
			return startupDone_;
		}

		inline std::recursive_mutex & GetMutex()
		{
			return mutex_;
		}

		void FinishStartup();
		virtual void OnGameSessionLoading();
		void OnModuleLoading();
		void OnModuleResume();

		bool LoadScript(std::string const & script, std::string const & name = "");
		int CallWithTraceback(int narg, int nres);

		std::optional<int32_t> GetHitChance(CDivinityStats_Character * attacker, CDivinityStats_Character * target);
		bool GetSkillDamage(SkillPrototype * self, DamagePairList * damageList,
			CRPGStats_ObjectInstance *attackerStats, bool isFromItem, bool stealthed, float * attackerPosition,
			float * targetPosition, DeathType * pDeathType, int level, bool noRandomization);
		void OnNetMessageReceived(std::string const & channel, std::string const & payload);

	protected:
		lua_State * state_;
		std::recursive_mutex mutex_;
		bool startupDone_{ false };

		void OpenLibs();

		static int TracebackHandler(lua_State * L);
		static std::string GetBuiltinLibrary(int resourceId);
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
