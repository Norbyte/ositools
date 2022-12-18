#include <ScriptHelpers.h>

BEGIN_SE()

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
	if (EnumInfo<DamageType>::Find(name)) {
		OsiError("Tried to register damage type '" << name << "' multiple times!");
		return nullptr;
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

END_SE()
