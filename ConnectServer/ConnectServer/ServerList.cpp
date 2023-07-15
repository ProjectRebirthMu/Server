// ServerList.cpp: implementation of the CServerList class.
// Revisado: 14/07/23 17:18 GMT-3
// By: Qubit
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerList.h"
#include "MemScript.h"
#include "pugixml.hpp"
#include "Util.h"

CServerList gServerList;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerList::CServerList() {
	m_JoinServerState = 0;
	m_JoinServerStateTime = 0;
	m_JoinServerQueueSize = 0;
	m_GameServerState = 0;
	m_ServerListInfo.clear();
}

CServerList::~CServerList() {
	m_ServerListInfo.clear();
}

void CServerList::Load(char* path) {
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);

	if (res.status != pugi::status_ok) {
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	m_ServerListInfo.clear();
	m_GameServersCount = 0;

	for (pugi::xml_node Server = file.child("ServerList").child("Server"); Server; Server = Server.next_sibling()) {
		SERVER_LIST_INFO info;

		info.ServerCode = Server.attribute("Code").as_int();
		strncpy(info.ServerAddress, Server.attribute("IP").as_string(), sizeof(info.ServerAddress));
		info.ServerPort = Server.attribute("Port").as_int();
		info.ServerShow = Server.attribute("Visible").as_bool();
		strncpy(info.ServerName, Server.attribute("Name").as_string(), sizeof(info.ServerName));

		info.ServerState = 0;
		info.ServerStateTime = 0;
		info.UserTotal = 0;
		info.UserCount = 0;
		info.AccountCount = 0;
		info.PCPointCount = 0;
		info.MaxUserCount = 0;

		m_ServerListInfo.insert({ info.ServerCode, info });

		m_GameServersCount++;
	}
}

void CServerList::MainProc() {
	if (m_JoinServerState != 0 && (GetTickCount() - m_JoinServerStateTime) > 10000) {
		m_JoinServerState = 0;
		m_JoinServerStateTime = 0;
		LogAdd(LOG_RED, "[ServerList] JoinServer offline");
	}

	for (auto it = m_ServerListInfo.begin(); it != m_ServerListInfo.end(); it++) {
		if (it->second.ServerState != 0 && (GetTickCount() - it->second.ServerStateTime) > 10000) {
			it->second.ServerState = 0;
			it->second.ServerStateTime = 0;
			LogAdd(LOG_BLACK, "[ServerList] GameServer offline (%s) (%d)", it->second.ServerName, it->second.ServerCode);
			m_GameServersCount--;
		}
	}
}

bool CServerList::CheckJoinServerState() {
	if (m_JoinServerState == 0) {
		return false;
	}

	if (m_JoinServerQueueSize > MAX_JOIN_SERVER_QUEUE_SIZE) {
		return false;
	}

	return true;
}

long CServerList::GenerateServerList(BYTE* lpMsg, int* size) {
	int count = 0;

	PMSG_SERVER_LIST info;

	if (this->CheckJoinServerState()) {
		for (auto it = this->m_ServerListInfo.begin(); it != this->m_ServerListInfo.end(); it++) {
			if (it->second.ServerShow && it->second.ServerState) {
				info.ServerCode = it->second.ServerCode;

				info.UserTotal = it->second.UserTotal;

				info.type = 0xCC;

				memcpy(&lpMsg[*size], &info, sizeof(info));
				*size += sizeof(info);

				count++;
			}
		}
	}

	return count;
}

SERVER_LIST_INFO* CServerList::GetServerListInfo(int ServerCode) {
	auto it = m_ServerListInfo.find(ServerCode);
	if (it == m_ServerListInfo.end()) {
		return nullptr;
	}
	else {
		return &it->second;
	}
}

void CServerList::ServerProtocolCore(BYTE head, BYTE* lpMsg, int size) {
	switch (head) {
	case 0x01:
		this->GCGameServerLiveRecv((SDHP_GAME_SERVER_LIVE_RECV*)lpMsg);
		break;
	case 0x02:
		this->JCJoinServerLiveRecv((SDHP_JOIN_SERVER_LIVE_RECV*)lpMsg);
		break;
	}
}

void CServerList::GCGameServerLiveRecv(SDHP_GAME_SERVER_LIVE_RECV* lpMsg) {
	SERVER_LIST_INFO* lpServerListInfo = this->GetServerListInfo(lpMsg->ServerCode);

	if (lpServerListInfo == nullptr) {
		return;
	}

	if (lpServerListInfo->ServerState == 0) {
		LogAdd(LOG_BLACK, "[ServerList] GameServer online (%s) (%d)", lpServerListInfo->ServerName, lpServerListInfo->ServerCode);
		this->m_GameServersCount++;
	}

	lpServerListInfo->ServerState = 1;
	lpServerListInfo->ServerStateTime = GetTickCount();
	lpServerListInfo->UserTotal = lpMsg->UserTotal;
	lpServerListInfo->UserCount = lpMsg->UserCount;
	lpServerListInfo->AccountCount = lpMsg->AccountCount;
	lpServerListInfo->PCPointCount = lpMsg->PCPointCount;
	lpServerListInfo->MaxUserCount = lpMsg->MaxUserCount;
}

void CServerList::JCJoinServerLiveRecv(SDHP_JOIN_SERVER_LIVE_RECV* lpMsg) {
	if (m_JoinServerState == 0) {
		LogAdd(LOG_GREEN, "[ServerList] JoinServer online");
	}

	m_JoinServerState = 1;
	m_JoinServerStateTime = GetTickCount();
	m_JoinServerQueueSize = lpMsg->QueueSize;
}
