#pragma once

#define MAX 512
#define MAX_LOADSTRING 100
#define ID_EDIT 101

#define IDM_BTN_TEST1 204
#define IDM_BTN_TEST2 205
#define IDM_BTN_TEST3 206

#define IDM_BTN_ServConn 1000				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_ServDC 1001				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_SendFile 1002

#define SC_WIDTH 1280
#define SC_HEIGHT 800

#define chatX 300

#include "resource.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windowsx.h>

#pragma comment(lib, "ws2_32.lib")

// ���� ��� ������ �Լ�
DWORD WINAPI connServ(LPVOID Param);

// ȭ�� ũ�� ������ ������ ���� �Լ�
void ScreenFix(LPARAM lParam);
void ScreenSet(HWND hWnd);

// ȭ�� ������ ���� �Լ�
void drawInit();
void drawIntoRoom(HDC hdc, HWND hWnd, HINSTANCE hInst);
void drawRoomList(HDC hdc);
void drawUserName(HDC hdc, HINSTANCE hInst);
void drawChatArea(HDC hdc, HWND hWnd, HINSTANCE hInst);
void drawUserList(HDC hdc);
void drawExit();