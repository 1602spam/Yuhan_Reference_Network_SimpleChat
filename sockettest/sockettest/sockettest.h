#pragma once

#include "resource.h"

#define MAX 512								// Ŭ���̾�Ʈ�� �ִ� ������ ��
#define MAX_LOADSTRING 100					
#define IDM_BTN_ServSTART 101				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_ServCLOSE 102				// ���� ���� �޼����� ���� �ڵ尪

// ���� ���� ���¿� ���� ����
bool servRunning = false;

// ������ ������ ������ ����ü ����
SOCKET hServSock = NULL;

DWORD WINAPI runServ(LPVOID Param);