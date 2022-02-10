#include"framework.h"
#include"sockettest.h"


// ������κ��� ���� �о���� ���� ����ü ����
WSADATA wsaData;

// ���� ������ �����ϱ� ���� ����ü ����, Ŭ���̾�Ʈ�� ���� ���� ������ �����ϱ� ���� ����ü ����
SOCKET hClntSock[MAX];

// ������ �ּҰ��� �����ϱ����� ����ü ����, Ŭ���̾�Ʈ�� �ּҰ��� �����ϱ� ���� ����ü ����
SOCKADDR_IN servAddr, clntAddr[MAX];


fd_set set, cpset;

int fd_max, fd_num, chk_conn;

WCHAR buf[MAX] = {};
WCHAR buf2[MAX] = {};

/*!
* @breif    ������ ���۽ÿ� ����Ǵ� ������
* @details	�ش� �����尡 ������ ������ �ϸ� ������ ����Ѵ�.
* @param	LPVOID	param	hWnd�� �� ������ �ڵ鰪�� ���� ����ȴ�.
* @return   ���ϰ��� �ƹ� �ǹ̸� ���� �ʴ´�.
* @retval   1 ��������
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
		MessageBox(hWnd, L"winsock error!", L"����", NULL);
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
	{
		MessageBox(hWnd, L"socket() error!", L"����", NULL);
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
		MessageBox(hWnd, L"bind() ����", L"����", NULL);
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
			if (FD_ISSET(set.fd_array[i], &cpset))  //�迭 �� ���Ͽ��� ��ȭ ����
			{
				if (set.fd_array[i] == hServSock)   //�ش� ������ ���� �����̶�� ����
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
						//��� Ŭ���̾�Ʈ�� ������
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