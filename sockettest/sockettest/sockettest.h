#pragma once

#include "resource.h"

#define MAX 512								// �ִ� ���� Ŭ���̾�Ʈ �� �� �ִ� ��Ŷ ������
#define MAX_LOADSTRING 100				
#define IDM_BTN_ServSTART 101				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_ServCLOSE 102				// ���� ���� �޼����� ���� �ڵ尪

/*!
* @breif		���� ���� �� ����Ǵ� ������
* @details		�ش� �����尡 ������ ������ �ϸ� ������ ����Ѵ�.
* @param		LPVOID	param	hWnd�� �� ������ �ڵ鰪�� ���� ����ȴ�.
* @return		���ϰ��� �ƹ� �ǹ̸� ���� �ʴ´�.
* @retval		1 ��������
* @bug			
* @warning		
*/
DWORD WINAPI runServ(LPVOID Param);