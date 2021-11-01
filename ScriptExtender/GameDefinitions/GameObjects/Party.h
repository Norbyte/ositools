#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>

namespace dse
{
    namespace esv
    {
        struct PartyNPCData
        {
            float IdentifyPriceModifier;
            float RepairPriceModifier;
            float ShopPriceModifier;
        };


        struct PartyGroup
        {
            ObjectSet<Character*> Characters;
            uint8_t CombatFormation;
            glm::vec3 InitialPlayerPos;
            glm::vec3 InitialEnemyPos;
            uint8_t CombatGroupID;
            ObjectSet<ObjectHandle> UnknownHandles;
        };


        struct PartyFormation
        {
            ObjectSet<ObjectHandle> Characters;
            uint8_t Unknown;
            uint8_t CombatId;
        };


        struct Party
        {
            void* VMT;
            FixedString GUID;
            int NetID;
            ObjectHandle Handle;
            ObjectSet<Character*> Players;
            ObjectSet<Character*> Characters;
            ObjectSet<PartyGroup*> Groups;
            ObjectSet<FixedString> ConsumedItems;
            ObjectSet<FixedString> UnlockedRecipes;
            RefMap<ObjectHandle, PartyNPCData> NPCData;
            ObjectSet<ObjectHandle> InventoryViews;
            ObjectSet<void*> Waypoints;
            ObjectSet<ObjectHandle> UsedPortals;
            uint64_t field_130[6];
            int field_160;
            int PartyExperience;
        };


        struct PartyManager : NetworkObjectFactory<Party, 1>
        {
            void* GameEventManagerVMT;
            __int64 field_120;
            void* EventListenerVMT;
            __int64 field_130;
            ObjectSet<Party*> Parties;
            RefMap<uint8_t, ObjectHandle> HotSeatSlotToPartyHandleMap;
            RefMap<int32_t, ObjectHandle> UserIdToPartyHandleMap;
            RefMap<int16_t, PartyFormation*> PartyFormations;
            ObjectSet<Character*> CurrentParty;
            ObjectSet<Character*> Characters;
            ObjectHandle GameMasterPartyHandle;
            FixedString TeleportTargetID;
            STDWString TeleportArriveEvent;
            int64_t TeleportNumRequest;
            float TeleportTimeout;
            int field_204;
            int64_t EndNumRequest;
            int EndTimeout;
        };

    }
}
#pragma once
