#pragma once

#define WIN32_LEAN_AND_MEAN

#define _WIN32_WINNT _WIN32_WINNT_WINXP

constexpr const char* VERSION = "1.0.0.3";

constexpr const char* CONNECTSERVER_VERSION = "CS";
constexpr const char* CONNECTSERVER_CLIENT = "[Main.exe] OpenSource";

#ifndef PROTECT_STATE
#define PROTECT_STATE 0
#endif

// System Include
#include <windows.h>
#include <winsock2.h>
#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <math.h>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <Rpc.h>
#include <dbghelp.h>
#include <Psapi.h>
#include <CommCtrl.h>
#include <iostream>
#include <curl/curl.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Rpcrt4.lib")
#pragma comment(lib,"dbghelp.lib")
#pragma comment(lib,"Psapi.lib")

extern char CustomerName[32];
extern char CustomerHardwareId[36];
extern long MaxIpConnection;
extern char Version[9];

typedef unsigned __int64 QWORD;

#define _WINSOCK_DEPRECATED_NO_WARNINGS