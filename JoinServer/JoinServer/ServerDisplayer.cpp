// ServerDisplayer.cpp: implementation of the CServerDisplayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServerDisplayer.h"
#include "AccountManager.h"
#include "JoinServerProtocol.h"
#include "Log.h"
#include "Protect.h"
#include "ServerManager.h"
#include "SocketManager.h"
#include "Resource.h"

CServerDisplayer gServerDisplayer;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerDisplayer::CServerDisplayer() // OK
{
	for(int n=0;n < MAX_LOG_TEXT_LINE;n++)
	{
		memset(&this->m_log[n],0,sizeof(this->m_log[n]));
	}
}

CServerDisplayer::~CServerDisplayer() // OK
{
}

void CServerDisplayer::Init(HWND hWnd) // OK
{
	PROTECT_START

	this->m_hwnd = hWnd;

	PROTECT_FINAL

	gLog.AddLog(1,"LOG");

	gLog.AddLog(1,"LOG_ACCOUNT");
}

void CServerDisplayer::Run() // OK
{
	this->LogTextPaint();
	this->PaintStatusBar();
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

    HBITMAP hBitmap2 = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2));
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
            textRect.top = 105 + ((line - 1) * 15);
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

void CServerDisplayer::PaintStatusBar() // OK
{
	char buff[256];

	wsprintf(buff, "[%s] JoinServer | XML %s", JOINSERVER_VERSION, VERSION);

	SetWindowText(this->m_hwnd, buff);

	HWND hWndStatusBar = GetDlgItem(this->m_hwnd, IDC_STATUSBAR);

	char szTempText[85];

	wsprintf(szTempText, "QueueSize: %d", gSocketManager.GetQueueSize());
	SendMessage(hWndStatusBar, SB_SETTEXT, 0, (LPARAM)szTempText);

	wsprintf(szTempText, "Connected: %d", gAccountManager.GetAccountCount());
	SendMessage(hWndStatusBar, SB_SETTEXT, 1, (LPARAM)szTempText);

	int state = 0;

	for (int n = 0; n < MAX_SERVER; n++)
	{
		if (gServerManager[n].CheckState() == 0)
		{
			continue;
		}

		if ((GetTickCount() - gServerManager[n].m_PacketTime) <= 60000)
		{
			state = 1;
			break;
		}
	}

	wsprintf(szTempText, "Mode: %s", (state == 0) ? "Standby" : "Active");
	SendMessage(hWndStatusBar, SB_SETTEXT, 2, (LPARAM)szTempText);

	sprintf_s(szTempText, "Licença: Premium");
	SendMessage(hWndStatusBar, SB_SETTEXT, 3, (LPARAM)szTempText);

	SendMessage(hWndStatusBar, SB_SETTEXT, 4, (LPARAM)NULL);

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