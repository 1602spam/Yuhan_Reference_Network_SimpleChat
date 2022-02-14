#pragma once

#include "resource.h"
#include <WinSock2.h>
#include <ws2tcpip.h>    

#pragma comment(lib, "ws2_32.lib")

#define MAX 512
#define MAX_LOADSTRING 100
#define ID_EDIT 101
#define IDM_BTN_ServConn 102				// 연결 시작 메세지에 대한 코드값
#define IDM_BTN_ServDC 103				// 연결 종료 메세지에 대한 코드값

DWORD WINAPI connServ(LPVOID Param);