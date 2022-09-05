#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/GameObjects/Controllers.h>

BEGIN_NS(esv)

void TaskController::PurgeQueue()
{
	if (UpdateInProgress) return;

	auto const& sym = GetStaticSymbols();

	while (!Tasks.empty()) {
		auto task = Tasks.pop();
		auto character = GetEntityWorld()->GetComponent<esv::Character>(task->Character);
		character->NeedsUpdateCount--;
		character->ServerControlRefCount--;

		if (task->TaskState != 3) {
			task->TaskState = 3;
			task->Exit();
		}

		sym.esv__OsirisTaskFactory__ReleaseTask(*sym.esv__OsirisTaskFactory, task);
	}
}

void TaskController::FlushQueue()
{
	if (UpdateInProgress) {
		FlushRequested = true;
		return;
	}

	auto const& sym = GetStaticSymbols();

	while (!Tasks.empty()) {
		auto task = Tasks.pop();
		auto character = GetEntityWorld()->GetComponent<esv::Character>(task->Character);
		character->NeedsUpdateCount--;
		character->ServerControlRefCount--;

		if (task->TaskState != 3) {
			task->TaskState = 3;
			task->Finish();
		}

		sym.esv__OsirisTaskFactory__ReleaseTask(*sym.esv__OsirisTaskFactory, task);
	}
}

void TaskController::QueueTask(Task* task)
{
	if (task->Character != this->Character) {
		OsiError("Tried to queue task on a different character?");
		return;
	}

	for (int32_t i = 0; i < Tasks.size(); i++) {
		if (Tasks[i] == task) {
			OsiError("Task is already queued!");
			return;
		}
	}

	Tasks.push_back(task);

	auto character = GetEntityWorld()->GetComponent<esv::Character>(task->Character);
	character->NeedsUpdateCount++;
	character->ServerControlRefCount++;
}

void GameAction::Destroy()
{
	switch (ActionType) {
	case GameActionType::RainAction:
		static_cast<RainAction*>(this)->Finished = true;
		break;

	case GameActionType::StormAction:
		static_cast<StormAction*>(this)->LifeTime = 0.0f;
		break;

	case GameActionType::WallAction:
		static_cast<WallAction*>(this)->LifeTime = 0.0f;
		break;

	case GameActionType::TornadoAction:
		static_cast<TornadoAction*>(this)->Finished = true;
		break;

	case GameActionType::StatusDomeAction:
		static_cast<StatusDomeAction*>(this)->Finished = true;
		break;

	case GameActionType::GameObjectMoveAction:
		static_cast<GameObjectMoveAction*>(this)->DoneMoving = true;
		break;

	default:
		OsiError("Don't know how to destroy game action type " << (unsigned)ActionType);
		break;
	}
}

END_NS()

/// <lua_module>Action</lua_module>
BEGIN_NS(esv::lua::action)

Task* CreateOsirisTask(TaskType type, ProxyParam<Character> character)
{
	auto const& sym = GetStaticSymbols();
	ComponentHandle handle;
	character->GetObjectHandle(handle);
	return sym.esv__OsirisTaskFactory__CreateTask(*sym.esv__OsirisTaskFactory, type, handle.Handle);
}

void QueueOsirisTask(ProxyParam<Task> task)
{
	auto character = GetEntityWorld()->GetComponent<esv::Character>(task->Character);
	character->OsirisController->QueueTask(task.Object);
}

template <class T>
T* PrepareGameAction(GameActionType type, FixedString const& skillId, ProxyParam<Character> character)
{
	auto const& sym = GetStaticSymbols();
	auto actionMgr = sym.GetGameActionManager();
	auto action = static_cast<T*>(sym.esv__GameActionManager__CreateAction(actionMgr, type, 0));

	ComponentHandle characterHandle;
	character->GetObjectHandle(characterHandle);
	action->Owner = characterHandle;

	return action;
}

#define MAKE_ACTION(ty) case GameActionType::ty: \
	action = PrepareGameAction<ty>(type, skillId, character); \
	static_cast<ty*>(action)->Position = character->WorldPos; \
	static_cast<ty*>(action)->SkillId = skillId; \
	break;

GameAction* CreateGameAction(GameActionType type, FixedString const& skillId, ProxyParam<Character> character)
{
	GameAction* action{ nullptr };

	switch (type) {
		MAKE_ACTION(RainAction)
		MAKE_ACTION(StormAction)
		MAKE_ACTION(TornadoAction)
		MAKE_ACTION(PathAction)
		MAKE_ACTION(StatusDomeAction)

	case GameActionType::WallAction:
		action = PrepareGameAction<WallAction>(type, skillId, character);
		static_cast<WallAction*>(action)->SkillId = skillId;
		break;

	case GameActionType::GameObjectMoveAction:
		action = PrepareGameAction<GameObjectMoveAction>(type, skillId, character);
		break;

	default:
		OsiErrorS("Don't know how to make actions of this type");
		break;
	}
	
	return action;
}

#undef MAKE_ACTION

void ExecuteGameAction(lua_State* L, ProxyParam<GameAction> action, std::optional<glm::vec3> targetPos)
{
	if (action->Active) {
		OsiErrorS("Game action was already executed!");
		return;
	}

	auto const& sym = GetStaticSymbols();

	if (action->ActionType == GameActionType::WallAction) {
		auto wall = static_cast<WallAction*>(action.Object);
		sym.esv__WallAction__CreateWall(wall);
	}

	if (action->ActionType == GameActionType::TornadoAction) {
		auto tornado = static_cast<TornadoAction*>(action.Object);
		sym.esv__TornadoAction__Setup(tornado);
	}

	if (action->ActionType == GameActionType::GameObjectMoveAction) {
		if (!targetPos) {
			luaL_error(L, "ExecuteGameAction() needs a position parameter when executing a move action!");
			return;
		}

		auto move = static_cast<GameObjectMoveAction*>(action.Object);
		sym.esv__GameObjectMoveAction__Setup(move, move->Owner, *targetPos);
	}

	auto actionMgr = sym.GetGameActionManager();
	sym.AddGameActionWrapper(actionMgr, action.Object);
}

void DestroyGameAction(ProxyParam<GameAction> action)
{
	action->Destroy();
}

void RegisterActionLib()
{
	DECLARE_MODULE(Action, Server)
	BEGIN_MODULE()
	MODULE_FUNCTION(CreateOsirisTask)
	MODULE_FUNCTION(QueueOsirisTask)
	MODULE_FUNCTION(CreateGameAction)
	MODULE_FUNCTION(ExecuteGameAction)
	MODULE_FUNCTION(DestroyGameAction)
	END_MODULE()
}

END_NS()
