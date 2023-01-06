﻿#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

char sendData[] = "asdsadsadasd";

class ServerSession : public Session
{
protected:
	void OnConnected() override
	{
		cout << "Server Connected" << endl;
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(sendData, sizeof(sendData));
		Send(sendBuffer);
	}
	int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "On Recv Len = " << len << endl;

		this_thread::sleep_for(1s);
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
		sendBuffer->CopyData(sendData, sizeof(sendData));
		Send(sendBuffer);
		return len;
	}
	void OnSend(int32 len) override
	{
		cout << "On Send Len = " << len << endl;
	}
	void OnDisconnected() override
	{
		cout << "Server Disconnected" << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	cout << "Client" << endl;

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		5);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}
