#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	namespace func
	{
		bool StatExists(OsiArgumentDesc & args)
		{
			auto statName = args.Get(0).String;
			
			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->objects.Find(statName);
			return object != nullptr;
		}

		bool StatAttributeExists(OsiArgumentDesc & args)
		{
			auto statName = args.Get(0).String;
			auto attributeName = args.Get(1).String;

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->objects.Find(statName);
			if (object == nullptr) {
				return false;
			}

			int attributeIndex;
			auto attrInfo = stats->GetAttributeInfo(object, attributeName, attributeIndex);
			return attrInfo != nullptr;
		}

		bool GetStatAttributeInt(OsiArgumentDesc & args)
		{
			auto statName = args.Get(0).String;
			auto attributeName = args.Get(1).String;

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->objects.Find(statName);
			if (object == nullptr) {
				return false;
			}

			auto value = stats->GetAttributeInt(object, attributeName);
			if (!value) {
				return false;
			}

			args.Get(2).Int32 = *value;
			return true;
		}

		bool GetStatAttributeString(OsiArgumentDesc & args)
		{
			auto statName = args.Get(0).String;
			auto attributeName = args.Get(1).String;

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->objects.Find(statName);
			if (object == nullptr) {
				return false;
			}

			auto value = stats->GetAttributeFixedString(object, attributeName);
			if (!value) {
				return false;
			}

			args.Get(2).String = const_cast<char *>(value->Str);
			return true;
		}

		bool GetStatType(OsiArgumentDesc & args)
		{
			auto statsId = args.Get(0).String;

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->objects.Find(statsId);
			if (object == nullptr) {
				return false;
			}

			auto typeInfo = stats->GetTypeInfo(object);
			if (typeInfo == nullptr) {
				return false;
			}

			args.Get(1).String = const_cast<char *>(typeInfo->Name.Str);
			return true;
		}

		void SetStatAttributeInt(OsiArgumentDesc const & args)
		{
			auto statName = args.Get(0).String;
			auto attributeName = args.Get(1).String;

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return;
			}

			auto object = stats->objects.Find(statName);
			if (object == nullptr) {
				return;
			}

			stats->SetAttributeInt(object, attributeName, args.Get(2).Int32);
		}

		void SetStatAttributeString(OsiArgumentDesc const & args)
		{
			auto statName = args.Get(0).String;
			auto attributeName = args.Get(1).String;

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return;
			}

			auto object = stats->objects.Find(statName);
			if (object == nullptr) {
				return;
			}

			stats->SetAttributeString(object, attributeName, args.Get(2).String);
		}
	}

	void CustomFunctionLibrary::RegisterStatFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto statExists = std::make_unique<CustomQuery>(
			"NRD_StatExists",
			std::vector<CustomFunctionParam>{
				{ "StatsId", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::StatExists
		);
		functionMgr.Register(std::move(statExists));

		auto statAttributeExists = std::make_unique<CustomQuery>(
			"NRD_StatAttributeExists",
			std::vector<CustomFunctionParam>{
				{ "StatsId", ValueType::String, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::StatAttributeExists
		);
		functionMgr.Register(std::move(statAttributeExists));

		auto getStatAttributeInt = std::make_unique<CustomQuery>(
			"NRD_GetStatAttributeInt",
			std::vector<CustomFunctionParam>{
				{ "StatsId", ValueType::String, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::GetStatAttributeInt
		);
		functionMgr.Register(std::move(getStatAttributeInt));

		auto getStatAttributeString = std::make_unique<CustomQuery>(
			"NRD_GetStatAttributeString",
			std::vector<CustomFunctionParam>{
				{ "StatsId", ValueType::String, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::GetStatAttributeString
		);
		functionMgr.Register(std::move(getStatAttributeString));

		auto getStatType = std::make_unique<CustomQuery>(
			"NRD_GetStatType",
			std::vector<CustomFunctionParam>{
				{ "StatsId", ValueType::String, FunctionArgumentDirection::In },
				{ "Type", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::GetStatType
		);
		functionMgr.Register(std::move(getStatType));

		auto setStatAttributeInt = std::make_unique<CustomCall>(
			"NRD_SetStatAttributeInt",
			std::vector<CustomFunctionParam>{
				{ "StatsId", ValueType::String, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }, 
			},
			&func::SetStatAttributeInt
		);
		functionMgr.Register(std::move(setStatAttributeInt));

		auto setStatAttributeString = std::make_unique<CustomCall>(
			"NRD_SetStatAttributeString",
			std::vector<CustomFunctionParam>{
				{ "StatsId", ValueType::String, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::SetStatAttributeString
		);
		functionMgr.Register(std::move(setStatAttributeString));
	}

}
