#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_NS(character_creation)

struct ColorDesc
{
	TranslatedString ColorName;
	uint32_t Value;
	uint32_t ID;
};


struct AttributeChange
{
	PlayerUpgradeAttribute Attribute;
	int AmountIncreased;
};


struct AbilityChange
{
	AbilityType Ability;
	int AmountIncreased;
};


struct EquipmentProperty
{
	FixedString RaceName;
	FixedString PreviewEquipmentSet;
	FixedString StartingEquipmentSet;
};


struct RaceDesc
{
	TranslatedString RaceDisplayName;
	FixedString RaceName;
	TranslatedString RaceDescription;
	FixedString SkillSet;
	ObjectSet<FixedString> MaleIcons;
	ObjectSet<FixedString> FemaleIcons;
	ObjectSet<FixedString> MaleRootTemplateIDs;
	ObjectSet<FixedString> FemaleRootTemplateIDs;
	ObjectSet<TranslatedString> MaleDefaultPlayerNames;
	ObjectSet<TranslatedString> FemaleDefaultPlayerNames;
	ObjectSet<AttributeChange> AttributeChanges;
	ObjectSet<AbilityChange> AbilityChanges;
	ObjectSet<FixedString> TagsAdded;
	ObjectSet<stats::TalentType> TalentsAdded;
	ObjectSet<ColorDesc> SkinColors;
	ObjectSet<ColorDesc> HairColors;
	ObjectSet<ColorDesc> ClothColors;
	TranslatedString Color_M;
	int field_3A8;
	int field_3AC;
	uint32_t FemaleDefaultHairColor;
	uint32_t FemaleDefaultSkinColor;
	uint32_t MaleDefaultHairColor;
	uint32_t MaleDefaultSkinColor;
	bool AllowBald;
};


struct OriginDesc
{
	TranslatedString OriginDisplayName;
	TranslatedString OriginDescription;
	TranslatedString OriginLongDescription;
	FixedString SkillSet;
	FixedString CharacterUUID;
	FixedString OriginName;
	FixedString PreviewEquipmentSet;
	FixedString RootTemplateOverride;
	bool UserCanAlterTags;
	ObjectSet<uint32_t> SupportedGenders;
	ObjectSet<FixedString> SupportedRaces;
	ObjectSet<AttributeChange> AttributeChanges;
	ObjectSet<AbilityChange> AbilityChanges;
	ObjectSet<stats::TalentType> TalentsAdded;
};


struct ClassDesc
{
	TranslatedString ClassName;
	TranslatedString ClassDescription;
	TranslatedString ClassLongDescription;
	FixedString ClassType;
	FixedString SkillSet;
	int Voice;
	int Icon;
	ObjectSet<EquipmentProperty> EquipmentProperties;
	ObjectSet<AttributeChange> AttributeChanges;
	ObjectSet<AbilityChange> AbilityChanges;
	ObjectSet<stats::TalentType> TalentsAdded;
	int NumStartingAttributePoints;
	int NumStartingCombatAbilityPoints;
	int NumStartingCivilAbilityPoints;
	int NumStartingTalentPoints;
	int Price;
	bool AreStatsWeighted;
};


struct AiPersonalityDesc
{
	TranslatedString Name;
	FixedString ID;
};


struct VoiceDesc
{
	uint32_t ID;
	FixedString FemaleSpeakerID;
	FixedString MaleSpeakerID;
	TranslatedString Name;
};


struct CharacterCreationManager
{
	void* VMT;
	ObjectSet<ClassDesc*> ClassPresets;
	ObjectSet<ClassDesc*> HenchmanPresets;
	ObjectSet<ClassDesc*> CompanionPresets;
	ObjectSet<RaceDesc> RacePresets;
	ObjectSet<OriginDesc> GenericOriginPresets;
	ObjectSet<OriginDesc> OriginPresets;
	RaceDesc DefaultRace;
	OriginDesc DefaultOrigin;
	TranslatedString field_758;
	FixedString field_800;
	uint32_t field_808;
	FixedString field_810;
	FixedString field_818;
	TranslatedString field_820;
	ObjectSet<AiPersonalityDesc> AiPersonalities;
	ObjectSet<VoiceDesc> Voices;
	RefMap<uint32_t, ObjectSet<FixedString>*> Tags;
	uint8_t field_918;
	RefMap<FixedString, void*> SubtitlePlayers;
};



END_SE()
