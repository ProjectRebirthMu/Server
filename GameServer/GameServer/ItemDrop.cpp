// ItemDrop.cpp: implementation of the CItemDrop class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemDrop.h"
#include "BonusManager.h"
#include "CrywolfSync.h"
#include "DSProtocol.h"
#include "ItemManager.h"
#include "ItemOptionRate.h"
#include "MemScript.h"
#include "Monster.h"
#include "RandomManager.h"
#include "Util.h"

CItemDrop gItemDrop;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemDrop::CItemDrop() // OK
{
	this->m_ItemDropInfo.clear();
}

CItemDrop::~CItemDrop() // OK
{
}

void CItemDrop::Load(char* path) // OK
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(path);

	if (res.status != pugi::status_ok)
	{
		ErrorMessageBox("File %s load fail. Error: %s", path, res.description());
		return;
	}

	this->m_ItemDropInfo.clear();

	pugi::xml_node ItemDrop = file.child("ItemDrop");

	for (pugi::xml_node RuleDrop = ItemDrop.child("RuleDrop"); RuleDrop; RuleDrop = RuleDrop.next_sibling())
	{
		int MapNumber = RuleDrop.attribute("MapNumber").as_int();

		for (pugi::xml_node Item = RuleDrop.child("Item"); Item; Item = Item.next_sibling())
		{
			ITEM_DROP_INFO info;

			info.MapNumber = MapNumber;
			info.Index = SafeGetItem(GET_ITEM(Item.attribute("Cat").as_int(), Item.attribute("Index").as_int()));
			info.Level = Item.attribute("Level").as_int();
			info.Option0 = Item.attribute("LevelOption").as_int();
			info.Option1 = Item.attribute("SkillOption").as_int();
			info.Option2 = Item.attribute("LuckOption").as_int();
			info.Option3 = Item.attribute("JoLOption").as_int();
			info.Option4 = Item.attribute("ExcOption").as_int();
			info.Option5 = Item.attribute("SetOption").as_int();
			info.Option6 = Item.attribute("SocketOption").as_int();
			info.Duration = Item.attribute("Duration").as_int();
			info.MonsterClass = Item.attribute("MonsterIndex").as_int();
			info.MonsterLevelMin = Item.attribute("MonsterMinLevel").as_int();
			info.MonsterLevelMax = Item.attribute("MonsterMaxLevel").as_int();
			info.DropRate = Item.attribute("DropRate").as_int();

			this->m_ItemDropInfo.push_back(info);
		}
	}
}

int CItemDrop::DropItem(LPOBJ lpObj,LPOBJ lpTarget) // OK
{

	CRandomManager RandomManager;

	for (std::vector<ITEM_DROP_INFO>::iterator it = this->m_ItemDropInfo.begin(); it != this->m_ItemDropInfo.end(); it++)
	{
		int DropRate;

		ITEM_INFO ItemInfo;

		if (gItemManager.GetInfo(it->Index, &ItemInfo) == 0)
		{
			continue;
		}

		if (it->MapNumber != -1 && it->MapNumber != lpObj->Map)
		{
			continue;
		}

		if(it->MonsterClass != -1 && it->MonsterClass != lpObj->Class)
		{
			continue;
		}

		if(it->MonsterLevelMin != -1 && it->MonsterLevelMin > lpObj->Level)
		{
			continue;
		}

		if(it->MonsterLevelMax != -1 && it->MonsterLevelMax < lpObj->Level)
		{
			continue;
		}

		if ((DropRate = it->DropRate) == -1 || (GetLargeRand() % 1000000) < (DropRate = this->GetItemDropRate(lpObj, lpTarget, it->Index, it->Level, it->DropRate))) 
		{
			int rate = (1000000 / ((DropRate == -1) ? 1000000 : DropRate));

			RandomManager.AddElement((int)(&(*it)), rate);
		}
	}

	ITEM_DROP_INFO* lpItemDropInfo;

	if(RandomManager.GetRandomElement((int*)&lpItemDropInfo) == 0)
	{
		return 0;
	}
	else
	{
		WORD ItemIndex = lpItemDropInfo->Index;
		BYTE ItemLevel = lpItemDropInfo->Level;
		BYTE ItemOption1 = 0;
		BYTE ItemOption2 = 0;
		BYTE ItemOption3 = 0;
		BYTE ItemNewOption = 0;
		BYTE ItemSetOption = 0;
		BYTE ItemSocketOption[MAX_SOCKET_OPTION] = {0xFF,0xFF,0xFF,0xFF,0xFF};

		gItemOptionRate.GetItemOption0(lpItemDropInfo->Option0,&ItemLevel);

		gItemOptionRate.GetItemOption1(lpItemDropInfo->Option1,&ItemOption1);

		gItemOptionRate.GetItemOption2(lpItemDropInfo->Option2,&ItemOption2);

		gItemOptionRate.GetItemOption3(lpItemDropInfo->Option3,&ItemOption3);

		gItemOptionRate.GetItemOption4(lpItemDropInfo->Option4,&ItemNewOption);

		gItemOptionRate.GetItemOption5(lpItemDropInfo->Option5,&ItemSetOption);

		gItemOptionRate.GetItemOption6(lpItemDropInfo->Option6,&ItemSocketOption[0]);

		gItemOptionRate.MakeNewOption(ItemIndex,ItemNewOption,&ItemNewOption);

		gItemOptionRate.MakeSetOption(ItemIndex,ItemSetOption,&ItemSetOption);

		gItemOptionRate.MakeSocketOption(ItemIndex,ItemSocketOption[0],&ItemSocketOption[0]);

		GDCreateItemSend(lpTarget->Index,lpObj->Map,(BYTE)lpObj->X,(BYTE)lpObj->Y,ItemIndex,ItemLevel,0,ItemOption1,ItemOption2,ItemOption3,lpTarget->Index,((ItemNewOption==0)?lpItemDropInfo->Grade:ItemNewOption),ItemSetOption,0,0,ItemSocketOption,0xFF,((lpItemDropInfo->Duration>0)?((DWORD)time(0)+lpItemDropInfo->Duration):0));

		return 1;
	}
}

int CItemDrop::GetItemDropRate(LPOBJ lpObj, LPOBJ lpTarget, int ItemIndex, int ItemLevel, int DropRate) // OK
{
	if (ItemIndex == GET_ITEM(12, 15) || ItemIndex == GET_ITEM(14, 13) || ItemIndex == GET_ITEM(14, 14) || ItemIndex == GET_ITEM(14, 16) || ItemIndex == GET_ITEM(14, 22) || ItemIndex == GET_ITEM(14, 31))
	{
		if (gCrywolfSync.CheckApplyPenalty() != 0 && gCrywolfSync.GetOccupationState() == 1)
		{
			if ((GetLargeRand() % 100) >= gCrywolfSync.GetGemDropPenaltiyRate())
			{
				return 0;
			}
		}
	}

	return DropRate;
}
