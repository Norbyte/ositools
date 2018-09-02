#pragma once

#include <cstdint>
#include "osidebug.pb.h"
#include "DivInterface.h"
#include "DebugMessages.h"

namespace osidbg
{
	enum BreakpointType
	{
		BreakOnValid = 1 << 0,
		BreakOnPushDown = 1 << 1,
		BreakOnInsert = 1 << 2,
		BreakpointTypeAll = BreakOnValid | BreakOnPushDown | BreakOnInsert
	};

	enum GlobalBreakpointType
	{
		GlobalBreakOnStoryLoaded = 1 << 0,
		GlobalBreakOnValid = 1 << 1,
		GlobalBreakOnPushDown = 1 << 2,
		GlobalBreakOnInsert = 1 << 3,
		GlobalBreakpointTypeAll = GlobalBreakOnStoryLoaded | GlobalBreakOnValid | GlobalBreakOnPushDown | GlobalBreakOnInsert
	};

	struct OsirisGlobals;

	class Debugger
	{
	public:
		Debugger(OsirisGlobals const & globals, DebugMessageHandler & messageHandler);
		~Debugger();

		void StoryLoaded();

		inline bool IsPaused() const
		{
			return isPaused_;
		}

		ResultCode SetGlobalBreakpoints(GlobalBreakpointType type);
		ResultCode SetBreakpoint(uint32_t nodeId, BreakpointType type);
		ResultCode ContinueExecution();
		void ClearAllBreakpoints();

		void RuleActionPreHook(RuleActionNode * action);
		void RuleActionPostHook(RuleActionNode * action);

	private:
		struct Breakpoint
		{
			uint32_t nodeId;
			BreakpointType type;
		};

		OsirisGlobals const & globals_;
		DebugMessageHandler & messageHandler_;
		uint32_t globalBreakpoints_;
		std::unordered_map<uint32_t, Breakpoint> breakpoints_;
		bool isPaused_{ false };
		std::mutex breakpointMutex_;
		std::condition_variable breakpointCv_;
		std::vector<CallStackFrame> callStack_;

		void BreakpointInServerThread();
		void GlobalBreakpointInServerThread(GlobalBreakpointReason reason);

		void IsValidPreHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter);
		void IsValidPostHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter, bool succeeded);
		void PushDownPreHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted);
		void PushDownPostHook(NodeType type, Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted);
		void InsertPreHook(NodeType type, Node * node, TuplePtrLL * tuple, bool deleted);
		void InsertPostHook(NodeType type, Node * node, TuplePtrLL * tuple, bool deleted);

		void PushFrame(NodeType type, Node * node, TupleLL * tuple, TuplePtrLL * ptrTuple, BreakpointReason reason);
		void PopFrame(NodeType type, Node * node, TupleLL * tuple, TuplePtrLL * ptrTuple, BreakpointReason reason);
	};
}
