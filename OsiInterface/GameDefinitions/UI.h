#pragma once

#include <windows.h>
#include "BaseTypes.h"
#include "Enumerations.h"

namespace dse
{
	namespace ig
	{
		enum class DataType : int32_t
		{
			None = 0x1,
			Bool = 0x3,
			Double = 0x4,
			String = 0x5,
			WString = 0x6,
			Array = 0xA,
			Object = 0xC,
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
			void* Parent;
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
				void (*SetSize)(FlashPlayer* self, int* size);
				int* (*GetSize)(FlashPlayer* self);
				void (*field_D0)(FlashPlayer* self);
				void (*Render)(FlashPlayer* self);
				void (*field_E0)(FlashPlayer* self);
				void (*field_E8)(FlashPlayer* self);
				void (*GetFlashMovieProperties)(FlashPlayer* self);
				void (*GotoFrame)(FlashPlayer* self, uint64_t frame);
				void (*GotoFrame2)(FlashPlayer* self, uint64_t frame);
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
			virtual void SetRenderRectangle() = 0;
			virtual void SetSize(int * size) = 0;
			virtual int * GetSize() = 0;
			virtual void field_D0() = 0;
			virtual void Render() = 0;
			virtual void field_E0() = 0;
			virtual void field_E8() = 0;
			virtual void GetFlashMovieProperties() = 0;
			virtual void GotoFrame(uint64_t frame) = 0;
			virtual void GotoFrame2(uint64_t frame) = 0;
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
			virtual void SetStageSize() = 0;
			virtual void AddInvokeName(int index, char const * name) = 0;
			virtual void field_1A8() = 0;
			virtual void LoadTextureFile(Path * path) = 0;
			virtual void EnableMouseProcessing() = 0;


			__int64 field_8;
			int field_10;
			int field_14;
			int field_18;
			int field_1C;
			int field_20;
			int field_24;
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
			void * VMT_APIeventListener;
			CRITICAL_SECTION CriticalSection;
			void * VMT_FileFormatIO;
			void * SomeContextPtr;
			ObjectSet<FlashPlayer *> FlashPlayers;
			ObjectSet<void *> TextureSubstitutionDestroyCallbacks;
			void * FlashWorkerThread;
			DWORD Libraries[15];
			int RenderFrameViewport[4];
			bool Initialized;
			bool IsRendering;
			bool FlashRenderFrameStarted;
			ObjectHandle RenderTargetTextureHandle;
			uint64_t field_E0;
			void * GDraw;
			ObjectSet<Path> Paths;
			ObjectSet<STDString> ObjSet_StdString;
			ObjectSet<void *> Fonts;
			ObjectSet<uint32_t> FlashErrorValues;
		};

		typedef void (* TraceCallbackProc)(void *, void *, char const *);
		typedef void (* WarningCallbackProc)(void *, void *, int, char const *);
		typedef void (* SetWarningCallbackProc)(WarningCallbackProc, void *);
		typedef void (* SetTraceCallbackUTF8Proc)(TraceCallbackProc, void *);
	}


	enum UIObjectFlags : uint32_t
	{
		OF_Load = 0x1,
		OF_Loaded = 0x2,
		OF_RequestDelete = 0x4,
		OF_Visible = 0x8,
		OF_Activated = 0x10,
		OF_PlayerInput1 = 0x20,
		OF_PlayerInput2 = 0x40,
		OF_PlayerInput3 = 0x80,
		OF_PlayerInput4 = 0x100,
		OF_PlayerModal1 = 0x200,
		OF_PlayerModal2 = 0x400,
		OF_PlayerModal3 = 0x800,
		OF_PlayerModal4 = 0x1000,
		OF_PositioningFlag1 = 0x8000,
		OF_PositioningFlag2 = 0x10000,
		OF_DeleteOnChildDestroy = 0x20000,
		OF_PauseRequest = 0x40000,
		OF_SortOnAdd = 0x80000,
		OF_FullScreen = 0x400000,
		OF_PlayerTextInput1 = 0x800000,
		OF_PlayerTextInput2 = 0x1000000,
		OF_PlayerTextInput3 = 0x2000000,
		OF_PlayerTextInput4 = 0x4000000,
		OF_DontHideOnDelete = 0x10000000,
		OF_PrecacheUIData = 0x20000000,
		OF_PreventCameraMove = 0x40000000,
	};


	struct UIObject : Noncopyable<UIObject>
	{
		typedef void(* OnFunctionCalledProc)(UIObject * self, const char *, unsigned int, ig::InvokeDataValue *);

		struct VMT
		{
			OnFunctionCalledProc OnFunctionCalled;
			void(* OnCustomDrawCallback)(UIObject * self, void *);
			void (* Destroy)(UIObject * self, bool);
			void(* SetHandle)(UIObject * self, ObjectHandle *);
			ObjectHandle * (* GetHandle)(UIObject * self, ObjectHandle *);
			void(* RequestDelete)(UIObject * self);
			void(* SetOwnerPlayerId)(UIObject * self, uint64_t);
			void(* SetPos)(UIObject * self, int *);
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
			bool (* SetPlayerHandle)(UIObject * self, ObjectHandle * handle);
			ObjectHandle * (* GetPlayerHandle)(UIObject * self, ObjectHandle * handle);
			bool (* Unknown1)(UIObject * self);
			void(* Unknown2)(UIObject * self);
			void * (* Unknown3)(UIObject * self);
			void (* Unknown4)(UIObject * self, void * a1);
		};


		virtual void OnFunctionCalled(const char * a1, unsigned int a2, ig::InvokeDataValue * a3);
		virtual void OnCustomDrawCallback(void * a1);
		virtual void Destroy(bool a1);
		virtual void SetHandle(ObjectHandle * a1);
		virtual ObjectHandle * GetHandle(ObjectHandle *);
		virtual void RequestDelete();
		virtual void SetOwnerPlayerId(uint64_t a1);
		virtual void SetPos(int * a1);
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
		virtual bool SetPlayerHandle(ObjectHandle * handle);
		virtual ObjectHandle * GetPlayerHandle(ObjectHandle * handle);
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
		ObjectHandle ChildUIHandle;
		ObjectHandle ParentUIHandle;
		int Layer;
		int RenderOrder;
		int MovieLayout;
		float FlashSize[2];
		int field_74;
		int field_78;
		float FlashMovieSize[2];
		int SysPanelX;
		int SysPanelY;
		float SysPanelW;
		float SysPanelH;
		float Left;
		float Top;
		float Right;
		float MinWidth;
		float MinHeight;
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
		ObjectHandle UIObjectHandle;
		int Type;
		int16_t PlayerId;
	};

	struct CustomDrawStruct
	{
		void* VMT;
		void* IconMesh;
		FixedString IconName;
		ObjectHandle CustomTextureHandle;
		char DrawEffect;
	};


	struct UIObjectFunctor
	{
		UIObject * (* CreateProc)(Path *);
		Path Path;
	};



	struct UIObjectManager : public ObjectFactory<UIObject, 3>
	{
		struct SomeObject
		{
			uint64_t ObjectIndex_M;
			ObjectHandle ObjHandle1;
			ObjectHandle ObjHandle2;
			__int16 field_18;
			BYTE field_1A[6];
		};

		using RegisterUIObjectCreatorProc = void (UIObjectManager * self, unsigned int index, UIObjectFunctor * creator);
		using CreateUIObjectProc = ObjectHandle * (UIObjectManager * self, ObjectHandle * handle, unsigned int layerIndex, unsigned int creatorId, int flags, uint64_t resourceFlags, uint16_t playerId);
		using DestroyUIObjectProc = void (UIObjectManager * self, ObjectHandle * handle);
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
		RefMap<uint32_t, uint32_t> *field_120;
		RefMap<uint32_t, uint32_t> *field_128;
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
			ObjectHandle ObjHandle1;
			ObjectHandle TooltipHandle;
			uint64_t field_168;
			uint64_t field_170;
			STDString field_178;
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
			ObjectHandle OH1;
			ObjectHandle ObjectBeingExamined;
			uint64_t field_250;
		};

	}
}
