#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

struct VisualFactory;

struct Resource : public ProtectedGameObject<Resource>
{
    void* VMT;
    Path XMLName;
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


struct ResourceContainer : public ProtectedGameObject<ResourceContainer>
{
    struct Bank : public ProtectedGameObject<Bank>
    {
        void* VMT;
        Map<FixedString, Resource*> Resources;
        __int64 SRWKernelLock;
        int field_28;
    };

    void* VMT;
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

    void DestroyVisual(ComponentHandle const& handle);

    void* VMT;
    ResourceBank* ResourceBanks[2];
    void* MeshProxyFactory;
    VisualFactory* VisualFactory;
    PrimitiveSmallSet<uint32_t> AnimationClassNames; // ObjectSet<TAnimationSimpleNameClassname>
    void* EffectFactory;
    void* VisualTemplateManager;
    void* AnimationTemplateManager;
    void* PhysicsTemplateManager;
    void* field_60;
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
    void* field_C0;
    void* ModelProxyManager;
    void* MeshProxyTemplateManager;
    PrimitiveSmallSet<void*>* PhysicsTemplates;
    ObjectSet<void*> ScriptDataLoaderResults; // ObjectSet<ScriptDataLoaderResult*>;
    CRITICAL_SECTION CriticalSection;
    RefMap<FixedString, void*> field_128;
    Map<FixedString, Path*> UnknownPaths;
    ObjectSet<void*> VisualLoaderDescs; // ObjectSet<VisualLoaderDesc>
    Map<FixedString, void*> PhysicsDataLoaders; // Map<FixedString, PhysicsDataLoader*>
    Map<FixedString, void*> ScriptDataLoaders; // Map<FixedString, ScriptDataLoader*>
    Map<FixedString, void*> GenomeAnimationManagers; // Map<FixedString, GenomeManager*>
    void* GenomeTypeManager;
    void* PhysicsObjectManager;
    RefMap<FixedString, void*> field_1C8;
    ObjectSet<FixedString> OS_FS;
    void* field_1F8;
    int field_200;
    char field_204;
};

END_SE()
