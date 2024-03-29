#pragma once

#include "Types.h"
#include "CoreGlobal.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "Container.h"

#include <windows.h>
#include <iostream>
using namespace std;

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Lock.h"
#include "ObjectPool.h"
#include "Memory.h"
#include "SendBuffer.h"
#include "Session.h"

#include "LockQueue.h"