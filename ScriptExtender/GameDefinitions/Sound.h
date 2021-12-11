#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Module.h>
#include <Windows.h>

namespace dse
{
    using SoundObjectId = uint64_t;

    struct SoundCache
    {
        void* VMT;
        RefMap<FixedString, uint32_t> field_8;
        ObjectSet<uint32_t> SoundCache;
        RefMap<FixedString, void*> field_x;
        CRITICAL_SECTION CriticalSection1;
        CRITICAL_SECTION CriticalSection2;
    };


    struct SoundBankManager : SoundCache
    {
        uint64_t field_98;
        void* SoundManager;
        CRITICAL_SECTION CriticalSection;
        RefMap<FixedString, void*> field_D0;
        RefMap<FixedString, void*> field_E0;
        Pool UnknPool;
        ObjectSet<Path> Paths;
    };


    struct SoundEventManager : SoundCache
    {
        void* SoundManager;
        RefMap<FixedString, void*> field_A0;
        Pool LoadPool;
#if !defined(OSI_EOCAPP)
        Pool UnloadPool;
#endif
    };


    struct SoundSyncManager : SoundCache
    {
        void* SoundManager;
        Pool LoadPool;
    };


    struct SoundManager
    {
        virtual void field_0() = 0;
        virtual void field_8() = 0;
        virtual void field_10() = 0;
        virtual void field_18() = 0;
        virtual void field_20() = 0;
        virtual void field_28() = 0;
        virtual void field_30() = 0;
        virtual void field_38() = 0;
        virtual void field_40() = 0;
        virtual void field_48() = 0;
        virtual void SetEnvironmentSends() = 0;
        virtual void field_58() = 0;
        virtual void field_60() = 0;
        virtual void AddRumbleListener() = 0;
        virtual void field_70() = 0;
        virtual void field_78() = 0;
        virtual void field_80() = 0;
        virtual bool SetSwitch(const char* switchGroup, const char* switchState, SoundObjectId gameObjectId) = 0;
        virtual bool SetState(const char* stateGroupName, const char* stateName) = 0;
        virtual int SetRTPCValue(SoundObjectId gameObjectId, char const* rtpcName, float value) = 0;
        virtual float GetRTPCValue(SoundObjectId gameObjectId, char const* rtpcName) = 0;
        virtual int ResetRTPCValue(SoundObjectId gameObjectId, char const* rtpcName) = 0;
        virtual void StopAllOnObject(SoundObjectId gameObjectId) = 0;
        virtual void StopAll() = 0;
        virtual void StopAllHUDSounds() = 0;
        virtual void StopAllGMSounds() = 0;
        virtual void StopAllAmbientSounds() = 0;
        virtual void StopAllMusicSounds() = 0;
        virtual void PauseAllSounds() = 0;
        virtual void ResumeAllSounds() = 0;
        virtual void GetSoundInterface() = 0;
        virtual void GetSpeakerConfiguration() = 0;
        virtual void SetObjectObstructionAndOcclusion() = 0;
        virtual void field_108() = 0;
        virtual uint32_t GetIDFromString(char const* string) = 0;
        virtual void Suspend() = 0;
        virtual void WakeupFromSuspend() = 0;
        virtual void Initialize() = 0;
        virtual void RenderAudio() = 0;
        virtual void field_138() = 0;
        virtual void ClearBanks() = 0;
        virtual void ClearBanksAndEvents() = 0;
        virtual void field_150() = 0;
        virtual void RegisterSoundObjectInternal() = 0;
        virtual void field_160() = 0;
        virtual void LoadEvent_Blocking() = 0;
        virtual void LoadEvent_Blocking2() = 0;
        virtual void UnloadEvent_Blocking() = 0;
        virtual void UnloadEvent_Blocking2() = 0;
        virtual void PrepareEvent() = 0;
        virtual void PrepareEvent2() = 0;
        virtual void PrepareEvent3() = 0;
        virtual void PrepareEvent4() = 0;
        virtual void LoadBank_Blocking() = 0;
        virtual void UnloadBank_Blocking() = 0;
        virtual void LoadBank() = 0;
        virtual void UnloadBank() = 0;
        virtual void PrepareBank_Blocking() = 0;
        virtual void UnprepareBank_Blocking() = 0;
        virtual void PrepareBank2() = 0;
        virtual void PrepareBank3() = 0;
        virtual void PrepareGameSyncs() = 0;
        virtual void PrepareGameSyncs2() = 0;
        virtual void PrepareGameSyncs3() = 0;
        virtual void PrepareGameSyncs4() = 0;
        virtual bool PostEvent(SoundObjectId gameObjectId, uint32_t eventId, float positionSec, void* cookie) = 0;
        virtual bool PostEvent(SoundObjectId gameObjectId, char const* eventName, float positionSec, void* cookie) = 0;
        virtual bool PostEvent(void* soundEventRequest) = 0;
        virtual bool PlayExternalSound(SoundObjectId gameObjectId, uint32_t eventId, Path const& path, unsigned int codecId, void* cookie) = 0;

        // void* VMT;
        Module Module;
        ObjectSet<void*> SfxSoundbankSearchPaths; // ObjectSet<SoundbankSearchPath>
        ObjectSet<void*> LocaSoundbankSearchPaths; // ObjectSet<SoundbankSearchPath>
        SoundBankManager SoundBankManager;
        SoundEventManager SoundEventManager;
        SoundSyncManager SoundSyncManager;
        RefMap<FixedString, void*> field_640;
        __int64 ListenerPositions;
        __int64 field_658;
        __int64 field_660;
        __int64 field_668;
        ObjectSet<void*> OS_TimedUnregisterData; // ObjectSet<TimedUnregisterData>
        SoundObjectId MusicHandle;
        SoundObjectId AmbientHandle;
        SoundObjectId HUDHandle;
        SoundObjectId GMSoundHandle;
        SoundObjectId PlayerSoundHandles[4];
        CRITICAL_SECTION CriticalSection;
        ObjectSet<void*> OS_State; // ObjectSet<State>
        RefMap<FixedString, void*> field_718;
        RefMap<FixedString, void*> field_728;
        __int64 field_738;
        __int64 field_740;
        __int64 field_748;
        CRITICAL_SECTION field_750;
        CRITICAL_SECTION field_778;
        CRITICAL_SECTION field_7A0;
        __int64 SRWKernelLock;
        __int64 field_7D0;
    };



    struct EoCSoundManager : SoundManager
    {
        __int64 field_7D8;
        __int64 field_7E0;
        __int64 field_7E8;
        CRITICAL_SECTION field_7F0;
        __int64 field_818;
        int field_820;
        int field_824;
        __int64 field_828;
        __int64 field_830;
        __int64 field_838;
        ObjectSet<SoundObjectId> SoundObjectHandles;
        void* IOHook;
        RefMap<FixedString, void*> field_868;
        ObjectSet<void*> OS_PauseAudioDesc; // ObjectSet<PauseAudioDesc>
        __int64 field_898;
        int PrepareEventPool;
        uint32_t SomeSoundIDs[4];
    };
}
