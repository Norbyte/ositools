template <class Predicate>
void GetCharacterGuidsGeneric(ObjectSet<FixedString> guids, FixedString const& requestedLevel, Predicate pred)
{
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->CharacterConversionHelpers;
	auto characters = helpers.RegisteredCharacters.Find(levelName);
	if (characters == nullptr) {
		OsiError("No characters registered for level: " << levelName);
		return;
	}

	for (auto character : **characters) {
		if (pred(character)) {
			guids.push_back(character->MyGuid);
		}
	}
}


template <class Predicate>
void GetCharactersGeneric(ObjectSet<Character *>& characters, FixedString const& requestedLevel, Predicate pred)
{
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->CharacterConversionHelpers;
	auto levelCharacters = helpers.RegisteredCharacters.Find(levelName);
	if (levelCharacters == nullptr) {
		OsiError("No characters registered for level: " << levelName);
		return;
	}

	for (auto character : **levelCharacters) {
		if (pred(character)) {
			characters.push_back(character);
		}
	}
}

template <class Predicate>
void GetItemGuidsGeneric(ObjectSet<FixedString>& guids, FixedString const& requestedLevel, Predicate pred)
{
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->ItemConversionHelpers;
	auto items = helpers.RegisteredItems.Find(levelName);
	if (items == nullptr) {
		OsiError("No items registered for level: " << levelName);
		return;
	}

	for (auto item : **items) {
		if (pred(item)) {
			guids.push_back(item->MyGuid);
		}
	}
}

template <class Predicate>
void GetItemsGeneric(ObjectSet<Item*>& resultItems, FixedString const& requestedLevel, Predicate pred)
{
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->ItemConversionHelpers;
	auto items = helpers.RegisteredItems.Find(levelName);
	if (items == nullptr) {
		OsiError("No items registered for level: " << levelName);
		return;
	}

	for (auto item : **items) {
		if (pred(item)) {
			resultItems.push_back(item);
		}
	}
}

template <class Predicate>
void GetTriggerGuidsGeneric(ObjectSet<FixedString>& guids, FixedString const& requestedLevel, Predicate pred)
{
	FixedString levelName = requestedLevel;
	if (!levelName) {
		auto level = GetStaticSymbols().GetCurrentServerLevel();
		if (level == nullptr) {
			OsiError("No current level!");
			return;
		}

		levelName = level->LevelDesc->LevelName;
	}

	auto& helpers = GetEoCServer()->EntityManager->TriggerConversionHelpers;
	auto triggers = helpers.RegisteredTriggers.Find(levelName);
	if (triggers == nullptr) {
		OsiError("No triggers registered for level: " << levelName);
		return;
	}

	// FIXME - re-add when migrated to new proxy
	/*for (auto trigger : **triggers) {
		if (pred(trigger)) {
			auto guid = trigger->GetGuid();
			if (guid && *guid) {
				settable(L, index++, *guid);
			}
		}
	}*/
}
