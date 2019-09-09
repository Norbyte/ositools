#pragma once

#include "../CustomFunctions.h"

namespace osidbg
{
	FixedString ToFixedString(const char * s);
	char const * NameGuidToFixedString(std::string const & nameGuid);
	EsvCharacter * FindCharacterByNameGuid(std::string const & nameGuid);
	EsvCharacter * FindCharacterByHandle(ObjectHandle const & handle);
	EsvItem * FindItemByNameGuid(std::string const & nameGuid);
	EsvItem * FindItemByHandle(ObjectHandle const & handle);


	struct ShootProjectileApiHelper
	{
		ShootProjectileHelper Helper;
		bool HasStartPosition{ false };
		bool HasEndPosition{ false };

		ShootProjectileApiHelper();
		void SetInt(char const * prop, int32_t value);
		void SetGuidString(char const * prop, char const * value);
		void SetVector(char const * prop, float const * value);
		void SetString(char const * prop, char const * value);
		bool Shoot();
	};

	class CustomFunctionLibrary
	{
	public:
		CustomFunctionLibrary(class OsirisProxy & osiris);

		void Register();
		void RegisterHelperFunctions();
		void RegisterMathFunctions();
		void RegisterStatFunctions();
		void RegisterStatusFunctions();
		void RegisterGameActionFunctions();
		void RegisterProjectileFunctions();

	private:
		OsirisProxy & osiris_;
	};

}