#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>
#include <GameDefinitions/Symbols.h>

namespace osidbg
{
	ecl::EoCClient * GetEoCClient()
	{
		auto clientPtr = gStaticSymbols.EoCClient;
		if (clientPtr == nullptr || *clientPtr == nullptr) {
			return nullptr;
		} else {
			return *clientPtr;
		}
	}

	ModManager * GetModManager()
	{
		auto client = GetEoCClient();
		if (client == nullptr || client->ModManager == nullptr) {
			return nullptr;
		} else {
			return client->ModManager;
		}
	}

	FixedString ToFixedString(const char * s)
	{
		if (s == nullptr) {
			OsiErrorS("Attempted to look up a null string!");
			return FixedString{};
		}

		auto stringTable = gStaticSymbols.GetGlobalStringTable();
		if (stringTable == nullptr) {
			OsiErrorS("Global string table not available!");
			return FixedString{};
		}

		auto str = stringTable->Find(s, strlen(s));
		return FixedString(str);
	}

	FixedString MakeFixedString(const char * s)
	{
		auto str = ToFixedString(s);
		if (!str) {
			auto createFixedString = gStaticSymbols.CreateFixedString;
			if (createFixedString != nullptr) {
#if defined(OSI_EOCAPP)
				str = FixedString(createFixedString(s, -1));
#else
				createFixedString(&str, s, -1);
#endif
				if (!str) {
					OsiErrorS("Failed to register FixedString in global string table?!");
				}
			} else {
				OsiErrorS("ls::FixedString::Create not available!");
			}
		}

		return str;
	}

	FixedString NameGuidToFixedString(char const * nameGuid)
	{
		if (nameGuid == nullptr) {
			return FixedString{};
		}

		auto nameLen = strlen(nameGuid);
		if (nameLen < 36) {
			OsiError("GUID (" << nameGuid << ") too short!");
			return FixedString{};
		}

		auto guid = nameGuid + nameLen - 36;

		auto stringTable = gStaticSymbols.GetGlobalStringTable();
		if (stringTable == nullptr) {
			OsiErrorS("Global string table not available!");
			return FixedString{};
		}

		return FixedString(stringTable->Find(guid, 36));
	}

	esv::EoCServerObject * FindGameObjectByNameGuid(char const * nameGuid, bool logError)
	{
		if (nameGuid == nullptr) {
			OsiError("Attempted to look up object with null name!");
			return nullptr;
		}

		auto character = FindCharacterByNameGuid(nameGuid, false);
		if (character != nullptr) {
			return character;
		}

		auto item = FindItemByNameGuid(nameGuid, false);
		if (item != nullptr) {
			return item;
		}

		if (logError) {
			OsiError("No EoC server object found with GUID '" << nameGuid << "'");
		}

		return nullptr;
	}

	esv::EoCServerObject * FindGameObjectByHandle(ObjectHandle const & handle, bool logError)
	{
		if (handle.Handle == 0) {
			return nullptr;
		}

		auto character = FindCharacterByHandle(handle, false);
		if (character != nullptr) {
			return character;
		}

		auto item = FindItemByHandle(handle, false);
		if (item != nullptr) {
			return item;
		}

		if (logError) {
			OsiError("No EoC server object found with handle 0x" << std::hex << handle.Handle);
		}

		return nullptr;
	}

	EntityWorld * GetEntityWorld()
	{
		auto charFactory = gStaticSymbols.GetCharacterFactory();
		if (charFactory != nullptr) {
			return charFactory->Entities;
		}

		auto itemFactory = gStaticSymbols.GetItemFactory();
		if (itemFactory != nullptr) {
			return itemFactory->Entities;
		}

		OsiErrorS("EntityWorld not available!");
		return nullptr;
	}



	std::string ComponentTypeToName(ComponentType type)
	{
		auto label = EnumInfo<ComponentType>::Find(type);
		if (label) {
			return *label;
		} else {
			return std::string("<Type ") + std::to_string((uint32_t)type) + ">";
		}
	}

	void * FindComponentByNameGuid(ComponentType componentType, char const * nameGuid, bool logError)
	{
		if (nameGuid == nullptr) {
			OsiError("Attempted to look up component with null name!");
			return nullptr;
		}

		auto fs = NameGuidToFixedString(nameGuid);
		if (!fs) {
			OsiError("Could not map GUID '" << nameGuid << "' to FixedString");
			return nullptr;
		}

		auto entityWorld = GetEntityWorld();
		if (entityWorld == nullptr) {
			return nullptr;
		}

		auto component = entityWorld->Components[(uint32_t)componentType].component->FindComponentByGuid(&fs);
		if (component != nullptr) {
			return component;
		} else {
			if (logError) {
				OsiError("No " << ComponentTypeToName(componentType) << " component found with GUID '" << nameGuid << "'");
			}
			return nullptr;
		}
	}

	void * FindComponentByHandle(ComponentType componentType, ObjectHandle const & handle, bool logError)
	{
		if (handle.Handle == 0) {
			return nullptr;
		}

		auto entityWorld = GetEntityWorld();
		if (entityWorld == nullptr) {
			return nullptr;
		}

		auto component = entityWorld->FindComponentByHandle(handle, componentType);
		if (component != nullptr) {
			return component;
		} else {
			if (logError) {
				OsiError("No " << ComponentTypeToName(componentType) << " component found with handle 0x" << std::hex << handle.Handle);
			}
			return nullptr;
		}
	}

	esv::Character * FindCharacterByNameGuid(char const * nameGuid, bool logError)
	{
		auto component = FindComponentByNameGuid(ComponentType::Character, nameGuid, logError);
		if (component != nullptr) {
			return (esv::Character *)((uint8_t *)component - 8);
		} else {
			return nullptr;
		}
	}

	esv::Character * FindCharacterByHandle(ObjectHandle const & handle, bool logError)
	{
		auto component = FindComponentByHandle(ComponentType::Character, handle, logError);
		if (component != nullptr) {
			return (esv::Character *)((uint8_t *)component - 8);
		} else {
			return nullptr;
		}
	}

	esv::Item * FindItemByNameGuid(char const * nameGuid, bool logError)
	{
		auto component = FindComponentByNameGuid(ComponentType::Item, nameGuid, logError);
		if (component != nullptr) {
			return (esv::Item *)((uint8_t *)component - 8);
		} else {
			return nullptr;
		}
	}

	esv::Item * FindItemByHandle(ObjectHandle const & handle, bool logError)
	{
		auto component = FindComponentByHandle(ComponentType::Item, handle, logError);
		if (component != nullptr) {
			return (esv::Item *)((uint8_t *)component - 8);
		} else {
			return nullptr;
		}
	}

	esv::GameAction * FindGameActionByHandle(ObjectHandle const & handle)
	{
		auto actionMgr = gStaticSymbols.GetGameActionManager();

		for (uint32_t i = 0; i < actionMgr->GameActions.Size; i++) {
			auto action = actionMgr->GameActions[i];
			if (action->MyHandle == handle) {
				return action;
			}
		}

		OsiError("Nm game action found with handle " << (int64_t)handle);
		return nullptr;
	}

	ShootProjectileApiHelper::ShootProjectileApiHelper()
	{
		Helper.Random = (uint8_t)rand();
		Helper.DamageList = nullptr;
		Helper.CasterLevel = -1;
		Helper.HitObject = nullptr;
		Helper.IsTrap = false;
		Helper.UnknownFlag1 = true;
		Helper.StatusClearChance = 0.0f;
		Helper.IsFromItem = false;
		Helper.IsStealthed = false;
		Helper.IgnoreObjects = false;
	}

	void ShootProjectileApiHelper::SetInt(char const * prop, int32_t value)
	{
		if (strcmp(prop, "CasterLevel") == 0) {
			Helper.CasterLevel = value;
		}
		else if (strcmp(prop, "StatusClearChance") == 0) {
			Helper.StatusClearChance = value / 100.0f;
		}
		else if (strcmp(prop, "IsTrap") == 0) {
			Helper.IsTrap = value ? 1 : 0;
		}
		else if (strcmp(prop, "UnknownFlag1") == 0) {
			Helper.UnknownFlag1 = value ? 1 : 0;
		}
		else if (strcmp(prop, "IsFromItem") == 0) {
			Helper.IsFromItem = value ? 1 : 0;
		}
		else if (strcmp(prop, "IsStealthed") == 0) {
			Helper.IsStealthed = value ? 1 : 0;
		}
		else if (strcmp(prop, "IgnoreObjects") == 0) {
			Helper.IgnoreObjects = value ? 1 : 0;
		}
		else {
			OsiError("Unknown int property: '" << prop << "'");
		}
	}

	void ShootProjectileApiHelper::SetGuidString(char const * prop, char const * value)
	{
		auto obj = FindGameObjectByNameGuid(value);

		if (obj == nullptr) {
			OsiError("GUID '" << value << "' is not a valid item or object");
			return;
		}

		ObjectHandle handle;
		obj->GetObjectHandle(&handle);
		glm::vec3 position = obj->WorldPos;

		if (strcmp(prop, "SourcePosition") == 0) {
			Helper.StartPosition = position;
			HasStartPosition = true;
		}
		else if (strcmp(prop, "TargetPosition") == 0) {
			Helper.EndPosition = position;
			HasEndPosition = true;
		}
		else if (strcmp(prop, "Caster") == 0) {
			Helper.Caster = handle;
		}
		else if (strcmp(prop, "Source") == 0) {
			Helper.Source = handle;
		}
		else if (strcmp(prop, "Target") == 0) {
			Helper.Target = handle;
		}
		else {
			OsiError("Unknown object property '" << prop << "'");
		}
	}

	void ShootProjectileApiHelper::SetVector(char const * prop, glm::vec3 const & value)
	{
		if (strcmp(prop, "SourcePosition") == 0) {
			Helper.StartPosition = value;
			HasStartPosition = true;
		}
		else if (strcmp(prop, "TargetPosition") == 0) {
			Helper.EndPosition = value;
			HasEndPosition = true;
		}
		else {
			OsiError("Unknown vector3 property '" << prop << "'");
		}
	}

	void ShootProjectileApiHelper::SetString(char const * prop, char const * value)
	{
		auto fs = ToFixedString(value);
		if (!fs) {
			OsiError("Could not map value '" << value << "' to FixedString");
			return;
		}

		if (strcmp(prop, "SkillId") == 0) {
			Helper.SkillId = fs;
		}
		else if (strcmp(prop, "CleanseStatuses") == 0) {
			Helper.CleanseStatuses = fs;
		}
		else {
			OsiError("Unknown string property '" << prop << "'");
		}
	}

	bool ShootProjectileApiHelper::Shoot()
	{
		if (!HasStartPosition) {
			OsiErrorS("No start position!");
			return false;
		}

		if (!HasEndPosition) {
			OsiErrorS("No end position!");
			return false;
		}

		if (!Helper.SkillId) {
			OsiErrorS("No skill id!");
			return false;
		}

		auto shoot = gStaticSymbols.ShootProjectile;
		if (shoot == nullptr) {
			OsiErrorS("ShootProjectile helper not found!");
			return false;
		}

		shoot(&Helper);
		return true;
	}
}
