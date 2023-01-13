#pragma once
#include <functional>


class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(const function<void(void)>& callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

	static void DoGlobalQueueWork();
private:
	Mutex			_lock;
	vector<thread>	_threads;
};

