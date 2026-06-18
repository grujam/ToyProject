#include "stdafx.h"
#include "MainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    g_hInst = hInstance;
	CMainWindow* MainWnd = CMainWindow::GetInstance();
    if (!MainWnd)
        return 0;

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
	
	return 0;
}