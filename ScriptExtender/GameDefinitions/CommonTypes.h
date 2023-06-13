#pragma once

#include <GameDefinitions/Base/Base.h>

BEGIN_SE()

struct SurfaceCell
{
    glm::i16vec2 Position;
};

struct ProjectileResult
{
	int HitInterpolation{ 0 };
	ComponentHandle Target;
	glm::vec3 Position;
};

struct WeaponAnimData
{
	float FirstTextEventTime;
	float TextEventTime;
	float TimeDelta;
};

struct ArmorDivider
{
	float DamagePerHit;
	int field_4;
};

struct NumberDivider
{
	stats::DamageType Type;
	float Amount;
	float Accumulator;
};

struct NumberDividers
{
	ArmorDivider ArmorAbsorptionDivider;
	ObjectSet<NumberDivider> DamageDividers;
};

struct Random
{
	int* List;
	uint32_t ListOffset;
	uint32_t ListCapacity;
	uint32_t Seed1;
	uint32_t Seed2;
	bool Initialized;
	bool RegenerateAtEnd; 
};

END_NS()

BEGIN_NS(eoc)

struct TextBuffer
{
	wchar_t* Buf;
	uint64_t Capacity;
	uint64_t Length;
	uint64_t Unknown;

	void Replace(WStringView replacement);
};

struct Text
{
	struct Param
	{
		int32_t PlaceholderOffset;
		int32_t Unknown[2];
		int32_t PlaceholderSize;
	};

	void* VMT;
	TextBuffer* Buf;
	int32_t Precision;
	int32_t _Pad;
	Param Params[8];

	void ReplaceParam(int index, WStringView replacement);
};

END_NS()