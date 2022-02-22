#include "framework.h"
#include "sockettest.h"

WSADATA wsaData;						// winsock2.h ����� ���� �ʱ�ȭ

SOCKET hClntSock[MAX], hServSock;		// Ŭ���̾�Ʈ ���� �迭, ���� ����

SOCKADDR_IN servAddr, clntAddr[MAX];	// ���� ����, Ŭ���̾�Ʈ ���� �ּ�

fd_set set, cpset;						// ���� ��ȭ ������ ���� set�� ���� �Ѽ� ������ ���� cpset

int fd_num, chk_conn;					// ���� select�� recv �Լ� ��ȯ�� ����

bool servRunning;						// ���� ���� ���θ� ��Ÿ��

WCHAR buf[MAX] = {};					//
WCHAR buf2[MAX] = {};					//runServ �����忡�� ���
WCHAR buf3[MAX] = {};

POINT apos[] = {
{10, 10},		//Chat log area
{800, 10}		//Clnt list area
};

vector <const WCHAR*> clntList;
int cntClnt = 0;

void app_print(HWND hWnd, HDC hdc, const wchar_t* str, POINT* pos);	// ���� �޽��� ���

void WSAStartup_error(HWND hWnd, int code);				// WSAStartup() ���� �ڵ鷯

void bind_error(HWND hWnd, int code);					// bind() ���� �ڵ鷯

void termServ(HWND hWnd, HDC hdc);						// ���� ����

int seekCommand(const wchar_t* str);

int getDestSock(wchar_t* str);

void sendToAll(const wchar_t* str);

void sendToSocket(const wchar_t* str, int j);

void cmdHelp(int i);

void aposClear();

/*!
* @breif		���� ���� �� ����Ǵ� ������
* @details		�ش� �����尡 ������ ������ �ϸ� ������ ����Ѵ�.
* @param		LPVOID	param	hWnd�� �� ������ �ڵ鰪�� ���� ����ȴ�.
* @return		���ϰ��� �ƹ� �ǹ̸� ���� �ʴ´�.
* @retval		1 ��������
* @bug
* @warning
*/
DWORD WINAPI runServ(LPVOID Param)
{
	HWND hWnd = (HWND)Param;		// ������ Param ���� hWnd�� ����
	
	int portNumber = 10000;			// bind�� ��Ʈ ��ȣ ����
	
	int szClntAddr;					// Ŭ���̾�Ʈ �ּ��� ũ�⸦ ����
	
	timeval timeout;				// select �Լ����� ����� Ÿ�Ӿƿ�
	
	int i, j, k;					// �Լ� ��ȯ�� �� �ݺ��� ó���� �ӽ� ����

	HDC hdc = GetDC(hWnd);			// Device Context �Ҵ�
	
	SetBkMode(hdc, TRANSPARENT);	// �׸��� ���� ������ �����ϰ� ����
	InvalidateRect(hWnd, NULL, TRUE);

	aposClear();

	// ���� ���α׷� ���ۺ�
	// DLL �ʱ�ȭ �� ���ø����̼� �䱸���� ���� Ȯ��
	if ((i = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		// ���� �ڵ鷯 ȣ��
		WSAStartup_error(hWnd, i);
		// ������ ����
		termServ(hWnd, hdc);
	}

	// ���� ���� �Ҵ�
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	// ���� �Ҵ� ���� ��
	if (hServSock == INVALID_SOCKET)
	{
		// ���� �޽��� ���
		MessageBox(hWnd, L"socket() error!", L"����", NULL);
		// ������ ����
		termServ(hWnd, hdc);
	}

	// ���� ���� �Ϸ� �޽��� ���
	app_print(hWnd, hdc, L"Successfully created socket.", &apos[0]);

	// servAddr ����ü�� �޸� �ʱ�ȭ
	memset(&servAddr, 0x00, sizeof(servAddr));
	// IPv4 �ּ� ü�踦 ���
	servAddr.sin_family = AF_INET;
	// htonl == host to network large, 4����Ʈ ȣ��Ʈ ��� ���� �����͸� ��Ʈ��ũ �ּ� ��� ����
	// INADDR_ANY == ���� ��� ������ ��ī�� IP�ּ�
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// htons == host to network small, 2����Ʈ ȣ��Ʈ ��� ���� �����͸� ��Ʈ��ũ �ּ� ��� ����
	// ��Ʈ��ȣ�� portNumber ������ �ʱ�ȭ
	servAddr.sin_port = htons(portNumber);

	// ������ �Ϸ�� servAddr ����ü�� hServSock ���Ͽ� bind, ������ �߻��� ��
	if ((i = bind(hServSock, (sockaddr*)&servAddr, sizeof(servAddr))) == SOCKET_ERROR)
	{
		// �����޽��� ���
		bind_error(hWnd, i);
		// ������ ����
		termServ(hWnd, hdc);
	}
	// bind ���� �� �޽��� ���
	app_print(hWnd, hdc, L"Bind succeed.", &apos[0]);

	// Ŭ���̾�Ʈ�� ���� ��û ���, ��⿭ ũ�� 10
	listen(hServSock, 10);
	// listen �޽��� ���
	app_print(hWnd, hdc, L"Listening...", &apos[0]);

	// file descriptor�� ������ ���� �迭 ����ü
	// FD_ZERO�� fd_set�� �ʱ�ȭ, FD_SET���� fd_set�� ���� ������ ����
	FD_ZERO(&set);
	FD_SET(hServSock, &set);

	app_print(hWnd, hdc, L"Server is running...", &apos[0]);

	//Ÿ�Ӿƿ� ����ü ��� ����(��, �и���)
	timeout.tv_sec = 5;
	timeout.tv_usec = 5000;

	// ���� �ݺ� ����
	while (1)
	{
		// ���� ���� ���� �ƴϸ� while�� Ż��, ������ ������ ����
		if (servRunning == false)
			break;

		// select���� ���� fd_set ������ �ѼյǱ� ������ ���� set�� �����ؼ� ���
		cpset = set;

		// select���� file descriptor�� ������ ����, ����� fd_set�� ���� ���� �ϳ��� �ִ� ����
		// �μ��� ���ʷ� (����, �Է� ������ ��Ʈ, ��� ������ ��Ʈ, ���� ������ ��Ʈ, Ÿ�Ӿƿ�)
		fd_num = select(set.fd_count, &cpset, 0, 0, &timeout);

		// ���� �߻� �� while�� Ż��, ������ ������ ����
		if (fd_num == -1)
			break;
		// ��ȭ���� ���� ��� ó������ �ʰ� �ݺ�
		else if (fd_num == 0)
			continue;

		// set�� file descriptor ������ŭ �ݺ�
		for (i = 0; i < (int)set.fd_count; i++)
		{
			// �ش� ���Ͽ��� ��ȭ�� �߻��ߴ��� �Ǵ�
			if (FD_ISSET(set.fd_array[i], &cpset))
			{
				// �ش� ������ ���� �����̶�� �ű� ���� ��ȣ�� �� ��
				if (set.fd_array[i] == hServSock)
				{
					// Ŭ���̾�Ʈ �ּ� ũ�⸦ ���ϰ� accept, accept�� ���� �� ��ȯ�� ������ Ŭ���̾�Ʈ ���� �迭�� �Ҵ�
					szClntAddr = sizeof(clntAddr);
					hClntSock[i] = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);

					// �ش� ������ fd_set�� �Ҵ��ϰ� fd_count���� �۴ٸ� i�� ����
					FD_SET(hClntSock[i], &set);
					if (set.fd_count < i)
						set.fd_count = i;

					WCHAR* arr = new WCHAR[MAX]();
					wsprintf(arr, L"%d", (int)hClntSock[i]);
					clntList.push_back(arr);
					cntClnt++;
					
					// ���ῡ �����ߴٸ� ���� ���� �޽��� ���
					if (hClntSock[i] != -1) {
						wsprintf(buf, L"Connected Client: %d", (int)hClntSock[i]);
						app_print(hWnd, hdc, buf, &apos[0]);
						// ��� Ŭ���̾�Ʈ�� ����
						sendToAll(buf);
					}
				}
				// �ش� ������ ���� ������ �ƴ� ���
				else
				{
					// ���� Ȯ���� ���� recv �Լ� ��ȯ
					chk_conn = recv(set.fd_array[i], (char*)buf, MAX, 0);
					// ���� ���� 0�̸� ���� ����, 1�̸� ������ �������� ��Ÿ��
					if (chk_conn <= 0)
					{
						wsprintf(buf3, L"%d", (int)hClntSock[i]);
						for (k = 0; k < clntList.size(); k++)
						{
							if (wcscmp(clntList[k], buf3) == 0)
							{
								const WCHAR* tmp2 = NULL;
								tmp2 = clntList[k];
								//clntList.erase(*(clntList.front()+k));
								delete tmp2;
								cntClnt--;
								break;
							}
						}
						// ���� ���� �޽��� ����
						wsprintf(buf, L"Disconnected Client: %d", (int)set.fd_array[i]);
						// �ش� ������ set���� ã�� �ݾ���
						closesocket(set.fd_array[i]);
						// ����� ������ �ִ� fd_set �� �迭 ���Ҹ� 0���� �ʱ�ȭ
						FD_CLR(set.fd_array[i], &set);
						// ���� ���� �޽��� ���
						app_print(hWnd, hdc, buf, &apos[0]);
						// ��� Ŭ���̾�Ʈ�� ����
						sendToAll(buf);
					}
					// recv �Լ��� ���� �۵��� ���(��ȯ�� 1�� ���)
					else
					{
						k = seekCommand(buf);
						switch(k) {
						
						case -1:// /w ��ɾ� ������ �������� ������ ���� ��ɾ� ��ȯ
						{
							wsprintf(buf2, L"[COMMAND] %d         : ", (int)set.fd_array[i]);
							wcscat(buf2, buf);
							app_print(hWnd, hdc, buf2, &apos[0]);

							wsprintf(buf2, L"[ERROR] Command is not completed, /help for more info!");
							app_print(hWnd, hdc, buf2, &apos[0]);
							sendToSocket(buf2, i);
						}
							break;
						case 0: // �޽����� Ŀ�ǵ�� �������� ������ ��� Ŭ���̾�Ʈ�� ����
							{
								// buf2���� �ش� ���� ��ȣ�� ����
								wsprintf(buf2, L"%d         : ", (int)set.fd_array[i]);
								// buf2 �ڿ� recv�� �޽����� ����Ǿ� �ִ� buf�� ����
								wcscat(buf2, buf);
								// buf2�� ���
								// 846		: hello �� ���� ���·� ���
								app_print(hWnd, hdc, buf2, &apos[0]);
								sendToAll(buf2);
							} break;
						// �޽����� /w Ŀ�ǵ�� �����Ѵٸ� 
						case 1: {
							wsprintf(buf2, L"[COMMAND] %d         : ", (int)set.fd_array[i]);
							wcscat(buf2, buf);
							app_print(hWnd, hdc, buf2, &apos[0]);
							// Ŀ�ǵ��� �����ڸ� �и��� ��ȯ
							j = getDestSock(buf);
							if (0 <= j) {
								if (set.fd_array[j] == set.fd_array[i]) {
									wsprintf(buf2, L"[ERROR] You can't DM yourself!");
									app_print(hWnd, hdc, buf2, &apos[0]);
									sendToSocket(buf2, i);
								}
								else {
									wsprintf(buf2, L"(DM) %d �� %d    : ", (int)set.fd_array[i], (int)set.fd_array[j]);
									wcscat(buf2, buf);
									app_print(hWnd, hdc, buf2, &apos[0]);
									sendToSocket(buf2, i);

									wsprintf(buf2, L"(DM) %d �� %d    : ", (int)set.fd_array[j], (int)set.fd_array[i]);
									wcscat(buf2, buf);
									sendToSocket(buf2, j);
								}
							}
							else
							{
								wsprintf(buf2, L"[ERROR] Could't find username!");
								app_print(hWnd, hdc, buf2, &apos[0]);
								sendToSocket(buf2, i);
							}
						}
							break;
						case 2:
						{
							wsprintf(buf2, L"[COMMAND] %d         : ", (int)set.fd_array[i]);
							wcscat(buf2, buf);
							app_print(hWnd, hdc, buf2, &apos[0]);
							cmdHelp(i);
						}
							break;
						default:
						{
						}
						break;
						}
					}
				}
			}
		}
	}

	// ���� ����
	app_print(hWnd, hdc, L"Server Terminated.", &apos[0]);
	termServ(hWnd, hdc);
	return 0;
}



/*!
* @breif		WSAStartup �Լ��� �����޽����� ó���Ѵ�.
* @details		WSAStartup �Լ��� ��ȯ�ϴ� �������� ���� �����޽����� ǥ���Ѵ�.
* @details		reference : https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
* @param		HWND	hWnd		�����쿡 ���� �ڵ鰪
* @param		int		code		WSAStartup�� ��ȯ�ϴ� �ڵ尪
* @return		void
*/
void WSAStartup_error(HWND hWnd, int code)
{
	switch (code)
	{
	case WSASYSNOTREADY:
		// The underlying network subsystem is not ready for network communication.
		MessageBox(hWnd, L"winsock error!", L"���α׷��� ����� ���� �غ� ���� �ʾҽ��ϴ�.", NULL);
		break;
	case WSAVERNOTSUPPORTED:
		// The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.
		MessageBox(hWnd, L"winsock error!", L"windows ���� ���� ������ �������� �ʽ��ϴ�.", NULL);
		break;
	case WSAEINPROGRESS:
		// A blocking Windows Sockets 1.1 operation is in progress.
		MessageBox(hWnd, L"winsock error!", L"���� �۾��� ���ܵǾ����ϴ�.", NULL);
		break;
	case WSAEPROCLIM:
		// A limit on the number of tasks supported by the Windows Sockets implementation has been reached.
		MessageBox(hWnd, L"winsock error!", L"������ ������ �ʰ��Ǿ����ϴ�.", NULL);
		break;
	case WSAEFAULT:
		// The lpWSAData parameter is not a valid pointer.
		MessageBox(hWnd, L"winsock error!", L"�߸��� ȣ���� ����Ǿ����ϴ�.", NULL);
		break;
	default:
		MessageBox(hWnd, L"winsock error!", L"����", NULL);
		break;
	}
}

/*!
* @breif		bind �Լ��� �����޽����� ó���Ѵ�.
* @details		bind �Լ��� ��ȯ�ϴ� �������� ���� �����޽����� ǥ���Ѵ�.
* @details		reference : https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-bind
* @param		HWND	hWnd		�����쿡 ���� �ڵ鰪
* @param		int		code		WSAStartup�� ��ȯ�ϴ� �ڵ尪
* @return		void
*/
void bind_error(HWND hWnd, int code)
{
	switch (code)
	{
	case WSANOTINITIALISED:
		MessageBox(hWnd, L"bind error!", L"WSAStartup �Լ��� ȣ����� �ʾҽ��ϴ�.", NULL);
		break;
	case WSAENETDOWN:
		MessageBox(hWnd, L"bind error!", L"��Ʈ��ũ �����ý����� ������ �����߽��ϴ�.", NULL);
		break;
	case WSAEACCES:
		MessageBox(hWnd, L"bind error!", L"�׼��� �������� ���ܵǾ����ϴ�.", NULL);
		break;
	case WSAEADDRINUSE:
		MessageBox(hWnd, L"bind error!", L"�ش� �ּ� �Ǵ� ��Ʈ�� �̹� ����� �Դϴ�.", NULL);
		break;
	case WSAEADDRNOTAVAIL:
		MessageBox(hWnd, L"bind error!", L"��ȿ���� ���� �ּ��Դϴ�.", NULL);
		break;
	case WSAEFAULT:
		MessageBox(hWnd, L"bind error!", L"�߸��� ������ ������ �߻��߽��ϴ�.", NULL);
		break;
	case WSAEINPROGRESS:
		MessageBox(hWnd, L"bind error!", L"���� �۾��� ���ܵǾ����ϴ�.", NULL);
		break;
	case WSAEINVAL:
		MessageBox(hWnd, L"bind error!", L"�߸��� �μ��� ���޵Ǿ����ϴ�.", NULL);
		break;
	case WSAENOBUFS:
		MessageBox(hWnd, L"bind error!", L"�޸𸮰� �ʰ��Ǿ����ϴ�.", NULL);
		break;
	case WSAENOTSOCK:
		MessageBox(hWnd, L"bind error!", L"������ �ƴմϴ�.", NULL);
		break;
	default:
		MessageBox(hWnd, L"bind error!", L"����", NULL);
		break;
	}
}

int seekCommand(const wchar_t* str)
{
	int strlen = 0;

	strlen = lstrlenW(str);

	if ((1 == strlen && wcsncmp(str, L"/", strlen) == 0) ||
		(2 == strlen && wcsncmp(str, L"/w", strlen) == 0) ||
		(3 == strlen && wcsncmp(str, L"/w ", strlen) == 0) ||
		(4 <= strlen && wcsncmp(str, L"/w  ", 4) == 0)) // ��ɾ� ������ �ҿ����� ��� -1 ��ȯ
		return -1;
	else if (wcsncmp(str, L"/w", 2) == 0 && 3 < strlen) // /w�� �����ϴ� �޽����� ��� �ӼӸ� ó���� ���� 1 ��ȯ
		return 1;

	if (wcsncmp(str, L"/help", 5) == 0) // /help �Է� �� ���� ó���� ���� 2 ��ȯ
		return 2;

	return 0;	// �ش��ϴ� Ŀ�ǵ尡 ���� ��� 0�� ��ȯ
}

void sendToAll(const wchar_t* str) {
	int i;
	for (i = 0; i < set.fd_count; i++)
		send(set.fd_array[i], (char*)str, MAX, 0);
}

int getDestSock(wchar_t* str) {
	wchar_t cpstr[MAX] = {};
	wchar_t* pt1;
	int i = -1, j = 0, slen = 0;

	wcscpy(cpstr, str);
	slen = lstrlenW(cpstr);

	while(j<2 && i<slen) {
		i++;
		if (wcsncmp(&cpstr[i], L" ", 1) == 0)
			j++;
	}

	wcscpy(str, &cpstr[i+1]);
	
	// ��ɾ /w 123 hello world�� ��
	// wide character �迭���� ���� ���� " " ���� ��ġ�� ��ȯ
	pt1 = wcstok(cpstr, L" ");
	// �� ���� ���� ���� " "�� �߻��ϱ� ���� ��ġ�� ��ȯ
	pt1 = wcstok(NULL, L" ");

	// fd_set�� ������ ������ �ִ��� Ȯ��
	for (i = 0; i < set.fd_count; i++)
		if (set.fd_array[i] == _wtoi(pt1))
			return i;

	return -1;
}

void cmdHelp(int i) {
	vector <const WCHAR*> help;
	help.push_back(L" ����[COMMAND]����");
	help.push_back(L"/help - Display all commands.");
	help.push_back(L"/w [Username] [Message] - Send DM to specific user.");
	help.push_back(L" ������������");
	for(const auto & j : help)
		sendToSocket(j, i);
}

void sendToSocket(const wchar_t* str, int i) {
	send(set.fd_array[i], (char*)str, MAX, 0);
}

void app_print(HWND hWnd, HDC hdc, const wchar_t* str, POINT* pos)
{
	TextOut(hdc, pos->x, pos->y, str, lstrlenW(str));
	pos->y += 20;

	InvalidateRect(hWnd, NULL, 0);
}

void aposClear() {
	apos[0] = { 10, 10 };		//Chat log area
	apos[1] = { 800, 10 };		//Clnt list area
}

void termServ(HWND hWnd, HDC hdc) {
	ReleaseDC(hWnd, hdc);
	closesocket(hServSock);
	WSACleanup();
	servRunning = false;
	clntList.clear();
	cntClnt = 0;
	ExitThread(0);
}