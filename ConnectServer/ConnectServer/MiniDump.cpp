// MiniDump.cpp: implementation of the CMiniDump class.
// Revisado: 14/07/23 16:18 GMT-3
// By: Qubit
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiniDump.h"

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LONG WINAPI DumpExceptionFilter(EXCEPTION_POINTERS* info)
{
    char path[MAX_PATH];
    SYSTEMTIME SystemTime;
    GetLocalTime(&SystemTime);
    _snprintf_s(path, MAX_PATH, "%d-%d-%d_%dh%dm%ds.dmp", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

    HANDLE file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (file != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = info;
        mdei.ClientPointers = 0;

        if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, (MINIDUMP_TYPE)(MiniDumpScanMemory | MiniDumpWithIndirectlyReferencedMemory), &mdei, 0, 0) != 0) {
            CloseHandle(file);
            return EXCEPTION_EXECUTE_HANDLER;
        }
    }

    CloseHandle(file);

    return EXCEPTION_CONTINUE_SEARCH;
}

void CMiniDump::Start()
{
    SetErrorMode(SEM_FAILCRITICALERRORS);

    PreviousExceptionFilter = SetUnhandledExceptionFilter(DumpExceptionFilter);
}

void CMiniDump::Clean()
{
    SetUnhandledExceptionFilter(PreviousExceptionFilter);
}
