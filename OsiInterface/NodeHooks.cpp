#include "stdafx.h"
#include "NodeHooks.h"
#include <sstream>
#include <memory>

namespace osidbg
{
	template <typename T>
	class ROWriteAnchor
	{
	public:
		ROWriteAnchor(T * region)
			: region_(region)
		{
			BOOL succeeded = VirtualProtect((LPVOID)region_, sizeof(T), PAGE_READWRITE, &oldProtect_);
			if (!succeeded) Fail(L"VirtualProtect() on region failed");
		}
		
		~ROWriteAnchor()
		{
			BOOL succeeded = VirtualProtect((LPVOID)region_, sizeof(T), oldProtect_, &oldProtect_);
			if (!succeeded) Fail(L"VirtualProtect() on region failed");
		}

	private:
		T * region_;
		DWORD oldProtect_;
	};

	NodeVMTWrapper::NodeVMTWrapper(NodeVMT * vmt, NodeWrapOptions & options)
		: vmt_(vmt), options_(options)
	{
		originalVmt_ = *vmt_;

		ROWriteAnchor<NodeVMT> _(vmt_);
		if (options_.WrapIsValid) {
			vmt_->IsValid = &s_WrappedIsValid;
		}

		if (options_.WrapPushDownTuple) {
			vmt_->PushDownTuple = &s_WrappedPushDownTuple;
		}

		if (options_.WrapPushDownTupleDelete) {
			vmt_->PushDownTupleDelete = &s_WrappedPushDownTupleDelete;
		}

		if (options_.WrapInsertTuple) {
			vmt_->InsertTuple = &s_WrappedInsertTuple;
		}

		if (options_.WrapDeleteTuple) {
			vmt_->DeleteTuple = &s_WrappedDeleteTuple;
		}
	}

	NodeVMTWrapper::~NodeVMTWrapper()
	{
		ROWriteAnchor<NodeVMT> _(vmt_);
		*vmt_ = originalVmt_;
	}

	void DebugDumpTuple(std::wstringstream & ss, TupleLL const & tuple);

	bool NodeVMTWrapper::WrappedIsValid(Node * node, VirtTupleLL * tuple, AdapterRef * adapter)
	{
		return originalVmt_.IsValid(node, tuple, adapter);
	}

	void NodeVMTWrapper::WrappedPushDownTuple(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint which)
	{
		originalVmt_.PushDownTuple(node, tuple, adapter, which);
	}

	void NodeVMTWrapper::WrappedPushDownTupleDelete(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint which)
	{
		originalVmt_.PushDownTupleDelete(node, tuple, adapter, which);
	}

	void NodeVMTWrapper::WrappedInsertTuple(Node * node, TuplePtrLL * tuple)
	{
		originalVmt_.InsertTuple(node, tuple);
	}

	void NodeVMTWrapper::WrappedDeleteTuple(Node * node, TuplePtrLL * tuple)
	{
		originalVmt_.DeleteTuple(node, tuple);
	}

	bool NodeVMTWrapper::s_WrappedIsValid(Node * node, VirtTupleLL * tuple, AdapterRef * adapter)
	{
		return gNodeVMTWrappers->WrappedIsValid(node, tuple, adapter);
	}

	void NodeVMTWrapper::s_WrappedPushDownTuple(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint which)
	{
		gNodeVMTWrappers->WrappedPushDownTuple(node, tuple, adapter, which);
	}

	void NodeVMTWrapper::s_WrappedPushDownTupleDelete(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint which)
	{
		gNodeVMTWrappers->WrappedPushDownTupleDelete(node, tuple, adapter, which);
	}

	void NodeVMTWrapper::s_WrappedInsertTuple(Node * node, TuplePtrLL * tuple)
	{
		gNodeVMTWrappers->WrappedInsertTuple(node, tuple);
	}

	void NodeVMTWrapper::s_WrappedDeleteTuple(Node * node, TuplePtrLL * tuple)
	{
		gNodeVMTWrappers->WrappedDeleteTuple(node, tuple);
	}

	NodeWrapOptions VMTWrapOptions[(unsigned)NodeType::Max + 1] = {
		{ true, false, false, true, true }, // Database
		{ true, false, false, true, true }, // Proc
		{ true, false, false, false, false }, // DivQuery
		{ true, true, true, false, false }, // And
		{ true, true, true, false, false }, // NotAnd
		{ true, true, true, false, false }, // RelOp
		{ true, true, true, false, false }, // Rule
		{ true, false, false, false, false }, // InternalQuery
		{ true, false, false, false, false } // UserQuery
	};

	NodeVMTWrappers::NodeVMTWrappers(NodeVMT ** vmts)
		: vmts_(vmts)
	{
		for (unsigned i = 0; i <= (unsigned)NodeType::Max; i++) {
			wrappers_[i] = std::make_unique<NodeVMTWrapper>(vmts_[i], VMTWrapOptions[i]);
			vmtToTypeMap_[vmts[i]] = (NodeType)i;
		}
	}

	NodeType NodeVMTWrappers::GetType(Node * node)
	{
		NodeVMT * vfptr = *reinterpret_cast<NodeVMT **>(node);
		auto typeIt = vmtToTypeMap_.find(vfptr);
		if (typeIt == vmtToTypeMap_.end()) Fail(L"Called virtual method on a node that could not be identified");
		return typeIt->second;
	}

	bool NodeVMTWrappers::WrappedIsValid(Node * node, VirtTupleLL * tuple, AdapterRef * adapter)
	{
		auto type = GetType(node);

		if (IsValidPreHook) {
			IsValidPreHook(node, tuple, adapter);
		}

		bool succeeded = wrappers_[(unsigned)type]->WrappedIsValid(node, tuple, adapter);

		//std::wstringstream ss;
		//DebugDumpTuple(ss, tuple->Data);

		//Debug(L"IsValid<%d> (%s)", type, ss.str().c_str());

		if (IsValidPostHook) {
			IsValidPostHook(node, tuple, adapter, succeeded);
		}

		return succeeded;
	}

	void NodeVMTWrappers::WrappedPushDownTuple(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint which)
	{
		auto type = GetType(node);

		//std::wstringstream ss;
		//DebugDumpTuple(ss, tuple->Data);

		//Debug(L"WrappedPushDownTuple<%d> (%s)", type, ss.str().c_str());

		if (PushDownPreHook) {
			PushDownPreHook(node, tuple, adapter, which, false);
		}

		wrappers_[(unsigned)type]->WrappedPushDownTuple(node, tuple, adapter, which);

		if (PushDownPostHook) {
			PushDownPostHook(node, tuple, adapter, which, false);
		}
	}

	void NodeVMTWrappers::WrappedPushDownTupleDelete(Node * node, VirtTupleLL * tuple, AdapterRef * adapter, EntryPoint which)
	{
		auto type = GetType(node);

		//std::wstringstream ss;
		//DebugDumpTuple(ss, tuple->Data);

		//Debug(L"WrappedPushDownTuple2<%d> (%s)", type, ss.str().c_str());

		if (PushDownPreHook) {
			PushDownPreHook(node, tuple, adapter, which, true);
		}

		wrappers_[(unsigned)type]->WrappedPushDownTupleDelete(node, tuple, adapter, which);

		if (PushDownPostHook) {
			PushDownPostHook(node, tuple, adapter, which, true);
		}
	}

	void NodeVMTWrappers::WrappedInsertTuple(Node * node, TuplePtrLL * tuple)
	{
		auto type = GetType(node);

		if (InsertPreHook) {
			InsertPreHook(node, tuple, false);
		}

		wrappers_[(unsigned)type]->WrappedInsertTuple(node, tuple);

		if (InsertPostHook) {
			InsertPostHook(node, tuple, false);
		}
	}

	void NodeVMTWrappers::WrappedDeleteTuple(Node * node, TuplePtrLL * tuple)
	{
		auto type = GetType(node);

		if (InsertPreHook) {
			InsertPreHook(node, tuple, true);
		}

		wrappers_[(unsigned)type]->WrappedDeleteTuple(node, tuple);

		if (InsertPostHook) {
			InsertPostHook(node, tuple, true);
		}
	}

	std::unique_ptr<NodeVMTWrappers> gNodeVMTWrappers;
}
