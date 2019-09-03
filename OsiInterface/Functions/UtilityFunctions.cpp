#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include <random>

namespace osidbg
{
	namespace func
	{
		void Log(OsiArgumentDesc const & args)
		{
			auto msg = args.Get(0).ToString();
			Debug("[Osiris] %s", msg.c_str());
			gOsirisProxy->GetWrappers().AssertOriginal(false, msg.c_str(), false);
		}

		std::unique_ptr<std::mt19937_64> OsiRng;

		bool RandomFloat(OsiArgumentDesc & args)
		{
			auto min = args.Get(0).Float;
			auto max = args.Get(1).Float;

			if (!OsiRng) {
				OsiRng = std::make_unique<std::mt19937_64>();
				time_t tm;
				OsiRng->seed(time(&tm));
			}

			std::uniform_real_distribution<float> dist(min, max);
			args.Get(2).Float = dist(*OsiRng);
			return true;
		}

		bool Sin(OsiArgumentDesc & args)
		{
			auto x = args.Get(0).Float;
			args.Get(1).Float = sin(x);
			return true;
		}

		bool Cos(OsiArgumentDesc & args)
		{
			auto x = args.Get(0).Float;
			args.Get(1).Float = cos(x);
			return true;
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
	}

	void CustomFunctionLibrary::RegisterHelperFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto debugLog = std::make_unique<CustomCall>(
			"NRD_DebugLog",
			std::vector<CustomFunctionParam>{ { "Message", ValueType::None, FunctionArgumentDirection::In } },
			&func::Log
		);
		functionMgr.Register(std::move(debugLog));

		auto randomFloat = std::make_unique<CustomQuery>(
			"NRD_RandomFloat",
			std::vector<CustomFunctionParam>{
				{ "Min", ValueType::Real, FunctionArgumentDirection::In },
				{ "Max", ValueType::Real, FunctionArgumentDirection::In },
				{ "Result", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::RandomFloat
		);
		functionMgr.Register(std::move(randomFloat));

		auto sine = std::make_unique<CustomQuery>(
			"NRD_Sin",
			std::vector<CustomFunctionParam>{
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Sine", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::Sin
		);
		functionMgr.Register(std::move(sine));

		auto cosine = std::make_unique<CustomQuery>(
			"NRD_Cos",
			std::vector<CustomFunctionParam>{
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Cosine", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::Cos
		);
		functionMgr.Register(std::move(cosine));

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
	}

}
