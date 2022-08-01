#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Symbols.h>

namespace dse::esv
{
	namespace func
	{
		template <class TAction>
		TAction * PrepareAction(OsiArgumentDesc const & args, GameActionType type)
		{
			auto character = GetEntityWorld()->GetComponent<Character>(args[0].String);
			if (character == nullptr) {
				OsiError("Character '" << args[0].String << "' does not exist!");
				return nullptr;
			}

			auto pos = args.GetVector(2);

			if (pos.x == -1.0f && pos.y == -1.0f && pos.z == -1.0f) {
				pos = character->WorldPos;
			}

			auto const & lib = GetStaticSymbols();
			auto actionMgr = lib.GetGameActionManager();

			if (lib.esv__GameActionManager__CreateAction == nullptr || lib.esv__GameActionManager__AddGameAction == nullptr || actionMgr == nullptr) {
				OsiErrorS("Game Action maanger not available");
				return nullptr;
			}

			auto action = (TAction *)lib.esv__GameActionManager__CreateAction(actionMgr, type, 0);

			action->SkillId = FixedString(args[1].String);
			ComponentHandle characterHandle;
			character->GetObjectHandle(characterHandle);
			action->OwnerHandle = characterHandle;

			action->Position = pos;
			return action;
		}

		bool CreateRain(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<esv::RainAction>(args, GameActionType::RainAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			auto const & lib = GetStaticSymbols();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameActionWrapper(actionMgr, action);

			args[5].Set((int64_t)action->MyHandle);
			return true;
		}

		bool CreateStorm(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<esv::StormAction>(args, GameActionType::StormAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			auto const & lib = GetStaticSymbols();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameActionWrapper(actionMgr, action);

			args[5].Set((int64_t)action->MyHandle);
			return true;
		}

		bool CreateWall(OsiArgumentDesc & args)
		{
			auto character = GetEntityWorld()->GetComponent<Character>(args[0].String);
			if (character == nullptr) {
				OsiError("Character '" << args[0].String << "' does not exist!");
				return false;
			}

			auto stats = GetStaticSymbols().GetStats();
			if (stats == nullptr) {
				return false;
			}

			auto object = stats->Objects.Find(args[1].String);
			if (object == nullptr) {
				OsiError("No such skill entry: '" << args[1].String << "'");
				return false;
			}

			auto lifetime = object->GetInt(GFS.strLifetime);
			if (!lifetime) {
				OsiError("Couldn't fetch lifetime of skill '" << args[1].String << "'");
				return false;
			}

			auto const & lib = GetStaticSymbols();
			auto actionMgr = lib.GetGameActionManager();

			if (lib.esv__WallAction__CreateWall == nullptr) {
				OsiErrorS("Wall action symbols not available in library manager");
				return false;
			}

			if (lib.esv__GameActionManager__CreateAction == nullptr || lib.esv__GameActionManager__AddGameAction == nullptr || actionMgr == nullptr) {
				OsiErrorS("Game Action maanger not available");
				return false;
			}

			auto action = (esv::WallAction *)lib.esv__GameActionManager__CreateAction(actionMgr, GameActionType::WallAction, 0);

			action->SkillId = FixedString(args[1].String);
			ComponentHandle characterHandle;
			character->GetObjectHandle(characterHandle);
			action->OwnerHandle = characterHandle;

			action->IsFromItem = false;
			action->LifeTime = *lifetime / 1000.0f;

			action->Source = args.GetVector(2);
			action->Target = args.GetVector(5);

			lib.esv__WallAction__CreateWall(action);
			lib.AddGameActionWrapper(actionMgr, action);

			args[8].Set((int64_t)action->MyHandle);
			return true;
		}

		bool CreateTornado(OsiArgumentDesc & args)
		{
			auto const & lib = GetStaticSymbols();
			if (lib.esv__TornadoAction__Setup == nullptr) {
				OsiErrorS("Tornado action symbols not available in library manager");
				return false;
			}

			auto action = PrepareAction<esv::TornadoAction>(args, GameActionType::TornadoAction);
			if (action == nullptr) return false;

			action->IsFromItem = false;

			action->Target = args.GetVector(5);

			auto actionMgr = lib.GetGameActionManager();
			lib.esv__TornadoAction__Setup(action);
			lib.AddGameActionWrapper(actionMgr, action);

			args[8].Set((int64_t)action->MyHandle);
			return true;
		}

		bool CreateDome(OsiArgumentDesc & args)
		{
			auto action = PrepareAction<esv::StatusDomeAction>(args, GameActionType::StatusDomeAction);
			if (action == nullptr) return false;

			auto const & lib = GetStaticSymbols();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameActionWrapper(actionMgr, action);

			args[5].Set((int64_t)action->MyHandle);
			return true;
		}

		bool CreateGameObjectMove(OsiArgumentDesc & args)
		{
			auto gameObjectGuid = args[0].String;
			glm::vec3 targetPosition = args.GetVector(1);
			auto beamEffectName = args[4].String;
			auto casterGuid = args[5].String;
			auto & gameActionHandle = args[6];

			auto objectToMove = GetEntityWorld()->GetGameObject(gameObjectGuid, true);
			if (objectToMove == nullptr) return false;

			ComponentHandle objectHandle;
			objectToMove->GetObjectHandle(objectHandle);

			esv::Character * caster{ nullptr };
			if (beamEffectName != nullptr && strlen(beamEffectName) > 0) {
				caster = GetEntityWorld()->GetComponent<Character>(casterGuid);
				if (caster == nullptr) {
					OsiError("Caster character '" << casterGuid << "' does not exist!");
					return false;
				}
			}

			auto const & lib = GetStaticSymbols();
			auto actionMgr = lib.GetGameActionManager();

			if (lib.esv__GameObjectMoveAction__Setup == nullptr) {
				OsiErrorS("GameObjectMove symbols not available in library manager");
				return false;
			}

			if (lib.esv__GameActionManager__CreateAction == nullptr || lib.esv__GameActionManager__AddGameAction == nullptr || actionMgr == nullptr) {
				OsiErrorS("Game Action maanger not available");
				return false;
			}

			auto action = (esv::GameObjectMoveAction *)lib.esv__GameActionManager__CreateAction(actionMgr, GameActionType::GameObjectMoveAction, 0);


			if (caster != nullptr) {
				ComponentHandle casterHandle;
				caster->GetObjectHandle(casterHandle);
				action->CasterCharacterHandle = casterHandle;
				action->BeamEffectName = FixedString(beamEffectName);
			}

			lib.esv__GameObjectMoveAction__Setup(action, objectHandle, &targetPosition);
			lib.AddGameActionWrapper(actionMgr, action);

			gameActionHandle.Set((int64_t)action->MyHandle);
			return true;
		}

		void DestroyGameActionInternal(esv::GameAction & action)
		{
			switch (action.ActionType) {
			case GameActionType::RainAction:
				static_cast<esv::RainAction &>(action).Finished = true;
				break;

			case GameActionType::StormAction:
				static_cast<esv::StormAction &>(action).LifeTime = 0.0f;
				break;

			case GameActionType::WallAction:
				static_cast<esv::WallAction &>(action).LifeTime = 0.0f;
				break;

			case GameActionType::TornadoAction:
				static_cast<esv::TornadoAction &>(action).Finished = true;
				break;

			case GameActionType::StatusDomeAction:
				static_cast<esv::StatusDomeAction &>(action).Finished = true;
				break;

			case GameActionType::GameObjectMoveAction:
				static_cast<esv::GameObjectMoveAction &>(action).DoneMoving = true;
				break;

			default:
				OsiError("Don't know how to destroy game action type " << (unsigned)action.ActionType);
				break;
			}
		}

		void GameActionDestroy(OsiArgumentDesc const & args)
		{
			auto gameAction = FindGameActionByHandle(ComponentHandle{ args[0].Int64 });
			if (gameAction == nullptr) return;

			DestroyGameActionInternal(*gameAction);
		}

		bool GameActionGetLifeTime(OsiArgumentDesc & args)
		{
			auto gameAction = FindGameActionByHandle(ComponentHandle{ args[0].Int64 });
			if (gameAction == nullptr) return false;

			float lifeTime;
			switch (gameAction->ActionType) {
			case GameActionType::RainAction:
				lifeTime = static_cast<esv::RainAction *>(gameAction)->LifeTime;
				break;

			case GameActionType::StormAction:
				lifeTime = static_cast<esv::StormAction *>(gameAction)->LifeTime;
				break;

			case GameActionType::WallAction:
				lifeTime = static_cast<esv::WallAction *>(gameAction)->LifeTime;
				break;

			case GameActionType::StatusDomeAction:
				lifeTime = static_cast<esv::StatusDomeAction *>(gameAction)->LifeTime;
				break;

			default:
				OsiError("Not supported for game action type " << (unsigned)gameAction->ActionType);
				return false;
			}

			args[1].Float = lifeTime;
			return true;
		}

		void GameActionSetLifeTime(OsiArgumentDesc const & args)
		{
			auto gameAction = FindGameActionByHandle(ComponentHandle{ args[0].Int64 });
			if (gameAction == nullptr) return;

			auto lifeTime = args[1].Float;
			if (lifeTime < 0.0f) {
				OsiErrorS("Lifetime must be a positive value");
				return;
			}

			switch (gameAction->ActionType) {
			case GameActionType::RainAction:
				static_cast<esv::RainAction *>(gameAction)->LifeTime = lifeTime;
				break;

			case GameActionType::StormAction:
				static_cast<esv::StormAction *>(gameAction)->LifeTime = lifeTime;
				break;

			case GameActionType::StatusDomeAction:
				static_cast<esv::StatusDomeAction *>(gameAction)->LifeTime = lifeTime;
				break;

			default:
				OsiError("Not supported for game action type " << (unsigned)gameAction->ActionType);
				break;
			}
		}

		bool Summon(OsiArgumentDesc & args)
		{
			auto character = GetEntityWorld()->GetComponent<Character>(args[0].String);
			if (character == nullptr) {
				OsiError("Character '" << args[0].String << "' does not exist!");
				return false;
			}

			auto objectTemplate = NameGuidToFixedString(args[1].String);
			if (!objectTemplate) {
				OsiError("Malformed template GUID: '" << args[1].String << "'!");
				return false;
			}

			auto const & lib = GetStaticSymbols();
			if (lib.esv__SummonHelpers__Summon == nullptr) {
				OsiErrorS("Summon helper symbols not available in library manager");
				return false;
			}

			auto pos = args.GetVector(2);
			auto lifetime = args[5].Float;
			auto summonLevel = args[6].Int32;
			auto isTotem = args[7].Int32 != 0;
			auto mapToAiGrid = args[8].Int32 != 0;
			auto & summonGuid = args[9];

			if (pos.x == -1.0f && pos.y == -1.0f && pos.z == -1.0f) {
				pos = character->WorldPos;
			}

			esv::SummonHelperResults results;
			esv::SummonHelperSummonArgs summonArgs;

			ComponentHandle characterHandle;
			character->GetObjectHandle(characterHandle);
			summonArgs.OwnerCharacterHandle = characterHandle;
			summonArgs.GameObjectTemplateFS = objectTemplate;
			summonArgs.Level = *character->GetCurrentLevel();
#if defined(OSI_EOCAPP)
			summonArgs.SummoningAbilityLevel = -1;
#endif
			summonArgs.Position = pos;
			summonArgs.SummonLevel = summonLevel;
			summonArgs.Lifetime = lifetime;
			summonArgs.IsTotem = isTotem;
			summonArgs.MapToAiGrid = mapToAiGrid;

			lib.esv__SummonHelpers__Summon(&results, &summonArgs);
			if (results.SummonHandle) {
				FixedString guid;

				auto summonCharacter = GetEntityWorld()->GetComponent<Character>(results.SummonHandle);
				if (summonCharacter != nullptr) {
					guid = *summonCharacter->GetGuid();
				} else {
					auto summonItem = GetEntityWorld()->GetComponent<Item>(results.SummonHandle);
					if (summonItem != nullptr) {
						guid = *summonItem->GetGuid();
					}
				}

				if (guid) {
					summonGuid.Set(guid);
					return true;
				}
				else {
					OsiErrorS("Summoned object is not a character or item!");
					return false;
				}
			}
			else {
				OsiErrorS("esv::SummonHelpers::Summon() call failed!");
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
				{ "OwnerCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateRain
		);
		functionMgr.Register(std::move(createRain));

		auto createStorm = std::make_unique<CustomQuery>(
			"NRD_CreateStorm",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateStorm
		);
		functionMgr.Register(std::move(createStorm));

		auto createWall = std::make_unique<CustomQuery>(
			"NRD_CreateWall",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "SourceX", ValueType::Real, FunctionArgumentDirection::In },
				{ "SourceY", ValueType::Real, FunctionArgumentDirection::In },
				{ "SourceZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetX", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetY", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateWall
		);
		functionMgr.Register(std::move(createWall));

		auto createTornado = std::make_unique<CustomQuery>(
			"NRD_CreateTornado",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "PositionX", ValueType::Real, FunctionArgumentDirection::In },
				{ "PositionY", ValueType::Real, FunctionArgumentDirection::In },
				{ "PositionZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetX", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetY", ValueType::Real, FunctionArgumentDirection::In },
				{ "TargetZ", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateTornado
		);
		functionMgr.Register(std::move(createTornado));

		auto createDome = std::make_unique<CustomQuery>(
			"NRD_CreateDome",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
			},
			&func::CreateDome
		);
		functionMgr.Register(std::move(createDome));

		auto gameObjectMove = std::make_unique<CustomQuery>(
			"NRD_CreateGameObjectMove",
			std::vector<CustomFunctionParam>{
				{ "TargetObject", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "BeamEffectName", ValueType::String, FunctionArgumentDirection::In },
				{ "CasterCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "GameActionHandle", ValueType::Integer64, FunctionArgumentDirection::Out },
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
				{ "OwnerCharacter", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Template", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
				{ "Lifetime", ValueType::Real, FunctionArgumentDirection::In },
				{ "Level", ValueType::Integer, FunctionArgumentDirection::In },
				{ "IsTotem", ValueType::Integer, FunctionArgumentDirection::In },
				{ "MapToAiGrid", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Summon", ValueType::CharacterGuid, FunctionArgumentDirection::Out },
			},
			&func::Summon
		);
		functionMgr.Register(std::move(summon));
	}

}
