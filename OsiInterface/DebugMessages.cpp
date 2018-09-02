#include "stdafx.h"
#include "DebugMessages.h"
#include "NodeHooks.h"
#include "OsirisProxy.h"
#include <sstream>

namespace osidbg
{
	DebugMessageHandler::DebugMessageHandler(DebugInterface & intf)
		: intf_(intf)
	{
		intf_.SetMessageHandler(
			std::bind(&DebugMessageHandler::HandleMessage, this, std::placeholders::_1),
			std::bind(&DebugMessageHandler::HandleDisconnect, this));
	}

	void MakeMsgColumn(MsgTypedValue & msgTv, TypedValue const & tv)
	{
		msgTv.set_type_id(tv.TypeId);
		auto const & val = tv.Value.Val;

		switch ((ValueType)tv.TypeId) {
		case ValueType::None: break;
		case ValueType::Integer: msgTv.set_intval(val.Int32); break;
		case ValueType::Integer64: msgTv.set_intval(val.Int64); break;
		case ValueType::Real: msgTv.set_floatval(val.Float); break;
		default: *msgTv.mutable_stringval() = val.String; break;
		}
	}

	void MakeMsgTuple(MsgTuple & msgTuple, TupleLL const & tuple)
	{
		auto head = tuple.Items.Head;
		auto col = head->Next;
		while (col != head) {
			auto column = msgTuple.add_column();
			MakeMsgColumn(*column, col->Item.Value);
			col = col->Next;
		}
	}

	void MakeMsgTuple(MsgTuple & msgTuple, TuplePtrLL const & tuple)
	{
		auto head = tuple.Items.Head;
		auto col = head->Next;
		while (col != head) {
			auto column = msgTuple.add_column();
			MakeMsgColumn(*column, *col->Item);
			col = col->Next;
		}
	}

	void DebugDumpTV(std::wstringstream & ss, TypedValue const & tv)
	{
		switch ((ValueType)tv.TypeId) {
		case ValueType::Integer:
			ss << tv.Value.Val.Int32;
			break;

		case ValueType::Integer64:
			ss << tv.Value.Val.Int64;
			break;

		case ValueType::Real:
			ss << tv.Value.Val.Float;
			break;

		case ValueType::None:
		case ValueType::Undefined:
			ss << "(None)";
			break;

		default:
			if (tv.Value.Val.String) {
				ss << '"' << tv.Value.Val.String << '"';
			}
			else {
				ss << "(Null)";
			}
			break;
		}
	}

	void DebugDumpTuple(std::wstringstream & ss, TupleLL const & tuple)
	{
		auto head = tuple.Items.Head;
		auto cur = head->Next;
		while (cur != head) {
			DebugDumpTV(ss, cur->Item.Value);
			ss << ", ";
			cur = cur->Next;
		}
	}

	void DebugDumpTuple(std::wstringstream & ss, TuplePtrLL const & tuple)
	{
		auto head = tuple.Items.Head;
		auto cur = head->Next;
		while (cur != head) {
			DebugDumpTV(ss, *cur->Item);
			ss << ", ";
			cur = cur->Next;
		}
	}

	void MakeMsgCallStack(BkBreakpointTriggered & msg, std::vector<CallStackFrame> const & callStack)
	{
		for (auto const & frame : callStack) {
			auto msgFrame = msg.add_call_stack();
			msgFrame->set_node_id(frame.node->Id);
			msgFrame->set_type((MsgFrame_FrameType)frame.frameType);
			auto tuple = msgFrame->mutable_tuple();
			if (frame.tupleLL != nullptr) {
				MakeMsgTuple(*tuple, *frame.tupleLL);
			}
			else {
				MakeMsgTuple(*tuple, *frame.tuplePtrLL);
			}
		}
	}

	void DebugMessageHandler::SendBreakpointTriggered(std::vector<CallStackFrame> const & callStack)
	{
		auto const & lastFrame = *callStack.rbegin();
		BackendToDebugger msg;
		auto trigger = msg.mutable_breakpointtriggered();
		MakeMsgCallStack(*trigger, callStack);
		Send(msg);

		std::wstringstream tup;
		if (lastFrame.tupleLL != nullptr) {
			DebugDumpTuple(tup, *lastFrame.tupleLL);
		}
		else {
			DebugDumpTuple(tup, *lastFrame.tuplePtrLL);
		}
		Debug(L" <-- BkBreakpointTriggered(%d, %d, (%s))", lastFrame.node->Id, lastFrame.frameType, tup.str().c_str());
	}

	void DebugMessageHandler::SendGlobalBreakpointTriggered(GlobalBreakpointReason reason)
	{
		BackendToDebugger msg;
		auto trigger = msg.mutable_globalbreakpointtriggered();
		trigger->set_reason((BkGlobalBreakpointTriggered_Reason)reason);
		Send(msg);

		Debug(L" <-- BkGlobalBreakpointTriggered(%d)", reason);
	}

	void DebugMessageHandler::SendStoryLoaded()
	{
		BackendToDebugger msg;
		auto storyLoaded = msg.mutable_storyloaded();
		Send(msg);
		Debug(L" <-- BkStoryLoaded()");
	}

	void DebugMessageHandler::SendDebugSessionEnded()
	{
		BackendToDebugger msg;
		auto sessionEnded = msg.mutable_debugsessionended();
		Send(msg);
		Debug(L" <-- BkDebugSessionEnded()");
	}

	void DebugMessageHandler::SetDebugger(Debugger * debugger)
	{
		debugger_ = debugger;
	}

	void DebugMessageHandler::HandleIdentify(uint32_t seq, DbgIdentifyRequest const & req)
	{
		Debug(L" --> DbgIdentifyRequest()");
		SendVersionInfo(seq);
	}

	void DebugMessageHandler::HandleSetGlobalBreakpoints(uint32_t seq, DbgSetGlobalBreakpoints const & req)
	{
		Debug(L" --> DbgSetGlobalBreakpoints(%d)", req.breakpoint_mask());

		ResultCode rc;
		if (!debugger_) {
			Debug(L"SetGlobalBreakpoints: Not attached to story debugger!");
			rc = ResultCode::NoDebuggee;
		}
		else
		{
			rc = debugger_->SetGlobalBreakpoints((GlobalBreakpointType)req.breakpoint_mask());
		}

		SendResult(seq, rc);
	}

	void DebugMessageHandler::HandleSetBreakpoints(uint32_t seq, DbgSetBreakpoints const & req)
	{
		Debug(L" --> DbgSetBreakpoints()");

		ResultCode rc;
		if (!debugger_) {
			Debug(L"SetBreakpoint: Not attached to story debugger!");
			rc = ResultCode::NoDebuggee;
		}
		else
		{
			for (auto const & bp : req.breakpoint()) {
				Debug(L"SetBreakpoint(%d, %d)", bp.node_id(), bp.breakpoint_mask());
				rc = debugger_->SetBreakpoint(
					bp.node_id(),
					(BreakpointType)bp.breakpoint_mask()
				);

				if (rc != ResultCode::Success) {
					break;
				}
			}
		}

		SendResult(seq, rc);
	}

	void DebugMessageHandler::HandleContinue(uint32_t seq, DbgContinue const & req)
	{
		Debug(L" --> DbgContinue()");

		ResultCode rc;
		if (!debugger_) {
			Debug(L"Continue: Not attached to story debugger!");
			rc = ResultCode::NoDebuggee;
		}
		else
		{
			rc = debugger_->ContinueExecution();
		}

		SendResult(seq, rc);
	}

	bool DebugMessageHandler::HandleMessage(DebuggerToBackend const * msg)
	{
		uint32_t seq = msg->seq_no();
		if (seq != inboundSeq_) {
			Debug(L"Incorrect sequence number in message; expected %d, got %d", inboundSeq_, seq);
			return false;
		}

		inboundSeq_++;

		switch (msg->msg_case()) {
		case DebuggerToBackend::kIdentify:
			HandleIdentify(seq, msg->identify());
			break;

		case DebuggerToBackend::kSetGlobalBreakpoints:
			HandleSetGlobalBreakpoints(seq, msg->setglobalbreakpoints());
			break;

		case DebuggerToBackend::kSetBreakpoints:
			HandleSetBreakpoints(seq, msg->setbreakpoints());
			break;

		case DebuggerToBackend::kContinue:
			HandleContinue(seq, msg->continue_());
			break;

			//case DebuggerToBackend::kGetDatabaseContents:
			//	HandleContinue(seq, msg->get_database_contents());
			//	break;

		default:
			Debug(L"Unknown message in stream.");
			return false;
		}

		return true;
	}

	void DebugMessageHandler::HandleDisconnect()
	{
		Debug(L"Disconnected from debugger frontend");
		if (debugger_) {
			debugger_->ClearAllBreakpoints();
			if (debugger_->IsPaused()) {
				debugger_->ContinueExecution();
			}
		}

		outboundSeq_ = 1;
		inboundSeq_ = 1;
	}

	void DebugMessageHandler::Send(BackendToDebugger & msg)
	{
		msg.set_seq_no(outboundSeq_++);
		intf_.Send(msg);
	}

	void DebugMessageHandler::SendResult(uint32_t seq, ResultCode code)
	{
		BackendToDebugger msg;
		msg.set_reply_seq_no(seq);
		auto results = msg.mutable_results();
		results->set_status_code((BkResult::StatusCode)code);
		Send(msg);
		Debug(L" <-- BkResult(%d)", code);
	}

	void DebugMessageHandler::SendVersionInfo(uint32_t seq)
	{
		BackendToDebugger msg;
		auto version = msg.mutable_versioninfo();
		version->set_protocol_version(1);
		version->set_story_loaded(debugger_ != nullptr);
		version->set_story_version("");
		Send(msg);
		Debug(L" <-- BkVersionInfoResponse()");
	}
}
