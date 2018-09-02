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

		isPaused_ = false;

		messageHandler_.SetDebugger(this);

		using namespace std::placeholders;
		gNodeVMTWrappers->IsValidPreHook = std::bind(&Debugger::IsValidPreHook, this, _1, _2, _3, _4);
		gNodeVMTWrappers->IsValidPostHook = std::bind(&Debugger::IsValidPostHook, this, _1, _2, _3, _4, _5);
		gNodeVMTWrappers->PushDownPreHook = std::bind(&Debugger::PushDownPreHook, this, _1, _2, _3, _4, _5, _6);
		gNodeVMTWrappers->PushDownPostHook = std::bind(&Debugger::PushDownPostHook, this, _1, _2, _3, _4, _5, _6);
		gNodeVMTWrappers->InsertPreHook = std::bind(&Debugger::InsertPreHook, this, _1, _2, _3, _4);
		gNodeVMTWrappers->InsertPostHook = std::bind(&Debugger::InsertPostHook, this, _1, _2, _3, _4);
		Debug(L"Debugger::Debugger(): Attached to story");
	}

	Debugger::~Debugger()
	{
		Debug(L"Debugger::~Debugger(): Shutting down debugger");
		messageHandler_.SendDebugSessionEnded();
		messageHandler_.SetDebugger(nullptr);

		if (gNodeVMTWrappers) {
			gNodeVMTWrappers->IsValidPreHook = std::function<void(NodeType, Node *, VirtTupleLL *, AdapterRef *)>();
			gNodeVMTWrappers->IsValidPostHook = std::function<void(NodeType, Node *, VirtTupleLL *, AdapterRef *, bool)>();
			gNodeVMTWrappers->PushDownPreHook = std::function<void(NodeType, Node *, VirtTupleLL *, AdapterRef *, EntryPoint, bool)>();
			gNodeVMTWrappers->PushDownPostHook = std::function<void(NodeType, Node *, VirtTupleLL *, AdapterRef *, EntryPoint, bool)>();
			gNodeVMTWrappers->InsertPreHook = std::function<void(NodeType, Node *, TuplePtrLL *, bool)>();
			gNodeVMTWrappers->InsertPostHook = std::function<void(NodeType, Node *, TuplePtrLL *, bool)>();
		}
	}

	void Debugger::StoryLoaded()
	{
		messageHandler_.SendStoryLoaded();
		if (globalBreakpoints_ & GlobalBreakpointType::GlobalBreakOnStoryLoaded) {
			GlobalBreakpointInServerThread(GlobalBreakpointReason::StoryLoaded);
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

	ResultCode Debugger::ContinueExecution()
	{
		std::unique_lock<std::mutex> lk(breakpointMutex_);
		if (!isPaused_) {
			Debug(L"Debugger::ContinueExecution(): Not paused");
			return ResultCode::NotInPause;
		}

		Debug(L"Debugger::ContinueExecution(): Continuing");
		isPaused_ = false;
		breakpointCv_.notify_one();

		return ResultCode::Success;
	}

	void Debugger::ClearAllBreakpoints()
	{
		globalBreakpoints_ = 0;
		breakpoints_.clear();
	}

	void Debugger::BreakpointInServerThread()
	{
		if (callStack_.empty()) {
			Fail(L"Tried to trigger breakpoint with empty callstack");
		}

		auto const & lastFrame = *callStack_.rbegin();
		Debug(L"Debugger::BreakpointInServerThread(): Node %d, type %d", lastFrame.node->Id, lastFrame.frameType);
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

	void Debugger::PushFrame(NodeType type, Node * node, TupleLL * tuple, TuplePtrLL * ptrTuple, BreakpointReason reason)
	{
		CallStackFrame frame;
		frame.type = type;
		frame.node = node;
		frame.tupleLL = tuple;
		frame.tuplePtrLL = ptrTuple;
		frame.frameType = reason;
		callStack_.push_back(frame);
	}

	void Debugger::PopFrame(NodeType type, Node * node, TupleLL * tuple, TuplePtrLL * ptrTuple, BreakpointReason reason)
	{
		if (callStack_.empty()) {
			Fail(L"Tried to remove frame from empty callstack");
		}

		auto const & lastFrame = *callStack_.rbegin();
		if (lastFrame.type != type
			|| lastFrame.node != node
			|| lastFrame.tupleLL != tuple
			|| lastFrame.tuplePtrLL != ptrTuple
			|| lastFrame.frameType != reason) {
			Fail(L"Call stack frame mismatch");
		}

		callStack_.pop_back();
	}

	void Debugger::IsValidPreHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter)
	{
		PushFrame(type, node, &tuple->Data, nullptr, BreakpointReason::NodeIsValid);

		auto it = breakpoints_.find(node->Id);
		if ((it != breakpoints_.end() && (it->second.type & BreakOnValid))
			|| (globalBreakpoints_ & GlobalBreakOnValid)) {
			BreakpointInServerThread();
		}
	}

	void Debugger::IsValidPostHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter, bool succeeded)
	{
		PopFrame(type, node, &tuple->Data, nullptr, BreakpointReason::NodeIsValid);
	}

	void Debugger::PushDownPreHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted)
	{
		PushFrame(type, node, &tuple->Data, nullptr, deleted ? BreakpointReason::NodePushDownTupleDelete : BreakpointReason::NodePushDownTuple);

		auto it = breakpoints_.find(node->Id);
		if ((it != breakpoints_.end() && (it->second.type & BreakOnPushDown))
			|| (globalBreakpoints_ & GlobalBreakOnPushDown)) {
			BreakpointInServerThread();
		}
	}

	void Debugger::PushDownPostHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted)
	{
		PopFrame(type, node, &tuple->Data, nullptr, deleted ? BreakpointReason::NodePushDownTupleDelete : BreakpointReason::NodePushDownTuple);
	}

	void Debugger::InsertPreHook(NodeType type, Node * node, TuplePtrLL * tuple, bool deleted)
	{
		PushFrame(type, node, nullptr, tuple, deleted ? BreakpointReason::NodeDeleteTuple : BreakpointReason::NodeInsertTuple);

		auto it = breakpoints_.find(node->Id);
		if ((it != breakpoints_.end() && (it->second.type & BreakOnInsert))
			|| (globalBreakpoints_ & GlobalBreakOnInsert)) {
			BreakpointInServerThread();
		}
	}

	void Debugger::InsertPostHook(NodeType type, Node * node, TuplePtrLL * tuple, bool deleted)
	{
		PopFrame(type, node, nullptr, tuple, deleted ? BreakpointReason::NodeDeleteTuple : BreakpointReason::NodeInsertTuple);
	}

	void Debugger::RuleActionPreHook(RuleActionNode * action)
	{
	}

	void Debugger::RuleActionPostHook(RuleActionNode * action)
	{
	}
}
