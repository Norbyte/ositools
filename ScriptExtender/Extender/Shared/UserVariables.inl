#include <Extender/Shared/UserVariables.h>
#include <Lua/Libs/Json.h>

// #define USER_VAR_DBG(msg, ...)
#define USER_VAR_DBG(msg, ...) DEBUG(msg, __VA_ARGS__)

BEGIN_SE()

void ExtenderProtocolBase::SyncUserVars(MsgUserVars const& msg)
{
	USER_VAR_DBG("Received sync message from peer");
	auto state = gExtender->GetCurrentExtensionState();
	for (auto const& var : msg.vars()) {
		if (var.net_id_type() == NetIdType::NETID_MODULE) {
			state->GetModVariables().NetworkSync(var);
		}
		else {
			state->GetUserVariables().NetworkSync(var);
		}
	}
}

void UserVariable::SavegameVisit(ObjectVisitor* visitor)
{
	if (visitor->IsReading()) {
		uint8_t type;
		visitor->VisitUInt8(GFS.strType, type, (uint8_t)UserVariableType::Null);
		Type = (UserVariableType)type;

		switch (Type) {
		case UserVariableType::Int64:
			visitor->VisitInt64(GFS.strValue, Int, 0ll);
			break;
		case UserVariableType::Double:
			visitor->VisitDouble(GFS.strValue, Dbl, 0.0);
			break;
		case UserVariableType::String:
			visitor->VisitFixedString(GFS.strValue, Str, GFS.strEmpty);
			break;
		case UserVariableType::Composite:
			visitor->VisitSTDString(GFS.strValue, CompositeStr, STDString{});
			break;
		}
	} else {
		auto type = (uint8_t)Type;
		visitor->VisitUInt8(GFS.strType, type, (uint8_t)UserVariableType::Null);
		switch (Type) {
		case UserVariableType::Int64:
			visitor->VisitInt64(GFS.strValue, Int, 0ll);
			break;
		case UserVariableType::Double:
			visitor->VisitDouble(GFS.strValue, Dbl, 0.0);
			break;
		case UserVariableType::String:
			visitor->VisitFixedString(GFS.strValue, Str, GFS.strEmpty);
			break;
		case UserVariableType::Composite:
			visitor->VisitSTDString(GFS.strValue, CompositeStr, STDString{});
			break;
		}
	}
}

void UserVariable::ToNetMessage(UserVar& var) const
{
	switch (Type) {
	case UserVariableType::Null:
		break;

	case UserVariableType::Int64:
		var.set_intval(Int);
		break;

	case UserVariableType::Double:
		var.set_dblval(Dbl);
		break;

	case UserVariableType::String:
		var.set_strval(Str.GetStringOrDefault());
		break;

	case UserVariableType::Composite:
		var.set_luaval(CompositeStr.c_str(), CompositeStr.size());
		break;
	}
}

void UserVariable::FromNetMessage(UserVar const& var)
{
	switch (var.val_case()) {
	case UserVar::kIntval:
		Type = UserVariableType::Int64;
		Int = var.intval();
		break;

	case UserVar::kDblval:
		Type = UserVariableType::Double;
		Dbl = var.dblval();
		break;

	case UserVar::kStrval:
		Type = UserVariableType::String;
		Str = FixedString(var.strval());
		break;

	case UserVar::kLuaval:
		Type = UserVariableType::Composite;
		CompositeStr = var.luaval();
		break;

	case UserVar::VAL_NOT_SET:
	default:
		Type = UserVariableType::Null;
		break;
	}
}


size_t UserVariable::Budget() const
{
	size_t budget = 12;
	switch (Type) {
	case UserVariableType::Null: budget += 0; break;
	case UserVariableType::Int64: budget += 8; break;
	case UserVariableType::Double: budget += 8; break;
	case UserVariableType::String: budget += Str.GetMetadata()->Length; break;
	case UserVariableType::Composite: budget += CompositeStr.size(); break;
	}

	return budget;
}



void UserVariableSyncWriter::Flush(bool force)
{
	if (isServer_) {
		auto state = GetStaticSymbols().GetServerState();
		if (!state 
			|| *state == dse::esv::GameState::LoadSession
			|| *state == dse::esv::GameState::LoadLevel
			|| *state == dse::esv::GameState::Sync) {
			return;
		}
	} else {
		auto state = GetStaticSymbols().GetClientState();
		if (!state
			|| *state == dse::ecl::GameState::LoadSession
			|| *state == dse::ecl::GameState::LoadLevel
			|| *state == dse::ecl::GameState::PrepareRunning) {
			return;
		}
	}


	if (!deferredSyncs_.empty()) {
		USER_VAR_DBG("Flushing deferred syncs");
		FlushSyncQueue(deferredSyncs_);
	}
	
	if (force && !nextTickSyncs_.empty()) {
		USER_VAR_DBG("Flushing next tick syncs");
		FlushSyncQueue(nextTickSyncs_);
	}

	SendSyncs();
}

void UserVariableSyncWriter::Sync(FixedString const& guid, FixedString const& key, UserVariablePrototype const& proto, UserVariable const* value)
{
	if (proto.NeedsSyncFor(isServer_)) {
		if (value && proto.Has(UserVariableFlags::SyncOnWrite)) {
			USER_VAR_DBG("Immediate sync var %s/%s", guid.GetStringOrDefault(), key.GetStringOrDefault());
			if (MakeSyncMessage()) {
				AppendToSyncMessage(guid, key, *value);
				SendSyncs();
			}
		} else if (proto.Has(UserVariableFlags::SyncOnTick)) {
			USER_VAR_DBG("Request next tick sync for var %s/%s", guid.GetStringOrDefault(), key.GetStringOrDefault());
			nextTickSyncs_.push_back(SyncRequest{
				.Entity = guid,
				.Variable = key
			});
		} else {
			USER_VAR_DBG("Request deferred sync for var %s/%s", guid.GetStringOrDefault(), key.GetStringOrDefault());
			deferredSyncs_.push_back(SyncRequest{
				.Entity = guid,
				.Variable = key
			});
		}
	}
}

void UserVariableSyncWriter::DeferredSync(FixedString const& guid, FixedString const& key)
{
	deferredSyncs_.push_back(SyncRequest{
		.Entity = guid,
		.Variable = key
	});
}

void UserVariableSyncWriter::AppendToSyncMessage(FixedString const& gameObject, FixedString const& key, UserVariable const& value)
{
	auto netId = vars_->GuidToNetId(gameObject);
	if (!netId) return;

	if (syncMsgBudget_ > SyncMessageBudget) {
		SendSyncs();
		MakeSyncMessage();
	}

	auto var = syncMsg_->GetMessage().mutable_user_vars()->add_vars();
	var->set_net_id_type(netId->first);
	var->set_net_id(netId->second.Id);
	var->set_key(key.GetStringOrDefault());
	value.ToNetMessage(*var);
	syncMsgBudget_ += value.Budget() + key.GetMetadata()->Length;
}

void UserVariableSyncWriter::FlushSyncQueue(ObjectSet<SyncRequest>& queue)
{
	if (!MakeSyncMessage()) return;

	for (auto const& req : queue) {
		auto value = vars_->Get(req.Entity, req.Variable);
		if (value && value->Dirty) {
			USER_VAR_DBG("Flush sync var %s/%s", req.Entity.GetStringOrDefault(), req.Variable.GetStringOrDefault());
			AppendToSyncMessage(req.Entity, req.Variable, *value);
			value->Dirty = false;
		}
	}

	queue.clear();
}

bool UserVariableSyncWriter::MakeSyncMessage()
{
	if (syncMsg_ == nullptr) {
		if (isServer_) {
			syncMsg_ = gExtender->GetServer().GetNetworkManager().GetFreeMessage();
		} else {
			syncMsg_ = gExtender->GetClient().GetNetworkManager().GetFreeMessage();
		}

		if (syncMsg_) {
			syncMsg_->GetMessage().mutable_user_vars();
		}
	}

	return syncMsg_ != nullptr;
}

void UserVariableSyncWriter::SendSyncs()
{
	if (syncMsg_ && syncMsg_->GetMessage().user_vars().vars_size() > 0) {
		if (isServer_) {
			USER_VAR_DBG("Syncing user vars to client(s)");
			gExtender->GetServer().GetNetworkManager().BroadcastToConnectedPeers(syncMsg_, ReservedUserId, false);
		} else {
			USER_VAR_DBG("Syncing user vars to server");
			gExtender->GetClient().GetNetworkManager().Send(syncMsg_);
		}

		syncMsg_ = nullptr;
		syncMsgBudget_ = 0;
	}
}


UserVariable* UserVariableManager::Get(FixedString const& gameObject, FixedString const& key)
{
	auto it = vars_.find(gameObject);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			return &valueIt.Value();
		}
	}

	return nullptr;
}

Map<FixedString, UserVariable>* UserVariableManager::GetAll(FixedString const& gameObject)
{
	auto it = vars_.find(gameObject);
	if (it != vars_.end()) {
		return &it.Value().Vars;
	} else {
		return nullptr;
	}
}

Map<FixedString, UserVariableManager::ComponentVariables>& UserVariableManager::GetAll()
{
	return vars_;
}

void UserVariableManager::MarkDirty(FixedString const& gameObject, FixedString const& key, UserVariable& value)
{
	auto proto = GetPrototype(key);
	if (proto == nullptr) return;

	value.Dirty = true;
	sync_.Sync(gameObject, key, *proto, &value);
}

UserVariableManager::ComponentVariables* UserVariableManager::Set(FixedString const& gameObject, FixedString const& key, UserVariablePrototype const& proto, UserVariable&& value)
{
	if (value.Dirty) {
		sync_.Sync(gameObject, key, proto, &value);
	}

	auto it = vars_.find(gameObject);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			valueIt.Value() = std::move(value);
		} else {
			it.Value().Vars.insert(std::make_pair(key, std::move(value)));
		}
		return &it.Value();
	} else {
		auto gameObjectVars = vars_.insert(std::make_pair(gameObject, ComponentVariables{}));
		gameObjectVars->Vars.insert(std::make_pair(key, std::move(value)));
		return gameObjectVars;
	}
}

void UserVariableManager::BindCache(lua::CachedUserVariableManager* cache)
{
	cache_ = cache;
}

void UserVariableManager::Update()
{
	Flush(true);
}

void UserVariableManager::Flush(bool force)
{
	sync_.Flush(force);
}

UserVariablePrototype const* UserVariableManager::GetPrototype(FixedString const& key) const
{
	auto it = prototypes_.find(key);
	if (it != prototypes_.end()) {
		return &it.Value();
	} else {
		return nullptr;
	}
}

void UserVariableManager::RegisterPrototype(FixedString const& key, UserVariablePrototype const& proto)
{
	prototypes_.insert(std::make_pair(key, proto));
}

void UserVariableManager::SavegameVisit(ObjectVisitor* visitor)
{
	STDString nullStr;
	if (visitor->EnterNode(GFS.strUserVariables, GFS.strEmpty)) {
		if (visitor->IsReading()) {
			vars_.clear();
			uint32_t numVars;
			visitor->VisitCount(GFS.strGameObjectVariables, &numVars);

			for (uint32_t i = 0; i < numVars; i++) {
				if (visitor->EnterNode(GFS.strGameObjectVariables, GFS.strGameObject)) {
					FixedString gameObject;
					visitor->VisitFixedString(GFS.strGameObject, gameObject, GFS.strEmpty);
					auto gameObjectVars = vars_.insert(std::make_pair(gameObject, ComponentVariables{}));

					uint32_t numgameObjectVars;
					visitor->VisitCount(GFS.strVariable, &numgameObjectVars);

					for (uint32_t j = 0; j < numgameObjectVars; j++) {
						if (visitor->EnterNode(GFS.strVariable, GFS.strName)) {
							FixedString name;
							visitor->VisitFixedString(GFS.strName, name, GFS.strEmpty);
							USER_VAR_DBG("Savegame restore var %s/%s", gameObject.GetStringOrDefault(), name.GetStringOrDefault());
							
							auto var = gameObjectVars->Vars.insert(std::make_pair(name, UserVariable{}));
							var->SavegameVisit(visitor);
							visitor->ExitNode(GFS.strVariable);

							auto proto = GetPrototype(name);
							if (proto && proto->NeedsSyncFor(isServer_)) {
								USER_VAR_DBG("Request deferred sync for var %s/%s", gameObject.GetStringOrDefault(), name.GetStringOrDefault());
								var->Dirty = true;
								sync_.DeferredSync(gameObject, name);
							}
						}
					}

					visitor->ExitNode(GFS.strGameObjectVariables);
				}
			}
		} else {
			if (cache_) {
				cache_->Flush();
			}

			for (auto& gameObject : vars_) {
				if (visitor->EnterNode(GFS.strGameObjectVariables, GFS.strGameObject)) {
					visitor->VisitFixedString(GFS.strGameObject, gameObject.Key, GFS.strEmpty);

					for (auto& kv : gameObject.Value.Vars) {
						auto proto = GetPrototype(kv.Key);
						if (proto && proto->Has(UserVariableFlags::Persistent)) {
							if (visitor->EnterNode(GFS.strVariable, GFS.strName)) {
								visitor->VisitFixedString(GFS.strName, kv.Key, GFS.strEmpty);
								USER_VAR_DBG("Savegame persist var %s/%s", gameObject.Key.GetStringOrDefault(), kv.Key.GetStringOrDefault());
								kv.Value.SavegameVisit(visitor);
								visitor->ExitNode(GFS.strVariable);
							}
						}
					}

					visitor->ExitNode(GFS.strGameObjectVariables);
				}
			}
		}

		visitor->ExitNode(GFS.strUserVariables);
	}
}

void UserVariableManager::NetworkSync(UserVar const& var)
{
	USER_VAR_DBG("Received sync for %d/%s", var.net_id(), var.key().c_str());
	auto gameObject = NetIdToGameObject(var);
	if (!gameObject) return;

	FixedString key(var.key());
	auto proto = GetPrototype(key);
	if (!proto) {
		ERR("Tried to sync variable '%s' that has no prototype!", var.key().c_str());
		return;
	}
	
	if (!proto->NeedsSyncFor(!isServer_)) {
		ERR("Tried to sync variable '%s' in illegal direction!", var.key().c_str());
		return;
	}

	UserVariable value;
	value.FromNetMessage(var);
	value.Dirty = true;

	Set(*gameObject->GetGuid(), key, *proto, std::move(value));

	if (cache_ && !proto->Has(UserVariableFlags::DontCache)) {
		ComponentHandle handle;
		gameObject->GetObjectHandle(handle);
		cache_->Invalidate(handle, key);
	}
}

IGameObject* UserVariableManager::NetIdToGameObject(UserVar const& var) const
{
	if (isServer_) {
		switch (var.net_id_type()) {
		case NETID_CHARACTER: return esv::GetEntityWorld()->GetComponent<esv::Character>(NetId(var.net_id()));
		case NETID_ITEM: return esv::GetEntityWorld()->GetComponent<esv::Item>(NetId(var.net_id()));

		default:
			ERR("Received user variable sync for unknown NetID class %d!", var.net_id_type());
			break;
		}
	} else {
		switch (var.net_id_type()) {
		case NETID_CHARACTER: return ecl::GetEntityWorld()->GetComponent<ecl::Character>(NetId(var.net_id()));
		case NETID_ITEM: return ecl::GetEntityWorld()->GetComponent<ecl::Item>(NetId(var.net_id()));

		default:
			ERR("Received user variable sync for unknown NetID class %d!", var.net_id_type());
			break;
		}
	}

	return nullptr;
}

FixedString UserVariableManager::NetIdToGuid(UserVar const& var) const
{
	auto obj = NetIdToGameObject(var);
	if (obj) {
		return *obj->GetGuid();
	} else {
		return FixedString{};
	}
}

std::optional<std::pair<NetIdType, NetId>> UserVariableManager::GuidToNetId(FixedString const& gameObject) const
{
	if (isServer_) {
		auto ch = esv::GetEntityWorld()->GetComponent<esv::Character>(gameObject, false);
		if (ch) {
			return std::pair{ NETID_CHARACTER, ch->NetID };
		}

		auto item = esv::GetEntityWorld()->GetComponent<esv::Item>(gameObject, false);
		if (item) {
			return std::pair{ NETID_ITEM, item->NetID };
		}
	}
	else {
		auto ch = ecl::GetEntityWorld()->GetComponent<ecl::Character>(gameObject, false);
		if (ch) {
			return std::pair{ NETID_CHARACTER, ch->NetID };
		}

		auto item = ecl::GetEntityWorld()->GetComponent<ecl::Item>(gameObject, false);
		if (item) {
			return std::pair{ NETID_ITEM, item->NetID };
		}
	}

	ERR("Tried to sync variables of unknown game object %s!", gameObject.GetStringOrDefault());
	return {};
}



UserVariable* ModVariableMap::Get(FixedString const& key)
{
	return vars_.try_get_ptr(key);
}

ModVariableMap::VariableMap& ModVariableMap::GetAll()
{
	return vars_;
}

UserVariable* ModVariableMap::Set(FixedString const& key, UserVariablePrototype const& proto, UserVariable&& value)
{
	UserVariable* var;
	auto valueIt = vars_.find(key);
	if (valueIt != vars_.end()) {
		valueIt.Value() = std::move(value);
		var = &valueIt.Value();
	} else {
		var = vars_.insert(std::make_pair(key, std::move(value)));
	}

	return var;
}

UserVariablePrototype const* ModVariableMap::GetPrototype(FixedString const& key) const
{
	return prototypes_.try_get_ptr(key);
}

void ModVariableMap::RegisterPrototype(FixedString const& key, UserVariablePrototype const& proto)
{
	prototypes_.insert(std::make_pair(key, proto));
}

void ModVariableMap::SavegameVisit(ObjectVisitor* visitor)
{
	STDString nullStr;
	if (visitor->IsReading()) {
		uint32_t numVars;
		visitor->VisitCount(GFS.strVariable, &numVars);

		for (uint32_t j = 0; j < numVars; j++) {
			if (visitor->EnterNode(GFS.strVariable, GFS.strName)) {
				FixedString name;
				visitor->VisitFixedString(GFS.strName, name, GFS.strEmpty);
				USER_VAR_DBG("Savegame restore var %s/%s", gameObject_.GetStringOrDefault(), name.GetStringOrDefault());
							
				auto var = vars_.insert(std::make_pair(name, UserVariable{}));
				var->SavegameVisit(visitor);
				visitor->ExitNode(GFS.strVariable);

				auto proto = GetPrototype(name);
				if (proto && proto->NeedsSyncFor(isServer_)) {
					USER_VAR_DBG("Request deferred sync for var %s/%s", gameObject_.GetStringOrDefault(), name.GetStringOrDefault());
					var->Dirty = true;
				}
			}
		}
	} else {
		for (auto& kv : vars_) {
			auto proto = GetPrototype(kv.Key);
			if (proto && proto->Has(UserVariableFlags::Persistent)) {
				if (visitor->EnterNode(GFS.strVariable, GFS.strName)) {
					visitor->VisitFixedString(GFS.strName, kv.Key, GFS.strEmpty);
					USER_VAR_DBG("Savegame persist var %s/%s", gameObject_.GetStringOrDefault(), kv.Key.GetStringOrDefault());
					kv.Value.SavegameVisit(visitor);
					visitor->ExitNode(GFS.strVariable);
				}
			}
		}
	}
}

UserVariablePrototype const* ModVariableMap::NetworkSync(UserVar const& var)
{
	FixedString key(var.key());
	auto proto = GetPrototype(key);
	if (!proto) {
		ERR("Tried to sync variable '%s' that has no prototype!", var.key().c_str());
		return nullptr;
	}
	
	if (!proto->NeedsSyncFor(!isServer_)) {
		ERR("Tried to sync variable '%s' in illegal direction!", var.key().c_str());
		return nullptr;
	}

	UserVariable value;
	value.FromNetMessage(var);
	value.Dirty = true;

	Set(key, *proto, std::move(value));
	return proto;
}







UserVariable* ModVariableManager::Get(FixedString const& modUuid, FixedString const& key)
{
	auto it = vars_.find(modUuid);
	if (it != vars_.end()) {
		return it.Value().Get(key);
	}

	return nullptr;
}

ModVariableMap::VariableMap* ModVariableManager::GetAll(FixedString const& modUuid)
{
	auto it = vars_.find(modUuid);
	if (it != vars_.end()) {
		return &it.Value().GetAll();
	} else {
		return nullptr;
	}
}

Map<FixedString, ModVariableMap>& ModVariableManager::GetAll()
{
	return vars_;
}

ModVariableMap* ModVariableManager::GetMod(FixedString const& modUuid)
{
	return vars_.try_get_ptr(modUuid);
}

ModVariableMap* ModVariableManager::GetOrCreateMod(FixedString const& modUuid)
{
	auto it = vars_.find(modUuid);
	if (it != vars_.end()) {
		return &it.Value();
	} else {
		return vars_.insert(std::make_pair(modUuid, ModVariableMap(modUuid, isServer_)));
	}
}

UserVariablePrototype const* ModVariableManager::GetPrototype(FixedString const& modUuid, FixedString const& key) const
{
	auto it = vars_.find(modUuid);
	if (it != vars_.end()) {
		return it.Value().GetPrototype(key);
	} else {
		return nullptr;
	}
}

void ModVariableManager::MarkDirty(FixedString const& modUuid, FixedString const& key, UserVariable& value)
{
	auto proto = GetPrototype(modUuid, key);
	if (proto == nullptr) return;

	value.Dirty = true;
	sync_.Sync(modUuid, key, *proto, &value);
}

ModVariableMap* ModVariableManager::Set(FixedString const& modUuid, FixedString const& key, UserVariablePrototype const& proto, UserVariable&& value)
{
	if (value.Dirty) {
		sync_.Sync(modUuid, key, proto, &value);
	}

	auto mod = GetOrCreateMod(modUuid);
	mod->Set(key, proto, std::move(value));
	return mod;
}

std::optional<std::pair<NetIdType, NetId>> ModVariableManager::GuidToNetId(FixedString const& guid) const
{
	auto it = modIndices_.find(guid);
	if (it != modIndices_.end()) {
		return std::pair<NetIdType, NetId>{ NetIdType::NETID_MODULE, NetId(it.Value()) };
	}

	return {};
}

void ModVariableManager::Flush(bool force)
{
	sync_.Flush(force);
}

void ModVariableManager::BindCache(lua::CachedModVariableManager* cache)
{
	cache_ = cache;
}

void ModVariableManager::Update()
{
	Flush(true);
}

void ModVariableManager::OnModuleLoading()
{
	modIndices_.clear();
	auto& mod = (isServer_ ? GetModManagerServer()->BaseModule : GetModManagerClient()->BaseModule);
	uint32_t nextIndex{ 0 };
	for (auto const& mod : mod.LoadOrderedModules) {
		modIndices_.insert(std::make_pair(mod.Info.ModuleUUID, nextIndex++));
	}
}

void ModVariableManager::RegisterPrototype(FixedString const& modUuid, FixedString const& key, UserVariablePrototype const& proto)
{
	GetOrCreateMod(modUuid)->RegisterPrototype(key, proto);
}

void ModVariableManager::SavegameVisit(ObjectVisitor* visitor)
{
	STDString nullStr;
	if (visitor->EnterNode(GFS.strModVariables, GFS.strEmpty)) {
		if (visitor->IsReading()) {
			vars_.clear();
			uint32_t numVars;
			visitor->VisitCount(GFS.strGameObjectVariables, &numVars);

			for (uint32_t i = 0; i < numVars; i++) {
				if (visitor->EnterNode(GFS.strModVariables, GFS.strModule)) {
					FixedString modUuid;
					visitor->VisitFixedString(GFS.strModule, modUuid, GFS.strEmpty);
					auto mod = GetOrCreateMod(modUuid);
					mod->SavegameVisit(visitor);
					for (auto const& kv : mod->GetAll()) {
						if (kv.Value.Dirty) {
							sync_.DeferredSync(modUuid, kv.Key);
						}
					}
					visitor->ExitNode(GFS.strModVariables);
				}
			}
		} else {
			if (cache_) {
				cache_->Flush();
			}

			for (auto& mod : vars_) {
				if (visitor->EnterNode(GFS.strModVariables, GFS.strModule)) {
					visitor->VisitFixedString(GFS.strModule, mod.Key, GFS.strEmpty);
					mod.Value.SavegameVisit(visitor);
					visitor->ExitNode(GFS.strModVariables);
				}
			}
		}

		visitor->ExitNode(GFS.strModVariables);
	}
}

void ModVariableManager::NetworkSync(UserVar const& var)
{
	USER_VAR_DBG("Received sync for %d/%s", var.net_id(), var.key().c_str());
	auto modUuid = NetIdToGuid(var);
	if (!modUuid) return;

	auto map = GetMod(modUuid);
	if (!map) {
		ERR("Tried to sync variable for nonexistent mod '%s'!", modUuid.GetStringOrDefault());
		return;
	}

	auto proto = map->NetworkSync(var);

	if (proto && cache_ && !proto->Has(UserVariableFlags::DontCache)) {
		cache_->Invalidate(var.net_id(), FixedString(var.key()));
	}
}

FixedString ModVariableManager::NetIdToGuid(UserVar const& var) const
{
	if (isServer_) {
		switch (var.net_id_type()) {
		// FIXME - overflow check!
		case NETID_MODULE: return GetModManagerServer()->BaseModule.LoadOrderedModules[var.net_id()].Info.ModuleUUID;

		default:
			ERR("Received mod variable sync for unknown NetID class %d!", var.net_id_type());
			break;
		}
	} else {
		switch (var.net_id_type()) {
		case NETID_MODULE: return GetModManagerClient()->BaseModule.LoadOrderedModules[var.net_id()].Info.ModuleUUID;

		default:
			ERR("Received mod variable sync for unknown NetID class %d!", var.net_id_type());
			break;
		}
	}

	return {};
}

END_SE()

BEGIN_NS(lua)

CachedUserVariable::CachedUserVariable(lua_State* L, UserVariable const& v)
{
	switch (v.Type) {
	case UserVariableType::Int64:
		Type = CachedUserVariableType::Int64;
		Int = v.Int;
		break;

	case UserVariableType::Double:
		Type = CachedUserVariableType::Double;
		Dbl = v.Dbl;
		break;

	case UserVariableType::String:
		Type = CachedUserVariableType::String;
		Str = v.Str;
		break;

	case UserVariableType::Composite:
		ParseReference(L, v.CompositeStr);
		break;

	case UserVariableType::Null:
	default:
		Type = CachedUserVariableType::Null;
		break;
	}
}

CachedUserVariable::CachedUserVariable(lua_State* L, Ref const& v)
{
	if (v.Type() != RefType::Local) {
		ERR("Tried to make cached user variable from non-local ref!");
		Type = CachedUserVariableType::Null;
		return;
	}

	switch (lua_type(L, v.Index())) {
	case LUA_TNIL:
		Type = CachedUserVariableType::Null;
		break;
		
	case LUA_TNUMBER:
		if (lua_isinteger(L, v.Index())) {
			Type = CachedUserVariableType::Int64;
			Int = get<int64_t>(L, v.Index());
		} else {
			Type = CachedUserVariableType::Double;
			Dbl = get<double>(L, v.Index());
		}
		break;

	case LUA_TSTRING:
		Type = CachedUserVariableType::String;
		Str = get<FixedString>(L, v.Index());
		break;

	case LUA_TTABLE:
		Type = CachedUserVariableType::Reference;
		Reference = RegistryEntry(L, v);
		break;

	default:
		Type = CachedUserVariableType::Null;
		ERR("Tried to make cached user variable from unsupported type '%s'!", lua_typename(L, lua_type(L, v.Index())));
		break;
	}
}

void CachedUserVariable::ParseReference(lua_State* L, StringView json)
{
	if (json::Parse(L, json)) {
		Reference = RegistryEntry(L, -1);
		lua_pop(L, 1);
		Type = CachedUserVariableType::Reference;
	} else {
		ERR("Failed to parse user variable blob");
		Type = CachedUserVariableType::Null;
	}
}

void CachedUserVariable::Push(lua_State* L) const
{
	switch (Type) {
	case CachedUserVariableType::Int64: push(L, Int); break;
	case CachedUserVariableType::Double: push(L, Dbl); break;
	case CachedUserVariableType::String: push(L, Str); break;
	case CachedUserVariableType::Reference: push(L, Reference); break;
	case CachedUserVariableType::Null:
	default: 
		push(L, nullptr);
		break;
	}
}

bool CachedUserVariable::LikelyChanged(CachedUserVariable const& o) const
{
	if (Type != o.Type) return true;

	switch (Type) {
	case CachedUserVariableType::Int64:
		return Int != o.Int;
	case CachedUserVariableType::Double:
		return Dbl != o.Dbl;
	case CachedUserVariableType::String:
		return Str != o.Str;
	case CachedUserVariableType::Reference:
		// There is no fast way to compare values apart from serializing the values on both sides;
		// assume that a write to a composite value is an explicit way to indicate that the value changed.
		return true;

	case CachedUserVariableType::Null:
	default:
		return false;
	}
}

UserVariable CachedUserVariable::ToUserVariable(lua_State* L) const
{
	UserVariable var;
	switch (Type) {
	case CachedUserVariableType::Null:
		var.Type = UserVariableType::Null;
		break;
		
	case CachedUserVariableType::Int64:
		var.Type = UserVariableType::Int64;
		var.Int = Int;
		break;
		
	case CachedUserVariableType::Double:
		var.Type = UserVariableType::Double;
		var.Dbl = Dbl;
		break;
		
	case CachedUserVariableType::String:
		var.Type = UserVariableType::String;
		var.Str = Str;
		break;
		
	case CachedUserVariableType::Reference:
		var.CompositeStr = StringifyReference(L);
		if (!var.CompositeStr.empty()) {
			var.Type = UserVariableType::Composite;
		} else {
			var.Type = UserVariableType::Null;
		}
		
		break;
	}

	return var;
}

STDString CachedUserVariable::StringifyReference(lua_State* L) const
{
	Reference.Push();
	json::StringifyContext ctx;
	ctx.Beautify = false;

	STDString str;
	try {
		str = json::Stringify(L, ctx, lua_absindex(L, -1));
	} catch (std::runtime_error& e) {
		ERR("Error stringifying user variable: %s", e.what());
		str.clear();
	}

	lua_pop(L, 1);
	return str;
}


CachedUserVariableManager::CachedUserVariableManager(UserVariableManager& global, bool isServer)
	: global_(global), isServer_(isServer),
	vars_(GetNearestLowerPrime(10000))
{
	global_.BindCache(this);
}

CachedUserVariableManager::~CachedUserVariableManager()
{
	global_.BindCache(nullptr);
}

FixedString CachedUserVariableManager::ComponentHandleToGuid(ComponentHandle handle)
{
	if (isServer_) {
		switch ((ObjectHandleType)handle.GetType()) {
		case ObjectHandleType::ServerCharacter:
		{
			auto ch = esv::GetEntityWorld()->GetComponent<esv::Character>(handle, false);
			if (ch) {
				return ch->MyGuid;
			}
			break;
		}

		case ObjectHandleType::ServerItem:
		{
			auto item = esv::GetEntityWorld()->GetComponent<esv::Item>(handle, false);
			if (item) {
				return item->MyGuid;
			}
			break;
		}
		}
	} else {
		switch ((ObjectHandleType)handle.GetType()) {
		case ObjectHandleType::ClientCharacter:
		{
			auto ch = ecl::GetEntityWorld()->GetComponent<ecl::Character>(handle, false);
			if (ch) {
				return ch->MyGuid;
			}
			break;
		}

		case ObjectHandleType::ClientItem:
		{
			auto item = ecl::GetEntityWorld()->GetComponent<ecl::Item>(handle, false);
			if (item) {
				return item->MyGuid;
			}
			break;
		}
		}
	}

	return FixedString{};
}

CachedUserVariable* CachedUserVariableManager::GetFromCache(ComponentHandle component, FixedString const& key, FixedString& gameObjectGuid)
{
	auto it = vars_.find(component);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			gameObjectGuid = it.Value().CachedGuid;
			return &valueIt.Value();
		}
	}

	return nullptr;
}

CachedUserVariable* CachedUserVariableManager::GetFromCache(ComponentHandle component, FixedString const& key)
{
	auto it = vars_.find(component);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			return &valueIt.Value();
		}
	}

	return nullptr;
}

CachedUserVariable* CachedUserVariableManager::PutCache(ComponentHandle component, FixedString const& key, FixedString const& gameObjectGuid, UserVariablePrototype const& proto, CachedUserVariable&& value, bool isWrite)
{
	CachedUserVariable* var;
	ComponentVariables* vars;
	bool wasDirty{ false };

	auto it = vars_.find(component);
	if (it != vars_.end()) {
		vars = &it.Value();
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			wasDirty = valueIt.Value().Dirty;
			bool dirty = valueIt.Value().Dirty || (isWrite && valueIt.Value().LikelyChanged(value));
			valueIt.Value() = std::move(value);
			var = &valueIt.Value();
			var->Dirty = dirty;
		} else {
			var = it.Value().Vars.insert(std::make_pair(key, std::move(value)));
			var->Dirty = isWrite;
		}
	} else {
		vars = vars_.insert(std::make_pair(component, ComponentVariables{}));
		if (gameObjectGuid) {
			vars->CachedGuid = gameObjectGuid;
		} else {
			vars->CachedGuid = ComponentHandleToGuid(component);
		}

		var = vars->Vars.insert(std::make_pair(key, std::move(value)));
		var->Dirty = isWrite;
	}

	if (!wasDirty && var->Dirty) {
		USER_VAR_DBG("Mark cached var for flush %s/%s", vars->CachedGuid.GetString(), key.GetStringOrDefault());
		flushQueue_.push_back(FlushRequest{
			.Component = component,
			.Variable = key,
			.Proto = &proto
		});
	}

	return var;
}

CachedUserVariable* CachedUserVariableManager::PutCache(lua_State* L, ComponentHandle component, FixedString const& key, FixedString const& gameObjectGuid, UserVariablePrototype const& proto, UserVariable const& value)
{
	CachedUserVariable var(L, value);
	return PutCache(component, key, gameObjectGuid, proto, std::move(var), false);
}

void CachedUserVariableManager::Push(lua_State* L, ComponentHandle component, FixedString const& key)
{
	auto proto = global_.GetPrototype(key);
	if (!proto) {
		push(L, nullptr);
		return;
	}

	return Push(L, component, key, *proto);
}

void CachedUserVariableManager::Push(lua_State* L, ComponentHandle component, FixedString const& key, UserVariablePrototype const& proto)
{
	FixedString gameObjectGuid;
	if (!proto.Has(UserVariableFlags::DontCache)) {
		auto cached = GetFromCache(component, key, gameObjectGuid);
		if (cached) {
			cached->Push(L);
			return;
		}
	}

	if (!gameObjectGuid) {
		gameObjectGuid = ComponentHandleToGuid(component);
		if (!gameObjectGuid) {
			push(L, nullptr);
			return;
		}
	}

	auto value = global_.Get(gameObjectGuid, key);
	if (!value) {
		push(L, nullptr);
		return;
	}

	if (!proto.Has(UserVariableFlags::DontCache)) {
		USER_VAR_DBG("Cache global var %016llx/%s", component.Handle, key.GetStringOrDefault());
		auto cached = PutCache(L, component, key, gameObjectGuid, proto, *value);
		cached->Push(L);
	} else {
		CachedUserVariable cached(L, *value);
		cached.Push(L);
	}
}

void CachedUserVariableManager::Set(lua_State* L, ComponentHandle component, FixedString const& key, CachedUserVariable && var)
{
	auto proto = global_.GetPrototype(key);
	if (!proto) {
		OsiError("Tried to set user variable '" << key << "' that is not registered!");
		return;
	}

	Set(L, component, key, *proto, std::move(var));
}

void CachedUserVariableManager::Set(lua_State* L, ComponentHandle component, FixedString const& key, UserVariablePrototype const& proto, CachedUserVariable && var)
{
	if (!proto.Has(UserVariableFlags::DontCache)) {
		USER_VAR_DBG("Set cached var %016llx/%s", component.Handle, key.GetStringOrDefault());
		auto cachedVar = PutCache(component, key, FixedString{}, proto, std::move(var), true);

		if (cachedVar->Dirty && proto.NeedsSyncFor(isServer_) && proto.Has(UserVariableFlags::SyncOnWrite)) {
			USER_VAR_DBG("Set global var %016llx/%s", component.Handle, key.GetStringOrDefault());
			auto userVar = cachedVar->ToUserVariable(L);
			cachedVar->Dirty = false;
			userVar.Dirty = true;
			global_.Set(ComponentHandleToGuid(component), key, proto, std::move(userVar));
		}
	} else {
		USER_VAR_DBG("Set global var %016llx/%s", component.Handle, key.GetStringOrDefault());
		auto userVar = var.ToUserVariable(L);
		userVar.Dirty = true;
		global_.Set(ComponentHandleToGuid(component), key, proto, std::move(userVar));
	}
}

void CachedUserVariableManager::Invalidate()
{
	vars_.clear();
	flushQueue_.clear();
}

void CachedUserVariableManager::Invalidate(ComponentHandle component, FixedString const& key)
{
	auto it = vars_.find(component);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			USER_VAR_DBG("Invalidate cached var %016llx/%s", component.Handle, key.GetStringOrDefault());
			it.Value().Vars.erase(valueIt);
		}
	}
}

void CachedUserVariableManager::Flush()
{
	LuaVirtualPin lua;

	if (lua) {
		for (auto const& req : flushQueue_) {
			FixedString gameObjectGuid;
			auto var = GetFromCache(req.Component, req.Variable, gameObjectGuid);
			if (var && var->Dirty) {
				USER_VAR_DBG("Flush cached var %016llx/%s", req.Component.Handle, req.Variable.GetStringOrDefault());
				auto userVar = var->ToUserVariable(lua->GetState());
				userVar.Dirty = true;
				global_.Set(gameObjectGuid, req.Variable, *req.Proto, std::move(userVar));
				var->Dirty = false;
			}
		}

		flushQueue_.clear();
	}
}



CachedModVariableManager::CachedModVariableManager(ModVariableManager& global, bool isServer)
	: global_(global), isServer_(isServer),
	vars_(GetNearestLowerPrime(100))
{
	global_.BindCache(this);
}

CachedModVariableManager::~CachedModVariableManager()
{
	global_.BindCache(nullptr);
}

FixedString CachedModVariableManager::ModIndexToGuid(uint32_t modIndex)
{
	if (isServer_) {
		return GetModManagerServer()->BaseModule.LoadOrderedModules[modIndex].Info.ModuleUUID;
	} else {
		return GetModManagerClient()->BaseModule.LoadOrderedModules[modIndex].Info.ModuleUUID;
	}
}

CachedUserVariable* CachedModVariableManager::GetFromCache(uint32_t modIndex, FixedString const& key, FixedString& modUuid)
{
	auto it = vars_.find(modIndex);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			modUuid = it.Value().CachedGuid;
			return &valueIt.Value();
		}
	}

	return nullptr;
}

CachedUserVariable* CachedModVariableManager::GetFromCache(uint32_t modIndex, FixedString const& key)
{
	auto it = vars_.find(modIndex);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			return &valueIt.Value();
		}
	}

	return nullptr;
}

CachedUserVariable* CachedModVariableManager::PutCache(uint32_t modIndex, FixedString const& key, FixedString const& modUuid, UserVariablePrototype const& proto, CachedUserVariable&& value, bool isWrite)
{
	CachedUserVariable* var;
	ModVariables* vars;
	bool wasDirty{ false };

	auto it = vars_.find(modIndex);
	if (it != vars_.end()) {
		vars = &it.Value();
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			wasDirty = valueIt.Value().Dirty;
			bool dirty = valueIt.Value().Dirty || (isWrite && valueIt.Value().LikelyChanged(value));
			valueIt.Value() = std::move(value);
			var = &valueIt.Value();
			var->Dirty = dirty;
		} else {
			var = it.Value().Vars.insert(std::make_pair(key, std::move(value)));
			var->Dirty = isWrite;
		}
	} else {
		vars = vars_.insert(std::make_pair(modIndex, ModVariables{}));
		if (modUuid) {
			vars->CachedGuid = modUuid;
		} else {
			vars->CachedGuid = ModIndexToGuid(modIndex);
		}

		var = vars->Vars.insert(std::make_pair(key, std::move(value)));
		var->Dirty = isWrite;
	}

	if (!wasDirty && var->Dirty) {
		USER_VAR_DBG("Mark cached mod var for flush %s/%s", vars->CachedGuid.GetString(), key.GetStringOrDefault());
		flushQueue_.push_back(FlushRequest{
			.ModIndex = modIndex,
			.Variable = key,
			.Proto = &proto
		});
	}

	return var;
}

CachedUserVariable* CachedModVariableManager::PutCache(lua_State* L, uint32_t modIndex, FixedString const& key, FixedString const& modUuid, UserVariablePrototype const& proto, UserVariable const& value)
{
	CachedUserVariable var(L, value);
	return PutCache(modIndex, key, modUuid, proto, std::move(var), false);
}

void CachedModVariableManager::Push(lua_State* L, uint32_t modIndex, FixedString const& key)
{
	auto proto = global_.GetPrototype(ModIndexToGuid(modIndex), key);
	if (!proto) {
		push(L, nullptr);
		return;
	}

	return Push(L, modIndex, key, *proto);
}

void CachedModVariableManager::Push(lua_State* L, uint32_t modIndex, FixedString const& key, UserVariablePrototype const& proto)
{
	FixedString modUuid;
	if (!proto.Has(UserVariableFlags::DontCache)) {
		auto cached = GetFromCache(modIndex, key, modUuid);
		if (cached) {
			cached->Push(L);
			return;
		}
	}

	if (!modUuid) {
		modUuid = ModIndexToGuid(modIndex);
		if (!modUuid) {
			push(L, nullptr);
			return;
		}
	}

	auto value = global_.Get(modUuid, key);
	if (!value) {
		push(L, nullptr);
		return;
	}

	if (!proto.Has(UserVariableFlags::DontCache)) {
		USER_VAR_DBG("Cache global mod var %d/%s", modIndex, key.GetStringOrDefault());
		auto cached = PutCache(L, modIndex, key, modUuid, proto, *value);
		cached->Push(L);
	} else {
		CachedUserVariable cached(L, *value);
		cached.Push(L);
	}
}

void CachedModVariableManager::Set(lua_State* L, uint32_t modIndex, FixedString const& key, CachedUserVariable && var)
{
	auto proto = global_.GetPrototype(ModIndexToGuid(modIndex), key);
	if (!proto) {
		OsiError("Tried to set user variable '" << key << "' that is not registered!");
		return;
	}

	Set(L, modIndex, key, *proto, std::move(var));
}

void CachedModVariableManager::Set(lua_State* L, uint32_t modIndex, FixedString const& key, UserVariablePrototype const& proto, CachedUserVariable && var)
{
	if (!proto.Has(UserVariableFlags::DontCache)) {
		USER_VAR_DBG("Set cached mod var %d/%s", modIndex, key.GetStringOrDefault());
		auto cachedVar = PutCache(modIndex, key, FixedString{}, proto, std::move(var), true);

		if (cachedVar->Dirty && proto.NeedsSyncFor(isServer_) && proto.Has(UserVariableFlags::SyncOnWrite)) {
			USER_VAR_DBG("Set global mod var %d/%s", modIndex, key.GetStringOrDefault());
			auto userVar = cachedVar->ToUserVariable(L);
			cachedVar->Dirty = false;
			userVar.Dirty = true;
			global_.Set(ModIndexToGuid(modIndex), key, proto, std::move(userVar));
		}
	} else {
		USER_VAR_DBG("Set global var %d/%s", modIndex, key.GetStringOrDefault());
		auto userVar = var.ToUserVariable(L);
		userVar.Dirty = true;
		global_.Set(ModIndexToGuid(modIndex), key, proto, std::move(userVar));
	}
}

void CachedModVariableManager::Invalidate()
{
	vars_.clear();
	flushQueue_.clear();
}

void CachedModVariableManager::Invalidate(uint32_t modIndex, FixedString const& key)
{
	auto it = vars_.find(modIndex);
	if (it != vars_.end()) {
		auto valueIt = it.Value().Vars.find(key);
		if (valueIt != it.Value().Vars.end()) {
			USER_VAR_DBG("Invalidate cached mod var %d/%s", modIndex, key.GetStringOrDefault());
			it.Value().Vars.erase(valueIt);
		}
	}
}

void CachedModVariableManager::Flush()
{
	LuaVirtualPin lua;

	if (lua) {
		for (auto const& req : flushQueue_) {
			FixedString modUuid;
			auto var = GetFromCache(req.ModIndex, req.Variable, modUuid);
			if (var && var->Dirty) {
				USER_VAR_DBG("Flush cached mod var %d/%s", req.ModIndex, req.Variable.GetStringOrDefault());
				auto userVar = var->ToUserVariable(lua->GetState());
				userVar.Dirty = true;
				global_.Set(modUuid, req.Variable, *req.Proto, std::move(userVar));
				var->Dirty = false;
			}
		}

		flushQueue_.clear();
	}
}

END_NS()
