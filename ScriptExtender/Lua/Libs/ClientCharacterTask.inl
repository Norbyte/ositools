#include <Lua/Shared/LuaMethodHelpers.h>
#include <Lua/Shared/LuaUserspaceClass.h>
#include <Lua/Client/CharacterTaskRegistry.h>

BEGIN_NS(ecl::lua)

CharacterTaskRegistry::CharacterTaskRegistry(ClientState& state)
	: state_(state)
{}

std::optional<dse::lua::RegistryEntry> CharacterTaskRegistry::ConstructTask(FixedString const& taskType, Character* character)
{
	auto ctor = constructors_.find(taskType);
	if (ctor == constructors_.end()) {
		ERR("Tried to construct character task of type '%s' that is not registered!", taskType.GetStringOrDefault());
		return {};
	}

	auto L = state_.GetState();
	ProtectedFunctionCaller<std::tuple<Character*>, dse::lua::RegistryEntry> caller;
	caller.Function = ctor.Value();
	caller.Args = std::tuple(character);
	if (caller.Call(L, "constructing CharacterTask")) {
		return std::move(caller.Retval.Value);
	} else {
		return {};
	}
}

void CharacterTaskRegistry::RegisterTaskType(FixedString const& taskType, dse::lua::Ref const& constructor)
{
	auto ctor = constructors_.find(taskType);
	if (ctor != constructors_.end()) {
		ERR("Tried to re-register character task of type '%s'!", taskType.GetStringOrDefault());
		return;
	}

	auto L = state_.GetState();
	constructors_.insert(std::make_pair(taskType, dse::lua::RegistryEntry(L, constructor)));
}


class UserspaceCharacterTaskClass : public RebindableUserspaceClassBase
{
public:
	inline UserspaceCharacterTaskClass(lua_State* L, FixedString const& taskType, Character* character)
		: RebindableUserspaceClassBase(L, PersistentRef{}),
		taskType_(taskType), character_(character)
	{}

	bool IsValid() const override
	{
		return ref_.IsValid(ecl::ExtensionState::Get().GetLua()->GetState());
	}

	bool Rebind() override
	{
		auto state = ecl::ExtensionState::Get().GetClientLua();
		auto task = state->GetCharacterTaskRegistry().ConstructTask(taskType_, character_);
		if (task) {
			L_ = state->GetState();
			reg_.ResetWithoutUnbind();
			reg_.Bind(L_, *task);
			ref_ = PersistentRef(L_, reg_);
			return true;
		} else {
			return false;
		}
	}

	void Attached()
	{
		CallMethod("Attached", Overload<void>{});
	}

	void Start()
	{
		CallMethod("Start", Overload<void>{});
	}

	void Stop()
	{
		CallMethod("Stop", Overload<void>{});
	}

	bool CanEnter()
	{
		return CallMethodOrDefault("CanEnter", false);
	}

	int GetPriority(int previousPriority)
	{
		return CallMethodOrDefault("GetPriority", 0, previousPriority);
	}

	int GetExecutePriority(int previousPriority)
	{
		return CallMethodOrDefault("GetExecutePriority", 0, previousPriority);
	}

	int GetActionCost()
	{
		return CallMethodOrDefault("GetActionCost", 0);
	}

	int GetTotalAPCost()
	{
		return CallMethodOrDefault("GetTotalAPCost", 0);
	}

	uint64_t GetAPWarning()
	{
		return CallMethodOrDefault("GetAPWarning", 0ull);
	}

	uint64_t GetError()
	{
		return CallMethodOrDefault("GetError", 0ull);
	}

	void SetCursor()
	{
		CallMethod("SetCursor", Overload<void>{});
	}

	WORD HandleInputEvent(dse::InputEvent* e, CharacterTask* task)
	{
		CallMethod("HandleInputEvent", Overload<void>{}, e, task);
		return 0;
	}

	void EnterPreview()
	{
		CallMethod("EnterPreview", Overload<void>{});
	}

	void UpdatePreview()
	{
		CallMethod("UpdatePreview", Overload<void>{});
	}

	void ExitPreview()
	{
		CallMethod("ExitPreview", Overload<void>{});
	}

	float GetSightRange()
	{
		return CallMethodOrDefault("GetSightRange", 0.0f);
	}

	void ClearAIColliding()
	{
		CallMethod("ClearAIColliding", Overload<void>{});
	}

	void SetAIColliding()
	{
		CallMethod("SetAIColliding", Overload<void>{});
	}

	int ValidateTargetRange()
	{
		return CallMethodOrDefault("GetSightRange", 0);
	}

	bool Enter()
	{
		return CallMethodOrDefault("Enter", false);
	}

	bool Update()
	{
		return CallMethodOrDefault("Update", true);
	}

	void Exit()
	{
		CallMethod("Exit", Overload<void>{});
	}

	bool CanExit()
	{
		return CallMethodOrDefault("CanExit", true);
	}

	bool CanExit2()
	{
		return CallMethodOrDefault("CanExit2", true);
	}

	void GetDescription(eoc::Text& desc)
	{
		auto fetchedDesc = CallMethod("GetDescription", Overload<STDString>{});
		if (fetchedDesc) {
			desc.Buf->Replace(FromUTF8(*fetchedDesc));
		}
	}

	void GetAPDescription(eoc::Text& desc)
	{
		auto fetchedDesc = CallMethod("GetAPDescription", Overload<STDString>{});
		if (fetchedDesc) {
			desc.Buf->Replace(FromUTF8(*fetchedDesc));
		}
	}

private:
	FixedString taskType_;
	Character* character_;
	PersistentRegistryEntry reg_;
};

struct UserspaceCharacterTask : public CharacterTask
{
	FixedString taskType_;
	std::unique_ptr<UserspaceCharacterTaskClass> cls_;

	UserspaceCharacterTask(int actionTypeId, FixedString const& taskType)
		: taskType_(taskType)
	{
		ActionTypeId = actionTypeId;
	}

	~UserspaceCharacterTask() override {}

	void NotifyAttached()
	{
		if (BoundCharacter && !cls_) {
			auto L = ecl::ExtensionState::Get().GetLua()->GetState();
			cls_ = std::make_unique<UserspaceCharacterTaskClass>(L, taskType_, BoundCharacter);
			cls_->Attached();
		}
	}

	UserspaceCharacterTask* CreateNew() override
	{
		return new UserspaceCharacterTask(ActionTypeId, taskType_);
	}

	void RaiseFlags(CharacterTaskFlags flags) override
	{
		Flags |= flags;
	}

	void ClearFlags(CharacterTaskFlags flags) override
	{
		Flags &= ~flags;
	}

	void SetCharacter(Character* c) override
	{
		BoundCharacter = c;
		NotifyAttached();
	}

	void Start() override
	{
		cls_->Start();
	}

	void Stop() override
	{
		cls_->Stop();
	}

	bool CanEnter() override
	{
		return cls_->CanEnter();
	}

	int GetPriority(int previousPriority) override
	{
		if (!cls_->IsValid() && cls_->Rebind()) {
			cls_->Attached();
			cls_->Start();

			if (IsInPreviewMode) {
				cls_->EnterPreview();
			}

			if ((Flags & CharacterTaskFlags::IsExecuting) == CharacterTaskFlags::IsExecuting) {
				cls_->Enter();
			}
		}

		return cls_->GetPriority(previousPriority);
	}

	int GetExecutePriority(int previousPriority) override
	{
		return cls_->GetExecutePriority(previousPriority);
	}

	int GetActionCost() override
	{
		return cls_->GetActionCost();
	}

	int GetTotalAPCost() override
	{
		return cls_->GetTotalAPCost();
	}

	uint64_t GetAPWarning() override
	{
		return cls_->GetAPWarning();
	}

	uint64_t GetError() override
	{
		return cls_->GetError();
	}

	void SetCursor() override
	{
		cls_->SetCursor();
	}
	
	void GetTaskInfo(eoc::Text& taskInfo, bool showAll) override
	{
		taskInfo.Buf->Replace(L"TaskInfo");
	}

	WORD* HandleInputEvent(WORD& result, dse::InputEvent* e) override
	{
		cls_->HandleInputEvent(e, this);
		result = 0;
		return &result;
	}

	void EnterPreview() override
	{
		cls_->EnterPreview();
		IsInPreviewMode = true;
	}

	void UpdatePreview() override
	{
		cls_->UpdatePreview();
	}

	void ExitPreview() override
	{
		cls_->ExitPreview();
		IsInPreviewMode = false;
	}

	bool IsInPreview() override
	{
		return IsInPreviewMode;
	}

	bool GetAbilityAndFlags(uint32_t*)
	{
		return false;
	}

	float GetSightRange()
	{
		return cls_->GetSightRange();
	}

	void ClearAIColliding() override
	{
		return cls_->ClearAIColliding();
	}

	void SetAIColliding() override
	{
		return cls_->SetAIColliding();
	}

	int ValidateTargetRange() override
	{
		return cls_->ValidateTargetRange();
	}

	bool Enter() override
	{
		return cls_->Enter();
	}
	
	bool Update() override
	{
		return cls_->Update(); // Return true to exit
	}

	void Exit() override
	{
		cls_->Exit();
	}

	bool CanExit() override
	{
		return cls_->CanExit();
	}

	bool CanExit2() override
	{
		return cls_->CanExit2();
	}

	void GetDescription(eoc::Text& desc) override
	{
		cls_->GetDescription(desc);
	}

	bool SyncSurfaceCells(ObjectSet<SurfaceCell>*) override
	{
		return false;
	}

	ComponentHandle* CreatePreviewEffect(ComponentHandle& pEffect) override
	{
		return &pEffect;
	}
	
	void GetAPDescription(eoc::Text& desc) override
	{
		cls_->GetAPDescription(desc);
	}
};

void RegisterCharacterTask(lua_State* L, FixedString taskType, Ref constructor)
{
	auto typeId = gExtender->GetClient().GetExtensionState().GetCharacterTaskBinder().GetOrRegisterType(taskType);
	ClientState::FromLua(L)->GetCharacterTaskRegistry().RegisterTaskType(taskType, constructor);
}

bool AttachCharacterTask(lua_State* L, ProxyParam<Character> ch, FixedString taskType)
{
	auto typeId = gExtender->GetClient().GetExtensionState().GetCharacterTaskBinder().GetType(taskType);
	if (!typeId) {
		OsiError("Cannot attach unknown task type '" << taskType << "' to character!");
		return false;
	}

	if (ch->InputController->TaskPrototypes.size() > *typeId && ch->InputController->TaskPrototypes[*typeId]) {
		OsiError("A task of type '" << taskType << "' is already attached to the character!");
		return false;
	}

	auto task = new UserspaceCharacterTask(*typeId, taskType);

	while (ch->InputController->TaskPrototypes.size() <= *typeId) {
		ch->InputController->TaskPrototypes.push_back(nullptr);
	}

	ch->InputController->TaskPrototypes[*typeId] = task;
	task->SetCharacter(ch);
	return true;
}

END_NS()
