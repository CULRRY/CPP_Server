#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"


int main()
{
	cout << "Server" << endl;
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100);

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

	char sendData[] = "Hello world";

	while (true)
	{
		Protocol::S_TEST pkt;
		pkt.set_id(1001);
		pkt.set_hp(300);
		pkt.set_attack(11);

		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(100);
			data->set_remaintime(1.2f);
			data->add_victims(400);
		}

		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(500);
			data->set_remaintime(2.3f);
			data->add_victims(1000);
			data->add_victims(2000);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		GSessionManager.Broadcast(sendBuffer);
		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}
