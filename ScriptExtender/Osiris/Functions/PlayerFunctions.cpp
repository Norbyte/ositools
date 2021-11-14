#include <stdafx.h>
#include "FunctionLibrary.h"
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse::esv
{
	namespace func
	{
		esv::Skill * CharacterGetSkill(char const * characterGuid, char const * skillId)
		{
			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) {
				OsiError("Character '" << characterGuid << "' does not exist!");
				return nullptr;
			}

			if (character->SkillManager == nullptr) {
				OsiError("Character '" << characterGuid << "' has no SkillManager!");
				return nullptr;
			}

			auto skill = character->SkillManager->Skills.Find(FixedString(skillId));
			if (skill == nullptr) {
				OsiError("Character '" << characterGuid << "' doesn't have skill '" << skillId << "'!");
				return nullptr;
			}

			return *skill;
		}

		bool SkillGetCooldown(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto skillId = args[1].String;

			auto skill = CharacterGetSkill(characterGuid, skillId);
			if (skill == nullptr) return false;

			args[2].Set(skill->ActiveCooldown);
			return true;
		}

		void SkillSetCooldown(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto skillId = args[1].String;

			auto skill = CharacterGetSkill(characterGuid, skillId);
			if (skill == nullptr) return;

			if (skill->OncePerCombat) {
				OsiError("Skill '" << skillId << " doesn't support cooldown!");
				return;
			}

			auto cooldown = args[2].Float;
			if (cooldown < 0.0f) {
				OsiErrorS("Cooldown cannot be negative!");
				return;
			}

			skill->ActiveCooldown = cooldown;
		}

		bool SkillGetInt(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto skillId = args[1].String;
			auto flag = args[2].String;
			auto & value = args[3].Int32;

			auto skill = CharacterGetSkill(characterGuid, skillId);
			if (skill == nullptr) return false;

			if (strcmp(flag, "IsActivated") == 0) {
				value = skill->IsActivated ? 1 : 0;
			} else if (strcmp(flag, "IsLearned") == 0) {
				value = skill->IsLearned ? 1 : 0;
			} else if (strcmp(flag, "ZeroMemory") == 0) {
				value = skill->ZeroMemory ? 1 : 0;
			} else if (strcmp(flag, "OncePerCombat") == 0) {
				value = skill->OncePerCombat ? 1 : 0;
			} else if (strcmp(flag, "Charges") == 0) {
				value = skill->NumCharges;
			} else {
				OsiError("Couldn't fetch unknown skill flag: " << flag);
				return false;
			}

			return true;
		}

		ObjectSet<esv::SkillBarItem> * GetSkillBar(char const * characterGuid)
		{
			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			if (character == nullptr) {
				OsiError("Character '" << characterGuid << "' does not exist!");
				return nullptr;
			}

			if (character->PlayerData == nullptr
				|| character->PlayerData->SkillBar.Size == 0) {
				OsiError("Character '" << characterGuid << "' has no skill bar!");
				return nullptr;
			}

			return &character->PlayerData->SkillBar;
		}

		esv::SkillBarItem * SkillBarGetSlot(char const * characterGuid, int slot)
		{
			auto skillBar = GetSkillBar(characterGuid);
			if (skillBar == nullptr) return nullptr;

			if (slot < 0 || slot >= (int)skillBar->Size) {
				OsiError("Invalid skill bar slot index: " << slot);
				return nullptr;
			}

			return &(*skillBar)[slot];
		}

		bool SkillBarGetItem(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto slot = args[1].Int32;
			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return false;

			if (skillBarItem->Type == esv::SkillBarItemType::Item) {
				auto item = GetEntityWorld()->GetItem(skillBarItem->ItemHandle);
				if (item != nullptr) {
					args[2].Set(item->MyGuid.Str);
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}

		bool SkillBarGetSkill(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto slot = args[1].Int32;
			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return false;

			if (skillBarItem->Type == esv::SkillBarItemType::Skill) {
				args[2].Set(skillBarItem->SkillOrStatId.Str);
				return true;
			} else {
				return false;
			}
		}

		bool SkillBarFindSkill(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto skillBar = GetSkillBar(characterGuid);
			if (skillBar == nullptr) return false;

			FixedString skillId(args[1].String);
			for (uint32_t i = 0; i < skillBar->Size; i++) {
				auto & skill = (*skillBar)[i];
				if (skill.Type == esv::SkillBarItemType::Skill
					&& skill.SkillOrStatId == skillId) {
					args[2].Set((int32_t)i);
					return true;
				}
			}

			return false;
		}

		bool SkillBarFindItem(OsiArgumentDesc & args)
		{
			auto characterGuid = args[0].String;
			auto skillBar = GetSkillBar(characterGuid);
			if (skillBar == nullptr) return false;

			auto itemGuid = args[1].String;
			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return false;
			}

			ComponentHandle handle;
			item->GetObjectHandle(handle);

			for (uint32_t i = 0; i < skillBar->Size; i++) {
				auto & skill = (*skillBar)[i];
				if (skill.Type == esv::SkillBarItemType::Item
					&& skill.ItemHandle == handle) {
					args[2].Set((int32_t)i);
					return true;
				}
			}

			return false;
		}

		void SkillBarSetSkill(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto slot = args[1].Int32;
			auto skillId = args[2].String;
			FixedString skillIdFs(skillId);

			auto * stats = GetStaticSymbols().GetStats();
			FixedString skillDataFs("SkillData");
			if (!stats->ObjectExists(skillIdFs, skillDataFs)) {
				OsiError("'" << skillId << "' is not a valid skill ID!");
				return;
			}

			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return;

			skillBarItem->Type = esv::SkillBarItemType::Skill;
			skillBarItem->SkillOrStatId = skillIdFs;
			skillBarItem->ItemHandle = ComponentHandle{};

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			character->PlayerData->ShouldReevaluateSkillBar = true;
		}

		void SkillBarSetItem(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto slot = args[1].Int32;
			auto itemGuid = args[2].String;

			auto item = GetEntityWorld()->GetItem(itemGuid);
			if (item == nullptr) {
				OsiError("Item '" << itemGuid << "' does not exist!");
				return;
			}

			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return;

			ComponentHandle handle;
			item->GetObjectHandle(handle);
			// FIXME - check if item is in the players' inventory?

			skillBarItem->Type = esv::SkillBarItemType::Item;
			skillBarItem->SkillOrStatId = item->StatsId;
			skillBarItem->ItemHandle = handle;

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			character->PlayerData->ShouldReevaluateSkillBar = true;
		}

		void SkillBarClear(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto slot = args[1].Int32;

			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return;

			skillBarItem->Type = esv::SkillBarItemType::None;
			skillBarItem->SkillOrStatId = FixedString("");
			skillBarItem->ItemHandle = ComponentHandle{};

			auto character = GetEntityWorld()->GetCharacter(characterGuid);
			character->PlayerData->ShouldReevaluateSkillBar = true;
		}

		esv::Character * FindPlayerByNameGuid(char const * guid)
		{
			auto character = GetEntityWorld()->GetCharacter(guid);
			if (character == nullptr) return nullptr;

			if (character->PlayerData == nullptr) {
				OsiError("Character " << guid << " is not a player!");
				return nullptr;
			}

			return character;
		}

		void PlayerSetBaseAttribute(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto attributeName = args[1].String;
			auto attributeValue = args[2].Int32;

			auto character = FindPlayerByNameGuid(characterGuid);
			if (character == nullptr) return;

			auto attribute = EnumInfo<PlayerUpgradeAttribute>::Find(attributeName);
			if (!attribute) {
				OsiError("Unknown character attribute: " << attributeName);
				return;
			}

			character->PlayerUpgrade.Attributes[(uint32_t)*attribute] = attributeValue;
			character->PlayerUpgrade.IsCustom = true;

			auto baseStats = character->Stats->DynamicStats[0];
			switch (*attribute) {
			case PlayerUpgradeAttribute::Strength: baseStats->Strength = attributeValue; break;
			case PlayerUpgradeAttribute::Finesse: baseStats->Finesse = attributeValue; break;
			case PlayerUpgradeAttribute::Intelligence: baseStats->Intelligence = attributeValue; break;
			case PlayerUpgradeAttribute::Constitution: baseStats->Constitution = attributeValue; break;
			case PlayerUpgradeAttribute::Memory: baseStats->Memory = attributeValue; break;
			case PlayerUpgradeAttribute::Wits: baseStats->Wits = attributeValue; break;
			default:
				OsiError("Unknown PlayerUpgradeAttribute " << (unsigned)*attribute);
				break;
			}
		}

		void PlayerSetBaseAbility(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto abilityName = args[1].String;
			auto abilityValue = args[2].Int32;

			auto character = FindPlayerByNameGuid(characterGuid);
			if (character == nullptr) return;

			auto ability = EnumInfo<AbilityType>::Find(abilityName);
			if (!ability) {
				OsiError("Unknown ability: " << abilityName);
				return;
			}

			character->PlayerUpgrade.Abilities[(uint32_t)*ability] = abilityValue;
			character->PlayerUpgrade.IsCustom = true;

			auto baseStats = character->Stats->DynamicStats[0];
			baseStats->Abilities[(uint32_t)*ability] = abilityValue;
		}

		void PlayerSetBaseTalent(OsiArgumentDesc const & args)
		{
			auto characterGuid = args[0].String;
			auto talentName = args[1].String;
			auto talentValue = args[2].Int32;

			auto character = FindPlayerByNameGuid(characterGuid);
			if (character == nullptr) return;

			auto talent = EnumInfo<TalentType>::Find(talentName);
			if (!talent) {
				OsiError("Unknown talent type: " << talentName);
				return;
			}

			auto baseStats = character->Stats->DynamicStats[0];

			if (talentValue != 0) {
				character->PlayerUpgrade.Talents.Set((uint32_t)*talent);
				baseStats->Talents.Set((uint32_t)*talent);
			} else {
				character->PlayerUpgrade.Talents.Clear((uint32_t)*talent);
				baseStats->Talents.Clear((uint32_t)*talent);
			}

			character->PlayerUpgrade.IsCustom = true;
		}

		template <OsiPropertyMapType Type>
		bool PlayerGetCustomData(OsiArgumentDesc & args)
		{
			auto player = FindPlayerByNameGuid(args[0].String);
			if (player == nullptr) return false;

			if (!player->PlayerData->CustomData.Initialized) {
				OsiError("Player custom data for character '" << args[0].String << "' has not been initialized!");
				return false;

			}

			return OsirisPropertyMapGet<eoc::PlayerCustomData>(gPlayerCustomDataPropertyMap, &player->PlayerData->CustomData, args, 1, Type);
		}

		template <OsiPropertyMapType Type>
		void PlayerSetCustomData(OsiArgumentDesc const & args)
		{
			auto player = FindPlayerByNameGuid(args[0].String);
			if (player == nullptr) return;

			if (!player->PlayerData->CustomData.Initialized) {
				OsiError("Player custom data for character '" << args[0].String << "' has not been initialized!");
				return;
			}

			OsirisPropertyMapSet<eoc::PlayerCustomData>(gPlayerCustomDataPropertyMap, &player->PlayerData->CustomData, args, 1, Type);
			player->PlayerData->ShouldReevaluateSkillBar = true;
		}
	}

	void CustomFunctionLibrary::RegisterPlayerFunctions()
	{
		auto & functionMgr = osiris_.GetCustomFunctionManager();
		auto skillGetCooldown = std::make_unique<CustomQuery>(
			"NRD_SkillGetCooldown",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "Cooldown", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::SkillGetCooldown
		);
		functionMgr.Register(std::move(skillGetCooldown));

		auto skillSetCooldown = std::make_unique<CustomCall>(
			"NRD_SkillSetCooldown",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "Cooldown", ValueType::Real, FunctionArgumentDirection::In }
			},
			&func::SkillSetCooldown
		);
		functionMgr.Register(std::move(skillSetCooldown));

		auto skillGetInt = std::make_unique<CustomQuery>(
			"NRD_SkillGetInt",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::SkillGetInt
		);
		functionMgr.Register(std::move(skillGetInt));

		auto skillBarGetItem = std::make_unique<CustomQuery>(
			"NRD_SkillBarGetItem",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::Out }
			},
			&func::SkillBarGetItem
		);
		functionMgr.Register(std::move(skillBarGetItem));

		auto skillBarGetSkill = std::make_unique<CustomQuery>(
			"NRD_SkillBarGetSkill",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Skill", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::SkillBarGetSkill
		);
		functionMgr.Register(std::move(skillBarGetSkill));

		auto skillBarFindSkill = std::make_unique<CustomQuery>(
			"NRD_SkillBarFindSkill",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Skill", ValueType::String, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::SkillBarFindSkill
		);
		functionMgr.Register(std::move(skillBarFindSkill));

		auto skillBarFindItem = std::make_unique<CustomQuery>(
			"NRD_SkillBarFindItem",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::SkillBarFindItem
		);
		functionMgr.Register(std::move(skillBarFindItem));

		auto skillBarSetSkill = std::make_unique<CustomCall>(
			"NRD_SkillBarSetSkill",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::SkillBarSetSkill
		);
		functionMgr.Register(std::move(skillBarSetSkill));

		auto skillBarSetItem = std::make_unique<CustomCall>(
			"NRD_SkillBarSetItem",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Item", ValueType::ItemGuid, FunctionArgumentDirection::In }
			},
			&func::SkillBarSetItem
		);
		functionMgr.Register(std::move(skillBarSetItem));

		auto skillBarClear = std::make_unique<CustomCall>(
			"NRD_SkillBarClear",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::SkillBarClear
		);
		functionMgr.Register(std::move(skillBarClear));

		auto playerSetBaseAttribute = std::make_unique<CustomCall>(
			"NRD_PlayerSetBaseAttribute",
			std::vector<CustomFunctionParam>{
				{ "Player", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Attribute", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::PlayerSetBaseAttribute
		);
		functionMgr.Register(std::move(playerSetBaseAttribute));

		auto playerSetBaseAbility = std::make_unique<CustomCall>(
			"NRD_PlayerSetBaseAbility",
			std::vector<CustomFunctionParam>{
				{ "Player", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Ability", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::PlayerSetBaseAbility
		);
		functionMgr.Register(std::move(playerSetBaseAbility));

		auto playerSetBaseTalent = std::make_unique<CustomCall>(
			"NRD_PlayerSetBaseTalent",
			std::vector<CustomFunctionParam>{
				{ "Player", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Talent", ValueType::String, FunctionArgumentDirection::In },
				{ "HasTalent", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::PlayerSetBaseTalent
		);
		functionMgr.Register(std::move(playerSetBaseTalent));

		auto playerGetCustomDataInt = std::make_unique<CustomQuery>(
			"NRD_PlayerGetCustomDataInt",
			std::vector<CustomFunctionParam>{
				{ "Player", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::PlayerGetCustomData<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(playerGetCustomDataInt));

		auto playerGetCustomDataString = std::make_unique<CustomQuery>(
			"NRD_PlayerGetCustomDataString",
			std::vector<CustomFunctionParam>{
				{ "Player", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::PlayerGetCustomData<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(playerGetCustomDataString));

		auto playerSetCustomDataInt = std::make_unique<CustomCall>(
			"NRD_PlayerSetCustomDataInt",
			std::vector<CustomFunctionParam>{
				{ "Player", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::PlayerSetCustomData<OsiPropertyMapType::Integer>
		);
		functionMgr.Register(std::move(playerSetCustomDataInt));

		auto playerSetCustomDataString = std::make_unique<CustomCall>(
			"NRD_PlayerSetCustomDataString",
			std::vector<CustomFunctionParam>{
				{ "Player", ValueType::CharacterGuid, FunctionArgumentDirection::In },
				{ "Property", ValueType::String, FunctionArgumentDirection::In },
				{ "Value", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::PlayerSetCustomData<OsiPropertyMapType::String>
		);
		functionMgr.Register(std::move(playerSetCustomDataString));
	}

}
