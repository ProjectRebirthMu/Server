// GameMaster.h: interface for the CGameMaster class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "User.h"

#define MAX_GAME_MASTER 100

struct GAME_MASTER_INFO
{
	char Account[11];
	char Name[11];
	BYTE Level;
};

class CGameMaster
{
public:
	CGameMaster();
	virtual ~CGameMaster();
	void Load(char* path);
	void SetGameMasterLevel(LPOBJ lpObj, int level);
	bool CheckGameMasterLevel(LPOBJ lpObj, int level);
private:
	std::map<std::string, GAME_MASTER_INFO> m_GameMasterInfo;
	int m_count;
};

extern CGameMaster gGameMaster;
