#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	namespace func
	{
		void ProjectilePrepareLaunch(OsiArgumentDesc const & args)
		{
			if (ExtensionState::Get().ProjectileHelper) {
				OsiWarn("Destroying active ProjectileHelper?");
			}

			ExtensionState::Get().ProjectileHelper = std::make_unique<ShootProjectileApiHelper>();
		}

		void ProjectileLaunch(OsiArgumentDesc const & args)
		{
			auto & helper = ExtensionState::Get().ProjectileHelper;
			if (helper != nullptr)
			{
				helper->Shoot();
				helper.reset();
			}
			else
			{
				OsiErrorS("No projectile to shoot!");
			}
		}

		void ProjectileSetInt(OsiArgumentDesc const & args)
		{
			auto prop = args[0].String;
			auto value = args[1].Int32;

			auto & helper = ExtensionState::Get().ProjectileHelper;
			if (!helper) {
				OsiErrorS("Called when no projectile is active!");
				return;
			}

			helper->SetInt(prop, value);
		}

		void ProjectileSetString(OsiArgumentDesc const & args)
		{
			auto prop = args[0].String;
			auto value = args[1].String;

			auto & helper = ExtensionState::Get().ProjectileHelper;
			if (!helper) {
				OsiErrorS("Called when no projectile is active!");
				return;
			}

			helper->SetString(prop, value);
		}

		void ProjectileSetGuidString(OsiArgumentDesc const & args)
		{
			auto prop = args[0].String;
			auto value = args[1].String;

			auto & helper = ExtensionState::Get().ProjectileHelper;
			if (!helper) {
				OsiErrorS("Called when no projectile is active!");
				return;
			}

			helper->SetGuidString(prop, value);
		}

		void ProjectileSetVector3(OsiArgumentDesc const & args)
		{
			auto prop = args[0].String;
			glm::vec3 vec = args.GetVector(1);

			auto & helper = ExtensionState::Get().ProjectileHelper;
			if (!helper) {
				OsiErrorS("Called when no projectile is active!");
				return;
			}

			helper->SetVector(prop, vec);
		}
	}

	void CustomFunctionLibrary::RegisterProjectileFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto projectilePrepareLaunch = std::make_unique<CustomCall>(
			"NRD_ProjectilePrepareLaunch",
			std::vector<CustomFunctionParam>{},
			&func::ProjectilePrepareLaunch
		);
		functionMgr.Register(std::move(projectilePrepareLaunch));

		auto projectileLaunch = std::make_unique<CustomCall>(
			"NRD_ProjectileLaunch",
			std::vector<CustomFunctionParam>{},
			&func::ProjectileLaunch
		);
		functionMgr.Register(std::move(projectileLaunch));

		auto projectileSetInt = std::make_unique<CustomCall>(
			"NRD_ProjectileSetInt",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::ProjectileSetInt
		);
		functionMgr.Register(std::move(projectileSetInt));

		auto projectileSetString = std::make_unique<CustomCall>(
			"NRD_ProjectileSetString",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::ProjectileSetString
		);
		functionMgr.Register(std::move(projectileSetString));

		auto projectileSetVector3 = std::make_unique<CustomCall>(
			"NRD_ProjectileSetVector3",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In }
			},
			&func::ProjectileSetVector3
		);
		functionMgr.Register(std::move(projectileSetVector3));

		auto projectileSetGuidString = std::make_unique<CustomCall>(
			"NRD_ProjectileSetGuidString",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::ProjectileSetGuidString
		);
		functionMgr.Register(std::move(projectileSetGuidString));
	}

}
