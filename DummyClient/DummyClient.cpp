#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"

char sendData[] = "asdsadsadasd";


class ServerSession : public PacketSession
{
protected:
	void OnConnected() override
	{
		cout << "Server Connected" << endl;
	}
	int32 OnRecvPacket(BYTE* buffer, int32 len) override
	{
		BufferReader br(buffer, 4096);

		PacketHeader header;
		br >> header;

		cout << "Packet ID : " << header.id << " Size : " << header.size << endl;
		uint64 id;
		uint32 hp;
		uint16 att;

		br >> id >> hp >> att;
		cout << "ID: " << id << " HP: " << hp << " ATT: " << att << endl;
		char recvBuffer[4096];
		br.Read(recvBuffer, header.size - sizeof(PacketHeader) - 8 - 4 - 2);
		cout << recvBuffer << endl;

		return len;
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
		1000);

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
