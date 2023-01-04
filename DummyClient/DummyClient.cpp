#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"

char sendBuffer[] = "asdsadsadasd";

class ServerSession : public Session
{
protected:
	void OnConnected() override
	{
		cout << "Server Connected" << endl;
		Send(reinterpret_cast<BYTE*>(sendBuffer), sizeof(sendBuffer));
	}
	int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "On Recv Len = " << len << endl;

		this_thread::sleep_for(1s);
		Send(reinterpret_cast<BYTE*>(sendBuffer), sizeof(sendBuffer));
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
		2);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
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
