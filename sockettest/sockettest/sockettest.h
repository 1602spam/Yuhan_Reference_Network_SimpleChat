#pragma once

#include "resource.h"

#define MAX 512								// 클라이언트의 최대 접속자 수
#define MAX_LOADSTRING 100					
#define IDM_BTN_ServSTART 101				// 서버 시작 메세지에 대한 코드값
#define IDM_BTN_ServCLOSE 102				// 서버 종료 메세지에 대한 코드값

// 서버 실행 상태에 대한 변수
bool servRunning = false;

// 소켓의 정보를 저장할 구조체 변수
SOCKET hServSock = NULL;

DWORD WINAPI runServ(LPVOID Param);