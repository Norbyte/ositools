#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(esv::lua::net)

void BroadcastMessage(char const* channel, char const* payload, std::optional<char const*> excludeCharacterGuid)
{
	esv::Character * excludeCharacter = nullptr;
	if (excludeCharacterGuid) {
		excludeCharacter = GetEntityWorld()->GetComponent<Character>(*excludeCharacterGuid);
	}

	auto & networkMgr = gExtender->GetServer().GetNetworkManager();
	auto msg = networkMgr.GetFreeMessage(ReservedUserId);
	if (msg != nullptr) {
		auto postMsg = msg->GetMessage().mutable_post_lua();
		postMsg->set_channel_name(channel);
		postMsg->set_payload(payload);
		if (excludeCharacter != nullptr) {
			networkMgr.Broadcast(msg, excludeCharacter->UserID);
		} else {
			networkMgr.Broadcast(msg, ReservedUserId);
		}
	}
}

void PostMessageToUserInternal(UserId userId, char const* channel, char const* payload)
{
	auto& networkMgr = gExtender->GetServer().GetNetworkManager();
	auto msg = networkMgr.GetFreeMessage(userId);
	if (msg != nullptr) {
		auto postMsg = msg->GetMessage().mutable_post_lua();
		postMsg->set_channel_name(channel);
		postMsg->set_payload(payload);
		networkMgr.Send(msg, userId);
	}
}

void PostMessageToClient(char const* characterGuid, char const* channel, char const* payload)
{
	auto character = GetEntityWorld()->GetComponent<Character>(characterGuid);
	if (character == nullptr) return;

	if (character->UserID == ReservedUserId) {
		OsiError("Attempted to send message to character " << characterGuid << " that has no user assigned!");
		return;
	}

	PostMessageToUserInternal(character->UserID, channel, payload);
}

void PostMessageToUser(int userId, char const* channel, char const* payload)
{
	if (UserId(userId) == ReservedUserId) {
		OsiError("Attempted to send message to reserved user ID!");
		return;
	}

	PostMessageToUserInternal(UserId(userId), channel, payload);
}

std::optional<bool> PlayerHasExtender(char const* characterGuid)
{
	auto character = GetEntityWorld()->GetComponent<Character>(characterGuid);
	if (character == nullptr || character->UserID == ReservedUserId) return {};

	// FIXME - access server from Lua context!
	auto& networkMgr = gExtender->GetServer().GetNetworkManager();
	return networkMgr.CanSendExtenderMessages(character->UserID.GetPeerId());
}

void RegisterNetLib()
{
	DECLARE_MODULE(Net, Server)
	BEGIN_MODULE()
	MODULE_FUNCTION(BroadcastMessage)
	MODULE_FUNCTION(PostMessageToClient)
	MODULE_FUNCTION(PostMessageToUser)
	MODULE_FUNCTION(PlayerHasExtender)
	END_MODULE()
}

END_NS()
