#include <stdafx.h>
#include "FunctionLibrary.h"
#include <OsirisProxy.h>

namespace osidbg
{
	FixedString ToFixedString(const char * s)
	{
		auto stringTable = gOsirisProxy->GetLibraryManager().GetGlobalStringTable();
		if (stringTable == nullptr) {
			OsiError("ToFixedString(): Global string table not available!");
			return FixedString(nullptr);
		}

		auto str = stringTable->Find(s, strlen(s));
		return FixedString(str);
	}

	char const * NameGuidToFixedString(std::string const & nameGuid)
	{
		if (nameGuid.size() < 36) {
			OsiError("NameGuidToFixedString(): GUID (" << nameGuid << ") too short!");
			return nullptr;
		}

		auto guid = nameGuid.substr(nameGuid.size() - 36, 36);

		auto stringTable = gOsirisProxy->GetLibraryManager().GetGlobalStringTable();
		if (stringTable == nullptr) {
			OsiError("NameGuidToFixedString(): Global string table not available!");
			return nullptr;
		}

		return stringTable->Find(guid.c_str(), guid.size());
	}

	esv::EoCServerObject * FindGameObjectByNameGuid(std::string const & nameGuid, bool logError)
	{
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
			OsiError("No EoC server object found with this handle (0x" << std::hex << handle.Handle << ")");
		}

		return nullptr;
	}

	EntityWorld * GetEntityWorld()
	{
		auto charFactory = gOsirisProxy->GetLibraryManager().GetCharacterFactory();
		if (charFactory != nullptr) {
			return charFactory->Entities;
		}

		auto itemFactory = gOsirisProxy->GetLibraryManager().GetItemFactory();
		if (itemFactory != nullptr) {
			return itemFactory->Entities;
		}

		OsiError("EntityWorld not available!");
		return nullptr;
	}

	void * FindComponentByNameGuid(ComponentType componentType, std::string const & nameGuid, bool logError)
	{
		auto stringPtr = NameGuidToFixedString(nameGuid);
		if (stringPtr == nullptr) {
			OsiError("Could not map GUID '" << nameGuid << "' to FixedString");
			return nullptr;
		}

		auto entityWorld = GetEntityWorld();
		if (entityWorld == nullptr) {
			return nullptr;
		}

		FixedString fs;
		fs.Str = stringPtr;

		auto component = entityWorld->Components.Buf[(uint32_t)componentType].component->FindComponentByGuid(&fs);
		if (component != nullptr) {
			return component;
		} else {
			if (logError) {
				OsiError("No " << (uint32_t)componentType << " component found with GUID '" << nameGuid << "'");
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

		auto component = entityWorld->Components.Buf[(uint32_t)componentType].component->FindComponentByHandle(&handle);
		if (component != nullptr) {
			return component;
		} else {
			if (logError) {
				OsiError("No Character component found with this handle (0x" << std::hex << handle.Handle << ")");
			}
			return nullptr;
		}
	}

	esv::Character * FindCharacterByNameGuid(std::string const & nameGuid, bool logError)
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

	esv::Item * FindItemByNameGuid(std::string const & nameGuid, bool logError)
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
		auto const & lib = gOsirisProxy->GetLibraryManager();
		auto actionMgr = lib.GetGameActionManager();

		for (uint32_t i = 0; i < actionMgr->GameActions.Size; i++) {
			auto action = actionMgr->GameActions.Buf[i];
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
		Helper.CollisionInfo = nullptr;
		Helper.IsTrap = false;
		Helper.UnknownFlag1 = true;
		Helper.StatusClearChance = 0.0f;
		Helper.HasCaster = false;
		Helper.IsStealthed = false;
		Helper.UnknownFlag2 = false;
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
		else if (strcmp(prop, "HasCaster") == 0) {
			Helper.HasCaster = value ? 1 : 0;
		}
		else if (strcmp(prop, "IsStealthed") == 0) {
			Helper.IsStealthed = value ? 1 : 0;
		}
		else if (strcmp(prop, "UnknownFlag2") == 0) {
			Helper.UnknownFlag2 = value ? 1 : 0;
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
		else if (strcmp(prop, "Source") == 0) {
			if (obj != nullptr) {
				Helper.SourceCharacter = handle;
			}
		}
		else if (strcmp(prop, "Target") == 0) {
			Helper.Target = handle;
		}
		else if (strcmp(prop, "Target2") == 0) {
			Helper.Target2 = handle;
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
		else if (strcmp(prop, "FS2") == 0) {
			Helper.FS2 = fs;
		}
		else {
			OsiError("Unknown string property '" << prop << "'");
		}
	}

	bool ShootProjectileApiHelper::Shoot()
	{
		if (!HasStartPosition) {
			OsiError("No start position!");
			return false;
		}

		if (!HasEndPosition) {
			OsiError("No end position!");
			return false;
		}

		if (!Helper.SkillId.Str) {
			OsiError("No skill id!");
			return false;
		}

		auto shoot = gOsirisProxy->GetLibraryManager().ShootProjectile;
		if (shoot == nullptr) {
			OsiError("ShootProjectile helper not found!");
			return false;
		}

		shoot(&Helper, Helper.StatusClearChance);
		return true;
	}
}
