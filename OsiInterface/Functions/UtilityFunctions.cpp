#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include <random>

namespace osidbg
{
	FunctionHandle ForLoopEventHandle;
	FunctionHandle ForLoopObjectEventHandle;

	namespace func
	{
		void DebugLog(OsiArgumentDesc const & args)
		{
			auto msg = args.Get(0).ToString();
			OsiMsg(msg);
		}

		bool StringCompare(OsiArgumentDesc & args)
		{
			auto a = args.Get(0).String;
			auto b = args.Get(1).String;

			int rel = strcmp(a, b);
			if (rel < 0) {
				args.Get(2).Int32 = -1;
			}
			else if (rel == 0) {
				args.Get(2).Int32 = 0;
			}
			else {
				args.Get(2).Int32 = 1;
			}

			return true;
		}

		bool StringLength(OsiArgumentDesc & args)
		{
			auto str = args.Get(0).String;
			args.Get(1).Int32 = (int32_t)strlen(str);
			return true;
		}

		bool StringToInt(OsiArgumentDesc & args)
		{
			auto a = args.Get(0).String;
			try {
				args.Get(1).Int32 = std::stoi(a);
				return true;
			}
			catch (std::invalid_argument &) {
				return false;
			}
		}

		bool StringToFloat(OsiArgumentDesc & args)
		{
			auto a = args.Get(0).String;
			try {
				args.Get(1).Float = std::stof(a);
				return true;
			}
			catch (std::invalid_argument &) {
				return false;
			}
		}

		void ForLoop(OsiArgumentDesc const & args)
		{
			auto eventName = args.Get(0).String;
			auto count = args.Get(1).Int32;

			for (int32_t index = 0; index < count; index++) {
				auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
				eventArgs->Add(OsiArgumentValue{ (int64_t)index });

				auto osiris = gOsirisProxy->GetDynamicGlobals().OsirisObject;
				gOsirisProxy->GetWrappers().Event.CallOriginal(osiris, (uint32_t)ForLoopEventHandle, eventArgs);

				delete eventArgs;
			}
		}

		void ForLoopObject(OsiArgumentDesc const & args)
		{
			auto objectGuid = args.Get(0).String;
			auto eventName = args.Get(1).String;
			auto count = args.Get(2).Int32;

			for (int32_t index = 0; index < count; index++) {
				auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, objectGuid });
				eventArgs->Add(OsiArgumentValue{ ValueType::String, eventName });
				eventArgs->Add(OsiArgumentValue{ (int64_t)index });

				auto osiris = gOsirisProxy->GetDynamicGlobals().OsirisObject;
				gOsirisProxy->GetWrappers().Event.CallOriginal(osiris, (uint32_t)ForLoopObjectEventHandle, eventArgs);

				delete eventArgs;
			}
		}
	}

	void CustomFunctionLibrary::RegisterHelperFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto debugLog = std::make_unique<CustomCall>(
			"NRD_DebugLog",
			std::vector<CustomFunctionParam>{ { "Message", ValueType::None, FunctionArgumentDirection::In } },
			&func::DebugLog
		);
		functionMgr.Register(std::move(debugLog));

		auto stringCompare = std::make_unique<CustomQuery>(
			"NRD_StringCompare",
			std::vector<CustomFunctionParam>{
				{ "A", ValueType::String, FunctionArgumentDirection::In },
				{ "B", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::StringCompare
		);
		functionMgr.Register(std::move(stringCompare));

		auto stringLength = std::make_unique<CustomQuery>(
			"NRD_StringLength",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Length", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::StringLength
		);
		functionMgr.Register(std::move(stringLength));

		auto stringToInt = std::make_unique<CustomQuery>(
			"NRD_StringToInt",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::StringToInt
		);
		functionMgr.Register(std::move(stringToInt));

		auto stringToFloat = std::make_unique<CustomQuery>(
			"NRD_StringToFloat",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::StringToFloat
		);
		functionMgr.Register(std::move(stringToFloat));

		auto startLoop = std::make_unique<CustomCall>(
			"NRD_ForLoop",
			std::vector<CustomFunctionParam>{ 
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Count", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ForLoop
		);
		functionMgr.Register(std::move(startLoop));

		auto startLoop2 = std::make_unique<CustomCall>(
			"NRD_ForLoop",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Count", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::ForLoopObject
		);
		functionMgr.Register(std::move(startLoop2));

		auto forLoopEvent = std::make_unique<CustomEvent>(
			"NRD_Loop",
			std::vector<CustomFunctionParam>{
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Num", ValueType::Integer, FunctionArgumentDirection::In },
			}
		);
		ForLoopEventHandle = functionMgr.Register(std::move(forLoopEvent));

		auto forLoopObjectEvent = std::make_unique<CustomEvent>(
			"NRD_Loop",
			std::vector<CustomFunctionParam>{
				{ "Object", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Num", ValueType::Integer, FunctionArgumentDirection::In },
			}
		);
		ForLoopObjectEventHandle = functionMgr.Register(std::move(forLoopObjectEvent));
	}

}
