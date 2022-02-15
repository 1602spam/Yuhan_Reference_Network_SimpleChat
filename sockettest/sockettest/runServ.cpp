#include "framework.h"
#include "sockettest.h"

// winsock2.h 사용을 위해 초기화
WSADATA wsaData;
// 클라이언트 소켓 배열, 서버 소켓
SOCKET hClntSock[MAX], hServSock;
// 서버 소켓, 클라이언트 소켓 주소
SOCKADDR_IN servAddr, clntAddr[MAX];
// 소켓 변화 감지를 위한 set와 원본 훼손 방지를 위한 cpset
fd_set set, cpset;
// 각각 select와 recv 함수 반환값 저장
int fd_num, chk_conn;
// 서버 실행 여부를 나타냄
bool servRunning;

WCHAR buf[MAX] = {};
WCHAR buf2[MAX] = {};

// app_print를 통한 출력 시 y값
int t_y;

// 각종 메시지 출력
void app_print(HWND hWnd, HDC hdc, const wchar_t* str);
// WSAStartup() 오류 핸들러
void WSAStartup_error(HWND hWnd, int code);
// bind() 오류 핸들러
void bind_error(HWND hWnd, int code);
// 서버 종료
void termServ(HWND hWnd, HDC hdc);

/*!
* @breif		서버의 동작시에 실행되는 스레드
* @details		해당 스레드가 서버의 역할을 하며 수신을 담당한다.
* @param		LPVOID	param	hWnd값 즉 윈도우 핸들값을 갖고 실행된다.
* @return		리턴값은 아무 의미를 갖지 않는다.
* @retval		1 정상종료
* @bug
* @warning
*/


DWORD WINAPI runServ(LPVOID Param)
{
	// 인자인 Param 값을 hWnd로 변경
	HWND hWnd = (HWND)Param;
	// bind할 포트 번호
	int portNumber = 10000;
	// 클라이언트 주소의 크기를 저장
	int szClntAddr;
	// select 함수에서 사용할 타임아웃
	timeval timeout;
	// 함수 반환값 및 반복문 처리할 임시 변수
	int i, j;
	// app_print y값 초기화
	t_y = 0;

	// Device Context 할당
	HDC hdc = GetDC(hWnd);
	// 그리기 영역 바탕을 투명하게 설정
	SetBkMode(hdc, TRANSPARENT);
	InvalidateRect(hWnd, NULL, TRUE);

	// 소켓 프로그램 시작부
	// DLL 초기화 및 애플리케이션 요구사항 충족 확인
	if ((i = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		// 오류 핸들러 호출
		WSAStartup_error(hWnd,i);
		// 스레드 종료
		servRunning = false;
		ExitThread(0);
	}

	// 서버 소켓 할당
	hServSock = socket(PF_INET, SOCK_STREAM, 0);

	// 소켓 할당 실패 시
	if (hServSock == INVALID_SOCKET)
	{
		// 오류 메시지 출력
		MessageBox(hWnd, L"socket() error!", L"에러", NULL);
		// 스레드 종료
		termServ(hWnd, hdc);
	}

	// 소켓 생성 완료 메시지 출력
	app_print(hWnd, hdc, L"Successfully created socket.");

	// servAddr 구조체의 메모리 초기화
	memset(&servAddr, 0x00, sizeof(servAddr));
	// IPv4 주소 체계를 사용
	servAddr.sin_family = AF_INET;
	// htonl == host to network large, 4바이트 호스트 방식 정렬 데이터를 네트워크 주소 방식 정렬
	// INADDR_ANY == 현재 사용 가능한 랜카드 IP주소
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// htons == host to network small, 2바이트 호스트 방식 정렬 데이터를 네트워크 주소 방식 정렬
	// 포트번호를 portNumber 변수로 초기화
	servAddr.sin_port = htons(portNumber);

	// 설정이 완료된 servAddr 구조체를 hServSock 소켓에 bind, 오류가 발생할 시
	if ((i = bind(hServSock, (sockaddr*)&servAddr, sizeof(servAddr))) == SOCKET_ERROR)
	{
		// 에러메시지 출력
		bind_error(hWnd, i);
		// 스레드 종료
		termServ(hWnd, hdc);
	}
	// bind 성공 시 메시지 출력
	app_print(hWnd, hdc, L"Bind succeed.");

	// 클라이언트의 연결 요청 대기, 대기열 크기 10
	listen(hServSock, 10);
	// listen 메시지 출력
	app_print(hWnd, hdc, L"Listening...");

	// file descriptor는 소켓이 들어가는 배열 구조체
	// FD_ZERO로 fd_set을 초기화, FD_SET으로 fd_set에 서버 소켓을 넣음
	FD_ZERO(&set);
	FD_SET(hServSock, &set);

	app_print(hWnd, hdc, L"Server is running...");
	
	//타임아웃 구조체 멤버 설정(초, 밀리초)
	timeout.tv_sec = 5;
	timeout.tv_usec = 5000;

	// 무한 반복 구문
	while (1)
	{
		// 서버 실행 중이 아니면 while문 탈출, 서버와 스레드 종료
		if (servRunning == false)
			break;

		// select문을 쓰면 fd_set 원형이 훼손되기 때문에 기존 set를 복사해서 사용
		cpset = set;
		
		// select문은 file descriptor의 변경을 감지, 현재는 fd_set에 서버 소켓 하나만 있는 상태
		// 인수는 차례로 (개수, 입력 감지할 세트, 출력 감지할 세트, 오류 감지할 세트, 타임아웃)
		fd_num = select(set.fd_count, &cpset, 0, 0, &timeout);

		// 오류 발생 시 while문 탈출, 서버와 스레드 종료
		if (fd_num == -1)
			break;
		// 변화값이 없는 경우 처리하지 않고 반복
		else if (fd_num == 0)
			continue;

		// set의 file descriptor 개수만큼 반복
		for (i = 0; i < set.fd_count; i++)
		{
			// 해당 소켓에서 변화가 발생했는지 판단
			if (FD_ISSET(set.fd_array[i], &cpset))
			{
				// 해당 소켓이 서버 소켓이라면 신규 연결 신호가 온 것
				if (set.fd_array[i] == hServSock)
				{
					// 클라이언트 주소 크기를 구하고 accept, accept로 연결 후 반환된 소켓은 클라이언트 소켓 배열에 할당
					szClntAddr = sizeof(clntAddr);
					hClntSock[i] = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);

					// 해당 소켓을 fd_set에 할당하고 fd_count보다 작다면 i로 설정
					FD_SET(hClntSock[i], &set);
					if (set.fd_count < i)
						set.fd_count = i;

					// 연결에 성공했다면 연결 성공 메시지 출력
					if (hClntSock[i] != -1) {
						wsprintf(buf, L"Connected Client: %d", hClntSock[i]);
						app_print(hWnd, hdc, buf);
						// 모든 클라이언트에 전송
						for (j = 0; j < set.fd_count; j++)
							send(set.fd_array[j], (char*)buf, MAX, 0);
					}
				}
				// 해당 소켓이 서버 소켓이 아닌 경우
				else
				{
					// 연결 확인을 위한 recv 함수 반환
					chk_conn = recv(set.fd_array[i], (char*)buf, MAX, 0);
					// 리턴 값이 0이면 정상 종료, 1이면 비정상 종료임을 나타냄
					if (chk_conn <= 0)
					{
						// 접속 종료 메시지 설정
						wsprintf(buf, L"Disconnected Client: %d", set.fd_array[i]);
						// 해당 소켓을 set에서 찾아 닫아줌
						closesocket(set.fd_array[i]);
						// 종료된 소켓이 있던 fd_set 내 배열 원소를 0으로 초기화
						FD_CLR(set.fd_array[i], &set);
						
						// 접속 종료 메시지 출력
						app_print(hWnd, hdc, buf);
						// 모든 클라이언트에 전송
						for (j = 0; j < set.fd_count; j++)
							send(set.fd_array[j], (char*)buf, MAX, 0);
					}
					// recv 함수가 정상 작동한 경우(반환값 1인 경우)
					else
					{
						// buf2에는 해당 소켓 번호를 저장
						wsprintf(buf2, L"%d         : ", set.fd_array[i]);
						// buf2 뒤에 recv한 메시지가 저장되어 있는 buf를 붙임
						wcscat(buf2, buf);
						// buf2를 출력
						// 846		: hello 와 같은 형태로 출력
						app_print(hWnd, hdc, buf2);
						// 모든 클라이언트에 전송
						for (j = 0; j < set.fd_count; j++)
							send(set.fd_array[j], (char*)buf2, MAX, 0);
					}
				}
			}
		}
	}

	// 서버 종료
	app_print(hWnd, hdc, L"Server Terminated.");
	termServ(hWnd, hdc);
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
		MessageBox(hWnd, L"bind error!", L"네트워크 하위시스템이 가동에 실패했습니다.", NULL);
		break;
	case WSAEACCES:
		MessageBox(hWnd, L"bind error!", L"액세스 위반으로 차단되었습니다.", NULL);
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
		MessageBox(hWnd, L"bind error!", L"메모리가 초과되었습니다.", NULL);
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
* @breif		메시지를 출력합니다.
* @details		서버 상태와 클라이언트 메시지를 출력합니다.
* @param		HDC		hdc		윈도우에 대한 핸들값
* @param		const	WHCAR*	str		출력할 문자열
* @return		void
*/

void app_print(HWND hWnd, HDC hdc, const wchar_t* str)
{
	TextOut(hdc, 10, 10+t_y, str, lstrlenW(str));
	t_y += 20;
	InvalidateRect(hWnd, NULL, 0);
}

void termServ(HWND hWnd, HDC hdc) {
	ReleaseDC(hWnd, hdc);
	closesocket(hServSock);
	WSACleanup();
	servRunning = false;
	ExitThread(0);
}