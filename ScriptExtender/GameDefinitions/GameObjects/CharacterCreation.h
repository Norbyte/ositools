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
	ColorDesc DefaultColor;
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
	TranslatedString DefaultAiPersonality;
	FixedString field_800;
	VoiceDesc DefaultVoice;
	ObjectSet<AiPersonalityDesc> AiPersonalities;
	ObjectSet<VoiceDesc> Voices;
	RefMap<uint32_t, ObjectSet<FixedString>*> Tags;
	uint8_t field_918;
	RefMap<FixedString, void*> SubtitlePlayers;
};

END_NS()


BEGIN_NS(ecl::character_creation)

using namespace dse::character_creation;

struct CharacterCustomizationState
{
	ClassDesc* Class;
	int AttributePointsAssigned;
	int CombatAbilityPointsAssigned;
	int CivilAbilityPointsAssigned;
	int TalentPointsAssigned;
	STDWString Name;
	STDWString VerifiedName;
	ColorDesc SkinColor;
	ColorDesc HairColor;
	ColorDesc Color1;
	ColorDesc Color2;
	ColorDesc Color3;
	bool IsMale;
	FixedString Equipment;
	FixedString MusicInstrumentID;
	FixedString Origin;
	FixedString Icon;
	FixedString RootTemplate;
	uint64_t VisualSetIndices;
	TranslatedString AiPersonality;
	int VoiceId;
	RefMap<int, int> field_4B0;
	ObjectSet<FixedString> Skills;
};


struct CharacterCustomization
{
	void* VMT;
	ScratchBuffer ScratchStr;
	__int64 field_28;
	ComponentHandle CharacterHandle;
	int field_38;
	char field_3C;
	uint8_t ArmorState;
	CharacterCustomizationState State;
	CharacterCustomizationState State2;
};


struct CharacterCreationManager
{
	void* GameEventManagerListenerVMT;
	__int64 field_8;
	Map<NetId, CharacterCustomization*> Customizations;
	Map<NetId, int32_t> RefCounts;
	int field_40;
	uint8_t field_44;
	int field_48;
	ecl::EoCUI* CharCreationUI1;
	ecl::EoCUI* CharCreationUI2;
	ComponentHandle CharCreationUIHandle1;
	ComponentHandle CharCreationUIHandle2;
	void* field_70;
	void* field_78;
};


struct UICharacterCreationWizard : public ecl::EoCUI
{
	struct FlashSlotData
	{
		uint32_t Index;
		FixedString Value;
	};

	struct Points
	{
		uint8_t Attribute;
		uint8_t Ability;
		uint8_t Civil;
		uint8_t SkillSlots;
		uint8_t Talent;
	};

	void* IMessageBoxOwnerVMT;
	void* VideoListenerVMT;
	void* VMT2;
	void* NetEventManagerListenerVMT;
	uint8_t field_1B8;
	uint8_t CurrentStep;
	ComponentHandle CharacterHandle;
	void* PlayerIconRenderer;
	CustomDrawStruct IconTT;
	CustomDrawStruct IconP;
	CustomDrawStruct IconS;
	CustomDrawStruct IconR;
	CustomDrawStruct IconG;
	void* field_298;
	ObjectSet<CustomDrawStruct> CustomIcons1;
	ObjectSet<CustomDrawStruct> CustomIcons2;
	ObjectSet<void*> UserIconStructs;
	CharacterCreationManager* CharacterCreationManager;
	ObjectSet<FlashSlotData> FlashSlots;
	uint8_t SkillIconSize;
	uint8_t ListSpacing;
	uint8_t IconSpacingH;
	uint8_t IconSpacingV;
	uint8_t NumberOfCols;
	Points AvailablePoints;
	Points AssignedPoints;
	int CharIconWidth;
	int CharIconHeight;
	float UserIconWidth;
	float UserIconHeight;
	NetId CharacterNetId;
	float Rotation;
	float IconSize;
	float IconSpacing;
	uint16_t ChosenListCols;
	uint8_t ChosenListIconSize;
	int ChosenListSpacingH;
	int ChosenListSpacingV;
	uint8_t CCFlags;
	__int64 field_368;
	__int64 field_370;
	ObjectSet<Visual*> Visuals;
	float Zoom;
	uint8_t field_39C;
	void* ZoomCamera;
	void* CurrentCamera;
	float ZoomCameraDistance;
	uint8_t RotateFlag;
	int MousePos;
	int field_3BC;
	ObjectSet<FixedString> Voices;
	__int64 field_3E0;
	uint8_t OriginIndex;
	uint8_t field_3E9;
	__int64 field_3F0;
	bool IsMale;
	uint8_t field_3F9;
	uint8_t field_3FA;
	int PlayerIndex;
	uint8_t field_400;
	uint8_t CCFlags2;
	int field_404;
	int field_408;
	int field_40C;
	int field_410;
	void* SubtitlePlayer;
	__int64 field_420;
	uint8_t HelmetState;
	__int64 VerifyingName;
	char field_438;
	glm::vec3 Position;
	int field_448;
	uint8_t ExtraStatPointTalentFlags;
	void* CustomizationSteps;
	char field_458;
	char field_459;
	char field_45A;
	int field_45C;
	STDWString field_460;
};


END_NS()
