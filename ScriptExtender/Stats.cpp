#include <stdafx.h>

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/Enumerations.h>
#include <ScriptHelpers.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

#include <GameDefinitions/StatsObject.inl>

BEGIN_SE()

void IScriptCheckObject::Dump(STDString& buf, ScriptOperatorType parentOperator) const
{
	switch (GetType()) {
	case ScriptCheckType::Operator: {
		auto self = static_cast<ScriptCheckOperator const*>(this);
		bool needsBrackets = parentOperator == ScriptOperatorType::Not
			|| (parentOperator != ScriptOperatorType::None
				&& parentOperator != self->Type);

		switch (self->Type) {
		case ScriptOperatorType::And: {
			if (needsBrackets) buf += "(";
			if (self->Left) self->Left->Dump(buf, self->Type);
			buf += "&";
			if (self->Right) self->Right->Dump(buf, self->Type);
			if (needsBrackets) buf += ")";
			break;
		}

		case ScriptOperatorType::Or: {
			if (needsBrackets) buf += "(";
			if (self->Left) self->Left->Dump(buf, self->Type);
			buf += "|";
			if (self->Right) self->Right->Dump(buf, self->Type);
			if (needsBrackets) buf += ")";
			break;
		}

		case ScriptOperatorType::Not: {
			buf += "!";
			if (self->Left) self->Left->Dump(buf, self->Type);
			break;
		}

		default:
			OsiError("Cannot handle script operator type: " << (unsigned)self->Type);
			break;
		}
		break;
	}

	case ScriptCheckType::Variable: {
		auto self = static_cast<ScriptCheckVariable const*>(this);
		if (self->ConditionId == -1) {
			buf += "INVALID_CONDITION_ID";
		} else {
			buf += GetStaticSymbols().GetStats()->ConditionsManager.Variables[self->ConditionId];
			if (self->Value && self->Value != GFS.strEmpty) {
				buf += ":";
				buf += self->Value.GetString();
			}
		}
		break;
	}

	default:
		OsiError("Cannot handle script check type: " << (unsigned)GetType());
		break;
	}
}

STDString IScriptCheckObject::Dump() const
{
	STDString buf;
	Dump(buf, ScriptOperatorType::None);
	return buf;
}

END_SE()

BEGIN_NS(stats)

STDString Condition::Dump() const
{
	if (!ScriptCheckBlock) return "";

	return ScriptCheckBlock->Dump();
}

void HitDamageInfo::ClearDamage()
{
	TotalDamageDone = 0;
	ArmorAbsorption = 0;
	DamageList.clear();
}

void HitDamageInfo::ClearDamage(stats::DamageType damageType)
{
	for (auto const& dmg : DamageList) {
		if (dmg.DamageType == damageType) {
			TotalDamageDone -= dmg.Amount;
		}
	}

	DamageList.ClearDamage(damageType);
}

void HitDamageInfo::AddDamage(stats::DamageType damageType, int32_t amount)
{
	TotalDamageDone += amount;
	DamageList.AddDamage(damageType, amount);
}

void HitDamageInfo::CopyFrom(HitDamageInfo const& src)
{
	Equipment = src.Equipment;
	TotalDamageDone = src.TotalDamageDone;
	DamageDealt = src.DamageDealt;
	DeathType = src.DeathType;
	DamageType = src.DamageType;
	AttackDirection = src.AttackDirection;
	ArmorAbsorption = src.ArmorAbsorption;
	LifeSteal = src.LifeSteal;
	EffectFlags = src.EffectFlags;
	HitWithWeapon = src.HitWithWeapon;
	DamageList = src.DamageList;
}


void DamagePairList::ClearDamage(stats::DamageType damageType)
{
	for (uint32_t i = 0; i < size(); i++) {
		if ((*this)[i].DamageType == damageType) {
			remove_at(i);
			i--;
		}
	}
}

void DamagePairList::AddDamage(DamageType damageType, int32_t amount)
{
	if (amount == 0) return;

	bool added{ false };
	for (uint32_t i = 0; i < size(); i++) {
		if ((*this)[i].DamageType == damageType) {
			auto newAmount = (*this)[i].Amount + amount;
			if (newAmount == 0) {
				remove_at(i);
			} else {
				(*this)[i].Amount = newAmount;
			}

			added = true;
			break;
		}
	}

	if (!added && amount != 0) {
		TDamagePair dmg;
		dmg.DamageType = damageType;
		dmg.Amount = amount;
		push_back(dmg);
	}
}

int DamagePairList::GetByType(DamageType damageType)
{
	int32_t amount = 0;
	for (auto const& dmg : *this) {
		if (dmg.DamageType == damageType) {
			amount += dmg.Amount;
		}
	}

	return amount;
}


void DamagePairList::ClearAll(std::optional<DamageType> damageType)
{
	if (damageType) {
		ClearDamage(*damageType);
	} else {
		clear();
	}
}

void DamagePairList::Multiply(float multiplier)
{
	for (auto& dmg : *this) {
		dmg.Amount = (int32_t)round(dmg.Amount * multiplier);
	}
}

void DamagePairList::LuaMerge(lua_State* L)
{
	auto other = lua::CheckedGetObject<DamagePairList>(L, 2);

	for (auto const& dmg : *other) {
		AddDamage(dmg.DamageType, dmg.Amount);
	}
}

void DamagePairList::ConvertDamageType(DamageType newType)
{
	int32_t totalDamage = 0;
	for (auto const& dmg : *this) {
		totalDamage += dmg.Amount;
	}

	clear();
	AddDamage(newType, totalDamage);
}

void DamagePairList::AggregateSameTypeDamages()
{
	for (uint32_t i = size(); i > 0; i--) {
		auto & src = (*this)[i - 1];
		for (uint32_t j = i - 1; j > 0; j--) {
			auto & dest = (*this)[j - 1];
			if (src.DamageType == dest.DamageType) {
				dest.Amount += src.Amount;
				remove_at(i - 1);
				break;
			}
		}
	}
}

UserReturn DamagePairList::LuaToTable(lua_State* L)
{
	lua_newtable(L); // Stack: tab

	for (uint32_t i = 0; i < size(); i++) {
		auto const & item = (*this)[i];

		lua::push(L, i + 1); // Stack: tab, index
		lua_newtable(L); // Stack: tab, index, dmgTab
		lua::setfield(L, "DamageType", item.DamageType);
		lua::setfield(L, "Amount", item.Amount);

		lua_settable(L, -3); // Stack: tab
	}

	return 1;
}

void DamagePairList::LuaCopyFrom(lua_State* L)
{
	auto other = CheckedGetObject<DamagePairList>(L, 2);
	*this = *other;
}

void SkillPrototypeManager::SyncSkillStat(Object* object, SkillPrototype* proto)
{
	auto stats = GetStaticSymbols().GetStats();
	proto->Ability = *object->GetInt(GFS.strAbility);
	proto->Tier = *object->GetInt(GFS.strTier);
	proto->Requirement = *object->GetInt(GFS.strRequirement);
	proto->Level = *object->GetInt(GFS.strLevel);
	proto->Icon = FixedString(*object->GetString(GFS.strIcon));
	proto->MagicCost = *object->GetInt(GFS.strMagicCost);
	proto->MemoryCost = *object->GetInt(GFS.strMemoryCost);
	proto->ActionPoints = *object->GetInt(GFS.strActionPoints);
	proto->Cooldown = *object->GetInt(GFS.strCooldown) * 6.0f;
	proto->CooldownReduction = *object->GetInt(GFS.strCooldownReduction) / 100.0f;
	proto->ChargeDuration = *object->GetInt(GFS.strChargeDuration) * 6.0f;

	FixedString displayNameKey(*object->GetString(GFS.strDisplayName));
	TranslatedString displayName;
	if (script::GetTranslatedStringFromKey(displayNameKey, displayName)) {
		proto->DisplayName = displayName.Handle.ReferenceString;
	}

	STDString aiFlags = object->AIFlags.GetStringOrDefault();
	proto->AiFlags = (AIFlags)0;
	if (aiFlags.find("CanNotUse") != STDString::npos) proto->AiFlags |= AIFlags::CanNotUse;
	if (aiFlags.find("IgnoreSelf") != STDString::npos) proto->AiFlags |= AIFlags::IgnoreSelf;
	if (aiFlags.find("IgnoreDebuff") != STDString::npos) proto->AiFlags |= AIFlags::IgnoreDebuff;
	if (aiFlags.find("IgnoreBuff") != STDString::npos) proto->AiFlags |= AIFlags::IgnoreBuff;
	if (aiFlags.find("StatusIsSecondary") != STDString::npos) proto->AiFlags |= AIFlags::StatusIsSecondary;
	if (aiFlags.find("IgnoreControl") != STDString::npos) proto->AiFlags |= AIFlags::IgnoreControl;
	if (aiFlags.find("CanNotTargetFrozen") != STDString::npos) proto->AiFlags |= AIFlags::CanNotTargetFrozen;
}

void SkillPrototypeManager::SyncSkillStat(Object* object)
{
	auto stats = GetStaticSymbols().GetStats();
	auto skillTypeName = object->GetString(GFS.strSkillType);
	if (!skillTypeName) {
		OsiError("Skill stats object has no SkillType?");
		return;
	}

	// Cone is an alias of Zone, but is not part of the SkillType enum
	std::optional<SkillType> skillType;
	if (strcmp(*skillTypeName, "Cone") == 0) {
		skillType = SkillType::Zone;
	} else {
		skillType = EnumInfo<SkillType>::Find(FixedString(*skillTypeName));
	}

	if (!skillType) {
		OsiError("Unsupported SkillType: " << *skillTypeName);
		return;
	}

	auto pProto = Prototypes.try_get(object->Name);
	if (pProto == nullptr) {
		auto proto = GameAlloc<SkillPrototype>();
		proto->StatsObjectIndex = object->Handle;
		proto->SkillTypeId = *skillType;
		proto->SkillId = object->Name;
		proto->RootSkillPrototype = nullptr;
		SyncSkillStat(object, proto);

		Prototypes.insert(proto->SkillId, proto);
		PrototypeNames.push_back(proto->SkillId);

		auto lv1Proto = GameAlloc<SkillPrototype>();
		*lv1Proto = *proto;

		STDString lv1Name = proto->SkillId.GetStringOrDefault();
		lv1Name += "_-1";
		lv1Proto->SkillId = FixedString(lv1Name.c_str());
		lv1Proto->Level = -1;

		proto->ChildPrototypes.push_back(lv1Proto);
		lv1Proto->RootSkillPrototype = proto;

		Prototypes.insert(lv1Proto->SkillId, lv1Proto);
		PrototypeNames.push_back(lv1Proto->SkillId);
	} else {
		SyncSkillStat(object, pProto);

		STDString lv1Name = pProto->SkillId.GetStringOrDefault();
		lv1Name += "_-1";
		auto lv1Proto = Prototypes.try_get(FixedString(lv1Name.c_str()));
		if (lv1Proto) {
			SyncSkillStat(object, lv1Proto);
		}
	}
}

void StatusPrototypeManager::SyncStatusStat(Object* object)
{
	auto stats = GetStaticSymbols().GetStats();
	auto statusTypeName = object->GetString(GFS.strStatusType);
	if (!statusTypeName) {
		OsiError("Status stats object has no StatusType?");
		return;
	}

	auto statusType = EnumInfo<StatusType>::Find(FixedString(*statusTypeName));
	if (!statusType) {
		OsiError("Unsupported StatusType: " << *statusTypeName);
		return;
	}

	StatusPrototype* proto;
	auto pProto = Prototypes.try_get(object->Name);
	if (pProto == nullptr) {
		auto hitProto = Prototypes.try_get(GFS.strHIT);
		if (!hitProto) {
			OsiError("Couldn't sync new status entry - missing HIT status!");
			return;
		}

		proto = GameAlloc<StatusPrototype>();
		proto->VMT = hitProto->VMT;
		proto->StatsObjectIndex = object->Handle;
		proto->StatusId = *statusType;
		proto->StatusName = object->Name;
		proto->HasStats = false;

		Prototypes.insert(proto->StatusName, proto);
		PrototypeNames.push_back(proto->StatusName);
	} else {
		proto = pProto;
	}

	if (proto->HasStats) {
		OsiError("Cannot sync stats of builtin status " << proto->StatusName);
		return;
	}

	FixedString displayNameKey(*object->GetString(GFS.strDisplayName));
	TranslatedString displayName;
	if (script::GetTranslatedStringFromKey(displayNameKey, displayName)) {
		proto->DisplayName = displayName;
	}

	proto->Icon = FixedString(*object->GetString(GFS.strIcon));
	// FIXME - AbsorbSurfaceType
}

void Requirement::ToProtobuf(StatRequirement* msg) const
{
	msg->set_requirement((int32_t)RequirementId);
	msg->set_int_param(Param);
	if (Tag) {
		msg->set_string_param(Tag.GetStringOrDefault());
	}
	msg->set_negate(Not);
}

void Requirement::FromProtobuf(StatRequirement const& msg)
{
	RequirementId = (RequirementType)msg.requirement();
	Param = msg.int_param();
	if (!msg.string_param().empty()) {
		Tag = FixedString(msg.string_param().c_str());
	}

	Not = msg.negate();
}


void PropertyData::ToProtobuf(StatProperty* msg) const
{
	msg->set_name(Name.GetStringOrDefault());
	msg->set_type((uint32_t)TypeId);
	msg->set_property_context((uint32_t)Context);
	if (Conditions) {
		STDString name(Name.GetStringOrDefault());
		if (name[name.length() - 1] == ')') {
			auto ifPos = name.find("_IF(");
			if (ifPos != std::string::npos) {
				auto condition = name.substr(ifPos + 4, name.length() - ifPos - 5);
				msg->set_conditions(condition.c_str());
			}
		}
	}

	switch (TypeId) {
	case PropertyType::Custom:
		break;

	case PropertyType::Status:
	{
		auto const& p = (PropertyStatus const&)*this;
		msg->add_string_params(p.Status.GetStringOrDefault());
		msg->add_float_params(p.StatusChance);
		msg->add_float_params(p.Duration);
		msg->add_string_params(p.StatsId.GetStringOrDefault());
		msg->add_int_params(p.Arg4);
		msg->add_int_params(p.Arg5);
		msg->add_bool_params(p.SurfaceBoost);
		for (auto boost : p.SurfaceBoosts) {
			msg->add_surface_boosts((int32_t)boost);
		}
		break;
	}

	case PropertyType::SurfaceChange:
	{
		auto const& p = (PropertySurfaceChange const&)*this;
		msg->add_int_params(p.SurfaceChange);
		msg->add_float_params(p.SurfaceChance);
		msg->add_float_params(p.Lifetime);
		msg->add_float_params(p.StatusChance);
		msg->add_float_params(p.Radius);
		break;
	}

	case PropertyType::GameAction:
	{
		auto const& p = (PropertyGameAction const&)*this;
		msg->add_int_params(p.GameAction);
		msg->add_float_params(p.Arg1);
		msg->add_float_params(p.Arg2);
		msg->add_string_params(p.Arg3.GetStringOrDefault());
		msg->add_float_params(p.Arg4);
		msg->add_float_params(p.Arg5);
		msg->add_int_params(p.StatusHealType);
		break;
	}

	case PropertyType::OsirisTask:
	{
		auto const& p = (PropertyOsirisTask const&)*this;
		msg->add_int_params(p.OsirisTask);
		msg->add_float_params(p.Chance);
		msg->add_int_params(p.VitalityOnRevive);
		break;
	}

	case PropertyType::Sabotage:
	{
		auto const& p = (PropertySabotage const&)*this;
		msg->add_int_params(p.Amount);
		break;
	}

	case PropertyType::Summon:
	{
		auto const& p = (PropertySummon const&)*this;
		msg->add_string_params(p.Template.GetStringOrDefault());
		msg->add_float_params(p.Duration);
		msg->add_bool_params(p.IsTotem);
		msg->add_string_params(p.Skill.GetStringOrDefault());
		break;
	}

	case PropertyType::Force:
	{
		auto const& p = (PropertyForce const&)*this;
		msg->add_int_params(p.Distance);
		break;
	}

	case PropertyType::CustomDescription:
	{
		auto const& p = (PropertyCustomDescription const&)*this;
		msg->add_string_params(ToUTF8(p.TextLine1).c_str());
		break;
	}

	case PropertyType::Extender:
	{
		auto const& p = (PropertyExtender const&)*this;
		msg->add_string_params(p.PropertyName.GetStringOrDefault());
		msg->add_float_params(p.Arg1);
		msg->add_float_params(p.Arg2);
		msg->add_string_params(p.Arg3.GetStringOrDefault());
		msg->add_int_params(p.Arg4);
		msg->add_int_params(p.Arg5);
		break;
	}

	default:
		WARN("Couldn't convert unknown property type %d to protobuf!", TypeId);
	}
}

void PropertyData::FromProtobuf(StatProperty const& msg)
{
	auto stats = GetStaticSymbols().GetStats();

	Name = FixedString(msg.name().c_str());
	Context = (PropertyContext)msg.property_context();

	if (!msg.conditions().empty()) {
		STDString conditions(msg.conditions().c_str());
		auto scriptCheckBlock = stats->BuildScriptCheckBlockFromProperties(conditions);
		if (scriptCheckBlock) {
			auto statConditions = GameAlloc<Condition>();
			statConditions->ScriptCheckBlock = scriptCheckBlock;
			statConditions->Name = GFS.strEmpty; // TODO?
			Conditions = statConditions;
		} else {
			OsiError("Failed to parse conditions from protobuf: " << conditions);
		}
	}

	switch (TypeId) {
	case PropertyType::Custom:
		break;

	case PropertyType::Status:
	{
		auto& p = (PropertyStatus&)*this;
		p.Status = FixedString(msg.string_params()[0].c_str());
		p.StatusChance = msg.float_params()[0];
		p.Duration = msg.float_params()[1];
		p.StatsId = FixedString(msg.string_params()[1].c_str());
		p.Arg4 = msg.int_params()[0];
		p.Arg5 = msg.int_params()[1];
		p.SurfaceBoost = msg.bool_params()[0];
		for (auto boost : msg.surface_boosts()) {
			p.SurfaceBoosts.push_back((SurfaceType)boost);
		}
		break;
	}

	case PropertyType::SurfaceChange:
	{
		auto& p = (PropertySurfaceChange&)*this;
		p.SurfaceChange = msg.int_params()[0];
		p.SurfaceChance = msg.float_params()[0];
		p.Lifetime = msg.float_params()[1];
		p.StatusChance = msg.float_params()[2];
		p.Radius = msg.float_params()[3];
		break;
	}

	case PropertyType::GameAction:
	{
		auto& p = (PropertyGameAction&)*this;
		p.GameAction = msg.int_params()[0];
		p.Arg1 = msg.float_params()[0];
		p.Arg2 = msg.float_params()[1];
		p.Arg3 = FixedString(msg.string_params()[0].c_str());
		p.Arg4 = msg.float_params()[2];
		p.Arg5 = msg.float_params()[3];
		p.StatusHealType = msg.int_params()[1];
		break;
	}

	case PropertyType::OsirisTask:
	{
		auto& p = (PropertyOsirisTask&)*this;
		p.OsirisTask = msg.int_params()[0];
		p.Chance = msg.float_params()[0];
		p.VitalityOnRevive = msg.int_params()[1];
		break;
	}

	case PropertyType::Sabotage:
	{
		auto& p = (PropertySabotage&)*this;
		p.Amount = msg.int_params()[0];
		break;
	}

	case PropertyType::Summon:
	{
		auto& p = (PropertySummon&)*this;
		p.Template = FixedString(msg.string_params()[0].c_str());
		p.Duration = msg.float_params()[0];
		p.IsTotem = msg.bool_params()[0];
		p.Skill = FixedString(msg.string_params()[1].c_str());
		break;
	}

	case PropertyType::Force:
	{
		auto& p = (PropertyForce&)*this;
		p.Distance = msg.int_params()[0];
		break;
	}

	case PropertyType::CustomDescription:
	{
		auto& p = (PropertyCustomDescription&)*this;
		p.TextLine1 = FromUTF8(msg.string_params()[0]);
		break;
	}

	case PropertyType::Extender:
	{
		auto& p = (PropertyExtender&)*this;
		p.PropertyName = FixedString(msg.string_params()[0].c_str());
		p.Arg1 = msg.float_params()[0];
		p.Arg2 = msg.float_params()[1];
		p.Arg3 = FixedString(msg.string_params()[1].c_str());
		p.Arg4 = msg.int_params()[0];
		p.Arg5 = msg.int_params()[1];
		break;
	}

	default:
		WARN("Couldn't parse unknown property type %d from protobuf!", TypeId);
	}
}

void PropertyList::ToProtobuf(FixedString const& name, StatPropertyList* msg) const
{
	msg->set_name(name.GetStringOrDefault());
	for (auto const& prop : Properties.Elements) {
		auto const& dataProp = (PropertyData const*)prop;
		dataProp->ToProtobuf(msg->add_properties());
	}
}

void PropertyList::FromProtobuf(StatPropertyList const& msg)
{
	auto stats = GetStaticSymbols().GetStats();
	for (auto const& prop : msg.properties()) {
		auto property = stats->ConstructProperty((PropertyType)prop.type());
		if (property) {
			property->FromProtobuf(prop);
			Properties.Add(property->Name, property);
			AllPropertyContexts |= property->Context;
		}
	}
}

int64_t LevelMap::LuaGetScaledValue(int value, int level)
{
	return GetScaledValue(value, level);
}

bool ValueList::IsFlagType(FixedString const& typeName)
{
	return typeName == GFS.strAttributeFlags;
}

AttributeType ValueList::GetPropertyType() const
{
	if (Name == GFS.strConstantInt) {
		return AttributeType::Int;
	}
		
	if (Name == GFS.strFixedString) {
		return AttributeType::FixedString;
	}

	if (Name == GFS.strProperties) {
		return AttributeType::PropertyList;
	}

	if (Name == GFS.strConditions) {
		return AttributeType::Conditions;
	}

	if (Name == GFS.strRequirements || Name == GFS.strMemorizationRequirements) {
		return AttributeType::Requirements;
	}

	if (Name == GFS.strAttributeFlags) {
		return AttributeType::Flags;
	}

	if (Values.size() > 0) {
		return AttributeType::Enumeration;
	}
		
	return AttributeType::Unknown;
}

Modifier* gExtraPropertiesModifier{ nullptr };

std::optional<ModifierInfo> ModifierList::GetModifierInfo(FixedString const& attributeName) const
{
	auto stats = GetStaticSymbols().GetStats();
	if (attributeName == GFS.strExtraProperties 
		&& (IsItemType() || Name == GFS.strPotion)) {
		if (!gExtraPropertiesModifier) {
			gExtraPropertiesModifier = GameAlloc<Modifier>();
			gExtraPropertiesModifier->ValueListIndex = *stats->ModifierValueLists.FindIndex(GFS.strProperties);
			gExtraPropertiesModifier->LevelMapIndex = -1;
			gExtraPropertiesModifier->UnknownZero = 0;
			gExtraPropertiesModifier->Name = GFS.strExtraProperties;
		}

		auto valueList = stats->ModifierValueLists.Find(gExtraPropertiesModifier->ValueListIndex);
		return ModifierInfo{ gExtraPropertiesModifier, valueList, -1 };
	}

	auto index = Attributes.FindIndex(attributeName);
	if (!index) {
		return {};
	} else {
		auto modifier = Attributes.Find(*index);
		auto valueList = stats->ModifierValueLists.Find(modifier->ValueListIndex);
		return ModifierInfo{ modifier, valueList, *index };
	}
}

bool ModifierList::IsItemType() const
{
	return Name == GFS.strWeapon
		|| Name == GFS.strArmor
		|| Name == GFS.strShield;
}

CRPGStatsVMTMappings gCRPGStatsVMTMappings;

CRPGStatsVMTMappings::CRPGStatsVMTMappings()
{
	PropertyTypes = {
		{PropertyType::Custom, nullptr},
		{PropertyType::Status, nullptr},
		{PropertyType::SurfaceChange, nullptr},
		{PropertyType::GameAction, nullptr},
		{PropertyType::OsirisTask, nullptr},
		{PropertyType::Sabotage, nullptr},
		{PropertyType::Summon, nullptr},
		{PropertyType::Force, nullptr}
	};
}
	
void CRPGStatsVMTMappings::MapVMTs()
{
	if (VMTsMapped) return;
	auto stats = GetStaticSymbols().GetStats();

	if (stats->Objects.Elements.size() > 0) {
		ObjectVMT = stats->Objects.Elements[0]->VMT;
	}

	for (auto const& propList : stats->PropertyLists) {
		SkillPropertiesVMT = *(void**)propList.Value;
		for (auto prop : propList.Value->Properties.Elements) {
			PropertyTypes[prop->TypeId] = *(void**)prop;
		}
	}

	VMTsMapped = true;
}

bool RPGStats::ObjectExists(FixedString const& statsId, FixedString const& type)
{
	auto object = Objects.Find(statsId);
	if (object == nullptr) {
		return false;
	}

	auto typeInfo = GetTypeInfo(object);
	if (typeInfo == nullptr) {
		return false;
	}

	return typeInfo->Name == type;
}


std::optional<Object*> RPGStats::CreateObject(FixedString const& name, FixedString const& type)
{
	auto modifierIdx = ModifierLists.FindIndex(type);
	if (!modifierIdx) {
		OsiError("Unknown modifier list type: " << type);
		return {};
	}

	return CreateObject(name, *modifierIdx);
}

std::optional<Object*> RPGStats::CreateObject(FixedString const& name, int32_t modifierListIndex)
{
	auto modifier = ModifierLists.Find(modifierListIndex);
	if (!modifier) {
		OsiError("Modifier list doesn't exist: " << name);
		return {};
	}

	auto object = Objects.Find(name);
	if (object) {
		OsiError("A stats object already exists with this name: " << name);
		return {};
	}

	gCRPGStatsVMTMappings.MapVMTs();
	if (!gCRPGStatsVMTMappings.ObjectVMT) {
		OsiError("Cannot construct stats object - VMT not mapped!");
		return {};
	}

	object = GameAlloc<Object>();
	object->VMT = gCRPGStatsVMTMappings.ObjectVMT;
	object->ModifierListIndex = modifierListIndex;
	object->IndexedProperties.resize(modifier->Attributes.Elements.size(), 0);
	object->DivStats = DivinityStats;
	object->Name = name;
	object->PropertyLists.ResizeHashtable(3);
	object->Conditions.ResizeHashtable(3);

	object->Handle = Objects.Elements.size();
	Objects.Add(name, object);

	return object;
}

void RPGStats::SyncObjectFromServer(MsgS2CSyncStat const& msg)
{
	auto object = Objects.Find(FixedString(msg.name().c_str()));
	if (object) {
		object->FromProtobuf(msg);
		SyncWithPrototypeManager(object);
	} else {
		auto newObject = CreateObject(FixedString(msg.name().c_str()), msg.modifier_list());
		if (!newObject) {
			OsiError("Could not construct stats object from server: " << msg.name());
			return;
		}

		(*newObject)->FromProtobuf(msg);
		SyncWithPrototypeManager(*newObject);
	}
}

void RPGStats::SyncWithPrototypeManager(Object* object)
{
	auto modifier = ModifierLists.Find(object->ModifierListIndex);
	if (modifier->Name == GFS.strSkillData) {
		auto skillProtoMgr = GetStaticSymbols().eoc__SkillPrototypeManager;
		if (skillProtoMgr && *skillProtoMgr) {
			(*skillProtoMgr)->SyncSkillStat(object);
		}
	} else if (modifier->Name == GFS.strStatusData) {
		auto statusProtoMgr = GetStaticSymbols().eoc__StatusPrototypeManager;
		if (statusProtoMgr && *statusProtoMgr) {
			(*statusProtoMgr)->SyncStatusStat(object);
		}
	}
}

void RPGStats::BroadcastSyncAll()
{
	for (auto const& statsId : gExtender->GetServer().GetExtensionState().GetDynamicStats()) {
		auto object = Objects.Find(statsId);
		if (!object) {
			OsiError("Stat entry '" << statsId << "' is marked as dynamic but cannot be found! It will not be synced to the client!");
		} else {
			object->BroadcastSyncMessage(true);
		}
	}
}

std::optional<FixedString*> RPGStats::GetFixedString(int stringId)
{
	if (stringId > 0) {
		return &FixedStrings[stringId];
	} else {
		return {};
	}
}

FixedString* RPGStats::GetOrCreateFixedString(int& stringId)
{
	if (stringId <= 0) {
		stringId = (int)FixedStrings.size();
		FixedStrings.push_back(FixedString{});
	}

	return &FixedStrings[stringId];
}

Condition* RPGStats::CreateConditions(FixedString const& statName, FixedString const& modifierName, STDString const& conditions)
{
	auto scriptCheckBlock = BuildScriptCheckBlockFromProperties(conditions);
	if (scriptCheckBlock) {
		auto statConditions = GameAlloc<Condition>();
		statConditions->ScriptCheckBlock = scriptCheckBlock;
		STDString name = statName.GetString();
		name += "_";
		name += modifierName.GetString();
		statConditions->Name = FixedString(name.c_str());
		return statConditions;
	} else {
		OsiWarn("Failed to parse conditions: " << conditions);
		return nullptr;
	}
}

std::optional<uint64_t*> RPGStats::GetFlags(int flagsId)
{
	if (flagsId > 0) {
		return Flags[flagsId];
	} else {
		return {};
	}
}

uint64_t* RPGStats::GetOrCreateFlags(int& flagsId)
{
	if (flagsId <= 0) {
		flagsId = (int)Flags.size();
		auto flags = GameAlloc<uint64_t>();
		*flags = 0;
		Flags.push_back(flags);
	}

	return Flags[flagsId];
}

PropertyList* RPGStats::ConstructPropertyList(FixedString const& propertyName)
{
	gCRPGStatsVMTMappings.MapVMTs();
	if (!gCRPGStatsVMTMappings.SkillPropertiesVMT) {
		OsiError("Cannot construct object property list - VMT not mapped!");
		return nullptr;
	}

	auto properties = GameAlloc<PropertyList>();
	properties->Properties.VMT = gCRPGStatsVMTMappings.SkillPropertiesVMT;
	properties->Properties.NameToIndex.ResizeHashtable(31);
	properties->Name = propertyName;
	return properties;
}

PropertyData* RPGStats::ConstructProperty(PropertyType type)
{
	auto stats = GetStaticSymbols().GetStats();

	PropertyData* prop{ nullptr };
	switch (type) {
	case PropertyType::Custom:
		prop = GameAlloc<PropertyCustom>();
		break;

	case PropertyType::Status:
		prop = GameAlloc<PropertyStatus>();
		break;

	case PropertyType::SurfaceChange:
		prop = GameAlloc<PropertySurfaceChange>();
		break;

	case PropertyType::GameAction:
		prop = GameAlloc<PropertyGameAction>();
		break;

	case PropertyType::OsirisTask:
		prop = GameAlloc<PropertyOsirisTask>();
		break;

	case PropertyType::Sabotage:
		prop = GameAlloc<PropertySabotage>();
		break;

	case PropertyType::Summon:
		prop = GameAlloc<PropertySummon>();
		break;

	case PropertyType::Force:
		prop = GameAlloc<PropertyForce>();
		break;

	case PropertyType::CustomDescription:
		prop = GameAlloc<PropertyCustomDescription>();
		break;

	case PropertyType::Extender:
		prop = GameAlloc<PropertyExtender>();
		break;

	default:
		OsiError("Unhandled object property type!");
		return nullptr;
	}

	if (type < PropertyType::CustomDescription) {
		gCRPGStatsVMTMappings.MapVMTs();
		auto typeIt = gCRPGStatsVMTMappings.PropertyTypes.find(type);
		if (typeIt == gCRPGStatsVMTMappings.PropertyTypes.end()) {
			OsiError("Unknown object property type: " << (unsigned)type);
			return nullptr;
		}

		if (typeIt->second == nullptr) {
			OsiError("Cannot construct object property of type  " << (unsigned)type << " - VMT not mapped!");
			return nullptr;
		}

		*(void**)prop = typeIt->second;
	}

	prop->TypeId = type;
	prop->Context = (PropertyContext)0;
	prop->Conditions = nullptr;

	return prop;
}

std::optional<int> RPGStats::EnumLabelToIndex(FixedString const& enumName, char const* enumLabel)
{
	auto rpgEnum = ModifierValueLists.Find(enumName);
	if (rpgEnum == nullptr) {
		OsiError("No enum named '" << enumName << "' exists");
		return {};
	}

	auto index = rpgEnum->Values.find(FixedString(enumLabel));
	if (!index) {
		return {};
	} else {
		return index.Value();
	}
}

FixedString RPGStats::EnumIndexToLabel(FixedString const& enumName, int index)
{
	auto rpgEnum = ModifierValueLists.Find(enumName);
	if (rpgEnum == nullptr) {
		OsiError("No enum named '" << enumName << "' exists");
		return FixedString{};
	}

	auto value = rpgEnum->Values.find_by_value(index);
	if (value) {
		return value.Key();
	} else {
		return FixedString{};
	}
}

Modifier * RPGStats::GetModifierInfo(FixedString const& modifierListName, FixedString const& modifierName)
{
	auto modifiers = ModifierLists.Find(modifierListName);
	if (modifiers != nullptr) {
		return modifiers->Attributes.Find(modifierName);
	} else {
		return nullptr;
	}
}

ModifierList * RPGStats::GetTypeInfo(Object * object)
{
	return ModifierLists.Find(object->ModifierListIndex);
}

int RPGStats::GetOrCreateFixedString(const char * value)
{
	FixedString fs(value);
	if (!fs) return -1;

	auto & strings = FixedStrings;
	for (uint32_t i = 0; i < strings.size(); i++) {
		if (strings[i] == fs) {
			return i;
		}
	}

	strings.push_back(fs);
	return strings.size() - 1;
}

std::optional<StatAttributeFlags> RPGStats::StringToAttributeFlags(const char * value)
{
	StatAttributeFlags flags{ 0 };
	STDString token;
	std::istringstream tokenStream(value);
	while (std::getline(tokenStream, token, ';')) {
		auto label = EnumInfo<StatAttributeFlags>::Find(FixedString(token.c_str()));
		if (label) {
			flags |= *label;
		} else {
			OsiError("Invalid AttributeFlag: " << token);
		}
	}

	return flags;
}

IScriptCheckObject* RPGStats::BuildScriptCheckBlock(STDString const& source)
{
	auto build = GetStaticSymbols().ls__ScriptCheckBlock__Build;
	if (!build) {
		OsiError("ScriptCheckBlock::Build not available!");
		return nullptr;
	}

	return build(source, ConditionsManager.Variables, 0, (int)source.size());
}

IScriptCheckObject* RPGStats::BuildScriptCheckBlockFromProperties(STDString const& source)
{
	STDString updated = source;
	for (size_t i = 0; i < updated.size(); i++) {
		if (updated[i] == ';') {
			updated[i] = '&';
		}
	}
		
	return BuildScriptCheckBlock(updated);
}

float RPGStats::GetExtraData(FixedString const& key, float defaultVal) const
{
	auto it = ExtraData->Properties.find(key);
	if (it != ExtraData->Properties.end()) {
		return it.Value();
	} else {
		return defaultVal;
	}
}

Object * StatFindObject(char const * name, bool warnOnError)
{
	auto stats = GetStaticSymbols().GetStats();
	if (stats == nullptr) {
		OsiError("RPGStats not available");
		return nullptr;
	}

	auto object = stats->Objects.Find(name);
	if (object == nullptr) {
		if (warnOnError) {
			OsiError("Stat object '" << name << "' does not exist");
		}
		return nullptr;
	}

	return object;
}

Object * StatFindObject(int index)
{
	auto stats = GetStaticSymbols().GetStats();
	if (stats == nullptr) {
		OsiError("RPGStats not available");
		return nullptr;
	}

	auto object = stats->Objects.Find(index);
	if (object == nullptr) {
		OsiError("Stat object #" << index << " does not exist");
		return nullptr;
	}

	return object;
}

Object * SkillPrototype::GetStats() const
{
	return StatFindObject(StatsObjectIndex);
}

Object * StatusPrototype::GetStats() const
{
	return StatFindObject(StatsObjectIndex);
}

void CharacterStatsGetters::WrapAll()
{
	if (Wrapped) return;

/*#define DEFN_GETTER(type, name) if (Get##name != nullptr) { \
Wrapper##name.Wrap(Get##name); \
}

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER*/

	if (GetHitChance != nullptr) {
		WrapperHitChance.Wrap(GetHitChance);
	}

	Wrapped = true;
}

void CharacterStatsGetters::UnwrapAll()
{
#define DEFN_GETTER(type, name) Wrapper##name.ClearHook();
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER
	WrapperHitChance.ClearHook();
}

std::optional<int32_t> CharacterStatsGetters::GetStat(Character * character, 
	FixedString const& name, bool original, bool excludeBoosts)
{
	auto statType = EnumInfo<CharacterStatGetterType>::Find(name);
	if (!statType) {
		return {};
	}

	return GetStat(character, *statType, original, excludeBoosts);
}
	

std::optional<int32_t> CharacterStatsGetters::GetStat(Character * character, 
	CharacterStatGetterType statType, bool original, bool excludeBoosts)
{
	switch (statType) {
#define DEFN_GETTER(type, n) case CharacterStatGetterType::n: \
return CharacterStatGetter<n##Tag>(Get##n, Wrapper##n, character, original, excludeBoosts);

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

	case CharacterStatGetterType::PhysicalResistance:
		return character->GetPhysicalResistance(excludeBoosts);

	case CharacterStatGetterType::PiercingResistance:
		return character->GetPiercingResistance(excludeBoosts);

	case CharacterStatGetterType::CorrosiveResistance:
		return character->GetCorrosiveResistance(excludeBoosts);

	case CharacterStatGetterType::MagicResistance:
		return character->GetMagicResistance(excludeBoosts);

	case CharacterStatGetterType::DamageBoost:
		return character->GetDamageBoost();

	default:
		OsiError("No stat fetcher defined for stat type ID " << (unsigned)statType);
		return {};
	}
}


std::optional<int32_t> Character::GetHitChance(Character * target)
{
	auto getter = GetStaticSymbols().CharStatsGetters.GetHitChance;
	if (getter == nullptr) {
		return {};
	}

	return getter(this, target);
}


std::optional<int32_t> Character::GetStat(FixedString const& name, bool excludeBoosts)
{
	return GetStaticSymbols().CharStatsGetters.GetStat(this, name, false, excludeBoosts);
}


bool Character::HasTalent(TalentType talent, bool excludeBoosts)
{
	if (talent == TalentType::None) {
		return false;
	}

	auto getter = GetStaticSymbols().CharStatsGetters.GetTalent;
	if (getter) {
		return getter(this, talent, excludeBoosts);
	} else {
		return false;
	}
}


int32_t Character::GetAbility(AbilityType ability, bool excludeBoosts)
{
	auto getter = GetStaticSymbols().CharStatsGetters.GetAbility;
	if (getter) {
		return getter(this, ability, excludeBoosts, false);
	} else {
		return 0;
	}
}

Item * Character::GetItemBySlot(ItemSlot slot, std::optional<bool> mustBeEquipped)
{
	for (auto stat : EquippedItems) {
		if (stat->ItemSlot == slot && ((mustBeEquipped && !*mustBeEquipped) || stat->IsEquipped)) {
			auto index = stat->ItemStatsHandle;
			if (index >= 0 
				&& index < DivStats->ItemList->Handles.Handles.size()) {
				auto item = DivStats->ItemList->Handles.Handles[index];
				if (item) {
					return item;
				}
			}
		}
	}

	return nullptr;
}

Item * Character::GetMainWeapon()
{
	auto weapon = GetItemBySlot(ItemSlot::Weapon, true);
	if (weapon == nullptr || weapon->ItemType != EquipmentStatsType::Weapon) {
		weapon = GetItemBySlot(ItemSlot::Shield, true);
		if (weapon == nullptr || weapon->ItemType != EquipmentStatsType::Weapon) {
			weapon = DivStats->DefaultWeaponStats;
		}
	}

	return weapon;
}

Item * Character::GetOffHandWeapon()
{
	auto mainWeapon = GetItemBySlot(ItemSlot::Weapon, true);
	if (mainWeapon == nullptr || mainWeapon->ItemType != EquipmentStatsType::Weapon) {
		return nullptr;
	}

	auto offHandWeapon = GetItemBySlot(ItemSlot::Shield, true);
	if (offHandWeapon == nullptr || offHandWeapon->ItemType != EquipmentStatsType::Weapon) {
		return nullptr;
	}

	return offHandWeapon;
}

bool Character::IsBoostActive(uint32_t conditionMask)
{
	return conditionMask == 0
		|| (conditionMask & 1) && ActiveBoostConditions[1] > 0
		|| (conditionMask & 2) && ActiveBoostConditions[2] > 0
		|| (conditionMask & 4) && ActiveBoostConditions[3] > 0
		|| (conditionMask & 8) && ActiveBoostConditions[4] > 0
		|| (conditionMask & 0x10) && ActiveBoostConditions[5] > 0
		|| (conditionMask & 0x20) && ActiveBoostConditions[6] > 0
		|| (conditionMask & 0x40) && ActiveBoostConditions[7] > 0
		|| (conditionMask & 0x80) && ActiveBoostConditions[8] > 0
		|| (conditionMask & 0x100) && ActiveBoostConditions[9] > 0
		|| (conditionMask & 0x200) && ActiveBoostConditions[10] > 0
		|| (conditionMask & 0x300) && ActiveBoostConditions[11] > 0
		|| (conditionMask & 0x400) && ActiveBoostConditions[12] > 0
		|| (conditionMask & 0x500) && ActiveBoostConditions[13] > 0
		|| (conditionMask & 0x600) && ActiveBoostConditions[14] > 0
		|| (conditionMask & 0x700) && ActiveBoostConditions[15] > 0;
}

bool Character::ComputeScaledDamage(Item* weapon, DamageDescList& damages, bool keepCurrentDamages)
{
	struct DamageRange
	{
		int32_t minDamage{ 0 };
		int32_t maxDamage{ 0 };
	};

	auto maxDamageType = gExtender->GetCustomDamageTypes().MaxDamageTypeId();
	ObjectSet<DamageRange> damageRanges;
	damageRanges.resize(maxDamageType);

	if (!keepCurrentDamages) {
		damages.clear();
	}

	auto computeDamageProc = GetStaticSymbols().CDivinityStats_Item__ComputeDamage;
	auto getDamageBoostProc = GetStaticSymbols().CDivinityStats_Character__GetDamageBoost;
	auto getWeapopnAbilityProc = GetStaticSymbols().CDivinityStats_Character__GetWeaponAbility;
	auto getWeapopnAbilityBoostProc = GetStaticSymbols().CDivinityStats_Character__GetWeaponAbilityBoost;
	auto getItemRequirementAttributeProc = GetStaticSymbols().CDivinityStats_Character__GetItemRequirementAttribute;
	auto scaledDamageFromPrimaryAttributeProc = GetStaticSymbols().eoc__ScaledDamageFromPrimaryAttribute;

	weapon->ComputeDamage(damages, keepCurrentDamages);

	for (uint32_t i = 0; i < damages.size(); i++) {
		auto const& dmg = damages[i];
		damageRanges[(uint32_t)dmg.DamageType].minDamage += dmg.MinDamage;
		damageRanges[(uint32_t)dmg.DamageType].maxDamage += dmg.MaxDamage;
	}

	damages.clear();

	auto damageBoost = getDamageBoostProc(this);
	int32_t weaponAbilityBoost{ 0 };
	if (weapon->ItemType == EquipmentStatsType::Weapon) {
#if defined(OSI_EOCAPP)
		auto weaponAbility = getWeapopnAbilityProc(this, weapon);
		weaponAbilityBoost = getWeapopnAbilityBoostProc(this, weaponAbility, false, 0);
#else
		weaponAbilityBoost = getWeapopnAbilityBoostProc(this, weapon, false);
#endif
	}

	uint32_t requirementId{ 0 };
	int32_t attributeDmgBoost{ 0 };
	int32_t sneakDamageMultiplier{ 0 };
	auto itemRequirementAttribute = getItemRequirementAttributeProc(this, weapon, requirementId, false);
	if (itemRequirementAttribute) {
		attributeDmgBoost = (int32_t)roundf(scaledDamageFromPrimaryAttributeProc(itemRequirementAttribute) * 100.0f);
	}

	if ((Flags & CharacterFlags::IsSneaking) == CharacterFlags::IsSneaking) {
		sneakDamageMultiplier = (int)GetStaticSymbols().GetStats()->GetExtraData(GFS.strSneakDamageMultiplier);
	}

	auto damageMultiplier = 100 + attributeDmgBoost + weaponAbilityBoost + damageBoost + sneakDamageMultiplier;
	damageMultiplier = std::max(damageMultiplier, -100);

	for (uint32_t damageType = 0; damageType < maxDamageType; damageType++) {
		auto& dmgRange = damageRanges[damageType];
		if (dmgRange.minDamage || dmgRange.maxDamage) {
			DamageDesc dmgDesc{
				.MinDamage = (int32_t)ceilf((float)(damageMultiplier * dmgRange.minDamage) / 100.0f),
				.MaxDamage = (int32_t)ceilf((float)(damageMultiplier * dmgRange.maxDamage) / 100.0f),
				.DamageType = (DamageType)damageType,
				.field_C = 0
			};

			damages.push_back(dmgDesc);
		}
	}

	return true;
}

template <class IterFunc, class ItemFunc>
int32_t ComputeCharacterResistance(Character * character, IterFunc iter, ItemFunc iterItem, bool excludeBoosts)
{
	int32_t value = 0;
	bool appliedMaxResist = false;

	auto lastStatIt = excludeBoosts ? (character->DynamicStats.begin() + 3) : character->DynamicStats.end();

	unsigned i = 0;
	for (auto it = character->DynamicStats.begin(); it != lastStatIt; it++, i++) {
		auto & stat = *it;
		if (stat->BoostConditionsMask == 0 || character->IsBoostActive(stat->BoostConditionsMask)) {
			value = iter(character, value, i, stat);

			if (!appliedMaxResist) {
				value = std::min(value, character->MaxResistance);
				appliedMaxResist = true;
			}
		}
	}

	if (!excludeBoosts) {
		auto & itemHandles = character->DivStats->ItemList->Handles;
		for (auto stat : character->EquippedItems) {
			if (stat->IsEquipped && stat->ItemStatsHandle >= 0 && stat->ItemStatsHandle < itemHandles.Handles.size()) {
				auto item = itemHandles.Handles[stat->ItemStatsHandle];
				if (item != nullptr) {
					value = iterItem(character, value, item);
				}
			}
		}
	}

	return value;
}

int32_t Character::GetPhysicalResistance(bool excludeBoosts)
{
	auto addDynamic = [](Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
		return val + stat->PhysicalResistance;
	};
	auto addItem = [](Character * self, int32_t val, Item * item) -> int32_t {
		return val + item->GetPhysicalResistance();
	};

	return ComputeCharacterResistance(this, addDynamic, addItem, excludeBoosts);
}

int32_t Character::GetPiercingResistance(bool excludeBoosts)
{
	auto addDynamic = [](Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
		return val + stat->PiercingResistance;
	};
	auto addItem = [](Character * self, int32_t val, Item * item) -> int32_t {
		return val + item->GetPiercingResistance();
	};

	return ComputeCharacterResistance(this, addDynamic, addItem, excludeBoosts);
}

int32_t Character::GetCorrosiveResistance(bool excludeBoosts)
{
	auto addDynamic = [](Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
		return val + stat->CorrosiveResistance;
	};
	auto addItem = [](Character * self, int32_t val, Item * item) -> int32_t {
		return val + item->GetCorrosiveResistance();
	};

	return ComputeCharacterResistance(this, addDynamic, addItem, excludeBoosts);
}

int32_t Character::GetMagicResistance(bool excludeBoosts)
{
	auto addDynamic = [](Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
		return val + stat->MagicResistance;
	};
	auto addItem = [](Character * self, int32_t val, Item * item) -> int32_t {
		return val + item->GetMagicResistance();
	};

	return ComputeCharacterResistance(this, addDynamic, addItem, excludeBoosts);
}

int32_t Character::GetDamageBoost()
{
	int32_t damageBoost = 0;

	for (auto dynamicStat : DynamicStats) {
		if (dynamicStat->BoostConditionsMask == 0 || IsBoostActive(dynamicStat->BoostConditionsMask)) {
			damageBoost += dynamicStat->DamageBoost;
			if (Level > 1) {
				damageBoost += (int)round(dynamicStat->DamageBoostGrowthPerLevel * (Level - 1) * 0.1f);
			}
		}
	}

	return damageBoost;
}

int32_t Item::GetPhysicalResistance()
{
	int32_t resistance = 0;
	for (auto dynamicStat : DynamicStats) {
		resistance += dynamicStat->PhysicalResistance;
	}

	return resistance;
}

int32_t Item::GetPiercingResistance()
{
	int32_t resistance = 0;
	for (auto dynamicStat : DynamicStats) {
		resistance += dynamicStat->PiercingResistance;
	}

	return resistance;
}

int32_t Item::GetMagicResistance()
{
	int32_t resistance = 0;
	for (auto dynamicStat : DynamicStats) {
		resistance += dynamicStat->MagicResistance;
	}

	return resistance;
}

int32_t Item::GetCorrosiveResistance()
{
	int32_t resistance = 0;
	for (auto dynamicStat : DynamicStats) {
		resistance += dynamicStat->CorrosiveResistance;
	}

	return resistance;
}


bool Item::HasTalent(TalentType talent)
{
	for (auto dynamicStat : DynamicStats) {
		if (dynamicStat->Talents.HasTalent(talent)) {
			return true;
		}
	}

	return false;
}


int32_t Item::GetAbility(AbilityType ability)
{
	int32_t points = 0;
	for (auto dynamicStat : DynamicStats) {
		points += dynamicStat->AbilityModifiers[(unsigned)ability];
	}

	return points;
}

bool Item::ComputeDamage(DamageDescList& damages, bool keepCurrentDamages)
{
	struct DamageRange
	{
		float minDamage{ .0f };
		float maxDamage{ .0f };
	};

	auto maxDamageType = gExtender->GetCustomDamageTypes().MaxDamageTypeId();
	ObjectSet<DamageRange> damageRanges;
	damageRanges.resize(maxDamageType);

	if (!keepCurrentDamages) {
		damages.clear();
	}

	int32_t damageBoost = 0;

	auto base = (EquipmentAttributesWeapon*)DynamicStats[0];
	for (auto attr : DynamicStats) {
		if (attr->StatsType != EquipmentStatsType::Weapon) {
			continue;
		}

		auto stat = (EquipmentAttributesWeapon*)attr;
		auto minDamage = (float)stat->MinDamage;
		auto maxDamage = (float)stat->MaxDamage;
		if (stat->DamageFromBase) {
			if (base == stat) {
				if (base->MinDamage) {
					minDamage = fmaxf((stat->DamageFromBase * stat->MinDamage) * 0.01f, 1.0f);
				}
				
				if (base->MaxDamage) {
					maxDamage = fmaxf((stat->DamageFromBase * stat->MaxDamage) * 0.01f, 1.0f);
				}
			} else {
				auto dmgFromBase = base->DamageFromBase * 0.01f;
				auto dmgFromBase2 = stat->DamageFromBase * 0.01f;
				maxDamage = fmaxf(base->MaxDamage * dmgFromBase * dmgFromBase2, 1.0f);
				minDamage = fmaxf(base->MinDamage * dmgFromBase * dmgFromBase2, 1.0f);
			}
		}

		if (minDamage > 0.0f) {
			maxDamage = fmaxf(maxDamage, minDamage + 1.0f);
		}

		auto& dmgRange = damageRanges[(uint32_t)stat->DamageType];
		damageBoost += stat->DamageBoost;
		dmgRange.minDamage += minDamage;
		dmgRange.maxDamage += maxDamage;
	}

	for (uint32_t damageType = 0; damageType < maxDamageType; damageType++) {
		auto& dmgRange = damageRanges[damageType];
		if (dmgRange.minDamage || dmgRange.maxDamage) {
			if (damageBoost) {
				auto multiplier = (damageBoost * 0.01f) + 1.0f;
				DamageDesc dmgDesc{
					.MinDamage = (int32_t)ceilf(multiplier * dmgRange.minDamage),
					.MaxDamage = (int32_t)ceilf(multiplier * dmgRange.maxDamage),
					.DamageType = (DamageType)damageType,
					.field_C = 0
				};

				damages.push_back(dmgDesc);
			} else {
				DamageDesc dmgDesc{
					.MinDamage = (int32_t)roundf(dmgRange.minDamage),
					.MaxDamage = (int32_t)roundf(dmgRange.maxDamage),
					.DamageType = (DamageType)damageType,
					.field_C = 0
				};

				damages.push_back(dmgDesc);
			}
		}
	}

	return damages.size() != 0;
}

bool PropertyExtender::GetDescription(STDWString* Line1, void* unknown, stats::Character* character, bool unknown2)
{
	ecl::LuaClientPin lua(ecl::ExtensionState::Get());
	if (lua) {
		auto desc = lua->GetSkillPropertyDescription(this);
		if (desc) {
			*Line1 = FromUTF8(*desc);
			return true;
		}
	}

	*Line1 = L"";
	return true;
}


LegacyPropertyMapBase & EquipmentAttributes::GetPropertyMap() const
{
	switch (StatsType) {
	case EquipmentStatsType::Weapon:
		return gEquipmentAttributesWeaponPropertyMap;

	case EquipmentStatsType::Armor:
		return gEquipmentAttributesArmorPropertyMap;

	case EquipmentStatsType::Shield:
		return gEquipmentAttributesShieldPropertyMap;

	default:
		OsiError("Unknown equipment stats type: " << (unsigned)StatsType);
		return gEquipmentAttributesPropertyMap;
	}
}

END_NS()

BEGIN_SE()


#define DEFN_GETTER(type, name) decltype(stats::CharacterStatsGetters::Wrapper##name) * decltype(stats::CharacterStatsGetters::Wrapper##name)::gHook;
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

decltype(stats::CharacterStatsGetters::WrapperHitChance)* decltype(stats::CharacterStatsGetters::WrapperHitChance)::gHook;

END_SE()
