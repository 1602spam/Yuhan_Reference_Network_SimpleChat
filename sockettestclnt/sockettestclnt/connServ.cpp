#include "framework.h"
#include "sockettestclnt.h"

WCHAR buf[MAX];
HANDLE handle;
WSADATA wsaData;
SOCKET hClntSock, hServSock;
SOCKADDR_IN servAddr;
fd_set set, cpset;

int fd_num, chk_conn;
bool clntConnected;
char IPaddress[] = "127.0.0.1";
int PortNumber = 10000;
int t_y;
void app_print(HWND hWnd, HDC hdc, const wchar_t* str);


DWORD WINAPI connServ(LPVOID Param)
{
    HWND hWnd = (HWND)Param;
    HDC hdc = GetDC(hWnd);
    timeval timeout;
    SetBkMode(hdc, TRANSPARENT);
    InvalidateRect(hWnd, NULL, 1);

    t_y = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        wsprintf(buf, L"Winsock initialize failed.");
    else
        wsprintf(buf, L"Winsock initialize succeed.");
    app_print(hWnd, hdc, buf);
    
    hClntSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hClntSock == INVALID_SOCKET)
    {
        wsprintf(buf, L"Failed to create socket.");
        WSACleanup();
        TerminateProcess(GetCurrentProcess(), 0);
    }
    else
        wsprintf(buf, L"Successfully created socket.");
    app_print(hWnd, hdc, buf);

    memset(&servAddr, 0x00, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr.s_addr); // CnC Server IP address
    servAddr.sin_port = htons(PortNumber); // CnC Server Port Number

    wsprintf(buf, L"Trying to connect...");
    app_print(hWnd, hdc, buf);

    if (connect(hClntSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        wsprintf(buf, L"Connection failed.");
        app_print(hWnd, hdc, buf);
    }
    else
    {
        wsprintf(buf, L"Connection Succeed.");
        app_print(hWnd, hdc, buf);

        FD_ZERO(&set);
        FD_SET(hServSock, &set);
        
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        while (1) {
            if (clntConnected == 0)
                break;

            cpset = set;
            fd_num = select(set.fd_count, &cpset, 0, 0, &timeout);

            //오류 발생 시 while문 탈출, 서버와 스레드 종료
            if (fd_num == -1)
                break;
            //변화값이 없을 경우 처리하지 않고 반복
            else if (fd_num == 0)
                continue;
            
            if (FD_ISSET(hServSock, &cpset)) {
                chk_conn = recv(hServSock, (char*)buf, MAX, 0);
                if (chk_conn <= 0)
                {
                    wsprintf(buf, L"Server error occurred.");
                    app_print(hWnd, hdc, buf);
                    break;
                }
                else
                {
                    app_print(hWnd, hdc, buf);
                }
            }
        }
    }

    app_print(hWnd, hdc, L"Disconnected.");
    ReleaseDC(hWnd, hdc);

    // 스레드 종료
    closesocket(hClntSock);
    WSACleanup();
    clntConnected = 0;
    ExitThread(0);
    return 0;
}

void app_print(HWND hWnd, HDC hdc, const wchar_t* str)
{
    TextOut(hdc, 10, 10 + t_y, str, lstrlenW(str));
    t_y += 20;
    InvalidateRect(hWnd, NULL, 0);
}