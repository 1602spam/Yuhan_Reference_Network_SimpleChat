#include"framework.h"
#include"sockettest.h"


// 통신으로부터 값을 읽어오기 위한 구조체 변수
WSADATA wsaData;

// 소켓 정보를 저장하기 위한 구조체 변수, 클라이언트에 대한 소켓 정보를 저장하기 위한 구조체 변수
SOCKET hClntSock[MAX];

// 서버의 주소값을 저장하기위한 구조체 변수, 클라이언트의 주소값을 저장하기 위한 구조체 변수
SOCKADDR_IN servAddr, clntAddr[MAX];


fd_set set, cpset;

int fd_max, fd_num, chk_conn;

WCHAR buf[MAX] = {};
WCHAR buf2[MAX] = {};

/*!
* @breif    서버의 동작시에 실행되는 스레드
* @details	해당 스레드가 서버의 역할을 하며 수신을 담당한다.
* @param	LPVOID	param	hWnd값 즉 윈도우 핸들값을 갖고 실행된다.
* @return   리턴값은 아무 의미를 갖지 않는다.
* @retval   1 정상종료
* @bug
* @warning
*/
DWORD WINAPI runServ(LPVOID Param)
{
	HWND hWnd;
	hWnd = (HWND)Param;
	int portNumber = 10000;
	int szClntAddr;
	int i, j = 0;
	timeval timeout;
	HDC hdc = GetDC(hWnd);
	SetBkMode(hdc, TRANSPARENT);
	InvalidateRect(hWnd, NULL, 1);

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

	TextOut(hdc, 10, 10 + j, L"Successfully created socket.", lstrlenW(L"Successfully created socket."));
	j += 20;
	InvalidateRect(hWnd, NULL, 0);

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
	else {
		TextOut(hdc, 10, 10 + j, L"Bind succeed.", lstrlenW(L"Bind succeed."));
		j += 20;
		InvalidateRect(hWnd, NULL, 0);
	}

	listen(hServSock, 10);

	TextOut(hdc, 10, 10 + j, L"Listening...", lstrlenW(L"Listening..."));
	j += 20;
	InvalidateRect(hWnd, NULL, 0);

	FD_ZERO(&set);
	FD_SET(hServSock, &set);

	TextOut(hdc, 10, 10 + j, L"Server is running...", lstrlenW(L"Server is running..."));
	j += 20;
	InvalidateRect(hWnd, NULL, 0);

	while (1)
	{
		if (servRunning == false)
			break;

		cpset = set;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		fd_num = select(set.fd_count, &cpset, 0, 0, &timeout);

		if (fd_num == -1)
			break;
		else if (fd_num == 0)
			continue;

		for (i = 0; i < set.fd_count; i++)
		{
			if (FD_ISSET(set.fd_array[i], &cpset))  //배열 내 소켓에서 변화 감지
			{
				if (set.fd_array[i] == hServSock)   //해당 소켓이 서버 소켓이라면 연결
				{
					szClntAddr = sizeof(clntAddr);
					hClntSock[i] = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
					FD_SET(hClntSock[i], &set);
					if (set.fd_count < i)
						set.fd_count = i;
					wsprintf(buf, L"Connected Client: %d", hClntSock[i]);
					if (hClntSock[i] != -1) {
						TextOut(hdc, 10, 10 + j, buf, lstrlenW(buf));
						j += 20;
					}
					InvalidateRect(hWnd, NULL, 0);
				}
				else
				{
					chk_conn = recv(set.fd_array[i], (char*)buf, MAX, 0);

					if (chk_conn <= 0)
					{
						wsprintf(buf, L"Disconnected Client: %d", set.fd_array[i]);
						closesocket(set.fd_array[i]);
						FD_CLR(set.fd_array[i], &set);
						TextOut(hdc, 10, 10 + j, buf, lstrlenW(buf));
						j += 20;
						InvalidateRect(hWnd, NULL, 0);
					}
					else
					{
						wsprintf(buf2, L"%d         : ", set.fd_array[i]);
						wcscat(buf2, buf);
						TextOut(hdc, 10, 10 + j, buf2, lstrlenW(buf2));
						j += 20;
						InvalidateRect(hWnd, NULL, 0);
						//모든 클라이언트에 재전송
					}
				}
			}
		}
	}
	TextOut(hdc, 10, 10 + j, L"Server Terminated.", lstrlenW(L"Server Terminated."));
	j += 20;
	InvalidateRect(hWnd, NULL, 0);
	ReleaseDC(hWnd, hdc);
	closesocket(hServSock);
	WSACleanup();
	ExitThread(0);
	return 0;
}