// ServerDisplayer.cpp: implementation of the CServerDisplayer class.
// Revisado: 14/07/23 16:29 GMT-3
// By: Qubit
//////////////////////////////////////////////////////////////////////

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
    for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
    {
        memset(&this->m_log[n], 0, sizeof(this->m_log[n]));
    }
}

CServerDisplayer::~CServerDisplayer()
{
}

void CServerDisplayer::Init(HWND hWnd)
{
    PROTECT_START

        auto& log = gLog;
    char* message = "Log";

    this->m_hwnd = hWnd;
    PROTECT_FINAL
        log.AddLog(1, message);
}

void CServerDisplayer::Run()
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

void CServerDisplayer::PaintStatusBar()
{
    char buff[256];
    sprintf(buff, "ConnectServer | XML %s", VERSION);

    SetWindowText(m_hwnd, buff);

    HWND hWndStatusBar = GetDlgItem(m_hwnd, IDC_STATUSBAR);
    DWORD dwStyle = GetWindowLong(hWndStatusBar, GWL_STYLE);
    dwStyle |= SBARS_SIZEGRIP | CCS_BOTTOM | WS_BORDER | WS_CLIPSIBLINGS;
    SetWindowLong(hWndStatusBar, GWL_STYLE, dwStyle);

    char szTempText[256];
    std::sprintf(szTempText, "QueueSize: %d", gSocketManager.GetQueueSize());
    SendMessage(hWndStatusBar, SB_SETTEXT, 0, (LPARAM)szTempText);

    std::sprintf(szTempText, "Connected: %d", GetUserCount());
    SendMessage(hWndStatusBar, SB_SETTEXT, 1, (LPARAM)szTempText);

    std::sprintf(szTempText, "GameServers: %d/%d", gServerList.m_GameServersList, gServerList.m_GameServersCount);
    SendMessage(hWndStatusBar, SB_SETTEXT, 2, (LPARAM)szTempText);

    std::sprintf(szTempText, "JoinServer: %s", (gServerList.CheckJoinServerState() == 1) ? "ON" : "OFF");
    SendMessage(hWndStatusBar, SB_SETTEXT, 3, (LPARAM)szTempText);

    std::sprintf(szTempText, "Versão: %s", VERSION);
    SendMessage(hWndStatusBar, SB_SETTEXT, 4, (LPARAM)szTempText);

    std::sprintf(szTempText, "Modo: %s", (gServerList.CheckJoinServerState() == 0) ? "Standby" : "Active");
    SendMessage(hWndStatusBar, SB_SETTEXT, 5, (LPARAM)szTempText);

    std::sprintf(szTempText, "Licença: Premium");
    SendMessage(hWndStatusBar, SB_SETTEXT, 6, (LPARAM)szTempText);

    SendMessage(hWndStatusBar, SB_SETTEXT, 7, 0);

    ShowWindow(hWndStatusBar, SW_SHOW);
}

void CServerDisplayer::LogAddText(eLogColor color, const char* text, int size)
{
    size = min(size, MAX_LOG_TEXT_SIZE - 1);

    memset(this->m_log[this->m_count].text, 0, sizeof(this->m_log[this->m_count].text));
    std::memcpy(this->m_log[this->m_count].text, text, size);
    this->m_log[this->m_count].color = color;
    this->m_count = (this->m_count + 1) % MAX_LOG_TEXT_LINE;

    gLog.Output(LOG_GENERAL, "%s", &text[9]);
}