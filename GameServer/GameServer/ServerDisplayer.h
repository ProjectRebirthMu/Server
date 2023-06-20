// ServerDisplayer.h: interface for the CServerDisplayer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_LOG_TEXT_LINE 12
#define MAX_LOG_TEXT_SIZE 100

#define MAX_LOGCONNECT_TEXT_LINE 13
#define MAX_LOGCONNECT_TEXT_SIZE 55

#define MAX_LOGGLOBAL_TEXT_LINE 8
#define MAX_LOGGLOBAL_TEXT_SIZE 100

enum eLogColor
{
	LOG_BLACK = 0,
	LOG_RED = 1,
	LOG_GREEN = 2,
	LOG_BLUE = 3,
	LOG_ORANGE = 4,
	LOG_PURPLE = 5,
	LOG_PINK = 6,
	LOG_YELLOW = 7,
	//MC bot
	LOG_BOT = 4,
	LOG_USER = 5,
	LOG_EVENT = 6,
	LOG_ALERT = 7,
	//MC bot
};

struct LOG_DISPLAY_INFO
{
	char text[MAX_LOG_TEXT_SIZE];
	eLogColor color;
};

struct LOGCONNECT_DISPLAY_INFO
{
	char text[MAX_LOGCONNECT_TEXT_SIZE];
	eLogColor color;
};

struct LOGGLOBAL_DISPLAY_INFO
{
	char text[MAX_LOGGLOBAL_TEXT_SIZE];
	eLogColor color;
};

class CServerDisplayer
{
public:
	CServerDisplayer();
	virtual ~CServerDisplayer();
	void Init(HWND hWnd);
	void Run();
	void PaintAllInfo();
	void LogTextPaint();
	void LogTextPaintGlobalMessage();
	void LogAddText(eLogColor color,const char* text,int size);
	void LogAddTextConnect(eLogColor color,char* text,int size);
	void LogAddTextGlobal(eLogColor color,char* text,int size);
	int EventBc;
	int EventDs;
	int EventCc;
	int EventIt;
	int EventCustomLottery;
	int EventCustomBonus;
	//int EventCustomArena;
	int EventCustomQuiz;
	int EventMoss;
	int EventKing;
	int EventDrop;
	int EventTvT;
	int EventGvG;
	int EventInvasion[30];
	int EventCustomArena[30];
	int EventCs;
	int EventCsState;
	int EventCastleDeep;
	int EventCryWolf;
	int EventCryWolfState;
private:
	HWND m_hwnd;
	HFONT m_fonttitle;
	LOG_DISPLAY_INFO m_log[MAX_LOG_TEXT_LINE];
	LOGCONNECT_DISPLAY_INFO m_logConnect[MAX_LOGCONNECT_TEXT_LINE];
	LOGGLOBAL_DISPLAY_INFO m_logGlobal[MAX_LOGGLOBAL_TEXT_LINE];
	int g_nScrollPos;
	int m_count;
	int m_countConnect;
	int m_countGlobal;
};

extern CServerDisplayer gServerDisplayer;
