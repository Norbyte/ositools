BEGIN_NS(ecl::lua::ui)

uint32_t NextCustomCreatorId = 1000;

UIObject* Create(char const* name, char const* path, int layer, std::optional<UIObjectFlags> inFlags)
{
	auto& sym = GetStaticSymbols();
	auto uiManager = sym.GetUIObjectManager();
	if (!uiManager || !sym.ecl__EoCUI__ctor || !sym.ecl__EoCUI__vftable) {
		OsiError("Some EoCUI symbol mappings are missing!");
		return nullptr;
	}

	UIObjectFlags flags;
	if (inFlags) {
		flags = (UIObjectFlags)*inFlags;
	} else {
		flags = UIObjectFlags::OF_Load | UIObjectFlags::OF_PlayerInput1 | UIObjectFlags::OF_DeleteOnChildDestroy;
	}

	const UIObjectFlags requiredFlags = UIObjectFlags::OF_Load | UIObjectFlags::OF_DeleteOnChildDestroy;
	const UIObjectFlags disallowedFlags = UIObjectFlags::OF_Loaded | UIObjectFlags::OF_RequestDelete | UIObjectFlags::OF_Activated;
	if (((flags & requiredFlags) != requiredFlags) || (flags & disallowedFlags) != (UIObjectFlags)0) {
		OsiError("Attempted to create UI object with invalid flags set!");
		return nullptr;
	}

	// FIXME - playerId, registerInvokeNames?

	LuaClientPin pin(ExtensionState::Get());
	auto existingUi = pin->GetUIObject(name);
	if (existingUi != nullptr) {
		OsiError("An UI object with name '" << name << "' already exists!");
		return nullptr;
	}

	auto absPath = sym.ToPath(path, PathRootType::Data);

	std::optional<uint32_t> creatorId;
	for (auto const& creator : uiManager->UIObjectCreators) {
		if (creator.Value->Path.Name == absPath.c_str()) {
			creatorId = creator.Key;
			break;
		}
	}

	if (!creatorId) {
		auto creator = GameAlloc<UIObjectFunctor>();
		creator->Path.Name = absPath;
		creator->CreateProc = CustomUI::Creator;

		sym.RegisterUIObjectCreator(uiManager, NextCustomCreatorId, creator);
		creatorId = NextCustomCreatorId++;
	}

	ComponentHandle handle;
	sym.UIObjectManager__CreateUIObject(uiManager, &handle, layer, *creatorId, flags, 0x80, 0);

	if (!handle) {
		OsiError("Failed to create UI object");
		return nullptr;
	}

	auto object = uiManager->Get(handle);
	if (!object) {
		OsiError("Failed to look up constructed UI object");
		return nullptr;
	}

	if (!object->FlashPlayer) {
		OsiError("Flash player initialization failed");
		return nullptr;
	}

	pin->OnCustomClientUIObjectCreated(name, handle);
	return object;
}

UIObject* GetByName(char const* name)
{
	LuaClientPin pin(ExtensionState::Get());
	return pin->GetUIObject(name);
}

UIObject* GetByType(int typeId)
{
	// UIDummyOverhead is not an EoCUI object
	// TODO - better fix for this (split UIObject)!
	if (typeId == 0xf) return nullptr;

	auto uiManager = GetStaticSymbols().GetUIObjectManager();
	if (uiManager != nullptr) {
		return uiManager->GetByType(typeId);
	} else {
		return nullptr;
	}
}

UIObject* GetByPath(char const* path)
{
	auto absPath = GetStaticSymbols().ToPath(path, PathRootType::Data);
	auto uiManager = GetStaticSymbols().GetUIObjectManager();

	if (uiManager) {
		for (auto ui : uiManager->Components) {
			if (ui && ui->FlashPlayer && absPath == ui->Path.Name.c_str()) {
				return ui;
			}
		}
	}

	return nullptr;
}

void Destroy(char const* name)
{
	LuaClientPin pin(ExtensionState::Get());
	auto ui = pin->GetUIObject(name);
	if (ui != nullptr) {
		ui->RequestDelete();
	} else {
		OsiError("No UI object exists with name '" << name << "'!");
	}
}

void SetDirty(ComponentHandle const& handle, uint64_t flags)
{
	auto ui = GetStaticSymbols().GetUIObjectManager();
	if (ui && ui->CharacterDirtyFlags) {
		EnterCriticalSection(&ui->CriticalSection);
		auto curFlags = ui->CharacterDirtyFlags->Find(handle);
		if (curFlags != nullptr) {
			*curFlags |= flags;
		} else {
			*ui->CharacterDirtyFlags->Insert(handle) = flags;
		}
		LeaveCriticalSection(&ui->CriticalSection);
	}
}

void EnableCustomDrawCallDebugging(bool enabled)
{
	ecl::LuaClientPin lua(ecl::ExtensionState::Get());
	if (lua) {
		lua->GetCustomDrawHelper().DebugDrawCalls = enabled;
	}
}

// int64 handle to double conversion hack for use in Flash external interface calls
// (Some of the builtin functions treat handles as double values)
double HandleToDouble(lua_State* L, ComponentHandle handle)
{
	return *reinterpret_cast<double*>(&handle);
}

ComponentHandle DoubleToHandle(lua_State* L, double dbl)
{
	return ComponentHandle(*reinterpret_cast<int64_t*>(&dbl));
}

ecl::character_creation::UICharacterCreationWizard* GetCharacterCreationWizard()
{
	auto ui = GetByType(3);
	if (!ui) {
		ui = GetByType(4);
	}

	if (ui) {
		return static_cast<ecl::character_creation::UICharacterCreationWizard*>(ui);
	} else {
		return nullptr;
	}
}

UserReturn GetPickingState(lua_State* L, std::optional<int> playerIndex)
{
	StackCheck _(L, 1);
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (level == nullptr || level->PickingHelperManager == nullptr) {
		push(L, nullptr);
		return 1;
	}

	auto helper = level->PickingHelperManager->PlayerHelpers.Find(playerIndex.value_or(1));
	if (helper == nullptr) {
		push(L, nullptr);
		return 1;
	}

	auto const& base = (*helper)->b;
	lua_newtable(L);
	if ((*helper)->GameObjectPick) {
		setfield(L, "WorldPosition", (*helper)->GameObjectPick->WorldPos.Position);
	}

	setfield(L, "WalkablePosition", base.WalkablePickPos.Position);

	if (base.HoverCharacterHandle2) {
		setfield(L, "HoverCharacter", base.HoverCharacterHandle2);
		setfield(L, "HoverCharacterPosition", base.HoverCharacterPickPos.Position);
	}

	if (base.HoverCharacterHandle) {
		setfield(L, "HoverCharacter2", base.HoverCharacterHandle);
	}

	if (base.HoverItemHandle) {
		setfield(L, "HoverItem", base.HoverItemHandle);
		setfield(L, "HoverItemPosition", base.HoverItemPickPos.Position);
	}

	if (base.HoverCharacterOrItemHandle) {
		setfield(L, "HoverEntity", base.HoverCharacterOrItemHandle);
	}

	if (base.PlaceablePickHandle) {
		setfield(L, "PlaceableEntity", base.PlaceablePickHandle);
		setfield(L, "PlaceablePosition", base.PlaceablePickInfo.Position);
	}

	return 1;
}

dse::DragDropManager* GetDragDrop()
{
	return *GetStaticSymbols().ls__DragDropManager;
}

bool LoadFlashLibrary(STDString const& moduleName, STDString const& path)
{
	auto binding = GetStaticSymbols().GetResourceManager()->FlashBinding;
	Path libPath = GetStaticSymbols().ToPath(path, PathRootType::Data);
	Path dataPath;
	auto libraryId = binding->LoadLibraryFromFile(moduleName.c_str(), libPath, dataPath);
	return libraryId >= 0;
}

glm::ivec2 GetViewportSize()
{
	auto binding = GetStaticSymbols().GetResourceManager()->FlashBinding;
	return glm::ivec2(binding->RenderFrameViewport.x, binding->RenderFrameViewport.y);
}

void RegisterUILib()
{
	DECLARE_MODULE(UI, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(Create)
	MODULE_FUNCTION(GetByName)
	MODULE_FUNCTION(GetByType)
	MODULE_FUNCTION(GetByPath)
	MODULE_FUNCTION(Destroy)
	MODULE_FUNCTION(SetDirty)
	MODULE_FUNCTION(EnableCustomDrawCallDebugging)
	MODULE_FUNCTION(HandleToDouble)
	MODULE_FUNCTION(DoubleToHandle)
	MODULE_FUNCTION(GetCharacterCreationWizard)
	MODULE_FUNCTION(GetPickingState)
	MODULE_FUNCTION(GetDragDrop)
	MODULE_FUNCTION(LoadFlashLibrary)
	MODULE_FUNCTION(GetViewportSize)
	END_MODULE()
}

END_NS()
