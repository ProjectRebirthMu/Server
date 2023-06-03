// GameMaster.cpp: implementation of the CGameMaster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameMaster.h"
#include "MemScript.h"
#include "Util.h"

CGameMaster gGameMaster;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameMaster::CGameMaster() // OK
{
	this->m_count = 0;
}

CGameMaster::~CGameMaster() // OK
{

}

void CGameMaster::Load(char* path) // OK
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);

	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	pugi::xml_node GameMasterList = file.child("GameMasterList");

	for (pugi::xml_node GameMaster = GameMasterList.child("GameMaster"); GameMaster; GameMaster = GameMaster.next_sibling())
	{
		GAME_MASTER_INFO info;

		strcpy_s(info.Account, GameMaster.attribute("Account").as_string());

		strcpy_s(info.Name, GameMaster.attribute("Character").as_string());

		info.Level = GameMaster.attribute("Level").as_int();

		this->m_GameMasterInfo.insert(std::pair<std::string, GAME_MASTER_INFO>(info.Name, info));
	}
}

void CGameMaster::SetGameMasterLevel(LPOBJ lpObj, int level) // OK
{
	GAME_MASTER_INFO info;

	memcpy(info.Account, lpObj->Account, sizeof(info.Account));

	memcpy(info.Name, lpObj->Name, sizeof(info.Name));

	info.Level = level;

	this->m_GameMasterInfo.insert(std::pair<std::string, GAME_MASTER_INFO>(info.Name, info));
}

bool CGameMaster::CheckGameMasterLevel(LPOBJ lpObj, int level) // OK
{
	std::map<std::string, GAME_MASTER_INFO>::iterator it = this->m_GameMasterInfo.find(lpObj->Name);

	if (it != this->m_GameMasterInfo.end())
	{
		if (strcmp(it->second.Account, lpObj->Account) == 0 && strcmp(it->second.Name, lpObj->Name) == 0)
		{
			return it->second.Level;
		}
	}

	return -1;
}
