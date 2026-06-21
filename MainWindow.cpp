#include "stdafx.h"
#include "MainWindow.h"

#include <memory>
#include <commctrl.h>

#pragma comment (lib, "comctl32.lib")

WNDCLASSEXW CMainWindow::m_Wnd = {};
HWND CMainWindow::m_WndHandle = nullptr;
HWND CMainWindow::m_ToolBarHandle = nullptr;
std::shared_ptr<CCanvas> CMainWindow::m_Canvas = nullptr;

LRESULT CMainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) 
    {
    case WM_CREATE:
    {
        CreateToolBar(hwnd);
        CreateCanvas();
        return 0;
    }
    case WM_COMMAND: {
        switch (wParam) {
        case ID_BRUSH_SELECT:       SetBrushMode(BrushMode::SELECT);        break;
        case ID_BRUSH_MULTIMOVE:    SetBrushMode(BrushMode::MULTIMOVE);     break;
        case ID_BRUSH_DOT:          SetBrushMode(BrushMode::DOT);           break;
        case ID_BRUSH_LINE:         SetBrushMode(BrushMode::LINE);          break;
        case ID_BRUSH_CIRCLE:       SetBrushMode(BrushMode::CIRCLE);        break;
        case ID_BRUSH_TRIANGLE:     SetBrushMode(BrushMode::TRIANGLE);      break;
        case ID_BRUSH_RECT:         SetBrushMode(BrushMode::RECTANGLE);     break;
        case ID_BRUSH_DELETE:       SetBrushMode(BrushMode::DELETE_SHAPE);  break;
        case ID_EDIT_CLEAR:
            m_Canvas->EraseAll();
            InvalidateRect(m_WndHandle, NULL, TRUE);
            break;
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        SetFocus(m_WndHandle);
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        RECT tbRect = {};

        if (m_ToolBarHandle) 
            GetWindowRect(m_ToolBarHandle, &tbRect);

        RECT wndRect = {};
        GetWindowRect(m_WndHandle, &wndRect);

        //범위 밖 확인
        int tbH = tbRect.bottom - tbRect.top;
        pt.y -= tbH;
        if (pt.y < 0) 
            return 0;

        m_Canvas->OnLeftClickDown(pt);
        InvalidateRect(m_WndHandle, NULL, FALSE);
        return 0;
    }

    case WM_MOUSEMOVE: {
        POINT pt = {LOWORD(lParam), HIWORD(lParam)};
        RECT tbRect = {};

        if (m_ToolBarHandle) 
            GetWindowRect(m_ToolBarHandle, &tbRect);

        int tbH = tbRect.bottom - tbRect.top;
        pt.y -= tbH;

        m_Canvas->OnMouseMove(pt);
        InvalidateRect(m_WndHandle, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONUP: {
        POINT pt = {LOWORD(lParam), HIWORD(lParam)};
        RECT tbRect = {};
        if (m_ToolBarHandle) 
            GetWindowRect(m_ToolBarHandle, &tbRect);

        int tbH = tbRect.bottom - tbRect.top;
        pt.y -= tbH;

        m_Canvas->OnLeftClickUp(pt);
        InvalidateRect(m_WndHandle, NULL, FALSE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_WndHandle, &ps);

        RECT clientRect;
        GetClientRect(m_WndHandle, &clientRect);

        int tbH = 0;
        if (m_ToolBarHandle) {
            RECT tbRect;
            GetClientRect(m_ToolBarHandle, &tbRect);
            tbH = tbRect.bottom;
        }

        int canvasW = clientRect.right;
        int canvasH = clientRect.bottom - tbH;

        if (canvasW <= 0) 
            canvasW = 1;

        if (canvasH <= 0) 
            canvasH = 1;

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, canvasW, canvasH);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBitmap);

        m_Canvas->Draw(memDC, canvasW, canvasH);

        BitBlt(hdc, 0, tbH, canvasW, canvasH, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBmp);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(m_WndHandle, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return LRESULT(TRUE);

    case WM_DESTROY:
        m_Canvas = nullptr;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void CMainWindow::CreateToolBar(HWND hwnd)
{
    m_ToolBarHandle = CreateWindowExW(0, TOOLBARCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)ID_TOOLBAR, g_hInst, NULL);

    SendMessage(m_ToolBarHandle, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    TBBUTTON tbButtons[9]
    {
        { 0, ID_BRUSH_SELECT, TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_CHECK | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Select" },
        { 1, ID_BRUSH_MULTIMOVE, TBSTATE_ENABLED, BTNS_CHECK | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"MultiMove" },
        { 2, ID_BRUSH_DOT, TBSTATE_ENABLED, BTNS_CHECK | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Point" },
        { 3, ID_BRUSH_LINE, TBSTATE_ENABLED, BTNS_CHECK | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Line" },
        { 4, ID_BRUSH_CIRCLE, TBSTATE_ENABLED, BTNS_CHECK | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Circle" },
        { 5, ID_BRUSH_TRIANGLE, TBSTATE_ENABLED, BTNS_CHECK | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Triangle" },
        { 6, ID_BRUSH_RECT, TBSTATE_ENABLED, BTNS_CHECK | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Rectangle" },
        { 7, ID_BRUSH_DELETE, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_CHECKGROUP, {0}, 0, (INT_PTR)L"Delete" },
        { 8, ID_EDIT_CLEAR, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Delete All" }
    };

    SendMessage(m_ToolBarHandle, TB_ADDBUTTONS, 9, (LPARAM)tbButtons);
    SendMessage(m_ToolBarHandle, TB_AUTOSIZE, 0, 0);
}

void CMainWindow::CreateCanvas()
{
    m_Canvas = std::make_shared<CCanvas>();
}

void CMainWindow::SetBrushMode(BrushMode eBrushMode)
{
    if (m_Canvas)
        m_Canvas->SetBrushMode(eBrushMode);
}
