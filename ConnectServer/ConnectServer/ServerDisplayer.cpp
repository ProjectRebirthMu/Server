// Rev - 2023

#include "stdafx.h"
#include "ServerDisplayer.h"
#include "Log.h"
#include "resource.h"
#include "ServerList.h"
#include "SocketManager.h"
#include "Util.h"
#include "Protect.h"

CServerDisplayer gServerDisplayer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerDisplayer::CServerDisplayer()
{
	memset(&this->m_log, 0, sizeof(this->m_log));

	this->m_fonttitle = CreateFont(
		50,                           // Font size
		0,                            // Font width (0 for default)
		0,                            // Font italic angle (0 for default)
		0,                            // Font orientation angle (0 for default)
		FW_BOLD,                      // Font weight (bold)
		TRUE,                         // Font italic (TRUE to enable, FALSE to disable)
		FALSE,                        // Font underline (TRUE to enable, FALSE to disable)
		0,                            // Font strikeout (0 to disable)
		ANSI_CHARSET,                 // Character set (ANSI)
		OUT_DEFAULT_PRECIS,           // Output precision
		CLIP_DEFAULT_PRECIS,          // Clipping precision
		DEFAULT_QUALITY,              // Font quality
		DEFAULT_PITCH | FF_DONTCARE,  // Pitch and font family style
		"Roboto"                      // Font name
	);

	this->m_brush = CreateSolidBrush(RGB(21, 24, 43));
}

CServerDisplayer::~CServerDisplayer() // OK
{
	DeleteObject(static_cast<HGDIOBJ>(this->m_fonttitle));
	DeleteObject(static_cast<HGDIOBJ>(this->m_brush));
}

void CServerDisplayer::Init(HWND hWnd) // OK
{
	PROTECT_START
		this->m_hwnd = hWnd;
	PROTECT_FINAL
		gLog.AddLog(1, "Log");
}

void CServerDisplayer::Run() // OK
{
	this->LogTextPaint();
	this->PaintAllInfo();
	this->PaintStatusBar();
}

void CServerDisplayer::PaintAllInfo() // OK
{
	RECT rect;

	GetClientRect(this->m_hwnd, &rect);

	rect.top = 0;
	rect.bottom = 80;

	HDC hdc = GetDC(this->m_hwnd);

	int OldBkMode = SetBkMode(hdc, TRANSPARENT);
	HFONT OldFont = (HFONT)SelectObject(hdc, this->m_fonttitle);

	SetTextColor(hdc, RGB(255, 255, 255));
	FillRect(hdc, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText, "ConnectServer | XML Ver: 1.0.0.1");

	TextOut(hdc, 55, 15, this->m_DisplayerText, strlen(this->m_DisplayerText));

	SelectObject(hdc, OldFont);
	SetBkMode(hdc, OldBkMode);
	ReleaseDC(this->m_hwnd, hdc);
}

void CServerDisplayer::LogTextPaint()
{
	RECT rect;
	GetClientRect(this->m_hwnd, &rect);
	rect.top = -10;
	rect.bottom = 500;

	HDC hdc = GetDC(this->m_hwnd);
	FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

	int line = MAX_LOG_TEXT_LINE;
	int count = (((this->m_count - 1) >= 0) ? (this->m_count - 1) : (MAX_LOG_TEXT_LINE - 1));

	for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
	{
		COLORREF textColor;
		switch (this->m_log[count].color)
		{
		case LOG_BLACK:
			textColor = RGB(192, 192, 192);
			break;
		case LOG_RED:
			textColor = RGB(255, 0, 0);
			break;
		case LOG_GREEN:
			textColor = RGB(110, 255, 0);
			break;
		case LOG_BLUE:
			textColor = RGB(0, 110, 255);
			break;
		case LOG_ORANGE:
			textColor = RGB(255, 110, 0);
			break;
		case LOG_PURPLE:
			textColor = RGB(160, 70, 160);
			break;
		case LOG_PINK:
			textColor = RGB(255, 0, 128);
			break;
		case LOG_YELLOW:
			textColor = RGB(255, 240, 0);
			break;
		}

		const int size = lstrlenA(m_log[count].text);
		if (size > 0)
		{
			RECT textRect;
			textRect.left = rect.left;
			textRect.right = rect.right;
			textRect.top = 65 + ((line - 1) * 15);
			textRect.bottom = textRect.top + 15;
			SetTextColor(hdc, textColor);
			SetBkMode(hdc, TRANSPARENT);
			DrawTextA(hdc, m_log[count].text, size, &textRect, DT_CENTER);
			--line;
		}

		--count;
		if (count < 0)
			count = MAX_LOG_TEXT_LINE - 1;
	}

	ReleaseDC(this->m_hwnd, hdc);
}

void CServerDisplayer::PaintStatusBar()
{
	char buff[256];
	sprintf_s(buff, "[%s] ConnectServer | XML %s", VERSION, VERSION);

	SetWindowText(m_hwnd, buff);

	HWND hWndStatusBar = GetDlgItem(m_hwnd, IDC_STATUSBAR);
	DWORD dwStyle = GetWindowLong(hWndStatusBar, GWL_STYLE);
	dwStyle |= SBARS_SIZEGRIP | CCS_BOTTOM | WS_BORDER | WS_CLIPSIBLINGS;
	SetWindowLong(hWndStatusBar, GWL_STYLE, dwStyle);

	char szTempText[256];

	sprintf_s(szTempText, "QueueSize: %d", gSocketManager.GetQueueSize());
	SendMessage(hWndStatusBar, SB_SETTEXT, 0, (LPARAM)szTempText);

	sprintf_s(szTempText, "Connected: %d", GetUserCount());
	SendMessage(hWndStatusBar, SB_SETTEXT, 1, (LPARAM)szTempText);

	sprintf_s(szTempText, "GameServers: %d/%d", gServerList.m_GameServersList, gServerList.m_GameServersCount);
	SendMessage(hWndStatusBar, SB_SETTEXT, 2, (LPARAM)szTempText);

	sprintf_s(szTempText, "JoinServer: %s", (gServerList.CheckJoinServerState() == 1) ? "ON" : "OFF");
	SendMessage(hWndStatusBar, SB_SETTEXT, 3, (LPARAM)szTempText);

	sprintf_s(szTempText, "Versão: %s", VERSION);
	SendMessage(hWndStatusBar, SB_SETTEXT, 4, (LPARAM)szTempText);

	sprintf_s(szTempText, "Modo: %s", (gServerList.CheckJoinServerState() == 0) ? "Standby" : "Active");
	SendMessage(hWndStatusBar, SB_SETTEXT, 5, (LPARAM)szTempText);

	sprintf_s(szTempText, "Licença: Premium");
	SendMessage(hWndStatusBar, SB_SETTEXT, 6, (LPARAM)szTempText);

	SendMessage(hWndStatusBar, SB_SETTEXT, 7, 0);

	ShowWindow(hWndStatusBar, SW_SHOW);
}

void CServerDisplayer::LogAddText(eLogColor color, const char* text, int size)
{
	PROTECT_START
		size = min(size, MAX_LOG_TEXT_SIZE - 1);

	memset(this->m_log[this->m_count].text, 0, sizeof(this->m_log[this->m_count].text));

	memcpy(this->m_log[this->m_count].text, text, size);

	this->m_log[this->m_count].color = color;

	this->m_count = (++this->m_count >= MAX_LOG_TEXT_LINE) ? 0 : this->m_count;

	PROTECT_FINAL

		gLog.Output(LOG_GENERAL, "%s", &text[9]);
}
