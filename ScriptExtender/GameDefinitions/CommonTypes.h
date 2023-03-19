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