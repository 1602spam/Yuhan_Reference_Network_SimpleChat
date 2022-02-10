#include"framework.h"
#include"sockettest.h"

// 통신으로부터 값을 읽어오기 위한 구조체 변수
WSADATA wsaData;

// 소켓 정보를 저장하기 위한 구조체 변수, 클라이언트에 대한 소켓 정보를 저장하기 위한 구조체 변수
SOCKET hClntSock[MAX];

// 소켓 디스크립터 값을 저장하기위한 구조체 변수, 클라이언트의 주소값을 저장하기 위한 구조체 변수
SOCKADDR_IN servAddr, clntAddr[MAX];

fd_set set, cpset;

int fd_max, fd_num, chk_conn;

WCHAR buf[MAX] = {};
WCHAR buf2[MAX] = {};

/*!
* @breif		서버의 동작시에 실행되는 스레드
* @details		해당 스레드가 서버의 역할을 하며 수신을 담당한다.
* @param		LPVOID	param	hWnd값 즉 윈도우 핸들값을 갖고 실행된다.
* @return		리턴값은 아무 의미를 갖지 않는다.
* @retval		1 정상종료
* @bug
* @warning
*/

void WSAStartup_error(HWND hWnd, int code);
void bind_error(HWND hWnd, int code);
void app_print(HDC hdc, const wchar_t* str);

DWORD WINAPI runServ(LPVOID Param)
{
	// 인자인 Param 값을 hWnd 값으로 변경
	HWND hWnd = (HWND)Param;
	int portNumber = 10000;
	int szClntAddr;
	int i = 0;
	timeval timeout;

	// DC 를 할당
	HDC hdc = GetDC(hWnd);

	// 글짜 배경 표시를 투명하게 변경
	SetBkMode(hdc, TRANSPARENT);
	// 윈도우 영역을 다시 그림
	InvalidateRect(hWnd, NULL, TRUE);

	// 소켓 프로그램 시작부
	// DLL초기화 및 애플리케이션 요구사항 충족 확인
	if ((i = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		// 결과에 따라 에러를 출력
		WSAStartup_error(hWnd,i);

		// 스레드 종료
		servRunning = false;
		ExitThread(0);
	}

	// 서버 소켓 할당
	// 반환값은 소켓 디스크립터
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	// 소켓 할당에 실패한 경우
	if (hServSock == INVALID_SOCKET)
	{
		// 에러메시지 출력
		MessageBox(hWnd, L"socket() error!", L"에러", NULL);
		// 소켓 통신 종료
		WSACleanup();

		// 스레드 종료
		WSACleanup();
		servRunning = false;
		ExitThread(0);
	}

	// 통신이 준비되었음을 알리는 코드
	app_print(hdc, L"Successfully created socket.");
	
	InvalidateRect(hWnd, NULL, 0);

	// servAddr 구조체의 메모리 초기화
	memset(&servAddr, 0x00, sizeof(servAddr));
	// IPv4 인터넷 프로토콜 주소 체계를 사용
	servAddr.sin_family = AF_INET;
	// 주소값을 0으로 초기화
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// 포트번호를 portNumber 변수로 초기화
	servAddr.sin_port = htons(portNumber);


	// 로컬 주소를 소켓과 연결
	if ((i = bind(hServSock, (sockaddr*)&servAddr, sizeof(servAddr))) == SOCKET_ERROR)
	{
		// 에러메시지 출력
		bind_error(hWnd, i);

		// 스레드 종료
		closesocket(hServSock);
		WSACleanup();
		servRunning = false;
		ExitThread(0);
	}
	else {
		// 성공 메시지 출력
		app_print(hdc, L"Bind succed");
		InvalidateRect(hWnd, NULL, 0);
	}

	// 네트워크 입력을 받음
	// 사용자가 들어올 때 까지 대기
	listen(hServSock, 10);

	app_print(hdc, L"Listening...");
	InvalidateRect(hWnd, NULL, 0);

	// file descriptor는 소켓이 들어가는 배열 구조체
	// FD_ZERO로 fd_set을 초기화, FD_SET으로 fd_set에 서버 소켓을 넣음
	FD_ZERO(&set);
	FD_SET(hServSock, &set);

	app_print(hdc, L"Server is running...");
	InvalidateRect(hWnd, NULL, 0);

	// 무한 반복 구문
	while (1)
	{
		//서버 실행 중이 아니면 while문 탈출, 서버와 스레드 종료
		if (servRunning == false)
			break;

		//select문을 쓰면 fd_set 원형이 훼손되기 때문에 기존 set를 복사해서 사용
		cpset = set;
		//타임아웃 구조체 멤버 설정(초, 밀리초)
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		//select문은 file descriptor의 변경을 감지, 현재는 fd_set에 서버 소켓 하나만 있는 상태
		//인수는 차례로 (개수, 입력 감지할 세트, 출력 감지할 세트, 오류 감지할 세트, 타임아웃)
		fd_num = select(set.fd_count, &cpset, 0, 0, &timeout);

		//오류 발생 시 while문 탈출, 서버와 스레드 종료
		if (fd_num == -1)
			break;
		//변화값이 없을 경우 처리하지 않고 반복
		else if (fd_num == 0)
			continue;

		//set의 file descriptor 개수만큼 반복
		for (i = 0; i < set.fd_count; i++)
		{
			//어느 소켓에서 변화가 발생했는지 판단
			if (FD_ISSET(set.fd_array[i], &cpset))
			{
				//해당 소켓이 서버 소켓이라면 신규 연결 신호가 온 것
				if (set.fd_array[i] == hServSock)
				{
					//클라이언트 주소 크기를 구하고 accept, 연결된 소켓은 클라이언트 소켓 배열에 할당
					szClntAddr = sizeof(clntAddr);
					hClntSock[i] = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);

					//해당 소켓을 fd_set에 할당하고 fd_count보다 작다면 i로 설정
					FD_SET(hClntSock[i], &set);
					if (set.fd_count < i)
						set.fd_count = i;

					//연결에 성공했다면 연결 성공 메시지 출력
					if (hClntSock[i] != -1) {
						wsprintf(buf, L"Connected Client: %d", hClntSock[i]);
						app_print(hdc, buf);
					}

					//app_print도 hdc, GetDC, ReleaseDC, InvalidateRect 쓰나요?
					InvalidateRect(hWnd, NULL, 0);
				}
				//해당 소켓이 서버 소켓이 아닌 경우
				else
				{
					//연결 확인을 위한 recv 함수 반환
					chk_conn = recv(set.fd_array[i], (char*)buf, MAX, 0);
					//리턴 값이 0이면 정상 종료, 1이면 비정상 종료임을 나타냄
					if (chk_conn <= 0)
					{
						//접속 종료 메시지 설정
						wsprintf(buf, L"Disconnected Client: %d", set.fd_array[i]);
						
						//해당 소켓을 set에서 찾아 종료
						closesocket(set.fd_array[i]);
						//종료된 소켓이 있던 fd_set 내 배열 원소를 0으로 초기화
						FD_CLR(set.fd_array[i], &set);
						
						//접속 종료 메시지 출력
						app_print(hdc, buf);
						InvalidateRect(hWnd, NULL, 0);
					}
					//recv 함수가 정상 작동한 경우(반환값 1인 경우)
					else
					{
						//buf2에는 해당 소켓 번호를 저장
						wsprintf(buf2, L"%d         : ", set.fd_array[i]);
						//buf2 뒤에 recv한 메시지가 저장되어 있는 buf를 붙임
						wcscat(buf2, buf);
						//buf2를 출력
						//846		: hello 와 같은 형태로 출력
						app_print(hdc, buf2);
						InvalidateRect(hWnd, NULL, 0);
					}
				}
			}
		}
	}

	// 서버 종료
	app_print(hdc, L"Server Terminated.");
	InvalidateRect(hWnd, NULL, 0);
	ReleaseDC(hWnd, hdc);

	// 스레드 종료
	closesocket(hServSock);
	WSACleanup();
	ExitThread(0);
	return 0;
}



/*!
* @breif		WSAStartup 함수의 오류메시지를 처리한다.
* @details		WSAStartup 함수가 반환하는 오류값에 따라 에러메시지를 표시한다.
* @details		reference : https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
* @param		HWND	hWnd		윈도우에 대한 핸들값
* @param		int		code		WSAStartup이 반환하는 코드값
* @return		void
*/
void WSAStartup_error(HWND hWnd, int code)
{
	switch (code)
	{
	case WSASYSNOTREADY:
		// The underlying network subsystem is not ready for network communication.
		MessageBox(hWnd, L"winsock error!", L"프로그램이 통신을 위한 준비가 되지 않았습니다.", NULL);
		break;
	case WSAVERNOTSUPPORTED:
		// The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.
		MessageBox(hWnd, L"winsock error!", L"windows 소켓 지원 버전이 제공되지 않습니다.", NULL);
		break;
	case WSAEINPROGRESS:
		// A blocking Windows Sockets 1.1 operation is in progress.
		MessageBox(hWnd, L"winsock error!", L"소켓 작업이 차단되었습니다.", NULL);
		break;
	case WSAEPROCLIM:
		// A limit on the number of tasks supported by the Windows Sockets implementation has been reached.
		MessageBox(hWnd, L"winsock error!", L"소켓의 갯수가 초과되었습니다.", NULL);
		break;
	case WSAEFAULT:
		// The lpWSAData parameter is not a valid pointer.
		MessageBox(hWnd, L"winsock error!", L"잘못된 호출이 진행되었습니다.", NULL);
		break;
	default:
		MessageBox(hWnd, L"winsock error!", L"에러", NULL);
		break;
	}
}

/*!
* @breif		bind 함수의 오류메시지를 처리한다.
* @details		bind 함수가 반환하는 오류값에 따라 에러메시지를 표시한다.
* @details		reference : https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-bind
* @param		HWND	hWnd		윈도우에 대한 핸들값
* @param		int		code		WSAStartup이 반환하는 코드값
* @return		void
*/
void bind_error(HWND hWnd, int code)
{
	switch (code)
	{
	case WSANOTINITIALISED:
		MessageBox(hWnd, L"bind error!", L"WSAStartup 함수가 호출되지 않았습니다.", NULL);
		break;	
	case WSAENETDOWN:
		MessageBox(hWnd, L"bind error!", L"네트워크 하위시스템이 가동에 실패 했습니다.", NULL);
		break;
	case WSAEACCES:
		MessageBox(hWnd, L"bind error!", L"액세스위반으로 차단되었습니다.", NULL);
		break;
	case WSAEADDRINUSE:
		MessageBox(hWnd, L"bind error!", L"해당 주소 또는 포트는 이미 사용중 입니다.", NULL);
		break;
	case WSAEADDRNOTAVAIL:
		MessageBox(hWnd, L"bind error!", L"유효하지 않은 주소입니다.", NULL);
		break;
	case WSAEFAULT:
		MessageBox(hWnd, L"bind error!", L"잘못된 포인터 참조가 발생했습니다.", NULL);
		break;
	case WSAEINPROGRESS:
		MessageBox(hWnd, L"bind error!", L"소켓 작업이 차단되었습니다.", NULL);
		break;
	case WSAEINVAL:
		MessageBox(hWnd, L"bind error!", L"잘못된 인수가 전달되었습니다.", NULL);
		break;
	case WSAENOBUFS:
		MessageBox(hWnd, L"bind error!", L"메모리가 초가되었습니다.", NULL);
		break;
	case WSAENOTSOCK:
		MessageBox(hWnd, L"bind error!", L"소켓이 아닙니다.", NULL);
		break;
	default:
		MessageBox(hWnd, L"bind error!", L"에러", NULL);
		break;
	}
}
/*!
* @breif		메시지의 출력 담당하는 메소드
* @details		메시지를 출력한다.
* @param		HDC		hdc		윈도우에 대한 핸들값
* @param		const	WHCAR*	str		출력할 문자열
* @return		void
*/
void app_print(HDC hdc, const wchar_t* str)
{
	static int i = 10;
	TextOut(hdc, 10, i, str, lstrlenW(str));
	i += 20;
}