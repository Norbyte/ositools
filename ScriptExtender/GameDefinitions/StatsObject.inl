#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/Symbols.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_NS(lua)

stats::PropertyList* LuaToObjectPropertyList(lua_State* L, FixedString const& propertyName)
{
	auto properties = GetStaticSymbols().GetStats()->ConstructPropertyList(propertyName);
	LuaSerializer s(L, false);
	s << properties;
	return properties;
}

END_NS()

BEGIN_NS(stats)

ModifierList* Object::GetModifierList() const
{
	return GetStaticSymbols().GetStats()->ModifierLists.Find(ModifierListIndex);
}

std::optional<ModifierInfo> Object::GetModifierInfo(FixedString const& attributeName) const
{
	auto modifierList = GetModifierList();
	auto modifier = modifierList->GetModifierInfo(attributeName);
	if (!modifier) {
		OsiError("Stats entry '" << Name << "' (of type " << modifierList->Name << ") has no property named '" << attributeName << "'");
	}

	return modifier;
}

std::optional<char const*> Object::GetString(FixedString const& modifierName) const
{
	auto modifier = GetModifierInfo(modifierName);
	if (!modifier) return {};

	return GetString(*modifier);
}

std::optional<char const*> Object::GetString(ModifierInfo const& modifier) const
{
	auto index = IndexedProperties[modifier.Index];
	if (modifier.ValueList->Name == GFS.strFixedString) {
		auto val = GetStaticSymbols().GetStats()->GetFixedString(index);
		if (val) {
			return (*val)->GetString();
		} else {
			return "";
		}
	} else if (modifier.ValueList->Name == GFS.strConditions) {
		// FIXME - conditions not readable yet!
		OsiError("Conditions property '" << modifier.Modifier->Name << "' is not readable");
		return {};
	} else if (modifier.ValueList->Values.Count() > 0) {
		auto enumLabel = modifier.ValueList->Values.FindByValue(index);
		if (enumLabel) {
			return enumLabel->GetString();
		}
	}

	return {};
}

std::optional<int> Object::GetInt(FixedString const& modifierName) const
{
	auto modifier = GetModifierInfo(modifierName);
	if (!modifier) return {};

	return GetInt(*modifier);
}

std::optional<int> Object::GetInt(ModifierInfo const& modifier) const
{
	if (modifier.ValueList->Name == GFS.strConstantInt
		|| modifier.ValueList->Values.Count() > 0) {
		return IndexedProperties[modifier.Index];
	} else {
		return {};
	}
}

std::optional<int> Object::GetIntScaled(ModifierInfo const& modifier, int level) const
{
	auto stats = GetStaticSymbols().GetStats();
	auto levelMap = stats->LevelMaps.Find(modifier.Modifier->LevelMapIndex);
	auto value = IndexedProperties[modifier.Index];
	if (levelMap) {
		return (int32_t)levelMap->GetScaledValue(value, level);
	} else {
		return value;
	}
}

std::optional<uint64_t> Object::GetFlagsInt64(ModifierInfo const& modifier) const
{
	if (ValueList::IsFlagType(modifier.ValueList->Name)) {
		auto index = IndexedProperties[modifier.Index];
		auto flags = GetStaticSymbols().GetStats()->GetFlags(index);

		if (flags) {
			return **flags;
		} else {
			return 0;
		}
	}

	return {};
}

std::optional<ObjectSet<FixedString>> Object::GetFlags(ModifierInfo const& modifier) const
{
	ObjectSet<FixedString> flagSet;
	auto flags = GetFlagsInt64(modifier);
	if (!flags) {
		return {};
	}

	for (auto const& kv : modifier.ValueList->Values) {
		if (*flags & (1ull << (kv.Value - 1))) {
			flagSet.Add(kv.Key);
		}
	}

	return flagSet;
}

std::optional<PropertyList*> Object::GetPropertyList(ModifierInfo const& modifier) const
{
	if (modifier.ValueList->Name != GFS.strProperties) {
		return {};
	}

	auto properties = PropertyLists.Find(modifier.Modifier->Name);
	if (properties) {
		return *properties;
	} else {
		return {};
	}
}


bool Object::SetString(ModifierInfo const& modifier, const char * value)
{
	auto stats = GetStaticSymbols().GetStats();
	if (modifier.ValueList->Name == GFS.strFixedString) {
		int poolIdx{ -1 };
		auto fs = stats->GetOrCreateFixedString(poolIdx);
		if (fs != nullptr) {
			*fs = FixedString(value);
			IndexedProperties[modifier.Index] = poolIdx;
		}
	} else if (modifier.ValueList->Name == GFS.strConditions) {
		auto conditions = stats->CreateConditions(Name, modifier.Modifier->Name, value);
		Conditions.Insert(modifier.Modifier->Name, conditions);
	} else if (modifier.ValueList->Values.Count() > 0) {
		auto enumIndex = modifier.ValueList->Values.Find(FixedString(value));
		if (enumIndex != nullptr) {
			IndexedProperties[modifier.Index] = *enumIndex;
		} else {
			OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << ": Value (\"" << value << "\") is not a valid enum label");
			return false;
		}
	} else {
		OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << " to string value: Inappropriate type: " << modifier.ValueList->Name);
		return false;
	}

	return true;
}

bool Object::SetInt(ModifierInfo const& modifier, int32_t value)
{
	if (modifier.ValueList->Name == GFS.strConstantInt) {
		IndexedProperties[modifier.Index] = value;
	} else if (modifier.ValueList->Values.Count() > 0) {
		if (value >= 0 && value < (int)modifier.ValueList->Values.Count()) {
			IndexedProperties[modifier.Index] = value;
		} else {
			OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << ": Enum index (\"" << value << "\") out of range");
			return false;
		}
	} else {
		OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << " to integer value: Inappropriate type: " << modifier.ValueList->Name);
		return false;
	}

	return true;
}

bool Object::SetFlags(ModifierInfo const& modifier, uint64_t value)
{
	auto stats = GetStaticSymbols().GetStats();
	if (ValueList::IsFlagType(modifier.ValueList->Name)) {
		int poolIdx{ -1 };
		auto i64 = stats->GetOrCreateFlags(poolIdx);
		if (i64 != nullptr) {
			*i64 = value;
			IndexedProperties[modifier.Index] = poolIdx;
		}
	} else {
		OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << " to int64 value: Inappropriate type: " << modifier.ValueList->Name);
		return false;
	}

	return true;
}

bool Object::SetFlags(ModifierInfo const& modifier, ObjectSet<FixedString> const& value)
{
	if (!ValueList::IsFlagType(modifier.ValueList->Name)) {
		OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << " to flag array: Inappropriate type: " << modifier.ValueList->Name);
		return false;
	}

	auto stats = GetStaticSymbols().GetStats();
	int64_t flags{ 0 };
	for (auto const& flag : value) {
		auto flagValue = modifier.ValueList->Values.Find(flag);
		if (!flagValue) {
			OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << ": Value (\"" << flag << "\") is not a valid enum label");
			return false;
		}

		flags |= (1ll << (*flagValue - 1));
	}

	int poolIdx{ -1 };
	auto i64 = stats->GetOrCreateFlags(poolIdx);
	if (i64 != nullptr) {
		*i64 = flags;
		IndexedProperties[modifier.Index] = poolIdx;
	}

	return true;
}

bool Object::SetPropertyList(ModifierInfo const& modifier, std::optional<PropertyList*> const& value)
{
	if (modifier.ValueList->Name != GFS.strProperties) {
		OsiError("Couldn't set " << Name << "." << modifier.Modifier->Name << " to property list: Inappropriate type: " << modifier.ValueList->Name);
		return false;
	}

	if (value) {
		PropertyLists.Insert(modifier.Modifier->Name, *value);
	} else {
		// FIXME - clearing property lists not implemented!
	}
	return true;
}

void Object::ToProtobuf(MsgS2CSyncStat* msg) const
{
	msg->set_name(Name.Str);
	msg->set_level(Level);
	msg->set_modifier_list(ModifierListIndex);

	auto stats = GetStaticSymbols().GetStats();
	auto modifierList = stats->ModifierLists.Find(ModifierListIndex);

	for (size_t i = 0; i < IndexedProperties.size(); i++) {
		auto value = IndexedProperties[i];
		auto indexedProp = msg->add_indexed_properties();
		auto modifier = modifierList->Attributes.Find((uint32_t)i);
		auto enumeration = stats->ModifierValueLists.Find(modifier->ValueListIndex);
		switch (enumeration->GetPropertyType()) {
		case AttributeType::Int:
		case AttributeType::Enumeration:
			indexedProp->set_intval(value);
			break;

		case AttributeType::FixedString:
			indexedProp->set_stringval(stats->FixedStrings[value].Str);
			break;
		}
	}

	msg->set_ai_flags(AIFlags.Str);

	for (auto const& reqmt : Requirements) {
		reqmt.ToProtobuf(msg->add_requirements());
	}

	for (auto const& reqmt : MemorizationRequirements) {
		reqmt.ToProtobuf(msg->add_memorization_requirements());
	}

	for (auto const& category : ComboCategories) {
		msg->add_combo_categories(category.Str);
	}

	for (auto const& propList : PropertyLists) {
		propList.Value->ToProtobuf(propList.Key, msg->add_property_lists());
	}
}

void Object::FromProtobuf(MsgS2CSyncStat const& msg)
{
	auto stats = GetStaticSymbols().GetStats();
	Level = msg.level();

	if (msg.indexed_properties_size() != IndexedProperties.size()) {
		OsiError("IndexedProperties size mismatch for '" << Name << "'! Got "
			<< msg.indexed_properties_size() << ", expected " << IndexedProperties.size());
		return;
	}

	auto modifierList = stats->ModifierLists.Find(ModifierListIndex);
	for (size_t i = 0; i < IndexedProperties.size(); i++) {
		auto modifier = modifierList->Attributes.Find((uint32_t)i);
		auto enumeration = stats->ModifierValueLists.Find(modifier->ValueListIndex);
		auto const& prop = msg.indexed_properties().Get((uint32_t)i);
		switch (enumeration->GetPropertyType()) {
		case AttributeType::Int:
		case AttributeType::Enumeration:
			IndexedProperties[i] = prop.intval();
			break;

		case AttributeType::FixedString:
			IndexedProperties[i] = stats->GetOrCreateFixedString(prop.stringval().c_str());
			break;

		default:
			IndexedProperties[i] = 0;
			break;
		}
	}

	AIFlags = FixedString(msg.ai_flags().c_str());

	Requirements.Clear();
	for (auto const& reqmt : msg.requirements()) {
		Requirement requirement;
		requirement.FromProtobuf(reqmt);
		Requirements.Add(requirement);
	}

	MemorizationRequirements.Clear();
	for (auto const& reqmt : msg.memorization_requirements()) {
		Requirement requirement;
		requirement.FromProtobuf(reqmt);
		MemorizationRequirements.Add(requirement);
	}

	ComboCategories.Clear();
	for (auto const& category : msg.combo_categories()) {
		ComboCategories.Add(FixedString(category.c_str()));
	}

	PropertyLists.Clear();
	for (auto const& props : msg.property_lists()) {
		FixedString name(props.name().c_str());
		auto propertyList = stats->ConstructPropertyList(name);
		propertyList->FromProtobuf(props);
		PropertyLists.Insert(name, propertyList);
	}
}

void Object::BroadcastSyncMessage(bool syncDuringLoading) const
{
	auto msg = gExtender->GetServer().GetNetworkManager().GetFreeMessage(ReservedUserId);
	if (!msg) {
		OsiErrorS("Failed to get free message");
		return;
	}

	auto& wrap = msg->GetMessage();
	ToProtobuf(wrap.mutable_s2c_sync_stat());
	if (syncDuringLoading) {
		gExtender->GetServer().GetNetworkManager().BroadcastToConnectedPeers(msg, ReservedUserId, true);
	} else {
		gExtender->GetServer().GetNetworkManager().Broadcast(msg, ReservedUserId, true);
	}
}

using namespace dse::lua;

void Object::LuaGetAttributeShared(lua_State* L, FixedString const& attribute, std::optional<int> level, bool byRef)
{
	auto stats = GetStaticSymbols().GetStats();

	if (attribute == GFS.strLevel) {
		push(L, Level);
		return;
	} else if (attribute == GFS.strName) {
		push(L, Name);
		return;
	} else if (attribute == GFS.strModId) {
		push(L, gExtender->GetStatLoadOrderHelper().GetStatsEntryMod(Name));
		return;
	} else if (attribute == GFS.strUsing) {
		if (Using) {
			auto parent = stats->Objects.Find(Using);
			if (parent != nullptr) {
				push(L, parent->Name);
				return;
			}
		}

		push(L, nullptr);
		return;
	} else if (attribute == GFS.strAIFlags) {
		push(L, AIFlags);
		return;
	} else if (attribute == GFS.strComboCategory) {
		if (byRef) {
			MakeObjectRef(L, &ComboCategories);
		} else {
			LuaWrite(L, ComboCategories);
		}
		return;
	}

	auto modifier = GetModifierInfo(attribute);
	if (!modifier) {
		push(L, nullptr);
		return;
	}

	switch (modifier->ValueList->GetPropertyType()) {
	case AttributeType::Int:
	{
		std::optional<int> value;
		if (level) {
			if (*level == -1) {
				*level = Level;
			}

			value = GetIntScaled(*modifier, *level);
		} else {
			value = GetInt(*modifier);
		}

		LuaWrite(L, value);
		break;
	}

	case AttributeType::Conditions:
	{
		// FIXME - cache conditions during parsing?
		OsiError("Conditions property '" << modifier->Modifier->Name << "' is not readable");
		push(L, nullptr);
		break;
	}

	case AttributeType::FixedString:
	case AttributeType::Enumeration:
	{
		auto value = GetString(*modifier);
		if (value) {
			push(L, *value);
		} else {
			push(L, "");
		}
		break;
	}

	case AttributeType::Flags:
	{
		auto value = GetFlags(*modifier);
		LuaWrite(L, value);
		break;
	}

	case AttributeType::Requirements:
	{
		if (modifier->Modifier->Name == GFS.strRequirements) {
			if (byRef) {
				MakeObjectRef(L, &Requirements);
			} else {
				LuaWrite(L, Requirements);
			}
		} else if (modifier->Modifier->Name == GFS.strMemorizationRequirements) {
			if (byRef) {
				MakeObjectRef(L, &MemorizationRequirements);
			} else {
				LuaWrite(L, MemorizationRequirements);
			}
		} else {
			push(L, nullptr);
		}
		break;
	}

	case AttributeType::PropertyList:
	{
		auto propList = GetPropertyList(*modifier);
		if (byRef) {
			if (propList) {
				MakeObjectRef(L, *propList);
			} else {
				push(L, nullptr);
			}
		} else {
			LuaWrite(L, propList);
		}
		break;
	}

	default:
		OsiError("Don't know how to fetch values of type '" << modifier->ValueList->Name << "'");
		push(L, nullptr);
		break;
	}
}

int Object::LuaGetAttributeLegacy(lua_State* L, FixedString const& attribute, std::optional<int> level)
{
	StackCheck _(L, 1);
	auto stats = GetStaticSymbols().GetStats();
	LuaGetAttributeShared(L, attribute, level, false);
	return 1;
}

int Object::LuaGetAttribute(lua_State* L, FixedString const& attribute, std::optional<int> level)
{
	StackCheck _(L, 1);
	auto stats = GetStaticSymbols().GetStats();
	LuaGetAttributeShared(L, attribute, level, true);
	return 1;
}

bool Object::LuaSetAttribute(lua_State * L, FixedString const& attribute, int valueIdx)
{
	StackCheck _(L);

	if (attribute == GFS.strLevel) {
		Level = get<int32_t>(L, valueIdx);
		return true;
	} else if (attribute == GFS.strRequirements) {
		LuaRead(L, Requirements);
		return true;
	} else if (attribute == GFS.strMemorizationRequirements) {
		LuaRead(L, MemorizationRequirements);
		return true;
	} else if (attribute == GFS.strAIFlags) {
		AIFlags = get<FixedString>(L, valueIdx);
		return true;
	} else if (attribute == GFS.strComboCategory) {
		ComboCategories.Clear();
		if (lua_type(L, valueIdx) != LUA_TTABLE) {
			OsiError("Must pass a table when setting ComboCategory");
			return false;
		}

		for (auto category : iterate(L, valueIdx)) {
			auto categoryName = get<FixedString>(L, category);
			ComboCategories.Add(categoryName);
		}

		return true;
	} else if (attribute == GFS.strSkillProperties || attribute == GFS.strExtraProperties) {
		STDString name = Name.Str;
		name += "_";
		name += attribute.GetString();
		FixedString statsPropertyKey(name.c_str());

		auto newList = LuaToObjectPropertyList(L, statsPropertyKey);
		if (newList) {
			auto propertyList = PropertyLists.Find(attribute);
			if (propertyList) {
				// FIXME - add Remove() support!
				PropertyLists.Clear();
				// FIXME - need to remove from stats.PropertyLists too!
				// GameFree(*propertyList);
			}

			PropertyLists.Insert(attribute, newList);
		}

		return true;
	}

	auto modifier = GetModifierInfo(attribute);
	if (!modifier) {
		push(L, nullptr);
		return false;
	}

	auto stats = GetStaticSymbols().GetStats();
	auto valueType = lua_type(L, valueIdx);
	switch (modifier->ValueList->GetPropertyType()) {
	case AttributeType::Int:
	{
		auto val = get<int32_t>(L, valueIdx);
		return SetInt(*modifier, val);
	}

	case AttributeType::Conditions:
	{
		auto value = get<STDString>(L, valueIdx);
		auto conditions = stats->CreateConditions(Name, modifier->Modifier->Name, value);
		Conditions.Insert(modifier->Modifier->Name, conditions);
		return false;
	}

	case AttributeType::FixedString:
	{
		auto value = get<char const*>(L, valueIdx);
		return SetString(*modifier, value);
	}

	case AttributeType::Enumeration:
	{
		if (valueType == LUA_TSTRING) {
			auto value = get<char const*>(L, valueIdx);
			return SetString(*modifier, value);
		} else {
			auto value = get<int>(L, valueIdx);
			return SetInt(*modifier, value);
		}
	}

	case AttributeType::Flags:
	{
		ObjectSet<FixedString> value;
		LuaRead(L, value);
		return SetFlags(*modifier, value);
	}

	case AttributeType::Requirements:
	{
		if (modifier->Modifier->Name == GFS.strRequirements) {
			LuaRead(L, Requirements);
			return true;
		} else if (modifier->Modifier->Name == GFS.strMemorizationRequirements) {
			LuaRead(L, MemorizationRequirements);
			return true;
		} else {
			return false;
		}
	}

	case AttributeType::PropertyList:
	{
		STDString name = Name.Str;
		name += "_";
		name += attribute.GetString();
		FixedString statsPropertyKey(name.c_str());

		auto newList = LuaToObjectPropertyList(L, statsPropertyKey);
		if (newList) {
			SetPropertyList(*modifier, newList);
		}
		return true;
	}

	default:
		OsiError("Don't know how to fetch values of type '" << modifier->ValueList->Name << "'");
		push(L, nullptr);
		return false;
	}
}

bool Object::LuaFallbackGet(lua_State* L, lua::LifetimeHolder const& lifetime, Object* object, FixedString const& prop)
{
	return object->LuaGetAttribute(L, prop, {});
}

bool Object::LuaFallbackSet(lua_State* L, lua::LifetimeHolder const& lifetime, Object* object, FixedString const& prop, int index)
{
	return object->LuaSetAttribute(L, prop, index);
}

bool StatusPrototype::LuaFallbackGet(lua_State* L, lua::LifetimeHolder const& lifetime, StatusPrototype* object, FixedString const& prop)
{
	return Object::LuaFallbackGet(L, lifetime, object->GetStats(), prop);
}

bool StatusPrototype::LuaFallbackSet(lua_State* L, lua::LifetimeHolder const& lifetime, StatusPrototype* object, FixedString const& prop, int index)
{
	return Object::LuaFallbackSet(L, lifetime, object->GetStats(), prop, index);
}

bool SkillPrototype::LuaFallbackGet(lua_State* L, lua::LifetimeHolder const& lifetime, SkillPrototype* object, FixedString const& prop)
{
	return Object::LuaFallbackGet(L, lifetime, object->GetStats(), prop);
}

bool SkillPrototype::LuaFallbackSet(lua_State* L, lua::LifetimeHolder const& lifetime, SkillPrototype* object, FixedString const& prop, int index)
{
	return Object::LuaFallbackSet(L, lifetime, object->GetStats(), prop, index);
}

END_NS()
