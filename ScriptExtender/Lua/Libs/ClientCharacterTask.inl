#include <Lua/Shared/LuaMethodHelpers.h>
#include <Lua/Shared/LuaUserspaceClass.h>

BEGIN_NS(ecl::lua)

class UserspaceCharacterTaskClass : public UserspaceClassBase
{
public:
	inline UserspaceCharacterTaskClass(lua_State* L, RegistryOrLocalRef const& reg)
		: UserspaceClassBase(L, reg)
	{}

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
};

struct UserspaceCharacterTask : public CharacterTask
{
	lua_State* L_;
	RegistryEntry self_;
	UserspaceCharacterTaskClass cls_;

	UserspaceCharacterTask(lua_State* L, RegistryOrLocalRef const& idx)
		: L_(L), self_(L, idx), cls_(L, self_)
	{}

	~UserspaceCharacterTask() override {}

	UserspaceCharacterTask* CreateNew() override
	{
		auto task = new UserspaceCharacterTask(L_, self_);
		task->ActionTypeId = ActionTypeId;
		return task;
	}

	void RaiseFlags(uint32_t flags) override
	{
		Flags |= flags;
	}

	void ClearFlags(uint32_t flags) override
	{
		Flags &= ~flags;
	}

	void SetCharacter(Character* c) override
	{
		BoundCharacter = c;
	}

	void Start() override
	{
		cls_.Start();
	}

	void Stop() override
	{
		cls_.Stop();
	}

	bool CanEnter() override
	{
		return cls_.CanEnter();
	}

	int GetPriority(int previousPriority) override
	{
		return cls_.GetPriority(previousPriority);
	}

	int GetExecutePriority(int previousPriority) override
	{
		return cls_.GetExecutePriority(previousPriority);
	}

	int GetActionCost() override
	{
		return cls_.GetActionCost();
	}

	int GetTotalAPCost() override
	{
		return cls_.GetTotalAPCost();
	}

	uint64_t GetAPWarning() override
	{
		return cls_.GetAPWarning();
	}

	uint64_t GetError() override
	{
		return cls_.GetError();
	}

	void SetCursor() override
	{
		cls_.SetCursor();
	}
	
	void GetTaskInfo(eoc::Text& taskInfo, bool showAll) override
	{
		taskInfo.Buf->Replace(L"TaskInfo");
	}

	WORD* HandleInputEvent(WORD& result, dse::InputEvent* e) override
	{
		cls_.HandleInputEvent(e, this);
		result = 0;
		return &result;
	}

	void EnterPreview() override
	{
		cls_.EnterPreview();
		IsInPreviewMode = true;
	}

	void UpdatePreview() override
	{
		cls_.UpdatePreview();
	}

	void ExitPreview() override
	{
		cls_.ExitPreview();
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
		return cls_.GetSightRange();
	}

	void ClearAIColliding() override
	{
		return cls_.ClearAIColliding();
	}

	void SetAIColliding() override
	{
		return cls_.SetAIColliding();
	}

	int ValidateTargetRange() override
	{
		return cls_.ValidateTargetRange();
	}

	bool Enter() override
	{
		return cls_.Enter();
	}
	
	bool Update() override
	{
		return cls_.Update(); // Return true to exit
	}

	void Exit() override
	{
		cls_.Exit();
	}

	bool CanExit() override
	{
		return cls_.CanExit();
	}

	bool CanExit2() override
	{
		return cls_.CanExit2();
	}

	void GetDescription(eoc::Text& desc) override
	{
		cls_.GetDescription(desc);
	}

	bool SyncSurfaceCells(ObjectSet<esv::SurfaceCell>*) override
	{
		return false;
	}

	ComponentHandle* CreatePreviewEffect(ComponentHandle& pEffect) override
	{
		return &pEffect;
	}
	
	void GetAPDescription(eoc::Text& desc) override
	{
		cls_.GetAPDescription(desc);
	}
};

void RegisterCharacterTask(lua_State* L, ProxyParam<Character> ch, RegistryOrLocalRef obj)
{
	auto task = new UserspaceCharacterTask(L, obj);
	task->ActionTypeId = (int)ch->InputController->TaskPrototypes.size();
	ch->InputController->TaskPrototypes.push_back(task);
}

END_NS()
