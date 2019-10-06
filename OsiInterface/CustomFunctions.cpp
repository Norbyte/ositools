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
		Debug("Function %s/%d: Argument count mismatch", name_.c_str(), params_.size());
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

	if (typeId != param.Type) {
		Debug("Function %s/%d: Argument '%s' type mismatch; expected %d, got %d",
			name_.c_str(), params_.size(), param.Name.c_str(),
			param.Type, typeId);
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
		Debug("CustomFunctionManager::Call(): Cannot call %08x - not a custom function!", (uint32_t)handle);
		return false;
	}

	if (handle.functionIndex() >= calls_.size()) {
		Debug("CustomFunctionManager::Call(): Call index %d out of bounds!", handle.functionIndex());
		return false;
	}

	return calls_[handle.functionIndex()]->Call(params);
}

bool CustomFunctionManager::Query(FunctionHandle handle, OsiArgumentDesc & params)
{
	if (handle.classIndex() != QueryClassId) {
		Debug("CustomFunctionManager::Query(): Cannot query %08x - not a custom function!", (uint32_t)handle);
		return false;
	}

	if (handle.functionIndex() >= queries_.size()) {
		Debug("CustomFunctionManager::Query(): Query index %d out of bounds!", handle.functionIndex());
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

void CustomFunctionInjector::ThrowEvent(FunctionHandle handle, OsiArgumentDesc * args) const
{
	auto it = divToOsiMappings_.find(handle);
	if (it != divToOsiMappings_.end()) {
		auto osiris = gOsirisProxy->GetDynamicGlobals().OsirisObject;
		gOsirisProxy->GetWrappers().Event.CallOriginal(osiris, it->second, args);
	} else {
		Debug(L"CustomFunctionInjector::ThrowEvent(): Event handle not mapped: %08x", (unsigned)handle);
	}
}

void CustomFunctionInjector::OnAfterGetFunctionMappings(void * Osiris, MappingInfo ** Mappings, uint32_t * MappingCount)
{
	Debug(L"CustomFunctionInjector::OnAfterGetFunctionMappings(): No. funcs: %d", *MappingCount);

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
			Debug("Function mapping (%s): %08x --> %08x", mapping.Name, mapping.Id, (unsigned int)mapped->Handle());
		}
	}

	*MappingCount = outputIndex;
	Debug(L"CustomFunctionInjector::OnAfterGetFunctionMappings(): No. funcs after filtering: %d", *MappingCount);
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
	Debug("CustomFunctionInjector::ExtendStoryHeader(): Appending to header:\r\n");
	// TODO - if debug?
	OutputDebugStringA(headers.c_str());
	std::cout << headers << std::endl;
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
		ExtendStoryHeader(storyHeaderPath_);
		storyHeaderFile_ = NULL;
	}
}

}