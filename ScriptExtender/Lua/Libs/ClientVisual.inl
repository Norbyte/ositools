#include <Lua/Shared/LuaMethodHelpers.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/GameObjects/Material.h>
#include <Extender/ScriptExtender.h>

BEGIN_SE()

void ResourceManager::DestroyVisual(ComponentHandle const& handle)
{
	if ((ObjectHandleType)handle.GetType() == ObjectHandleType::Visual) {
		auto del = GetStaticSymbols().ls__VisualFactory__DestroyVisual;
		del(VisualFactory, handle.Handle);
	} else if ((ObjectHandleType)handle.GetType() == ObjectHandleType::Effect) {
		auto mgr = GetStaticSymbols().ls__gEffectsManager;
		auto del = GetStaticSymbols().ls__EffectsManager__DestroyEffect;
		del(*mgr, handle);
	} else {
		OsiError("Don't know how to destroy visual of type " << GetHandleTypeName(handle));
	}
}

void Actor::AddMeshBinding(MeshBinding* binding)
{
	binding->RemapToSkeleton(Skeleton);
	MeshBindings.push_back(binding);
}


bool MaterialParameters::OverrideParameter(FixedString const& parameter, TextureMap* value)
{
	return TextureMaps.Override(parameter, value);
}

bool MaterialParameters::OverrideParameter(FixedString const& parameter, float value)
{
	return Scalars.Override(parameter, value);
}

bool MaterialParameters::OverrideParameter(FixedString const& parameter, glm::vec2 const& value)
{
	return Vector2s.Override(parameter, value);
}

bool MaterialParameters::OverrideParameter(FixedString const& parameter, MaterialVector3 const& value)
{
	return Vector3s.Override(parameter, value);
}

bool MaterialParameters::OverrideParameter(FixedString const& parameter, MaterialVector4 const& value)
{
	return Vector4s.Override(parameter, value);
}

bool MaterialParameters::OverrideParameter(FixedString const& parameter, MaterialTexture2D const& value)
{
	return Texture2Ds.Override(parameter, value);
}

bool MaterialParameters::OverrideParameter(FixedString const& parameter, MaterialSamplerState const& value)
{
	return SamplerStates.Override(parameter, value);
}


void Visual::OverrideVec3MaterialParameter(FixedString const& parameter, glm::vec3 const& vec, bool isColor)
{
	OverrideMaterialParameter(parameter, MaterialVector3{ vec, isColor });
}

void Visual::OverrideVec4MaterialParameter(FixedString const& parameter, glm::vec4 const& vec, bool isColor)
{
	OverrideMaterialParameter(parameter, MaterialVector4{ vec, isColor });
}

void Visual::OverrideTextureMaterialParameter(FixedString const& parameter, FixedString const& textureId)
{
	OverrideMaterialParameter(parameter, MaterialTexture2D{ textureId });
}

END_SE()

BEGIN_NS(ecl::lua::visual)

Visual* Get(ComponentHandle const& handle)
{
	if ((ObjectHandleType)handle.GetType() != ObjectHandleType::Visual) {
		OsiError("Expected a visual handle, got " << GetHandleTypeName(handle));
		return nullptr;
	}

	auto factory = GetStaticSymbols().GetResourceManager()->VisualFactory;
	return factory->Get(handle);
}

ClientMultiVisual* Create(lua_State* L, glm::vec3 const& position)
{
	auto init = GetStaticSymbols().ecl__MultiEffectHandler__Init;
	auto state = ClientState::FromLua(L);
	auto effect = state->GetVisualSystem().Create();
	init(effect, ":", position, nullptr, nullptr, nullptr, GFS.strEmpty);
	return effect;
}

ClientMultiVisual* CreateOnCharacter(lua_State* L, glm::vec3 const& position, std::optional<ProxyParam<Character>> target, std::optional<ProxyParam<Character>> listenForTextKeys)
{
	auto init = GetStaticSymbols().ecl__MultiEffectHandler__Init;
	auto state = ClientState::FromLua(L);
	auto effect = state->GetVisualSystem().Create();
	init(effect, ":", position, target ? target->Object : nullptr, nullptr, listenForTextKeys ? listenForTextKeys->Object : nullptr, GFS.strEmpty);
	return effect;
}

ClientMultiVisual* CreateOnItem(lua_State* L, glm::vec3 const& position, std::optional<ProxyParam<Item>> target, std::optional<ProxyParam<Item>> listenForTextKeys)
{
	auto init = GetStaticSymbols().ecl__MultiEffectHandler__Init;
	auto state = ClientState::FromLua(L);
	auto effect = state->GetVisualSystem().Create();
	init(effect, ":", position, target ? target->Object : nullptr, nullptr, listenForTextKeys ? listenForTextKeys->Object : nullptr, GFS.strEmpty);
	return effect;
}

void ClientMultiVisual::Delete(lua_State* L)
{
	auto state = ClientState::FromLua(L);

	for (auto const& handle : AttachedVisuals) {
		state->GetVisualSystem().RequestDeleteVisual(handle);
	}

	AttachedVisuals.clear();

	state->GetVisualSystem().Destroy(this);
}

void ClientMultiVisual::ParseFromStats(char const* effect, std::optional<char const*> weaponBones)
{
	auto init = GetStaticSymbols().ecl__MultiEffectHandler__Init;

	auto target = GetEntityWorld()->GetGameObject(TargetObjectHandle);
	auto listenForTextKeys = GetEntityWorld()->GetGameObject(ListenForTextKeysHandle);
	init(this, effect, Position, target, nullptr, listenForTextKeys, weaponBones ? FixedString(*weaponBones) : GFS.strEmpty);
}

VisualFlags ParseVisualFlags(lua_State* L, int index)
{
	VisualFlags visualFlags{ 0 };

	if (try_gettable<bool>(L, "CastShadow", index, true)) {
		visualFlags |= VisualFlags::CastShadow;
	}

	if (try_gettable<bool>(L, "ReceiveDecal", index, false)) {
		visualFlags |= VisualFlags::ReceiveDecal;
	}

	if (try_gettable<bool>(L, "Reflecting", index, false)) {
		visualFlags |= VisualFlags::Reflecting;
	}

	if (try_gettable<bool>(L, "IsShadowProxy", index, false)) {
		visualFlags |= VisualFlags::IsShadowProxy;
	}

	if (try_gettable<bool>(L, "AllowReceiveDecalWhenAnimated", index, false)) {
		visualFlags |= VisualFlags::AllowReceiveDecalWhenAnimated;
	}

	return visualFlags;
}

VisualAttachmentFlags ParseVisualAttachFlags(lua_State* L, int index)
{
	VisualAttachmentFlags flags{ VisualAttachmentFlags::DestroyWithParent };

	if (try_gettable<bool>(L, "ExcludeFromBounds", index, false)) {
		flags |= VisualAttachmentFlags::ExcludeFromBounds;
	}

	if (try_gettable<bool>(L, "KeepRot", index, false)) {
		flags |= VisualAttachmentFlags::KeepRot;
	}

	if (try_gettable<bool>(L, "KeepScale", index, false)) {
		flags |= VisualAttachmentFlags::KeepScale;
	}

	if (try_gettable<bool>(L, "UseLocalTransform", index, false)) {
		flags |= VisualAttachmentFlags::UseLocalTransform;
	}

	if (try_gettable<bool>(L, "InheritAnimations", index, false)) {
		flags |= VisualAttachmentFlags::InheritAnimations;
	}

	if (try_gettable<bool>(L, "ParticleSystem", index, false)) {
		flags |= VisualAttachmentFlags::ParticleSystem;
	}

	if (try_gettable<bool>(L, "DoNotUpdate", index, false)) {
		flags |= VisualAttachmentFlags::DoNotUpdate;
	}

	if (try_gettable<bool>(L, "Armor", index, false)) {
		flags |= VisualAttachmentFlags::Armor;
	}

	if (try_gettable<bool>(L, "Weapon", index, false)) {
		flags |= VisualAttachmentFlags::Weapon;
	}

	if (try_gettable<bool>(L, "Wings", index, false)) {
		flags |= VisualAttachmentFlags::Wings;
	}

	if (try_gettable<bool>(L, "Horns", index, false)) {
		flags |= VisualAttachmentFlags::Horns;
	}

	if (try_gettable<bool>(L, "Overhead", index, false)) {
		flags |= VisualAttachmentFlags::Overhead;
	}

	if (try_gettable<bool>(L, "WeaponFX", index, false)) {
		flags |= VisualAttachmentFlags::WeaponFX;
	}

	if (try_gettable<bool>(L, "BonusWeaponFX", index, false)) {
		flags |= VisualAttachmentFlags::BonusWeaponFX;
	}

	if (try_gettable<bool>(L, "WeaponOverlayFX", index, false)) {
		flags |= VisualAttachmentFlags::WeaponOverlayFX;
	}

	return flags;
}

Visual* ClientMultiVisual::AddVisual(lua_State* L, FixedString const& visualId)
{
	int nargs = lua_gettop(L);
	bool allowTPose{ false };
	bool resetScale{ false };
	bool syncAnimationWithParent{ false };
	FixedString boneName;
	int32_t boneIndex{ -1 };
	VisualAttachmentFlags attachFlags{ VisualAttachmentFlags::DestroyWithParent };
	VisualFlags visualFlags{ 0 };
	std::optional<glm::vec4> color1, color2, color3, color4, color5;

	if (nargs >= 2) {
		luaL_checktype(L, 3, LUA_TTABLE);
		allowTPose = try_gettable<bool>(L, "AllowTPose", 3, false);
		resetScale = try_gettable<bool>(L, "ResetScale", 3, false);
		syncAnimationWithParent = try_gettable<bool>(L, "SyncAnimationWithParent", 3, false);
		boneName = try_gettable<FixedString>(L, "Bone", 3, GFS.strEmpty);
		color1 = try_gettable<glm::vec4>(L, "Color1", 3);
		color2 = try_gettable<glm::vec4>(L, "Color2", 3);
		color3 = try_gettable<glm::vec4>(L, "Color3", 3);
		color4 = try_gettable<glm::vec4>(L, "Color4", 3);
		color5 = try_gettable<glm::vec4>(L, "Color5", 3);

		attachFlags = ParseVisualAttachFlags(L, 3);
		visualFlags = ParseVisualFlags(L, 3);
	}

	auto instantiate = GetStaticSymbols().ResourceManager__InstantiateVisual;
	auto addAttachment = GetStaticSymbols().ls__Visual__AddAttachment;

	auto target = GetEntityWorld()->GetGameObject(TargetObjectHandle);
	if (!target) {
		OsiErrorS("Cannot add visuals when no target character or item is specified.");
		return nullptr;
	}

	auto parentVisual = target->GetVisual();
	if (!parentVisual) {
		OsiErrorS("Target character/item has no visuals!");
		return nullptr;
	}

	auto visual = instantiate(GetStaticSymbols().GetResourceManager(), visualId, true, true, true);
	if (!visual) {
		OsiError("Failed to instantiate visual: " << visualId);
		return nullptr;
	}

	visual->VisualFlags = visualFlags;

	if (allowTPose) {
		// TODO!
	}

	if (resetScale) {
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		visual->SetWorldScale(scale);
	}

	if (boneName) {
		if (!parentVisual->Skeleton) {
			OsiError("Cannot attach to bone - visual has no skeleton!");
			return nullptr;
		}

		if (!parentVisual->Skeleton->GetBoneIndexFromName(boneName.GetString(), &boneIndex)) {
			OsiError("Visual has no such bone: " << boneName);
			return nullptr;
		}
	}

	addAttachment(parentVisual, visual, boneName, boneIndex, attachFlags);

	if (syncAnimationWithParent) {
		// TODO - SetAnimation, SynchronizeAnimationTo
	} else {
		if (parentVisual->Actor) {
			for (auto const& object : visual->SubObjects) {
				if ((object.Renderable->GetRTTI() & 7) == 7) {
					auto animatable = static_cast<AnimatableObject*>(object.Renderable);
					parentVisual->Actor->AddMeshBinding(animatable->MeshBinding);
				}
			}
		}
	}

	if (color1) {
		visual->OverrideMaterialParameter(GFS.strColor1, MaterialVector4{ *color1, true });
	}

	if (color2) {
		visual->OverrideMaterialParameter(GFS.strColor2, MaterialVector4{ *color2, true });
	}

	if (color3) {
		visual->OverrideMaterialParameter(GFS.strColor3, MaterialVector4{ *color3, true });
	}

	if (color4) {
		visual->OverrideMaterialParameter(GFS.strColor4, MaterialVector4{ *color4, true });
	}

	if (color5) {
		visual->OverrideMaterialParameter(GFS.strColor5, MaterialVector4{ *color5, true });
	}

	// TODO - Cloth logic
	// TODO - SetHairRenderPasses
	
	if (parentVisual->Actor) {
		parentVisual->Actor->SetTextKeyEventHandler(target);
	}

	AttachedVisuals.push_back(visual->Handle);
	return visual;
}

void RegisterVisualLib()
{
	DECLARE_MODULE(Visual, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(Get)
	MODULE_FUNCTION(Create)
	MODULE_FUNCTION(CreateOnCharacter)
	MODULE_FUNCTION(CreateOnItem)
	END_MODULE()
}

END_NS()
