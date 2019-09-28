#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	namespace func
	{
		bool ItemGetStatsId(OsiArgumentDesc & args)
		{
			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("ItemGetStatsId(): Item '" << itemGuid << "' does not exist!");
				return false;
			}

			if (!item->StatsId.Str) {
				OsiError("ItemGetStatsId(): Item '" << itemGuid << "' has no stats ID!");
				return false;
			}
			else {
				args.Get(1).String = item->StatsId.Str;
				return true;
			}
		}

		void ItemSetIdentified(OsiArgumentDesc const & args)
		{
			auto itemGuid = args.Get(0).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("ItemSetIdentified(): Item '" << itemGuid << "' does not exist!");
				return;
			}

			item->StatsDynamic->IsIdentified = args.Get(1).Int32 ? 1 : 0;
		}

		bool SkillGetCooldown(OsiArgumentDesc & args)
		{
			auto characterGuid = args.Get(0).String;
			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				OsiError("SkillGetCooldown(): Character '" << characterGuid << "' does not exist!");
				return false;
			}

			if (character->SkillManager == nullptr) {
				OsiError("SkillGetCooldown(): Character '" << characterGuid << "' has no SkillManager!");
				return false;
			}

			auto skillId = args.Get(1).String;
			auto skill = character->SkillManager->Skills.Find(skillId);
			if (skill == nullptr) {
				OsiError("SkillGetCooldown(): Character '" << characterGuid << "' doesn't have skill '" << skillId << "'!");
				return false;
			}

			args.Get(2).Float = (*skill)->ActiveCooldown;
			return true;
		}

		void SkillSetCooldown(OsiArgumentDesc const & args)
		{
			auto characterGuid = args.Get(0).String;
			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				OsiError("SkillSetCooldown(): Character '" << characterGuid << "' does not exist!");
				return;
			}

			if (character->SkillManager == nullptr) {
				OsiError("SkillSetCooldown(): Character '" << characterGuid << "' has no SkillManager!");
				return;
			}

			auto skillId = args.Get(1).String;
			auto skill = character->SkillManager->Skills.Find(skillId);
			if (skill == nullptr) {
				OsiError("SkillSetCooldown(): Character '" << characterGuid << "' doesn't have skill '" << skillId << "'!");
				return;
			}

			if ((*skill)->OncePerCombat) {
				OsiError("SkillSetCooldown(): Skill '" << skillId << " doesn't support cooldown!");
				return;
			}

			auto cooldown = args.Get(2).Float;
			if (cooldown < 0.0f) {
				OsiError("SkillSetCooldown(): Cooldown cannot be negative!");
				return;
			}

			(*skill)->ActiveCooldown = cooldown;
		}

		ObjectSet<EsvSkillBarItem> * GetSkillBar(char const * characterGuid)
		{
			auto character = FindCharacterByNameGuid(characterGuid);
			if (character == nullptr) {
				OsiError("GetSkillBar(): Character '" << characterGuid << "' does not exist!");
				return nullptr;
			}

			if (character->PlayerData == nullptr
				|| character->PlayerData->SkillBar.Size == 0) {
				OsiError("GetSkillBar(): Character '" << characterGuid << "' has no skill bar!");
				return nullptr;
			}

			return &character->PlayerData->SkillBar;
		}

		EsvSkillBarItem * SkillBarGetSlot(char const * characterGuid, int slot)
		{
			auto skillBar = GetSkillBar(characterGuid);
			if (skillBar == nullptr) return nullptr;

			if (slot < 0 || slot >= (int)skillBar->Size) {
				OsiError("SkillBarGetSlot(): Invalid skill bar slot index: " << slot);
				return nullptr;
			}

			return &skillBar->Buf[slot];
		}

		bool SkillBarGetItem(OsiArgumentDesc & args)
		{
			auto characterGuid = args.Get(0).String;
			auto slot = args.Get(1).Int32;
			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return false;

			if (skillBarItem->Type == EsvSkillBarItem::kItem) {
				auto item = FindItemByHandle(skillBarItem->ItemHandle);
				if (item != nullptr) {
					args.Get(2).String = item->MyGuid.Str;
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
			auto characterGuid = args.Get(0).String;
			auto slot = args.Get(1).Int32;
			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return false;

			if (skillBarItem->Type == EsvSkillBarItem::kSkill) {
				args.Get(2).String = skillBarItem->SkillOrStatId.Str;
				return true;
			} else {
				return false;
			}
		}

		bool SkillBarFindSkill(OsiArgumentDesc & args)
		{
			auto characterGuid = args.Get(0).String;
			auto skillBar = GetSkillBar(characterGuid);
			if (skillBar == nullptr) return false;

			auto skillId = ToFixedString(args.Get(1).String);
			for (uint32_t i = 0; i < skillBar->Size; i++) {
				auto & skill = skillBar->Buf[i];
				if (skill.Type == EsvSkillBarItem::kSkill
					&& skill.SkillOrStatId == skillId) {
					args.Get(2).Int32 = i;
					return true;
				}
			}

			return false;
		}

		bool SkillBarFindItem(OsiArgumentDesc & args)
		{
			auto characterGuid = args.Get(0).String;
			auto skillBar = GetSkillBar(characterGuid);
			if (skillBar == nullptr) return false;

			auto itemGuid = args.Get(1).String;
			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("SkillBarFindItem(): Item '" << itemGuid << "' does not exist!");
				return false;
			}

			ObjectHandle handle;
			item->GetObjectHandle(&handle);

			for (uint32_t i = 0; i < skillBar->Size; i++) {
				auto & skill = skillBar->Buf[i];
				if (skill.Type == EsvSkillBarItem::kItem
					&& skill.ItemHandle == handle) {
					args.Get(2).Int32 = i;
					return true;
				}
			}

			return false;
		}

		void SkillBarSetSkill(OsiArgumentDesc const & args)
		{
			auto characterGuid = args.Get(0).String;
			auto slot = args.Get(1).Int32;
			auto skillId = ToFixedString(args.Get(2).String);

			auto * stats = gOsirisProxy->GetLibraryManager().GetStats();
			auto skillDataFs = ToFixedString("SkillData");
			if (!stats->ObjectExists(skillId, skillDataFs)) {
				OsiError("SkillBarSetSkill(): '" << skillId.Str << "' is not a valid skill ID!");
				return;
			}

			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return;

			skillBarItem->Type = EsvSkillBarItem::kSkill;
			skillBarItem->SkillOrStatId = skillId;
			skillBarItem->ItemHandle = ObjectHandle{};

			auto character = FindCharacterByNameGuid(characterGuid);
			character->PlayerData->Dirty = true;
		}

		void SkillBarSetItem(OsiArgumentDesc const & args)
		{
			auto characterGuid = args.Get(0).String;
			auto slot = args.Get(1).Int32;
			auto itemGuid = args.Get(2).String;

			auto item = FindItemByNameGuid(itemGuid);
			if (item == nullptr) {
				OsiError("SkillBarSetItem(): Item '" << itemGuid << "' does not exist!");
				return;
			}

			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return;

			ObjectHandle handle;
			item->GetObjectHandle(&handle);
			// FIXME - check if item is in the players' inventory?

			skillBarItem->Type = EsvSkillBarItem::kItem;
			skillBarItem->SkillOrStatId = item->StatsId;
			skillBarItem->ItemHandle = handle;

			auto character = FindCharacterByNameGuid(characterGuid);
			character->PlayerData->Dirty = true;
		}

		void SkillBarClear(OsiArgumentDesc const & args)
		{
			auto characterGuid = args.Get(0).String;
			auto slot = args.Get(1).Int32;

			auto skillBarItem = SkillBarGetSlot(characterGuid, slot);
			if (skillBarItem == nullptr) return;

			skillBarItem->Type = EsvSkillBarItem::kNone;
			skillBarItem->SkillOrStatId = ToFixedString("");
			skillBarItem->ItemHandle = ObjectHandle{};

			auto character = FindCharacterByNameGuid(characterGuid);
			character->PlayerData->Dirty = true;
		}

		void BreakOnCharacter(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			OsiWarn("GotIt");
		}

		void BreakOnItem(OsiArgumentDesc const & args)
		{
			auto item = FindItemByNameGuid(args.Get(0).String);
			OsiWarn("GotIt");
		}

		void DoExperiment(OsiArgumentDesc const & args)
		{
			auto character = FindCharacterByNameGuid(args.Get(0).String);
			if (character) {
				auto stats = character->Stats->DynamicStats[1];
				stats->PoisonResistance += 50;
				stats->APStart += 3;
				stats->APRecovery += 3;

				auto stats0 = character->Stats->DynamicStats[0];
				stats0->PoisonResistance += 50;
				OsiError("DoExperiment(): Applied to character");
			}

			auto item = FindItemByNameGuid(args.Get(1).String);
			if (item) {
				auto stats = item->StatsDynamic->DynamicAttributes_Start[1];
				stats->FireResistance += 50;

				auto stats0 = item->StatsDynamic->DynamicAttributes_Start[0];
				stats0->FireResistance += 50;
				OsiError("DoExperiment(): Applied to item");
			}

			OsiError("Nothing to see here");
		}
	}

	CustomFunctionLibrary::CustomFunctionLibrary(class OsirisProxy & osiris)
		: osiris_(osiris)
	{}

	void CustomFunctionLibrary::Register()
	{
		RegisterHelperFunctions();
		RegisterMathFunctions();
		RegisterStatFunctions();
		RegisterStatusFunctions();
		RegisterGameActionFunctions();
		RegisterProjectileFunctions();
		RegisterHitFunctions();

		auto & functionMgr = osiris_.GetCustomFunctionManager();

		auto itemGetStatsId = std::make_unique<CustomQuery>(
			"NRD_ItemGetStatsId",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "StatsId", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::ItemGetStatsId
		);
		functionMgr.Register(std::move(itemGetStatsId));


		auto itemSetIdentified = std::make_unique<CustomCall>(
			"NRD_ItemSetIdentified",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "IsIdentified", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::ItemSetIdentified
		);
		functionMgr.Register(std::move(itemSetIdentified));


		auto skillGetCooldown = std::make_unique<CustomQuery>(
			"NRD_SkillGetCooldown",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "Cooldown", ValueType::Real, FunctionArgumentDirection::Out }
			},
			&func::SkillGetCooldown
		);
		functionMgr.Register(std::move(skillGetCooldown));


		auto skillSetCooldown = std::make_unique<CustomCall>(
			"NRD_SkillSetCooldown",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In },
				{ "Cooldown", ValueType::Real, FunctionArgumentDirection::In }
			},
			&func::SkillSetCooldown
		);
		functionMgr.Register(std::move(skillSetCooldown));


		auto skillBarGetItem = std::make_unique<CustomQuery>(
			"NRD_SkillBarGetItem",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::Out }
			},
			&func::SkillBarGetItem
		);
		functionMgr.Register(std::move(skillBarGetItem));

		auto skillBarGetSkill = std::make_unique<CustomQuery>(
			"NRD_SkillBarGetSkill",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Skill", ValueType::String, FunctionArgumentDirection::Out }
			},
			&func::SkillBarGetSkill
		);
		functionMgr.Register(std::move(skillBarGetSkill));

		auto skillBarFindSkill = std::make_unique<CustomQuery>(
			"NRD_SkillBarFindSkill",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Skill", ValueType::String, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::SkillBarFindSkill
		);
		functionMgr.Register(std::move(skillBarFindSkill));

		auto skillBarFindItem = std::make_unique<CustomQuery>(
			"NRD_SkillBarFindItem",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::Out }
			},
			&func::SkillBarFindItem
		);
		functionMgr.Register(std::move(skillBarFindItem));

		auto skillBarSetSkill = std::make_unique<CustomCall>(
			"NRD_SkillBarSetSkill",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "SkillId", ValueType::String, FunctionArgumentDirection::In }
			},
			&func::SkillBarSetSkill
		);
		functionMgr.Register(std::move(skillBarSetSkill));

		auto skillBarSetItem = std::make_unique<CustomCall>(
			"NRD_SkillBarSetItem",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In },
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::SkillBarSetItem
		);
		functionMgr.Register(std::move(skillBarSetItem));

		auto skillBarClear = std::make_unique<CustomCall>(
			"NRD_SkillBarClear",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In },
				{ "Slot", ValueType::Integer, FunctionArgumentDirection::In }
			},
			&func::SkillBarClear
		);
		functionMgr.Register(std::move(skillBarClear));

		auto breakOnCharacter = std::make_unique<CustomCall>(
			"NRD_BreakOnCharacter",
			std::vector<CustomFunctionParam>{
				{ "Character", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::BreakOnCharacter
		);
		functionMgr.Register(std::move(breakOnCharacter));


		auto breakOnItem = std::make_unique<CustomCall>(
			"NRD_BreakOnItem",
			std::vector<CustomFunctionParam>{
				{ "Item", ValueType::GuidString, FunctionArgumentDirection::In }
			},
			&func::BreakOnItem
		);
		functionMgr.Register(std::move(breakOnItem));


		auto experiment = std::make_unique<CustomCall>(
			"NRD_Experiment",
			std::vector<CustomFunctionParam>{
				{ "Arg1", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg2", ValueType::String, FunctionArgumentDirection::In },
				{ "Arg3", ValueType::String, FunctionArgumentDirection::In },
			},
			&func::DoExperiment
		);
		functionMgr.Register(std::move(experiment));
	}

}
