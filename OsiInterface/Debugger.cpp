#include "stdafx.h"
#include "Debugger.h"
#include "NodeHooks.h"
#include "OsirisProxy.h"
#include <sstream>

#undef DUMP_TRACEPOINTS

namespace osidbg
{
	Debugger::Debugger(OsirisGlobals const & globals, DebugMessageHandler & messageHandler)
		: globals_(globals), messageHandler_(messageHandler),
		breakpoints_(new std::unordered_map<uint64_t, Breakpoint>())
	{
		if (messageHandler_.IsConnected()) {
			globalBreakpoints_ = GlobalBreakpointType::GlobalBreakOnStoryLoaded;
		}
		else {
			globalBreakpoints_ = 0;
		}

		messageHandler_.SetDebugger(this);

		using namespace std::placeholders;
		gNodeVMTWrappers->IsValidPreHook = std::bind(&Debugger::IsValidPreHook, this, _1, _2, _3);
		gNodeVMTWrappers->IsValidPostHook = std::bind(&Debugger::IsValidPostHook, this, _1, _2, _3, _4);
		gNodeVMTWrappers->PushDownPreHook = std::bind(&Debugger::PushDownPreHook, this, _1, _2, _3, _4, _5);
		gNodeVMTWrappers->PushDownPostHook = std::bind(&Debugger::PushDownPostHook, this, _1, _2, _3, _4, _5);
		gNodeVMTWrappers->InsertPreHook = std::bind(&Debugger::InsertPreHook, this, _1, _2, _3);
		gNodeVMTWrappers->InsertPostHook = std::bind(&Debugger::InsertPostHook, this, _1, _2, _3);
		Debug(L"Debugger::Debugger(): Attached to story");

		DetectGameVersion();
	}

	Debugger::~Debugger()
	{
		Debug(L"Debugger::~Debugger(): Shutting down debugger");
		messageHandler_.SendDebugSessionEnded();
		messageHandler_.SetDebugger(nullptr);

		if (gNodeVMTWrappers) {
			gNodeVMTWrappers->IsValidPreHook = std::function<void(Node *, VirtTupleLL *, AdapterRef *)>();
			gNodeVMTWrappers->IsValidPostHook = std::function<void(Node *, VirtTupleLL *, AdapterRef *, bool)>();
			gNodeVMTWrappers->PushDownPreHook = std::function<void(Node *, VirtTupleLL *, AdapterRef *, EntryPoint, bool)>();
			gNodeVMTWrappers->PushDownPostHook = std::function<void(Node *, VirtTupleLL *, AdapterRef *, EntryPoint, bool)>();
			gNodeVMTWrappers->InsertPreHook = std::function<void(Node *, TuplePtrLL *, bool)>();
			gNodeVMTWrappers->InsertPostHook = std::function<void(Node *, TuplePtrLL *, bool)>();
		}
	}

	void Debugger::StoryLoaded()
	{
		ServerThreadReentry();
		isInitialized_ = false;
		UpdateRuleActionMappings();
		messageHandler_.SendStoryLoaded();
		if (globalBreakpoints_ & GlobalBreakpointType::GlobalBreakOnStoryLoaded) {
			GlobalBreakpointInServerThread(GlobalBreakpointReason::StoryLoaded);
		}
	}

	void Debugger::MergeStarted()
	{
		ServerThreadReentry();
		// Disable debugging during merge, as the nodes will be changing dynamically
		// which breaks most debugger assumptions
		debuggingDisabled_ = true;

	}

	void Debugger::MergeFinished()
	{
		ServerThreadReentry();
		debuggingDisabled_ = false;

		isInitialized_ = true;
		UpdateRuleActionMappings();
		if (globalBreakpoints_ & GlobalBreakpointType::GlobalBreakOnGameInit) {
			GlobalBreakpointInServerThread(GlobalBreakpointReason::GameInit);
		}
	}

	void Debugger::GameInitHook()
	{
		ServerThreadReentry();
		isInitialized_ = true;
		if (globalBreakpoints_ & GlobalBreakpointType::GlobalBreakOnGameInit) {
			GlobalBreakpointInServerThread(GlobalBreakpointReason::GameInit);
		}
	}

	void Debugger::DeleteAllDataHook()
	{
		ServerThreadReentry();
		isInitialized_ = false;
		if (globalBreakpoints_ & GlobalBreakpointType::GlobalBreakOnGameExit) {
			GlobalBreakpointInServerThread(GlobalBreakpointReason::GameExit);
		}
	}

	ResultCode Debugger::SetGlobalBreakpoints(GlobalBreakpointType breakpoints)
	{
		if (breakpoints & ~GlobalBreakpointTypeAll) {
			return ResultCode::UnsupportedBreakpointType;
		}

		Debug(L"Debugger::SetGlobalBreakpoints(): Set to %08x", breakpoints);
		globalBreakpoints_ = breakpoints;
		return ResultCode::Success;
	}

	void Debugger::BeginUpdatingNodeBreakpoints()
	{
		Debug(L"Debugger::BeginUpdatingNodeBreakpoints()");
		pendingBreakpoints_.reset(new std::unordered_map<uint64_t, Breakpoint>());
	}

	ResultCode Debugger::AddBreakpoint(uint32_t nodeId, uint32_t goalId, bool isInit, int32_t actionIndex, BreakpointType type)
	{
		if (type & ~BreakpointTypeAll) {
			Debug(L"Debugger::AddBreakpoint(): Unsupported breakpoint type set: %08x", type);
			return ResultCode::UnsupportedBreakpointType;
		}

		if (nodeId > (*globals_.Nodes)->Db.Size) {
			Debug(L"Debugger::AddBreakpoint(): Tried to set on nonexistent node ID %d", nodeId);
			return ResultCode::InvalidNodeId;
		}

		if (goalId > (*globals_.Goals)->Count) {
			Debug(L"Debugger::AddBreakpoint(): Tried to set on nonexistent goal ID %d", nodeId);
			return ResultCode::InvalidGoalId;
		}

		uint64_t breakpointId;
		if (actionIndex == -1) {
			if (nodeId == 0) {
				Debug(L"Debugger::AddBreakpoint(): Node ID must be nonzero for node actions", nodeId);
				return ResultCode::InvalidNodeId;
			}

			breakpointId = MakeNodeBreakpointId(nodeId);
		} else {
			if (nodeId != 0) {
				breakpointId = MakeRuleActionBreakpointId(nodeId, actionIndex);
			} else if (goalId != 0) {
				if (isInit) {
					breakpointId = MakeGoalInitBreakpointId(goalId, actionIndex);
				} else {
					breakpointId = MakeGoalExitBreakpointId(goalId, actionIndex);
				}
			} else {
				Debug(L"Debugger::AddBreakpoint(): No node/goal specified");
				return ResultCode::InvalidNodeId;
			}
		}

		Debug(L"Debugger::AddBreakpoint(): Set on key %016x to %08x", breakpointId, type);
		Breakpoint bp;
		bp.nodeId = nodeId;
		bp.goalId = goalId;
		bp.isInit = isInit;
		bp.actionIndex = actionIndex;
		bp.type = type;
		(*pendingBreakpoints_)[breakpointId] = bp;

		return ResultCode::Success;
	}

	void Debugger::FinishUpdatingNodeBreakpoints()
	{
		Debug(L"Debugger::FinishUpdatingNodeBreakpoints()");

		pendingActions_.push([this]() {
			auto pendingBps = std::move(this->pendingBreakpoints_);
			if (pendingBps.get() != nullptr) {
				Debug(L"Debugger::FinishUpdatingNodeBreakpoints(): Syncing breakpoints in server thread");
				this->breakpoints_.swap(pendingBps);
			}
		});
	}

	ResultCode Debugger::ContinueExecution(DbgContinue_Action action, uint32_t breakpointMask, uint32_t flags)
	{
		if (breakpointMask & ~BreakpointTypeAll) {
			Debug(L"Debugger::ContinueExecution(): Unsupported breakpoint type set: %08x", breakpointMask);
			return ResultCode::UnsupportedBreakpointType;
		}

		if (flags & ~ContinueFlagAll) {
			Debug(L"Debugger::ContinueExecution(): Unsupported flag set: %08x", flags);
			return ResultCode::UnsupportedContinueFlags;
		}

		std::unique_lock<std::mutex> lk(breakpointMutex_);

		if (action == DbgContinue_Action_PAUSE) {
			if (isPaused_) {
				Debug(L"Debugger::ContinueExecution(): Already paused");
				return ResultCode::InPause;
			}

			Debug(L"Debugger::ContinueExecution(): Force pause on next node");
			// Forcibly break on the next call
			forceBreakpoint_ = true;
			forceBreakpointMask_ = breakpointMask;
			forceBreakpointFlags_ = flags;
			maxBreakDepth_ = 0x7fffffff;
			// This is not a "continue" message, it just sets the breakpoint flags,
			// so we don't go through the continue code here
			return ResultCode::Success;
		}

		if (!isPaused_) {
			Debug(L"Debugger::ContinueExecution(): Not paused");
			return ResultCode::NotInPause;
		}

		forceBreakpointMask_ = breakpointMask;
		forceBreakpointFlags_ = flags;

		switch (action) {
		case DbgContinue_Action_CONTINUE:
			// No forced break on the next node
			forceBreakpoint_ = false;
			maxBreakDepth_ = 0;
			break;

		case DbgContinue_Action_STEP_OVER:
			// Step over the current frame; max depth is the current call stack depth
			forceBreakpoint_ = true;
			maxBreakDepth_ = (uint32_t)callStack_.size();
			break;

		case DbgContinue_Action_STEP_INTO:
			// Step into the current frame; max depth is unlimited
			forceBreakpoint_ = true;
			maxBreakDepth_ = 0x7fffffff;
			break;

		case DbgContinue_Action_STEP_OUT:
			// Step out of the current frame; max depth is current - 1
			forceBreakpoint_ = true;
			maxBreakDepth_ = (uint32_t)callStack_.size() - 1;
			break;

		default:
			Debug(L"Debugger::ContinueExecution(): Continue action %d not known", action);
			return ResultCode::InvalidContinueAction;
		}

		Debug(L"Debugger::ContinueExecution(): Continuing; action %d, mask %08x, flags %08x", action, breakpointMask, flags);
		isPaused_ = false;
		breakpointCv_.notify_one();

		return ResultCode::Success;
	}

	void Debugger::ClearAllBreakpoints()
	{
		globalBreakpoints_ = 0;
		breakpoints_->clear();
		forceBreakpoint_ = false;
		maxBreakDepth_ = 0;
		forceBreakpointMask_ = 0;
	}

	void Debugger::SyncStory()
	{
		auto const & goalDb = (*globals_.Goals);
		for (unsigned i = 0; i < goalDb->Count; i++) {
			auto goal = goalDb->FindGoal(i + 1);
			messageHandler_.SendSyncStory(goal);
		}

		auto const & databaseDb = (*globals_.Databases)->Db;
		for (unsigned i = 0; i < databaseDb.Size; i += 100) {
			uint32_t numDatabases = std::min<uint32_t>(databaseDb.Size - i, 100);
			messageHandler_.SendSyncStory(&databaseDb.Start[i], numDatabases);
		}

		auto const & nodeDb = (*globals_.Nodes)->Db;
		for (unsigned i = 0; i < nodeDb.Size; i += 100) {
			uint32_t numNodes = std::min<uint32_t>(nodeDb.Size - i, 100);
			messageHandler_.SendSyncStory(&nodeDb.Start[i], numNodes);
		}
	}

	void Debugger::DetectGameVersion()
	{
		auto & dbs = (*globals_.Databases)->Db;
		assert(dbs.Size > 0);
		auto db1 = *dbs.Start;
		bool isDE = Database::IsDatabaseDOS2DE(db1,
			gOsirisProxy->GetOsirisDllStart(),
			(uint8_t *)gOsirisProxy->GetOsirisDllStart() + gOsirisProxy->GetOsirisDllSize());
		if (isDE) {
			gGameType = GameType::DOS2DE;
			Debug(L"Debugger::DetectGameVersion(): Detected D:OS2 DE");
		} else {
			gGameType = GameType::DOS2;
			Debug(L"Debugger::DetectGameVersion(): Detected D:OS2 Classic");
		}
	}

	void Debugger::ServerThreadReentry()
	{
		// Called when the debugger is entered from any of the server thread hooks
		std::function<void ()> func;
		while (pendingActions_.try_pop(func)) {
			func();
		}
	}

	void Debugger::FinishedSingleStep()
	{
		// Called when we're finished single stepping and want to cancel single-step triggers
		forceBreakpoint_ = false;
		maxBreakDepth_ = 0;
		forceBreakpointMask_ = 0;
	}

	bool Debugger::ForcedBreakpointConditionsSatisfied(Node * bpNode, BreakpointType bpType)
	{
		// Check if the current frame type is one we can break on
		if (!(forceBreakpointMask_ & bpType)) {
			return false;
		}

		// Check if we're on the correct stack depth
		if (callStack_.size() > maxBreakDepth_) {
			return false;
		}

		// Skip rule pushdown frames (avoids unnecessary additional single-stepping frame)
		if (forceBreakpointFlags_ & ContinueSkipRulePushdown) {
			if (bpType == BreakpointType::BreakOnPushDown
				&& bpNode != nullptr
				&& gNodeVMTWrappers->GetType(bpNode) == NodeType::Rule) {
				return false;
			}
		}

		// Skip database propagation nodes
		if (forceBreakpointFlags_ & ContinueSkipDbPropagation) {
			// Look for a likely database propagation signature in the call stack
			// (an Insert/Delete frame followed by a Pushdown frame)
			for (auto i = 0; i < callStack_.size() - 1; i++) {
				auto & first = callStack_[i];
				auto & second = callStack_[i + 1];

				if ((first.frameType == BreakpointReason::NodeInsertTuple
					|| first.frameType == BreakpointReason::NodeDeleteTuple)
					&& (second.frameType == BreakpointReason::NodePushDownTuple
						|| second.frameType == BreakpointReason::NodePushDownTupleDelete)) {
					// Check whether the first node is a parent of the second node
					auto secondType = gNodeVMTWrappers->GetType(second.node);
					uint32_t parentNodeId;
					if (secondType == NodeType::Rule || secondType == NodeType::RelOp)
					{
						auto rel = static_cast<RelNode *>(second.node);
						parentNodeId = rel->Parent.Id;
					}
					else if (secondType == NodeType::And || secondType == NodeType::NotAnd) {
						auto join = static_cast<JoinNode *>(second.node);
						parentNodeId = join->Left.Id;
					}
					else {
						Debug(L"Debugger::ForcedBreakpointConditionsSatisfied(): Illegal call order: %d --> %d",
							first.node->Id, second.node->Id);
						parentNodeId = first.node->Id;
					}

					if (parentNodeId != first.node->Id) {
						return false;
					}
				}
			}
		}

		return true;
	}

	bool Debugger::ShouldTriggerBreakpoint(Node * bpNode, uint64_t bpNodeId, BreakpointType bpType, GlobalBreakpointType globalBpType)
	{
		if (debuggingDisabled_) {
			return false;
		}

		// Check if there is a breakpoint on this node ID
		auto it = breakpoints_->find(bpNodeId);
		if (it != breakpoints_->end()
			&& (it->second.type & bpType)) {
			return true;
		}

		// Check if there is a global breakpoint for this frame type
		if (globalBreakpoints_ & globalBpType) {
			return true;
		}

		// Check if we're single stepping
		if (forceBreakpoint_ 
			&& ForcedBreakpointConditionsSatisfied(bpNode, bpType)) {
			return true;
		}

		return false;
	}

	void Debugger::ConditionalBreakpointInServerThread(Node * bpNode, uint64_t bpNodeId, BreakpointType bpType, GlobalBreakpointType globalBpType)
	{
		if (ShouldTriggerBreakpoint(bpNode, bpNodeId, bpType, globalBpType)) {
			FinishedSingleStep();
			BreakpointInServerThread();
		}
	}

	void Debugger::BreakpointInServerThread()
	{
		if (callStack_.empty()) {
			Fail(L"Tried to trigger breakpoint with empty callstack");
		}

		auto const & lastFrame = *callStack_.rbegin();
		Debug(L"Debugger::BreakpointInServerThread(): type %d", lastFrame.frameType);
		{
			std::unique_lock<std::mutex> lk(breakpointMutex_);
			isPaused_ = true;
		}

		messageHandler_.SendBreakpointTriggered(callStack_);

		{
			std::unique_lock<std::mutex> lk(breakpointMutex_);
			breakpointCv_.wait(lk, [this]() { return !this->isPaused_; });
		}

		Debug(L"Continuing from breakpoint.");
		ServerThreadReentry();
	}

	void Debugger::GlobalBreakpointInServerThread(GlobalBreakpointReason reason)
	{
		if (debuggingDisabled_) return;

		Debug(L"Debugger::GlobalBreakpointInServerThread(): Reason %d", reason);
		{
			std::unique_lock<std::mutex> lk(breakpointMutex_);
			isPaused_ = true;
		}

		messageHandler_.SendGlobalBreakpointTriggered(reason);

		{
			std::unique_lock<std::mutex> lk(breakpointMutex_);
			breakpointCv_.wait(lk, [this]() { return !this->isPaused_; });
		}

		Debug(L"Continuing from breakpoint.");
		ServerThreadReentry();
	}

	void Debugger::PushFrame(CallStackFrame const & frame)
	{
		callStack_.push_back(frame);
	}

	void Debugger::PopFrame(CallStackFrame const & frame)
	{
		if (callStack_.empty()) {
			Fail(L"Tried to remove frame from empty callstack");
		}

		auto const & lastFrame = *callStack_.rbegin();
		if (lastFrame.frameType != frame.frameType
			|| lastFrame.node != frame.node
			|| lastFrame.goal != frame.goal
			|| lastFrame.tupleLL != frame.tupleLL
			|| lastFrame.tuplePtrLL != frame.tuplePtrLL
			|| lastFrame.actionIndex != frame.actionIndex) {
			Fail(L"Call stack frame mismatch");
		}

		callStack_.pop_back();
	}

	void Debugger::IsValidPreHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter)
	{
		ServerThreadReentry();
		PushFrame({ BreakpointReason::NodeIsValid, node, nullptr, 0, &tuple->Data, nullptr });

#if defined(DUMP_TRACEPOINTS)
		Debug(L"IsValid(Node %d)", node->Id);
#endif
		ConditionalBreakpointInServerThread(
			node,
			MakeNodeBreakpointId(node->Id),
			BreakOnValid,
			GlobalBreakOnValid);
	}

	void Debugger::IsValidPostHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, bool succeeded)
	{
		ServerThreadReentry();
		PopFrame({ BreakpointReason::NodeIsValid, node, nullptr, 0, &tuple->Data, nullptr });
	}

	void Debugger::PushDownPreHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted)
	{
		ServerThreadReentry();
		auto reason = deleted ? BreakpointReason::NodePushDownTupleDelete : BreakpointReason::NodePushDownTuple;
		PushFrame({ reason, node, nullptr, 0, &tuple->Data, nullptr });

#if defined(DUMP_TRACEPOINTS)
		Debug(L"PushDown(Node %d)", node->Id);
#endif
		ConditionalBreakpointInServerThread(
			node,
			MakeNodeBreakpointId(node->Id),
			BreakOnPushDown,
			GlobalBreakOnPushDown);
	}

	void Debugger::PushDownPostHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted)
	{
		ServerThreadReentry();
		auto reason = deleted ? BreakpointReason::NodePushDownTupleDelete : BreakpointReason::NodePushDownTuple;
		PopFrame({ reason, node, nullptr, 0, &tuple->Data, nullptr });
	}

	void Debugger::InsertPreHook(Node * node, TuplePtrLL * tuple, bool deleted)
	{
		ServerThreadReentry();
		auto reason = deleted ? BreakpointReason::NodeDeleteTuple : BreakpointReason::NodeInsertTuple;
		PushFrame({ reason, node, nullptr, 0, nullptr, tuple });

#if defined(DUMP_TRACEPOINTS)
		Debug(L"%s(Node %d)", deleted ? L"Delete" : L"Insert", node->Id);
#endif
		ConditionalBreakpointInServerThread(
			node,
			MakeNodeBreakpointId(node->Id),
			deleted ? BreakOnDelete : BreakOnInsert,
			deleted ? GlobalBreakOnDelete : GlobalBreakOnInsert);
	}

	void Debugger::InsertPostHook(Node * node, TuplePtrLL * tuple, bool deleted)
	{
		ServerThreadReentry();
		auto reason = deleted ? BreakpointReason::NodeDeleteTuple : BreakpointReason::NodeInsertTuple;
		PopFrame({ reason, node, nullptr, 0, nullptr, tuple });
	}

	uint64_t Debugger::MakeNodeBreakpointId(uint32_t nodeId)
	{
		return ((uint64_t)BreakpointItemType::BP_Node << 56) | nodeId;
	}

	uint64_t Debugger::MakeRuleActionBreakpointId(uint32_t nodeId, uint32_t actionIndex)
	{
		return ((uint64_t)BreakpointItemType::BP_RuleAction << 56) | ((uint64_t)actionIndex << 32) | nodeId;
	}

	uint64_t Debugger::MakeGoalInitBreakpointId(uint32_t goalId, uint32_t actionIndex)
	{
		return ((uint64_t)BreakpointItemType::BP_GoalInit << 56) | ((uint64_t)actionIndex << 32) | goalId;
	}

	uint64_t Debugger::MakeGoalExitBreakpointId(uint32_t goalId, uint32_t actionIndex)
	{
		return ((uint64_t)BreakpointItemType::BP_GoalExit << 56) | ((uint64_t)actionIndex << 32) | goalId;
	}

	void Debugger::AddRuleActionMappings(Node * node, Goal * goal, bool isInit, RuleActionList * actions)
	{
		auto head = actions->Actions.Head;
		auto current = head->Next;
		uint32_t actionIndex = 0;
		while (current != head) {
			ruleActionMappings_.insert(std::pair<RuleActionNode *, RuleActionMapping>(
				current->Item,
				{ current->Item, node, goal, isInit, actionIndex }
			));
			current = current->Next;
			actionIndex++;
		}
	}

	void Debugger::UpdateRuleActionMappings()
	{
		ruleActionMappings_.clear();

		auto const & nodeDb = (*globals_.Nodes)->Db;
		for (unsigned i = 0; i < nodeDb.Size; i++) {
			auto node = nodeDb.Start[i];
			NodeType type = gNodeVMTWrappers->GetType(node);
			if (type == NodeType::Rule) {
				auto rule = static_cast<RuleNode *>(node);
				AddRuleActionMappings(rule, nullptr, false, rule->Calls);
			}
		}

		auto const & goalDb = (*globals_.Goals);
		for (unsigned i = 0; i < goalDb->Count; i++) {
			auto goal = goalDb->FindGoal(i + 1);
			AddRuleActionMappings(nullptr, goal, true, goal->InitCalls);
			AddRuleActionMappings(nullptr, goal, false, goal->ExitCalls);
		}
	}

	RuleActionMapping const * Debugger::FindActionMapping(RuleActionNode * action)
	{
		auto mapping = ruleActionMappings_.find(action);
		if (mapping == ruleActionMappings_.end()) {
			Debug(L"Debugger::FindActionMapping(%016x): Could not find action mapping for rule action", action);
			return nullptr;
		}

		return &mapping->second;
	}

	void Debugger::RuleActionPreHook(RuleActionNode * action)
	{
		// Avoid action mapping errors during merge
		if (debuggingDisabled_) {
			return;
		}

		ServerThreadReentry();
		auto const * mapping = FindActionMapping(action);
		if (mapping == nullptr) {
			return;
		}

		BreakpointReason reason;
		BreakpointType bpType;
		GlobalBreakpointType globalBpType;
		uint64_t breakpointId;
		if (mapping->rule != nullptr) {
			reason = BreakpointReason::RuleActionCall;
			bpType = BreakpointType::BreakOnRuleAction;
			globalBpType = GlobalBreakpointType::GlobalBreakOnRuleAction;
			breakpointId = MakeRuleActionBreakpointId(mapping->rule->Id, mapping->actionIndex);
#if defined(DUMP_TRACEPOINTS)
			Debug(L"RuleAction(Rule %d, Action %d)", mapping->rule->Id, mapping->actionIndex);
#endif
		} else if (mapping->isInit) {
			reason = BreakpointReason::GoalInitCall;
			bpType = BreakpointType::BreakOnInitCall;
			globalBpType = GlobalBreakpointType::GlobalBreakOnInitCall;
			breakpointId = MakeGoalInitBreakpointId(mapping->goal->Id, mapping->actionIndex);
#if defined(DUMP_TRACEPOINTS)
			Debug(L"GoalInit(Goal %d, Action %d)", mapping->goal->Id, mapping->actionIndex);
#endif
		} else {
			reason = BreakpointReason::GoalExitCall;
			bpType = BreakpointType::BreakOnExitCall;
			globalBpType = GlobalBreakpointType::GlobalBreakOnExitCall;
			breakpointId = MakeGoalExitBreakpointId(mapping->goal->Id, mapping->actionIndex);
#if defined(DUMP_TRACEPOINTS)
			Debug(L"GoalExit(Goal %d, Action %d)", mapping->goal->Id, mapping->actionIndex);
#endif
		}

		PushFrame({ reason, mapping->rule, mapping->goal, mapping->actionIndex, nullptr, nullptr });

		ConditionalBreakpointInServerThread(nullptr, breakpointId, bpType, globalBpType);

		// Capture debug output from the DebugBreak() Osiris call and forward it to the dbg frontend
		
		if (action->FunctionName != nullptr
			&& action->Arguments != nullptr
			&& action->Arguments->Size == 1
			&& strcmp(action->FunctionName, "DebugBreak") == 0) {
			TypedValue * message = action->Arguments->Head->Next->Item;
			if (message->TypeId == (uint32_t)ValueType::String
				&& message->Value.Val.String != nullptr) {
				messageHandler_.SendDebugOutput(message->Value.Val.String);
			}
			else {
				Debug(L"Invalid message parameter type for DebugBreak(): %d", message->TypeId);
			}
		}
	}

	void Debugger::RuleActionPostHook(RuleActionNode * action)
	{
		// Avoid action mapping errors during merge
		if (debuggingDisabled_) {
			return;
		}

		ServerThreadReentry();
		auto const * mapping = FindActionMapping(action);
		if (mapping == nullptr) {
			return;
		}

		BreakpointReason reason;
		if (mapping->rule != nullptr) {
			reason = BreakpointReason::RuleActionCall;
		}
		else if (mapping->isInit) {
			reason = BreakpointReason::GoalInitCall;
		}
		else {
			reason = BreakpointReason::GoalExitCall;
		}

		PopFrame({ reason, mapping->rule, mapping->goal, mapping->actionIndex, nullptr, nullptr });
	}
}
