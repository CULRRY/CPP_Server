#include "pch.h"
#include "Lock.h"

#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{
#ifdef _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	const uint32 lockThreadID = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (lockThreadID == LThreadID)
	{
		_writeCount++;
		return;
	}

	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadID << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(expected, desired))
			{
				_writeCount++;
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#ifdef _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_MASK");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#ifdef _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif

	const uint32 lockThreadID = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (lockThreadID == LThreadID)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	const int64 beginTick = ::GetTickCount64();
	while(true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(expected, expected + 1))
			{
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}

}

void Lock::ReadUnlock(const char* name)
{
#ifdef _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif

	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
