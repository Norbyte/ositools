#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse::esv
{
	std::optional<FixedString> CustomStatHelpers::CreateStat(char const* name, char const* description)
	{
		auto statDefn = FindStatDefinitionByName(name);
		if (statDefn != nullptr) {
			OsiWarn("Custom stat '" << name << "' already exists!");
			return statDefn->Id;
		}

		CreateStatInternal(name, description);

		statDefn = FindStatDefinitionByName(name);
		if (statDefn != nullptr) {
			return statDefn->Id;
		} else {
			OsiWarn("Unable to create custom stat '" << name << "' - not found after create message ?!");
			return {};
		}
	}

	CustomStatDefinitionComponent * CustomStatHelpers::FindStatDefinitionByName(char const * name)
	{
		STDWString wstrName = FromUTF8(name);

		auto entityWorld = GetEntityWorld();
		auto statSystem = entityWorld->GetCustomStatSystem();

		for (auto const& defn : statSystem->CreatedDefinitions) {
			auto statDefn = entityWorld->GetComponent<CustomStatDefinitionComponent>(defn.Handle);
			if (statDefn != nullptr && statDefn->Name == wstrName) {
				return statDefn;
			}
		}

		for (auto const& defn : statSystem->InSyncDefinitions) {
			auto statDefn = entityWorld->GetComponent<CustomStatDefinitionComponent>(defn.Handle);
			if (statDefn != nullptr && statDefn->Name == wstrName) {
				return statDefn;
			}
		}

		return nullptr;
	}

	CustomStatDefinitionComponent * CustomStatHelpers::FindStatDefinitionById(char const * id)
	{
		FixedString fs(id);

		auto entityWorld = GetEntityWorld();
		auto statSystem = entityWorld->GetCustomStatSystem();

		for (auto const& defn : statSystem->CreatedDefinitions) {
			auto statDefn = entityWorld->GetComponent<CustomStatDefinitionComponent>(defn.Handle);
			if (statDefn != nullptr && statDefn->Id == fs) {
				return statDefn;
			}
		}

		for (auto const& defn : statSystem->InSyncDefinitions) {
			auto statDefn = entityWorld->GetComponent<CustomStatDefinitionComponent>(defn.Handle);
			if (statDefn != nullptr && statDefn->Id == fs) {
				return statDefn;
			}
		}

		OsiError("Could not find custom stat definition '" << id << "'");
		return nullptr;
	}

	void CustomStatHelpers::ProcessMessage(net::Message * msg)
	{
		struct DummyType
		{
			uint16_t unkn1;
			uint16_t unkn2;
		};

		DummyType dummy1;
		dummy1.unkn2 = 0;

		auto processMsg = GetStaticSymbols().esv__CustomStatsProtocol__ProcessMsg;
		if (processMsg == nullptr) {
			OsiErrorS("esv::CustomStatsProtocol::ProcessMsg not found!");
			return;
		}

		processMsg(nullptr, nullptr, &dummy1, msg);
	}

	void CustomStatHelpers::SyncCharacterStats(EntityHandle entityHandle, eoc::CustomStatsComponent * stats,
		FixedString statKey, int statValue)
	{
		eocnet::CustomStatsSyncMessage statSyncMsg;
		if (stats == nullptr) {
			statSyncMsg.MsgId = NetMessage::NETMSG_CUSTOM_STATS_CREATE;
		} else {
			statSyncMsg.MsgId = NetMessage::NETMSG_CUSTOM_STATS_UPDATE;
		}

		auto entityWorld = GetEntityWorld();
		auto netComponent = entityWorld->GetComponent<NetComponent>(entityHandle);

		eocnet::CustomStatsSyncInfo stat;
		stat.NetId = netComponent->NetID;
		stat.Stats.ResizeHashtable(0x25);

		if (stats != nullptr) {
			for (auto const& val : stats->StatValues) {
				stat.Stats.insert(val.Key, val.Value);
			}
		}

		stat.Stats.insert(statKey, statValue);
		statSyncMsg.Stats.push_back(std::move(stat));

		ProcessMessage(&statSyncMsg);
	}

	void CustomStatHelpers::CreateStatInternal(char const * name, char const * description)
	{
		eocnet::CustomStatsDefinitionSyncMessage msg;
		msg.MsgId = NetMessage::NETMSG_CUSTOM_STATS_DEFINITION_CREATE;

		msg.StatDefns.resize(1);
		eocnet::CustomStatDefinitionSyncInfo defn = msg.StatDefns[0];
		defn.Name = FromUTF8(name);
		defn.Description = FromUTF8(description);
		msg.StatDefns.push_back(defn);

		ProcessMessage(&msg);
	}

	std::optional<int> CustomStatHelpers::GetCharacterStat(EntityHandle entityHandle, char const* statId)
	{
		if (!entityHandle) return {};

		auto statDefn = CustomStatHelpers::FindStatDefinitionById(statId);
		if (statDefn == nullptr) return {};

		auto entityWorld = GetEntityWorld();
		auto statsComponent = entityWorld->GetComponent<CustomStatsComponent>(entityHandle, false);
		if (statsComponent == nullptr) {
			// The game UI displays nonexistent stat entries as zero, 
			// so we'll do the same in the API
			return 0;
		}

		return statsComponent->StatValues.try_get(statDefn->Id, 0);
	}

	bool CustomStatHelpers::SetCharacterStat(EntityHandle entityHandle, char const* statId, int value)
	{
		if (!entityHandle) return false;

		auto statDefn = FindStatDefinitionById(statId);
		if (statDefn == nullptr) return false;

		auto entityWorld = GetEntityWorld();
		auto statsComponent = entityWorld->GetComponent<CustomStatsComponent>(entityHandle, false);

		SyncCharacterStats(entityHandle, statsComponent, statDefn->Id, value);
		return true;
	}

	namespace func
	{
		bool CharacterGetCustomStat(OsiArgumentDesc & args)
		{
			auto character = GetEntityWorld()->GetComponent<Character>(args[0].String);
			auto statId = args[1].String;
			auto & statValue = args[2];

			if (character == nullptr) return false;

			auto value = CustomStatHelpers::GetCharacterStat(character->Base.Entity, statId);
			if (value) {
				statValue.Set(*value);
				return true;
			} else {
				return false;
			}
		}

		void CharacterSetCustomStat(OsiArgumentDesc const & args)
		{
			auto character = GetEntityWorld()->GetComponent<Character>(args[0].String);
			auto statId = args[1].String;
			auto statValue = args[2].Int32;

			if (character == nullptr) return;

			CustomStatHelpers::SetCharacterStat(character->Base.Entity, statId, statValue);
		}

		bool CreateCustomStat(OsiArgumentDesc & args)
		{
			auto statName = args[0].String;
			auto statDescription = args[1].String;
			auto & statId = args[2];

			auto statsId = CustomStatHelpers::CreateStat(statName, statDescription);
			if (statsId) {
				statId.Set(*statsId);
				return true;
			} else {
				return false;
			}
		}

		bool GetCustomStat(OsiArgumentDesc & args)
		{
			auto statName = args[0].String;
			auto & statId = args[1];

			auto statDefn = CustomStatHelpers::FindStatDefinitionByName(statName);
			if (statDefn != nullptr) {
				statId.Set(statDefn->Id);
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

BEGIN_NS(ecl)

std::optional<int> CustomStatHelpers::GetCharacterStat(EntityHandle entityHandle, FixedString const& statId)
{
	if (!entityHandle) return {};

	auto entityWorld = GetEntityWorld();
	auto statsComponent = entityWorld->GetComponent<CustomStatsComponent>(entityHandle, false);
	if (statsComponent == nullptr) {
		// The game UI displays nonexistent stat entries as zero, 
		// so we'll do the same in the API
		return 0;
	}

	return statsComponent->StatValues.try_get(statId, 0);
}

END_NS()
