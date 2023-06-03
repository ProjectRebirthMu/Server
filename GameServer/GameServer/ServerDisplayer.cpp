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

	this->m_fonttitle = CreateFont(
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
	this->m_brush2 = CreateSolidBrush(RGB(40, 40, 40));
}

CServerDisplayer::~CServerDisplayer() // OK
{
	DeleteObject(this->m_fonttitle);
	DeleteObject(this->m_brush);
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
	this->SetWindowName();
	this->PaintAllInfo();
	this->PaintOnline();
	this->PaintPremium();
	this->PaintSeason();
	this->PaintEventTime();
	this->PaintInvasionTime();
	this->PaintCustomArenaTime();
	this->LogTextPaintConnect();
	this->LogTextPaintGlobalMessage();
}

void CServerDisplayer::SetWindowName() // OK
{
	char buff[256];

	wsprintf(buff,"[%s] %s (ON: %d) GameServer | XML Ver: %s",GAMESERVER_VERSION,gServerInfo.m_ServerName, gObjTotalUser,VERSION);

	SetWindowText(this->m_hwnd,buff);

	HWND hWndStatusBar = GetDlgItem(this->m_hwnd, IDC_STATUSBAR);

	RECT rect;

	GetClientRect(this->m_hwnd,&rect);

	RECT rect2;

	GetClientRect(hWndStatusBar,&rect2);

	MoveWindow(hWndStatusBar,0,rect.bottom-rect2.bottom+rect2.top,rect.right,rect2.bottom-rect2.top,true);

            int iStatusWidths[] = {190,270,360,450,580, -1};

            char text[256];

            SendMessage(hWndStatusBar, SB_SETTEXT, 7, (LPARAM)text);

			wsprintf(text, "Connected: %d/%d", gObjTotalUser, gServerInfo.m_ServerMaxUserNumber);

            SendMessage(hWndStatusBar, SB_SETTEXT, 0,(LPARAM)text);

			wsprintf(text, "OffStore: %d", gObjOffStore);

            SendMessage(hWndStatusBar, SB_SETTEXT, 1,(LPARAM)text);

			wsprintf(text, "OffAttack: %d", gObjOffAttack);

            SendMessage(hWndStatusBar, SB_SETTEXT, 2,(LPARAM)text);

			wsprintf(text, "Bots Buffer: %d", gObjTotalBot);

            SendMessage(hWndStatusBar, SB_SETTEXT, 3,(LPARAM)text);

			wsprintf(text, "Monsters: %d/%d", gObjTotalMonster,MAX_OBJECT_MONSTER);

            SendMessage(hWndStatusBar, SB_SETTEXT, 4,(LPARAM)text);

			SendMessage(hWndStatusBar, SB_SETTEXT, 5,(LPARAM)NULL);

            ShowWindow(hWndStatusBar, SW_SHOW);
}

void CServerDisplayer::PaintAllInfo() // OK
{
	RECT rect;

	GetClientRect(m_hwnd, &rect);

	rect.top = 0;
	rect.bottom = 80;

	HDC hdc = GetDC(m_hwnd);

	int OldBkMode = SetBkMode(hdc, TRANSPARENT);
	HFONT OldFont = (HFONT)SelectObject(hdc, m_fonttitle);

	SetTextColor(hdc, RGB(255, 255, 255));

	FillRect(hdc, &rect, m_brush);
	strcpy_s(m_DisplayerText, "GameServer | XML Ver: 1.0.0.1");

    TextOut(hdc, 130, 15, m_DisplayerText, strlen(m_DisplayerText));

	SelectObject(hdc, OldFont);
	SetBkMode(hdc, OldBkMode);
	ReleaseDC(m_hwnd, hdc);
}

void CServerDisplayer::PaintOnline() // OK
{}

void CServerDisplayer::PaintSeason() // OK
{}

void CServerDisplayer::PaintPremium() // OK
{}

void CServerDisplayer::PaintEventTime() // OK
{}

void CServerDisplayer::PaintInvasionTime() // OK
{}

void CServerDisplayer::PaintCustomArenaTime() // OK
{}

void CServerDisplayer::LogTextPaint()
{
	RECT rect;
	GetClientRect(m_hwnd, &rect);
	rect.left = 0;
	rect.top = 255;
	rect.bottom = 500;
	HDC hdc = GetDC(m_hwnd);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);

	int line = MAX_LOG_TEXT_LINE;
	int count = m_count - 1;

	if (count < 0)
		count = MAX_LOG_TEXT_LINE - 1;

	int lineHeight = 15;

	for (int n = 0; n < MAX_LOG_TEXT_LINE; ++n)
	{
		switch (m_log[count].color)
		{
		case LOG_BLACK:
			SetTextColor(hdc, RGB(192, 192, 192));
			break;
		case LOG_RED:
			SetTextColor(hdc, RGB(255, 0, 0));
			break;
		case LOG_GREEN:
			SetTextColor(hdc, RGB(110, 255, 0));
			break;
		case LOG_BLUE:
			SetTextColor(hdc, RGB(0, 110, 255));
			break;
		case LOG_ORANGE:
			SetTextColor(hdc, RGB(255, 110, 0));
			break;
		case LOG_PURPLE:
			SetTextColor(hdc, RGB(160, 70, 160));
			break;
		case LOG_PINK:
			SetTextColor(hdc, RGB(255, 0, 128));
			break;
		case LOG_YELLOW:
			SetTextColor(hdc, RGB(255, 240, 0));
			break;
		default:
			break;
		}
		SetBkMode(hdc, TRANSPARENT);

		const int size = lstrlenA(m_log[count].text);
		if (size > 0)
		{
			RECT textRect = rect;
			textRect.top += (line - 1) * lineHeight;
			textRect.bottom = textRect.top + lineHeight;
			DrawTextA(hdc, m_log[count].text, size, &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			--line;
		}

		--count;
		if (count < 0)
			count = MAX_LOG_TEXT_LINE - 1;
	}

	ReleaseDC(m_hwnd, hdc);
}

void CServerDisplayer::LogTextPaintConnect() // OK
{}

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

void CServerDisplayer::LogAddText(eLogColor color,char* text,int size) // OK
{
	size = ((size>=MAX_LOG_TEXT_SIZE)?(MAX_LOG_TEXT_SIZE-1):size);

	memset(&this->m_log[this->m_count].text,0,sizeof(this->m_log[this->m_count].text));

	memcpy(&this->m_log[this->m_count].text,text,size);

	this->m_log[this->m_count].color = color;

	this->m_count = (((++this->m_count)>=MAX_LOG_TEXT_LINE)?0:this->m_count);

	gLog.Output(LOG_GENERAL,"%s",&text[9]);
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