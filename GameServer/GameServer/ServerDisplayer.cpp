// ServerDisplayer.cpp: implementation of the CServerDisplayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerDisplayer.h"
#include "CustomArena.h"
#include "GameMain.h"
#include "Log.h"
#include "resource.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "User.h"

CServerDisplayer gServerDisplayer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerDisplayer::CServerDisplayer() // OK
{
	for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
	{
		memset(&this->m_log[n], 0, sizeof(this->m_log[n]));
	}
}

CServerDisplayer::~CServerDisplayer() // OK
{
}

void CServerDisplayer::Init(HWND hWnd) // OK
{
	this->m_hwnd = hWnd;

	gLog.AddLog(1,"Log");

	gLog.AddLog(gServerInfo.m_WriteChatLog,"Chat_log");

	gLog.AddLog(gServerInfo.m_WriteCommandLog,"Command_log");

	gLog.AddLog(gServerInfo.m_WriteTradeLog,"LogTrade_log");

	gLog.AddLog(gServerInfo.m_WriteConnectLog,"Connect_log");

	gLog.AddLog(gServerInfo.m_WriteHackLog,"Hack_log");

	gLog.AddLog(gServerInfo.m_WriteCashShopLog,"CashShop_log");

	gLog.AddLog(gServerInfo.m_WriteChaosMixLog,"ChaosMix_log");
}

void CServerDisplayer::Run() // OK
{
	this->LogTextPaint();
	this->LogTextPaintGlobalMessage();
}

void CServerDisplayer::LogTextPaint()
{
    RECT rect;
    GetClientRect(this->m_hwnd, &rect);
    HDC hdc = GetDC(this->m_hwnd);

    HBITMAP hBitmap1 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
    if (hBitmap1 != NULL)
    {
        HDC hdcMem1 = CreateCompatibleDC(hdc);
        HBITMAP hBitmapOld1 = (HBITMAP)SelectObject(hdcMem1, hBitmap1);

        BITMAP bitmap1;
        GetObject(hBitmap1, sizeof(BITMAP), &bitmap1);

        BitBlt(hdc, 0, 0, bitmap1.bmWidth, bitmap1.bmHeight, hdcMem1, 0, 0, SRCCOPY);

        SelectObject(hdcMem1, hBitmapOld1);
        DeleteDC(hdcMem1);
        DeleteObject(hBitmap1);
    }

#if (GAMESERVER_TYPE <= 0)
    HBITMAP hBitmap2 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2));
#else 
    HBITMAP hBitmap2 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3));
#endif

    if (hBitmap2 != NULL)
    {
        HDC hdcMem2 = CreateCompatibleDC(hdc);
        HBITMAP hBitmapOld2 = (HBITMAP)SelectObject(hdcMem2, hBitmap2);

        BITMAP bitmap2;
        GetObject(hBitmap2, sizeof(BITMAP), &bitmap2);

        BitBlt(hdc, 0, 0, bitmap2.bmWidth, bitmap2.bmHeight, hdcMem2, 0, 0, SRCCOPY);

        SelectObject(hdcMem2, hBitmapOld2);
        DeleteDC(hdcMem2);
        DeleteObject(hBitmap2);
    }

    int line = MAX_LOG_TEXT_LINE;
    int count = (this->m_count - 1 >= 0) ? (this->m_count - 1) : (MAX_LOG_TEXT_LINE - 1);

    for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
    {
        COLORREF textColor = RGB(192, 192, 192);

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

        const int size = lstrlenA(this->m_log[count].text);
        if (size > 0)
        {
            RECT textRect;
            textRect.left = rect.left;
            textRect.right = rect.right;
            textRect.top = 255 + ((line - 1) * 15);
            textRect.bottom = textRect.top + 15;

            SetBkColor(hdc, RGB(18, 21, 38));
            SetBkMode(hdc, OPAQUE);

            SIZE textSize;
            GetTextExtentPoint32A(hdc, this->m_log[count].text, size, &textSize);

            int rectWidth = textSize.cx + 2;
            int rectHeight = textSize.cy + 2;

            int rectLeft = (rect.right - rectWidth) / 2;
            int rectTop = textRect.top;

            RECT backgroundRect = { rectLeft, rectTop, rectLeft + rectWidth, rectTop + rectHeight };
            FillRect(hdc, &backgroundRect, CreateSolidBrush(RGB(18, 21, 38)));

            SetTextColor(hdc, textColor);
            SetBkMode(hdc, TRANSPARENT);

            UINT textFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
            DrawTextA(hdc, this->m_log[count].text, size, &textRect, textFormat);

            --line;
        }

        --count;
        if (count < 0)
            count = MAX_LOG_TEXT_LINE - 1;
    }

    ReleaseDC(this->m_hwnd, hdc);
}

void CServerDisplayer::LogTextPaintGlobalMessage()
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);

	rect.left = 0;
	rect.top = 85;
	rect.bottom = 50;

	HDC hdc = GetDC(m_hwnd);

	TRIVERTEX vertices[2] = { { rect.left, rect.top, 0, 0, 0, 0 }, { rect.left, rect.bottom, 0, 0, 0, 0 } };
	GRADIENT_RECT gradientRect = { 0, 1 };

	HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Roboto");

	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	SetTextColor(hdc, RGB(150, 150, 150));

	const char* globalMessage = "Mensagem Global";
	int messageLength = strlen(globalMessage);

	RECT textRect;
	textRect.left = rect.left + 5;
	textRect.top = rect.top + 2;
	textRect.right = textRect.left + rect.right;
	textRect.bottom = textRect.top + rect.bottom;

	DrawTextA(hdc, globalMessage, messageLength, &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	int line = MAX_LOGGLOBAL_TEXT_LINE;
	int count = (((m_countGlobal - 1) >= 0) ? (m_countGlobal - 1) : (MAX_LOGGLOBAL_TEXT_LINE - 1));

	for (int n = 0; n < MAX_LOGGLOBAL_TEXT_LINE; n++)
	{
		SetTextColor(hdc, RGB(153, 153, 0));

		int size = strlen(m_logGlobal[count].text);

		if (size > 1)
		{
			textRect.left = rect.left + 10;
			textRect.top = rect.top + 5 + (line * 15);
			textRect.right = textRect.left + rect.right;
			textRect.bottom = textRect.top + rect.bottom;

			DrawTextA(hdc, m_logGlobal[count].text, size, &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			line--;
		}

		count = (((--count) >= 0) ? count : (MAX_LOGGLOBAL_TEXT_LINE - 1));
	}

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
	ReleaseDC(m_hwnd, hdc);
}

void CServerDisplayer::LogAddText(eLogColor color, const char* text, int size)
{
        size = min(size, MAX_LOG_TEXT_SIZE - 1);

    memset(this->m_log[this->m_count].text, 0, sizeof(this->m_log[this->m_count].text));

    memcpy(this->m_log[this->m_count].text, text, size);

    this->m_log[this->m_count].color = color;

    this->m_count = (++this->m_count >= MAX_LOG_TEXT_LINE) ? 0 : this->m_count;

    gLog.Output(LOG_GENERAL, "%s", &text[9]);
}

void CServerDisplayer::LogAddTextConnect(eLogColor color,char* text,int size) // OK
{
	size = ((size>=MAX_LOGCONNECT_TEXT_SIZE)?(MAX_LOGCONNECT_TEXT_SIZE-1):size);

	memset(&this->m_logConnect[this->m_countConnect].text,0,sizeof(this->m_logConnect[this->m_countConnect].text));

	memcpy(&this->m_logConnect[this->m_countConnect].text,text,size);

	this->m_logConnect[this->m_countConnect].color = color;

	this->m_countConnect = (((++this->m_countConnect)>=MAX_LOGCONNECT_TEXT_LINE)?0:this->m_countConnect);

	gLog.Output(LOG_GENERAL,"%s",&text[9]);
}

void CServerDisplayer::LogAddTextGlobal(eLogColor color,char* text,int size) // OK
{
	size = min(size, MAX_LOG_TEXT_SIZE - 1);

	memset(&this->m_logGlobal[this->m_countGlobal].text,0,sizeof(this->m_logGlobal[this->m_countGlobal].text));

	memcpy(&this->m_logGlobal[this->m_countGlobal].text,text,size);

	this->m_logGlobal[this->m_countGlobal].color = color;

	this->m_countGlobal = (((++this->m_countGlobal)>=MAX_LOGGLOBAL_TEXT_LINE)?0:this->m_countGlobal);
}