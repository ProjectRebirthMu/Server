// MemScript.cpp: implementation of the CMemScript class.
// Revisado: 14/07/23 14:32 GMT-3
// By: Qubit
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemScript.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemScript::CMemScript() // OK
{
	this->m_buff = nullptr; // using nullptr instead of 0
	this->m_size = 0;
	memset(this->m_path, 0, sizeof(this->m_path));

	this->SetLastError(4);
}

CMemScript::~CMemScript() // OK
{
	if (this->m_buff != nullptr) // using nullptr instead of 0
	{
		delete[] this->m_buff;
		this->m_buff = nullptr; // using nullptr instead of 0
	}
	this->m_size = 0;
}

bool CMemScript::SetBuffer(const char* path) // Updated
{
	strcpy_s(this->m_path, sizeof(this->m_path), path);

	HANDLE file = CreateFileA(this->m_path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, nullptr);

	if (file == INVALID_HANDLE_VALUE)
	{
		this->SetLastError(0);
		return false;
	}

	this->m_size = GetFileSize(file, nullptr);

	if (this->m_buff != nullptr)
	{
		delete[] this->m_buff;
		this->m_buff = nullptr;
	}

	this->m_buff = new (std::nothrow) char[this->m_size];

	if (this->m_buff == nullptr)
	{
		this->SetLastError(1);
		CloseHandle(file);
		return false;
	}

	DWORD OutSize = 0;

	if (ReadFile(file, this->m_buff, this->m_size, &OutSize, nullptr) == 0)
	{
		this->SetLastError(2);
		CloseHandle(file);
		return false;
	}

	CloseHandle(file);

	this->m_count = 0;

	this->m_tick = GetTickCount();

	return true;
}

bool CMemScript::GetBuffer(char* buff, DWORD* size) // Updated
{
	if (this->m_buff == nullptr)
	{
		this->SetLastError(3);
		return false;
	}
	memcpy_s(buff, *size, this->m_buff, this->m_size);
	*size = this->m_size;
	return true;
}

char CMemScript::GetChar() // Updated
{
	if (this->m_count >= this->m_size)
	{
		return -1;
	}
	return this->m_buff[this->m_count++];
}

void CMemScript::UnGetChar(char ch) // Updated
{
	if (this->m_count == 0)
	{
		return;
	}

	this->m_buff[--this->m_count] = ch;
}

char CMemScript::CheckComment(char ch) // Updated
{
	if (ch != '/')
	{
		return ch;
	}

	char next_char = this->GetChar();
	if (next_char != '/')
	{
		this->UnGetChar(next_char);
		return ch;
	}

	while (true)
	{
		next_char = this->GetChar();
		if (next_char == -1)
		{
			return ch;
		}

		if (next_char == '\n')
		{
			return ch;
		}
	}
}

eTokenResult CMemScript::GetToken() // Updated
{
	if ((GetTickCount() - this->m_tick) > 1000)
	{
		this->SetLastError(4);
		throw 1;
	}

	this->m_number = 0;
	memset(this->m_string, 0, sizeof(this->m_string));

	char ch;

	while (true)
	{
		ch = this->GetChar();
		if (ch == -1)
		{
			return TOKEN_END;
		}

		if (isspace(ch) != 0)
		{
			continue;
		}

		if (ch != '\n' && ch != this->CheckComment(ch))
		{
			break;
		}
	}

	if (ch == '-' || ch == '.' || ch == '*' || ch >= '0' && ch <= '9')
	{
		return this->GetTokenNumber(ch);
	}

	if (ch == '"')
	{
		return this->GetTokenString(ch);
	}

	return this->GetTokenCommon(ch);
}

eTokenResult CMemScript::GetTokenNumber(char ch) // Updated
{
	int count = 0;

	this->UnGetChar(ch);

	while ((ch = this->GetChar()) != -1 && (ch == '-' || ch == '.' || ch == '*' || isdigit(ch)))
	{
		this->m_string[count++] = ch;
	}

	if (strcmp(this->m_string, "*") == 0)
	{
		this->m_number = -1;
	}
	else
	{
		this->m_number = std::stof(this->m_string);
	}

	this->m_string[count] = 0;

	return TOKEN_NUMBER;
}

eTokenResult CMemScript::GetTokenString(char ch) // Updated
{
	int count = 0;

	while ((ch = this->GetChar()) != -1 && ch != '"')
	{
		this->m_string[count++] = ch;
	}

	if (ch != '"')
	{
		this->UnGetChar(ch);
	}

	this->m_string[count] = 0;

	return TOKEN_STRING;
}

eTokenResult CMemScript::GetTokenCommon(char ch) // Updated
{
	if (isalpha(ch) == 0)
	{
		return TOKEN_ERROR;
	}

	int count = 0;

	this->m_string[count++] = ch;

	while ((ch = this->GetChar()) != -1 && (ch == '.' || ch == '_' || isalnum(ch)))
	{
		this->m_string[count++] = ch;
	}

	this->UnGetChar(ch);

	this->m_string[count] = 0;

	return TOKEN_STRING;
}

void CMemScript::SetLastError(int error) // OK
{
	switch (error)
	{
	case 0:
		wsprintf(this->m_LastError, MEM_SCRIPT_ERROR_CODE0, this->m_path);
		break;
	case 1:
		wsprintf(this->m_LastError, MEM_SCRIPT_ERROR_CODE1, this->m_path);
		break;
	case 2:
		wsprintf(this->m_LastError, MEM_SCRIPT_ERROR_CODE2, this->m_path);
		break;
	case 3:
		wsprintf(this->m_LastError, MEM_SCRIPT_ERROR_CODE3, this->m_path);
		break;
	case 4:
		wsprintf(this->m_LastError, MEM_SCRIPT_ERROR_CODE4, this->m_path);
		break;
	default:
		wsprintf(this->m_LastError, MEM_SCRIPT_ERROR_CODEX, this->m_path, error);
		break;
	}
}

char* CMemScript::GetLastError() // OK
{
	return this->m_LastError;
}

int CMemScript::GetNumber() // OK
{
	return (int)this->m_number;
}

int CMemScript::GetAsNumber() // OK
{
	this->GetToken();

	return (int)this->m_number;
}

float CMemScript::GetFloatNumber() // OK
{
	return this->m_number;
}

float CMemScript::GetAsFloatNumber() // OK
{
	this->GetToken();

	return this->m_number;
}

char* CMemScript::GetString() // OK
{
	return this->m_string;
}

char* CMemScript::GetAsString() // OK
{
	this->GetToken();

	return this->m_string;
}
