#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"
#include "DBConnectionPool.h"
#include "DBBind.h"

enum
{
	WORKER_TICK = 64,
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DistuributeResevedJobs();

		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={MariaDB ODBC 3.1 Driver};Server=localhost;Database=ServerDB;Uid=root;Pwd=3784;"));

	// Create Table
	{
		auto query = L"							\
			DROP TABLE IF EXISTS Gold;";


		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}

	// Create Table
	{
		auto query = L"							\
			CREATE TABLE Gold					\
			(									\
				id int not null auto_increment, \
				gold int null,					\
				name NVARCHAR(50) NULL,			\
				createDate DATETIME NULL,		\
				primary key (id)				\
			);";


		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}

	// Add Data
	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		// 기존에 바인딩 된 정보 날림

		DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO Gold(gold, name, createDate) VALUES(?, ?, ?)");
		int32 gold = 100;
		WCHAR name[100] = L"호준";
		TIMESTAMP_STRUCT ts = {};
		ts.year = 2023;
		ts.month = 1;
		ts.day = 16;

		dbBind.BindParam(0, gold);
		dbBind.BindParam(1, name);
		dbBind.BindParam(2, ts);

		ASSERT_CRASH(dbBind.Execute());

		GDBConnectionPool->Push(dbConn);
	}

	// Read
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		// 기존에 바인딩 된 정보 날림
		DBBind<1, 4> dbBind(*dbConn, L"SELECT id, gold, name, createDate FROM Gold WHERE gold = (?)");
		int32 gold = 100;
		int32 outId = 0;
		int32 outGold = 0;
		WCHAR outName[100];
		TIMESTAMP_STRUCT outDate = {};
		dbBind.BindParam(0, gold);
		dbBind.BindCol(0, outId);
		dbBind.BindCol(1, outGold);
		dbBind.BindCol(2, outName);
		dbBind.BindCol(3, outDate);

		// SQL 실행
		ASSERT_CRASH(dbBind.Execute());

		wcout.imbue(locale("kor"));
		while (dbConn->Fetch())
		{
			wcout << "Id: " << outId << " Gold : " << outGold << " Name : " << outName << endl;
			wcout << "Date : " << outDate.year << " " << outDate.month << " " << outDate.day << endl;
		}

		GDBConnectionPool->Push(dbConn);


		
	}

	ClientPacketHandler::Init();

	cout << "Server" << endl;
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					DoWorkerJob(service);
				}
			});
	}

	DoWorkerJob(service);

	GThreadManager->Join();
}
