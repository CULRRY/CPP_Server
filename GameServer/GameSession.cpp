#include "pch.h"
#include "GameSession.h"

#include "GameSessionManager.h"
#include "ServerPacketHandler.h"

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = PacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);


	ServerPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{

}

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}
