#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse::esv
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

		template <OsiPropertyMapType Type>
		void ProjectileSet(OsiArgumentDesc const & args)
		{
			auto prop = args[0].String;
			auto value = args[1].Int32;

			auto & helper = ExtensionState::Get().ProjectileHelper;
			if (!helper) {
				OsiErrorS("Called when no projectile is active!");
				return;
			}

			if (strcmp(prop, "AlwaysDamage") == 0) {
				helper->AlwaysDamage = value != 0;
			} else if(strcmp(prop, "CanDeflect") == 0) {
				helper->CanDeflect = value != 0;
			} else {
				OsirisPropertyMapSet(gShootProjectileHelperPropertyMap, &helper->Helper, args, 0, Type);
			}
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

			helper->SetGuidString(ToFixedString(prop), value);
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

			helper->SetVector(ToFixedString(prop), vec);
		}

		void ProjectileAddDamage(OsiArgumentDesc const & args)
		{
			auto damageType = args[0].String;
			auto amount = args[1].Int32;

			auto & helper = ExtensionState::Get().ProjectileHelper;
			if (!helper) {
				OsiErrorS("Called when no projectile is active!");
				return;
			}

			auto damageTypeLbl = EnumInfo<DamageType>::Find(damageType);
			if (!damageTypeLbl) {
				OsiError("Unknown DamageType: " << damageType);
				return;
			}

			helper->AddDamage(*damageTypeLbl, amount);
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
			&func::ProjectileSet<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(projectileSetInt));

		auto projectileSetString = std::make_unique<CustomCall>(
			"NRD_ProjectileSetString",
			std::vector<CustomFunctionParam>{
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::ProjectileSet<OsiPropertyMapType::String>
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
			
		auto projectileAddDamage = std::make_unique<CustomCall>(
			"NRD_ProjectileAddDamage",
			std::vector<CustomFunctionParam>{
				{ "DamageType", ValueType::String, FunctionArgumentDirection::In },
				{ "Amount", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::ProjectileAddDamage
		);
		functionMgr.Register(std::move(projectileAddDamage));
	}

}
