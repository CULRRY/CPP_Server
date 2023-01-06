#pragma once
#include "Session.h"


class GameSession : public Session
{
public:
	~GameSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;
	virtual void OnConnected() override;
	virtual void OnDisconnected() override;

public:
};

