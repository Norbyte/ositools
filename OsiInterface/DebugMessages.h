#pragma once

#include <cstdint>
#include "osidebug.pb.h"
#include "DivInterface.h"
#include "DebugInterface.h"

namespace osidbg
{
	enum class GlobalBreakpointReason
	{
		StoryLoaded = 0,
		GameInit = 1
	};

	enum class BreakpointReason
	{
		NodeIsValid = 0,
		NodePushDownTuple = 1,
		NodeInsertTuple = 2,
		NodeDeleteTuple = 3,
		NodePushDownTupleDelete = 4,
		RuleActionCall = 5,
		GoalInitCall = 6,
		GoalExitCall = 7
	};

	enum class ResultCode
	{
		Success = 0,
		UnsupportedBreakpointType = 1,
		InvalidNodeId = 2,
		NotInPause = 3,
		NoDebuggee = 4,
		InvalidContinueAction = 5,
		InPause = 6,
		InvalidGoalId = 7
	};

	struct CallStackFrame
	{
		BreakpointReason frameType;
		Node * node;
		Goal * goal;
		uint32_t actionIndex;
		TupleLL * tupleLL;
		TuplePtrLL * tuplePtrLL;
	};

	class Debugger;

	class DebugMessageHandler
	{
	public:
		DebugMessageHandler(DebugInterface & intf);

		inline bool IsConnected() const
		{
			return intf_.IsConnected();
		}

		void SetDebugger(Debugger * debugger);
		void SendBreakpointTriggered(std::vector<CallStackFrame> const & callStack);
		void SendGlobalBreakpointTriggered(GlobalBreakpointReason reason);
		void SendStoryLoaded();
		void SendDebugSessionEnded();

	private:
		DebugInterface & intf_;
		Debugger * debugger_{ nullptr };
		uint32_t inboundSeq_{ 1 };
		uint32_t outboundSeq_{ 1 };

		bool HandleMessage(DebuggerToBackend const * msg);
		void HandleConnect();
		void HandleDisconnect();

		void HandleIdentify(uint32_t seq, DbgIdentifyRequest const & req);
		void HandleSetGlobalBreakpoints(uint32_t seq, DbgSetGlobalBreakpoints const & req);
		void HandleSetBreakpoints(uint32_t seq, DbgSetBreakpoints const & req);
		void HandleContinue(uint32_t seq, DbgContinue const & req);
		void HandleGetDatabaseContents(uint32_t seq, DbgGetDatabaseContents const & req);

		void Send(BackendToDebugger & msg);
		void SendVersionInfo(uint32_t seq);
		void SendResult(uint32_t seq, ResultCode code);
	};
}
