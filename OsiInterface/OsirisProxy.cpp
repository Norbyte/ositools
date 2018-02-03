#include "stdafx.h"
#include "OsirisProxy.h"
#include <detours.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace osi
{

std::unique_ptr<OsirisProxy> gOsirisProxy;

uint8_t * ResolveRealFunctionAddress(uint8_t * Address)
{
	// Resolve function pointer through relocations
	for (uint8_t * ptr = Address; ptr < Address + 64; ptr++)
	{
		// Look for the instruction "cmp qword ptr [rip+xxxxxx], 0"
		if (ptr[0] == 0x48 && ptr[1] == 0x83 && ptr[2] == 0x3d && ptr[6] == 0x00 &&
			// Look for the instruction "jmp xxxx"
			ptr[13] == 0xe9)
		{
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 14);
			return ptr + relOffset + 18;
		}
	}

	// Could not find any relocations
	return Address;
}

void OsirisProxy::FindOsirisGlobals(FARPROC CtorProc)
{
	Debug(L"OsirisProxy::FindOsirisGlobals:");
	uint8_t * Addr = ResolveRealFunctionAddress((uint8_t *)CtorProc);

	// Try to find pointers of Osiris globals
	const unsigned NumGlobals = 8;
	uint8_t * globals[NumGlobals];
	unsigned foundGlobals = 0;
	for (uint8_t * ptr = Addr; ptr < Addr + 0x300; ptr++)
	{
		// Look for the instruction "jmp short $+7"
		if (ptr[0] == 0xEB && ptr[1] == 0x07 &&
			// Look for the instruction "mov cs:xxx, rbx"
			ptr[2] == 0x48 && ptr[3] == 0x89 && ptr[4] == 0x1D)
		{
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 5);
			uint64_t osiPtr = (uint64_t)ptr + relOffset + 9;
			globals[foundGlobals++] = (uint8_t *)osiPtr;
			if (foundGlobals == NumGlobals) break;
		}
	}

	if (foundGlobals < NumGlobals)
	{
		Fail(L"Could not locate global Osiris variables");
	}

	OsirisVariables = (VariableDb **)globals[0];
	OsirisTypes = (TypeDb **)globals[1];
	OsirisFunctions = (TypeDb **)globals[2];
	OsirisObjects = (TypeDb **)globals[3];
	OsirisGoals = (void **)globals[4];
	OsirisAdapters = (AdapterDb **)globals[5];
	OsirisDatabases = (DatabaseDb **)globals[6];
	OsirisNodes = (NodeDb **)globals[7];

	Debug(L"\tOsirisVariables = %p", OsirisVariables);
	Debug(L"\tOsirisTypes = %p", OsirisTypes);
	Debug(L"\tOsirisFunctions = %p", OsirisFunctions);
	Debug(L"\tOsirisObjects = %p", OsirisObjects);
	Debug(L"\tOsirisGoals = %p", OsirisGoals);
	Debug(L"\tOsirisAdapters = %p", OsirisAdapters);
	Debug(L"\tOsirisDatabases = %p", OsirisDatabases);
	Debug(L"\tOsirisNodes = %p", OsirisNodes);
}

void OsirisProxy::FindDebugFlags(FARPROC SetOptionProc)
{
	uint8_t * Addr = ResolveRealFunctionAddress((uint8_t *)SetOptionProc);

	// Try to find pointer of global var DebugFlags
	for (uint8_t * ptr = Addr; ptr < Addr + 0x80; ptr++)
	{
		// Look for the instruction "mov ecx, cs:xxx"
		if (ptr[0] == 0x8B && ptr[1] == 0x0D &&
			// Look for the instruction "shr e*x, 14h"
			ptr[8] == 0xC1 && ptr[10] == 0x14)
		{
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 2);
			uint64_t dbgPtr = (uint64_t)ptr + relOffset + 6;
			GlobalDebugFlags = (DebugFlag *)dbgPtr;
			break;
		}
	}

	if (GlobalDebugFlags == nullptr) {
		Fail(L"Could not locate global variable DebugFlags");
	}

	Debug(L"OsirisProxy::FindDebugFlags: DebugFlags = %p", GlobalDebugFlags);
}

void OsirisProxy::Initialize()
{
	Debug(L"OsirisProxy::Initialize: Starting");
	OsirisModule = LoadLibrary(L"osiris_x64.dll");
	if (OsirisModule == NULL) {
		Fail(L"Could not load osiris_x64.dll");
	}

	FARPROC OsirisCtorProc = GetProcAddress(OsirisModule, "??0COsiris@@QEAA@XZ");
	if (OsirisCtorProc == NULL) {
		Fail(L"Could not locate COsiris::COsiris() in osiris_x64.dll");
	}

	FindOsirisGlobals(OsirisCtorProc);

	FARPROC RegisterDIVFunctionsProc = GetProcAddress(OsirisModule, "?RegisterDIVFunctions@COsiris@@QEAAXPEAUTOsirisInitFunction@@@Z");
	if (RegisterDIVFunctionsProc == NULL) {
		Fail(L"Could not locate RegisterDIVFunctions in osiris_x64.dll");
	}

	Debug(L"OsirisProxy::Initialize: Detouring RegisterDIVFunctions");
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	RegisterDivFunctions.Wrap(RegisterDIVFunctionsProc, &SRegisterDIVFunctionsWrapper);
	DetourTransactionCommit();

	FARPROC SetOptionProc = GetProcAddress(OsirisModule, "?SetOption@COsiris@@QEAAXI@Z");
	if (SetOptionProc == NULL) {
		Fail(L"Could not locate COsiris::SetOption in osiris_x64.dll");
	}

	FindDebugFlags(SetOptionProc);

	FARPROC OpenLogFileProc = GetProcAddress(OsirisModule, "?OpenLogFile@COsiris@@QEAA_NPEB_W0@Z");
	if (OpenLogFileProc == NULL) {
		Fail(L"Could not locate COsiris::OpenLogFile in osiris_x64.dll");
	}

	OsirisOpenLogFileProc = (COsirisOpenLogFileProc)OpenLogFileProc;

	FARPROC CloseLogFileProc = GetProcAddress(OsirisModule, "?CloseLogFile@COsiris@@QEAAXXZ");
	if (CloseLogFileProc == NULL) {
		Fail(L"Could not locate COsiris::CloseLogFile in osiris_x64.dll");
	}

	OsirisCloseLogFileProc = (COsirisCloseLogFileProc)CloseLogFileProc;

	FARPROC ReadHeaderProc = GetProcAddress(OsirisModule, "?_ReadHeader@COsiris@@IEAA_NAEAVCOsiSmartBuf@@AEAE111PEADAEAI@Z");
	if (ReadHeaderProc == NULL) {
		Fail(L"Could not locate COsiris::_ReadHeader in osiris_x64.dll");
	}

	OsirisReadHeaderProc = (COsirisReadHeaderProc)ReadHeaderProc;
}

void OsirisProxy::Shutdown()
{
	Debug(L"OsirisProxy::Shutdown: Starting");
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	RegisterDivFunctions.Unwrap();
	WrappedOsirisReadHeader.Unwrap();
	DetourTransactionCommit();

	FreeLibrary(OsirisModule);
}

void OsirisProxy::EnableHooks(bool Enabled)
{
	HooksEnabled = Enabled;
}

void OsirisProxy::SetupLogging(bool Enabled, DebugFlag LogLevel, std::wstring const & Path)
{
	DeleteFileW(Path.c_str());
	LoggingEnabled = Enabled;
	DesiredLogLevel = LogLevel;
	LogDirectory = Path;
}

void OsirisProxy::RestartLogging()
{
	DebugFlag NewFlags = (DebugFlag)((DesiredLogLevel & 0xffff0000) | (*GlobalDebugFlags & 0x0000ffff));

	if (LogFilename.empty())
	{
		BOOL created = CreateDirectoryW(LogDirectory.c_str(), NULL);
		if (created == FALSE)
		{
			DWORD lastError = GetLastError();
			if (lastError != ERROR_ALREADY_EXISTS)
			{
				std::wstringstream err;
				err << L"Could not create log directory '" << LogDirectory << "': Error " << lastError;
				Fail(err.str().c_str());
			}
		}

		auto now = std::chrono::system_clock::now();
		auto tt = std::chrono::system_clock::to_time_t(now);
		std::tm tm;
		gmtime_s(&tm, &tt);

		std::wstringstream ss;
		ss  << std::setfill(L'0') 
			<< (tm.tm_year + 1900) << "-" 
			<< std::setw(2) << (tm.tm_mon + 1) << "-"
			<< std::setw(2) << tm.tm_mday << " "
			<< std::setw(2) << tm.tm_hour << "-"
			<< std::setw(2) << tm.tm_min << "-"
			<< std::setw(2) << tm.tm_sec << ".log";
		LogFilename = LogDirectory + L"\\" + ss.str();

		Debug(L"OsirisProxy::RestartLogging: Starting debug logging.\r\n"
			"\tPath=%s\r\n"
			"\tOriginalDebugFlags=%08x\r\n"
			"\tDebugFlags=%08x", LogFilename.c_str(), *GlobalDebugFlags, NewFlags);
	}

	OsirisCloseLogFileProc(OsirisObject);
	*GlobalDebugFlags = NewFlags;
	OsirisOpenLogFileProc(OsirisObject, LogFilename.c_str(), L"ab+");
}

int OsirisProxy::RegisterDIVFunctionsWrapper(void * Osiris, DivFunctions * Functions)
{
	OsirisObject = Osiris;
	uint8_t * interfaceLoadPtr = nullptr;
	uint8_t * errorMessageFunc = ResolveRealFunctionAddress((uint8_t *)Functions->ErrorMessage);

	// Try to find ptr of gOsirisInterface
	OsirisInterface * osirisInterface = nullptr;
	for (uint8_t * ptr = errorMessageFunc; ptr < errorMessageFunc + 64; ptr++) {
		// Look for the instruction "mov rbx, cs:gOsirisInterface"
		if (ptr[0] == 0x48 && ptr[1] == 0x8B && ptr[2] == 0x1D && ptr[6] < 0x02) {
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 3);
			uint64_t osiPtr = (uint64_t)ptr + relOffset + 7;
			osirisInterface = *(OsirisInterface **)osiPtr;
			Manager = osirisInterface->Manager;
			break;
		}
	}

	if (Manager == nullptr) {
		Fail(L"Could not locate osi::OsirisInterface");
	}

	if (HooksEnabled) {
		CallOriginal = Functions->Call;
		QueryOriginal = Functions->Query;
		ErrorOriginal = Functions->ErrorMessage;
		AssertOriginal = Functions->Assert;

		Functions->Call = &SCallWrapper;
		Functions->Query = &SQueryWrapper;
		Functions->ErrorMessage = &SErrorWrapper;
		Functions->Assert = &SAssertWrapper;
	}

	if (LoggingEnabled) {
		if (!WrappedOsirisReadHeader.IsWrapped()) {
			Debug(L"OsirisProxy::RegisterDIVFunctionsWrapper: Hooking COsiris::_ReadHeader");
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			WrappedOsirisReadHeader.Wrap(OsirisReadHeaderProc, &SOsirisReadHeader);
			DetourTransactionCommit();
		}

		RestartLogging();
	}

	Debug(L"OsirisProxy::RegisterDIVFunctionsWrapper: Initializing story.");
	Debug(L"\tErrorMessageProc = %p", errorMessageFunc);
	Debug(L"\tOsirisManager = %p", Manager);
	Debug(L"\tOsirisInterface = %p", osirisInterface);

	return RegisterDivFunctions(Osiris, Functions);
}

int OsirisProxy::SRegisterDIVFunctionsWrapper(void * Osiris, DivFunctions * Functions)
{
	return gOsirisProxy->RegisterDIVFunctionsWrapper(Osiris, Functions);
}

bool OsirisProxy::CallWrapper(uint32_t FunctionId, CallParam * Params)
{
	if (NodeVMTs.empty()) {
		ResolveNodeVMTs(*OsirisNodes);
	}

	DebugDumpCall("Call", FunctionId, Params);
	return CallOriginal(FunctionId, Params);
}

bool OsirisProxy::SCallWrapper(uint32_t FunctionId, CallParam * Params)
{
	return gOsirisProxy->CallWrapper(FunctionId, Params);
}

bool OsirisProxy::QueryWrapper(uint32_t FunctionId, CallParam * Params)
{
	if (NodeVMTs.empty()) {
		ResolveNodeVMTs(*OsirisNodes);
	}

	DebugDumpCall("Query", FunctionId, Params);
	return QueryOriginal(FunctionId, Params);
}

bool OsirisProxy::SQueryWrapper(uint32_t FunctionId, CallParam * Params)
{
	return gOsirisProxy->QueryWrapper(FunctionId, Params);
}

void OsirisProxy::ErrorWrapper(char const * Message)
{
	ErrorOriginal(Message);
}

void OsirisProxy::SErrorWrapper(char const * Message)
{
	gOsirisProxy->ErrorWrapper(Message);
}

void OsirisProxy::AssertWrapper(char const * Unknown, char const * Message, bool Unknown2)
{
	AssertOriginal(Unknown, Message, Unknown2);
}

void OsirisProxy::SAssertWrapper(char const * Unknown, char const * Message, bool Unknown2)
{
	gOsirisProxy->AssertWrapper(Unknown, Message, Unknown2);
}

int OsirisProxy::OsirisReadHeader(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags)
{
	int retval = WrappedOsirisReadHeader(Osiris, OsiSmartBuf, MajorVersion, MinorVersion, BigEndian, Unused, StoryFileVersion, DebugFlags);
	uint32_t NewFlags = (DesiredLogLevel & 0xffff0000) | (*DebugFlags & 0x0000ffff);
	Debug(L"OsirisProxy::OsirisReadHeader: Change DebugFlags from %08x to %08x", *DebugFlags, NewFlags);
	*DebugFlags = NewFlags;
	return retval;
}

int OsirisProxy::SOsirisReadHeader(void * Osiris, void * OsiSmartBuf, unsigned __int8 * MajorVersion, unsigned __int8 * MinorVersion, unsigned __int8 * BigEndian, unsigned __int8 * Unused, char * StoryFileVersion, unsigned int * DebugFlags)
{
	return gOsirisProxy->OsirisReadHeader(Osiris, OsiSmartBuf, MajorVersion, MinorVersion, BigEndian, Unused, StoryFileVersion, DebugFlags);
}

void DebugDumpParam(std::wstringstream & ss, CallParam & param)
{
	switch (param.Type) {
	case ValueType::Integer:
		ss << param.Value.Int;
		break;

	case ValueType::Integer64:
		ss << param.Value.Int64;
		break;

	case ValueType::Real:
		ss << param.Value.Real;
		break;

	case ValueType::None:
	case ValueType::Undefined:
		ss << "(None)";
		break;

	default:
		if (param.Value.String) {
			ss << '"' << param.Value.String << '"';
		} else {
			ss << "(Null)";
		}
		break;
	}
}

void OsirisProxy::DebugDumpCall(char const * Type, OsirisFunctionHandle Handle, CallParam * Params)
{
	Node * firstNode = *(*OsirisNodes)->Db.Start;
	Database * db = (*OsirisDatabases)->Db.Start[3];
	auto * node = &db->Facts.Start->Item;
	std::wstringstream ss;

	auto functionId = Handle.GetFunctionId();
	auto function = Manager->Functions.Buffer[functionId];
	ss << "[" << Type << "] " << function->Name << " (";

	bool hasOutputs = false;
	auto param = Params;
	unsigned paramNo = 0;
	while (param) {
		auto & arg = function->Arguments[paramNo];
		if (arg.Direction == FunctionArgumentDirection::In) {
			if (paramNo > 0) {
				ss << ", ";
			}
			ss << arg.Name << ": ";
			DebugDumpParam(ss, *param);
		} else {
			hasOutputs = true;
		}

		param = param->NextParam;
		paramNo++;
	}
	ss << ")";

	if (hasOutputs) {
		ss << " -> (";
		param = Params;
		paramNo = 0;
		while (param) {
			auto & arg = function->Arguments[paramNo];
			if (arg.Direction == FunctionArgumentDirection::Out) {
				ss << arg.Name << ": ";
				DebugDumpParam(ss, *param);
				if (param->NextParam) {
					ss << ", ";
				}
			}

			param = param->NextParam;
			paramNo++;
		}
		ss << ")";
	}
	
	ss << std::endl;

	OutputDebugStringW(ss.str().c_str());
}


void OsirisProxy::ResolveNodeVMTs(NodeDb * Db)
{
	Debug(L"OsirisProxy::ResolveNodeVMTs");
	std::set<NodeVMT *> VMTs;

	for (unsigned i = 0; i < Db->Db.Size; i++) {
		auto node = Db->Db.Start[i];
		auto vmt = *(NodeVMT **)node;
		VMTs.insert(vmt);
	}

	if (VMTs.size() != 9) {
		Fail(L"Could not locate all Osiris node VMT-s");
	}

	// RuleNode has a different SetRightValueIndex_M implementation
	void * srv{ nullptr };
	std::vector<NodeVMT *> srvA, srvB;
	for (auto vmt : VMTs) {
		if (srv == nullptr) {
			srv = vmt->SetRightValueIndex_M;
		}

		if (srv == vmt->SetRightValueIndex_M) {
			srvA.push_back(vmt);
		} else {
			srvB.push_back(vmt);
		}
	}

	NodeVMT * ruleNodeVMT;
	if (srvA.size() == 1) {
		ruleNodeVMT = *srvA.begin();
	} else if (srvB.size() == 1) {
		ruleNodeVMT = *srvB.begin();
	} else {
		Fail(L"Could not locate RuleNode::__vfptr");
	}

	Debug(L"RuleNode::__vfptr is %p", ruleNodeVMT);
	NodeVMTs.insert(std::make_pair(ruleNodeVMT, NodeType::Rule));

	// RelOpNode is the only node that has the same GetAdapter implementation
	NodeVMT * relOpNodeVMT{ nullptr };
	for (auto vmt : VMTs) {
		if (vmt->GetAdapter == ruleNodeVMT->GetAdapter
			&& vmt != ruleNodeVMT) {
			if (relOpNodeVMT == nullptr) {
				relOpNodeVMT = vmt;
			} else {
				Fail(L"RelOpNode::__vfptr pattern matches multiple VMT-s");
			}
		}
	}

	if (relOpNodeVMT == nullptr) {
		Fail(L"Could not locate RelOpNode::__vfptr");
	}

	Debug(L"RuleNode::__vfptr is %p", relOpNodeVMT);
	NodeVMTs.insert(std::make_pair(relOpNodeVMT, NodeType::RelOp));

	// Find And, NotAnd
	NodeVMT * and1VMT{ nullptr }, *and2VMT{ nullptr };
	for (auto vmt : VMTs) {
		if (vmt->SetNextNode == ruleNodeVMT->SetNextNode
			&& vmt->GetAdapter != ruleNodeVMT->GetAdapter) {
			if (and1VMT == nullptr) {
				and1VMT = vmt;
			} else if (and2VMT == nullptr) {
				and2VMT = vmt;
			} else {
				Fail(L"AndNode::__vfptr pattern matches multiple VMT-s");
			}
		}
	}

	if (and1VMT == nullptr || and2VMT == nullptr) {
		Fail(L"Could not locate AndNode::__vfptr");
	}

	Debug(L"AndNode::__vfptr is %p and %p", and1VMT, and2VMT);
	// No reliable way to detect these; assume that AndNode VMT < NotAndNode VMT
	if (and1VMT < and2VMT) {
		NodeVMTs.insert(std::make_pair(and1VMT, NodeType::And));
		NodeVMTs.insert(std::make_pair(and2VMT, NodeType::NotAnd));
	} else {
		NodeVMTs.insert(std::make_pair(and1VMT, NodeType::NotAnd));
		NodeVMTs.insert(std::make_pair(and2VMT, NodeType::And));
	}

	// Find Query nodes
	void * snn{ nullptr };
	std::map<void *, std::vector<NodeVMT *>> snnMaps;
	std::vector<NodeVMT *> * queryVMTs{ nullptr };
	for (auto vmt : VMTs) {
		if (snnMaps.find(vmt->SetNextNode) == snnMaps.end()) {
			std::vector<NodeVMT *> nvmts{ vmt };
			snnMaps.insert(std::make_pair(vmt->SetNextNode, nvmts));
		} else {
			snnMaps[vmt->SetNextNode].push_back(vmt);
		}
	}

	for (auto & map : snnMaps) {
		if (map.second.size() == 3) {
			queryVMTs = &map.second;
			break;
		}
	}

	if (queryVMTs == nullptr) {
		Fail(L"Could not locate all Query node VMT-s");
	}

	for (auto vmt : *queryVMTs) {
		auto getName = (QueryGetNameProc)vmt->QueryGetName;
		std::string name{ getName() };
		if (name == "internal query") {
			Debug(L"InternalQuery::__vfptr is %p", vmt);
			NodeVMTs.insert(std::make_pair(vmt, NodeType::InternalQuery));
		} else if (name == "DIV query") {
			Debug(L"DivQuery::__vfptr is %p", vmt);
			NodeVMTs.insert(std::make_pair(vmt, NodeType::DivQuery));
		} else if (name == "Osi user query") {
			Debug(L"UserQuery::__vfptr is %p", vmt);
			NodeVMTs.insert(std::make_pair(vmt, NodeType::UserQuery));
		} else {
			Fail(L"Unrecognized Query node VMT");
		}
	}


	// Proc node has different IsProc() code
	NodeVMT * procNodeVMT{ nullptr }, * databaseNodeVMT{ nullptr };
	for (auto vmt : VMTs) {
		if (vmt->IsProc != ruleNodeVMT->IsProc) {
			if (procNodeVMT == nullptr) {
				procNodeVMT = vmt;
			} else {
				Fail(L"ProcNode::__vfptr pattern matches multiple VMT-s");
			}
		}

		if (vmt->IsDataNode != ruleNodeVMT->IsDataNode
			&& vmt->IsProc == ruleNodeVMT->IsProc) {
			if (databaseNodeVMT == nullptr) {
				databaseNodeVMT = vmt;
			} else {
				Fail(L"DatabaseNode::__vfptr pattern matches multiple VMT-s");
			}
		}
	}

	if (procNodeVMT == nullptr) {
		Fail(L"Could not locate ProcNode::__vfptr");
	}

	if (databaseNodeVMT == nullptr) {
		Fail(L"Could not locate DatabaseNode::__vfptr");
	}

	Debug(L"ProcNode::__vfptr is %p", procNodeVMT);
	Debug(L"DatabaseNode::__vfptr is %p", databaseNodeVMT);
	NodeVMTs.insert(std::make_pair(procNodeVMT, NodeType::Proc));
	NodeVMTs.insert(std::make_pair(databaseNodeVMT, NodeType::Database));
}

}