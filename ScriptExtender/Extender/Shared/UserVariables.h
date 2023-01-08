#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Base/ObjectVisitor.h>

BEGIN_SE()

class MsgUserVars;
class UserVar;

enum class UserVariableType
{
	Null = 0,
	Int64 = 1,
	Double = 2,
	String = 3,
	Composite = 4
};

struct UserVariable
{
	UserVariable() : Type(UserVariableType::Null) {}
	UserVariable(int64_t v) : Type(UserVariableType::Int64), Int(v) {}
	UserVariable(double v) : Type(UserVariableType::Double), Dbl(v) {}
	UserVariable(FixedString const& v) : Type(UserVariableType::String), Str(v) {}
	UserVariable(STDString const& v) : Type(UserVariableType::Composite), CompositeStr(v) {}

	void SavegameVisit(ObjectVisitor* visitor);
	void ToNetMessage(UserVar& var) const;
	void FromNetMessage(UserVar const& var);

	UserVariableType Type{ UserVariableType::Null };
	bool Dirty{ false };
	int64_t Int{ 0ll };
	double Dbl{ 0.0 };
	FixedString Str;
	STDString CompositeStr;
};

enum class UserVariableFlags
{
	IsOnServer = 1 << 0,
	IsOnClient = 1 << 1,
	SyncServerToClient = 1 << 2,
	SyncClientToServer = 1 << 3,
	SyncOnWrite = 1 << 4,
	DontCache = 1 << 5,
	WriteableOnServer = 1 << 6,
	WriteableOnClient = 1 << 7,
	SyncOnTick = 1 << 8,
	Persistent = 1 << 9
};

template<> struct IsBitmask<UserVariableFlags>
{
	static const bool value = true;
};

struct UserVariablePrototype
{
	UserVariableFlags Flags{ 0 };

	inline bool Has(UserVariableFlags flag) const
	{
		return (Flags & flag) == flag;
	}

	inline bool NeedsSyncFor(bool server) const
	{
		return Has(server ? UserVariableFlags::SyncServerToClient : UserVariableFlags::SyncClientToServer);
	}

	inline bool IsAvailableFor(bool server) const
	{
		return Has(server ? UserVariableFlags::IsOnServer : UserVariableFlags::IsOnClient);
	}

	inline bool IsWriteableFor(bool server) const
	{
		return Has(server ? UserVariableFlags::WriteableOnServer : UserVariableFlags::WriteableOnClient);
	}
};

class UserVariableManager
{
public:
	inline UserVariableManager(bool isServer)
		: isServer_(isServer)
	{}

	UserVariable* Get(ComponentHandle component, FixedString const& key);
	Map<FixedString, UserVariable>* GetAll(ComponentHandle component);
	void Set(ComponentHandle component, FixedString const& key, UserVariablePrototype const& proto, UserVariable&& value);
	UserVariablePrototype const* GetPrototype(FixedString const& key) const;
	void RegisterPrototype(FixedString const& key, UserVariablePrototype const& proto);

	void BindCache(lua::CachedUserVariableManager* cache);
	void Update();
	void Flush(bool force);
	void SavegameVisit(ObjectVisitor* visitor);
	void OnNetworkSync(MsgUserVars const& msg);

private:
	struct SyncRequest
	{
		ComponentHandle Component;
		FixedString Variable;
	};

	Map<ComponentHandle, Map<FixedString, UserVariable>> vars_;
	Map<FixedString, UserVariablePrototype> prototypes_;
	ObjectSet<SyncRequest> deferredSyncs_;
	ObjectSet<SyncRequest> nextTickSyncs_;
	ScriptExtenderMessage* syncMsg_{ nullptr };
	bool isServer_;
	lua::CachedUserVariableManager* cache_{ nullptr };

	void NetworkSync(UserVar const& var);
	std::optional<ComponentHandle> NetIdToComponentHandle(UserVar const& var);
	void Sync(ComponentHandle component, FixedString const& key, UserVariable const& value);
	void ComponentHandleToNetId(ComponentHandle component, UserVar* var);
	void FlushSyncQueue(ObjectSet<SyncRequest>& queue);
	bool MakeSyncMessage();
	void SendSyncs();
};

END_SE()

BEGIN_NS(lua)

enum class CachedUserVariableType
{
	Null = 0,
	Int64 = 1,
	Double = 2,
	String = 3,
	Reference = 4
};

struct CachedUserVariable
{
	CachedUserVariable() : Type(CachedUserVariableType::Null) {}
	CachedUserVariable(int64_t v) : Type(CachedUserVariableType::Int64), Int(v) {}
	CachedUserVariable(double v) : Type(CachedUserVariableType::Double), Dbl(v) {}
	CachedUserVariable(FixedString const& v) : Type(CachedUserVariableType::String), Str(v) {}
	CachedUserVariable(lua_State* L, Ref const& v);
	CachedUserVariable(lua_State* L, UserVariable const& v);

	CachedUserVariableType Type{ CachedUserVariableType::Null };
	bool Dirty{ false };
	int64_t Int{ 0ll };
	double Dbl{ 0.0 };
	FixedString Str;
	RegistryEntry Reference;

	void Push(lua_State* L) const;
	bool LikelyChanged(CachedUserVariable const& o) const;
	UserVariable ToUserVariable(lua_State* L) const;
	STDString StringifyReference(lua_State* L) const;
	void ParseReference(lua_State* L, StringView json);
};

class CachedUserVariableManager
{
public:
	CachedUserVariableManager(UserVariableManager& global, bool isServer);
	~CachedUserVariableManager();

	inline UserVariableManager& GetGlobal() const
	{
		return global_;
	}

	inline bool IsServer() const
	{
		return isServer_;
	}

	void Push(lua_State* L, ComponentHandle component, FixedString const& key);
	void Push(lua_State* L, ComponentHandle component, FixedString const& key, UserVariablePrototype const& proto);
	void Set(lua_State* L, ComponentHandle component, FixedString const& key, CachedUserVariable && var);
	void Set(lua_State* L, ComponentHandle component, FixedString const& key, UserVariablePrototype const& proto, CachedUserVariable && var);
	void Invalidate(ComponentHandle component, FixedString const& key);
	void Flush();

private:
	struct FlushRequest
	{
		ComponentHandle Component;
		FixedString Variable;
		UserVariablePrototype const* Proto;
	};

	UserVariableManager& global_;
	bool isServer_;
	Map<ComponentHandle, Map<FixedString, CachedUserVariable>> vars_;
	ObjectSet<FlushRequest> flushQueue_;

	CachedUserVariable* GetFromCache(ComponentHandle component, FixedString const& key);
	CachedUserVariable* PutCache(lua_State* L, ComponentHandle component, FixedString const& key, UserVariablePrototype const& proto, UserVariable const& value);
	CachedUserVariable* PutCache(ComponentHandle component, FixedString const& key, UserVariablePrototype const& proto, CachedUserVariable && value, bool isWrite);
};

END_NS()
