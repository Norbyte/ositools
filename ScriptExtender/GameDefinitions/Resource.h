#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

struct VisualFactory;

struct Resource : public ProtectedGameObject<Resource>
{
    void* VMT;
#if !defined(OSI_EOCAPP)
    Path XMLName;
#endif
    Path SourceFile;
    FixedString PackageName;
    FixedString ModName;
    FixedString UUID;
    STDString Name;
    bool IsLocalized;
    bool IsActive;
    bool IsDirty;
    bool IsOriginal;
};

struct TwoStepLoadable
{
    void* VMT;
    int Loaded;
    void* SRWKernelLock;
    void* LoadJob;
};

struct DeferredLoadableResource : public Resource
{
    TwoStepLoadable TwoStepLoader;
};

struct NonDeferredLoadableResource : public Resource
{
    uint16_t RefCount;
    uint8_t LoadState;
};

struct ResourceContainer : public ProtectedGameObject<ResourceContainer>
{
    struct Bank : public ProtectedGameObject<Bank>
    {
        void* VMT;
        Map<FixedString, Resource*> Resources;
        __int64 SRWKernelLock;
        int field_28;
    };

    virtual ~ResourceContainer();
    virtual void Destroy();
    virtual bool AddResource(Resource* resource);
    virtual void RemoveResource(Resource* resource, bool destroy);
    virtual void RemoveResource(ResourceType type, FixedString const& uuid, bool destroy);
    virtual Resource* GetResource(ResourceType type, FixedString const& uuid);
    virtual bool HasResource(Resource* resource);

    Bank* Banks[14];
};

struct ResourceBank;

struct ResourcePackage : public ProtectedGameObject<ResourcePackage>
{
    void* VMT;
    ResourceContainer Container;
    ResourceBank* ResourceBank;
    FixedString Name;
};

struct ResourceBank : public ProtectedGameObject<ResourceBank>
{
    void* VMT;
    ResourceContainer Container;
    Map<FixedString, ResourcePackage*> Packages;
    void* LoadHelper;
    FixedString FS_;
};

struct ResourceManager : public ProtectedGameObject<ResourceManager>
{
    using GetInstanceProc = ResourceManager * ();
    using InstantiateVisualProc = Visual* (ResourceManager* self, FixedString const& visualId, bool playEffects, bool prepareAnims, bool unknown);
    using GetResourceBankProc = ResourceBank* (ResourceManager* self);

    void DestroyVisual(ComponentHandle const& handle);

    void* VMT;
    ResourceBank* ResourceBanks[2];
    void* MeshProxyFactory;
    VisualFactory* VisualFactory;
    PrimitiveSmallSet<FixedString> AnimationPreloadList;
    void* EffectFactory;
    void* VisualTemplateManager;
    void* AnimationTemplateManager;
    void* PhysicsTemplateManager;
    void* PhysicsInstantiator;
    void* ModelManager;
    void* TextureManager;
    void* TextureCache;
    void* ScriptManager;
    void* ShaderManager;
    void* MaterialManager;
    EoCSoundManager* SoundManager;
    void* VideoManager;
    void* AtmosphereManager;
    void* VisualSetManager;
    void* MaterialSetManager;
    void* GameStatManager;
    void* ModelProxyManager;
    void* MeshProxyTemplateManager;
    PrimitiveSmallSet<void*>* PhysicsLoadResult; // PhysicsTemplate*
    ObjectSet<void*> ScriptDataLoaderResults; // ObjectSet<ScriptDataLoaderResult*>;
    CRITICAL_SECTION CriticalSection;
    RefMap<FixedString, void*> ResourceDependencies; // <Path, ResourceDependencies*>
    Map<FixedString, Path*> Sources;
    ObjectSet<void*> VisualLoaders; // ObjectSet<VisualLoaderDesc>
    Map<FixedString, void*> PhysicsDataLoaders; // Map<FixedString, PhysicsDataLoader*>
    Map<FixedString, void*> ScriptDataLoaders; // Map<FixedString, ScriptDataLoader*>
    Map<FixedString, void*> GenomeAnimationManagers; // Map<FixedString, GenomeManager*>
    void* GenomeTypeManager;
    void* PhysicsObjectManager;
    RefMap<FixedString, void*> field_1C8;
    ObjectSet<FixedString> OS_FS;
    ig::IggyBinding* FlashBinding;
    uint32_t ResourceTypesEnabled;
    bool AllowDeferredLoading;
};

END_SE()
