#include "framework.h"
#include "sockettestclnt.h"

RECT r_username;
RECT r_roomlist;
RECT r_userlist;
RECT r_chatarea;
RECT r_emptyarea;

void ScreenSet(HWND hWnd) {
    int clnt_X = 1280, clnt_Y = 800;
    MoveWindow(hWnd, 300, 100, clnt_X, clnt_Y, true);

}
void ScreenFix(LPARAM lParam){
    ((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 1280;
    ((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 800;
    ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 1280;
    ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 800;
}

void drawInitScreen(HDC hdc, HWND hWnd, HINSTANCE hInst)
{
    drawRoomList(hdc);
    drawUserName(hdc, hInst);
    drawEmptyArea(hdc);
    
}

void drawIntoRoom(HDC hdc, HWND hWnd, HINSTANCE hInst) {
    drawRoomList(hdc);
    drawUserName(hdc, hInst);
    drawChatArea(hdc, hWnd, hInst);
    drawUserList(hdc);

    
}

void drawIntoSet(HDC hdc, HWND hWnd, HINSTANCE hInst) {
    drawRoomList(hdc);
    drawEmptyArea(hdc);

    
}
void drawRoomList(HDC hdc) {
    r_roomlist.left = 0;
    r_roomlist.top = 0;
    r_roomlist.right = 300;
    r_roomlist.bottom = 800;
    Rectangle(hdc, r_roomlist.left, r_roomlist.top, r_roomlist.right, r_roomlist.bottom);
}

void drawUserName(HDC hdc, HINSTANCE hInst) {
    r_username.left = 0;
    r_username.top = 650;
    r_username.right = 300;
    r_username.bottom = 800;
    Rectangle(hdc, r_username.left, r_username.top, r_username.right, r_username.bottom);
}

void drawChatArea(HDC hdc, HWND hWnd, HINSTANCE hInst) {
    r_chatarea.left = 300;
    r_chatarea.top = 0;
    r_chatarea.right = 980;
    r_chatarea.bottom = 800;
    Rectangle(hdc, r_chatarea.left, r_chatarea.top, r_chatarea.right, r_chatarea.bottom);
}

void drawUserList(HDC hdc) {
    r_userlist.left = 980;
    r_userlist.top = 0;
    r_userlist.right = 1280;
    r_userlist.bottom = 800;
    Rectangle(hdc, r_userlist.left, r_userlist.top, r_userlist.right, r_userlist.bottom);
}

void drawEmptyArea(HDC hdc) {
    r_emptyarea.left = 300;
    r_emptyarea.top = 0;
    r_emptyarea.right = 1280;
    r_emptyarea.bottom = 800;
    Rectangle(hdc, r_emptyarea.left, r_emptyarea.top, r_emptyarea.right, r_emptyarea.bottom);
}