#pragma once

#include "stdafx.h"
#include "CustomFunctions.h"
#include "OsirisProxy.h"
#include <fstream>
#include <sstream>

namespace osidbg {

bool CustomFunction::ValidateArgs(OsiArgumentDesc const & params) const
{
	if (params.Count() != params_.size()) {
		OsiError("Function " << name_  << "/" << params_.size() << ": Argument count mismatch");
		return false;
	}

	for (auto i = 0; i < params_.size(); i++) {
		auto const & value = params.Get(i);
		auto const & param = params_[i];
		if (!ValidateParam(param, value)) {
			return false;
		}
	}

	return true;
}

bool CustomFunction::ValidateParam(CustomFunctionParam const & param, OsiArgumentValue const & value) const
{
	if (param.Type == ValueType::None
		|| param.Dir == FunctionArgumentDirection::Out) {
		return true;
	}

	auto typeId = value.TypeId;
	if (typeId == ValueType::CharacterGuid
		|| typeId == ValueType::ItemGuid
		|| typeId == ValueType::TriggerGuid
		|| typeId == ValueType::SplineGuid
		|| typeId == ValueType::LevelTemplateGuid) {
		typeId = ValueType::GuidString;
	}

	auto paramTypeId = param.Type;
	if (paramTypeId == ValueType::CharacterGuid
		|| paramTypeId == ValueType::ItemGuid
		|| paramTypeId == ValueType::TriggerGuid
		|| paramTypeId == ValueType::SplineGuid
		|| paramTypeId == ValueType::LevelTemplateGuid) {
		paramTypeId = ValueType::GuidString;
	}

	if (typeId != paramTypeId) {
		OsiError("Function " << name_ << "/" << params_.size() << ": Argument '" << param.Name
			<< "' type mismatch; expected " << (unsigned)paramTypeId << ", got " << (unsigned)typeId);
		return false;
	}

	return true;
}

void CustomFunction::GenerateHeader(std::stringstream & ss) const
{
	switch (handle_.type()) {
	case EoCFunctionType::Call: ss << "call "; break;
	case EoCFunctionType::Query: ss << "query "; break;
	case EoCFunctionType::Event: ss << "event "; break;
	default: throw new std::runtime_error("EoC function not supported");
	}

	ss << name_ << "(";
	for (unsigned i = 0; i < params_.size(); i++) {
		if (i > 0) {
			ss << ", ";
		}

		auto const & param = params_[i];
		if (handle_.type() == EoCFunctionType::Query) {
			if (param.Dir == FunctionArgumentDirection::In) {
				ss << "[in]";
			} else {
				ss << "[out]";
			}
		}

		switch (param.Type) {
		case ValueType::None: ss << "(STRING)"; break; // "ANY" type passed from Osiris as string
		case ValueType::Integer: ss << "(INTEGER)"; break;
		case ValueType::Integer64: ss << "(INTEGER64)"; break;
		case ValueType::Real: ss << "(REAL)"; break;
		case ValueType::String: ss << "(STRING)"; break;
		case ValueType::GuidString: ss << "(GUIDSTRING)"; break;
		case ValueType::CharacterGuid: ss << "(CHARACTERGUID)"; break;
		case ValueType::ItemGuid: ss << "(ITEMGUID)"; break;
		case ValueType::TriggerGuid: ss << "(TRIGGERGUID)"; break;
		default: throw new std::runtime_error("Type not supported");
		}

		ss << "_" << param.Name;
	}

	ss << ") (" << (unsigned)handle_.type() << "," << handle_.classIndex() << "," << handle_.functionIndex() << ",1)\r\n";
}


bool CustomCall::Call(OsiArgumentDesc const & params)
{
	if (!ValidateArgs(params)) {
		return false;
	}

	handler_(std::ref(params));
	return true;
}


bool CustomQuery::Query(OsiArgumentDesc & params)
{
	if (!ValidateArgs(params)) {
		return false;
	}

	return handler_(std::ref(params));
}


FunctionHandle CustomFunctionManager::Register(std::unique_ptr<CustomCall> call)
{
	RegisterSignature(call.get());
	FunctionHandle handle{ EoCFunctionType::Call, CallClassId, (unsigned)calls_.size() };
	call->AssignHandle(handle);
	calls_.push_back(std::move(call));
	return handle;
}

FunctionHandle CustomFunctionManager::Register(std::unique_ptr<CustomQuery> qry)
{
	RegisterSignature(qry.get());
	FunctionHandle handle{ EoCFunctionType::Query, QueryClassId, (unsigned)queries_.size() };
	qry->AssignHandle(handle);
	queries_.push_back(std::move(qry));
	return handle;
}

FunctionHandle CustomFunctionManager::Register(std::unique_ptr<CustomEvent> event)
{
	RegisterSignature(event.get());
	FunctionHandle handle{ EoCFunctionType::Event, EventClassId, (unsigned)events_.size() };
	event->AssignHandle(handle);
	events_.push_back(std::move(event));
	return handle;
}

CustomFunction * CustomFunctionManager::Find(FunctionNameAndArity const & signature)
{
	auto it = signatures_.find(signature);
	if (it == signatures_.end()) {
		return nullptr;
	} else {
		return it->second;
	}
}

bool CustomFunctionManager::Call(FunctionHandle handle, OsiArgumentDesc const & params)
{
	if (handle.classIndex() != CallClassId) {
		OsiError("Cannot call " << (uint32_t)handle << " - not a custom function!");
		return false;
	}

	if (handle.functionIndex() >= calls_.size()) {
		OsiError("Call index " << handle.functionIndex() << " out of bounds!");
		return false;
	}

	return calls_[handle.functionIndex()]->Call(params);
}

bool CustomFunctionManager::Query(FunctionHandle handle, OsiArgumentDesc & params)
{
	if (handle.classIndex() != QueryClassId) {
		OsiError("Cannot query " << (uint32_t)handle << " - not a custom function!");
		return false;
	}

	if (handle.functionIndex() >= queries_.size()) {
		OsiError("Query index " << handle.functionIndex() << " out of bounds!");
		return false;
	}

	return queries_[handle.functionIndex()]->Query(params);
}

void CustomFunctionManager::RegisterSignature(CustomFunction * func)
{
	auto signature = func->NameAndArity();
	if (signatures_.find(signature) != signatures_.end()) {
		throw std::runtime_error(std::string("Could not register function ") + signature.Name.c_str() + "/"
			+ std::to_string(signature.Arity) + " - already registered!");
	}

	signatures_.insert(std::make_pair(signature, func));
}

std::string CustomFunctionManager::GenerateHeaders() const
{
	std::stringstream ss;

	for (auto const & call : calls_) {
		call->GenerateHeader(ss);
	}

	for (auto const & query : queries_) {
		query->GenerateHeader(ss);
	}

	for (auto const & event : events_) {
		event->GenerateHeader(ss);
	}

	return ss.str();
}

void CustomFunctionManager::PreProcessStory(std::string const & original, std::string & postProcessed)
{
	std::string ph1;
	ph1.reserve(original.size());
	postProcessed.reserve(original.size());

	std::size_t pos = 0;
	while (pos < original.size()) {
		auto next = original.find("/* [OSITOOLS_ONLY]", pos);
		if (next == std::string::npos) {
			ph1 += original.substr(pos);
			break;
		}

		auto end = original.find("*/", next);
		if (end == std::string::npos) {
			ph1 += original.substr(pos);
			break;
		}

		ph1 += original.substr(pos, next - pos);
		ph1 += original.substr(next + 19, end - next - 19);
		pos = end + 2;
	}

	pos = 0;
	while (pos < ph1.size()) {
		auto next = ph1.find("// [BEGIN_NO_OSITOOLS]", pos);
		if (next == std::string::npos) {
			postProcessed += ph1.substr(pos);
			break;
		}

		auto end = ph1.find("// [END_NO_OSITOOLS]", next);
		if (end == std::string::npos) {
			postProcessed += ph1.substr(pos);
			break;
		}

		postProcessed += ph1.substr(pos, next - pos);
		pos = end + 21;
	}
}

void CustomFunctionManager::PreProcessStory(wchar_t const * path)
{
	if (!ExtensionState::Get().PreprocessStory) return;

	std::string original;
	std::string postProcessed;

	{
		std::ifstream f(path, std::ios::in | std::ios::binary);
		if (!f.good()) return;

		f.seekg(0, std::ios::end);
		original.resize(f.tellg());
		f.seekg(0, std::ios::beg);
		f.read(original.data(), original.size());
	}

	PreProcessStory(original, postProcessed);

	{
		std::ofstream f(path, std::ios::out | std::ios::binary);
		if (!f.good()) return;

		f.write(postProcessed.data(), postProcessed.size());
	}
}


CustomFunctionInjector::CustomFunctionInjector(OsirisWrappers & wrappers, CustomFunctionManager & functions)
	: wrappers_(wrappers), functions_(functions)
{}

void CustomFunctionInjector::Initialize()
{
	using namespace std::placeholders;
	wrappers_.GetFunctionMappings.AddPostHook(std::bind(&CustomFunctionInjector::OnAfterGetFunctionMappings, this, _1, _2, _3));
	wrappers_.Call.SetWrapper(std::bind(&CustomFunctionInjector::CallWrapper, this, _1, _2, _3));
	wrappers_.Query.SetWrapper(std::bind(&CustomFunctionInjector::QueryWrapper, this, _1, _2, _3));
	wrappers_.CreateFileW.AddPostHook(std::bind(&CustomFunctionInjector::OnCreateFile, this, _1, _2, _3, _4, _5, _6, _7, _8));
	wrappers_.CloseHandle.AddPostHook(std::bind(&CustomFunctionInjector::OnCloseHandle, this, _1, _2));
}

unsigned gCustomEventDepth{ 0 };

struct CustomEventGuard
{
	static constexpr unsigned MaxDepth = 10;

	CustomEventGuard() { gCustomEventDepth++; }
	~CustomEventGuard() { gCustomEventDepth--; }

	bool CanThrowEvent() const
	{
		return gCustomEventDepth < MaxDepth;
	}
};

void CustomFunctionInjector::ThrowEvent(FunctionHandle handle, OsiArgumentDesc * args) const
{
	auto it = divToOsiMappings_.find(handle);
	if (it != divToOsiMappings_.end()) {
		CustomEventGuard guard;
		if (guard.CanThrowEvent()) {
			auto osiris = gOsirisProxy->GetDynamicGlobals().OsirisObject;
			gOsirisProxy->GetWrappers().Event.CallOriginal(osiris, it->second, args);
		} else {
			OsiError("Maximum Osiris event depth (" << gCustomEventDepth << ") exceeded");
		}
	} else {
		OsiError("Event handle not mapped: " << std::hex << (unsigned)handle);
	}
}

void OsiFunctionToSymbolInfo(Function & func, OsiSymbolInfo & symbol)
{
	symbol.name = func.Signature->Name;
	symbol.type = func.Type;
	symbol.nodeId = func.Node.Id;
	symbol.EoCFunctionId = 0;

	symbol.params.resize(func.Signature->Params->Params.Size);
	uint32_t paramIdx = 0;
	auto paramHead = func.Signature->Params->Params.Head;
	auto param = paramHead->Next;
	while (param != paramHead) {
		symbol.params[paramIdx].output = func.Signature->OutParamList.isOutParam(paramIdx);
		symbol.params[paramIdx++].type = (ValueType)param->Item.Type;
		param = param->Next;
	}
}

void CustomFunctionInjector::CreateOsirisSymbolMap(MappingInfo ** Mappings, uint32_t * MappingCount)
{
	// Create a map of Osiris symbols
	osiSymbols_.clear();
	osiSymbols_.reserve(10000);

	std::unordered_map<FunctionNameAndArity, uint32_t> symbolMap;
	auto funcs = *gOsirisProxy->GetGlobals().Functions;
	auto visit = [&symbolMap, this](STDString const & str, Function * func) {
		OsiSymbolInfo symbol;
		OsiFunctionToSymbolInfo(*func, symbol);

		FunctionNameAndArity sig{ symbol.name, (uint32_t)symbol.params.size() };
		symbolMap.insert(std::make_pair(sig, (uint32_t)osiSymbols_.size()));

		osiSymbols_.push_back(std::move(symbol));
	};

	for (auto i = 0; i < 0x3ff; i++) {
		auto map = funcs->Hash[i].NodeMap;
		map.Iterate(visit);
	}

	// Map EoC function indices to Osiris functions
	for (unsigned i = 0; i < *MappingCount; i++) {
		auto const & mapping = (*Mappings)[i];
		FunctionNameAndArity sig{ mapping.Name, mapping.NumParams };
		auto it = symbolMap.find(sig);
		if (it != symbolMap.end()) {
			osiSymbols_[it->second].EoCFunctionId = mapping.Id;
		} else {
			Debug("No Osiris definition found for engine function %s/%d", mapping.Name, mapping.NumParams);
		}
	}
}

void CustomFunctionInjector::OnAfterGetFunctionMappings(void * Osiris, MappingInfo ** Mappings, uint32_t * MappingCount)
{
	CreateOsirisSymbolMap(Mappings, MappingCount);
	gOsirisProxy->GetExtensionState().Reset();

	// Remove local functions
	auto outputIndex = 0;
	osiToDivMappings_.clear();
	divToOsiMappings_.clear();
	for (unsigned i = 0; i < *MappingCount; i++) {
		auto const & mapping = (*Mappings)[i];
		FunctionNameAndArity sig{ mapping.Name, mapping.NumParams };
		auto mapped = functions_.Find(sig);
		if (mapped == nullptr) {
			(*Mappings)[outputIndex++] = mapping;
		} else {
			osiToDivMappings_.insert(std::make_pair(mapping.Id, mapped->Handle()));
			divToOsiMappings_.insert(std::make_pair(mapped->Handle(), mapping.Id));
#if 0
			Debug("Function mapping (%s): %08x --> %08x", mapping.Name, mapping.Id, (unsigned int)mapped->Handle());
#endif
		}
	}

	Debug("CustomFunctionInjector mapping phase: %d -> %d functions", *MappingCount, outputIndex);
	*MappingCount = outputIndex;

	ExtensionState::Get().StoryFunctionMappingsUpdated();
}

bool CustomFunctionInjector::CallWrapper(std::function<bool (uint32_t, OsiArgumentDesc *)> const & next, uint32_t handle, OsiArgumentDesc * params)
{
	auto it = osiToDivMappings_.find(handle);
	if (it != osiToDivMappings_.end()) {
		return functions_.Call(it->second, *params);
	} else {
		return next(handle, params);
	}
}

bool CustomFunctionInjector::QueryWrapper(std::function<bool(uint32_t, OsiArgumentDesc *)> const & next, uint32_t handle, OsiArgumentDesc * params)
{
	auto it = osiToDivMappings_.find(handle);
	if (it != osiToDivMappings_.end()) {
		return functions_.Query(it->second, *params);
	} else {
		return next(handle, params);
	}
}

void CustomFunctionInjector::ExtendStoryHeader(std::wstring const & headerPath)
{
	extendingStory_ = true;

	std::ifstream f(headerPath.c_str(), std::ios::in | std::ios::binary);
	f.seekg(0, std::ios::end);
	auto length = f.tellg();
	f.seekg(0, std::ios::beg);
	std::string s(length, '\0');
	f.read(const_cast<char *>(s.data()), length);
	f.close();

	auto headers = functions_.GenerateHeaders();
#if 0
	Debug("CustomFunctionInjector::ExtendStoryHeader(): Appending to header:\r\n");
	OutputDebugStringA(headers.c_str());
	std::cout << headers << std::endl;
#endif
	s += headers;

	std::ofstream wf(headerPath.c_str(), std::ios::out | std::ios::binary);
	wf.write(s.data(), s.size());
	wf.close();

	extendingStory_ = false;
}

void CustomFunctionInjector::OnCreateFile(LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile,
	HANDLE hFile)
{
	if (!extendingStory_ && (dwDesiredAccess & GENERIC_WRITE)) {
		auto length = wcslen(lpFileName);
		if (length > 16 && wcscmp(&lpFileName[length - 16], L"story_header.div") == 0) {
			Debug(L"CustomFunctionInjector::OnCreateFile: %s", lpFileName);
			storyHeaderFile_ = hFile;
			storyHeaderPath_ = lpFileName;
		}
	}
}

void CustomFunctionInjector::OnCloseHandle(HANDLE hFile, BOOL bSucceeded)
{
	if (bSucceeded && !extendingStory_ && storyHeaderFile_ != NULL && hFile == storyHeaderFile_) {
		if (ExtensionState::Get().EnableExtensions) {
			ExtendStoryHeader(storyHeaderPath_);
		}

		storyHeaderFile_ = NULL;
	}
}

}