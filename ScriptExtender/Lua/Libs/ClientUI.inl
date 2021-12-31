BEGIN_NS(ecl::lua::ui)

UIObject* FindUIObject(ig::FlashPlayer* player)
{
	auto uiManager = GetStaticSymbols().GetUIObjectManager();
	if (uiManager == nullptr) {
		OsiError("Couldn't get symbol for UIObjectManager!");
		return {};
	}

	for (auto const& ui : uiManager->UIObjects) {
		if (ui->FlashPlayer == player) {
			return ui;
		}
	}

	return {};
}


uint32_t NextCustomCreatorId = 1000;

UIObject* CreateUI(char const* name, char const* path, int layer, std::optional<uint32_t> inFlags)
{
	auto& sym = GetStaticSymbols();
	auto uiManager = sym.GetUIObjectManager();
	if (!uiManager || !sym.ecl__EoCUI__ctor || !sym.ecl__EoCUI__vftable) {
		OsiError("Some EoCUI symbol mappings are missing!");
		return nullptr;
	}

	UIObjectFlags flags;
	if (inFlags) {
		flags = (UIObjectFlags )*inFlags;
	} else {
		flags = UIObjectFlags::OF_Load | UIObjectFlags::OF_PlayerInput1 | UIObjectFlags::OF_DeleteOnChildDestroy;
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

	// FIXME - TEMP CAST
	auto object = (UIObject*)uiManager->Get(handle);
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

void RegisterUILib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"Create", LuaWrapFunction(&CreateUI)},
		{"GetByName", LuaWrapFunction(&GetByName)},
		{"GetByType", LuaWrapFunction(&GetByType)},
		{"GetByPath", LuaWrapFunction(&GetByPath)},
		{"Destroy", LuaWrapFunction(&Destroy)},
		{"SetDirty", LuaWrapFunction(&SetDirty)},
		{"EnableCustomDrawCallDebugging", LuaWrapFunction(&EnableCustomDrawCallDebugging)},
		{"HandleToDouble", LuaWrapFunction(&HandleToDouble)},
		{"DoubleToHandle", LuaWrapFunction(&DoubleToHandle)},
		{0,0}
	};

	RegisterLib(L, "UI", lib);
}

END_NS()
