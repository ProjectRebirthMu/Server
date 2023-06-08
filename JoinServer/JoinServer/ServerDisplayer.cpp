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
	for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
	{
		memset(&this->m_log[n], 0, sizeof(this->m_log[n]));
	}

	this->m_font = CreateFont(
		50,                           // Tamanho da fonte
		0,                            // Largura da fonte (0 para o padrão)
		0,                            // Angulo de inclinação da fonte (0 para o padrão)
		0,                            // Ângulo de orientação da fonte (0 para o padrão)
		FW_BOLD,                      // Peso da fonte (bold)
		TRUE,                         // Itálico (TRUE para ativar, FALSE para desativar)
		FALSE,                        // Sublinhado (TRUE para ativar, FALSE para desativar)
		0,                            // Tachado (0 para desativar)
		ANSI_CHARSET,                 // Conjunto de caracteres (ANSI)
		OUT_DEFAULT_PRECIS,           // Precisão de saída
		CLIP_DEFAULT_PRECIS,          // Precisão de recorte
		DEFAULT_QUALITY,              // Qualidade da fonte
		DEFAULT_PITCH | FF_DONTCARE,  // Estilo de pitch e família da fonte
		"Roboto"                      // Nome da fonte
	);

	this->m_brush = CreateSolidBrush(RGB(21, 24, 43));
}

CServerDisplayer::~CServerDisplayer() // OK
{
	DeleteObject(this->m_font);
	DeleteObject(this->m_brush);
}

void CServerDisplayer::Init(HWND hWnd) // OK
{
	PROTECT_START

		this->m_hwnd = hWnd;

	PROTECT_FINAL

		gLog.AddLog(1, "LOG");

	gLog.AddLog(1, "LOG_ACCOUNT");
}

void CServerDisplayer::Run() // OK
{
	this->PaintAllInfo();
	this->LogTextPaint();
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
	HFONT OldFont = (HFONT)SelectObject(hdc, this->m_font);

	SetTextColor(hdc, RGB(255, 255, 255));
	FillRect(hdc, &rect, this->m_brush);
	strcpy_s(this->m_DisplayerText, "JoinServer | XML Ver: 1.0.0.1");

	TextOut(hdc, 55, 15, this->m_DisplayerText, strlen(this->m_DisplayerText));

	SelectObject(hdc, OldFont);
	SetBkMode(hdc, OldBkMode);
	ReleaseDC(this->m_hwnd, hdc);
}

void CServerDisplayer::LogTextPaint() // OK
{
	RECT rect;

	GetClientRect(this->m_hwnd, &rect);

	rect.top = 100;
	rect.bottom = 500;

	HDC hdc = GetDC(this->m_hwnd);

	FillRect(hdc, &rect, (HBRUSH)GetStockObject(4));

	int line = MAX_LOG_TEXT_LINE;

	int count = (((this->m_count - 1) >= 0) ? (this->m_count - 1) : (MAX_LOG_TEXT_LINE - 1));

	for (int n = 0; n < MAX_LOG_TEXT_LINE; n++)
	{
		switch (this->m_log[count].color)
		{
		case LOG_BLACK:
			SetTextColor(hdc, RGB(192, 192, 192));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_RED:
			SetTextColor(hdc, RGB(255, 0, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_GREEN:
			SetTextColor(hdc, RGB(110, 255, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_BLUE:
			SetTextColor(hdc, RGB(0, 110, 255));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_ORANGE:
			SetTextColor(hdc, RGB(255, 110, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_PURPLE:
			SetTextColor(hdc, RGB(160, 70, 160));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_PINK:
			SetTextColor(hdc, RGB(255, 0, 128));
			SetBkMode(hdc, TRANSPARENT);
			break;
		case LOG_YELLOW:
			SetTextColor(hdc, RGB(255, 240, 0));
			SetBkMode(hdc, TRANSPARENT);
			break;
		}

		int size = strlen(this->m_log[count].text);

		if (size > 1)
		{
			RECT textRect;
			textRect.left = rect.left;
			textRect.right = rect.right;
			textRect.top = 65 + ((line - 1) * 15);
			textRect.bottom = textRect.top + 15;
			DrawTextA(hdc, m_log[count].text, size, &textRect, DT_CENTER);
			--line;
		}

		count = (((--count) >= 0) ? count : (MAX_LOG_TEXT_LINE - 1));
	}

	ReleaseDC(this->m_hwnd, hdc);
}

void CServerDisplayer::PaintStatusBar() // OK
{
	char buff[256];

	wsprintf(buff, "[%s] DataServer | XML %s", JOINSERVER_VERSION, VERSION);

	SetWindowText(this->m_hwnd, buff);

	HWND hWndStatusBar = GetDlgItem(this->m_hwnd, IDC_STATUSBAR);

	char szTempText[85];

	wsprintf(szTempText, "QueueSize: %d", gSocketManager.GetQueueSize());
	SendMessage(hWndStatusBar, SB_SETTEXT, 0, (LPARAM)szTempText);

	int state = 0;

	for (int n = 0; n < MAX_SERVER; n++)
	{
		if (gServerManager[n].CheckState() == 0)
			continue;

		if ((GetTickCount() - gServerManager[n].m_PacketTime) <= 60000)
		{
			state = 1;
			break;
		}
	}

	wsprintf(szTempText, "Mode: %s", (state == 0) ? "Standby" : "Active");
	SendMessage(hWndStatusBar, SB_SETTEXT, 1, (LPARAM)szTempText);

	sprintf_s(szTempText, "Licença: Premium");
	SendMessage(hWndStatusBar, SB_SETTEXT, 2, (LPARAM)szTempText);

	SendMessage(hWndStatusBar, SB_SETTEXT, 3, (LPARAM)NULL);

	ShowWindow(hWndStatusBar, SW_SHOW);
}

void CServerDisplayer::LogAddText(eLogColor color, char* text, int size) // OK
{
	PROTECT_START

		size = ((size >= MAX_LOG_TEXT_SIZE) ? (MAX_LOG_TEXT_SIZE - 1) : size);

	memset(&this->m_log[this->m_count].text, 0, sizeof(this->m_log[this->m_count].text));

	memcpy(&this->m_log[this->m_count].text, text, size);

	this->m_log[this->m_count].color = color;

	this->m_count = (((++this->m_count) >= MAX_LOG_TEXT_LINE) ? 0 : this->m_count);

	PROTECT_FINAL

		gLog.Output(LOG_GENERAL, "%s", &text[9]);
}