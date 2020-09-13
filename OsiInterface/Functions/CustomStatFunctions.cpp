#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include <GameDefinitions/Symbols.h>
#include "PropertyMaps.h"

namespace dse::esv
{
	namespace func
	{
		esv::CustomStatDefinitionComponent * FindCustomStatDefinitionByName(char const * name)
		{
			STDWString wstrName = FromUTF8(name);

			auto entityWorld = GetEntityWorld();
			auto statSystem = entityWorld->GetCustomStatSystem();

			for (auto const& defn : statSystem->CreatedDefinitions) {
				auto statDefn = entityWorld->GetCustomStatDefinitionComponent(defn.Handle);
				if (statDefn != nullptr && statDefn->Name == wstrName) {
					return statDefn;
				}
			}

			for (auto const& defn : statSystem->InSyncDefinitions) {
				auto statDefn = entityWorld->GetCustomStatDefinitionComponent(defn.Handle);
				if (statDefn != nullptr && statDefn->Name == wstrName) {
					return statDefn;
				}
			}

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

			for (auto const& defn : statSystem->CreatedDefinitions) {
				auto statDefn = entityWorld->GetCustomStatDefinitionComponent(defn.Handle);
				if (statDefn != nullptr && statDefn->Id == fs) {
					return statDefn;
				}
			}

			for (auto const& defn : statSystem->InSyncDefinitions) {
				auto statDefn = entityWorld->GetCustomStatDefinitionComponent(defn.Handle);
				if (statDefn != nullptr && statDefn->Id == fs) {
					return statDefn;
				}
			}

			OsiError("Could not find custom stat definition '" << id << "'");
			return nullptr;
		}

		void ProcessCustomStatsMessage(net::Message * msg)
		{
			struct DummyType
			{
				uint16_t unkn1;
				uint16_t unkn2;
			};

			DummyType dummy1;
			dummy1.unkn2 = 0;

			auto processMsg = GetStaticSymbols().EsvCustomStatsProtocolProcessMsg;
			if (processMsg == nullptr) {
				OsiErrorS("esv::CustomStatsProtocol::ProcessMsg not found!");
				return;
			}

			processMsg(nullptr, nullptr, &dummy1, msg);
		}

		bool CharacterGetCustomStat(OsiArgumentDesc & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			auto statId = args[1].String;
			auto & statValue = args[2];

			if (character == nullptr) return false;

			auto statDefn = FindCustomStatDefinitionById(statId);
			if (statDefn == nullptr) return false;

			auto entityWorld = GetEntityWorld();
			auto statsComponent = entityWorld->GetCustomStatsComponentByEntityHandle(character->Base.EntityObjectHandle, false);
			if (statsComponent == nullptr) {
				// The game UI displays nonexistent stat entries as zero, 
				// so we'll do the same in the API
				statValue = 0;
				return true;
			}

			auto value = statsComponent->StatValues.Find(ToFixedString(statDefn->Id.Str));
			if (value == nullptr) {
				statValue.Set(0);
			} else {
				statValue.Set(*value);
			}

			return true;
		}

		void CharacterSyncCustomStats(esv::Character * character, eoc::CustomStatsComponent * stats,
			FixedString statKey, int statValue)
		{
			eocnet::CustomStatsSyncMessage statSyncMsg;
			if (stats == nullptr) {
				statSyncMsg.MsgId = NetMessage::NETMSG_CUSTOM_STATS_CREATE;
			} else {
				statSyncMsg.MsgId = NetMessage::NETMSG_CUSTOM_STATS_UPDATE;
			}

			// FIXME - memory leak!
			statSyncMsg.Stats.Buf = GameAllocArray<eocnet::CustomStatsSyncInfo>(1);
			statSyncMsg.Stats.Capacity = 1;
			statSyncMsg.Stats.Size = 1;

			auto entityWorld = GetEntityWorld();
			auto netComponent = entityWorld->GetNetComponentByEntityHandle(character->Base.EntityObjectHandle);

			auto & stat = statSyncMsg.Stats[0];
			stat.NetId = netComponent->NetID;
			stat.Stats.Init(0x25);

			if (stats != nullptr) {
				stats->StatValues.Iterate([&stat](FixedString const& key, int value) {
					stat.Stats.Insert(key, value);
				});
			}

			stat.Stats.Insert(statKey, statValue);

			ProcessCustomStatsMessage(&statSyncMsg);
		}

		void CharacterSetCustomStat(OsiArgumentDesc const & args)
		{
			auto character = GetEntityWorld()->GetCharacter(args[0].String);
			auto statId = args[1].String;
			auto statValue = args[2].Int32;

			if (character == nullptr) return;

			auto statDefn = FindCustomStatDefinitionById(statId);
			if (statDefn == nullptr) return;

			auto entityWorld = GetEntityWorld();
			auto statsComponent = entityWorld->GetCustomStatsComponentByEntityHandle(character->Base.EntityObjectHandle, false);

			CharacterSyncCustomStats(character, statsComponent, statDefn->Id, statValue);
		}

		void CreateCustomStatInternal(char const * name, char const * description)
		{
			eocnet::CustomStatsDefinitionSyncMessage msg;
			msg.MsgId = NetMessage::NETMSG_CUSTOM_STATS_DEFINITION_CREATE;

			// FIXME - memory leak!
			msg.StatDefns.Buf = GameAllocArray<eocnet::CustomStatDefinitionSyncInfo>(1);
			msg.StatDefns.Capacity = 1;
			msg.StatDefns.Size = 1;

			auto & defn = msg.StatDefns[0];
			defn.Name = FromUTF8(name);
			defn.Description = FromUTF8(description);

			ProcessCustomStatsMessage(&msg);
		}

		bool CreateCustomStat(OsiArgumentDesc & args)
		{
			auto statName = args[0].String;
			auto statDescription = args[1].String;
			auto & statId = args[2];

			auto statDefn = FindCustomStatDefinitionByName(statName);
			if (statDefn != nullptr) {
				OsiWarn("Custom stat '" << statName << "' already exists!");
				statId.Set(statDefn->Id.Str);
				return true;
			}

			CreateCustomStatInternal(statName, statDescription);

			statDefn = FindCustomStatDefinitionByName(statName);
			if (statDefn != nullptr) {
				statId.Set(statDefn->Id.Str);
				return true;
			} else {
				OsiWarn("Unable to create custom stat '" << statName << "' - not found after create message ?!");
				return false;
			}
		}

		bool GetCustomStat(OsiArgumentDesc & args)
		{
			auto statName = args[0].String;
			auto & statId = args[1];

			auto statDefn = FindCustomStatDefinitionByName(statName);
			if (statDefn != nullptr) {
				statId.Set(statDefn->Id.Str);
				return true;
			} else {
				OsiWarn("Custom stat '" << statName << "' does not exist!");
				return false;
			}
		}
	}

	void CustomFunctionLibrary::RegisterCustomStatFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();

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
