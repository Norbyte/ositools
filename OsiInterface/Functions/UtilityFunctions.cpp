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
			auto msg = args[0].ToString();
			OsiMsg(msg);
		}

		bool StringCompare(OsiArgumentDesc & args)
		{
			auto a = args[0].String;
			auto b = args[1].String;

			int rel = strcmp(a, b);
			if (rel < 0) {
				args[2].Int32 = -1;
			}
			else if (rel == 0) {
				args[2].Int32 = 0;
			}
			else {
				args[2].Int32 = 1;
			}

			return true;
		}

		bool StringLength(OsiArgumentDesc & args)
		{
			auto str = args[0].String;
			args[1].Int32 = (int32_t)strlen(str);
			return true;
		}

		bool StringToInt(OsiArgumentDesc & args)
		{
			auto a = args[0].String;
			try {
				args[1].Int32 = std::stoi(a);
				return true;
			}
			catch (std::invalid_argument &) {
				return false;
			}
		}

		bool StringToReal(OsiArgumentDesc & args)
		{
			auto a = args[0].String;
			try {
				args[1].Float = std::stof(a);
				return true;
			}
			catch (std::invalid_argument &) {
				return false;
			}
		}

		bool IsHexChar(char c)
		{
			return (c >= '0' && c <= '9')
				|| (c >= 'a' && c <= 'f');
		}

		bool IsValidGuidString(std::string const & s)
		{
			if (s.length() < 36) return false;

			auto guidPos = s.length() - 36;
			unsigned const nibbles[5] = { 8, 4, 4, 4, 12 };

			for (auto n = 0; n < 5; n++) {
				for (unsigned i = 0; i < nibbles[n]; i++) {
					if (!IsHexChar(s[guidPos++])) return false;
				}

				if (n != 4 && s[guidPos++] != '-') return false;
			}

			return true;
		}

		bool StringToGuidString(OsiArgumentDesc & args)
		{
			if (IsValidGuidString(args[0].String)) {
				args[1].String = args[0].String;
				return true;
			} else {
				return false;
			}
		}

		// We need to keep the returned string alive
		// until it reaches Osi
		std::string gRealToStringTemp;

		bool RealToString(OsiArgumentDesc & args)
		{
			gRealToStringTemp = std::to_string(args[0].Float);
			args[1].String = gRealToStringTemp.c_str();
			return true;
		}

		void ForLoop(OsiArgumentDesc const & args)
		{
			auto eventName = args[0].String;
			auto count = args[1].Int32;

			for (int32_t index = 0; index < count; index++) {
				auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
				eventArgs->Add(OsiArgumentValue{ (int64_t)index });

				gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(ForLoopEventHandle, eventArgs);

				delete eventArgs;
			}
		}

		void ForLoopObject(OsiArgumentDesc const & args)
		{
			auto objectGuid = args[0].String;
			auto eventName = args[1].String;
			auto count = args[2].Int32;

			for (int32_t index = 0; index < count; index++) {
				auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, objectGuid });
				eventArgs->Add(OsiArgumentValue{ ValueType::String, eventName });
				eventArgs->Add(OsiArgumentValue{ (int64_t)index });

				gOsirisProxy->GetCustomFunctionInjector().ThrowEvent(ForLoopObjectEventHandle, eventArgs);

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

		auto stringToReal = std::make_unique<CustomQuery>(
			"NRD_StringToReal",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::StringToReal
		);
		functionMgr.Register(std::move(stringToReal));

		auto stringToGuidString = std::make_unique<CustomQuery>(
			"NRD_GuidString",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::GuidString, FunctionArgumentDirection::Out }
			},
			&func::StringToGuidString
		);
		functionMgr.Register(std::move(stringToGuidString));

		auto realToString = std::make_unique<CustomQuery>(
			"NRD_RealToString",
			std::vector<CustomFunctionParam>{
				{ "Real", ValueType::Real, FunctionArgumentDirection::In },
				{ "Result", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::RealToString
		);
		functionMgr.Register(std::move(realToString));

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
