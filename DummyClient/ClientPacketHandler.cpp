#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}

	
}


struct BuffData
{
	uint64 buffId;
	float remainTime;
};

struct S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 att;
	vector<BuffData> buffs;
};

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	cout << "Packet ID : " << header.id << " Size : " << header.size << endl;
	uint64 id;
	uint32 hp;
	uint16 att;

	br >> id >> hp >> att;
	cout << "ID: " << id << " HP: " << hp << " ATT: " << att << endl;

	vector<BuffData> buffs;
	uint16 buffCount;
	br >> buffCount;

	buffs.resize(buffCount);
	for (int32 i = 0; i < buffCount; i++)
	{
		br >> buffs[i].buffId >> buffs[i].remainTime;
	}

	cout << "BuffCount : " << buffCount << endl;

	for (int32 i = 0; i < buffCount; i++)
	{
		cout << "BuffInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}

}
