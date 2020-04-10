#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace dse::esv
{
	namespace func
	{
		bool RandomReal(OsiArgumentDesc & args)
		{
			auto min = args[0].Float;
			auto max = args[1].Float;

			std::uniform_real_distribution<float> dist(min, max);
			args[2].Set(dist(ExtensionState::Get().OsiRng));
			return true;
		}

		bool Sin(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(sin(x));
			return true;
		}

		bool Cos(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(cos(x));
			return true;
		}

		bool Tan(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(tan(x));
			return true;
		}

		bool Round(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(round(x));
			return true;
		}

		bool Ceil(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(ceil(x));
			return true;
		}

		bool Floor(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(floor(x));
			return true;
		}

		bool Abs(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(abs(x));
			return true;
		}

		bool Pow(OsiArgumentDesc & args)
		{
			auto base = args[0].Float;
			auto exp = args[1].Float;
			args[2].Set(pow(base, exp));
			return true;
		}

		bool Sqrt(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(sqrt(x));
			return true;
		}

		bool Exp(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(exp(x));
			return true;
		}

		bool Factorial(OsiArgumentDesc & args)
		{
			auto n = args[0].Int32;
			int32_t fact = 0;
			for (int32_t i = 1; i <= n; i++) {
				fact *= i;
			}

			args[1].Set(fact);
			return true;
		}

		bool Log(OsiArgumentDesc & args)
		{
			auto x = args[0].Float;
			args[1].Set(log(x));
			return true;
		}
		
		bool IsDivisible(OsiArgumentDesc & args)
		{
			auto n = args[0].Int32;
			auto d = args[1].Int32;

			if (d != 0 && n%d == 0) {
				args[2].Set(1);
			} else {
				args[2].Set(0);
			}

			return true;
		}
	}

#define MATH_QUERY1(funcName) { \
	auto fn = std::make_unique<CustomQuery>( \
		"NRD_" #funcName, \
			std::vector<CustomFunctionParam>{ \
				{ "In", ValueType::Real, FunctionArgumentDirection::In }, \
				{ "Out", ValueType::Real, FunctionArgumentDirection::Out } \
		}, \
		&func::funcName \
	); \
	functionMgr.Register(std::move(fn)); \
}

	void CustomFunctionLibrary::RegisterMathFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto randomReal = std::make_unique<CustomQuery>(
			"NRD_RandomReal",
			std::vector<CustomFunctionParam>{
				{ "Min", ValueType::Real, FunctionArgumentDirection::In },
				{ "Max", ValueType::Real, FunctionArgumentDirection::In },
				{ "Result", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::RandomReal
		);
		functionMgr.Register(std::move(randomReal));

		auto factorial = std::make_unique<CustomQuery>(
			"NRD_Factorial",
			std::vector<CustomFunctionParam>{
				{ "In", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Out", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::Factorial
		);
		functionMgr.Register(std::move(factorial));

		auto pow = std::make_unique<CustomQuery>(
			"NRD_Pow",
			std::vector<CustomFunctionParam>{
				{ "Base", ValueType::Real, FunctionArgumentDirection::In },
				{ "Exp", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Out", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::Pow
		);
		functionMgr.Register(std::move(pow));

		MATH_QUERY1(Sin)
		MATH_QUERY1(Cos)
		MATH_QUERY1(Tan)
		MATH_QUERY1(Round)
		MATH_QUERY1(Ceil)
		MATH_QUERY1(Floor)
		MATH_QUERY1(Abs)
		MATH_QUERY1(Sqrt)
		MATH_QUERY1(Exp)
		MATH_QUERY1(Log)

		auto isDivisible = std::make_unique<CustomQuery>(
			"NRD_IsDivisible",
			std::vector<CustomFunctionParam>{
				{ "Numerator", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Denominator", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Divisible", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::IsDivisible
		);
		functionMgr.Register(std::move(isDivisible));
	}

}
