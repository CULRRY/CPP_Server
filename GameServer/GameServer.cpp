#include "pch.h"
#include <iostream>

#include "ThreadManager.h"
#include "Memory.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#include  "SocketUtils.h"
#pragma comment(lib, "ws2_32.lib")


int main()
{
	SOCKET socket = SocketUtils::CreateSocket();

	SocketUtils::BindAnyAddress(socket, 7777);

	SocketUtils::Listen(socket);

	SOCKET clientSocket = ::accept(socket, nullptr, nullptr);

	cout << "Client Connected!" << endl;

	while (true)
	{

	}

	GThreadManager->Join();
}
