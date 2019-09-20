#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	namespace func
	{
		template <class TAction>
		TAction * PrepareAction(OsiArgumentDesc const & args, EsvGameAction::ActionType type)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("PrepareAction(): Character '" << args.Get(0).String << "' does not exist!");
				return nullptr;
			}

			auto skillId = ToFixedString(args.Get(1).String);
			if (!skillId) {
				OsiError("PrepareAction(): '" << args.Get(1).String << "' is not a valid FixedString!");
				return nullptr;
			}

			auto x = args.Get(2).Float;
			auto y = args.Get(3).Float;
			auto z = args.Get(4).Float;

			if (x == -1.0f && y == -1.0f && z == -1.0f) {
				x = character->WorldPos[0];
				y = character->WorldPos[1];
				z = character->WorldPos[2];
			}

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			auto action = (TAction *)lib.CreateGameAction(actionMgr, type, 0);

			action->SkillId = skillId;
			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			action->OwnerHandle = characterHandle;

			action->Position[0] = x;
			action->Position[1] = y;
			action->Position[2] = z;
			return action;
		}

		bool CreateRain(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvRainAction>(args, EsvGameAction::RainAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);

			args.Get(5).Int64 = action->MyHandle;
			return true;
		}

		bool CreateStorm(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvStormAction>(args, EsvGameAction::StormAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);

			args.Get(5).Int64 = action->MyHandle;
			return true;
		}

		bool CreateWall(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("CreateWall(): Character '" << args.Get(0).String << "' does not exist!");
				return false;
			}

			auto skillId = ToFixedString(args.Get(1).String);
			if (!skillId) {
				OsiError("CreateWall(): '" << args.Get(1).String << "' is not a valid FixedString!");
				return false;
			}

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->objects.Find(args.Get(1).String);
			if (object == nullptr) {
				OsiError("CreateWall(): No such skill entry: '" << args.Get(1).String << "'");
				return false;
			}

			auto lifetime = stats->GetAttributeInt(object, "Lifetime");
			if (!lifetime) {
				OsiError("CreateWall(): Couldn't fetch lifetime of skill '" << args.Get(1).String << "'");
				return false;
			}

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			auto action = (EsvWallAction *)lib.CreateGameAction(actionMgr, EsvGameAction::WallAction, 0);

			action->SkillId = skillId;
			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			action->OwnerHandle = characterHandle;

			action->IsFromItem = false;
			action->LifeTime = *lifetime / 1000.0f;

			action->Source[0] = args.Get(2).Float;
			action->Source[1] = args.Get(3).Float;
			action->Source[2] = args.Get(4).Float;

			action->Target[0] = args.Get(5).Float;
			action->Target[1] = args.Get(6).Float;
			action->Target[2] = args.Get(7).Float;

			lib.WallActionCreateWall(action);
			lib.AddGameAction(actionMgr, action);

			args.Get(8).Int64 = action->MyHandle;
			return true;
		}

		bool CreateTornado(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvTornadoAction>(args, EsvGameAction::TornadoAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			action->Target[0] = args.Get(5).Float;
			action->Target[1] = args.Get(6).Float;
			action->Target[2] = args.Get(7).Float;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.TornadoActionSetup(action);
			lib.AddGameAction(actionMgr, action);

			args.Get(8).Int64 = action->MyHandle;
			return true;
		}

		bool CreateDome(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvStatusDomeAction>(args, EsvGameAction::StatusDomeAction);
			if (action == nullptr) return false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);

			args.Get(5).Int64 = action->MyHandle;
			return true;
		}

		bool CreateGameObjectMove(OsiArgumentDesc & args)
		{
			ObjectHandle objectHandle;

			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character != nullptr) {
				character->GetObjectHandle(&objectHandle);
			} else {
				auto item = FindItemByNameGuid(args.Get(0).String);
				if (item != nullptr) {
					item->GetObjectHandle(&objectHandle);
				} else {
					OsiError("CreateGameObjectMove(): Game object '" << args.Get(0).String << "' does not exist!");
					return false;
				}
			}

			auto beamEffectName = args.Get(4).String;
			FixedString beamEffectFs;
			EsvCharacter * caster{ nullptr };
			if (beamEffectName != nullptr && strlen(beamEffectName) > 0) {
				beamEffectFs = ToFixedString(beamEffectName);

				if (!beamEffectFs) {
					OsiError("CreateGameObjectMove(): Beam effect is not a valid FixedString!");
					return false;
				}

				auto casterGuid = args.Get(5).String;
				caster = FindCharacterByNameGuid(casterGuid);
				if (caster == nullptr) {
					OsiError("CreateGameObjectMove(): Caster character '" << casterGuid << "' does not exist!");
					return false;
				}
			}

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			auto action = (EsvGameObjectMoveAction *)lib.CreateGameAction(actionMgr, EsvGameAction::GameObjectMoveAction, 0);

			float targetPosition[3];
			targetPosition[0] = args.Get(1).Float;
			targetPosition[1] = args.Get(2).Float;
			targetPosition[2] = args.Get(3).Float;

			if (caster != nullptr) {
				ObjectHandle casterHandle;
				caster->GetObjectHandle(&casterHandle);
				action->CasterCharacterHandle = casterHandle;
				action->BeamEffectName = beamEffectFs;
			}

			lib.GameObjectMoveActionSetup(action, objectHandle, targetPosition);
			lib.AddGameAction(actionMgr, action);

			args.Get(6).Int64 = action->MyHandle;
			return true;
		}

		bool Summon(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("Summon(): Character '" << args.Get(0).String << "' does not exist!");
				return false;
			}

			auto objectTemplate = NameGuidToFixedString(args.Get(1).String);
			if (!objectTemplate) {
				OsiError("Summon(): Template '" << args.Get(1).String << "' not in FixedString table!");
				return false;
			}

			auto x = args.Get(2).Float;
			auto y = args.Get(3).Float;
			auto z = args.Get(4).Float;
			auto lifetime = args.Get(5).Float;
			auto summonLevel = args.Get(6).Int32;
			auto isTotem = args.Get(7).Int32 != 0;
			auto mapToAiGrid = args.Get(8).Int32 != 0;

			if (x == -1.0f && y == -1.0f && z == -1.0f) {
				x = character->WorldPos[0];
				y = character->WorldPos[1];
				z = character->WorldPos[2];
			}

			SummonHelperResults results;
			SummonHelperSummonArgs summonArgs;

			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			summonArgs.OwnerCharacterHandle = characterHandle;
			summonArgs.GameObjectTemplateFS = objectTemplate;
			summonArgs.Level = *character->GetCurrentLevel();
			summonArgs.Position[0] = x;
			summonArgs.Position[1] = y;
			summonArgs.Position[2] = z;
			summonArgs.SummonLevel = summonLevel;
			summonArgs.Lifetime = lifetime;
			summonArgs.IsTotem = isTotem;
			summonArgs.MapToAiGrid = mapToAiGrid;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			lib.SummonHelpersSummon(&results, &summonArgs);
			if (results.SummonHandle) {
				FixedString guid;

				auto summonCharacter = FindCharacterByHandle(results.SummonHandle);
				if (summonCharacter != nullptr) {
					guid = *summonCharacter->GetGuid();
				}
				else {
					auto summonItem = FindItemByHandle(results.SummonHandle);
					if (summonItem != nullptr) {
						guid = *summonItem->GetGuid();
					}
				}

				if (guid) {
					args.Get(9).String = const_cast<char *>(guid.Str);
					return true;
				}
				else {
					Debug("Summon(): Summoned object is not a character or item!");
					return false;
				}
			}
			else {
				OsiError("Summon(): esv::SummonHelpers::Summon() call failed!");
				return false;
			}
		}
	}

	void CustomFunctionLibrary::RegisterGameActionFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto createRain = std::make_unique<CustomQuery>(
			"NRD_CreateRain",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameObjectHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateRain
		);
		functionMgr.Register(std::move(createRain));

		auto createStorm = std::make_unique<CustomQuery>(
			"NRD_CreateStorm",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameObjectHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateStorm
		);
		functionMgr.Register(std::move(createStorm));

		auto createWall = std::make_unique<CustomQuery>(
			"NRD_CreateWall",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "SourceX", ValueType::Real, FunctionArgumentDirection::In },
				{ "SourceY", ValueType::Real, FunctionArgumentDirection::In },
				{ "SourceZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetX", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetY", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameObjectHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateWall
		);
		functionMgr.Register(std::move(createWall));

		auto createTornado = std::make_unique<CustomQuery>(
			"NRD_CreateTornado",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "PositionX", ValueType::Real, FunctionArgumentDirection::In },
				{ "PositionY", ValueType::Real, FunctionArgumentDirection::In },
				{ "PositionZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetX", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetY", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameObjectHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateTornado
		);
		functionMgr.Register(std::move(createTornado));

		auto createDome = std::make_unique<CustomQuery>(
			"NRD_CreateDome",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameObjectHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateDome
		);
		functionMgr.Register(std::move(createDome));

		auto gameObjectMove = std::make_unique<CustomQuery>(
			"NRD_CreateGameObjectMove",
			std::vector<CustomFunctionParam>{
				{ "TargetCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "BeamEffectName", ValueType::String, FunctionArgumentDirection::In },
				{ "CasterCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "GameObjectHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateGameObjectMove
		);
		functionMgr.Register(std::move(gameObjectMove));

		auto summon = std::make_unique<CustomQuery>(
			"NRD_Summon",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Template", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "Lifetime", ValueType::Real, FunctionArgumentDirection::In },
				{ "Level", ValueType::Integer, FunctionArgumentDirection::In },
				{ "IsTotem", ValueType::Integer, FunctionArgumentDirection::In },
				{ "MapToAiGrid", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Summon", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::Summon
		);
		functionMgr.Register(std::move(summon));
	}

}
