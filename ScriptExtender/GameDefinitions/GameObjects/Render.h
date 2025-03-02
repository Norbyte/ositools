#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <d3d11.h>

BEGIN_SE()

struct CommandDispatcherBuffer
{
	void* BufferStart;
	void* BufferCur;
	uint64_t Size;
};

struct CommandDispatcher : public ProtectedGameObject<CommandDispatcher>
{
	CommandDispatcherBuffer ExecuteBuffer;
	CommandDispatcherBuffer DestructorBuffer;
	bool StoreCommand;
};

struct Dispatcher : public ProtectedGameObject<Dispatcher>
{
	void* VMT;
	CommandDispatcherBuffer* Buffers[2];
	CRITICAL_SECTION CriticalSections[2];
};


struct ThreadDispatcher : public Dispatcher
{
	int field_68;
};


template <class T>
struct ObjectContainer : public ProtectedGameObject<ObjectContainer<T>>
{
	T** Objects;
	uint32_t** Salts;
	uint32_t PageSize;
	Queue<uint32_t> FreeList;
	CRITICAL_SECTION PageLock;
	SRWLOCK SRWSpinLock;
	int field_60;
	ObjectSet<T*> Objects2;
	CRITICAL_SECTION PageAllocLock;
	uint32_t NumUsedPages;
	uint32_t NumTotalPages;

	T* Get(ComponentHandle h) const
	{
		if (!h) return nullptr;

		auto index = h.GetIndex();
		auto page = index / PageSize;
		auto offset = index - page * PageSize;
		if (page >= NumUsedPages || index >= PageSize) return nullptr;

		if (Salts[page][index] != h.GetSalt()) return nullptr;

		return &Objects[page][index];
	}
};

END_SE()

BEGIN_NS(rf)

struct D3DInitStruct
{
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
};

struct D3DInitStructParent
{
	void* VMT;
	void* SomePtr;
	void* unknown3;
	D3DInitStruct* DevicePtrs;
};

struct AppMaybe
{
	using AnotherD3DInitProc = void* (void* param1, void* initStruct);
	void* VMT;
	char unknown[16];
	D3DInitStructParent* D3DStuff;
};

struct Texture
{
	ID3D11Resource* Resource;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11DepthStencilView* DepthStencilView2;
	ID3D11UnorderedAccessView* UnorderedAccessView;
	ID3D11RenderTargetView** RenderTargetViews;
	ID3D11DepthStencilView** DepthStencilViewArray;
	void* TexArray2;
	__int64 field_40;
	DXGI_FORMAT DXGIFormatTexture;
	DXGI_FORMAT DXGIFormatRTV;
	DXGI_FORMAT DXGIFormatDSV;
	DXGI_FORMAT DXGIFormat4;
	uint32_t Format;
	uint16_t UsageFlag;
	uint8_t AccessFlag;
	int MSAASamples;
	int SizeX;
	int SizeY;
	int SizeZ;
	int TexArraySize;
	int NumMipSizes;
	int NumMipSizes2;
	int MipBytes_M;
	int field_80;
	int field_84;
	__int64 field_88;
};

struct RendererBase
{
#if !defined(OSI_EOCAPP)
	Map<FixedString, void*> DebugRenderPasses;
#endif
	ObjectContainer<Texture> TextureContainer;
	ObjectContainer<void*> ShaderContainer;
	ObjectContainer<void*> VertexBufferContainer;
	ObjectContainer<void*> IndexBufferContainer;
	ObjectContainer<void*> StructuredBufferContainer;
	ObjectContainer<void*> TexturedFontContainer;
	ObjectContainer<void*> VertexFormatContainer;
	ObjectContainer<void*> SamplerStateContainer;
	ObjectContainer<void*> BlendStateContainer;
	ObjectContainer<void*> DepthStateContainer;
	ObjectContainer<void*> RasterizerStateContainer;
	CRITICAL_SECTION CriticalSection;
	__int64 GameTime;
	__int64 field_830;
	__int64 API;
	void *field_840;
	void *field_848;
	char field_850;
	__int64 field_858;
	__int64 field_860;
	__int64 field_868;
	__int64 field_870;
	__int64 field_878;
	__int64 field_880;
	__int64 field_888;
	int field_890;
	__int64 field_898;
	int field_8A0;
	__int64 field_8A8;
	__int64 field_8B0;
	__int64 field_8B8;
	__int64 field_8C0;
	__int64 field_8C8;
	__int64 field_8D0;
	__int64 field_8D8;
	__int64 field_8E0;
	__int64 field_8E8;
	__int64 field_8F0;
	int field_8F8;
	char field_8FC;
	char field_8FD;
	__int64 field_900[64];
	int field_B00;
	int field_B04;
	int field_B08;
	__int64 field_B10;
	__int64 field_B18;
	int field_B20;
};


struct D3D11Renderer : public RendererBase
{
	ID3D11Device* D3D11Device;
	ID3D11DeviceContext* D3D11DeviceContext;
	__int64 field_B38;
	__int64 field_B40;
	ObjectContainer<void*> CompiledShaders;
	ObjectContainer<void*> ConstantBuffers;
	__int64 field_CB8;
	__int64 field_CC0;
	__int64 field_CC8;
	__int64 field_CD0;
	uint8_t field_CD8[144];
	uint8_t field_D68[240];
	__int64 field_E58;
	__int64 field_E60;
	__int64 SRWSpinLock;
	int field_E70;
	int field_E74;
	int field_E78;
	int field_E7C;
	__int64 field_E80;
	int field_E88;
	int field_E8C;
#if !defined(OSI_EOCAPP)
	__int64 field_E90;
#endif
	ObjectContainer<void*> TextureRemoveData;
};


struct RendererCommandBuffer : public CommandDispatcher
{
	D3D11Renderer* Renderer;
	uint64_t field_40;
	uint64_t field_48;
	uint64_t field_50;
};

END_NS()

BEGIN_SE()

struct RenderThread : public ProtectedGameObject<RenderThread>
{
	struct RenderThreadDispatcher : public ProtectedGameObject<RenderThreadDispatcher>
	{
		RenderThread* Thread;
		ThreadDispatcher* BaseDispatcher;
		void* LeaveGameDrawScopeJob;
		__int64 field_18;
		Dispatcher* ParentDispatcher;
		uint32_t ThreadID;
	};

	void* Events[4];
	RenderThreadDispatcher* RenderDispatcher;
	void* GameThread;
	rf::RendererCommandBuffer* RCB1;
	rf::RendererCommandBuffer* RCB2;
};

END_SE()
