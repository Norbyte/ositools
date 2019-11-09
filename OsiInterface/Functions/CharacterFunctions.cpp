#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include "PropertyMaps.h"
#include <locale>
#include <codecvt>

namespace osidbg
{
	namespace func
	{
		CharacterDynamicStat * GetCharacterDynamicStat(esv::Character * character, uint32_t index)
		{
			if (character->Stats == nullptr) {
				OsiError("Character has no stats!");
				return nullptr;
			}

			if (character->Stats->DynamicStats == nullptr) {
				OsiError("Character has no dynamic stats!");
				return nullptr;
			}

			auto numStats = character->Stats->DynamicStatsEnd - character->Stats->DynamicStats;
			if (numStats <= index) {
				OsiError("Tried to get dynamic stat " << index << ", character only has " << numStats);
				return nullptr;
			}

			return character->Stats->DynamicStats[index];
		}

		template <OsiPropertyMapType Type>
		bool CharacterGetStat(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			if (character == nullptr || character->Stats == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterStatsPropertyMap, character->Stats, args, 1, Type);
		}

		void CharacterSetStatInt(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			auto stat = args[1].String;
			auto value = args[2].Int32;

			if (character == nullptr || character->Stats == nullptr) return;

			auto clamped = value;
			if (strcmp(stat, "CurrentVitality") == 0) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxVitality);
			} else if (strcmp(stat, "CurrentArmor") == 0) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxArmor);
			} else if (strcmp(stat, "CurrentMagicArmor") == 0) {
				clamped = std::clamp(value, 0, (int32_t)character->Stats->MaxMagicArmor);
			}

			gCharacterStatsPropertyMap.setInt(character->Stats, stat, clamped, false, true);
		}

		template <OsiPropertyMapType Type>
		bool CharacterGetPermanentBoost(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			if (character == nullptr) return false;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return false;

			return OsirisPropertyMapGet(gCharacterDynamicStatPropertyMap, permanentBoosts, args, 1, Type);
		}

		template <OsiPropertyMapType Type>
		void CharacterSetPermanentBoost(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			if (character == nullptr) return;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return;

			OsirisPropertyMapSet(gCharacterDynamicStatPropertyMap, permanentBoosts, args, 1, Type);
		}

		bool CharacterIsTalentDisabled(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto talent = args[1].String;
			auto & disabled = args[2].Int32;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) return false;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return false;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return false;
			}

			disabled = permanentBoosts->IsTalentRemoved(*talentId) ? 1 : 0;
			return true;
		}

		void CharacterDisableTalent(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto talent = args[1].String;
			auto disabled = args[2].Int32;

			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) return;

			auto permanentBoosts = GetCharacterDynamicStat(character, 1);
			if (permanentBoosts == nullptr) return;

			auto talentId = EnumInfo<TalentType>::Find(talent);
			if (!talentId) {
				OsiError("Talent name is invalid: " << talent);
				return;
			}

			permanentBoosts->RemoveTalent(*talentId, disabled != 0);
		}

		esv::CustomStatDefinitionComponent * FindCustomStatDefinitionByName(char const * name)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wstrName = converter.from_bytes(name);

			auto entityWorld = GetEntityWorld();
			auto statSystem = entityWorld->GetCustomStatSystem();
			auto & defnHandles = statSystem->CustomStatDefinitionHandles.Set;

			for (uint32_t i = 0; i < defnHandles.Size; i++) {
				auto handle = defnHandles.Buf[i].Handle;
				auto statDefn = entityWorld->FindCustomStatDefinitionComponentByHandle(handle);
				if (statDefn != nullptr && wcscmp(statDefn->Name.GetPtr(), wstrName.c_str()) == 0) {
					return statDefn;
				}
			}

			OsiError("Could not find custom stat definition '" << name << "'");
			return nullptr;
		}

		esv::CustomStatDefinitionComponent * FindCustomStatDefinitionById(char const * id)
		{
			auto fs = ToFixedString(id);
			if (!fs) {
				OsiError("Custom stat definition '" << id << "' is not in fixed string table");
				return nullptr;
			}

			auto entityWorld = GetEntityWorld();
			auto statSystem = entityWorld->GetCustomStatSystem();
			auto & defnHandles = statSystem->CustomStatDefinitionHandles.Set;

			for (uint32_t i = 0; i < defnHandles.Size; i++) {
				auto handle = defnHandles.Buf[i].Handle;
				auto statDefn = entityWorld->FindCustomStatDefinitionComponentByHandle(handle);
				if (statDefn != nullptr && statDefn->Id == fs) {
					return statDefn;
				}
			}

			OsiError("Could not find custom stat definition '" << id << "'");
			return nullptr;
		}

		bool CharacterGetCustomStat(OsiArgumentDesc & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			auto statId = args[1].String;
			auto & statValue = args[2].Int32;

			auto statDefn = FindCustomStatDefinitionById(statId);
			if (statDefn == nullptr) return false;

			auto entityWorld = GetEntityWorld();
			auto statsComponent = entityWorld->GetCustomStatsComponentByEntityHandle(character->Base.EntityObjectHandle);
			if (statsComponent == nullptr) {
				OsiError("Character has no CustomStatsComponent");
				return false;
			}

			auto value = statsComponent->StatValues.Find(statDefn->Id.Str);
			if (value == nullptr) {
				OsiWarn("Character has no custom stat named '" << statId << "'");
				return false;
			}

			statValue = *value;
			return true;
		}

		void ProcessCustomStatsMessage(eocnet::Message * msg)
		{
			struct DummyType
			{
				uint16_t unkn1;
				uint16_t unkn2;
			};

			DummyType dummy1;
			dummy1.unkn2 = 0;

			auto processMsg = gOsirisProxy->GetLibraryManager().EsvCustomStatsProtocolProcessMsg;
			processMsg(nullptr, nullptr, &dummy1, msg);
		}

		void CharacterSyncCustomStats(esv::Character * character, eoc::CustomStatsComponent * stats,
			FixedString statKey, int statValue)
		{
			eocnet::CustomStatsSyncMessage statSyncMsg;
			if (stats == nullptr) {
				statSyncMsg.MessageId = NetMessage::NETMSG_CUSTOM_STATS_CREATE;
			} else {
				statSyncMsg.MessageId = NetMessage::NETMSG_CUSTOM_STATS_UPDATE;
			}

			// FIXME - memory leak!
			statSyncMsg.Stats.Set.Buf = GameAlloc<eocnet::CustomStatsSyncInfo>(1);
			statSyncMsg.Stats.Set.Capacity = 1;
			statSyncMsg.Stats.Set.Size = 1;

			auto entityWorld = GetEntityWorld();
			auto netComponent = entityWorld->GetNetComponentByEntityHandle(character->Base.EntityObjectHandle);

			auto & stat = statSyncMsg.Stats.Set.Buf[0];
			stat.NetId = netComponent->NetID;
			stat.Stats.Init(0x25);

			if (stats != nullptr) {
				stats->StatValues.Iterate([&stat](FixedString key, int value) {
					stat.Stats.Add(key, value);
				});
			}

			stat.Stats.Add(statKey, statValue);

			ProcessCustomStatsMessage(&statSyncMsg);
		}

		void CharacterSetCustomStat(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args[0].String);
			auto statId = args[1].String;
			auto statValue = args[2].Int32;

			auto statDefn = FindCustomStatDefinitionById(statId);
			if (statDefn == nullptr) return;

			auto entityWorld = GetEntityWorld();
			auto statsComponent = entityWorld->GetCustomStatsComponentByEntityHandle(character->Base.EntityObjectHandle);
			
			CharacterSyncCustomStats(character, statsComponent, statDefn->Id, statValue);
		}

		void CreateCustomStatInternal(char const * name, char const * description)
		{
			eocnet::CustomStatsDefinitionSyncMessage msg;
			msg.MessageId = NetMessage::NETMSG_CUSTOM_STATS_DEFINITION_CREATE;

			// FIXME - memory leak!
			msg.StatDefns.Set.Buf = GameAlloc<eocnet::CustomStatDefinitionSyncInfo>(1);
			msg.StatDefns.Set.Capacity = 1;
			msg.StatDefns.Set.Size = 1;
			
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wstrName = converter.from_bytes(name);
			std::wstring wstrDescription = converter.from_bytes(description);

			auto & defn = msg.StatDefns.Set.Buf[0];
			defn.Name.Set(wstrName);
			defn.Description.Set(wstrDescription);

			ProcessCustomStatsMessage(&msg);
		}

		bool CreateCustomStat(OsiArgumentDesc & args)
		{
			auto statName = args[0].String;
			auto statDescription = args[1].String;
			auto & statId = args[2].String;

			auto statDefn = FindCustomStatDefinitionByName(statName);
			if (statDefn != nullptr) {
				OsiWarn("Custom stat '" << statName << "' already exists!");
				statId = statDefn->Id.Str;
				return true;
			}

			CreateCustomStatInternal(statName, statDescription);

			statDefn = FindCustomStatDefinitionByName(statName);
			if (statDefn != nullptr) {
				statId = statDefn->Id.Str;
				return true;
			} else {
				OsiWarn("Unable to create custom stat '" << statName << "' - not found after create message ?!");
				return false;
			}
		}

		bool GetCustomStat(OsiArgumentDesc & args)
		{
			auto statName = args[0].String;
			auto & statId = args[1].String;

			auto statDefn = FindCustomStatDefinitionByName(statName);
			if (statDefn != nullptr) {
				statId = statDefn->Id.Str;
				return true;
			} else {
				OsiWarn("Custom stat '" << statName << "' does not exist!");
				return false;
			}
		}
	}

	void CustomFunctionLibrary::RegisterCharacterFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto characterGetStatInt = std::make_unique<CustomQuery>(
			"NRD_CharacterGetStatInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetStat<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(characterGetStatInt));

		auto characterSetStatInt = std::make_unique<CustomCall>(
			"NRD_CharacterSetStatInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterSetStatInt
		);
		functionMgr.Register(std::move(characterSetStatInt));


		auto characterGetPermanentBoostInt = std::make_unique<CustomQuery>(
			"NRD_CharacterGetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(characterGetPermanentBoostInt));


		auto characterSetPermanentBoostInt = std::make_unique<CustomCall>(
			"NRD_CharacterSetPermanentBoostInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Stat", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterSetPermanentBoost<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(characterSetPermanentBoostInt));


		auto characterIsTalentDisabled = std::make_unique<CustomQuery>(
			"NRD_CharacterIsTalentDisabled",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "IsDisabled", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterIsTalentDisabled
		);
		functionMgr.Register(std::move(characterIsTalentDisabled));


		auto characterDisableTalent = std::make_unique<CustomCall>(
			"NRD_CharacterDisableTalent",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "IsDisabled", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterDisableTalent
		);
		functionMgr.Register(std::move(characterDisableTalent));


		auto characterGetCustomStat = std::make_unique<CustomQuery>(
			"NRD_CharacterGetCustomStat",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatId", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out },
			},
			&func::CharacterGetCustomStat
		);
		functionMgr.Register(std::move(characterGetCustomStat));


		auto characterSetCustomStat = std::make_unique<CustomCall>(
			"NRD_CharacterSetCustomStat",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "StatId", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In },
			},
			&func::CharacterSetCustomStat
		);
		functionMgr.Register(std::move(characterSetCustomStat));


		auto createCustomStat = std::make_unique<CustomQuery>(
			"NRD_CreateCustomStat",
			std::vector<CustomFunctionParam>{
				{ "Name", ValueType::String, FunctionArgumentDirection::In },
				{ "Description", ValueType::String, FunctionArgumentDirection::In },
				{ "StatId", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::CreateCustomStat
		);
		functionMgr.Register(std::move(createCustomStat));


		auto getCustomStat = std::make_unique<CustomQuery>(
			"NRD_GetCustomStat",
			std::vector<CustomFunctionParam>{
				{ "Name", ValueType::String, FunctionArgumentDirection::In },
				{ "StatId", ValueType::GuidString, FunctionArgumentDirection::Out },
			},
			&func::GetCustomStat
		);
		functionMgr.Register(std::move(getCustomStat));
	}

}
