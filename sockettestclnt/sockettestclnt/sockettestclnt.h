#pragma once

#include "resource.h"
#include <WinSock2.h>
#include <ws2tcpip.h>    

#pragma comment(lib, "ws2_32.lib")

#define MAX 512
#define MAX_LOADSTRING 100
#define ID_EDIT 101
#define IDM_BTN_ServConn 102				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_ServDC 103				// ���� ���� �޼����� ���� �ڵ尪

DWORD WINAPI connServ(LPVOID Param);