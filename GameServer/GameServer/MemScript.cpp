// Revised: 31/05/2023 22:35 GMT-3
// MemScript.cpp: implementation of the CMemScript class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemScript.h"

// ¡Construction!
CMemScript::CMemScript() : m_buff(nullptr), m_size(0), m_path{} {
	SetLastError(0);
}

// !Destruction¡
CMemScript::~CMemScript() {
	delete[] m_buff;
}

bool CMemScript::SetBuffer(const std::string& path) // OK
{
	strcpy_s(this->m_path, path.c_str());
	HANDLE file = CreateFile(this->m_path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		this->SetLastError(0);
		return false;
	}

	this->m_size = GetFileSize(file, 0);

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

	if (ReadFile(file, this->m_buff, this->m_size, &OutSize, 0) == 0)
	{
		this->SetLastError(2);
		CloseHandle(file);
		delete[] this->m_buff;
		this->m_buff = nullptr;
		return false;
	}

	CloseHandle(file);

	this->m_count = 0;

	this->m_tick = GetTickCount();

	return true;
}

bool CMemScript::GetBuffer(std::vector<char>& buff, DWORD& size) // OK
{
	if (this->m_buff == nullptr)
	{
		this->SetLastError(3);
		return false;
	}

	buff.assign(this->m_buff, this->m_buff + size);
	size = this->m_size;

	return true;
}

char CMemScript::GetChar() // OK
{
	if (m_count >= m_size) {
		return -1;
	}

	char c = m_buff[m_count];
	m_count++;
	return c;
}

void CMemScript::UnGetChar(char ch) // OK
{
	if (m_count == 0) {
		return;
	}

	--m_count;
	m_buff[m_count] = ch;
}

char CMemScript::CheckComment(char ch) // OK
{
	if (ch != '/' || (ch = GetChar()) != '/')
	{
		return ch;
	}

	char nextChar;
	for (nextChar = GetChar(); nextChar != -1 && nextChar != '\n'; nextChar = GetChar())
	{
		ch = nextChar;
	}

	return nextChar;
}

eTokenResult CMemScript::GetToken() // OK
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
		if ((ch = this->GetChar()) == -1)
		{
			return TOKEN_END;
		}

		if (isspace(ch) != 0)
		{
			continue;
		}

		if ((ch = this->CheckComment(ch)) == -1)
		{
			return TOKEN_END;
		}
		else if (ch != '\n')
		{
			break;
		}
	}

	if (ch == '-' || ch == '.' || ch == '*' || (ch >= '0' && ch <= '9'))
	{
		return this->GetTokenNumber(ch);
	}

	if (ch == '"')
	{
		return this->GetTokenString(ch);
	}

	return this->GetTokenCommon(ch);
}

eTokenResult CMemScript::GetTokenNumber(char ch) // OK
{
	int count = 0;
	this->UnGetChar(ch);

	while ((ch = this->GetChar()) != -1 && (ch == '-' || ch == '.' || ch == '*' || isdigit(ch) != 0))
	{
		this->m_string[count++] = ch;
	}

	if (strcmp(this->m_string, "*") == 0)
	{
		this->m_number = -1;
	}
	else
	{
		this->m_number = atof(this->m_string);
	}

	this->m_string[count] = '\0';

	return TOKEN_NUMBER;
}

eTokenResult CMemScript::GetTokenString(char ch) // OK
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

	this->m_string[count] = '\0';

	return TOKEN_STRING;
}

eTokenResult CMemScript::GetTokenCommon(char ch) // OK
{
	if (!std::isalpha(ch))
	{
		return TOKEN_ERROR;
	}

	std::string tokenStr;
	tokenStr.push_back(ch);

	while ((ch = this->GetChar()) != -1 && (ch == '.' || ch == '_' || std::isalnum(ch)))
	{
		tokenStr.push_back(ch);
	}

	this->UnGetChar(ch);

	std::strcpy(this->m_string, tokenStr.c_str());

	return TOKEN_STRING;
}

void CMemScript::SetLastError(int error) // OK
{
	switch (error)
	{
	case 0:
		std::snprintf(this->m_LastError, sizeof(this->m_LastError), MEM_SCRIPT_ERROR_CODE0, this->m_path);
		break;
	case 1:
		std::snprintf(this->m_LastError, sizeof(this->m_LastError), MEM_SCRIPT_ERROR_CODE1, this->m_path);
		break;
	case 2:
		std::snprintf(this->m_LastError, sizeof(this->m_LastError), MEM_SCRIPT_ERROR_CODE2, this->m_path);
		break;
	case 3:
		std::snprintf(this->m_LastError, sizeof(this->m_LastError), MEM_SCRIPT_ERROR_CODE3, this->m_path);
		break;
	case 4:
		std::snprintf(this->m_LastError, sizeof(this->m_LastError), MEM_SCRIPT_ERROR_CODE4, this->m_path);
		break;
	default:
		std::snprintf(this->m_LastError, sizeof(this->m_LastError), MEM_SCRIPT_ERROR_CODEX, this->m_path, error);
		break;
	}
}

char* CMemScript::GetLastError() // OK
{
	return this->m_LastError;
}

int CMemScript::GetNumber() // OK
{
	return static_cast<int>(this->m_number);
}

int CMemScript::GetAsNumber() // OK
{
	this->GetToken();
	return static_cast<int>(this->m_number);
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