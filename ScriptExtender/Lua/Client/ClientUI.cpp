#include <stdafx.h>
#include <Lua/Client/ClientUI.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Client/LuaBindingClient.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/Client/ExtensionStateClient.h>
#include <GameDefinitions/Symbols.h>
#include <GameHooks/SymbolMapper.h>
#include <Extender/ScriptExtender.h>
#include <Extender/Client/ExtensionStateClient.h>

BEGIN_NS(lua)

using namespace dse::ecl::lua;
	
UIObjectProxyRefImpl::UIObjectProxyRefImpl(LifetimeHandle const& containerLifetime, UIObject* obj, LifetimeHandle const& lifetime)
	: handle_(obj->UIObjectHandle), containerLifetime_(containerLifetime), lifetime_(lifetime)
{}
		
UIObjectProxyRefImpl::~UIObjectProxyRefImpl()
{}

UIObject* UIObjectProxyRefImpl::Get(lua_State* L) const
{
	auto self = GetStaticSymbols().GetUIObjectManager()->Get(handle_);
	if (!lifetime_.IsAlive(L)) {
		WarnDeprecated56("An access was made to a UIObject instance after its lifetime has expired; this behavior is deprecated.");
	}

	return self;
}

void* UIObjectProxyRefImpl::GetRaw(lua_State* L)
{
	return Get(L);
}

FixedString const& UIObjectProxyRefImpl::GetTypeName() const
{
	return StaticLuaPropertyMap<UIObject>::PropertyMap.Name;
}

bool UIObjectProxyRefImpl::GetProperty(lua_State* L, FixedString const& prop)
{
	auto ui = Get(L);
	if (!ui) return false;
	return ObjectProxyHelpers<UIObject>::GetProperty(L, ui, containerLifetime_, prop);
}

bool UIObjectProxyRefImpl::SetProperty(lua_State* L, FixedString const& prop, int index)
{
	auto ui = Get(L);
	if (!ui) return false;
	return ObjectProxyHelpers<UIObject>::SetProperty(L, ui, containerLifetime_, prop, index);
}

int UIObjectProxyRefImpl::Next(lua_State* L, FixedString const& key)
{
	auto ui = Get(L);
	if (!ui) return 0;
	return ObjectProxyHelpers<UIObject>::Next(L, ui, containerLifetime_, key);
}

bool UIObjectProxyRefImpl::IsA(FixedString const& typeName)
{
	return ObjectProxyHelpers<UIObject>::IsA(typeName);
}

void MakeUIObjectRef(lua_State* L, LifetimeHandle const& lifetime, UIObject* value)
{
	if (value) {
		ObjectProxy2::MakeImpl<UIObjectProxyRefImpl, UIObject>(L, value, State::FromLua(L)->GetGlobalLifetime(), lifetime);
	} else {
		push(L, nullptr);
	}
}

void LuaToInvokeDataValue(lua_State * L, int index, ig::InvokeDataValue & val)
{
	switch (lua_type(L, index)) {
	case LUA_TNUMBER:
		val.TypeId = ig::DataType::Double;
		val.DoubleVal = lua_tonumber(L, index);
		break;

	case LUA_TBOOLEAN:
		val.TypeId = ig::DataType::Bool;
		val.BoolVal = lua_toboolean(L, index);
		break;

	case LUA_TSTRING:
		val.TypeId = ig::DataType::String;
		val.StringVal = lua_tostring(L, index);
		break;

	default:
		luaL_error(L, "Cannot pass value of type %s to Flash", lua_typename(L, lua_type(L, index)));
	}
}


void InvokeDataValueToLua(lua_State * L, ig::InvokeDataValue const & val)
{
	switch (val.TypeId) {
	case ig::DataType::None:
		lua_pushnil(L);
		break;

	case ig::DataType::Bool:
		push(L, val.BoolVal);
		break;

	case ig::DataType::Double:
		push(L, val.DoubleVal);
		break;

	case ig::DataType::String:
		push(L, val.StringVal);
		break;

	case ig::DataType::WString:
		push(L, val.WStringVal);
		break;

	default:
		OsiError("Flash value of type " << (unsigned)val.TypeId << " cannot be passed to Lua");
		lua_pushnil(L);
		break;
	}
}

void push(lua_State* L, ig::InvokeDataValue const& v)
{
	InvokeDataValueToLua(L, v);
}

ig::InvokeDataValue do_get(lua_State * L, int i, Overload<ig::InvokeDataValue>)
{
	ig::InvokeDataValue v;
	LuaToInvokeDataValue(L, i, v);
	return v;
}

END_NS()

BEGIN_NS(ecl::lua)

using namespace dse::lua;

CustomUI::CustomUI(dse::Path * path)
	: EoCUI(path)
{}

void CustomUI::OnFunctionCalled(const char * func, unsigned int numArgs, ig::InvokeDataValue * args)
{
	UICallEvent call { 
		.UI = this, 
		.Function = func, 
		.Args = {args, args + numArgs}, 
		.When = nullptr
	};
	bool prevented{ false };

	{
		LuaClientPin lua(ExtensionState::Get());
		if (lua) {
			prevented = (lua->OnUICall(call) == EventResult::ActionPrevented);
		}
	}

	if (!prevented) {
		EoCUI::OnFunctionCalled(func, numArgs, args);

		{
			LuaClientPin lua(ExtensionState::Get());
			if (lua) {
				lua->OnAfterUICall(call);
			}
		}
	}
}

void CustomUI::CustomDrawCallback(void* callback)
{
	auto cb = reinterpret_cast<FlashCustomDrawCallback*>(callback);
	ecl::LuaClientPin lua(gExtender->GetClient().GetExtensionState());
	if (lua && lua->GetCustomDrawHelper().DrawIcon(this, cb)) {
		return;
	}

	EoCUI::CustomDrawCallback(callback);
}

void CustomUI::Destroy(bool free)
{
	EoCUI::Destroy(false);
	if (free) {
		GameFree(this);
	}
}

const char * CustomUI::GetDebugName()
{
	return "extender::CustomUI";
}

UIObject * CustomUI::Creator(dse::Path * path)
{
	return GameAlloc<CustomUI>(path);
}



UIFlashPath::UIFlashPath() {}

UIFlashPath::UIFlashPath(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path)
	: paths_(parents)
{
	paths_.push_back(*path);

	for (auto i = 1; i < paths_.size(); i++) {
		paths_[i].Parent = &paths_[i - 1];
	}
}

ig::IggyValuePath* UIFlashPath::Last()
{
	return &*paths_.rbegin();
}

int PushFlashRef(lua_State* L, Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
bool SetFlashValue(lua_State* L, ig::IggyValuePath* path, int idx);


char const* const UIFlashObject::MetatableName = "FlashObject";

UIFlashObject::UIFlashObject(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path)
	: path_(parents, path)
{}

int UIFlashObject::Index(lua_State* L)
{
	StackCheck _(L, 1);
	ig::IggyValuePath path;
	auto name = get<char const*>(L, 2);
	if (GetStaticSymbols().IgValuePathMakeNameRef(&path, path_.Last(), name)) {
		return PushFlashRef(L, path_.paths_, &path);
	} else {
		push(L, nullptr);
		return 1;
	}
}

int UIFlashObject::NewIndex(lua_State* L)
{
	StackCheck _(L, 0);
	ig::IggyValuePath path;
	auto name = get<char const*>(L, 2);
	if (GetStaticSymbols().IgValuePathMakeNameRef(&path, path_.Last(), name)) {
		SetFlashValue(L, &path, 3);
	}

	return 0;
}



char const* const UIFlashArray::MetatableName = "FlashArray";

UIFlashArray::UIFlashArray(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path)
	: path_(parents, path)
{}

int UIFlashArray::Index(lua_State * L)
{
	StackCheck _(L, 1);
	ig::IggyValuePath path;
	auto index = get<int>(L, 2);
	if (GetStaticSymbols().IgValuePathPathMakeArrayRef(&path, path_.Last(), index, path_.Last()->Iggy)) {
		return PushFlashRef(L, path_.paths_, &path);
	} else {
		push(L, nullptr);
		return 1;
	}
}

int UIFlashArray::NewIndex(lua_State* L)
{
	StackCheck _(L, 0);
	ig::IggyValuePath path;
	auto index = get<int>(L, 2);
	if (GetStaticSymbols().IgValuePathPathMakeArrayRef(&path, path_.Last(), index, path_.Last()->Iggy)) {
		SetFlashValue(L, &path, 3);
	}

	return 0;
}

int UIFlashArray::Length(lua_State* L)
{
	StackCheck _(L, 1);
	uint32_t length;
	if (GetStaticSymbols().IgValueGetArrayLength(path_.Last(), nullptr, nullptr, &length) == 0) {
		push(L, length);
		return 1;
	} else {
		push(L, nullptr);
		return 1;
	}
}



char const* const UIFlashFunction::MetatableName = "FlashFunction";

UIFlashFunction::UIFlashFunction(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path)
	: path_(parents, path)
{}

bool LuaToFlashValue(lua_State * L, ig::IggyDataValue& value, int idx)
{
	switch (lua_type(L, idx)) {
	case LUA_TNIL:
		value.TypeId = ig::DataType::None;
		break;

	case LUA_TBOOLEAN:
		value.TypeId = ig::DataType::Bool;
		value.Int64 = get<bool>(L, idx) ? 1 : 0;
		break;

	case LUA_TNUMBER:
		value.TypeId = ig::DataType::Double;
		value.Double = get<double>(L, idx);
		break;

	case LUA_TSTRING:
	{
		auto str = const_cast<char*>(get<char const*>(L, idx));
		value.TypeId = ig::DataType::String;
		value.String = str;
		value.StringLength = (int)strlen(str);
		break;
	}

	default:
		OsiError("Can't convert Lua type '" << lua_typename(L, lua_type(L, idx)) << "' to Flash parameter!");
		return false;
	}

	return true;
}

int PushFlashValue(lua_State* L, ig::IggyDataValue const& value)
{
	switch (value.TypeId) {
	case ig::DataType::None:
	case ig::DataType::Null:
		lua_pushnil(L);
		return 1;

	case ig::DataType::Bool:
		push(L, value.Int32 != 0);
		return 1;

	case ig::DataType::Double:
		push(L, value.Double);
		return 1;

	case ig::DataType::String:
		push(L, value.String);
		return 1;

	case ig::DataType::WString:
		push(L, ToUTF8(value.WString));
		return 1;

	default:
		OsiError("Don't know how to push Flash type " << (unsigned)value.TypeId << "!");
		lua_pushnil(L);
		return 1;
	}
}

int UIFlashFunction::LuaCall(lua_State* L)
{
	StackCheck _(L, 1);
	int numArgs = lua_gettop(L) - 1;

	auto object = &path_.paths_[path_.paths_.size() - 2];
	auto method = path_.Last()->Name;

	Vector<ig::IggyDataValue> args;
	args.resize(numArgs);

	for (auto i = 0; i < numArgs; i++) {
		LuaToFlashValue(L, args[i], i + 2);
	}

	ig::IggyDataValue result;
	if (GetStaticSymbols().IgPlayerCallMethod(object->Iggy, &result, object, method, numArgs, args.data()) != 0) {
		push(L, nullptr);
		return 1;
	}

	return PushFlashValue(L, result);
}

bool SetFlashValue(lua_State* L, ig::IggyValuePath* path, int idx)
{
	auto const& s = GetStaticSymbols();

	switch (lua_type(L, idx)) {
	case LUA_TBOOLEAN:
	{
		auto val = get<bool>(L, idx);
		return s.IgValueSetBoolean(path, nullptr, nullptr, val ? 1 : 0) == 0;
	}

	case LUA_TNUMBER:
	{
		auto val = get<double>(L, idx);
		return s.IgValueSetF64(path, nullptr, nullptr, val) == 0;
	}

	case LUA_TSTRING:
	{
		auto val = get<char const*>(L, idx);
		return s.IgValueSetStringUTF8(path, nullptr, nullptr, val, (int)strlen(val)) == 0;
	}

	case LUA_TUSERDATA:
	{
		auto arr = UIFlashArray::AsUserData(L, idx);
		if (arr != nullptr) {
			OsiError("Assigning Flash arrays to values not supported yet!");
			return false;
		}

		auto obj = UIFlashObject::AsUserData(L, idx);
		if (obj != nullptr) {
			OsiError("Assigning Flash objects to values not supported yet!");
			return false;
		}

		auto fun = UIFlashFunction::AsUserData(L, idx);
		if (fun != nullptr) {
			OsiError("Assigning Flash functions to values not supported yet!");
			return false;
		}

		OsiError("Only Flash array/object/function userdata types can be assigned to Flash values!");
		return false;
	}

	default:
		OsiError("Can't convert Lua type '" << lua_typename(L, lua_type(L, idx))  << "' to Flash!");
		return false;
	}
}

int PushFlashRef(lua_State* L, Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path)
{
	auto const& s = GetStaticSymbols();

	ig::DataType type;
	if (s.IgValueGetType(path, nullptr, nullptr, &type) != 0) {
		push(L, nullptr);
		return 1;
	}

	switch (type) {
	case ig::DataType::None:
	case ig::DataType::Null:
		lua_pushnil(L);
		return 1;

	case ig::DataType::Bool:
	{
		uint32_t val;
		if (s.IgValueGetBoolean(path, nullptr, nullptr, &val) == 0) {
			push(L, val != 0);
			return 1;
		}
		break;
	}

	case ig::DataType::Double:
	{
		double val;
		if (s.IgValueGetF64(path, nullptr, nullptr, &val) == 0) {
			push(L, val);
			return 1;
		}
		break;
	}

	case ig::DataType::String:
	case ig::DataType::WString:
	{
		int resultLength{ 0 };
		if (s.IgValueGetStringUTF8(path, nullptr, nullptr, 0x10000, nullptr, &resultLength) == 0) {
			STDString str;
			str.resize(resultLength);
			if (s.IgValueGetStringUTF8(path, nullptr, nullptr, resultLength, str.data(), &resultLength) == 0) {
				push(L, str);
				return 1;
			}
		}
		break;
	}

	case ig::DataType::Array:
		UIFlashArray::New(L, parents, path);
		return 1;

	case ig::DataType::Object:
	case ig::DataType::Object2:
		UIFlashObject::New(L, parents, path);
		return 1;

	case ig::DataType::Function:
		UIFlashFunction::New(L, parents, path);
		return 1;

	default:
		OsiError("Don't know how to handle Flash type " << (unsigned)type << "!");
		break;
	}

	push(L, nullptr);
	return 1;
}

// Persistent for the lifetime of the app, as we don't restore FlashPlayer VMTs either
FlashPlayerHooks gFlashPlayerHooks;

END_NS()

BEGIN_SE()

using namespace dse::lua;

std::unordered_map<UIObject::VMT*, UIObject::CustomDrawCallbackProc> CustomDrawHelper::originalDrawHandlers_;

bool CustomDrawHelper::DrawIcon(UIObject* self, ecl::FlashCustomDrawCallback* callback)
{
	auto customIcons = icons_.find(self->UIObjectHandle);
	if (customIcons != icons_.end()) {
		auto icon = customIcons->second.find(callback->Name);
		if (icon != customIcons->second.end() && icon->second->IconMesh != nullptr) {
			auto draw = GetStaticSymbols().ls__UIHelper__CustomDrawObject;
			draw(callback, icon->second->IconMesh);

			if (DebugDrawCalls) {
				INFO("Custom draw callback handled: %s -> %s", ToUTF8(callback->Name).c_str(), icon->second->IconName.Str);
			}

			return true;
		}
	}

	return false;
}

void CustomDrawHelper::DrawCallback(UIObject* self, void* callback)
{
	auto cb = reinterpret_cast<ecl::FlashCustomDrawCallback*>(callback);
	if (DrawIcon(self, cb)) {
		return;
	}

	auto vmt = *reinterpret_cast<UIObject::VMT**>(self);
	auto handler = originalDrawHandlers_.find(vmt);
	if (handler != originalDrawHandlers_.end()) {
		if (DebugDrawCalls) {
			INFO(L"Custom draw callback unhandled: %s", cb->Name);
		}
		handler->second(self, callback);
	}
	else {
		OsiError("Couldn't find original CustomDrawCallback handler for UI object");
	}
}

void CustomDrawHelper::sDrawCallback(UIObject* self, void* callback)
{
	ecl::LuaClientPin lua(gExtender->GetClient().GetExtensionState());
	if (lua) {
		lua->GetCustomDrawHelper().DrawCallback(self, callback);
	}
}

void CustomDrawHelper::EnableCustomDraw(UIObject* ui)
{
	auto vmt = *reinterpret_cast<UIObject::VMT**>(ui);
	if (vmt->CustomDrawCallback == &sDrawCallback) return;

	// Custom UI element draw calls are already handled, no need to hook them
	if (strcmp(ui->GetDebugName(), "extender::CustomUI") == 0) return;

	WriteAnchor _w((uint8_t*)vmt, sizeof(*vmt));
	originalDrawHandlers_.insert(std::make_pair(vmt, vmt->CustomDrawCallback));
	vmt->CustomDrawCallback = &sDrawCallback;
}

bool CustomDrawHelper::SetCustomIcon(UIObject* ui, STDWString const& element, STDString const& icon, int width, int height, std::optional<STDString> materialGuid)
{
	if (width < 1 || height < 1 || width > 1024 || height > 1024) {
		OsiError("Invalid icon size");
		return false;
	}

	auto const& sym = GetStaticSymbols();
	auto vmt = sym.ls__CustomDrawStruct__VMT;
	auto clear = sym.ls__UIHelper__UIClearIcon;
	auto create = sym.ls__UIHelper__UICreateIconMesh;
	auto draw = sym.ls__UIHelper__CustomDrawObject;

	if (!vmt || !clear || !create || !draw) {
		OsiError("Not all UIHelper symbols are available");
		return false;
	}

	auto customIcons = icons_.find(ui->UIObjectHandle);
	if (customIcons == icons_.end()) {
		icons_.insert(std::make_pair(ui->UIObjectHandle, std::unordered_map<STDWString, std::unique_ptr<CustomDrawStruct>>()));
	}

	customIcons = icons_.find(ui->UIObjectHandle);
	auto curIcon = customIcons->second.find(element);
	if (curIcon != customIcons->second.end()) {
		clear(curIcon->second.get());
		customIcons->second.erase(curIcon);
	}

	auto newIcon = std::make_unique<CustomDrawStruct>();
	newIcon->VMT = vmt;
	create(FixedString(icon), newIcon.get(), width, height, FixedString(materialGuid ? *materialGuid : "9169b076-6e8d-44a4-bb52-95eedf9eab63"));

	if (newIcon->IconMesh) {
		customIcons->second.insert(std::make_pair(element, std::move(newIcon)));
		EnableCustomDraw(ui);
		return true;
	} else {
		OsiError("Failed to load icon: " << icon);
		return false;
	}
}

void CustomDrawHelper::ClearCustomIcon(UIObject* ui, STDWString const& element)
{
	auto customIcons = icons_.find(ui->UIObjectHandle);
	if (customIcons != icons_.end()) {
		auto curIcon = customIcons->second.find(element);
		if (curIcon != customIcons->second.end()) {
			auto clear = GetStaticSymbols().ls__UIHelper__UIClearIcon;
			clear(curIcon->second.get());
			customIcons->second.erase(curIcon);
		}
	}
}


void UIObject::LuaSetPosition(int x, int y)
{
	glm::ivec2 pos(x, y);
	SetPos(pos);
}

std::optional<glm::ivec2> UIObject::LuaGetPosition()
{
	if (FlashPlayer) {
		return FlashPlayer->GetPosition();
	} else {
		return {};
	}
}

/// <summary>
/// Displays the UI element.
/// </summary>
void UIObject::LuaShow()
{
	Show();
}

/// <summary>
/// Hides the UI element.
/// </summary>
void UIObject::LuaHide()
{
	Hide();
}

/// <summary>
/// Calls a method on the main timeline object of the UI element. 
/// The first argument (`func`) is the name of the ActionScript function to call; all subsequent arguments are passed to the ActionScript function as parameters.
/// Only `string`, `number` and `boolean` arguments are supported.
/// 
/// Example:
/// ```lua
/// local ui = Ext.UI.GetByPath(...)
/// ui:Invoke("exposedMethod", "test")
/// ```
/// 
/// ActionScript :
/// ```actionscript
/// function exposedMethod(val: String) : * {
///     this.testLabel.text = val;
/// }
/// </summary>
bool UIObject::LuaInvoke(lua_State * L, STDString const& method)
{
	if (!FlashPlayer) {
		return false;
	}

	auto root = FlashPlayer->GetRootObject();
	if (!root) {
		return false;
	}

	WarnDeprecated56("UIObject::Invoke() is deprecated; use GetRoot() to access the Flash function directly instead!");

	auto & invokes = FlashPlayer->Invokes;
	std::optional<uint32_t> invokeId;
	for (uint32_t i = 0; i < invokes.size(); i++) {
		if (method == invokes[i].Name) {
			invokeId = i;
			break;
		}
	}

	if (!invokeId) {
		invokeId = FlashPlayer->Invokes.size();
		// FlashPlayer keeps the pointer we pass to it, so we need to make sure that the
		// string buffer won't get reused later on
		auto invokeName = _strdup(method.c_str());
		FlashPlayer->AddInvokeName(*invokeId, invokeName);
	}

	auto numArgs = lua_gettop(L) - 2;
	Vector<ig::InvokeDataValue> args; 
	args.resize(numArgs);
	for (auto i = 0; i < numArgs; i++) {
		LuaToInvokeDataValue(L, i + 3, args[i]);
	}

	return FlashPlayer->InvokeArgs(*invokeId, args.data(), numArgs);
}


void UIObject::LuaGotoFrame(int frame, std::optional<bool> force)
{
	if (!FlashPlayer) return;

	if (force && *force) {
		FlashPlayer->ForceGotoFrame(frame);
	} else {
		FlashPlayer->GotoFrame(frame);
	}

	return;
}


void UIObject::SetValue(STDString const& path, ig::InvokeDataValue const& value, std::optional<int> arrayIndex)
{
	if (!FlashPlayer) return;
	auto root = FlashPlayer->GetRootObject();
	if (!root) return;

	WarnDeprecated56("UIObject::SetValue() is deprecated; use GetRoot() to access the field directly instead!");

	root->SetValue(path.c_str(), value, arrayIndex ? *arrayIndex : -1);
}

std::optional<ig::InvokeDataValue> UIObject::GetValue(lua_State* L, STDString const& path, std::optional<STDString> typeName, std::optional<int> arrayIndex)
{
	if (!FlashPlayer) return {};
	auto root = FlashPlayer->GetRootObject();
	if (!root) return {};

	WarnDeprecated56("UIObject::GetValue() is deprecated; use GetRoot() to access the field directly instead!");

	ig::InvokeDataValue value;
	ig::DataType type{ ig::DataType::None };
	if (typeName) {
		if (typeName == "number") {
			type = ig::DataType::Double;
		} else if (typeName == "boolean") {
			type = ig::DataType::Bool;
		} else if (typeName == "string") {
			type = ig::DataType::String;
		} else {
			luaL_error(L, "Unknown value type for Flash fetch: %s", typeName->c_str());
		}
	}

	if (root->GetValueWorkaround(path.c_str(), type, value, arrayIndex ? *arrayIndex : -1)) {
		return value;
	} else {
		return {};
	}
}


ComponentHandle UIObject::LuaGetHandle()
{
	return UIObjectHandle;
}


/// <summary>
/// Attempts to determine the handle of the player that this UI element is assigned to. 
/// If the element is not assigned to a player, the function returns `nil`.
/// Only certain elements have a player assigned, like character sheet, inventory; others don't have player handles at all.
/// </summary>
std::optional<ComponentHandle> UIObject::LuaGetPlayerHandle()
{
	ComponentHandle handle;
	if (Type == 104) {
		// ecl::UIExamine (104) doesn't implement GetPlayerHandle(), but we need it
		auto examine = reinterpret_cast<ecl::UIExamine*>(this);
		handle = examine->ObjectBeingExamined;
	} else {
		GetPlayerHandle(&handle);
	}

	if (handle) {
		return handle;
	} else {
		return {};
	}
}

/// <summary>
/// Returns the engine type ID of the UI. 
/// This ID can be used to retrieve the element using `Ext.UI.GetByType(...)`; it is also used to register ID-based listeners
/// with the `Ext.UI.RegisterTypeInvokeListener` and `Ext.UI.RegisterTypeCall` functions.
/// </summary>
int UIObject::GetTypeId()
{
	return Type;
}

/// <summary>
/// Returns the Flash main timeline object. 
/// This allows navigation of the Flash object hierarchy from Lua and provides full access to all propertiesand sub - properties in Flash.
/// This is the recommended way to interact with Flash objects within the UI; the old `GetValue` and `SetValue` methods are deprecated.
/// 
/// Examples:
/// ```lua
/// local root = Ext.UI.GetByPath("Public/Game/GUI/mainMenu.swf"):GetRoot()
/// -- Object properties on the main timeline and all child objects are readable
/// Ext.Print(root.mainMenu_mc.debugText_txt.htmlText)
/// -- ... and writeable
/// root.mainMenu_mc.debugText_txt.htmlText = "TEST TEST TEST"
/// 
/// -- Fetching array length supported via the # operator
/// Ext.Print(#root.events)
/// -- Indexed access to arrays supported
/// Ext.Print(root.events[2])
/// -- Array write support
/// root.events[2] = "TEST"
/// 
/// -- Method call support
/// root.mainMenu_mc.addMenuLabel("TEST LABEL")
/// Ext.Print(root.getHeight())
/// ```
/// </summary>
UserReturn UIObject::LuaGetRoot(lua_State* L)
{
	StackCheck _(L, 1);
	if (!FlashPlayer || !FlashPlayer->IggyPlayerRootPath) {
		push(L, nullptr);
		return 1;
	}

	Vector<ig::IggyValuePath> path;
	return PushFlashRef(L, path, FlashPlayer->IggyPlayerRootPath);
}


void UIObject::LuaDestroy()
{
	RequestDelete();
}

/// <summary>
/// Simulates an `ExternalInterface.call(...)` call from Flash, i.e. it calls an UI handler function in the game engine. 
/// The first argument (`func`) is the name of the UI function to call; all subsequent arguments are passed to the engine as parameters.
/// Only `string`, `number` and `boolean` arguments are supported.
/// 
/// Example:
/// ```lua
/// local ui = Ext.UI.GetByPath("Public/Game/GUI/characterSheet.swf")
/// ui:ExternalInterfaceCall("show")
/// ```
/// </summary>
/// <param name="L"></param>
/// <param name="method"></param>
void UIObject::LuaExternalInterfaceCall(lua_State * L, STDString const& method)
{
	auto numArgs = lua_gettop(L) - 2;
	Vector<ig::InvokeDataValue> args;
	args.resize(numArgs);
	for (auto i = 0; i < numArgs; i++) {
		LuaToInvokeDataValue(L, i + 3, args[i]);
	}

	OnFunctionCalled(method.c_str(), numArgs, args.data());
}


// This needs to be persistent for the lifetime of the app, as we don't restore altered VMTs
std::unordered_map<UIObject::VMT *, UIObject::OnFunctionCalledProc> OriginalUIObjectCallHandlers;

void UIObjectFunctionCallCapture(UIObject* self, const char* function, unsigned int numArgs, ig::InvokeDataValue* args)
{
	UICallEvent call { 
		.UI = self, 
		.Function = function, 
		.Args = {args, args + numArgs}, 
		.When = nullptr
	};
	bool prevented{ false };

	{
		ecl::LuaClientPin lua(ecl::ExtensionState::Get());
		if (lua) {
			prevented = (lua->OnUICall(call) == EventResult::ActionPrevented);
		}
	}

	if (!prevented) {
		auto vmt = *reinterpret_cast<UIObject::VMT**>(self);
		auto handler = OriginalUIObjectCallHandlers.find(vmt);
		if (handler != OriginalUIObjectCallHandlers.end()) {
			handler->second(self, call.Function.c_str(), (uint32_t)call.Args.size(), call.Args.data());
		} else {
			OsiError("Couldn't find original OnFunctionCalled handler for UI object");
		}

		{
			ecl::LuaClientPin lua(ecl::ExtensionState::Get());
			if (lua) {
				lua->OnAfterUICall(call);
			}
		}
	}
}

void UIObject::CaptureExternalInterfaceCalls()
{
	// Custom UI element calls are captured by default, no need to hook them
	if (strcmp(GetDebugName(), "extender::CustomUI") == 0) return;

	auto vmt = *reinterpret_cast<UIObject::VMT**>(this);
	if (vmt->OnFunctionCalled == &UIObjectFunctionCallCapture) return;

	WriteAnchor _w((uint8_t*)vmt, sizeof(*vmt));
	OriginalUIObjectCallHandlers.insert(std::make_pair(vmt, vmt->OnFunctionCalled));
	vmt->OnFunctionCalled = &UIObjectFunctionCallCapture;
}


void UIObject::EnableCustomDraw()
{
	ecl::LuaClientPin lua(gExtender->GetClient().GetExtensionState());
	if (lua) {
		lua->GetCustomDrawHelper().EnableCustomDraw(this);
	}
}

void UIObject::SetCustomIcon(STDWString const& element, STDString const& icon, int width, int height, std::optional<STDString> materialGuid)
{
	ecl::LuaClientPin lua(gExtender->GetClient().GetExtensionState());
	if (lua) {
		lua->GetCustomDrawHelper().SetCustomIcon(this, element, icon, width, height, materialGuid);
	}
}

void UIObject::ClearCustomIcon(STDWString const& element)
{
	ecl::LuaClientPin lua(gExtender->GetClient().GetExtensionState());
	if (lua) {
		lua->GetCustomDrawHelper().ClearCustomIcon(this, element);
	}
}


float UIObject::GetUIScaleMultiplier()
{
	return (GetStaticSymbols().ls__UIObject__GetUIScaleMultiplier)(this);
}

void UIObject::SetMovieClipSize(float width, float height, std::optional<float> scale)
{
	if (!FlashPlayer) return;

	auto uiScale = scale ? *scale : GetUIScaleMultiplier();
	this->MovieClipSize = glm::vec2(width, height);
	this->FlashSize = glm::vec2(width * uiScale, height * uiScale);
	FlashPlayer->SetStageSize((unsigned)width, (unsigned)height);
	auto pos = FlashPlayer->GetPosition();
	glm::ivec2 renderSize{ (int)this->FlashSize[0], (int)this->FlashSize[1] };
	FlashPlayer->SetRenderRectangle(pos, renderSize, 1);
}


void UIObject::CaptureInvokes()
{
	// Custom UI element calls are captured by default, no need to hook them
	if (strcmp(GetDebugName(), "extender::CustomUI") == 0) return;

	if (FlashPlayer == nullptr) {
		OsiWarn("Cannot capture UI invokes - UI element '" << Path.Name << "' has no flash player!");
		return;
	}

	auto vmt = *reinterpret_cast<ig::FlashPlayer::VMT**>(FlashPlayer);
	gFlashPlayerHooks.Hook(vmt);
}


END_SE()

BEGIN_NS(ecl::lua)

UIObject* FindUIObject(ig::FlashPlayer* player)
{
	auto uiManager = GetStaticSymbols().GetUIObjectManager();
	if (uiManager == nullptr) {
		OsiError("Couldn't get symbol for UIObjectManager!");
		return {};
	}

	for (auto const& ui : uiManager->Components) {
		if (ui->FlashPlayer == player) {
			return ui;
		}
	}

	return {};
}

template <class ...Args>
bool OnFlashPlayerPreInvoke(ig::FlashPlayer* self, int64_t& invokeId, Args... args)
{
	LuaClientPin lua(ExtensionState::Get());
	auto ui = FindUIObject(self);
	if (lua && ui) {
		Vector<ig::InvokeDataValue> invokeArgs{ (*args)... };
		auto invokeName = self->Invokes[(uint32_t)invokeId].Name;
		UICallEvent call{ 
			.UI = ui, 
			.Function = invokeName, 
			.Args = {invokeArgs.data(), invokeArgs.data() + invokeArgs.size()}, 
			.When = nullptr
		};
		bool prevent = (lua->OnUIInvoke(call) == EventResult::ActionPrevented);
		if (!prevent && invokeName != call.Function) {
			bool foundId{ false };
			for (uint32_t i = 0; i < self->Invokes.size(); i++) {
				if (call.Function == self->Invokes[i].Name) {
					invokeId = i;
					foundId = false;
					break;
				}
			}

			if (!foundId) {
				OsiError("Can't redirect UI invoke: No invoke entry for function '" << call.Function << "'");
			}
		}

		return prevent;
	} else {
		return false;
	}
}

template <class ...Args>
void OnFlashPlayerPostInvoke(ig::FlashPlayer* self, int64_t invokeId, Args... args)
{
	LuaClientPin lua(ExtensionState::Get());
	auto ui = FindUIObject(self);
	if (lua && ui) {
		Vector<ig::InvokeDataValue> invokeArgs{ (*args)... };
		auto invokeName = self->Invokes[(uint32_t)invokeId].Name;
		UICallEvent call { 
			.UI = ui, 
			.Function = invokeName, 
			.Args = {invokeArgs.data(), invokeArgs.data() + invokeArgs.size()}, 
			.When = nullptr
		};
		lua->OnAfterUIInvoke(call);
	}
}

static bool FlashPlayerInvoke6Capture(ig::FlashPlayer* self, int64_t invokeId,
	ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3, ig::InvokeDataValue* a4, ig::InvokeDataValue* a5, ig::InvokeDataValue* a6)
{
	bool prevent = OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3, a4, a5, a6);
	if (!prevent) {
		auto result = gFlashPlayerHooks.OriginalInvoke6(self, invokeId, a1, a2, a3, a4, a5, a6);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3, a4, a5, a6);
		return result;
	} else {
		return true;
	}
}

static bool FlashPlayerInvoke5Capture(ig::FlashPlayer* self, int64_t invokeId,
	ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3, ig::InvokeDataValue* a4, ig::InvokeDataValue* a5)
{
	bool prevent = OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3, a4, a5);
	if (!prevent) {
		auto result = gFlashPlayerHooks.OriginalInvoke5(self, invokeId, a1, a2, a3, a4, a5);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3, a4, a5);
		return result;
	} else {
		return true;
	}
}

static bool FlashPlayerInvoke4Capture(ig::FlashPlayer* self, int64_t invokeId,
	ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3, ig::InvokeDataValue* a4)
{
	bool prevent = OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3, a4);
	if (!prevent) {
		auto result = gFlashPlayerHooks.OriginalInvoke4(self, invokeId, a1, a2, a3, a4);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3, a4);
		return result;
	} else {
		return true;
	}
}

static bool FlashPlayerInvoke3Capture(ig::FlashPlayer* self, int64_t invokeId,
	ig::InvokeDataValue* a1, ig::InvokeDataValue* a2, ig::InvokeDataValue* a3)
{
	bool prevent = OnFlashPlayerPreInvoke(self, invokeId, a1, a2, a3);
	if (!prevent) {
		auto result = gFlashPlayerHooks.OriginalInvoke3(self, invokeId, a1, a2, a3);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2, a3);
		return result;
	} else {
		return true;
	}
}

static bool FlashPlayerInvoke2Capture(ig::FlashPlayer* self, int64_t invokeId, ig::InvokeDataValue* a1, ig::InvokeDataValue* a2)
{
	bool prevent = OnFlashPlayerPreInvoke(self, invokeId, a1, a2);
	if (!prevent) {
		auto result = gFlashPlayerHooks.OriginalInvoke2(self, invokeId, a1, a2);
		OnFlashPlayerPostInvoke(self, invokeId, a1, a2);
		return result;
	} else {
		return true;
	}
}

static bool FlashPlayerInvoke1Capture(ig::FlashPlayer* self, int64_t invokeId, ig::InvokeDataValue* a1)
{
	bool prevent = OnFlashPlayerPreInvoke(self, invokeId, a1);
	if (!prevent) {
		auto result = gFlashPlayerHooks.OriginalInvoke1(self, invokeId, a1);
		OnFlashPlayerPostInvoke(self, invokeId, a1);
		return result;
	} else {
		return true;
	}
}

static bool FlashPlayerInvoke0Capture(ig::FlashPlayer* self, int64_t invokeId)
{
	bool prevent = OnFlashPlayerPreInvoke(self, invokeId);
	if (!prevent) {
		auto result = gFlashPlayerHooks.OriginalInvoke0(self, invokeId);
		OnFlashPlayerPostInvoke(self, invokeId);
		return result;
	} else {
		return true;
	}
}

static bool FlashPlayerInvokeArgsCapture(ig::FlashPlayer* self, int64_t invokeId, ig::InvokeDataValue* args, unsigned numArgs)
{
	auto ui = FindUIObject(self);
	auto invokeName = self->Invokes[(uint32_t)invokeId].Name;

	UICallEvent call { 
		.UI = ui, 
		.Function = invokeName, 
		.Args = {args, args + numArgs}, 
		.When = nullptr
	};
	bool prevented{ false };

	{
		LuaClientPin lua(ExtensionState::Get());
		if (lua && ui) {
			prevented = (lua->OnUIInvoke(call) == EventResult::ActionPrevented);
			if (call.Function != invokeName) {
				bool foundId{ false };
				for (uint32_t i = 0; i < self->Invokes.size(); i++) {
					if (call.Function == self->Invokes[i].Name) {
						invokeId = i;
						foundId = false;
						break;
					}
				}

				if (!foundId) {
					OsiError("Can't redirect UI invoke: No invoke entry for function '" << call.Function << "'");
				}
			}
		}
	}

	if (!prevented) {
		auto result = gFlashPlayerHooks.OriginalInvokeArgs(self, invokeId, call.Args.data(), (uint32_t)call.Args.size());

		{
			LuaClientPin lua(ExtensionState::Get());
			if (lua && ui) {
				lua->OnAfterUIInvoke(call);
			}
		}

		return result;
	} else {
		return true;
	}
}

void FlashPlayerHooks::Hook(ig::FlashPlayer::VMT* vmt)
{
	if (Hooked) return;

	WriteAnchor _((uint8_t*)vmt, sizeof(*vmt));
	VMT = vmt;
	OriginalInvoke6 = vmt->Invoke6;
	OriginalInvoke5 = vmt->Invoke5;
	OriginalInvoke4 = vmt->Invoke4;
	OriginalInvoke3 = vmt->Invoke3;
	OriginalInvoke2 = vmt->Invoke2;
	OriginalInvoke1 = vmt->Invoke1;
	OriginalInvoke0 = vmt->Invoke0;
	OriginalInvokeArgs = vmt->InvokeArgs;

	vmt->Invoke6 = &FlashPlayerInvoke6Capture;
	vmt->Invoke5 = &FlashPlayerInvoke5Capture;
	vmt->Invoke4 = &FlashPlayerInvoke4Capture;
	vmt->Invoke3 = &FlashPlayerInvoke3Capture;
	vmt->Invoke2 = &FlashPlayerInvoke2Capture;
	vmt->Invoke1 = &FlashPlayerInvoke1Capture;
	vmt->Invoke0 = &FlashPlayerInvoke0Capture;
	vmt->InvokeArgs = &FlashPlayerInvokeArgsCapture;

	Hooked = true;
}

END_NS()
