#pragma once
#include "Globals.h"
#include "Canvas.h"

#include <windows.h>
#include <string>
#include <memory>

class CMainWindow
{
public:
	static CMainWindow* GetInstance()
	{
		static CMainWindow instance;
		return &instance;
	}

	CMainWindow()
	{
		m_Wnd.cbSize = sizeof(WNDCLASSEXW);
		m_Wnd.style = CS_HREDRAW | CS_VREDRAW;
		m_Wnd.lpfnWndProc = &CMainWindow::WndProc;
		m_Wnd.hInstance = g_hInst;
		m_Wnd.hIcon = NULL;
		m_Wnd.hCursor = LoadCursor(nullptr, IDC_ARROW);
		m_Wnd.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		m_Wnd.lpszMenuName = NULL;
		m_Wnd.lpszClassName = m_ClassName.c_str();
		m_Wnd.hIconSm = NULL;
		RegisterClassExW(&m_Wnd);

		m_WndHandle = CreateWindowExW(0, m_ClassName.c_str(), m_WindowName.c_str(), WS_OVERLAPPEDWINDOW, 0, 0, 1000, 1000, NULL, NULL, g_hInst, NULL);
		ShowWindow(m_WndHandle, SW_SHOW);
		UpdateWindow(m_WndHandle);
	}

	CMainWindow(const CMainWindow&) = delete;
	CMainWindow& operator=(const CMainWindow&) = delete;
	CMainWindow(CMainWindow&&) = delete;
	CMainWindow& operator=(CMainWindow&&) = delete;

private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static void CreateToolBar(HWND hwnd);
	static void CreateCanvas();
	static void SetBrushMode(BrushMode eBrushMode);

public:
	static WNDCLASSEXW m_Wnd;
	static HWND m_WndHandle;
	static HWND m_ToolBarHandle;

	const std::wstring m_ClassName = L"MainWindow";
	const std::wstring m_WindowName = L"∏ﬁ¿Œ √¢";

	static std::shared_ptr<CCanvas> m_Canvas;
};
