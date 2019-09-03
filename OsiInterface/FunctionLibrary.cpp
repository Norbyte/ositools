#include "stdafx.h"
#include "FunctionLibrary.h"
#include "OsirisProxy.h"
#include <random>

namespace osidbg
{
	FunctionHandle StatusIteratorEventHandle;
	bool DangerousStuffEnabled{ false };

	namespace func
	{
		void Log(OsiArgumentDesc const & args)
		{
			auto msg = args.Get(0).ToString();
			Debug("[Osiris] %s", msg.c_str());
			gOsirisProxy->GetWrappers().AssertOriginal(false, msg.c_str(), false);
		}

		bool RandomFloat(OsiArgumentDesc & args)
		{
			auto min = args.Get(0).Float;
			auto max = args.Get(1).Float;
			std::uniform_real_distribution<float> dist(min, max);
			std::mt19937_64 rng(12345);
			args.Get(2).Float = dist(rng);
			return true;
		}

		bool Sin(OsiArgumentDesc & args)
		{
			auto x = args.Get(0).Float;
			args.Get(1).Float = sin(x);
			return true;
		}

		bool Cos(OsiArgumentDesc & args)
		{
			auto x = args.Get(0).Float;
			args.Get(1).Float = cos(x);
			return true;
		}

		bool StringCompare(OsiArgumentDesc & args)
		{
			auto a = args.Get(0).String;
			auto b = args.Get(1).String;

			int rel = strcmp(a, b);
			if (rel < 0) {
				args.Get(2).Int32 = -1;
			}
			else if (rel == 0) {
				args.Get(2).Int32 = 0;
			}
			else {
				args.Get(2).Int32 = 1;
			}

			return true;
		}

		bool StringLength(OsiArgumentDesc & args)
		{
			auto str = args.Get(0).String;
			args.Get(1).Int32 = (int32_t)strlen(str);
			return true;
		}

		bool StringToInt(OsiArgumentDesc & args)
		{
			auto a = args.Get(0).String;
			try {
				args.Get(1).Int32 = std::stoi(a);
				return true;
			}
			catch (std::invalid_argument &) {
				return false;
			}
		}

		bool StringToFloat(OsiArgumentDesc & args)
		{
			auto a = args.Get(0).String;
			try {
				args.Get(1).Float = std::stof(a);
				return true;
			}
			catch (std::invalid_argument &) {
				return false;
			}
		}

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

		void IterateCharacterStatuses(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				return;
			}

			auto eventName = args.Get(1).String;
			auto statuses = character->StatusManager;
			if (statuses != nullptr) {
				auto index = 0;
				EsvStatus ** end = statuses->Statuses + statuses->StatusCount;
				for (auto status = statuses->Statuses; status != end; status++) {
					auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::String, eventName });
					eventArgs->Add(OsiArgumentValue{ ValueType::GuidString, args.Get(0).String });
					eventArgs->Add(OsiArgumentValue{ (int64_t)index });

					auto osiris = gOsirisProxy->GetDynamicGlobals().OsirisObject;
					gOsirisProxy->GetWrappers().Event.CallOriginal(osiris, (uint32_t)StatusIteratorEventHandle, eventArgs);
					index++;

					delete eventArgs;
				}
			}
		}

		EsvStatus * GetStatusHelper(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr || character->StatusManager == nullptr) {
				return nullptr;
			}

			auto handle = args.Get(1).Int64;
			auto statuses = character->StatusManager;
			if (handle < 0 || handle >= statuses->StatusCount) {
				Debug("GetStatusHelper(): Status handle out of bounds");
				return nullptr;
			}

			return statuses->Statuses[handle];
		}

		bool GetStatusAttributeString(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			if (strcmp(attributeName, "StatsId") == 0) {
				args.Get(3).String = const_cast<char *>(status->StatusId.Str);
			}
			else {
				Debug("GetStatusAttributeString(): Unknown attribute");
				return false;
			}

			return true;
		}

		bool GetStatusAttributeGuidString(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			ObjectHandle handle;
			if (strcmp(attributeName, "Obj1") == 0) {
				handle = status->ObjHandle1;
			}
			else if (strcmp(attributeName, "TargetCI") == 0) {
				handle = status->TargetCIHandle;
			}
			else if (strcmp(attributeName, "StatusSource") == 0) {
				handle = status->StatusSourceHandle;
			}
			else if (strcmp(attributeName, "Obj2") == 0) {
				handle = status->SomeHandle;
			}
			else {
				Debug("GetStatusAttributeGuidString(): Unknown attribute");
				return false;
			}

			auto character = FindCharacterByHandle(handle);
			if (character == nullptr) {
				return false;
			}

			args.Get(3).String = const_cast<char *>(character->MyGuid.Str);
			return true;
		}

		bool GetStatusAttributeFloat(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			if (strcmp(attributeName, "StartTimer") == 0) {
				args.Get(3).Float = status->StartTimer;
			}
			else if (strcmp(attributeName, "LifeTime") == 0) {
				args.Get(3).Float = status->LifeTime;
			}
			else if (strcmp(attributeName, "CurrentLifeTime") == 0) {
				args.Get(3).Float = status->CurrentLifeTime;
			}
			else if (strcmp(attributeName, "TurnTimer") == 0) {
				args.Get(3).Float = status->TurnTimer;
			}
			else if (strcmp(attributeName, "Strength") == 0) {
				args.Get(3).Float = status->Strength;
			}
			else if (strcmp(attributeName, "StatsMultiplier") == 0) {
				args.Get(3).Float = status->StatsMultiplier;
			}
			else {
				Debug("GetStatusAttributeFloat(): Unknown attribute");
				return false;
			}

			return true;
		}

		bool GetStatusAttributeInt(OsiArgumentDesc & args)
		{
			auto status = GetStatusHelper(args);
			if (status == nullptr) {
				return false;
			}

			auto attributeName = args.Get(2).String;
			if (strcmp(attributeName, "CanEnterChance") == 0) {
				args.Get(3).Int32 = status->CanEnterChance;
			}
			else if (strcmp(attributeName, "DamageSourceType") == 0) {
				args.Get(3).Int32 = status->DamageSourceType;
			}
			else if (strcmp(attributeName, "KeepAlive") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_KeepAlive) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsOnSourceSurface") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_IsOnSourceSurface) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsFromItem") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_IsFromItem) ? 1 : 0;
			}
			else if (strcmp(attributeName, "Channeled") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_Channeled) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsLifeTimeSet") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_IsLifeTimeSet) ? 1 : 0;
			}
			else if (strcmp(attributeName, "InitiateCombat") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_InitiateCombat) ? 1 : 0;
			}
			else if (strcmp(attributeName, "Influence") == 0) {
				args.Get(3).Int32 = (status->Flags0 & EsvStatus::SF0_Influence) ? 1 : 0;
			}
			else if (strcmp(attributeName, "BringIntoCombat") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_BringIntoCombat) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsHostileAct") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_IsHostileAct) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsInvulnerable") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_IsInvulnerable) ? 1 : 0;
			}
			else if (strcmp(attributeName, "IsResistingDeath") == 0) {
				args.Get(3).Int32 = (status->Flags1 & EsvStatus::SF1_IsResistingDeath) ? 1 : 0;
			}
			else if (strcmp(attributeName, "ForceStatus") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_ForceStatus) ? 1 : 0;
			}
			else if (strcmp(attributeName, "ForceFailStatus") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_ForceFailStatus) ? 1 : 0;
			}
			else if (strcmp(attributeName, "RequestDelete") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_RequestDelete) ? 1 : 0;
			}
			else if (strcmp(attributeName, "RequestDeleteAtTurnEnd") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_RequestDeleteAtTurnEnd) ? 1 : 0;
			}
			else if (strcmp(attributeName, "Started") == 0) {
				args.Get(3).Int32 = (status->Flags2 & EsvStatus::SF2_Started) ? 1 : 0;
			}
			else {
				Debug("GetStatusAttributeInt(): Unknown attribute");
				return false;
			}

			return true;
		}

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

		std::unique_ptr<ShootProjectileApiHelper> ProjectileHelper;

		void ProjectileBegin(OsiArgumentDesc const & args)
		{
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
				Debug("ProjectileEnd(): No projectile to shoot!");
			}
		}

		void ProjectileSetInt(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).Int32;

			if (!ProjectileHelper) return;
			ProjectileHelper->SetInt(prop, value);
		}

		void ProjectileSetString(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).String;

			if (!ProjectileHelper) return;
			ProjectileHelper->SetString(prop, value);
		}

		void ProjectileSetGuidString(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			auto value = args.Get(1).String;

			if (!ProjectileHelper) return;
			ProjectileHelper->SetGuidString(prop, value);
		}

		void ProjectileSetVector3(OsiArgumentDesc const & args)
		{
			auto prop = args.Get(0).String;
			float vec[3];
			vec[0] = args.Get(1).Float;
			vec[1] = args.Get(2).Float;
			vec[2] = args.Get(3).Float;

			if (!ProjectileHelper) return;
			ProjectileHelper->SetVector(prop, vec);
		}

		void DoExperiment(OsiArgumentDesc const & args)
		{
			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			/*auto action = (EsvGameActionTornado *)lib.CreateGameAction(actionMgr, 6, 0);

			action->SkillId = ToFixedString("Tornado_Air");
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			ObjectHandle characterHandle;
			character->GetObjectHandle(&characterHandle);
			action->OwnerHandle = characterHandle;
			action->IsFromItem = false;

			action->Position[0] = character->WorldPos[0];
			action->Position[1] = character->WorldPos[1];
			action->Position[2] = character->WorldPos[2];
			action->Target[0] = character->WorldPos[0];
			action->Target[1] = character->WorldPos[1];
			action->Target[2] = character->WorldPos[2];
			action->Target[0] += 10.0f;

			lib.AddGameAction(actionMgr, action);*/

			{
				auto action = (EsvRainAction *)lib.CreateGameAction(actionMgr, EsvGameAction::RainAction, 0);

				action->SkillId = ToFixedString("Rain_EnemySunshine");
				auto character = FindCharacterByNameGuid(args.Get(0).String);
				ObjectHandle characterHandle;
				character->GetObjectHandle(&characterHandle);
				action->OwnerHandle = characterHandle;
				action->IsFromItem = false;

				action->Position[0] = character->WorldPos[0];
				action->Position[1] = character->WorldPos[1];
				action->Position[2] = character->WorldPos[2];

				lib.AddGameAction(actionMgr, action);
			}

			{
				auto action = (EsvStatusDomeAction *)lib.CreateGameAction(actionMgr, EsvGameAction::StatusDomeAction, 0);

				action->SkillId = ToFixedString("Dome_CircleOfProtection");
				auto character = FindCharacterByNameGuid(args.Get(0).String);
				ObjectHandle characterHandle;
				character->GetObjectHandle(&characterHandle);
				action->OwnerHandle = characterHandle;

				action->Position[0] = character->WorldPos[0];
				action->Position[1] = character->WorldPos[1];
				action->Position[2] = character->WorldPos[2];

				lib.AddGameAction(actionMgr, action);
			}
		}

		template <class TAction>
		TAction * PrepareAction(OsiArgumentDesc const & args, EsvGameAction::ActionType type)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				return nullptr;
			}

			auto skillId = ToFixedString(args.Get(1).String);
			if (!skillId) {
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

		void CreateRain(OsiArgumentDesc const & args)
		{
			auto action = PrepareAction<EsvRainAction>(args, EsvGameAction::RainAction);

			action->IsFromItem = false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);
		}

		void CreateStorm(OsiArgumentDesc const & args)
		{
			auto action = PrepareAction<EsvStormAction>(args, EsvGameAction::StormAction);

			action->IsFromItem = false;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);
		}

		void CreateWall(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				return;
			}

			auto skillId = ToFixedString(args.Get(1).String);
			if (!skillId) {
				return;
			}

			auto stats = gOsirisProxy->GetLibraryManager().GetStats();
			if (stats == nullptr) {
				return;
			}

			auto object = stats->objects.Find(args.Get(1).String);
			if (object == nullptr) {
				return;
			}

			auto lifetime = stats->GetAttributeInt(object, "Lifetime");
			if (!lifetime) {
				return;
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
		}

		void CreateTornado(OsiArgumentDesc const & args)
		{
			auto action = PrepareAction<EsvTornadoAction>(args, EsvGameAction::TornadoAction);

			action->IsFromItem = false;

			action->Target[0] = args.Get(5).Float;
			action->Target[1] = args.Get(6).Float;
			action->Target[2] = args.Get(7).Float;

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.TornadoActionSetup(action);
			lib.AddGameAction(actionMgr, action);
		}

		void CreateDome(OsiArgumentDesc const & args)
		{
			auto action = PrepareAction<EsvStatusDomeAction>(args, EsvGameAction::StatusDomeAction);

			auto const & lib = gOsirisProxy->GetLibraryManager();
			auto actionMgr = lib.GetGameActionManager();
			lib.AddGameAction(actionMgr, action);
		}

		bool Summon(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character == nullptr) {
				return false;
			}

			auto objectTemplate = NameGuidToFixedString(args.Get(1).String);
			if (!objectTemplate) {
				Debug("Summon(): Template '%s' not in FixedString table!", args.Get(1).String);
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
				return false;
			}
		}
	}

	CustomFunctionLibrary::CustomFunctionLibrary(class OsirisProxy & osiris)
		: osiris_(osiris)
	{}

	void CustomFunctionLibrary::Register()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto debugLog = std::make_unique<CustomCall>(
			"NRD_DebugLog",
			std::vector<CustomFunctionParam>{ { "Message", ValueType::None, FunctionArgumentDirection::In } },
			&func::Log
		);
		functionMgr.Register(std::move(debugLog));

		auto randomFloat = std::make_unique<CustomQuery>(
			"NRD_RandomFloat",
			std::vector<CustomFunctionParam>{
				{ "Min", ValueType::Real, FunctionArgumentDirection::In },
				{ "Max", ValueType::Real, FunctionArgumentDirection::In },
				{ "Result", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::RandomFloat
		);
		functionMgr.Register(std::move(randomFloat));

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

		auto sine = std::make_unique<CustomQuery>(
			"NRD_Sin",
			std::vector<CustomFunctionParam>{
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Sine", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::Sin
		);
		functionMgr.Register(std::move(sine));

		auto cosine = std::make_unique<CustomQuery>(
			"NRD_Cos",
			std::vector<CustomFunctionParam>{
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Cosine", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::Cos
		);
		functionMgr.Register(std::move(cosine));

		auto stringCompare = std::make_unique<CustomQuery>(
			"NRD_StringCompare",
			std::vector<CustomFunctionParam>{
				{ "A", ValueType::String, FunctionArgumentDirection::In },
				{ "B", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::StringCompare
		);
		functionMgr.Register(std::move(stringCompare));

		auto stringLength = std::make_unique<CustomQuery>(
			"NRD_StringLength",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Length", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::StringLength
		);
		functionMgr.Register(std::move(stringLength));

		auto stringToInt = std::make_unique<CustomQuery>(
			"NRD_StringToInt",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::StringToInt
		);
		functionMgr.Register(std::move(stringToInt));

		auto stringToFloat = std::make_unique<CustomQuery>(
			"NRD_StringToFloat",
			std::vector<CustomFunctionParam>{
				{ "String", ValueType::String, FunctionArgumentDirection::In },
				{ "Result", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::StringToFloat
		);
		functionMgr.Register(std::move(stringToFloat));

		auto iterateCharacterStatuses = std::make_unique<CustomCall>(
			"NRD_IterateCharacterStatuses",
			std::vector<CustomFunctionParam>{
				{ "CharacterGuid", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Event", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::IterateCharacterStatuses
		);
		functionMgr.Register(std::move(iterateCharacterStatuses));

		auto getStatusAttributeInt = std::make_unique<CustomQuery>(
			"NRD_GetStatusAttributeInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::GetStatusAttributeInt
		);
		functionMgr.Register(std::move(getStatusAttributeInt));

		auto getStatusAttributeFloat = std::make_unique<CustomQuery>(
			"NRD_GetStatusAttributeFloat",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Real, FunctionArgumentDirection::Out },
			},
			&func::GetStatusAttributeFloat
		);
		functionMgr.Register(std::move(getStatusAttributeFloat));

		auto getStatusAttributeString = std::make_unique<CustomQuery>(
			"NRD_GetStatusAttributeString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out },
			},
			&func::GetStatusAttributeString
		);
		functionMgr.Register(std::move(getStatusAttributeString));

		auto getStatusAttributeGuidString = std::make_unique<CustomQuery>(
			"NRD_GetStatusAttributeGuidString",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::GetStatusAttributeGuidString
		);
		functionMgr.Register(std::move(getStatusAttributeGuidString));

		auto statusIteratorEvent = std::make_unique<CustomEvent>(
			"NRD_StatusIteratorEvent",
			std::vector<CustomFunctionParam>{
				{ "Event", ValueType::String, FunctionArgumentDirection::In },
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatusHandle", ValueType::Integer64, FunctionArgumentDirection::In },
			}
		);
		StatusIteratorEventHandle = functionMgr.Register(std::move(statusIteratorEvent));

		auto itemGetStatsId = std::make_unique<CustomQuery>(
			"NRD_ItemGetStatsId",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatsId", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::ItemGetStatsId
		);
		functionMgr.Register(std::move(itemGetStatsId));


		//////////////////////////////////////////////////////////
		//
		//                PROJECTILE HELPERS API
		//
		//////////////////////////////////////////////////////////


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

		auto createRain = std::make_unique<CustomCall>(
			"NRD_CreateRain",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::CreateRain
		);
		functionMgr.Register(std::move(createRain));

		auto createStorm = std::make_unique<CustomCall>(
			"NRD_CreateStorm",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::CreateStorm
		);
		functionMgr.Register(std::move(createStorm));

		auto createWall = std::make_unique<CustomCall>(
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
			},
			&func::CreateWall
		);
		functionMgr.Register(std::move(createWall));

		auto createTornado = std::make_unique<CustomCall>(
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
			},
			&func::CreateTornado
		);
		functionMgr.Register(std::move(createTornado));

		auto createDome = std::make_unique<CustomCall>(
			"NRD_CreateDome",
			std::vector<CustomFunctionParam>{
				{ "OwnerCharacter", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "X", ValueType::Real, FunctionArgumentDirection::In },
				{ "Y", ValueType::Real, FunctionArgumentDirection::In },
				{ "Z", ValueType::Real, FunctionArgumentDirection::In },
			},
			&func::CreateDome
		);
		functionMgr.Register(std::move(createDome));

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
