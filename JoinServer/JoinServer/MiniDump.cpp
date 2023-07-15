// MiniDump.cpp: implementation of the CMiniDump class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniDump.h"

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LONG WINAPI DumpExceptionFilter(EXCEPTION_POINTERS* info)
{
    CHAR path[MAX_PATH];
    SYSTEMTIME systemTime;

    GetLocalTime(&systemTime);

    wsprintf(path, "%d-%02d-%02d_%02dh%02dm%02ds.dmp", systemTime.wYear, systemTime.wMonth, systemTime.wDay,
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

    HANDLE file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (file != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = info;
        mdei.ClientPointers = FALSE;

        if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, (MINIDUMP_TYPE)(MiniDumpScanMemory + MiniDumpWithIndirectlyReferencedMemory)
            , &mdei, nullptr, nullptr))
        {
            CloseHandle(file);
            return EXCEPTION_EXECUTE_HANDLER;
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void CMiniDump::Start() // OK
{
	SetErrorMode(SEM_FAILCRITICALERRORS);

	PreviousExceptionFilter = SetUnhandledExceptionFilter(DumpExceptionFilter);
}

void CMiniDump::Clean() // OK
{
	SetUnhandledExceptionFilter(PreviousExceptionFilter);
}
