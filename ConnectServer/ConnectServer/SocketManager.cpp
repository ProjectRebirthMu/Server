// SocketManager.cpp: implementation of the CSocketManager class.
// Revisado: 14/07/23 15:18 GMT-3
// By: Qubit
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketManager.h"
#include "ClientManager.h"
#include "ConnectServerProtocol.h"
#include "IpManager.h"
#include "Protect.h"
#include "Util.h"

CSocketManager gSocketManager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketManager::CSocketManager()
	: m_listen(INVALID_SOCKET),
	m_CompletionPort(0),
	m_port(0),
	m_ServerAcceptThread(0),
	m_ServerWorkerThreadCount(0),
	m_ServerQueueSemaphore(CreateSemaphore(NULL, 0, MAX_QUEUE_SIZE, NULL)),
	m_ServerQueueThread(0)
{
	for (int n = 0; n < MAX_SERVER_WORKER_THREAD; n++)
	{
		this->m_ServerWorkerThread[n] = 0;
	}

	if (m_ServerQueueSemaphore == NULL)
	{
		LogAdd(LOG_RED, "[SocketManager] Erro ao criar o semáforo de fila do servidor: %d", WSAGetLastError());
	}
}

CSocketManager::~CSocketManager() // OK
{
	this->Clean();
}

bool CSocketManager::Start(WORD port)
{
	// Protect the following code
	// from being interrupted
	// by a SIGSEGV
	__try
	{
		this->m_port = port;

		if (this->CreateListenSocket() == 0 ||
			this->CreateCompletionPort() == 0 ||
			this->CreateAcceptThread() == 0 ||
			this->CreateWorkerThread() == 0 ||
			this->CreateServerQueue() == 0)
		{
			this->Clean();
			return false;
		}

		LogAdd(LOG_BLUE, "O servidor foi iniciado na porta [%d].", this->m_port);
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		// Log the error and exit
		LogAdd(LOG_RED, "O servidor falhou ao iniciar na porta [%d].", port);
		return false;
	}
}

void CSocketManager::Clean()
{
	if (m_ServerQueueThread != 0)
	{
		WaitForSingleObject(m_ServerQueueThread, INFINITE);
		CloseHandle(m_ServerQueueThread);
		m_ServerQueueThread = 0;
	}

	if (m_ServerQueueSemaphore != 0)
	{
		CloseHandle(m_ServerQueueSemaphore);
		m_ServerQueueSemaphore = 0;
	}

	m_ServerQueue.ClearQueue();

	for (DWORD n = 0; n < MAX_SERVER_WORKER_THREAD; n++)
	{
		if (m_ServerWorkerThread[n] != 0)
		{
			WaitForSingleObject(m_ServerWorkerThread[n], INFINITE);
			CloseHandle(m_ServerWorkerThread[n]);
			m_ServerWorkerThread[n] = 0;
		}
	}

	if (m_ServerAcceptThread != 0)
	{
		WaitForSingleObject(m_ServerAcceptThread, INFINITE);
		CloseHandle(m_ServerAcceptThread);
		m_ServerAcceptThread = 0;
	}

	if (m_CompletionPort != 0)
	{
		CloseHandle(m_CompletionPort);
		m_CompletionPort = 0;
	}

	if (m_listen != INVALID_SOCKET)
	{
		closesocket(m_listen);
		m_listen = INVALID_SOCKET;
	}
}

bool CSocketManager::CreateListenSocket() // OK
{
	if ((this->m_listen = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		LogAdd(LOG_RED, "[SocketManager] WSASocketW() failed with error: %d", WSAGetLastError());
		return false;
	}

	SOCKADDR_IN SocketAddr;
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	SocketAddr.sin_port = htons(this->m_port);

	if (bind(this->m_listen, (sockaddr*)&SocketAddr, sizeof(SocketAddr)) == SOCKET_ERROR) {
		LogAdd(LOG_RED, "[SocketManager] bind() failed with error: %d", WSAGetLastError());
		return false;
	}

	if (listen(this->m_listen, 5) == SOCKET_ERROR) {
		LogAdd(LOG_RED, "[SocketManager] listen() failed with error: %d", WSAGetLastError());
		return false;
	}

	return true;
}

bool CSocketManager::CreateCompletionPort()
{
	SOCKET socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET)
	{
		LogAdd(LOG_RED, "Erro: Não foi possível criar o socket. Código de erro: %d", WSAGetLastError());
		return false;
	}

	HANDLE completionPort = CreateIoCompletionPort((HANDLE)socket, NULL, 0, 0);
	if (completionPort == NULL)
	{
		LogAdd(LOG_RED, "[SocketManager] Falha ao criar a porta de conclusão de E/S com código de erro: %d", GetLastError());
		closesocket(socket);
		return false;
	}

	m_CompletionPort = completionPort;

	return true;
}

bool CSocketManager::CreateAcceptThread()
{
	HANDLE thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)this->ServerAcceptThread, this, 0, 0);
	if (thread == NULL)
	{
		LogAdd(LOG_RED, "[SocketManager] Falha ao criar thread com código de erro: %d", GetLastError());
		return false;
	}

	if (SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST) == 0)
	{
		LogAdd(LOG_RED, "[SocketManager] Falha ao definir a prioridade da thread. Código de erro: %d", GetLastError());
		CloseHandle(thread);
		return false;
	}

	return true;
}

bool CSocketManager::CreateWorkerThread()
{
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	DWORD workerThreadCount = min(SystemInfo.dwNumberOfProcessors, MAX_SERVER_WORKER_THREAD);

	for (DWORD n = 0; n < workerThreadCount; n++)
	{
		HANDLE workerThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)this->ServerWorkerThread, this, 0, 0);
		if (workerThread == NULL)
		{
			LogAdd(LOG_RED, "[SocketManager] Falha ao criar thread de trabalhador com código de erro: %d", GetLastError());
			return false;
		}

		if (SetThreadPriority(workerThread, THREAD_PRIORITY_HIGHEST) == 0)
		{
			LogAdd(LOG_RED, "[SocketManager] Falha ao definir prioridade da thread de trabalho com o código de erro: %d", GetLastError());
			CloseHandle(workerThread);
			return false;
		}

		this->m_ServerWorkerThread[n] = workerThread;
	}

	return true;
}

bool CSocketManager::CreateServerQueue()
{
	HANDLE semaphore = CreateSemaphore(0, 0, MAX_QUEUE_SIZE, 0);
	if (semaphore == NULL)
	{
		LogAdd(LOG_RED, "[SocketManager] CreateSemaphore() código de erro: %d", GetLastError());
		return false;
	}

	HANDLE thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)this->ServerQueueThread, this, 0, 0);
	if (thread == NULL)
	{
		LogAdd(LOG_RED, "[SocketManager] CreateThread() código de erro: %d", GetLastError());
		CloseHandle(semaphore);
		return false;
	}

	if (SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST) == 0)
	{
		LogAdd(LOG_RED, "[SocketManager] SetThreadPriority() código de erro: %d", GetLastError());
		CloseHandle(semaphore);
		CloseHandle(thread);
		return false;
	}

	this->m_ServerQueueSemaphore = semaphore;
	this->m_ServerQueueThread = thread;

	return true;
}

bool CSocketManager::DataRecv(int index, IO_MAIN_BUFFER* lpIoBuffer)
{
	if (lpIoBuffer->size < 3) {
		return true;
	}

	BYTE* lpMsg = lpIoBuffer->buff;
	int count = 0;

	while (true) {
		BYTE header = lpMsg[count];
		BYTE* pSize = lpMsg + count + 1;
		int size = (header == 0xC1) ? *pSize : MAKEWORD(pSize[1], pSize[0]);
		BYTE head = lpMsg[count + 2 + (header == 0xC2)];

		if (size < 3 || size > MAX_MAIN_PACKET_SIZE) {
			LogAdd(LOG_RED, "[SocketManager] Erro de tamanho do protocolo (Índice: %d, Cabeçalho: %x, Tamanho: %d, Cabeça: %x)", index, header, size, head);
			return false;
		}

		if (size <= lpIoBuffer->size) {
			QUEUE_INFO queueInfo;
			queueInfo.index = index;
			queueInfo.head = head;
			memcpy(queueInfo.buff, &lpMsg[count], size);
			queueInfo.size = size;

			if (this->m_ServerQueue.AddToQueue(&queueInfo) != 0) {
				ReleaseSemaphore(this->m_ServerQueueSemaphore, 1, 0);
			}

			count += size;
			lpIoBuffer->size -= size;

			if (lpIoBuffer->size <= 0) {
				break;
			}
		}
		else {
			if (count > 0 && lpIoBuffer->size > 0 && lpIoBuffer->size <= (MAX_MAIN_PACKET_SIZE - count)) {
				memmove(lpMsg, &lpMsg[count], lpIoBuffer->size);
			}
			break;
		}
	}

	return true;
}

bool CSocketManager::DataSend(int index, BYTE* lpMsg, int size) // OK
{
	if (!CLIENT_RANGE(index))
	{
		return false;
	}

	CClientManager* lpClientManager = &gClientManager[index];

	if (lpClientManager->CheckState() == 0)
	{
		return false;
	}

	if (size > MAX_MAIN_PACKET_SIZE)
	{
		LogAdd(LOG_RED, "[SocketManager] Tamanho máximo da mensagem atingido (Tipo: 1, Índice: %d, Tamanho: %d)", index, size);
		return false;
	}

	IO_SEND_CONTEXT* lpIoContext = lpClientManager->m_IoSendContext;

	if (lpIoContext->IoSize > 0)
	{
		if ((lpIoContext->IoSideBuffer.size + size) > MAX_SIDE_PACKET_SIZE)
		{
			LogAdd(LOG_RED, "[SocketManager] Tamanho máximo da mensagem atingido (Tipo: 2, Índice: %d, Tamanho: %d)", index, (lpIoContext->IoSideBuffer.size + size));
			this->Disconnect(index);
			return false;
		}

		memcpy(&lpIoContext->IoSideBuffer.buff[lpIoContext->IoSideBuffer.size], lpMsg, size);
		lpIoContext->IoSideBuffer.size += size;
		return true;
	}

	memcpy(lpIoContext->IoMainBuffer.buff, lpMsg, size);
	lpIoContext->wsabuf.buf = (char*)lpIoContext->IoMainBuffer.buff;
	lpIoContext->wsabuf.len = size;
	lpIoContext->IoType = IO_SEND;
	lpIoContext->IoSize = size;
	lpIoContext->IoMainBuffer.size = 0;

	DWORD SendSize = 0, Flags = 0;
	if (WSASend(lpClientManager->m_socket, &lpIoContext->wsabuf, 1, &SendSize, Flags, &lpIoContext->overlapped, 0) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			LogAdd(LOG_RED, "[SocketManager] WSASend() falhou com erro: %d", WSAGetLastError());
			this->Disconnect(index);
			return false;
		}
	}

	return true;
}

void CSocketManager::Disconnect(int index) // OK
{
	this->m_critical.lock();
	if (!CLIENT_RANGE(index)) {
		this->m_critical.unlock();
		return;
	}

	CClientManager* lpClientManager = &gClientManager[index];

	if (!lpClientManager->CheckState()) {
		this->m_critical.unlock();
		return;
	}

	// shutdown socket to prevent further sends/receives
	shutdown(lpClientManager->m_socket, SD_BOTH);

	// close socket
	if (closesocket(lpClientManager->m_socket) == SOCKET_ERROR) {
		LogAdd(LOG_RED, "[SocketManager] Falha ao fechar o socket com erro: %d", WSAGetLastError());
	}

	lpClientManager->DelClient();

	this->m_critical.unlock();
}

void CSocketManager::OnRecv(int index, DWORD IoSize, IO_RECV_CONTEXT* lpIoContext) // OK
{
	this->m_critical.lock();
	if (!CLIENT_RANGE(index)) {
		this->m_critical.unlock();
		return;
	}

	if (IoSize == 0) {
		this->Disconnect(index);
		this->m_critical.unlock();
		return;
	}

	CClientManager* lpClientManager = &gClientManager[index];

	lpIoContext->IoMainBuffer.size += IoSize;

	if (this->DataRecv(index, &lpIoContext->IoMainBuffer) == 0) {
		this->Disconnect(index);
		this->m_critical.unlock();
		return;
	}

	lpIoContext->wsabuf.buf = (CHAR*)&lpIoContext->IoMainBuffer.buff[lpIoContext->IoMainBuffer.size];
	lpIoContext->wsabuf.len = MAX_MAIN_PACKET_SIZE - lpIoContext->IoMainBuffer.size;
	lpIoContext->IoType = IO_RECV;

	DWORD RecvSize = 0, Flags = 0;
	int result = WSARecv(lpClientManager->m_socket, &lpIoContext->wsabuf, 1, &RecvSize, &Flags, &lpIoContext->overlapped, 0);
	if (result == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			LogAdd(LOG_RED, "[SocketManager] WSARecv() falhou com erro: %d", errorCode);
			this->Disconnect(index);
			this->m_critical.unlock();
			return;
		}
	}

	this->m_critical.unlock();
}

void CSocketManager::OnSend(int index, DWORD IoSize, IO_SEND_CONTEXT* lpIoContext)
{
	this->m_critical.lock();
	if (!CLIENT_RANGE(index)) {
		this->m_critical.unlock();
		return;
	}

	if (IoSize == 0) {
		this->Disconnect(index);
		this->m_critical.unlock();
		return;
	}

	CClientManager* lpClientManager = &gClientManager[index];

	lpIoContext->IoMainBuffer.size += IoSize;

	if (lpIoContext->IoMainBuffer.size >= lpIoContext->IoSize) {
		if (lpIoContext->IoSideBuffer.size <= 0) {
			lpIoContext->IoSize = 0;
			this->m_critical.unlock();
			return;
		}

		// Move the data from the side buffer to the main buffer if the main buffer is full.
		if (lpIoContext->IoSideBuffer.size > MAX_MAIN_PACKET_SIZE) {
			memcpy(lpIoContext->IoMainBuffer.buff, lpIoContext->IoSideBuffer.buff, MAX_MAIN_PACKET_SIZE);
			lpIoContext->wsabuf.buf = (char*)lpIoContext->IoMainBuffer.buff;
			lpIoContext->wsabuf.len = MAX_MAIN_PACKET_SIZE;
			lpIoContext->IoType = IO_SEND;
			lpIoContext->IoSize = MAX_MAIN_PACKET_SIZE;
			lpIoContext->IoMainBuffer.size = 0;
			memmove(lpIoContext->IoSideBuffer.buff, &lpIoContext->IoSideBuffer.buff[MAX_MAIN_PACKET_SIZE], (lpIoContext->IoSideBuffer.size - MAX_MAIN_PACKET_SIZE));
			lpIoContext->IoSideBuffer.size -= MAX_MAIN_PACKET_SIZE;
		}
		else {
			// Send the data from the side buffer if it is not full.
			memcpy(lpIoContext->IoMainBuffer.buff, lpIoContext->IoSideBuffer.buff, lpIoContext->IoSideBuffer.size);
			lpIoContext->wsabuf.buf = (char*)lpIoContext->IoMainBuffer.buff;
			lpIoContext->wsabuf.len = lpIoContext->IoSideBuffer.size;
			lpIoContext->IoType = IO_SEND;
			lpIoContext->IoSize = lpIoContext->IoSideBuffer.size;
			lpIoContext->IoMainBuffer.size = 0;
			lpIoContext->IoSideBuffer.size = 0;
		}
	}
	else {
		// Send the data from the main buffer.
		lpIoContext->wsabuf.buf = (char*)&lpIoContext->IoMainBuffer.buff[lpIoContext->IoMainBuffer.size];
		lpIoContext->wsabuf.len = lpIoContext->IoSize - lpIoContext->IoMainBuffer.size;
		lpIoContext->IoType = IO_SEND;
	}

	DWORD SendSize = 0, Flags = 0;
	int result = WSASend(lpClientManager->m_socket, &lpIoContext->wsabuf, 1, &SendSize, Flags, &lpIoContext->overlapped, 0);

	if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
		int errorCode = WSAGetLastError();
		LogAdd(LOG_RED, "[SocketManager] Falha no WSASend() com erro: %d", errorCode);
		this->Disconnect(index);
	}

	this->m_critical.unlock();
}

int CALLBACK CSocketManager::ServerAcceptCondition(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS, IN LPWSABUF lpCalleeId, OUT LPWSABUF lpCalleeData, OUT GROUP FAR* g, CSocketManager* lpSocketManager) {
	SOCKADDR_IN* SocketAddr = (SOCKADDR_IN*)lpCallerId->buf;
	if (gIpManager.CheckIpAddress(inet_ntoa(SocketAddr->sin_addr)) == 0) {
		return CF_REJECT;
	}
	else {
		return CF_ACCEPT;
	}
}

DWORD WINAPI CSocketManager::ServerAcceptThread(CSocketManager* lpSocketManager)
{
	SOCKADDR_IN SocketAddr;
	int SocketAddrSize = sizeof(SocketAddr);
	while (true) {
		SOCKET socket = WSAAccept(lpSocketManager->m_listen, (sockaddr*)&SocketAddr, &SocketAddrSize, (LPCONDITIONPROC)&CSocketManager::ServerAcceptCondition, (DWORD)lpSocketManager);
		if (socket == INVALID_SOCKET) {
			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				lpSocketManager->m_critical.lock();
				LogAdd(LOG_RED, "[SocketManager] WSAAccept() failed with error: %d", WSAGetLastError());
				lpSocketManager->m_critical.unlock();
			}
			continue;
		}

		lpSocketManager->m_critical.lock();

		int index = -1;
		if ((index = GetFreeClientIndex()) == -1) {
			closesocket(socket);
			lpSocketManager->m_critical.unlock();
			continue;
		}

		if (CreateIoCompletionPort((HANDLE)socket, lpSocketManager->m_CompletionPort, index, 0) == NULL) {
			lpSocketManager->m_critical.unlock();
			LogAdd(LOG_RED, "[SocketManager] CreateIoCompletionPort() failed with error: %d", GetLastError());
			closesocket(socket);
			continue;
		}

		CClientManager* lpClientManager = &gClientManager[index];
		lpClientManager->AddClient(index, inet_ntoa(SocketAddr.sin_addr), socket);

		DWORD RecvSize = 0, Flags = 0;
		if (WSARecv(socket, &lpClientManager->m_IoRecvContext->wsabuf, 1, &RecvSize, &Flags, &lpClientManager->m_IoRecvContext->overlapped, 0) == SOCKET_ERROR) {
			if (WSAGetLastError() != WSA_IO_PENDING) {
				LogAdd(LOG_RED, "[SocketManager] WSARecv() failed with error: %d", WSAGetLastError());
				lpSocketManager->Disconnect(index);
			}
		}

		lpSocketManager->m_critical.unlock();
	}

	return 0;
}

DWORD WINAPI CSocketManager::ServerWorkerThread(CSocketManager* lpSocketManager)
{
	DWORD IoSize;
	DWORD index;
	LPOVERLAPPED lpOverlapped;

	while (true) {
		if (GetQueuedCompletionStatus(lpSocketManager->m_CompletionPort, &IoSize, &index, &lpOverlapped, INFINITE) == 0) {
			if (lpOverlapped == NULL || (GetLastError() != ERROR_NETNAME_DELETED && GetLastError() != ERROR_CONNECTION_ABORTED && GetLastError() != ERROR_OPERATION_ABORTED && GetLastError() != ERROR_SEM_TIMEOUT)) {
				lpSocketManager->m_critical.lock();
				LogAdd(LOG_RED, "[SocketManager] GetQueuedCompletionStatus() failed with error: %d", GetLastError());
				lpSocketManager->m_critical.unlock();
				return 0;
			}
		}

		lpSocketManager->m_critical.lock();

		if (IoSize == 0 && index == 0 && lpOverlapped == 0) {
			lpSocketManager->m_critical.unlock();
			return 0;
		}

		IO_CONTEXT* lpIoContext = (IO_CONTEXT*)lpOverlapped;

		switch (lpIoContext->IoType) {
		case IO_RECV:
			lpSocketManager->OnRecv(index, IoSize, (IO_RECV_CONTEXT*)lpIoContext);
			break;
		case IO_SEND:
			lpSocketManager->OnSend(index, IoSize, (IO_SEND_CONTEXT*)lpIoContext);
			break;
		}

		lpSocketManager->m_critical.unlock();
	}

	return 0;
}

DWORD WINAPI CSocketManager::ServerQueueThread(CSocketManager* lpSocketManager)
{
	while (true) {
		if (WaitForSingleObject(lpSocketManager->m_ServerQueueSemaphore, INFINITE) == WAIT_FAILED) {
			LogAdd(LOG_RED, "[SocketManager] WaitForSingleObject() failed with error: %d", GetLastError());
			break;
		}

		static QUEUE_INFO QueueInfo;

		if (lpSocketManager->m_ServerQueue.GetFromQueue(&QueueInfo) != 0) {
			if (CLIENT_RANGE(QueueInfo.index) != 0 && gClientManager[QueueInfo.index].CheckState() != 0) {
				ConnectServerProtocolCore(QueueInfo.index, QueueInfo.head, QueueInfo.buff, QueueInfo.size);
			}
		}
	}

	return 0;
}

DWORD CSocketManager::GetQueueSize()
{
	return m_ServerQueue.GetQueueSize();
}
