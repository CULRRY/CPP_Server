#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000
	};

public:
	Session();
	virtual ~Session();

public:
	void					Send(SendBufferRef sendBuffer);
	bool					Connect();
	void					Disconnect(const WCHAR* cause);
		
	shared_ptr<Service>		GetService() { return _service.lock(); }
	void					SerService(shared_ptr<Service> service) { _service = service; }

public:
	void					SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress				GetAddress() { return _netAddress; }
	SOCKET					GetSocket() { return _socket; }
	bool					IsConnected() { return _connected; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

private:
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) override;

private:
	bool					RegisterConnect();
	void					RegisterDisconnect();
	void					RegisterRecv();
	void					RegisterSend();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessRecv(int32 numOfBytes);
	void					ProcessSend(int32 numOfBytes);

	void					HandleError(int32 errorCode);

protected:
	virtual void			OnConnected() {}
	virtual int32			OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void			OnSend(int32 len) {}
	virtual void			OnDisconnected() {}

private:
	weak_ptr<Service>		_service;
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_netAddress = {};
	Atomic<bool>			_connected = false;

private:
	USE_LOCK;
	RecvBuffer				_recvBuffer;

	Queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegisterd{ false };

private:
	SendEvent				_sendEvent;
	RecvEvent				_recvEvent;
	ConnectEvent			_connectEvent;
	DisconnectEvent			_disconnectEvent;
};


struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32 OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;

};