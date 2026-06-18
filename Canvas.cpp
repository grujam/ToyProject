#include "stdafx.h"
#include "Canvas.h"
#include <cstdio>
#include <cstdarg>
#include <algorithm>

CCanvas::CCanvas() 
    : m_Bitmap(NULL)
    , m_DCHandle(NULL)
{

}

CCanvas::~CCanvas() 
{
    EraseAll();

    if (m_DCHandle) 
        DeleteDC(m_DCHandle);

    if (m_Bitmap) 
        DeleteObject(m_Bitmap);
}

void CCanvas::SetBrushMode(BrushMode mode) 
{
    m_BrushMode = mode;
    m_ToolMode = ToolMode::IDLE;
    m_vecDrawPTs.clear();
    m_bPreview = false;

    if (m_bSelected) {
        m_bSelected->SetSelected(false);
        m_bSelected = nullptr;
    }
}

void CCanvas::OnLeftClickDown(POINT pt) 
{
    if (m_BrushMode == BrushMode::SELECT) 
    {
        //TODO: HitTest 추가
        return;
    }
    else if (m_BrushMode == BrushMode::DELETE_SHAPE)
    {
        //TODO: 도형 삭제 추가
        return;
    }

    m_bPreview = true;
    m_PreviewPT = pt;

    switch (m_BrushMode) 
    {
    case BrushMode::DOT: {
        DotShape* s = new DotShape(pt);
        m_vecShapes.push_back(s);
        break;
    }
    case BrushMode::LINE: {
        m_vecDrawPTs.push_back(pt);
        if (m_vecDrawPTs.size() == 2) {
            FinalizeShape();
        }
        break;
    }
    case BrushMode::CIRCLE: {
        m_vecDrawPTs.push_back(pt);
        if (m_vecDrawPTs.size() == 2) {
            FinalizeShape();
        }
        break;
    }
    case BrushMode::TRIANGLE: {
        m_vecDrawPTs.push_back(pt);
        if (m_vecDrawPTs.size() == 3) {
            FinalizeShape();
        }
        break;
    }
    case BrushMode::RECTANGLE: {
        m_vecDrawPTs.push_back(pt);
        if (m_vecDrawPTs.size() == 2) {
            FinalizeShape();
        }
        break;
    }
    default: break;
    }
}

void CCanvas::OnMouseMove(POINT pt) 
{
    m_PreviewPT = pt;
}

void CCanvas::OnLeftClickUp(POINT pt) 
{
    if (m_ToolMode == ToolMode::MOVING || m_ToolMode == ToolMode::EDIT) {
        m_ToolMode = ToolMode::IDLE;
    }
}

void CCanvas::FinalizeShape() 
{
    Shape* pShape = nullptr;
    switch (m_BrushMode) 
    {
    case BrushMode::LINE:
        pShape = new LineShape(m_vecDrawPTs[0], m_vecDrawPTs[1]);
        break;
    case BrushMode::CIRCLE: 
    {
        double dx = m_vecDrawPTs[1].x - m_vecDrawPTs[0].x;
        double dy = m_vecDrawPTs[1].y - m_vecDrawPTs[0].y;
        int r = (int)sqrt(dx * dx + dy * dy);
        pShape = new CircleShape(m_vecDrawPTs[0], r);
        break;
    }
    case BrushMode::TRIANGLE: 
    {
        TriangleShape* tri = new TriangleShape(m_vecDrawPTs[0], m_vecDrawPTs[1], m_vecDrawPTs[2]);
        pShape = tri;
        break;
    }
    case BrushMode::RECTANGLE: 
    {
        RectShape* rect = new RectShape(m_vecDrawPTs[0], m_vecDrawPTs[1]);
        pShape = rect;
        break;
    }
    default: 
        break;
    }

    if (pShape)
        m_vecShapes.push_back(pShape);

    m_vecDrawPTs.clear();
    m_bPreview = false;
}

void CCanvas::EraseAll() 
{
    for (auto* shape : m_vecShapes) 
        delete shape;

    m_vecShapes.clear();
    m_bSelected = nullptr;
    m_vecDrawPTs.clear();
    m_bPreview = false;
}

void CCanvas::DoubleBuffering(HDC hdc, int w, int h) 
{
    if (m_DCHandle && m_offW == w && m_offH == h) 
        return;

    if (m_DCHandle)
    {
        DeleteDC(m_DCHandle); 
        m_DCHandle = NULL;
    }

    if (m_Bitmap)
    {
        DeleteObject(m_Bitmap);
        m_Bitmap = NULL;
    }

    m_DCHandle = CreateCompatibleDC(hdc);
    m_Bitmap = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(m_DCHandle, m_Bitmap);
    m_offW = w; 
    m_offH = h;
}

void CCanvas::Draw(HDC hdc, int width, int height) 
{
    DoubleBuffering(hdc, width, height);

    RECT rc = { 0, 0, width, height };
    HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(m_DCHandle, &rc, bgBrush);
    DeleteObject(bgBrush);

    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(230, 230, 230));
    HPEN oldPen = (HPEN)SelectObject(m_DCHandle, gridPen);
    for (int x = 0; x < width; x += 20) 
    {
        MoveToEx(m_DCHandle, x, 0, NULL);
        LineTo(m_DCHandle, x, height);
    }
    for (int y = 0; y < height; y += 20) 
    {
        MoveToEx(m_DCHandle, 0, y, NULL);
        LineTo(m_DCHandle, width, y);
    }
    SelectObject(m_DCHandle, oldPen);
    DeleteObject(gridPen);

    for (auto* shape : m_vecShapes) 
    {
        shape->Draw(m_DCHandle);
    }

    if (m_bPreview && !m_vecDrawPTs.empty()) 
    {
        HPEN prevPen = CreatePen(PS_DOT, 1, RGB(100, 100, 200));
        HPEN pOld = (HPEN)SelectObject(m_DCHandle, prevPen);

        switch (m_BrushMode) 
        {
        case BrushMode::LINE:
            MoveToEx(m_DCHandle, m_vecDrawPTs[0].x, m_vecDrawPTs[0].y, NULL);
            LineTo(m_DCHandle, m_PreviewPT.x, m_PreviewPT.y);
            break;
        case BrushMode::CIRCLE: 
        {
            double dx = m_PreviewPT.x - m_vecDrawPTs[0].x;
            double dy = m_PreviewPT.y - m_vecDrawPTs[0].y;
            int r = (int)sqrt(dx * dx + dy * dy);
            if (r > 0)
                Ellipse(m_DCHandle,
                    m_vecDrawPTs[0].x - r, m_vecDrawPTs[0].y - r,
                    m_vecDrawPTs[0].x + r, m_vecDrawPTs[0].y + r);
        }
        break;
        case BrushMode::TRIANGLE:
        {
            if (m_vecDrawPTs.size() == 1) {
                MoveToEx(m_DCHandle, m_vecDrawPTs[0].x, m_vecDrawPTs[0].y, NULL);
                LineTo(m_DCHandle, m_PreviewPT.x, m_PreviewPT.y);
            }
            else if (m_vecDrawPTs.size() == 2) {
                POINT poly[3] = { m_vecDrawPTs[0], m_vecDrawPTs[1], m_PreviewPT };
                Polygon(m_DCHandle, poly, 3);
            }
        }
        break;
        case BrushMode::RECTANGLE: 
        {
            int x1 = min(m_vecDrawPTs[0].x, m_PreviewPT.x), y1 = min(m_vecDrawPTs[0].y, m_PreviewPT.y);
            int x2 = max(m_vecDrawPTs[0].x, m_PreviewPT.x), y2 = max(m_vecDrawPTs[0].y, m_PreviewPT.y);
            Rectangle(m_DCHandle, x1, y1, x2, y2);
        }
        break;
        default:
            break;
        }

        SelectObject(m_DCHandle, pOld);
        DeleteObject(prevPen);
    }

    BitBlt(hdc, 0, 0, width, height, m_DCHandle, 0, 0, SRCCOPY);
}

