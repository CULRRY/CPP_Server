#include "pch.h"
#include <iostream>

#include "ThreadManager.h"


void ThreadMain()
{
	cout << LThreadID << "\n";
	this_thread::sleep_for(1s);
}

int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadMain);
	}

	GThreadManager->Join();
}
