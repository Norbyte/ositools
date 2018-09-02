#include "stdafx.h"
#include "Debugger.h"
#include "NodeHooks.h"
#include "OsirisProxy.h"
#include <sstream>

namespace osidbg
{
	Debugger::Debugger(OsirisGlobals const & globals, DebugMessageHandler & messageHandler)
		: globals_(globals), messageHandler_(messageHandler)
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
		UpdateRuleActionMappings();
		messageHandler_.SendStoryLoaded();
		if (globalBreakpoints_ & GlobalBreakpointType::GlobalBreakOnStoryLoaded) {
			GlobalBreakpointInServerThread(GlobalBreakpointReason::StoryLoaded);
		}
	}

	void Debugger::GameInitHook()
	{
		if (globalBreakpoints_ & GlobalBreakpointType::GlobalBreakOnGameInit) {
			GlobalBreakpointInServerThread(GlobalBreakpointReason::GameInit);
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

	ResultCode Debugger::SetBreakpoint(uint32_t nodeId, BreakpointType type)
	{
		if (type & ~BreakpointTypeAll) {
			return ResultCode::UnsupportedBreakpointType;
		}

		if (nodeId == 0 || nodeId > (*globals_.Nodes)->Db.Size) {
			return ResultCode::InvalidNodeId;
		}

		if (!type) {
			Debug(L"Debugger::SetBreakpoint(): Removed on node %d", nodeId);
			auto it = breakpoints_.find(nodeId);
			if (it != breakpoints_.end()) {
				breakpoints_.erase(it);
			}
		}
		else
		{
			Debug(L"Debugger::SetBreakpoint(): Set on node %d to %08x", nodeId, type);
			Breakpoint bp;
			bp.nodeId = nodeId;
			bp.type = type;
			breakpoints_[nodeId] = bp;
		}

		return ResultCode::Success;
	}

	ResultCode Debugger::ContinueExecution(DbgContinue_Action action)
	{
		std::unique_lock<std::mutex> lk(breakpointMutex_);
		if (!isPaused_) {
			Debug(L"Debugger::ContinueExecution(): Not paused");
			return ResultCode::NotInPause;
		}

		if (action == DbgContinue_Action_PAUSE) {
			if (isPaused_) {
				Debug(L"Debugger::ContinueExecution(): Already paused");
				return ResultCode::InPause;
			}

			Debug(L"Debugger::ContinueExecution(): Force pause on next node");
			// Forcibly break on the next call
			forceBreakpoint_ = true;
			maxBreakDepth_ = 0x7fffffff;
			// This is not a "continue" message, it just sets the breakpoint flags,
			// so we don't go through the continue code here
			return ResultCode::Success;
		}

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

		Debug(L"Debugger::ContinueExecution(%d): Continuing", action);
		isPaused_ = false;
		breakpointCv_.notify_one();

		return ResultCode::Success;
	}

	void Debugger::ClearAllBreakpoints()
	{
		globalBreakpoints_ = 0;
		breakpoints_.clear();
		forceBreakpoint_ = false;
		maxBreakDepth_ = 0;
	}

	void Debugger::FinishedSingleStep()
	{
		// Called when we're finished single stepping and want to cancel single-step triggers
		forceBreakpoint_ = false;
		maxBreakDepth_ = 0;
	}

	bool Debugger::ShouldTriggerBreakpoint(uint64_t bpNodeId, BreakpointType bpType, GlobalBreakpointType globalBpType)
	{
		// Check if there is a breakpoint on this node ID
		auto it = breakpoints_.find(bpNodeId);
		if (it != breakpoints_.end()
			&& (it->second.type & bpType)) {
			return true;
		}

		// Check if there is a global breakpoint for this frame type
		if (globalBreakpoints_ & globalBpType) {
			return true;
		}

		// Check if we're single stepping
		if (forceBreakpoint_ && callStack_.size() <= maxBreakDepth_) {
			return true;
		}

		return false;
	}

	void Debugger::ConditionalBreakpointInServerThread(uint64_t bpNodeId, BreakpointType bpType, GlobalBreakpointType globalBpType)
	{
		if (ShouldTriggerBreakpoint(bpNodeId, bpType, globalBpType)) {
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
	}

	void Debugger::GlobalBreakpointInServerThread(GlobalBreakpointReason reason)
	{
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
		PushFrame({ BreakpointReason::NodeIsValid, node, nullptr, 0, &tuple->Data, nullptr });

		ConditionalBreakpointInServerThread(
			MakeNodeBreakpointId(node->Id),
			BreakOnValid,
			GlobalBreakOnValid);
	}

	void Debugger::IsValidPostHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, bool succeeded)
	{
		PopFrame({ BreakpointReason::NodeIsValid, node, nullptr, 0, &tuple->Data, nullptr });
	}

	void Debugger::PushDownPreHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted)
	{
		auto reason = deleted ? BreakpointReason::NodePushDownTupleDelete : BreakpointReason::NodePushDownTuple;
		PushFrame({ reason, node, nullptr, 0, &tuple->Data, nullptr });

		ConditionalBreakpointInServerThread(
			MakeNodeBreakpointId(node->Id),
			BreakOnPushDown,
			GlobalBreakOnPushDown);
	}

	void Debugger::PushDownPostHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted)
	{
		auto reason = deleted ? BreakpointReason::NodePushDownTupleDelete : BreakpointReason::NodePushDownTuple;
		PopFrame({ reason, node, nullptr, 0, &tuple->Data, nullptr });
	}

	void Debugger::InsertPreHook(Node * node, TuplePtrLL * tuple, bool deleted)
	{
		auto reason = deleted ? BreakpointReason::NodeDeleteTuple : BreakpointReason::NodeInsertTuple;
		PushFrame({ reason, node, nullptr, 0, nullptr, tuple });

		ConditionalBreakpointInServerThread(
			MakeNodeBreakpointId(node->Id),
			BreakOnInsert,
			GlobalBreakOnInsert);
	}

	void Debugger::InsertPostHook(Node * node, TuplePtrLL * tuple, bool deleted)
	{
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

	RuleActionMapping const & Debugger::FindActionMapping(RuleActionNode * action)
	{
		auto mapping = ruleActionMappings_.find(action);
		if (mapping == ruleActionMappings_.end()) {
			Fail(L"Could not find action mapping for rule action");
		}

		return mapping->second;
	}

	void Debugger::RuleActionPreHook(RuleActionNode * action)
	{
		auto const & mapping = FindActionMapping(action);
		BreakpointReason reason;
		BreakpointType bpType;
		GlobalBreakpointType globalBpType;
		uint64_t breakpointId;
		if (mapping.rule != nullptr) {
			reason = BreakpointReason::RuleActionCall;
			bpType = BreakpointType::BreakOnRuleAction;
			globalBpType = GlobalBreakpointType::GlobalBreakOnRuleAction;
			breakpointId = MakeRuleActionBreakpointId(mapping.rule->Id, mapping.actionIndex);
		} else if (mapping.isInit) {
			reason = BreakpointReason::GoalInitCall;
			bpType = BreakpointType::BreakOnInitCall;
			globalBpType = GlobalBreakpointType::GlobalBreakOnInitCall;
			breakpointId = MakeGoalInitBreakpointId(mapping.goal->Id, mapping.actionIndex);
		} else {
			reason = BreakpointReason::GoalExitCall;
			bpType = BreakpointType::BreakOnExitCall;
			globalBpType = GlobalBreakpointType::GlobalBreakOnExitCall;
			breakpointId = MakeGoalExitBreakpointId(mapping.goal->Id, mapping.actionIndex);
		}

		PushFrame({ reason, mapping.rule, mapping.goal, mapping.actionIndex, nullptr, nullptr });

		ConditionalBreakpointInServerThread(breakpointId, bpType, globalBpType);
	}

	void Debugger::RuleActionPostHook(RuleActionNode * action)
	{
		auto const & mapping = FindActionMapping(action);
		BreakpointReason reason;
		if (mapping.rule != nullptr) {
			reason = BreakpointReason::RuleActionCall;
		}
		else if (mapping.isInit) {
			reason = BreakpointReason::GoalInitCall;
		}
		else {
			reason = BreakpointReason::GoalExitCall;
		}

		PopFrame({ reason, mapping.rule, mapping.goal, mapping.actionIndex, nullptr, nullptr });
	}
}
