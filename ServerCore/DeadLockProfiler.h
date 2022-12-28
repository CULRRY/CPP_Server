#pragma once
#include <map>
#include <set>
#include <vector>


class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void dfs(int32 here);

private:
	unordered_map<const char*, int32>	_nameToID;
	unordered_map<int32, const char*>	_idToName;
	stack<int32>						_lockStack;
	map<int32, set<int32>>				_lockHistory;

	Mutex _lock;

private:
	vector<int32>	_discoveredOrder;
	int32			_discoveredCount = 0;
	vector<bool>	_finished;
	vector<int>		_parent;

};
