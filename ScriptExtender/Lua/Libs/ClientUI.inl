/// <lua_module>UI</lua_module>
BEGIN_NS(ecl::lua::ui)

uint32_t NextCustomCreatorId = 1000;

/// <summary>
/// Creates a new UI element. Returns the UI object on success and `nil` on failure.
/// </summary>
/// <param name="name">A user-defined unique name that identifies the UI element. To avoid name collisions, the name should always be prefixed with the mod name (e.g. `NRD_CraftingUI`)</param>
/// <param name="path">Path of the SWF file relative to the data directory (e.g. `"Public/ModName/GUI/CraftingUI.swf"`)</param>
/// <param name="layer">Stack order of the UI element. Overlapping elements with a larger layer value cover those with a smaller one.</param>
/// <param name="flags">UI object flags; default: `Load|PlayerInput1|DeleteOnChildDestroy`</param>
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

/// <summary>
/// Retrieves a UI element with the specified name. If no such element exists, the function returns `nil`.
/// </summary>
/// <param name="name">Name passed to `Ext.UI.Create` when creating the UI element</param>
UIObject* GetByName(char const* name)
{
	LuaClientPin pin(ExtensionState::Get());
	return pin->GetUIObject(name);
}

/// <summary>
/// Retrieves an engine UI element. If no such element exists, the function returns `nil`.
/// </summary>
/// <param name="typeId">Engine UI ID</param>
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

/// <summary>
/// Retrieves a built-in UI element at the specified path. If no such element exists, the function returns `nil`.
/// </summary>
/// <param name="path">SWF path relative to data directory (e.g. `"Public/ModName/GUI/CraftingUI.swf"`)</param>
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

/// <summary>
/// Destroys the specified UI element.
/// </summary>
/// <param name="name">Name passed to `Ext.UI.Create` when creating the UI element</param>
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

/// <summary>
/// **Experimental!** Forces an UI refresh for the specified character.
/// Supported flag values:
/// - 0x1 - AP
/// - 0x10 - Abilities
/// - 0x60 - Status icons
/// - 0x40000 - Health
/// - 0x80000 - Skill set
/// - 0x1000000 - Inventory
/// - 0x10000000 - Character transform
/// - 0x80000000 - Relations
/// </summary>
/// <param name="handle">UI object handle</param>
/// <param name="flags">Dirty flags</param>
void SetDirty(ComponentHandle const& handle, uint64_t flags)
{
	auto ui = GetStaticSymbols().GetUIObjectManager();
	if (ui && ui->CharacterDirtyFlags) {
		EnterCriticalSection(&ui->CriticalSection);
		auto curFlags = ui->CharacterDirtyFlags->find(handle);
		if (curFlags) {
			curFlags.Value() |= flags;
		} else {
			*ui->CharacterDirtyFlags->insert(handle, flags);
		}
		LeaveCriticalSection(&ui->CriticalSection);
	}
}

/// <summary>
/// Toggles printing of Flash elements where the custom draw callback is being called.
/// (i.e. icons where the image is supplied by engine code)
/// </summary>
/// <param name="enabled"></param>
void EnableCustomDrawCallDebugging(bool enabled)
{
	ecl::LuaClientPin lua(ecl::ExtensionState::Get());
	if (lua) {
		lua->GetCustomDrawHelper().DebugDrawCalls = enabled;
	}
}

/// <summary>
/// int64 handle to double conversion hack for use in Flash external interface calls
/// (Some of the builtin functions treat handles as double values)
/// </summary>
/// <param name="handle">Handle to convert</param>
double HandleToDouble(ComponentHandle handle)
{
	return *reinterpret_cast<double*>(&handle);
}

/// <summary>
/// double to int64 handle conversion hack for use in Flash external interface calls
/// (Some of the builtin functions treat handles as double values)
/// </summary>
/// <param name="dbl">Flash double value to convert</param>
ComponentHandle DoubleToHandle(double dbl)
{
	return ComponentHandle(*reinterpret_cast<int64_t*>(&dbl));
}

/// <summary>
/// Returns the character creation UI.
/// (The object returned by this call can be used to access additional character creation-specific fields that are not available via `GetByPath()` etc.)
/// </summary>
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

	auto helper = level->PickingHelperManager->PlayerHelpers.TryGet(playerIndex.value_or(1));
	if (helper == nullptr) {
		push(L, nullptr);
		return 1;
	}

	auto const& base = helper->b;
	lua_newtable(L);
	if (helper->GameObjectPick) {
		setfield(L, "WorldPosition", helper->GameObjectPick->WorldPos.Position);
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

/// <summary>
/// Loads a Flash library; other SWF files can import symbols from this library.
/// *Note:* The game can load at most 7 additional libraries, so only use this feature when necessary!
/// </summary>
/// <param name="moduleName">Library name</param>
/// <param name="path">SWF path relative to data directory (e.g. `"Public/ModName/GUI/SomeLibrary.swf"`)</param>
bool LoadFlashLibrary(STDString const& moduleName, STDString const& path)
{
	auto binding = GetStaticSymbols().GetResourceManager()->FlashBinding;
	Path libPath = GetStaticSymbols().ToPath(path, PathRootType::Data);
	Path dataPath;
	auto libraryId = binding->LoadLibraryFromFile(moduleName.c_str(), libPath, dataPath);
	return libraryId >= 0;
}

/// <summary>
/// Returns the size of the viewport (game window)
/// </summary>
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
