#pragma once

#include "resource.h"
#include <WinSock2.h>
#include <ws2tcpip.h>    
#include <windowsx.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX 512
#define MAX_LOADSTRING 100
#define ID_EDIT 101
<<<<<<< HEAD
#define IDM_BTN_ServConn 102				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_ServDC 103				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_TEST1 204
#define IDM_BTN_TEST2 205
#define IDM_BTN_TEST3 206
=======
#define IDM_BTN_ServConn 1000				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_ServDC 1001				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_SendFile 1002
>>>>>>> 06054cc2b8ef529b92a309600285f2bad3226edf

DWORD WINAPI connServ(LPVOID Param);

// ȭ�� ũ�� ������ ������ ���� �Լ�
void ScreenFix(LPARAM lParam);
void ScreenSet(HWND hWnd);

// ȭ�� ������ ���� �Լ�
void drawInitScreen(HDC hdc, HWND hWnd, HINSTANCE hInst);
void drawIntoRoom(HDC hdc, HWND hWnd, HINSTANCE hInst);
void drawIntoSet(HDC hdc, HWND hWnd, HINSTANCE hInst);
void drawRoomList(HDC hdc);
void drawUserName(HDC hdc, HINSTANCE hInst);
void drawChatArea(HDC hdc, HWND hWnd, HINSTANCE hInst);
void drawUserList(HDC hdc);
void drawEmptyArea(HDC hdc);