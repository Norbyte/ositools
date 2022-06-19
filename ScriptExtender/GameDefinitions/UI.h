#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>

namespace dse
{
	namespace ig
	{
		enum class DataType : int32_t
		{
			None = 0x1,
			Null = 0x2,
			Bool = 0x3,
			Double = 0x4,
			String = 0x5,
			WString = 0x6,
			Array = 0xA,
			Object = 0xB,
			Object2 = 0xC,
			Function = 0x10,
			UserData = 0x11,
			UserData2 = 0x12,
		};

		struct InvokeDataValue
		{
			DataType TypeId;
			STDString StringVal;
			STDWString WStringVal;
			bool BoolVal;
			void* PointerVal;
			double DoubleVal;
		};

		struct IggyValuePath
		{
			void* Iggy;
			IggyValuePath* Parent;
			void* Name;
			void* Ref;
			int32_t ArrayIndex;
			int32_t Type;
		};

		struct IggyDataValue
		{
			DataType TypeId;
			int field_4;
			int field_8;
			int field_C;
			union {
				int32_t Int32;
				int64_t Int64;
				float Float;
				double Double;
				char* String;
				wchar_t* WString;
				void* Object;
			};
			int StringLength;
			int field_1C;
		};

		using ValuePathMakeNameRefProc = int (*)(IggyValuePath* path, IggyValuePath* parent, char const* name);
		using ValuePathMakeArrayRefProc = int (*)(IggyValuePath* path, IggyValuePath* parent, int index, void* iggy);
		using ValuePathSetArrayIndexProc = void (*)(IggyValuePath* path, int index);

		using ValueGetTypeProc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, DataType* type);
		using ValueGetArrayLengthProc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, uint32_t* result);

		using ValueGetBooleanProc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, uint32_t* result);
		using ValueGetF64Proc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, double* result);
		using ValueGetStringUTF8Proc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, int maxLength, char* result, int* resultLength);

		using ValueSetBooleanProc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, uint32_t value);
		using ValueSetF64Proc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, double value);
		using ValueSetStringUTF8Proc = int (*)(IggyValuePath const* path, void* nameRef, char const* name, char const* value, int length);
		
		using PlayerCreateFastNameUTF8 = void* (*)(void* player, char const* name, int length);
		using PlayerCallMethod = int (*)(void* player, IggyDataValue* retval, IggyValuePath const* objectPath, void* methodName, int numArgs, IggyDataValue const* args);

		struct FlashObject : ProtectedGameObject<FlashObject>
		{
			virtual void Destroy() = 0;
			virtual void SetX() = 0;
			virtual void SetY() = 0;
			virtual void SetScaleX() = 0;
			virtual void SetScaleY() = 0;
			virtual void SetRotation() = 0;
			virtual void SetAlpha() = 0;
			virtual void SetWidth() = 0;
			virtual void SetHeight() = 0;
			virtual void SetVisible() = 0;
			virtual void SetValue(char const * path, InvokeDataValue const & value, int arrayIndex = -1) = 0;
			virtual unsigned GetX() = 0;
			virtual unsigned GetY() = 0;
			virtual unsigned GetScaleX() = 0;
			virtual unsigned GetScaleY() = 0;
			virtual float GetRotation() = 0;
			virtual float GetAlpha() = 0;
			virtual unsigned GetWidth() = 0;
			virtual unsigned GetHeight() = 0;
			virtual bool GetVisible() = 0;
			virtual bool GetValue(char const * path, DataType desiredType, InvokeDataValue & value, int arrayIndex = -1) = 0;

			IggyValuePath* IggyValue;
			void * ValuePathRef2;
			void * StrX;
			void * StrY;
			void * StrScaleX;
			void * StrScaleY;
			void * StrRotation;
			void * StrAlpha;
			void * StrWidth;
			void * StrHeight;
			void * StrVisible;

			bool GetValueWorkaround(char const* path, DataType desiredType, InvokeDataValue& value, int arrayIndex = -1);
		};




		struct FlashInvoke
		{
			__int64 field_0;
			int field_8;
			__int64 field_10;
			__int64 field_18;
			void *IggyDataValue;
			__int64 field_28;
		};


		struct FlashInputEvent
		{
			uint64_t Unknown;
			char const * EventName;
		};


		struct FlashInvokeDefinition
		{
			void * IggyName;
			char const * Name;
		};


		struct FlashPlayer : ProtectedGameObject<FlashPlayer>
		{
			struct VMT
			{
				typedef bool (*Invoke6Proc)(FlashPlayer* self, int64_t invokeEnum, InvokeDataValue* a3, InvokeDataValue* a4, InvokeDataValue* a5, InvokeDataValue* a6, InvokeDataValue* a7, InvokeDataValue* a8);
				typedef bool (*Invoke5Proc)(FlashPlayer* self, int64_t invokeEnum, InvokeDataValue* a3, InvokeDataValue* a4, InvokeDataValue* a5, InvokeDataValue* a6, InvokeDataValue* a7);
				typedef bool (*Invoke4Proc)(FlashPlayer* self, int64_t invokeEnum, InvokeDataValue* a3, InvokeDataValue* a4, InvokeDataValue* a5, InvokeDataValue* a6);
				typedef bool (*Invoke3Proc)(FlashPlayer* self, int64_t invokeEnum, InvokeDataValue* a3, InvokeDataValue* a4, InvokeDataValue* a5);
				typedef bool (*Invoke2Proc)(FlashPlayer* self, int64_t invokeEnum, InvokeDataValue* a3, InvokeDataValue* a4);
				typedef bool (*Invoke1Proc)(FlashPlayer* self, int64_t invokeEnum, InvokeDataValue* a3);
				typedef bool (*Invoke0Proc)(FlashPlayer* self, int64_t invokeEnum);
				typedef bool (*InvokeArgsProc)(FlashPlayer* self, int64_t invokeEnum, InvokeDataValue* args, unsigned numArgs);

				void (*Destroy)(FlashPlayer* self);
				void (*OnWTCompletion)(FlashPlayer* self);
				void (*OnWTCanceled)(FlashPlayer* self);
				void (*ExecuteWTKernel)(FlashPlayer* self);
				int (*GetHeuristic)(FlashPlayer* self);
				void (*OnInputEvent)(FlashPlayer* self);
				void (*SendEventToFlash)(FlashPlayer* self);
				uint16_t* (*OnInputEventText)(FlashPlayer* self, uint16_t* retval, FlashInputEvent* event);
				void (*SetModifierStates)(FlashPlayer* self, bool, bool, bool);
				void (*field_48)(FlashPlayer* self);
				Invoke6Proc Invoke6;
				Invoke5Proc Invoke5;
				Invoke4Proc Invoke4;
				Invoke3Proc Invoke3;
				Invoke2Proc Invoke2;
				Invoke1Proc Invoke1;
				Invoke0Proc Invoke0;
				InvokeArgsProc InvokeArgs;
				bool (*HasFreeStringScratchArea)(FlashPlayer* self, uint64_t size);
				bool (*HasFreeWStringScratchArea)(FlashPlayer* self, uint64_t size);
				bool (*HasInvokes)(FlashPlayer* self);
				void (*field_A8)(FlashPlayer* self);
				void (*InvokeByName)(FlashPlayer* self, char const* name, uint64_t unknown, InvokeDataValue* arg);
				void (*SetRenderRectangle)(FlashPlayer* self);
				void (*SetSize)(FlashPlayer* self, glm::ivec2 const& size);
				int* (*GetSize)(FlashPlayer* self);
				void (*field_D0)(FlashPlayer* self);
				void (*Render)(FlashPlayer* self);
				void (*field_E0)(FlashPlayer* self);
				void (*field_E8)(FlashPlayer* self);
				void (*GetFlashMovieProperties)(FlashPlayer* self);
				void (*GotoFrame)(FlashPlayer* self, uint64_t frame);
				void (*ForceGotoFrame)(FlashPlayer* self, uint64_t frame);
				FlashObject* (*CreateFlashObject)(FlashPlayer* self, char const* path, int arrayIndex);
				ig::FlashObject* (* GetRootObject)(FlashPlayer* self);
				void (*SetFrameRate)(FlashPlayer* self, uint32_t frameRate);
				void (*SetPath)(FlashPlayer* self, Path* path);
				void (*OnFunctionCalled)(FlashPlayer* self);
				void (*Tick)(FlashPlayer* self);
				void (*field_138)(FlashPlayer* self);
				void (*Init)(FlashPlayer* self);
				void (*OnEventTerminate)(FlashPlayer* self);
				void (*Activate)(FlashPlayer* self);
				void (*Deactivate)(FlashPlayer* self);
				void (*GainFocus)(FlashPlayer* self);
				void (*LoseFocus)(FlashPlayer* self);
				void (*CopyEditableValue)(FlashPlayer* self);
				void (*PasteEditableValue)(FlashPlayer* self);
				void (*Cut)(FlashPlayer* self);
				void (*SetFlashMousePosition)(FlashPlayer* self);
				void (*SetupEventMapping)(FlashPlayer* self);
				void (*SetStageSize)(FlashPlayer* self);
				void (*AddInvokeName)(FlashPlayer* self, int index, char const* name);
				void (*field_1A8)(FlashPlayer* self);
				void (*LoadTextureFile)(FlashPlayer* self, Path* path);
				void (*EnableMouseProcessing)(FlashPlayer* self);
			};

			virtual void Destroy() = 0;
			virtual void OnWTCompletion() = 0;
			virtual void OnWTCanceled() = 0;
			virtual void ExecuteWTKernel() = 0;
			virtual int GetHeuristic() = 0;
			virtual void OnInputEvent() = 0;
			virtual void SendEventToFlash() = 0;
			virtual uint16_t * OnInputEventText(uint16_t * retval, FlashInputEvent * event) = 0;
			virtual void SetModifierStates(bool, bool, bool) = 0;
			virtual void field_48() = 0;
			virtual bool Invoke6(int64_t invokeEnum, InvokeDataValue *a3, InvokeDataValue *a4, InvokeDataValue *a5, InvokeDataValue *a6, InvokeDataValue *a7, InvokeDataValue *a8) = 0;
			virtual bool Invoke5(int64_t invokeEnum, InvokeDataValue *a3, InvokeDataValue *a4, InvokeDataValue *a5, InvokeDataValue *a6, InvokeDataValue *a7) = 0;
			virtual bool Invoke4(int64_t invokeEnum, InvokeDataValue *a3, InvokeDataValue *a4, InvokeDataValue *a5, InvokeDataValue *a6) = 0;
			virtual bool Invoke3(int64_t invokeEnum, InvokeDataValue *a3, InvokeDataValue *a4, InvokeDataValue *a5) = 0;
			virtual bool Invoke2(int64_t invokeEnum, InvokeDataValue *a3, InvokeDataValue *a4) = 0;
			virtual bool Invoke1(int64_t invokeEnum, InvokeDataValue *a3) = 0;
			virtual bool Invoke0(int64_t invokeEnum) = 0;
			virtual bool InvokeArgs(int64_t invokeEnum, InvokeDataValue * args, unsigned numArgs) = 0;
			virtual bool HasFreeStringScratchArea(uint64_t size) = 0;
			virtual bool HasFreeWStringScratchArea(uint64_t size) = 0;
			virtual bool HasInvokes() = 0;
			virtual void field_A8() = 0;
			virtual void InvokeByName(char const * name, uint64_t unknown, InvokeDataValue * arg) = 0;
			virtual void SetRenderRectangle(glm::ivec2 const& topLeft, glm::ivec2 const& size, uint8_t type) = 0;
			virtual void SetPosition(glm::ivec2 const& position) = 0;
			virtual glm::ivec2 const& GetPosition() = 0;
			virtual void field_D0() = 0;
			virtual void Render() = 0;
			virtual void field_E0() = 0;
			virtual void field_E8() = 0;
			virtual void GetFlashMovieProperties() = 0;
			virtual void GotoFrame(uint64_t frame) = 0;
			virtual void ForceGotoFrame(uint64_t frame) = 0;
			virtual FlashObject * CreateFlashObject(char const * path, int arrayIndex) = 0;
			virtual ig::FlashObject *GetRootObject() = 0;
			virtual void SetFrameRate(uint32_t frameRate) = 0;
			virtual void SetPath(Path * path) = 0;
			virtual void OnFunctionCalled() = 0;
			virtual void Tick() = 0;
			virtual void field_138() = 0;
			virtual void Init() = 0;
			virtual void OnEventTerminate() = 0;
			virtual void Activate() = 0;
			virtual void Deactivate() = 0;
			virtual void GainFocus() = 0;
			virtual void LoseFocus() = 0;
			virtual void CopyEditableValue() = 0;
			virtual void PasteEditableValue() = 0;
			virtual void Cut() = 0;
			virtual void SetFlashMousePosition() = 0;
			virtual void SetupEventMapping() = 0;
			virtual void SetStageSize(unsigned width, unsigned height) = 0;
			virtual void AddInvokeName(int index, char const * name) = 0;
			virtual void field_1A8() = 0;
			virtual void LoadTextureFile(Path * path) = 0;
			virtual void EnableMouseProcessing() = 0;


			__int64 field_8;
			glm::ivec2 Position;
			glm::ivec2 RenderRectangeleSize;
			glm::ivec2 RenderRectangeleSize2;
			__int16 field_28;
			void * IggyPlayer;
			IggyValuePath * IggyPlayerRootPath;
			ObjectSet<FlashInvokeDefinition> Invokes;
			int FlashInvokePool;
			int field_64;
#if !defined(OSI_EOCAPP)
			char const * PoolName;
#endif
			CRITICAL_SECTION CriticalSection;
			char Invoked_M;
			char field_99;
			ObjectSet<FlashInvoke *> QueuedInvokes;
			ObjectSet<FlashInvoke *> Invokes2;
			ObjectSet<InvokeDataValue> InvokeValues;
			ObjectSet<FlashPlayer *> FlashPlayers;
			__int64 field_120;
			__int64 field_128;
			ig::FlashObject * RootFlashObject;
			RefMap<uint32_t, uint32_t> InputEvents;
			ScratchBuffer * ScratchString1;
			ScratchBuffer * ScratchStringW;
			ScratchBuffer * ScratchString2;
			Path Path;
			__int64 field_188;
			__int64 field_190;
		};

		struct IggyBinding
		{
			virtual ~IggyBinding() = 0;
			virtual void BeginRendering() = 0;
			virtual void EndRendering() = 0;
			virtual bool Init(void* gDrawContext) = 0;
			virtual void CleanupDeadFlashPlayers() = 0;
			virtual void StartFlashRenderFrame(void* d3d11Renderer) = 0;
			virtual void EndFlashRenderFrame(void* d3d11Renderer) = 0;
			virtual void Render(void* d3d11Renderer, void* unknown) = 0;
			virtual ComponentHandle* GetRenderTargetTextureHandle(ComponentHandle& handle) = 0;
			virtual void ResizeRenderTarget(void*, void*) = 0;
			virtual void LoadFlashMovieFromFile(Path const& path, uint32_t* scratchPoolSizes, uint32_t flags, uint32_t* invokePoolSize, uint32_t fontCacheBitmapWidth, uint32_t fontCacheBitmapHeight) = 0;
			virtual void LoadFlashMovieFromMemory(void* buf, uint64_t bufSize, uint32_t* scratchPoolSizes, uint32_t flags, uint32_t* invokePoolSize, uint32_t fontCacheBitmapWidth, uint32_t fontCacheBitmapHeight) = 0;
			virtual int64_t LoadLibraryFromFile(char const* libraryName, Path const& flashPath, Path const& resourcePath) = 0;
			virtual int64_t LoadLibraryFromMemory(char const* libraryName, void* flashBuf, uint64_t flashSize, void* resourceBuf, uint64_t resourceSize) = 0;
			virtual void DestroyLibrary(int64_t handle) = 0;
			virtual void DestroyAllLibraries() = 0;
			virtual bool InstallFont(char const* name, Path const& path, uint8_t flags) = 0;
			virtual bool SetFallbackFont(char const* name, uint8_t flags) = 0;
			virtual bool LoadTTFFontLibrary(Path const& path) = 0;
			virtual void SetSomething(uint64_t unknown) = 0;
			virtual void* GetFlashWorkerThread() = 0;
			virtual void RemoveError(int errorCode) = 0;
			virtual void AddError(int errorCode) = 0;

			CRITICAL_SECTION CriticalSection;
			void * VMT_FileFormatIO;
			void * SomeContextPtr;
			ObjectSet<FlashPlayer *> FlashPlayers;
			ObjectSet<void *> TextureSubstitutionDestroyCallbacks;
			void * FlashWorkerThread;
			std::array<int32_t, 15> Libraries;
			glm::ivec4 RenderFrameViewport;
			bool Initialized;
			bool IsRendering;
			bool FlashRenderFrameStarted;
			ComponentHandle RenderTargetTextureHandle;
			uint64_t field_E0;
			void * GDraw;
			ObjectSet<Path> FontPaths;
			ObjectSet<STDString> FontNames;
			ObjectSet<void *> Fonts;
			ObjectSet<uint32_t> FlashErrorValues;
		};

		typedef void (* TraceCallbackProc)(void *, void *, char const *);
		typedef void (* WarningCallbackProc)(void *, void *, int, char const *);
		typedef void (* SetWarningCallbackProc)(WarningCallbackProc, void *);
		typedef void (* SetTraceCallbackUTF8Proc)(TraceCallbackProc, void *);
	}

	BY_VAL(ig::InvokeDataValue);

	struct UIObject : Noncopyable<UIObject>
	{
		static constexpr auto ObjectTypeIndex = ObjectHandleType::UIObject;

		typedef void(* OnFunctionCalledProc)(UIObject * self, const char *, unsigned int, ig::InvokeDataValue *);
		typedef void(* CustomDrawCallbackProc)(UIObject * self, void *);
		typedef float (GetUIScaleMultiplierProc)(UIObject * self);

		struct VMT
		{
			OnFunctionCalledProc OnFunctionCalled;
			CustomDrawCallbackProc CustomDrawCallback;
			void (* Destroy)(UIObject * self, bool);
			void(* SetHandle)(UIObject * self, ComponentHandle *);
			ComponentHandle * (* GetHandle)(UIObject * self, ComponentHandle *);
			void(* RequestDelete)(UIObject * self);
			void(* SetOwnerPlayerId)(UIObject * self, uint64_t);
			void(* SetPos)(UIObject * self, glm::ivec2 const&);
			void(* KeepWithin)(UIObject * self, int, int);
			void(* Show)(UIObject * self);
			void(* Hide)(UIObject * self);
			const char *(* GetDebugName)(UIObject * self);
			bool(* IsControllerUI)(UIObject * self);
			void(* Init)(UIObject * self);
			void(* InitAPI)(UIObject * self);
			void(* Update)(UIObject * self, float);
			void(* PostUpdate)(UIObject * self, float);
			void(* Render)(UIObject * self, void *, void *);
			void(* RegisterInvokeNames)(UIObject * self);
			void(* Resize)(UIObject * self);
			void *(* OnInputEvent)(UIObject * self, void *, void *);
			uint8_t *(* SendEventToFlash)(UIObject * self, uint8_t * a2, void *a3, unsigned int a4);
			void *(* OnInputEventText)(UIObject * self, void *, void *);
			uint16_t *(* OnUnlinkedInput)(UIObject * self, uint16_t *, uint32_t, uint16_t);
			void(* SetModifierStates)(UIObject * self, bool, bool, bool, bool);
			bool(* OnAPIPreResetDevice)(UIObject * self, void *);
			bool(* OnAPIPostResetDevice)(UIObject * self, void *);
			void(* OnControllerModeChanged)(UIObject * self);
			void(* OnPlayerDisconnect)(UIObject * self, int);
			void(* ReleaseRenderData)(UIObject * self);
			void(* PrepareRenderData)(UIObject * self);
			void(* DoPrepareRenderData)(UIObject * self);
			void(* Activate)(UIObject * self);
			void(* Deactivate)(UIObject * self);
			void(* LoseFocus)(UIObject * self);
			int64_t(* GetBitmapHeight)(UIObject * self);
			int64_t(* GetBitmapWidth)(UIObject * self);
			void * (* GetCharacter)(UIObject * self);
			bool (* SetPlayerHandle)(UIObject * self, ComponentHandle * handle);
			ComponentHandle * (* GetPlayerHandle)(UIObject * self, ComponentHandle * handle);
			bool (* Unknown1)(UIObject * self);
			void(* Unknown2)(UIObject * self);
			void * (* Unknown3)(UIObject * self);
			void (* Unknown4)(UIObject * self, void * a1);
		};


		virtual void OnFunctionCalled(const char * a1, unsigned int a2, ig::InvokeDataValue * a3);
		virtual void CustomDrawCallback(void * a1);
		virtual void Destroy(bool a1);
		virtual void SetHandle(ComponentHandle * a1);
		virtual ComponentHandle * GetHandle(ComponentHandle *);
		virtual void RequestDelete();
		virtual void SetOwnerPlayerId(uint64_t a1);
		virtual void SetPos(glm::ivec2 const& a1);
		virtual void KeepWithin(int a1, int a2);
		virtual void Show();
		virtual void Hide();
		virtual const char * GetDebugName();
		virtual bool IsControllerUI();
		virtual void Init();
		virtual void InitAPI();
		virtual void Update(float a1);
		virtual void PostUpdate(float a1);
		virtual void Render(void * a1, void * a2);
		virtual void RegisterInvokeNames();
		virtual void Resize();
		virtual void * OnInputEvent(void * a1, void * a2);
		virtual uint8_t * SendEventToFlash(uint8_t * a2, void *a3, unsigned int a4);
		virtual void * OnInputEventText(void * a1, void * a2);
		virtual uint16_t * OnUnlinkedInput(uint16_t * a1, uint32_t a2, uint16_t a3);
		virtual void SetModifierStates(bool a1, bool a2, bool a3, bool a4);
		virtual bool OnAPIPreResetDevice(void *);
		virtual bool OnAPIPostResetDevice(void *);
		virtual void OnControllerModeChanged();
		virtual void OnPlayerDisconnect(int a1);
		virtual void ReleaseRenderData();
		virtual void PrepareRenderData();
		virtual void DoPrepareRenderData();
		virtual void Activate();
		virtual void Deactivate();
		virtual void LoseFocus();
		virtual int64_t GetBitmapHeight();
		virtual int64_t GetBitmapWidth();
		virtual void * GetCharacter();
		virtual bool SetPlayerHandle(ComponentHandle * handle);
		virtual ComponentHandle * GetPlayerHandle(ComponentHandle * handle);
		virtual bool Unknown1();
		virtual void Unknown2();
		virtual void * Unknown3();
		virtual void Unknown4(void * a1);


		int BufferSizes;
		int field_C;
		int field_10;
		UIObjectFlags Flags;
		ig::FlashPlayer * FlashPlayer;
		Path Path;
		bool IsDragging;
		ComponentHandle ChildUIHandle;
		ComponentHandle ParentUIHandle;
		int Layer;
		int RenderOrder;
		int MovieLayout;
		glm::vec2 FlashSize;
		glm::vec2 MovieClipSize;
		glm::vec2 FlashMovieSize;
		glm::ivec2 SysPanelPosition;
		glm::vec2 SysPanelSize;
		float Left;
		float Top;
		float Right;
		glm::vec2 MinSize;
		float UIScale;
		float CustomScale;
		FixedString AnchorObjectName;
		STDString AnchorId;
		STDString AnchorTarget;
		STDString AnchorPos;
		STDString AnchorTPos;
		bool UIScaling;
		bool IsUIMoving;
		bool IsDragging2;
		bool IsMoving2;
		bool RenderDataPrepared;
		bool InputFocused;
		bool HasAnchorPos;
		ComponentHandle UIObjectHandle;
		int Type;
		PlayerId PlayerId;


		void LuaSetPosition(int x, int y);
		std::optional<glm::ivec2> LuaGetPosition();
		void LuaShow();
		void LuaHide();
		bool LuaInvoke(lua_State* L, STDString const& method);
		void LuaGotoFrame(int frame, std::optional<bool> force);
		std::optional<ig::InvokeDataValue> GetValue(lua_State* L, STDString const& path, std::optional<STDString> typeName, std::optional<int> arrayIndex);
		void SetValue(STDString const& path, ig::InvokeDataValue const& value, std::optional<int> arrayIndex);
		ComponentHandle LuaGetHandle();
		std::optional<ComponentHandle> LuaGetPlayerHandle();
		int GetTypeId();
		UserReturn LuaGetRoot(lua_State* L);
		void LuaDestroy();
		void LuaExternalInterfaceCall(lua_State* L, STDString const& method);
		void CaptureExternalInterfaceCalls();
		void CaptureInvokes();
		void EnableCustomDraw();
		void SetCustomIcon(STDWString const& element, STDString const& icon, int width, int height, std::optional<STDString> materialGuid);
		void ClearCustomIcon(STDWString const& element);
		float GetUIScaleMultiplier();
		void SetMovieClipSize(float width, float height, std::optional<float> scale);
	};

	struct CustomDrawStruct
	{
		using UIClearIcon = void (CustomDrawStruct* drawStruct);
		using UICreateIconMesh = void (FixedString const& iconName, CustomDrawStruct* drawStruct, int width, int height, FixedString const& materialGuid);

		void* VMT{ nullptr };
		void* IconMesh{ nullptr };
		FixedString IconName;
		ComponentHandle CustomTextureHandle;
		uint8_t DrawEffect{ 0 };
	};


	struct UIObjectFunctor
	{
		UIObject * (* CreateProc)(Path *);
		Path Path;
	};



	struct UIObjectManager : public ComponentFactory<UIObject>
	{
		struct SomeObject
		{
			uint64_t ObjectIndex_M;
			ComponentHandle ObjHandle1;
			ComponentHandle ObjHandle2;
			__int16 field_18;
			BYTE field_1A[6];
		};

		using RegisterUIObjectCreatorProc = void (UIObjectManager * self, unsigned int index, UIObjectFunctor * creator);
		using CreateUIObjectProc = ComponentHandle * (UIObjectManager * self, ComponentHandle * handle, unsigned int layerIndex, unsigned int creatorId, UIObjectFlags flags, uint64_t resourceFlags, uint16_t playerId);
		using DestroyUIObjectProc = void (UIObjectManager * self, ComponentHandle * handle);
		using GetInstanceProc = UIObjectManager * ();

		void * InputEventListenerVMT;
		void * InputDeviceListenerVMT;
		void * APIEventListenerVMT;
		CRITICAL_SECTION CriticalSection3;
		int64_t WorkerThreadJobVMT;
		int64_t field_D0;
		RefMap<uint32_t, UIObjectFunctor *> UIObjectCreators;
		int64_t field_E8;
		ObjectSet<UIObject *> UIObjects;
		bool ShouldPrepareRenderData;
		bool SortNeeded;
		bool RefreshTopNeeded;
		bool IsRenderingObjects;
		bool AllowRawInput;
		bool field_115;
		int UIDesignedHeight;
		int UIDesignedWidth;
		Map<ComponentHandle, uint64_t> *LastFrameDirtyFlags;
		Map<ComponentHandle, uint64_t> *CharacterDirtyFlags;
		CRITICAL_SECTION CriticalSection;
		SomeObject SomeObjects[4];
		int64_t field_1D8;
		int64_t field_1E0;
		int64_t field_1E8;
		int64_t field_1F0;
		int64_t field_1F8;
		RefMap<uint32_t, uint32_t> field_200;
		int64_t field_210;
		int UIObjectAggregateFlags;
		int field_21C;
		int64_t field_220;
		int64_t field_228;

		UIObject* GetByType(int typeId) const;
	};

	struct DragController
	{
		void* VMT;
		PlayerId PlayerId;
	};

	struct DragDropManager
	{
		using StartDragStringProc = bool(DragDropManager* self, PlayerId playerId, FixedString const& objectId, bool setupStartDrag, uint64_t mousePos);
		using StartDragHandleProc = bool(DragDropManager* self, PlayerId playerId, ComponentHandle const& objectHandle, bool setupStartDrag, uint64_t mousePos);
		using StopDragProc = bool (DragDropManager* self, PlayerId playerId);

		struct PlayerDragInfo
		{
			ComponentHandle DragObject;
			EntityHandle DragEgg;
			FixedString DragId;
			glm::vec2 MousePos;
			bool IsDragging;
			bool IsActive;
			__int64 field_28;
			ObjectSet<DragController*> Controllers;
		};

		RefMap<PlayerId, PlayerDragInfo> PlayerDragDrops;

		bool StopDragging(PlayerId playerId);
		bool StartDraggingName(PlayerId playerId, FixedString const& objectId);
		bool StartDraggingObject(PlayerId playerId, ComponentHandle const& objectHandle);
	};

	
	namespace ecl
	{
		struct EoCUI : public UIObject
		{
			typedef void (* ctor)(EoCUI * self, dse::Path * path);

			EoCUI(dse::Path * path);

			bool field_150;
			bool field_151;
			bool field_152;
			bool field_153;
			bool field_154;
			bool SomeInitPerformed;
			__int16 field_156;
			ComponentHandle ObjHandle1;
			ComponentHandle TooltipHandle;
			uint64_t field_168;
			uint64_t field_170;
			STDString field_178;
		};

		struct FlashCustomDrawCallback
		{
			using CustomDrawObject = void (FlashCustomDrawCallback* callback, void* mesh);

			wchar_t* Name;
			uint64_t field_8;
			uint64_t field_10;
			uint64_t field_18;
			int field_20;
			float OpacityFade;
			uint64_t field_28;
			uint64_t field_30;
			float DrawRect[4];
			bool HasRect;
			bool SomeClippingFlag;
			bool SomeClippingFlag2;
			bool field_4B;
			glm::ivec2 TextureDimensions;
			glm::mat4 TransformMatrix;
			uint64_t field_98;
		};

		struct UIExamine : public EoCUI
		{
			uint64_t UIStatusIconHelper[3];
			void* NetEventManagerVMT;
			uint64_t field_1B8;
			void* GameEventManagerVMT;
			CustomDrawStruct field_1C8;
			CustomDrawStruct Icon;
			uint64_t field_218;
			uint64_t field_220;
			int field_228;
			char field_22C;
			char field_22D;
			char field_22E;
			char field_22F;
			uint64_t field_230;
			UserId UserID;
			ComponentHandle OH1;
			ComponentHandle ObjectBeingExamined;
			uint64_t field_250;
		};

		struct PickPosition
		{
			glm::vec3 Position;
			int field_C;
			int field_10;
			int field_14;
		};

		struct PickingHelperBase
		{
			PickPosition PlaceablePickInfo;
			PickPosition HoverCharacterPickPos;
			PickPosition HoverItemPickPos;
			PickPosition WalkablePickPos;
			ComponentHandle HoverCharacterHandle2;
			ComponentHandle HoverCharacterHandle;
			ComponentHandle HoverItemHandle;
			ComponentHandle HoverCharacterOrItemHandle;
			ComponentHandle PlaceablePickHandle;
			ComponentHandle OH6;
			uint64_t WalkablePickInfo1;
			char WalkablePickInfo2;
			char GameObjectPickFlags;
		};

		struct PickingHelper
		{
			struct GameObjectPicker
			{
				void* VMT;
				void* field_8;
				PickPosition WorldPos;
			};

			void* VMT;
			PickingHelperBase b;
			Level* Level;
			PlayerId PlayerId;
			ComponentHandle OH7;
			ObjectSet<ComponentHandle> OS_OH;
			void* field_E0;
			int field_E8;
			float ScreenPos[2];
			GameObjectPicker* GameObjectPick;
			void* PlaceablePick;
			void* WalkablePick;
			void* ControllerPointerManager;
		};

		struct PickingHelperManager
		{
			void* VMT;
			void* field_8;
			RefMap<PlayerId, PickingHelper*> PlayerHelpers;
			void* field_20;
			void* field_28;
		};
		
		struct UIDragController : public DragController
		{
			ComponentHandle DragObject;
			EntityHandle DragEgg;
			bool IsDragging;
		};

		struct DragDropManager
		{
			struct Controllers
			{
				ecl::UIDragController* UIDragController;
				DragController* WorldDragController;
				DragController* SkillDragController;
				DragController* EggDragController;
				DragController* CharacterDragController;
			};

			void* VMT;
			uint64_t field_8;
			RefMap<PlayerId, Controllers> PlayerControllers;
		};
	}
}
