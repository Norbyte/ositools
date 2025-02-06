#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/Proxies/LuaObjectProxy.h>
#include <GameDefinitions/UI.h>

BEGIN_SE()

void UIObjectFunctionCallCapture(UIObject* self, const char* function, unsigned int numArgs, ig::InvokeDataValue* args);

END_SE()

BEGIN_NS(lua)

class UIObjectProxyRefImpl : public ObjectProxyImplBase
{
public:
	UIObjectProxyRefImpl(LifetimeHandle const& containerLifetime, UIObject* obj, LifetimeHandle const& lifetime);
	~UIObjectProxyRefImpl() override;

	UIObject* Get(lua_State* L) const;
	void* GetRaw(lua_State* L) override;
	FixedString const& GetTypeName() const override;
	bool GetProperty(lua_State* L, FixedString const& prop) override;
	bool SetProperty(lua_State* L, FixedString const& prop, int index) override;
	int Next(lua_State* L, FixedString const& key) override;
	bool IsA(FixedString const& typeName) override;

private:
	ComponentHandle handle_;
	LifetimeHandle containerLifetime_;
	LifetimeHandle lifetime_;
};

void MakeUIObjectRef(lua_State* L, LifetimeHandle const& lifetime, UIObject* value);

void LuaToInvokeDataValue(lua_State* L, int index, ig::InvokeDataValue& val);
void InvokeDataValueToLua(lua_State* L, ig::InvokeDataValue const& val);

void push(lua_State* L, ig::InvokeDataValue const& v);
ig::InvokeDataValue do_get(lua_State* L, int i, Overload<ig::InvokeDataValue>);

END_NS()

BEGIN_NS(ecl::lua)

using namespace dse::lua;

struct UIFlashPath
{
public:
	Vector<ig::IggyValuePath> paths_;

	UIFlashPath();
	UIFlashPath(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
	ig::IggyValuePath* Last();
};

class UIFlashObject : public Userdata<UIFlashObject>, public Indexable, public NewIndexable
{
public:
	static char const* const MetatableName;

	UIFlashObject(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
	int Index(lua_State* L);
	int NewIndex(lua_State* L);

private:
	UIFlashPath path_;
};


class UIFlashArray : public Userdata<UIFlashArray>, public Indexable, public NewIndexable, public Lengthable
{
public:
	static char const* const MetatableName;

	UIFlashArray(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
	int Index(lua_State* L);
	int NewIndex(lua_State* L);
	int Length(lua_State* L);

private:
	UIFlashPath path_;
};


class UIFlashFunction : public Userdata<UIFlashFunction>, public Callable
{
public:
	static char const* const MetatableName;

	UIFlashFunction(Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
	int LuaCall(lua_State* L);

private:
	UIFlashPath path_;
};

struct CustomUI : public ecl::EoCUI
{
	CustomUI(dse::Path* path);

	void OnFunctionCalled(const char* func, unsigned int numArgs, ig::InvokeDataValue* args) override;
	void CustomDrawCallback(void* callback) override;
	void Destroy(bool free) override;
	const char* GetDebugName() override;
	static UIObject* Creator(dse::Path* path);
};

int PushFlashRef(lua_State* L, Vector<ig::IggyValuePath> const& parents, ig::IggyValuePath* path);
bool SetFlashValue(lua_State* L, ig::IggyValuePath* path, int idx);

struct FlashPlayerHooks
{
	bool Hooked{ false };
	ig::FlashPlayer::VMT* VMT{ nullptr };
	ig::FlashPlayer::VMT::Invoke6Proc OriginalInvoke6{ nullptr };
	ig::FlashPlayer::VMT::Invoke5Proc OriginalInvoke5{ nullptr };
	ig::FlashPlayer::VMT::Invoke4Proc OriginalInvoke4{ nullptr };
	ig::FlashPlayer::VMT::Invoke3Proc OriginalInvoke3{ nullptr };
	ig::FlashPlayer::VMT::Invoke2Proc OriginalInvoke2{ nullptr };
	ig::FlashPlayer::VMT::Invoke1Proc OriginalInvoke1{ nullptr };
	ig::FlashPlayer::VMT::Invoke0Proc OriginalInvoke0{ nullptr };
	ig::FlashPlayer::VMT::InvokeArgsProc OriginalInvokeArgs{ nullptr };

	void Hook(ig::FlashPlayer::VMT* vmt);
};

// Persistent for the lifetime of the app, as we don't restore FlashPlayer VMTs either
extern FlashPlayerHooks gFlashPlayerHooks;

class CustomDrawHelper
{
public:
	bool DebugDrawCalls{ false };

	bool DrawIcon(UIObject* self, ecl::FlashCustomDrawCallback* callback);
	void DrawCallback(UIObject* self, void* callback);
	static void sDrawCallback(UIObject* self, void* callback);
	void EnableCustomDraw(UIObject* ui);
	bool SetCustomIcon(UIObject* ui, STDWString const& element, STDString const& icon, int width, int height, std::optional<STDString> materialGuid);
	bool SetCustomPortraitIcon(UIObject* ui, STDWString const& element, ComponentHandle characterHandle, int width, int height, std::optional<STDString> materialGuid, std::optional<bool> zeroSaturation);
	void ClearCustomIcon(UIObject* ui, STDWString const& element);

private:
	// Needs to be static, as VMT overwrites are not restored when the client state is deleted
	static std::unordered_map<UIObject::VMT*, UIObject::CustomDrawCallbackProc> originalDrawHandlers_;
	std::unordered_map<ComponentHandle, std::unordered_map<STDWString, std::unique_ptr<CustomDrawStruct>>> icons_;
};

END_NS()
