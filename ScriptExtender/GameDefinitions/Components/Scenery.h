#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>

BEGIN_NS(ecl)

struct Scenery : public IEoCClientObject
{
	static constexpr auto ComponentPoolIndex = EntityComponentIndex::Scenery;
	static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientScenery;

	struct SoundSettings
	{
		GameObjectTemplate* Template;
		int16_t SoundAttenuation;
		FixedString SoundInitEvent;
		FixedString LoopSound;
	};

	struct FadeSettings
	{
		GameObjectTemplate* Template;
		bool FadeIn;
		float Opacity;
		FixedString FadeGroup;
	};

	BaseComponent Base;
	glm::vec3 Translation;
	SceneryFlags Flags;
	FixedString LevelName;
	glm::mat3 Rotation;
	float Scale;
	FixedString GUID;
	uint8_t CoverAmount;
	PhysicsObject* Physics;
	SoundSettings* SoundParams;
	FadeSettings* FadeParams;
	GameObjectTemplate* Template;
	FixedString VisualResourceID;
	FixedString DefaultState;
	uint8_t RenderChannel;
	bool IsBlocker;
	SceneryRenderFlags RenderFlags;

	SceneryFlags LuaGetFlags();
	void LuaSetFlags(SceneryFlags flags);
	bool LuaHasFlag(SceneryFlags flag);
	void LuaSetFlag(SceneryFlags flag, bool set);
};


END_NS()
