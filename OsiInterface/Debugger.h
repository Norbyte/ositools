#pragma once

#include <cstdint>
#include <concurrent_queue.h>
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
		BreakOnRuleAction = 1 << 3,
		BreakOnInitCall = 1 << 4,
		BreakOnExitCall = 1 << 5,
		BreakOnDelete = 1 << 6,
		BreakOnFailedQuery = 1 << 7,
		BreakpointTypeAll = BreakOnValid | BreakOnPushDown | BreakOnInsert
			| BreakOnRuleAction | BreakOnInitCall | BreakOnExitCall | BreakOnDelete
			| BreakOnFailedQuery
	};

	enum GlobalBreakpointType
	{
		GlobalBreakOnStoryLoaded = 1 << 0,
		GlobalBreakOnValid = 1 << 1,
		GlobalBreakOnPushDown = 1 << 2,
		GlobalBreakOnInsert = 1 << 3,
		GlobalBreakOnRuleAction = 1 << 4,
		GlobalBreakOnInitCall = 1 << 5,
		GlobalBreakOnExitCall = 1 << 6,
		GlobalBreakOnGameInit = 1 << 7,
		GlobalBreakOnGameExit = 1 << 8,
		GlobalBreakOnDelete = 1 << 9,
		GlobalBreakOnFailedQuery = 1 << 10,
		GlobalBreakpointTypeAll = GlobalBreakOnStoryLoaded | GlobalBreakOnValid | GlobalBreakOnPushDown 
			| GlobalBreakOnInsert | GlobalBreakOnRuleAction | GlobalBreakOnInitCall | GlobalBreakOnExitCall
			| GlobalBreakOnGameInit | GlobalBreakOnGameExit | GlobalBreakOnDelete | GlobalBreakOnFailedQuery
	};

	enum ContinueFlag
	{
		ContinueSkipRulePushdown = 1 << 0,
		ContinueSkipDbPropagation = 1 << 1,
		ContinueFlagAll = ContinueSkipRulePushdown | ContinueSkipDbPropagation
	};

	// Mapping of a rule action to its call site (rule then part, goal init/exit)
	struct RuleActionMapping
	{
		// Mapped action
		RuleActionNode * action;
		// Calling rule node
		Node * rule;
		// Calling goal
		Goal * goal;
		// Is this an INIT section call?
		bool isInit;
		// Index of action in section
		uint32_t actionIndex;
	};

	struct OsirisGlobals;

	class Debugger
	{
	public:
		Debugger(OsirisGlobals const & globals, DebugMessageHandler & messageHandler);
		~Debugger();

		void StoryLoaded();
		void MergeStarted();
		void MergeFinished();

		inline bool IsInitialized() const
		{
			return isInitialized_;
		}

		inline bool IsPaused() const
		{
			return isPaused_;
		}

		void BeginUpdatingNodeBreakpoints();
		ResultCode AddBreakpoint(uint32_t nodeId, uint32_t goalId, bool isInit, int32_t actionIndex, BreakpointType type);
		void FinishUpdatingNodeBreakpoints();

		ResultCode GetDatabaseContents(uint32_t databaseId);
		ResultCode SetGlobalBreakpoints(GlobalBreakpointType type);
		ResultCode ContinueExecution(DbgContinue_Action action, uint32_t breakpointMask, uint32_t flags);
		void ClearAllBreakpoints();
		void SyncStory();

		void GameInitHook();
		void DeleteAllDataHook();
		void RuleActionPreHook(RuleActionNode * action);
		void RuleActionPostHook(RuleActionNode * action);

	private:
		struct Breakpoint
		{
			uint32_t nodeId;
			uint32_t goalId;
			bool isInit;
			uint32_t actionIndex;
			BreakpointType type;
		};

		enum BreakpointItemType : uint8_t
		{
			BP_Node = 0,
			BP_RuleAction = 1,
			BP_GoalInit = 2,
			BP_GoalExit = 3
		};

		static uint64_t MakeNodeBreakpointId(uint32_t nodeId);
		static uint64_t MakeRuleActionBreakpointId(uint32_t nodeId, uint32_t actionIndex);
		static uint64_t MakeGoalInitBreakpointId(uint32_t goalId, uint32_t actionIndex);
		static uint64_t MakeGoalExitBreakpointId(uint32_t goalId, uint32_t actionIndex);

		OsirisGlobals const & globals_;
		DebugMessageHandler & messageHandler_;
		std::vector<CallStackFrame> callStack_;
		// Mapping of a rule action to its call site (rule then part, goal init/exit)
		std::unordered_map<RuleActionNode *, RuleActionMapping> ruleActionMappings_;
		// Did the engine call COsiris::InitGame() in this session?
		bool isInitialized_{ false };

		std::mutex breakpointMutex_;
		std::condition_variable breakpointCv_;

		uint32_t globalBreakpoints_;
		// Breakpoints that are currently active
		std::unique_ptr<std::unordered_map<uint64_t, Breakpoint>> breakpoints_;
		// Breakpoints that are being applied via the debugger protocol
		std::unique_ptr<std::unordered_map<uint64_t, Breakpoint>> pendingBreakpoints_;
		// Is debugging disabled?
		// (i.e. we don't handle any continue requests, and don't stop on breakpoints)
		bool debuggingDisabled_{ false };
		bool isPaused_{ false };
		// Forcibly triggers a breakpoint if all breakpoint conditions are met.
		bool forceBreakpoint_{ false };
		// Events that will trigger a forced breakpoint.
		// This only filters forced BPs, not traditional breakpoints.
		uint32_t forceBreakpointMask_{ 0 };
		// Additional filter flags on forced breakpoints.
		uint32_t forceBreakpointFlags_{ 0 };
		// Call stack depth at which we'll trigger a breakpoint
		// (used for step over/into/out)
		uint32_t maxBreakDepth_{ 0 };
		// Do we have any information about the result of the last IsValid query?
		bool hasLastQueryInfo_{ false };
		// Stack depth of last recorded query info
		uint32_t lastQueryDepth_{ 0 };
		// Did the last query succeed?
		bool lastQuerySucceeded_;
		// Results of last div query
		std::vector<OsiArgumentValue> lastQueryResults_;

		// Actions that we'll perform in the server thread instead of the messaging runtime thread.
		// This is needed to make sure that certain operations (eg. breakpoint update) execute in a thread-safe way.
		Concurrency::concurrent_queue<std::function<void ()>> pendingActions_;

		void DetectGameVersion();
		void ServerThreadReentry();

		void FinishedSingleStep();
		bool ForcedBreakpointConditionsSatisfied(Node * bpNode, BreakpointType bpType);
		bool ShouldTriggerBreakpoint(Node * bpNode, uint64_t bpNodeId, BreakpointType bpType, GlobalBreakpointType globalBpType);
		void ConditionalBreakpointInServerThread(Node * bpNode, uint64_t bpNodeId, BreakpointType bpType, GlobalBreakpointType globalBpType);
		void BreakpointInServerThread();
		void GlobalBreakpointInServerThread(GlobalBreakpointReason reason);

		void AddRuleActionMappings(Node * node, Goal * goal, bool isInit, RuleActionList * actions);
		void UpdateRuleActionMappings();
		RuleActionMapping const * FindActionMapping(RuleActionNode * action);

		void IsValidPreHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter);
		void IsValidPostHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, bool succeeded);
		void PushDownPreHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted);
		void PushDownPostHook(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint entry, bool deleted);
		void InsertPreHook(Node * node, TuplePtrLL * tuple, bool deleted);
		void InsertPostHook(Node * node, TuplePtrLL * tuple, bool deleted);
		void CallQueryPreHook(Node * node, OsiArgumentDesc * args);
		void CallQueryPostHook(Node * node, OsiArgumentDesc * args, bool succeeded);

		void PushFrame(CallStackFrame const & frame);
		void PopFrame(CallStackFrame const & frame);
	};
}
