#include <stdafx.h>
#include <Osiris/OsirisExtender.h>
#include <Extender/Shared/ExtensionHelpers.h>
#include <Extender/ScriptExtender.h>
#include <iomanip>

BEGIN_SE()

#if !defined(OSI_NO_DEBUGGER)
void OsirisDebugThreadRunner(OsirisDebugInterface & intf)
{
	intf.Run();
}
#endif

OsirisExtender::OsirisExtender(ExtenderConfig & config)
	: config_(config), 
	injector_(wrappers_, customFunctions_),
	functionLibrary_(*this)
{
}


OsirisExtender::~OsirisExtender()
{
	Shutdown();
}

void OsirisExtender::Initialize()
{
	if (initialized_) return;

	wrappers_.Initialize();

	wrappers_.RegisterDivFunctions.SetPreHook(&OsirisExtender::OnRegisterDIVFunctions, this);
	wrappers_.InitGame.SetPreHook(&OsirisExtender::OnInitGame, this);
	wrappers_.DeleteAllData.SetPreHook(&OsirisExtender::OnDeleteAllData, this);
	wrappers_.Error.SetPreHook(&OsirisExtender::OnError, this);
	wrappers_.Assert.SetPreHook(&OsirisExtender::OnAssert, this);
	wrappers_.Compile.SetWrapper(&OsirisExtender::CompileWrapper, this);
	wrappers_.Load.SetPostHook(&OsirisExtender::OnAfterOsirisLoad, this);
	wrappers_.Merge.SetWrapper(&OsirisExtender::MergeWrapper, this);
#if !defined(OSI_NO_DEBUGGER)
	wrappers_.RuleActionCall.SetWrapper(&OsirisExtender::RuleActionCall, this);
#endif

	injector_.Initialize();
	functionLibrary_.Register();
	initialized_ = true;
}

void OsirisExtender::Shutdown()
{
	if (!initialized_) return;

	if (debugger_) {
		debugger_.reset();
	}

	if (debuggerThread_) {
		debugInterface_->Shutdown();
		debuggerThread_->join();
		debugMsgHandler_.reset();
		debugInterface_.reset();
		debuggerThread_.reset();
	}

	wrappers_.Shutdown();
	initialized_ = false;
}

void OsirisExtender::OnBaseModuleLoaded()
{
	customFunctions_.ClearDynamicEntries();
}

void OsirisExtender::HookNodeVMTs()
{
	if (wrappers_.ResolveNodeVMTs()) {
		nodeVmtWrappers_.reset();
		nodeVmtWrappers_ = std::make_unique<NodeVMTWrappers>(wrappers_.VMTs);
	}
}

void OsirisExtender::LogError(std::string_view msg)
{
	if (storyLoaded_) {
		std::string log = "[Osiris] {E} ";
		log += msg;
		wrappers_.AssertOriginal(false, log.c_str(), false);
	}
}

void OsirisExtender::LogWarning(std::string_view msg)
{
	if (storyLoaded_) {
		std::string log = "[Osiris] {W} ";
		log += msg;
		wrappers_.AssertOriginal(false, log.c_str(), false);
	}
}

void OsirisExtender::LogMessage(std::string_view msg)
{
	if (storyLoaded_) {
		std::string log = "[Osiris] ";
		log += msg;
		wrappers_.AssertOriginal(false, log.c_str(), false);
	}
}

void OsirisExtender::RestartLogging(std::wstring const & Type)
{
	DebugFlag NewFlags = (DebugFlag)((*wrappers_.Globals.DebugFlags & 0x0000ffff) | (config_.DebugFlags & 0xffff0000));

	if (logFilename_.empty() || logType_ != Type) {
		logFilename_ = gExtender->MakeLogFilePath(Type, L"log");
		logType_ = Type;

		if (!logFilename_.empty()) {
			DEBUG(L"ScriptExtender::RestartLogging: Starting %s debug logging.\r\n"
				"\tPath=%s", Type.c_str(), logFilename_.c_str());
		}
	}

	wrappers_.CloseLogFile.CallOriginal(dynamicGlobals_.OsirisObject);

	if (!logFilename_.empty()) {
		*wrappers_.Globals.DebugFlags = NewFlags;
		wrappers_.OpenLogFile.CallOriginal(dynamicGlobals_.OsirisObject, logFilename_.c_str(), L"ab+");
	}
}

void OsirisExtender::OnRegisterDIVFunctions(void * Osiris, DivFunctions * Functions)
{
	wrappers_.RegisterDIVFunctionsPreHook(Osiris, Functions);

	storyLoaded_ = false;
	dynamicGlobals_.OsirisObject = Osiris;
	uint8_t * interfaceLoadPtr = nullptr;
	uint8_t * errorMessageFunc = ResolveRealFunctionAddress((uint8_t *)wrappers_.ErrorOriginal);

	// Look for TypedValue::VMT
	uint8_t const copyCtor1[] = {
		0x48, 0x89, 0x5C, 0x24, 0x08, // mov     [rsp+arg_0], rbx
		0x48, 0x89, 0x74, 0x24, 0x10, // mov     [rsp+arg_8], rsi
		0x57, // push    rdi
		0x48, 0x83, 0xEC, 0x20, // sub     rsp, 20h
		0x33, 0xF6, // xor     esi, esi
		0x48, 0x8D, 0x05 // lea     rax, TypedValue::VMT
	};

	auto start = reinterpret_cast<uint8_t *>(wrappers_.OsirisDllStart);
	auto end = start + wrappers_.OsirisDllSize - sizeof(copyCtor1);

	for (auto p = start; p < end; p++) {
		if (*p == 0x48
			&& memcmp(copyCtor1, p, sizeof(copyCtor1)) == 0) {
			wrappers_.Globals.TypedValueVMT = (void *)AsmResolveInstructionRef(p + 17);
			break;
		}
	}

	if (config_.EnableLogging) {
		RestartLogging(L"Osiris Runtime");
	}

#if 0
	DEBUG("ScriptExtender::OnRegisterDIVFunctions: Initializing story.");
	DEBUG("\tErrorMessageProc = %p", errorMessageFunc);
	DEBUG("\tOsirisManager = %p", Globals.Manager);
	DEBUG("\tOsirisInterface = %p", osirisInterface);
#endif

#if !defined(OSI_NO_DEBUGGER)
	// FIXME - move to DebuggerHooks
	if (config_.EnableDebugger) {
		if (!debuggerThread_) {
			DEBUG("Starting debugger server");
			try {
				debugInterface_ = std::make_unique<OsirisDebugInterface>(config_.DebuggerPort);
				debugMsgHandler_ = std::make_unique<osidbg::DebugMessageHandler>(std::ref(*debugInterface_));
				debuggerThread_ = std::make_unique<std::thread>(std::bind(OsirisDebugThreadRunner, std::ref(*debugInterface_)));
				DEBUG("Osiris debugger listening on 127.0.0.1:%d; DBG protocol version %d", 
					config_.DebuggerPort, osidbg::DebugMessageHandler::ProtocolVersion);
			} catch (std::exception & e) {
				ERR("Osiris debugger start failed: %s", e.what());
			}
		}
	}
#endif
}

void OsirisExtender::OnInitGame(void * Osiris)
{
	DEBUG("OsirisExtender::OnInitGame()");
	gExtender->GetClient().UpdateServerProgress(L"Init Story");
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_) {
		debugger_->GameInitHook();
	}
#endif
}

void OsirisExtender::OnDeleteAllData(void * Osiris, bool DeleteTypes)
{
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_) {
		DEBUG("OsirisExtender::OnDeleteAllData()");
		debugger_->DeleteAllDataHook();
		debugger_.reset();
	}
#endif
}

void OsirisExtender::OnError(char const * Message)
{
	ERR("Osiris Error: %s", Message);
}

void OsirisExtender::OnAssert(bool Successful, char const * Message, bool Unknown2)
{
	if (!Successful) {
		WARN("Osiris Assert: %s", Message);
	}
}

bool OsirisExtender::CompileWrapper(bool (* next)(void *, wchar_t const *, wchar_t const *), void * Osiris, wchar_t const * Path, wchar_t const * Mode)
{
	DEBUG(L"OsirisExtender::CompileWrapper: Starting compilation of '%s'", Path);
	auto OriginalFlags = *wrappers_.Globals.DebugFlags;
	std::wstring storyPath;

	gExtender->GetClient().UpdateServerProgress(L"Preprocess Story");
	customFunctions_.PreProcessStory(Path);

	if (config_.LogCompile || config_.LogFailedCompile) {
		if (!config_.LogCompile) {
			*wrappers_.Globals.DebugFlags = (DebugFlag)(OriginalFlags & ~DebugFlag::DF_CompileTrace);
		}

		RestartLogging(L"Compile");

		if (config_.LogCompile) {
			storyPath = gExtender->MakeLogFilePath(L"Compile", L"div");
			CopyFileW(Path, storyPath.c_str(), TRUE);
		}
	}

	gExtender->GetClient().UpdateServerProgress(L"Compile Story");
	auto ret = next(Osiris, Path, Mode);

	if (ret) {
		DEBUG("ScriptExtender::CompileWrapper: Success.");
	} else {
		ERR("ScriptExtender::CompileWrapper: Compilation FAILED.");
	}

	if (config_.LogCompile || config_.LogFailedCompile) {
		*wrappers_.Globals.DebugFlags = OriginalFlags;
		wrappers_.CloseLogFile.CallOriginal(dynamicGlobals_.OsirisObject);

		if (ret) {
			if (config_.LogCompile) {
				DeleteFileW(storyPath.c_str());
			} else if (!logFilename_.empty()) {
				DeleteFileW(logFilename_.c_str());
			}
		}
	}

	return ret;
}

void OsirisExtender::OnAfterOsirisLoad(void * Osiris, void * Buf, int retval)
{
	std::lock_guard _(storyLoadLock_);

#if !defined(OSI_NO_DEBUGGER)
	if (debuggerThread_) {
		HookNodeVMTs();
	}
#endif

	storyLoaded_ = true; 
	DEBUG("ScriptExtender::OnAfterOsirisLoad: %d nodes", (*wrappers_.Globals.Nodes)->Db.Size);

#if !defined(OSI_NO_DEBUGGER)
	if (debuggerThread_ && nodeVmtWrappers_) {
		debugger_.reset();
		debugger_ = std::make_unique<osidbg::Debugger>(wrappers_.Globals, std::ref(*debugMsgHandler_));
		debugger_->StoryLoaded();
	}
#endif

	if (gExtender->GetServer().HasExtensionState()) {
		gExtender->GetServer().GetExtensionState().StoryLoaded();
	}
}

bool OsirisExtender::MergeWrapper(bool (*next)(void*, wchar_t*), void * Osiris, wchar_t * Src)
{
	DEBUG("ScriptExtender::MergeWrapper() - Started merge");

	auto cli = GetEoCClient();
	auto srv = GetEoCServer();

	if (gExtender->GetServer().HasExtensionState()) {
		gExtender->GetServer().GetExtensionState().StorySetMerging(true);
	}

	gExtender->GetClient().UpdateServerProgress(L"Merge Story");

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->MergeStarted();
	}
#endif

	bool retval = next(Osiris, Src);

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->MergeFinished();
	}
#endif

	if (gExtender->GetServer().HasExtensionState()) {
		gExtender->GetServer().GetExtensionState().StorySetMerging(false);
	}

	DEBUG("ScriptExtender::MergeWrapper() - Finished merge");
	return retval;
}

void OsirisExtender::RuleActionCall(void (*next)(RuleActionNode*, void*, void*, void*, void*), RuleActionNode * Action, void * a1, void * a2, void * a3, void * a4)
{
#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->RuleActionPreHook(Action);
	}
#endif

	next(Action, a1, a2, a3, a4);

#if !defined(OSI_NO_DEBUGGER)
	if (debugger_ != nullptr) {
		debugger_->RuleActionPostHook(Action);
	}
#endif
}

END_SE()

BEGIN_NS(osi)

void OsirisManager::ExpandNotificationBuffer(uint32_t size)
{
	auto oldSize = EndOffset - StartOffset;
	auto oldCapacity = NotificationBuffer.capacity();
	if (oldSize + size > NotificationBuffer.capacity()) {
		NotificationBuffer.resize(oldCapacity + size);
		for (auto i = oldCapacity; i < NotificationBuffer.capacity(); i++) {
			NotificationBuffer[i] = GameAllocArray<char>(MaxParamSize);
		}
	}

	auto oldStartOffset = StartOffset;
	auto oldEndOffset = EndOffset;
	if (oldEndOffset <= oldStartOffset) {
		ObjectSet<char*> reorderBuf;
		reorderBuf.resize(oldCapacity);

		auto curOff = oldStartOffset;
		for (uint32_t i = 0; i < oldCapacity; i++) {
			curOff = (curOff + 1) % oldCapacity;
			reorderBuf[i] = NotificationBuffer[curOff];
		}

		memcpy(NotificationBuffer.data(), reorderBuf.data(), sizeof(char*) * oldCapacity);
		EndOffset = oldSize;
		StartOffset = 0;
	}
}

void OsirisManager::QueueNotification(OsirisNotification* fun)
{
	ExpandNotificationBuffer(fun->NumParams + 1);

	*reinterpret_cast<OsirisNotification**>(NotificationBuffer[EndOffset]) = fun;
	EndOffset = (EndOffset + 1) % NotificationBuffer.capacity();

	auto param = fun->ArgumentDescs;
	while (param != nullptr) {
		auto typeId = param->Value.TypeId;
		if (typeId > ValueType::GuidString) {
			typeId = (ValueType)BuiltinTypes[(unsigned)typeId].BuiltinTypeId;
		}

		if (typeId == ValueType::String || typeId == ValueType::GuidString) {
			strncpy_s(NotificationBuffer[EndOffset], MaxParamSize, param->Value.String, strlen(param->Value.String));
		} else {
			*reinterpret_cast<int32_t*>(NotificationBuffer[EndOffset]) = param->Value.Int32;
		}

		EndOffset = (EndOffset + 1) % NotificationBuffer.capacity();
		param = param->NextParam;
	}
}

END_NS()
