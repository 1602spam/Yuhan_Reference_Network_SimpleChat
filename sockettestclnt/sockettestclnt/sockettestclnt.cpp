// sockettestclnt.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "sockettestclnt.h"

#define SC_WIDTH 1424						// 윈도우 창의 넓이를 나타냅니다.
#define SC_HEIGHT 750						// 윈도우 창의 높이를 나타냅니다.

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

int STATE = 0;                              // 출력할 화면을 구별하기 위한 변수

extern SOCKET hSocket;
extern WCHAR buf[MAX];
extern bool clntConnected;

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
    LoadStringW(hInstance, IDC_SOCKETTESTCLNT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SOCKETTESTCLNT));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOCKETTESTCLNT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SOCKETTESTCLNT);
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
HWND h_chatarea;
HWND h_SETbtn;
HWND h_MAINbtn;
HWND h_CHATbtn;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        h_MAINbtn = CreateWindow(L"button", L"MAIN", WS_CHILD | WS_VISIBLE,
            10, 10, 50, 50, hWnd, (HMENU)IDM_BTN_TEST1, hInst, NULL);
        h_CHATbtn = CreateWindow(L"button", L"CHAT", WS_CHILD | WS_VISIBLE,
            80, 10, 50, 50, hWnd, (HMENU)IDM_BTN_TEST2, hInst, NULL);
        h_SETbtn = CreateWindow(L"button", L"SET", WS_CHILD | WS_VISIBLE,
            230, 670, 50, 50, hWnd, (HMENU)IDM_BTN_TEST3, hInst, NULL);
        h_chatarea = CreateWindow(L"edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL,
            300, 700, 680, 100, hWnd, (HMENU)ID_EDIT, hInst, NULL);
        ShowWindow(h_chatarea, SW_HIDE);
        ShowWindow(h_MAINbtn, SW_HIDE);
        // 서버를 실행하는 버튼을 생성한다.
        // 이벤트	IDM_BTN_ServSTART	101
        CreateWindow(L"button", L"Connect", WS_CHILD | WS_VISIBLE,
<<<<<<< HEAD
            10, 610, 130, 30, hWnd, (HMENU)IDM_BTN_ServConn, hInst, NULL);
=======
            SC_WIDTH - 400, 0, 200, 60, hWnd, (HMENU)IDM_BTN_ServConn, hInst, NULL);
>>>>>>> 06054cc2b8ef529b92a309600285f2bad3226edf

        // 서버를 종료하는 버튼을 생성한다.
        // 이벤트	IDM_BTN_ServCLOSE	102
        CreateWindow(L"button", L"Disconnect", WS_CHILD | WS_VISIBLE,
<<<<<<< HEAD
            150, 610, 130, 30, hWnd, (HMENU)IDM_BTN_ServDC, hInst, NULL);

        // 클라이언트의 크기 설정
        ScreenSet(hWnd);
    }
        break;
    case WM_GETMINMAXINFO:  // 윈도우의 크기나 위치를 바꿀 때 발생되는 메시지
    {
        // 윈도우 크기 고정
        ScreenFix(lParam);
=======
            SC_WIDTH - 200, 0, 200, 60, hWnd, (HMENU)IDM_BTN_ServDC, hInst, NULL);
>>>>>>> 06054cc2b8ef529b92a309600285f2bad3226edf
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

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            HDC hdc;
            hdc = GetDC(hWnd);
            switch (wmId)
            {
            case IDM_BTN_TEST1:
            {
                STATE = 0;
                ShowWindow(h_chatarea, SW_HIDE);
                ShowWindow(h_MAINbtn, SW_HIDE);
                ShowWindow(h_CHATbtn, SW_SHOW);
                ShowWindow(h_SETbtn, SW_SHOW);
                InvalidateRect(hWnd, NULL, TRUE);
            }
                break;
            case IDM_BTN_TEST2:
            {
                STATE = 1;
                ShowWindow(h_chatarea, SW_SHOW);
                ShowWindow(h_MAINbtn, SW_SHOW);
                InvalidateRect(hWnd, NULL, TRUE);
            }
                break;
            case IDM_BTN_TEST3:
            {
                STATE = 2;
                ShowWindow(h_chatarea, SW_HIDE);
                ShowWindow(h_MAINbtn, SW_SHOW);
                ShowWindow(h_CHATbtn, SW_HIDE);
                ShowWindow(h_SETbtn, SW_HIDE);
                InvalidateRect(hWnd, NULL, TRUE);
            }
                break;
            case IDM_BTN_ServConn:
                if (clntConnected == false)
                {
                    clntConnected = true;   //connServ 처리 과정 상 먼저 열어줘야 함
                    CreateThread(NULL, 0, connServ, (LPVOID)hWnd, 0, NULL);
                }
                break;

            case IDM_BTN_ServDC:
                {
                if (clntConnected)
                {
                    clntConnected = false;
                    WSACleanup(); //스레드 종료 시 WSACleanup을 실행하긴 하나 여기서 실행시켜 주지 않으면 bind 오류가 발생해서 필요해요
                    if (!hSocket)
                    {
                        closesocket(hSocket);
                        hSocket = NULL;
                    }
                }
                }
                break;

            case IDM_BTN_SendFile:
                {
                }
                break;

            case ID_EDIT:
                if (HIWORD(wParam) == EN_UPDATE)
                {
                    int length = GetWindowText((HWND)lParam, buf, sizeof(buf) / sizeof(WCHAR));
                    if (buf[length - 2] == TEXT('\r'))
                    {
                        buf[length - 2] = TEXT('\0');
                        SetWindowText((HWND)lParam, TEXT(""));
                        if(hSocket)
                            send(hSocket, (char*)buf, MAX, 0);
                    }
                }
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                WSACleanup();
                closesocket(hSocket);
                DestroyWindow(hWnd);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            if(STATE == 0)
                drawInitScreen(hdc, hWnd, hInst);
            else if(STATE == 1)
                drawIntoRoom(hdc, hWnd, hInst);
            else if(STATE == 2)
                drawIntoSet(hdc, hWnd, hInst);
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