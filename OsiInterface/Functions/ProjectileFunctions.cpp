#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	namespace func
	{
		std::unique_ptr<ShootProjectileApiHelper> ProjectileHelper;

		void ProjectileBegin(OsiArgumentDesc const & args)
		{
			if (ProjectileHelper) {
				OsiWarn("ProjectileBegin(): Destroying active ProjectileHelper?");
			}

			ProjectileHelper = std::make_unique<ShootProjectileApiHelper>();
		}

		void ProjectileEnd(OsiArgumentDesc const & args)
		{
			if (ProjectileHelper != nullptr)
			{
				ProjectileHelper->Shoot();
				ProjectileHelper.reset();
			}
			else
			{
				OsiError("ProjectileEnd(): No projectile to shoot!");
			}
		}

		void ProjectileSetInt(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).Int32;

			if (!ProjectileHelper) {
				OsiError("ProjectileSetInt(): Called when no projectile is active!");
				return;
			}

			ProjectileHelper->SetInt(prop, value);
		}

		void ProjectileSetString(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).String;

			if (!ProjectileHelper) {
				OsiError("ProjectileSetString(): Called when no projectile is active!");
				return;
			}

			ProjectileHelper->SetString(prop, value);
		}

		void ProjectileSetGuidString(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).String;

			if (!ProjectileHelper) {
				OsiError("ProjectileSetGuidString(): Called when no projectile is active!");
				return;
			}

			ProjectileHelper->SetGuidString(prop, value);
		}

		void ProjectileSetVector3(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			float vec[3];
			vec[0] = args.Get(1).Float;
			vec[1] = args.Get(2).Float;
			vec[2] = args.Get(3).Float;

			if (!ProjectileHelper) {
				OsiError("ProjectileSetVector3(): Called when no projectile is active!");
				return;
			}

			ProjectileHelper->SetVector(prop, vec);
		}
	}

	void CustomFunctionLibrary::RegisterProjectileFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto beginProjectile = std::make_unique<CustomCall>(
			"NRD_BeginProjectile",
			std::vector<CustomFunctionParam>{},
			&func::ProjectileBegin
		);
		functionMgr.Register(std::move(beginProjectile));

		auto endProjectile = std::make_unique<CustomCall>(
			"NRD_EndProjectile",
			std::vector<CustomFunctionParam>{},
			&func::ProjectileEnd
		);
		functionMgr.Register(std::move(endProjectile));

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
