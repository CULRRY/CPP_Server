#include "pch.h"
#include <iostream>

#include "ThreadManager.h"

class LockTest
{
	USE_LOCK;

public:
	int32 TestRead()
	{
		READ_LOCK;

		TestPush();

		if (_queue.empty())
			return -1;
		return _queue.front();
	}

	void TestPush()
	{
		WRITE_LOCK;
		_queue.push(rand() % 100);
	}

	void TestPop()
	{
		WRITE_LOCK;
		if (!_queue.empty())
			_queue.pop();
	}

private:
	queue<int32> _queue;
};


LockTest testLock;

void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{

		int32 val = testLock.TestRead();
		cout << val << endl;
		this_thread::sleep_for(1ms);
	}
}

int main()
{
	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}

	GThreadManager->Join();
}
