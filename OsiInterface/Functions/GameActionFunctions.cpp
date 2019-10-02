#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	namespace func
	{
		template <class TAction>
		TAction * PrepareAction(OsiArgumentDesc const & args, GameActionType type)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("Character '" << args.Get(0).String << "' does not exist!");
				return nullptr;
			}

			auto skillId = ToFixedString(args.Get(1).String);
			if (!skillId) {
				OsiError("'" << args.Get(1).String << "' is not a valid FixedString!");
				return nullptr;
			}

			auto pos = args.GetVector(2);

			if (pos.x == -1.0f && pos.y == -1.0f && pos.z == -1.0f) {
				pos = character->WorldPos;
			}

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			auto action = (TAction *)lib.CreateGameAction(actionMgr, type, 0);

			action->SkillId = skillId;
			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			action->OwnerHandle = characterHandle;

			action->Position = pos;
			return action;
		}

		bool CreateRain(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvRainAction>(args, GameActionType::RainAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);

			args.Get(5).Int64 = (int64_t)action->MyHandle;
			return true;
		}

		bool CreateStorm(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvStormAction>(args, GameActionType::StormAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);

			args.Get(5).Int64 = (int64_t)action->MyHandle;
			return true;
		}

		bool CreateWall(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("Character '" << args.Get(0).String << "' does not exist!");
				return false;
			}

			auto skillId = ToFixedString(args.Get(1).String);
			if (!skillId) {
				OsiError("'" << args.Get(1).String << "' is not a valid FixedString!");
				return false;
			}

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->objects.Find(args.Get(1).String);
			if (object == nullptr) {
				OsiError("No such skill entry: '" << args.Get(1).String << "'");
				return false;
			}

			auto lifetime = stats->GetAttributeInt(object, "Lifetime");
			if (!lifetime) {
				OsiError("Couldn't fetch lifetime of skill '" << args.Get(1).String << "'");
				return false;
			}

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			auto action = (EsvWallAction *)lib.CreateGameAction(actionMgr, GameActionType::WallAction, 0);

			action->SkillId = skillId;
			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			action->OwnerHandle = characterHandle;

			action->IsFromItem = false;
			action->LifeTime = *lifetime / 1000.0f;

			action->Source = args.GetVector(2);
			action->Target = args.GetVector(5);

			lib.WallActionCreateWall(action);
			lib.AddGameAction(actionMgr, action);

			args.Get(8).Int64 = (int64_t)action->MyHandle;
			return true;
		}

		bool CreateTornado(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvTornadoAction>(args, GameActionType::TornadoAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			action->Target = args.GetVector(5);

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.TornadoActionSetup(action);
			lib.AddGameAction(actionMgr, action);

			args.Get(8).Int64 = (int64_t)action->MyHandle;
			return true;
		}

		bool CreateDome(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<EsvStatusDomeAction>(args, GameActionType::StatusDomeAction);
			if (action == nullptr) return false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);

			args.Get(5).Int64 = (int64_t)action->MyHandle;
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
					OsiError("Game object '" << args.Get(0).String << "' does not exist!");
					return false;
				}
			}

			auto beamEffectName = args.Get(4).String;
			FixedString beamEffectFs;
			EsvCharacter * caster{ nullptr };
			if (beamEffectName != nullptr && strlen(beamEffectName) > 0) {
				beamEffectFs = ToFixedString(beamEffectName);

				if (!beamEffectFs) {
					OsiError("Beam effect is not a valid FixedString!");
					return false;
				}

				auto casterGuid = args.Get(5).String;
				caster = FindCharacterByNameGuid(casterGuid);
				if (caster == nullptr) {
					OsiError("Caster character '" << casterGuid << "' does not exist!");
					return false;
				}
			}

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			auto action = (EsvGameObjectMoveAction *)lib.CreateGameAction(actionMgr, GameActionType::GameObjectMoveAction, 0);

			glm::vec3 targetPosition = args.GetVector(1);

			if (caster != nullptr) {
				ObjectHandle casterHandle;
				caster->GetObjectHandle(&casterHandle);
				action->CasterCharacterHandle = casterHandle;
				action->BeamEffectName = beamEffectFs;
			}

			lib.GameObjectMoveActionSetup(action, objectHandle, &targetPosition);
			lib.AddGameAction(actionMgr, action);

			args.Get(6).Int64 = (int64_t)action->MyHandle;
			return true;
		}

		void DestroyGameActionInternal(EsvGameAction & action)
		{
			switch (action.GameActionType) {
			case GameActionType::RainAction:
				static_cast<EsvRainAction &>(action).Finished = true;
				break;

			case GameActionType::StormAction:
				static_cast<EsvStormAction &>(action).LifeTime = 0.0f;
				break;

			case GameActionType::WallAction:
				static_cast<EsvWallAction &>(action).LifeTime = 0.0f;
				break;

			case GameActionType::TornadoAction:
				static_cast<EsvTornadoAction &>(action).Finished = true;
				break;

			case GameActionType::StatusDomeAction:
				static_cast<EsvStatusDomeAction &>(action).Finished = true;
				break;

			case GameActionType::GameObjectMoveAction:
				static_cast<EsvGameObjectMoveAction &>(action).DoneMoving = true;
				break;

			default:
				OsiError("Don't know how to destroy game action type " << action.GameActionType);
				break;
			}
		}

		void GameActionDestroy(OsiArgumentDesc const & args)
		{
			auto gameAction = FindGameActionByHandle(ObjectHandle{ args.Get(0).Int64 });
			if (gameAction == nullptr) return;

			DestroyGameActionInternal(*gameAction);
		}

		bool GameActionGetLifeTime(OsiArgumentDesc & args)
		{
			auto gameAction = FindGameActionByHandle(ObjectHandle{ args.Get(0).Int64 });
			if (gameAction == nullptr) return false;

			float lifeTime;
			switch (gameAction->GameActionType) {
			case GameActionType::RainAction:
				lifeTime = static_cast<EsvRainAction *>(gameAction)->LifeTime;
				break;

			case GameActionType::StormAction:
				lifeTime = static_cast<EsvStormAction *>(gameAction)->LifeTime;
				break;

			case GameActionType::WallAction:
				lifeTime = static_cast<EsvWallAction *>(gameAction)->LifeTime;
				break;

			case GameActionType::StatusDomeAction:
				lifeTime = static_cast<EsvStatusDomeAction *>(gameAction)->LifeTime;
				break;

			default:
				OsiError("Not supported for game action type " << gameAction->GameActionType);
				return false;
			}

			args.Get(1).Float = lifeTime;
			return true;
		}

		void GameActionSetLifeTime(OsiArgumentDesc const & args)
		{
			auto gameAction = FindGameActionByHandle(ObjectHandle{ args.Get(0).Int64 });
			if (gameAction == nullptr) return;

			auto lifeTime = args.Get(1).Float;
			if (lifeTime < 0.0f) {
				OsiError("Lifetime must be a positive value");
				return;
			}

			switch (gameAction->GameActionType) {
			case GameActionType::RainAction:
				static_cast<EsvRainAction *>(gameAction)->LifeTime = lifeTime;
				break;

			case GameActionType::StormAction:
				static_cast<EsvStormAction *>(gameAction)->LifeTime = lifeTime;
				break;

			case GameActionType::StatusDomeAction:
				static_cast<EsvStatusDomeAction *>(gameAction)->LifeTime = lifeTime;
				break;

			default:
				OsiError("Not supported for game action type " << gameAction->GameActionType);
				break;
			}
		}

		bool Summon(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				OsiError("Character '" << args.Get(0).String << "' does not exist!");
				return false;
			}

			auto objectTemplate = NameGuidToFixedString(args.Get(1).String);
			if (!objectTemplate) {
				OsiError("Template '" << args.Get(1).String << "' not in FixedString table!");
				return false;
			}

			auto pos = args.GetVector(2);
			auto lifetime = args.Get(5).Float;
			auto summonLevel = args.Get(6).Int32;
			auto isTotem = args.Get(7).Int32 != 0;
			auto mapToAiGrid = args.Get(8).Int32 != 0;

			if (pos.x == -1.0f && pos.y == -1.0f && pos.z == -1.0f) {
				pos = character->WorldPos;
			}

			SummonHelperResults results;
			SummonHelperSummonArgs summonArgs;

			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			summonArgs.OwnerCharacterHandle = characterHandle;
			summonArgs.GameObjectTemplateFS = objectTemplate;
			summonArgs.Level = *character->GetCurrentLevel();
			summonArgs.Position = pos;
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
					args.Get(9).String = guid.Str;
					return true;
				}
				else {
					Debug("Summoned object is not a character or item!");
					return false;
				}
			}
			else {
				OsiError("esv::SummonHelpers::Summon() call failed!");
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

		auto destroyGameAction = std::make_unique<CustomCall>(
			"NRD_GameActionDestroy",
			std::vector<CustomFunctionParam>{
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::In }
			},
			&func::GameActionDestroy
		);
		functionMgr.Register(std::move(destroyGameAction));

		auto gameActionGetLifeTime = std::make_unique<CustomQuery>(
			"NRD_GameActionGetLifeTime",
			std::vector<CustomFunctionParam>{
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "LifeTime", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::GameActionGetLifeTime
		);
		functionMgr.Register(std::move(gameActionGetLifeTime));

		auto gameActionSetLifeTime = std::make_unique<CustomCall>(
			"NRD_GameActionSetLifeTime",
			std::vector<CustomFunctionParam>{
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "LifeTime", ValueType::Real, FunctionArgumentDirection::In }
			},
			&func::GameActionSetLifeTime
		);
		functionMgr.Register(std::move(gameActionSetLifeTime));

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
