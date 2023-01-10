#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ClientPacketHandler.h"

//WCHAR sendData[] = "asdsadsadasd";


class ServerSession : public PacketSession
{
protected:
	void OnConnected() override
	{
		cout << "Server Connected" << endl;
	}
	void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		ClientPacketHandler::HandlePacket(buffer, len);
	}
	void OnSend(int32 len) override
	{
		//cout << "On Send Len = " << len << endl;
	}
	void OnDisconnected() override
	{
		//cout << "Server Disconnected" << endl;
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
		1);

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
