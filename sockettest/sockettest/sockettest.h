#pragma once

#include "resource.h"

#define MAX 512								// �ִ� ���� Ŭ���̾�Ʈ �� �� �ִ� ��Ŷ ������
#define MAX_LOADSTRING 100				
#define IDM_BTN_ServSTART 101				// ���� ���� �޼����� ���� �ڵ尪
#define IDM_BTN_ServCLOSE 102				// ���� ���� �޼����� ���� �ڵ尪

#define SC_WIDTH 1424						// ������ â�� ���̸� ��Ÿ���ϴ�.
#define SC_HEIGHT 750						// ������ â�� ���̸� ��Ÿ���ϴ�.

/*!
* @breif		���� ���� �� ����Ǵ� ������
* @detail		�ش� �����尡 ������ ������ �ϸ� ������ ����Ѵ�.
* @param		LPVOID	param	hWnd�� �� ������ �ڵ鰪�� ���� ����ȴ�.
* @return		���ϰ��� �ƹ� �ǹ̸� ���� �ʴ´�.
* @retval		1 ��������
* @bug			
* @warning		
*/
DWORD WINAPI runServ(LPVOID Param);