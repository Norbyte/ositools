#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>

BEGIN_SE()

struct PlayerManager : public ProtectedGameObject<PlayerManager>
{
	struct PlayerData
	{
		EInputPlayerIndex InputPlayerIndex;
		void* RenderView;
	};

	void* VMT;
	PlayerId NextPlayerId;
	ObjectSet<PlayerId> FreePlayerIds;
	ObjectSet<PlayerId> PlayerIds;
	RefMap<PlayerId, PlayerData> Players;
	RefMap<EInputPlayerIndex, PlayerId> InputPlayerIndexToPlayerId;
	ObjectSet<void*> RenderViews;
};


END_SE()


BEGIN_NS(ecl)

struct PlayerManager : public dse::PlayerManager
{
	struct ClientPlayerInfo
	{
		NetId CharacterNetId;
		FixedString ProfileGuid;
		FixedString CameraControllerId;
	};

	void* VMT_IProfileSelector;
	void* VMT_EventListener;
	void* VMT_PlayerManager2;
	RefMap<PlayerId, ClientPlayerInfo> ClientPlayerData;
	RefMap<PlayerId, NetId> PlayerIdToNetId;
	ObjectSet<EInputPlayerIndex> LinkedDevices;
	EInputPlayerIndex CurrentInputPlayerIndex;
	__int16 field_EC;
	ObjectSet<PlayerId> ConnectedPlayers;
	int field_110;
	bool IsDisconnecting;
	PlayerId DebugLocalPlayerId;
	std::array<PlayerId, 3> DebugPlayerIds;
	std::array<FixedString, 3> DebugPlayerProfileGuids;
	uint64_t NextLocalPlayerId;
};


END_NS()
