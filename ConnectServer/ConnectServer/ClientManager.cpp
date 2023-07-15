// ClientManager.cpp: implementation of the CClientManager class.
// Revisado: 14/07/23 17:50 GMT-3
// By: Qubit
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientManager.h"
#include "ConnectServerProtocol.h"
#include "IpManager.h"
#include "Util.h"

CClientManager gClientManager[MAX_CLIENT];
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientManager::CClientManager() {
	m_index = -1;
	m_state = CLIENT_OFFLINE;
	m_socket = INVALID_SOCKET;
	m_IoRecvContext = 0;
	m_IoSendContext = 0;
	m_OnlineTime = 0;
	m_PacketTime = 0;
}

CClientManager::~CClientManager() {
	if (m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

bool CClientManager::CheckState() {
	if (CLIENT_RANGE(m_index) == 0 || m_state == CLIENT_OFFLINE || m_socket == INVALID_SOCKET) {
		return false;
	}
	else {
		return true;
	}
}

bool CClientManager::CheckAlloc() {
	if (m_IoRecvContext == 0 || m_IoSendContext == 0) {
		return false;
	}
	else {
		return true;
	}
}

bool CClientManager::CheckOnlineTime() {
	if ((GetTickCount() - m_OnlineTime) > MAX_ONLINE_TIME) {
		return false;
	}
	else {
		return true;
	}
}

void CClientManager::AddClient(int index, char* ip, SOCKET socket) {
	this->m_index = index;
	this->m_state = CLIENT_ONLINE;
	strcpy(this->m_IpAddr, ip);
	this->m_socket = socket;

	// Check if there is enough memory to allocate for the I/O receive and send contexts.
	if (this->CheckAlloc() == 0) {
		if (gClientCount >= MAX_CLIENT) {
			gClientCount = 0;
		}
		else {
			gClientCount++;
		}
	}

	// Create new I/O receive and send contexts for the client.
	this->m_IoRecvContext = new IO_RECV_CONTEXT;
	this->m_IoSendContext = new IO_SEND_CONTEXT;

	// Initialize the I/O receive context.
	memset(&this->m_IoRecvContext->overlapped, 0, sizeof(this->m_IoRecvContext->overlapped));
	this->m_IoRecvContext->wsabuf.buf = (char*)this->m_IoRecvContext->IoMainBuffer.buff;
	this->m_IoRecvContext->wsabuf.len = MAX_MAIN_PACKET_SIZE;
	this->m_IoRecvContext->IoType = IO_RECV;
	this->m_IoRecvContext->IoSize = 0;
	this->m_IoRecvContext->IoMainBuffer.size = 0;

	// Initialize the I/O send context.
	memset(&this->m_IoSendContext->overlapped, 0, sizeof(this->m_IoSendContext->overlapped));
	this->m_IoSendContext->wsabuf.buf = (char*)this->m_IoSendContext->IoMainBuffer.buff;
	this->m_IoSendContext->wsabuf.len = MAX_MAIN_PACKET_SIZE;
	this->m_IoSendContext->IoType = IO_SEND;
	this->m_IoSendContext->IoSize = 0;
	this->m_IoSendContext->IoMainBuffer.size = 0;
	this->m_IoSendContext->IoSideBuffer.size = 0;

	// Set the online time and packet time.
	this->m_OnlineTime = GetTickCount();
	this->m_PacketTime = 0;

	// Insert the IP address of the client into the IP address manager.
	gIpManager.InsertIpAddress(this->m_IpAddr);

	// Initialize the send function for the client.
	CCServerInitSend(this->m_index, 1);
}

void CClientManager::DelClient() {
	gIpManager.RemoveIpAddress(this->m_IpAddr);
	this->m_index = -1;
	this->m_state = CLIENT_OFFLINE;
	memset(this->m_IpAddr, 0, sizeof(this->m_IpAddr));
	this->m_socket = INVALID_SOCKET;
	this->m_OnlineTime = GetTickCount();
	this->m_PacketTime = 0;
}
