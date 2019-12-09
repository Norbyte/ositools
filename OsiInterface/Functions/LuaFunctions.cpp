#include <stdafx.h>
#include <OsirisProxy.h>
#include "FunctionLibrary.h"
#include <Version.h>

namespace osidbg
{
	namespace func
	{
		void OsiLuaReset(OsiArgumentDesc const & args)
		{
			auto bootstrapMods = args[0].Int32 == 1;

			auto & ext = ExtensionState::Get();
			ext.LuaReset(bootstrapMods);
		}

		void OsiLuaLoad(OsiArgumentDesc const & args)
		{
			LuaStatePin lua(ExtensionState::Get());
			if (!lua) {
				OsiErrorS("Called when the Lua VM has not been initialized!");
				return;
			}

			auto mod = args[0].String;
			auto fileName = args[1].String;

			if (strstr(fileName, "..") != nullptr) {
				OsiErrorS("Illegal file name");
				return;
			}

			ExtensionState::Get().LuaLoadGameFile(mod, fileName);
		}

		void OsiLuaCall(OsiArgumentDesc const & args)
		{
			LuaStatePin lua(ExtensionState::Get());
			if (!lua) {
				OsiErrorS("Called when the Lua VM has not been initialized!");
				return;
			}

			auto func = args[0].String;
			auto numArgs = args.Count() - 1;
			std::vector<OsiArgumentValue> luaArgs;
			luaArgs.resize(numArgs);

			for (uint32_t i = 0; i < numArgs; i++) {
				luaArgs[i] = args[i + 1];
			}

			lua->Call(func, luaArgs);
		}

		char const * QueryArgNames[3] = {
			"Arg1",
			"Arg2",
			"Arg3"
		};

		char const * QueryOutArgNames[3] = {
			"Out1",
			"Out2",
			"Out3"
		};

		template <uint32_t TInParams>
		bool OsiLuaQuery(OsiArgumentDesc & args)
		{
			LuaStatePin lua(ExtensionState::Get());
			if (!lua) {
				OsiErrorS("Called when the Lua VM has not been initialized!");
				return false;
			}

			auto func = args[0].String;
			auto numArgs = args.Count() - 1;

			std::vector<CustomFunctionParam> signature;
			signature.reserve(numArgs);
			for (uint32_t i = 0; i < TInParams; i++) {
				signature.push_back(CustomFunctionParam{
					QueryArgNames[i], args[i + 1].TypeId, FunctionArgumentDirection::In
				});
			}

			for (uint32_t i = 0; i < numArgs - TInParams; i++) {
				signature.push_back(CustomFunctionParam{
					QueryOutArgNames[i], args[i + TInParams + 1].TypeId, FunctionArgumentDirection::Out
				});
			}

			return lua->Query(func, nullptr, signature, *args.NextParam);
		}
	}


	void CustomFunctionLibrary::RegisterLuaFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();
		auto luaReset = std::make_unique<CustomCall>(
			"NRD_LuaReset",
			std::vector<CustomFunctionParam>{
				{ "BootstrapMods", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::OsiLuaReset
		);
		functionMgr.Register(std::move(luaReset));

		auto luaLoad = std::make_unique<CustomCall>(
			"NRD_LuaLoad",
			std::vector<CustomFunctionParam>{
				{ "ModNameGuid", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "FileName", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::OsiLuaLoad
		);
		functionMgr.Register(std::move(luaLoad));

		auto luaCall0 = std::make_unique<CustomCall>(
			"NRD_LuaCall",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::OsiLuaCall
		);
		functionMgr.Register(std::move(luaCall0));

		auto luaCall1 = std::make_unique<CustomCall>(
			"NRD_LuaCall",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In }
			},
			&func::OsiLuaCall
		);
		functionMgr.Register(std::move(luaCall1));

		auto luaCall2 = std::make_unique<CustomCall>(
			"NRD_LuaCall",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In }
			},
			&func::OsiLuaCall
		);
		functionMgr.Register(std::move(luaCall2));
		
		auto luaCall3 = std::make_unique<CustomCall>(
			"NRD_LuaCall",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg3", ValueType::None, FunctionArgumentDirection::In }
			},
			&func::OsiLuaCall
		);
		functionMgr.Register(std::move(luaCall3));
		
		auto luaCall4 = std::make_unique<CustomCall>(
			"NRD_LuaCall",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg3", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg4", ValueType::None, FunctionArgumentDirection::In }
			},
			&func::OsiLuaCall
		);
		functionMgr.Register(std::move(luaCall4));
		
		auto luaCall5 = std::make_unique<CustomCall>(
			"NRD_LuaCall",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg3", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg4", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg5", ValueType::None, FunctionArgumentDirection::In }
			},
			&func::OsiLuaCall
		);
		functionMgr.Register(std::move(luaCall5));

		auto luaQuery00 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery0",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::OsiLuaQuery<0>
		);
		functionMgr.Register(std::move(luaQuery00));

		auto luaQuery01 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery0",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Out1", ValueType::None, FunctionArgumentDirection::Out }
			},
			&func::OsiLuaQuery<0>
		);
		functionMgr.Register(std::move(luaQuery01));

		auto luaQuery11 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery1",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Out1", ValueType::None, FunctionArgumentDirection::Out }
			},
			&func::OsiLuaQuery<1>
		);
		functionMgr.Register(std::move(luaQuery11));

		auto luaQuery20 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery2",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In }
			},
			&func::OsiLuaQuery<2>
		);
		functionMgr.Register(std::move(luaQuery20));

		auto luaQuery21 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery2",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In },
				{ "Out1", ValueType::None, FunctionArgumentDirection::Out }
			},
			&func::OsiLuaQuery<2>
		);
		functionMgr.Register(std::move(luaQuery21));

		auto luaQuery22 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery2",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In },
				{ "Out1", ValueType::None, FunctionArgumentDirection::Out },
				{ "Out2", ValueType::None, FunctionArgumentDirection::Out }
			},
			&func::OsiLuaQuery<2>
		);
		functionMgr.Register(std::move(luaQuery22));

		auto luaQuery31 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery3",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg3", ValueType::None, FunctionArgumentDirection::In },
				{ "Out1", ValueType::None, FunctionArgumentDirection::Out }
			},
			&func::OsiLuaQuery<3>
		);
		functionMgr.Register(std::move(luaQuery31));

		auto luaQuery41 = std::make_unique<CustomQuery>(
			"NRD_LuaQuery4",
			std::vector<CustomFunctionParam>{
				{ "Func", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg1", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg3", ValueType::None, FunctionArgumentDirection::In },
				{ "Arg4", ValueType::None, FunctionArgumentDirection::In },
				{ "Out1", ValueType::None, FunctionArgumentDirection::Out }
			},
			&func::OsiLuaQuery<4>
		);
		functionMgr.Register(std::move(luaQuery41));
	}
}
