// ServerDisplayer.h: interface for the CServerDisplayer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_LOG_TEXT_LINE 22
#define MAX_LOG_TEXT_SIZE 80

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
};

struct LOG_DISPLAY_INFO
{
	char text[MAX_LOG_TEXT_SIZE];
	eLogColor color;
};

class CServerDisplayer
{
public:
	CServerDisplayer();
	virtual ~CServerDisplayer();
	void Init(HWND hWnd);
	void Run();
	void PaintStatusBar();
	void LogTextPaint();
	void LogAddText(eLogColor color,const char* text,int size);
private:
	HWND m_hwnd;
	LOG_DISPLAY_INFO m_log[MAX_LOG_TEXT_LINE];
	int m_count;
};

extern CServerDisplayer gServerDisplayer;
