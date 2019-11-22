#include "stdafx.h"
#include "OsirisProxy.h"
#include "NodeHooks.h"
#include "DebugInterface.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <psapi.h>

namespace osidbg
{

GameType gGameType = GameType::Unknown;

std::unique_ptr<OsirisProxy> gOsirisProxy;


OsirisProxy::OsirisProxy()
	: CustomInjector(Wrappers, CustomFunctions),
	FunctionLibrary(*this)
{
}

void OsirisProxy::Initialize()
{
	Debug("OsirisProxy::Initialize: Starting");
	auto initStart = std::chrono::high_resolution_clock::now();
	Wrappers.Initialize();

	using namespace std::placeholders;
	Wrappers.RegisterDivFunctions.AddPreHook(std::bind(&OsirisProxy::OnRegisterDIVFunctions, this, _1, _2));
	Wrappers.InitGame.AddPreHook(std::bind(&OsirisProxy::OnInitGame, this, _1));
	Wrappers.DeleteAllData.AddPreHook(std::bind(&OsirisProxy::OnDeleteAllData, this, _1, _2));
	Wrappers.Error.AddPreHook(std::bind(&OsirisProxy::OnError, this, _1));
	Wrappers.Assert.AddPreHook(std::bind(&OsirisProxy::OnAssert, this, _1, _2, _3));
	Wrappers.Compile.SetWrapper(std::bind(&OsirisProxy::CompileWrapper, this, _1, _2, _3, _4));
	Wrappers.Load.AddPostHook(std::bind(&OsirisProxy::OnAfterOsirisLoad, this, _1, _2, _3));
	Wrappers.Merge.SetWrapper(std::bind(&OsirisProxy::MergeWrapper, this, _1, _2, _3));
#if !defined(OSI_NO_DEBUGGER)
	Wrappers.RuleActionCall.SetWrapper(std::bind(&OsirisProxy::RuleActionCall, this, _1, _2, _3, _4, _5, _6));
#endif

	if (ExtensionsEnabled) {
		if (Libraries.FindLibraries()) {
			CustomInjector.Initialize();
			FunctionLibrary.Register();
			ResetExtensionState();

			Wrappers.InitializeExtensions();
			Wrappers.InitNetworkFixedStrings.AddPostHook(std::bind(&OsirisProxy::OnInitNetworkFixedStrings, this, _1, _2));
			Wrappers.GameStateLoadModuleDo.AddPostHook(std::bind(&OsirisProxy::OnBaseModuleLoaded, this, _1));

#if 0
			auto headers = CustomFunctions.GenerateHeaders();
			Debug(" === EXTENSION HEADERS ===");
			std::cout << headers << std::endl;
			Debug(" === END EXTENSION HEADERS ===");
#endif
		}
		else {
			Debug("OsirisProxy::Initialize: Could not load libraries; skipping scripting extension initialization.");
			ExtensionsEnabled = false;
		}
	}
	else {
		Debug("OsirisProxy::Initialize: Skipped library init -- scripting extensions not enabled.");
	}

	auto initEnd = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(initEnd - initStart).count();
	Debug("Library startup took %d ms", ms);
}

void OsirisProxy::Shutdown()
{
	Debug("OsirisProxy::Shutdown: Starting");
	Wrappers.Shutdown();
}


void OsirisProxy::EnableDebugging(bool Enabled, uint16_t Port)
{
	DebuggingEnabled = Enabled;
	DebuggerPort = Port;
}

void OsirisProxy::EnableExtensions(bool Enabled)
{
	ExtensionsEnabled = Enabled;
}

void OsirisProxy::SetupLogging(bool Enabled, DebugFlag LogLevel, std::wstring const & Path)
{
	DeleteFileW(Path.c_str());
	LoggingEnabled = Enabled;
	DesiredLogLevel = LogLevel;
	LogDirectory = Path;
}

void OsirisProxy::EnableCompileLogging(bool Log)
{
	CompilationLogEnabled = Log;
}

void OsirisProxy::SetupNetworkStringsDump(bool Enable)
{
	EnableNetworkStringsDump = Enable;
}

void OsirisProxy::LogOsirisError(std::string const & msg)
{
	LogOsirisMsg("{E} " + msg);
}

void OsirisProxy::LogOsirisWarning(std::string const & msg)
{
	LogOsirisMsg("{W} " + msg);
}

void OsirisProxy::LogOsirisMsg(std::string const & msg)
{
	auto log = "[Osiris] " + msg;
	DebugRaw(log.c_str());
	if (StoryLoaded) {
		Wrappers.AssertOriginal(false, log.c_str(), false);
	}
}

void OsirisProxy::RestartLogging(std::wstring const & Type)
{
	DebugFlag NewFlags = (DebugFlag)((DesiredLogLevel & 0xffff0000) | (*Wrappers.Globals.DebugFlags & 0x0000ffff));

	if (LogFilename.empty() || LogType != Type)
	{
		LogFilename = MakeLogFilePath(Type, L"log");
		LogType = Type;

		Debug(L"OsirisProxy::RestartLogging: Starting %s debug logging.\r\n"
			"\tPath=%s\r\n"
			"\tOriginalDebugFlags=%08x\r\n"
			"\tDebugFlags=%08x", 
			Type.c_str(), LogFilename.c_str(), *Wrappers.Globals.DebugFlags, NewFlags);
	}

	Wrappers.CloseLogFile.CallOriginal(DynGlobals.OsirisObject);
	*Wrappers.Globals.DebugFlags = NewFlags;
	Wrappers.OpenLogFile.CallOriginal(DynGlobals.OsirisObject, LogFilename.c_str(), L"ab+");
}

std::wstring OsirisProxy::MakeLogFilePath(std::wstring const & Type, std::wstring const & Extension)
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
	ss << LogDirectory << L"\\"
		<< Type << L" "
		<< std::setfill(L'0')
		<< (tm.tm_year + 1900) << L"-"
		<< std::setw(2) << (tm.tm_mon + 1) << L"-"
		<< std::setw(2) << tm.tm_mday << L" "
		<< std::setw(2) << tm.tm_hour << L"-"
		<< std::setw(2) << tm.tm_min << L"-"
		<< std::setw(2) << tm.tm_sec << L"." << Extension;
	return ss.str();
}

void OsirisProxy::HookNodeVMTs()
{
	gNodeVMTWrappers = std::make_unique<NodeVMTWrappers>(NodeVMTs);
}

#if !defined(OSI_NO_DEBUGGER)
void DebugThreadRunner(DebugInterface & intf)
{
	intf.Run();
}
#endif

void OsirisProxy::OnRegisterDIVFunctions(void * Osiris, DivFunctions * Functions)
{
	// FIXME - register before OsirisWrappers!
	StoryLoaded = false;
	DynGlobals.OsirisObject = Osiris;
	uint8_t * interfaceLoadPtr = nullptr;
	// uint8_t * errorMessageFunc = ResolveRealFunctionAddress((uint8_t *)Functions->ErrorMessage);
	uint8_t * errorMessageFunc = ResolveRealFunctionAddress((uint8_t *)Wrappers.ErrorOriginal);

	// Try to find ptr of gOsirisInterface
	OsirisInterface * osirisInterface = nullptr;
	for (uint8_t * ptr = errorMessageFunc; ptr < errorMessageFunc + 64; ptr++) {
		// Look for the instruction "mov rbx, cs:gOsirisInterface"
		if (ptr[0] == 0x48 && ptr[1] == 0x8B && ptr[2] == 0x1D && ptr[6] < 0x02) {
			int32_t relOffset = *reinterpret_cast<int32_t *>(ptr + 3);
			uint64_t osiPtr = (uint64_t)ptr + relOffset + 7;
			osirisInterface = *(OsirisInterface **)osiPtr;
			DynGlobals.Manager = osirisInterface->Manager;
			break;
		}
	}

	if (DynGlobals.Manager == nullptr) {
		Fail("Could not locate OsirisInterface");
	}

	if (LoggingEnabled) {
		RestartLogging(L"Runtime");
	}

#if 0
	Debug("OsirisProxy::OnRegisterDIVFunctions: Initializing story.");
	Debug("\tErrorMessageProc = %p", errorMessageFunc);
	Debug("\tOsirisManager = %p", Globals.Manager);
	Debug("\tOsirisInterface = %p", osirisInterface);
#endif

#if !defined(OSI_NO_DEBUGGER)
	// FIXME - move to DebuggerHooks
	if (DebuggingEnabled) {
		if (DebuggerThread == nullptr) {
			Debug("Starting debugger server");
			debugInterface_ = std::make_unique<DebugInterface>(DebuggerPort);
			debugMsgHandler_ = std::make_unique<DebugMessageHandler>(std::ref(*debugInterface_));

			DebuggerThread = new std::thread(std::bind(DebugThreadRunner, std::ref(*debugInterface_)));
		}
	} else if (!DebugDisableLogged) {
		Debug("Debugging not enabled; not starting debugger server thread.");
		DebugDisableLogged = true;
	}
#endif
}

void OsirisProxy::OnInitGame(void * Osiris)
{
	Debug("OsirisProxy::OnInitGame()");
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_) {
		debugger_->GameInitHook();
	}
#endif
}

void OsirisProxy::OnDeleteAllData(void * Osiris, bool DeleteTypes)
{
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_) {
		Debug("OsirisProxy::OnDeleteAllData()");
		debugger_->DeleteAllDataHook();
		debugger_.reset();
	}
#endif
}

void OsirisProxy::OnError(char const * Message)
{
	Debug("Osiris Error: %s", Message);
}

void OsirisProxy::OnAssert(bool Successful, char const * Message, bool Unknown2)
{
	if (!Successful) {
		Debug("Osiris Assert: %s", Message);
	}
}

bool OsirisProxy::CompileWrapper(std::function<bool(void *, wchar_t const *, wchar_t const *)> const & Next, void * Osiris, wchar_t const * Path, wchar_t const * Mode)
{
	Debug(L"OsirisProxy::CompileWrapper: Starting compilation of '%s'", Path);
	auto OriginalFlags = *Wrappers.Globals.DebugFlags;
	std::wstring storyPath;

	if (CompilationLogEnabled) {
		storyPath = MakeLogFilePath(L"Compile", L"div");
		*Wrappers.Globals.DebugFlags = (DebugFlag)(OriginalFlags & ~DebugFlag::DF_CompileTrace);
		CopyFileW(Path, storyPath.c_str(), TRUE);
		RestartLogging(L"Compile");
	}

	auto ret = Next(Osiris, Path, Mode);

	if (ret) {
		Debug("OsirisProxy::CompileWrapper: Success.");
	} else {
		Debug("OsirisProxy::CompileWrapper: Compilation FAILED.");
	}

	if (CompilationLogEnabled) {
		*Wrappers.Globals.DebugFlags = OriginalFlags;
		Wrappers.CloseLogFile.CallOriginal(DynGlobals.OsirisObject);

		if (ret) {
			DeleteFileW(storyPath.c_str());
			DeleteFileW(LogFilename.c_str());
		} else {
			auto filteredLogPath = MakeLogFilePath(L"Compile", L"final.log");
			std::ifstream logIn(LogFilename.c_str(), std::ios::in);
			std::ofstream logOut(filteredLogPath.c_str(), std::ios::out);
			std::string logLine;

			while (std::getline(logIn, logLine)) {
				if (logLine.length() < 9 ||
					(memcmp(logLine.c_str(), ">>> call", 8) != 0 &&
					 memcmp(logLine.c_str(), ">>> Auto-", 9) != 0)) {
					logOut << logLine << std::endl;
				}
			}

			logIn.close();
			logOut.close();

			DeleteFileW(LogFilename.c_str());
		}
	}

	return ret;
}

void OsirisProxy::OnAfterOsirisLoad(void * Osiris, void * Buf, int retval)
{
#if !defined(OSI_NO_DEBUGGER)
	if (DebuggerThread != nullptr && !ResolvedNodeVMTs) {
		ResolveNodeVMTs(*Wrappers.Globals.Nodes);
		ResolvedNodeVMTs = true;
		HookNodeVMTs();
	}
#endif

	StoryLoaded = true;
	Debug("OsirisProxy::OnAfterOsirisLoad: %d nodes", (*Wrappers.Globals.Nodes)->Db.Size);

#if !defined(OSI_NO_DEBUGGER)
	if (DebuggerThread != nullptr && gNodeVMTWrappers) {
		debugger_.reset();
		debugger_ = std::make_unique<Debugger>(Wrappers.Globals, std::ref(*debugMsgHandler_));
		debugger_->StoryLoaded();
	}
#endif
}

void OsirisProxy::OnInitNetworkFixedStrings(void * self, void * arg1)
{
	if (EnableNetworkStringsDump) {
		DumpNetworkFixedStrings();
	}
}

void OsirisProxy::DumpNetworkFixedStrings()
{
	auto nfs = Libraries.NetworkFixedStrings;
	if (nfs != nullptr && (*nfs)->Initialized) {
		auto const & strings = (*nfs)->FixedStrSet.Set;

		auto nfsLogPath = MakeLogFilePath(L"NetworkFixedStrings", L"log");
		std::ofstream logOut(nfsLogPath.c_str(), std::ios::out);
		for (uint32_t i = 0; i < strings.Size; i++) {
			auto str = strings.Buf[i].Str;
			logOut << (str == nullptr ? "(NULL)" : str) << std::endl;
		}
		logOut.close();
		Debug(L"OsirisProxy::DumpNetworkFixedStrings() - Saved to %s", nfsLogPath.c_str());


	} else {
		Debug("OsirisProxy::DumpNetworkFixedStrings() - Fixed strings not initialized yet");
	}
}

bool OsirisProxy::MergeWrapper(std::function<bool (void *, wchar_t *)> const & Next, void * Osiris, wchar_t * Src)
{
	Debug("OsirisProxy::MergeWrapper() - Started merge");

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->MergeStarted();
	}
#endif

	bool retval = Next(Osiris, Src);

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->MergeFinished();
	}
#endif

	Debug("OsirisProxy::MergeWrapper() - Finished merge");
	return retval;
}

void OsirisProxy::RuleActionCall(std::function<void (RuleActionNode *, void *, void *, void *, void *)> const & Next, RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4)
{
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->RuleActionPreHook(Action);
	}
#endif

	Next(Action, a1, a2, a3, a4);

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->RuleActionPostHook(Action);
	}
#endif
}

void OsirisProxy::SaveNodeVMT(NodeType type, NodeVMT * vmt)
{
	assert(type >= NodeType::Database && type <= NodeType::Max);
	NodeVMTs[(unsigned)type] = vmt;
}

void OsirisProxy::ResolveNodeVMTs(NodeDb * Db)
{
#if 0
	Debug("OsirisProxy::ResolveNodeVMTs");
#endif
	std::set<NodeVMT *> VMTs;

	for (unsigned i = 0; i < Db->Db.Size; i++) {
		auto node = Db->Db.Start[i];
		auto vmt = *(NodeVMT **)node;
		VMTs.insert(vmt);
	}

	if (VMTs.size() != (unsigned)NodeType::Max) {
		Fail("Could not locate all Osiris node VMT-s");
	}

	// RuleNode has a different SetLineNumber implementation
	void * srv{ nullptr };
	std::vector<NodeVMT *> srvA, srvB;
	for (auto vmt : VMTs) {
		if (srv == nullptr) {
			srv = vmt->SetLineNumber;
		}

		if (srv == vmt->SetLineNumber) {
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
		Fail("Could not locate RuleNode::__vfptr");
	}

#if 0
	Debug("RuleNode::__vfptr is %p", ruleNodeVMT);
#endif
	SaveNodeVMT(NodeType::Rule, ruleNodeVMT);

	// RelOpNode is the only node that has the same GetAdapter implementation
	NodeVMT * relOpNodeVMT{ nullptr };
	for (auto vmt : VMTs) {
		if (vmt->GetAdapter == ruleNodeVMT->GetAdapter
			&& vmt != ruleNodeVMT) {
			if (relOpNodeVMT == nullptr) {
				relOpNodeVMT = vmt;
			} else {
				Fail("RelOpNode::__vfptr pattern matches multiple VMT-s");
			}
		}
	}

	if (relOpNodeVMT == nullptr) {
		Fail("Could not locate RelOpNode::__vfptr");
	}

#if 0
	Debug("RuleNode::__vfptr is %p", relOpNodeVMT);
#endif
	SaveNodeVMT(NodeType::RelOp, relOpNodeVMT);

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
				Fail("AndNode::__vfptr pattern matches multiple VMT-s");
			}
		}
	}

	if (and1VMT == nullptr || and2VMT == nullptr) {
		Fail("Could not locate AndNode::__vfptr");
	}

#if 0
	Debug("AndNode::__vfptr is %p and %p", and1VMT, and2VMT);
#endif
	// No reliable way to detect these; assume that AndNode VMT < NotAndNode VMT
	if (and1VMT < and2VMT) {
		SaveNodeVMT(NodeType::And, and1VMT);
		SaveNodeVMT(NodeType::NotAnd, and2VMT);
	} else {
		SaveNodeVMT(NodeType::NotAnd, and1VMT);
		SaveNodeVMT(NodeType::And, and2VMT);
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
		Fail("Could not locate all Query node VMT-s");
	}

	for (auto vmt : *queryVMTs) {
		auto getName = (NodeVMT::GetQueryNameProc)vmt->GetQueryName;
		std::string name{ getName(nullptr) };
		if (name == "internal query") {
#if 0
			Debug("InternalQuery::__vfptr is %p", vmt);
#endif
			SaveNodeVMT(NodeType::InternalQuery, vmt);
		} else if (name == "DIV query") {
#if 0
			Debug("DivQuery::__vfptr is %p", vmt);
#endif
			SaveNodeVMT(NodeType::DivQuery, vmt);
		} else if (name == "Osi user query") {
#if 0
			Debug("UserQuery::__vfptr is %p", vmt);
#endif
			SaveNodeVMT(NodeType::UserQuery, vmt);
		} else {
			Fail("Unrecognized Query node VMT");
		}
	}


	// Proc node has different IsProc() code
	NodeVMT * procNodeVMT{ nullptr }, * databaseNodeVMT{ nullptr };
	for (auto vmt : VMTs) {
		if (vmt->IsProc != ruleNodeVMT->IsProc) {
			if (procNodeVMT == nullptr) {
				procNodeVMT = vmt;
			} else {
				Fail("ProcNode::__vfptr pattern matches multiple VMT-s");
			}
		}

		if (vmt->IsDataNode != ruleNodeVMT->IsDataNode
			&& vmt->IsProc == ruleNodeVMT->IsProc) {
			if (databaseNodeVMT == nullptr) {
				databaseNodeVMT = vmt;
			} else {
				Fail("DatabaseNode::__vfptr pattern matches multiple VMT-s");
			}
		}
	}

	if (procNodeVMT == nullptr) {
		Fail("Could not locate ProcNode::__vfptr");
	}

	if (databaseNodeVMT == nullptr) {
		Fail("Could not locate DatabaseNode::__vfptr");
	}

#if 0
	Debug("ProcNode::__vfptr is %p", procNodeVMT);
	Debug("DatabaseNode::__vfptr is %p", databaseNodeVMT);
#endif
	SaveNodeVMT(NodeType::Proc, procNodeVMT);
	SaveNodeVMT(NodeType::Database, databaseNodeVMT);
}

void OsirisProxy::OnBaseModuleLoaded(void * self)
{
	auto modManager = GetModManager();
	if (modManager != nullptr) {
		Debug(L"OsirisProxy::OnBaseModuleLoaded(): Loaded base module '%s'", modManager->BaseModule.Info.Name.GetPtr());
	}

	if (ExtensionsEnabled) {
		ResetExtensionState();
		ExtState->LoadConfigs();
		PostInitExtension();
		if (!Libraries.CriticalInitializationFailed()) {
			Libraries.EnableCustomStats();
			FunctionLibrary.OnBaseModuleLoaded();
		}
	}
}

void OsirisProxy::PostInitExtension()
{
	if (ExtensionsEnabled) {
		if (Libraries.PostStartupFindLibraries()) {
			FunctionLibrary.PostStartup();
		}
	}

	if (Libraries.CriticalInitializationFailed()) {
		Libraries.ShowStartupError(L"A severe error has occurred during Osiris Extender initialization. Extension features will be unavailable.", true, false);
	} else if (Libraries.InitializationFailed()) {
		Libraries.ShowStartupError(L"An error has occurred during Osiris Extender initialization. Some extension features might be unavailable.", true, false);
	}
}

void OsirisProxy::ResetExtensionState()
{
	ExtState = std::make_unique<ExtensionState>();
	ExtState->Reset();
}

}
