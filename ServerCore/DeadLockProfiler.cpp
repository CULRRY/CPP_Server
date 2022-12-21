#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	int32 lockID = 0;

	const auto findIt = _nameToID.find(name);
	if (findIt == _nameToID.end())
	{
		lockID = static_cast<int32>(_nameToID.size());
		_nameToID[name] = lockID;
		_idToName[lockID] = name;
	}
	else
	{
		lockID = findIt->second;
	}

	if (_lockStack.empty() == false)
	{
		const int32 prevID = _lockStack.top();
		if (lockID != prevID)
		{
			set<int32>& history = _lockHistory[prevID];
			if (history.contains(lockID) == false)
			{
				history.insert(lockID);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockID);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	const int32 lockID = _nameToID[name];
	if (_lockStack.top() != lockID)
		CRASH("INVALID_UNLOCK");

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToID.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockID = 0; lockID < lockCount; lockID++)
	{
		dfs(lockID);
	}

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::dfs(const int32 here)
{
	if (_discoveredOrder[here] != -1)
		return;

	_discoveredOrder[here] = _discoveredCount++;

	const auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	const set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			dfs(there);
			continue;
		}

		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
					break;
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	_finished[here] = true;

}
