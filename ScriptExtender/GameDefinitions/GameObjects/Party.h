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
            ObjectSet<ComponentHandle> UnknownHandles;
        };


        struct PartyFormation
        {
            ObjectSet<ComponentHandle> Characters;
            uint8_t Unknown;
            uint8_t CombatId;
        };


        struct Party : public ProtectedGameObject<Party>
        {
            void* VMT;
            FixedString GUID;
            int NetID;
            ComponentHandle Handle;
            ObjectSet<Character*> Players;
            ObjectSet<Character*> Characters;
            ObjectSet<PartyGroup*> Groups;
            ObjectSet<FixedString> ConsumedItems;
            ObjectSet<FixedString> UnlockedRecipes;
            RefMap<ComponentHandle, PartyNPCData> NPCData;
            ObjectSet<ComponentHandle> InventoryViews;
            ObjectSet<void*> Waypoints;
            ObjectSet<ComponentHandle> UsedPortals;
            uint64_t field_130[6];
            int field_160;
            int PartyExperience;
        };


        struct PartyManager : NetworkComponentFactory<Party>
        {
            void* GameEventManagerVMT;
            __int64 field_120;
            void* EventListenerVMT;
            __int64 field_130;
            ObjectSet<Party*> Parties;
            RefMap<uint8_t, ComponentHandle> HotSeatSlotToPartyHandleMap;
            RefMap<int32_t, ComponentHandle> UserIdToPartyHandleMap;
            RefMap<int16_t, PartyFormation*> PartyFormations;
            ObjectSet<Character*> CurrentParty;
            ObjectSet<Character*> Characters;
            ComponentHandle GameMasterPartyHandle;
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
