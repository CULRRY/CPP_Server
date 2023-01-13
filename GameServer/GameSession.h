#pragma once
#include "Session.h"


class GameSession : public PacketSession
{
public:
	~GameSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;

public:
	Vector<PlayerRef> _players;

	PlayerRef _currentPlayer;
	weak_ptr<class Room> _room;
};

