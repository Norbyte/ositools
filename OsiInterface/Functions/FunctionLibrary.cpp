#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	namespace func
	{
		bool ItemGetStatsId(OsiArgumentDesc & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			if (item == nullptr) {
				return false;
			}

			if (!item->StatsId.Str) {
				return false;
			}
			else {
				args.Get(1).String = const_cast<char *>(item->StatsId.Str);
				return true;
			}
			
		}

		void DoExperiment(OsiArgumentDesc const & args)
		{
			OsiError("Nothing to see here");
		}
	}

	CustomFunctionLibrary::CustomFunctionLibrary(class OsirisProxy & osiris)
		: osiris_(osiris)
	{}

	void CustomFunctionLibrary::Register()
	{
		RegisterHelperFunctions();
		RegisterStatFunctions();
		RegisterStatusFunctions();
		RegisterGameActionFunctions();
		RegisterProjectileFunctions();

		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto itemGetStatsId = std::make_unique<CustomQuery>(
			"NRD_ItemGetStatsId",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatsId", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::ItemGetStatsId
		);
		functionMgr.Register(std::move(itemGetStatsId));


		auto experiment = std::make_unique<CustomCall>(
			"NRD_Experiment",
			std::vector<CustomFunctionParam>{
				{ "Experiment", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::DoExperiment
		);
		functionMgr.Register(std::move(experiment));
	}

}
