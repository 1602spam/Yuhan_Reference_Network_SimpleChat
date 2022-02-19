#include "framework.h"
#include "sockettestclnt.h"

// ����� �޽��� ��� ����, ����� ����� ���
#define SHOW_DEBUG_LOG _

// �޽��� ó���� ���� ����
WCHAR buf[MAX];

// winsock2.h ����� ���� �ʱ�ȭ
WSADATA wsaData;

// Ŭ���̾�Ʈ ����
SOCKET hSocket;

// ���� �ּ�
SOCKADDR_IN servAddr;

// ���� ��ȭ ������ ���� set�� ���� �Ѽ� ������ ���� cpset
fd_set set, cpset;

// app_print �Լ����� ���ڿ� ����� ��ǥ
int t_y;

// ���� select�� recv �Լ� ��ȯ�� ����
int fd_num, chk_conn;

// Ŭ���̾�Ʈ ���� ����
bool clntConnected;

// ������ IP �ּ�
char connIP[] = "127.0.0.1";

// ������ ��Ʈ ��ȣ
int connPort = 10000;

// ���� ��� �Լ�
void app_print(HWND hWnd, HDC hdc, const wchar_t* str, POINT* pos);

// ���� ���� �Լ�
void termConn(HWND hWnd, HDC hdc);

DWORD WINAPI connServ(LPVOID Param)
{
    // Param�� HWND�� ��ȯ �� ����
    HWND hWnd = (HWND)Param;
    // Device Context �Ҵ�
    HDC hdc = GetDC(hWnd);
    // select �Լ����� ����� Ÿ�Ӿƿ�
    timeval timeout;

    POINT apos[2] = {
    {310, 10},
    {SC_WIDTH - 390, 80}
    };

    // �׸��� ���� ������ �����ϰ� ����
    SetBkMode(hdc, TRANSPARENT);
    InvalidateRect(hWnd, NULL, 1);
    
    // winsock �ʱ�ȭ ���� ��
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        // ���� �޽��� ���ۿ� �Է�
        wsprintf(buf, L"Winsock initialize failed.");

#ifdef SHOW_DEBUG_LOG
        app_print(hWnd, hdc, buf, &apos[0]); // SHOW_DEBUG_LOG ���ǵǾ��ٸ� ���
#endif

        // ������ ����
        termConn(hWnd, hdc);
    }
    else
        wsprintf(buf, L"Winsock initialize succeed.");

#ifdef SHOW_DEBUG_LOG
    app_print(hWnd, hdc, buf, &apos[0]);
#endif

    // ���� �Ҵ�
    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    // ���� �Ҵ� ���� ��
    if (hSocket == INVALID_SOCKET)
    {
        // ���� �޽��� ���ۿ� �Է�
        wsprintf(buf, L"Failed to create socket.");
#ifdef SHOW_DEBUG_LOG
        app_print(hWnd, hdc, buf, &apos[0]);
#endif
        termConn(hWnd, hdc);
    }
    else // �ƴ϶�� ���� �޽��� ���ۿ� �Է�
        wsprintf(buf, L"Successfully created socket.");

#ifdef SHOW_DEBUG_LOG
    app_print(hWnd, hdc, buf, &apos[0]);
#endif

    // �޸� �ʱ�ȭ
    memset(&servAddr, 0x00, sizeof(servAddr));
    // IPv4 �ּ�ü��
    servAddr.sin_family = AF_INET;
    // char ���ڿ��� �Էµ� �ּҸ� ���̳ʸ� ���·� ����
    inet_pton(AF_INET, connIP, &servAddr.sin_addr.s_addr);
    // htons == host to network small, 2����Ʈ ȣ��Ʈ ��� ���� �����͸� ��Ʈ��ũ �ּ� ��� ����
    // ��Ʈ��ȣ�� portNumber ������ �ʱ�ȭ
    servAddr.sin_port = htons(connPort);

#ifdef SHOW_DEBUG_LOG
    wsprintf(buf, L"Trying to connect...");
    app_print(hWnd, hdc, buf, &apos[0]);
#endif

    //���� ���� ��
    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        #ifdef SHOW_DEBUG_LOG
            wsprintf(buf, L"Connection failed.");
            app_print(hWnd, hdc, buf, &apos[0]);
        #endif
        // ������ ����
        termConn(hWnd, hdc);
    }
    else
    {
    #ifdef SHOW_DEBUG_LOG
        wsprintf(buf, L"Connection Succeed.");
        app_print(hWnd, hdc, buf, &apos[0]);
    #endif

        // fd_set �ʱ�ȭ
        FD_ZERO(&set);
        // fd_set�� hSocket �Ҵ�
        FD_SET(hSocket, &set);
        
        // Ÿ�Ӿƿ� ��� ����
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        while (1) {
            // ���� ���°� 0�̶�� while�� Ż�� �� ������ ����
            if (clntConnected == 0)
                break;

            // fd_set�� ���� ������ ���� ����
            cpset = set;
            // select���� ���� ��ȭ ����
            fd_num = select(set.fd_count, &cpset, 0, 0, &timeout);

            // ���� �߻� �� while�� Ż��, ������ ������ ����
            if (fd_num == -1)
                break;
            // ��ȭ���� ���� ��� ó������ �ʰ� �ݺ�
            else if (fd_num == 0)
                continue;
            
            // ���Ͽ� ��ȭ�� ���� ���(�ش� if���� ��� �� �� ����)
            if (FD_ISSET(hSocket, &cpset)) {
                // �����κ��� �޽��� ����
                chk_conn = recv(hSocket, (char*)buf, MAX, 0);
                // ���� �߻� ��
                if (chk_conn <= 0)
                {
                #ifdef SHOW_DEBUG_LOG
                    if (clntConnected) {
                        wsprintf(buf, L"Server error occurred.");
                        app_print(hWnd, hdc, buf, &apos[0]);
                    }
                #endif
                    break;
                }
                else // �������� �޽��� ���� ��
                {
                    // �ش� �޽��� ���
                    app_print(hWnd, hdc, buf, &apos[0]);
                }
            }
        }
    }
    wsprintf(buf, L"Disconnected.");
    app_print(hWnd, hdc, buf, &apos[0]);
    termConn(hWnd, hdc);
    return 0;
}

void app_print(HWND hWnd, HDC hdc, const wchar_t* str, POINT* pos)
{
    TextOut(hdc, pos->x, pos->y, str, lstrlenW(str));
    pos->y += 20;

    InvalidateRect(hWnd, NULL, 0);
}

void termConn(HWND hWnd, HDC hdc) {
    ReleaseDC(hWnd, hdc);
    closesocket(hSocket);
    WSACleanup();
    clntConnected = false;
    ExitThread(0);
}