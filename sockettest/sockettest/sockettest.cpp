// sockettest.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "sockettest.h"
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX 512
#define MAX_LOADSTRING 100
#define IDM_BTN_ServSTART 101
#define IDM_BTN_ServCLOSE 102

#define TIMER_ID_RECVMS 1
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SOCKETTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SOCKETTEST));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOCKETTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SOCKETTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
WSADATA wsaData;
SOCKET hServSock, hClntSock;
SOCKADDR_IN servAddr, clntAddr;

bool flag=false;

WCHAR buf[MAX] = {};

DWORD WINAPI createSock(LPVOID Param)
{
    HWND hWnd;
    hWnd = (HWND)Param;
    int portNumber = 10000;

    

    int szClntAddr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        MessageBox(hWnd, L"winsock error!", L"에러", NULL);
    }
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
    {
        MessageBox(hWnd, L"socket() error!", L"에러", NULL);
        WSACleanup();
    }
    MessageBox(hWnd, L"socket succeed..", L"성공", NULL);

    memset(&servAddr, 0x00, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(portNumber);

    if (bind(hServSock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        MessageBox(hWnd, L"bind() 오류", L"에러", NULL);
        closesocket(hServSock);
        WSACleanup();
    }

    listen(hServSock, 10);
 
    szClntAddr = sizeof(clntAddr);

    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);

    if (hClntSock == INVALID_SOCKET || hClntSock == SOCKET_ERROR)
    {
        MessageBox(hWnd, L"accept() error", L"에러", NULL);
        closesocket(hServSock);
        WSACleanup();
    }
    while(true)
    {
        SendMessage(hWnd, WM_USER + 1, 0, 0);
        Sleep(2000);
        if (flag == false)
        {
            closesocket(hServSock);
            WSACleanup();
            ExitThread(0);
        }
    }
    //SetTimer(hWnd, TIMER_ID_RECVMS, 100, NULL);
    closesocket(hServSock);
    WSACleanup();
    ExitThread(0);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateWindow(L"button", L"Server Start", WS_CHILD | WS_VISIBLE, 500, 200, 200, 60, hWnd, (HMENU)IDM_BTN_ServSTART, hInst, NULL);
        CreateWindow(L"button", L"Server Close", WS_CHILD | WS_VISIBLE, 850, 200, 200, 60, hWnd, (HMENU)IDM_BTN_ServCLOSE, hInst, NULL);
    }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_BTN_ServSTART:
                if (flag != true)
                {
                    CreateThread(NULL, 0, createSock, (LPVOID)hWnd, 0, NULL);
                    
                    flag = true;
                }
                break;
            case IDM_BTN_ServCLOSE:
                if (flag == true)
                {
                    WSACleanup();
                    closesocket(hServSock);
                    MessageBox(hWnd, L"서버 종료", L"서버종료", NULL);
                    flag = false;
                }
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_TIMER:
    {
        switch (wParam)
        {
        case TIMER_ID_RECVMS:
        {
            
            break;
        }
        }
    }
        break;
    case WM_LBUTTONDOWN:
    {

    }
    break;
    case WM_RBUTTONDOWN:
    {

    }
        break;
    case WM_USER+1:
        if (flag == true && recv(hClntSock, (char*)buf, MAX, 0))
            MessageBox(hWnd, buf, buf, NULL);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
