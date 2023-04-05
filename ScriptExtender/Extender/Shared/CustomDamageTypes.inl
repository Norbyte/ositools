#include <ScriptHelpers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_SE()

using namespace dse::stats;

CustomDamageTypeCallbacks* CustomDamageTypeDescriptor::GetCallbacks()
{
	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		return lua->GetCustomDamageTypes().GetOrRegister(DamageTypeId);
	} else {
		return nullptr;
	}
}


CustomDamageTypeDescriptor* CustomDamageTypeHelpers::AssignType(FixedString const& name)
{
	auto id = EnumInfo<DamageType>::Find(name);
	if (!id) {
		OsiError("Tried to register damage type info for nonexistent type '" << name << "'!");
		return nullptr;
	}

	auto desc = types_.insert(std::make_pair((uint32_t)*id, CustomDamageTypeDescriptor{}));
	desc.first->second.DamageTypeId = (uint32_t)*id;
	desc.first->second.EnumLabel = name;

	typeNames_.insert(std::make_pair(name, &desc.first->second));
	return &desc.first->second;
}

CustomDamageTypeDescriptor* CustomDamageTypeHelpers::RegisterNewType(FixedString const& name)
{
	auto damageType = EnumInfo<DamageType>::Find(name);
	if (damageType) {
		if (*damageType <= DamageType::Sentinel) {
			OsiError("Tried to register damage type '" << name << "' multiple times!");
			return nullptr;
		}

		auto it = types_.find((uint32_t)*damageType);
		assert(it != types_.end());
		return &it->second;
	}

	auto id = nextDamageTypeId_++;

	auto desc = types_.insert(std::make_pair(id, CustomDamageTypeDescriptor{}));
	desc.first->second.DamageTypeId = id;
	desc.first->second.EnumLabel = name;

	typeNames_.insert(std::make_pair(name, &desc.first->second));

	EnumInfo<DamageType>::Store->Add(id, name.GetStringOrDefault());

	auto damageTypeEnum = GetStaticSymbols().GetStats()->ModifierValueLists.Find("Damage Type");
	damageTypeEnum->Values.insert(std::make_pair(name, id));

	return &desc.first->second;
}

void CustomDamageTypeHelpers::Clear()
{
	EnumInfo<DamageType>::Store->Labels.resize((uint32_t)DamageType::Sentinel + 1);
	for (auto const& ty : typeNames_) {
		auto it = EnumInfo<DamageType>::Store->Values.find(ty.first);
		if (it != EnumInfo<DamageType>::Store->Values.end()) {
			EnumInfo<DamageType>::Store->Values.erase(it);
		}
	}

	types_.clear();
	typeNames_.clear();
	nextDamageTypeId_ = (uint32_t)DamageType::Sentinel + 1;
}

DamageType CustomDamageTypeHelpers::GetDamageType(stats::GetDamageTypeProc* wrapped, char const* damageType)
{
	auto it = typeNames_.find(FixedString(damageType));
	if (it != typeNames_.end()) {
		return (DamageType)it->second->DamageTypeId;
	}

	return wrapped(damageType);
}

char const* CustomDamageTypeHelpers::GetDamageTypeString(stats::GetDamageTypeStringProc* wrapped, DamageType damageType)
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end()) {
		return it->second.EnumLabel.GetStringOrDefault();
	}

	return wrapped(damageType);
}

TranslatedString* CustomDamageTypeHelpers::DamageTypeToTranslateString(stats::DamageTypeToTranslateStringProc* wrapped, 
	TranslatedString* result, DamageType damageType)
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end()) {
		new (result) TranslatedString();
		script::GetTranslatedString(it->second.NameHandle.GetStringOrDefault(), it->second.Name.c_str(), *result);
		return result;
	}

	return wrapped(result, damageType);
}

TranslatedString* CustomDamageTypeHelpers::DamageDescriptionToTranslateString(stats::DamageDescriptionToTranslateStringProc* wrapped, 
	TranslatedString* result, DamageType damageType)
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end()) {
		new (result) TranslatedString();
		script::GetTranslatedString(it->second.DescriptionHandle.GetStringOrDefault(), it->second.Description.c_str(), *result);
		return result;
	}

	return wrapped(result, damageType);
}

TranslatedString* CustomDamageTypeHelpers::DamageTypeToTranslateStringExtended(stats::DamageTypeToTranslateStringExtendedProc* wrapped, 
	TranslatedString* result, DamageType damageType)
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end()) {
		new (result) TranslatedString();
		script::GetTranslatedString(it->second.NameExtendedHandle.GetStringOrDefault(), it->second.NameExtended.c_str(), *result);
		return result;
	}

	return wrapped(result, damageType);
}

uint64_t CustomDamageTypeHelpers::GetColorCodeDmg(stats::GetColorCodeDmgProc* wrapped, DamageType damageType)
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end() && it->second.ColorCode >= 0 && it->second.ColorCode < 77) {
		return (uint32_t)it->second.ColorCode;
	}

	return wrapped(damageType);
}

#if defined(OSI_EOCAPP)
void CustomDamageTypeHelpers::GetColorCodeAndTypeDmg(stats::ColorCodeAndTypeDmgProc* wrapped, eoc::Text* text, unsigned int amount,
	bool reflected, DamageType damageType)
#else
void CustomDamageTypeHelpers::GetColorCodeAndTypeDmg(stats::ColorCodeAndTypeDmgProc* wrapped, eoc::Text* text, DamageType& damageType,
	unsigned int amount, bool reflected)
#endif
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end()) {
		TranslatedString amountAndType;
		script::GetTranslatedString(it->second.AmountAndTypeTextHandle.GetStringOrDefault(), it->second.AmountAndTypeText.c_str(), amountAndType);
		
		auto txt = amountAndType.Handle.ReferenceString;
		auto dmgPos = txt.find(L"[1]");
		if (dmgPos != std::string::npos) {
			txt = txt.substr(0, dmgPos) + FromUTF8(std::to_string(amount)) + txt.substr(dmgPos + 3);
		}

		if (reflected) {
			STDWString reflectedStr;
			script::GetTranslatedString("h5c7e82f0gf29dg4dd8g973eg7ba53972207f", reflectedStr);
			txt += reflectedStr;
		}

		text->Buf->Replace(txt);
		return;
	}

#if defined(OSI_EOCAPP)
	return wrapped(text, amount, reflected, damageType);
#else
	return wrapped(text, damageType, amount, reflected);
#endif
}

#if defined(OSI_EOCAPP)
bool CustomDamageTypeHelpers::ComputeScaledDamage(stats::Character::ComputeScaledDamageProc* wrapped, Character* self, Item* weapon,
	DamageDescList* damages, bool keepCurrentDamages)
#else
bool CustomDamageTypeHelpers::ComputeScaledDamage(stats::Character::ComputeScaledDamageProc* wrapped, Character* self, Item* weapon,
	DamageDescList* damages, bool includeBoosts, bool keepCurrentDamages)
#endif
{
	return self->ComputeScaledDamage(weapon, *damages, keepCurrentDamages);
}

bool CustomDamageTypeHelpers::ComputeItemDamage(stats::Item::ComputeDamageProc* wrapped, Item* self,
	DamageDescList* damages, bool keepCurrentDamages)
{
	return self->ComputeDamage(*damages, keepCurrentDamages);
}

#if defined(OSI_EOCAPP)
int32_t CustomDamageTypeHelpers::GetResistance(stats::Character::GetResistanceProc* wrapped, Character* self,
	DamageType damageType, bool baseValues)
#else
int32_t CustomDamageTypeHelpers::GetResistance(stats::Character::GetResistanceProc* wrapped, Character* self,
	DamageType damageType, bool baseValues, bool excludeBoosts)
#endif
{
	LuaVirtualPin lua;
	if (lua) {
		auto resistance = lua->GetCustomDamageTypes().GetResistance(self, damageType, baseValues);
		if (resistance) {
			return *resistance;
		}
	}

#if defined(OSI_EOCAPP)
	return wrapped(self, damageType, baseValues);
#else
	return wrapped(self, damageType, baseValues, excludeBoosts);
#endif
}

#if defined(OSI_EOCAPP)
float CustomDamageTypeHelpers::GetDamageBoostByType(stats::Character::GetDamageBoostByTypeProc* wrapped, Character* self,
	DamageType damageType)
#else
float CustomDamageTypeHelpers::GetDamageBoostByType(stats::Character::GetDamageBoostByTypeProc* wrapped, Character* self,
	DamageType damageType, bool excludeBoosts)
#endif
{
	LuaVirtualPin lua;
	if (lua) {
		auto boost = lua->GetCustomDamageTypes().GetDamageBoostByType(self, damageType);
		if (boost) {
			return *boost;
		}
	}

#if defined(OSI_EOCAPP)
	return wrapped(self, damageType);
#else
	return wrapped(self, damageType, excludeBoosts);
#endif
}

CustomDamageTypeCallbacks* CustomDamageTypeCallbackManager::GetOrRegister(uint32_t damageTypeId)
{
	auto it = types_.find(damageTypeId);
	if (it != types_.end()) {
		return &it->second;
	}

	auto desc = types_.insert(std::make_pair(damageTypeId, CustomDamageTypeCallbacks{}));
	desc.first->second.DamageTypeId = damageTypeId;
	return &desc.first->second;
}

void CustomDamageTypeCallbackManager::Clear()
{
	types_.clear();
}

std::optional<int32_t> CustomDamageTypeCallbackManager::GetResistance(Character* self, DamageType damageType, bool baseValues)
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end() && *it->second.GetResistanceCallback) {
		auto resistance = lua::ProtectedCallFunction<int32_t>(*it->second.GetResistanceCallback, self, damageType, baseValues);
		if (resistance) {
			return *resistance;
		}
	}

	return {};
}

std::optional<float> CustomDamageTypeCallbackManager::GetDamageBoostByType(Character* self, DamageType damageType)
{
	auto it = types_.find((uint32_t)damageType);
	if (it != types_.end() && *it->second.GetDamageBoostCallback) {
		auto damageBoost = lua::ProtectedCallFunction<float>(*it->second.GetDamageBoostCallback, self, damageType);
		if (damageBoost) {
			return *damageBoost;
		}
	}

	return {};
}

END_SE()
