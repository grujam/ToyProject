#include "stdafx.h"
#include "Canvas.h"
#include "Shape.h"
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
    if (m_BrushMode == BrushMode::MULTIMOVE && mode != BrushMode::MULTIMOVE)
    {
        for (const auto& pShape : m_vecMultiMove)
        {
            pShape->SetSelected(false);
        }
        m_vecMultiMove.clear();
    }

    m_BrushMode = mode;
    m_ToolMode = ToolMode::IDLE;
    m_vecDrawPTs.clear();
    m_bPreview = false;

    if (m_pSelected) {
        m_pSelected->SetSelected(false);
        m_pSelected = nullptr;
    }
}

void CCanvas::OnLeftClickDown(POINT pt) 
{
    if (m_BrushMode == BrushMode::SELECT) 
    {
        int nHandleIdx = -1;
        CShape* pHit = HitTestAll(pt, nHandleIdx);
        
        // 이미 선택 후 핸들을 고른 경우
        if (nHandleIdx >= 0 && pHit == m_pSelected) {
            m_ToolMode = ToolMode::EDIT;
            m_CurrentHandleIdx = nHandleIdx;
            m_draggingStartPt = pt;
            return;
        }

        // 처음 선택 시
        if (pHit) {
            if (m_pSelected && m_pSelected != pHit)
                m_pSelected->SetSelected(false);

            m_pSelected = pHit;
            pHit->SetSelected(true);
            m_ToolMode = ToolMode::MOVING;
            m_draggingStartPt = pt;
        }
        else {
            // 허공 선택 시
            if (m_pSelected)
            { 
                m_pSelected->SetSelected(false);
                m_pSelected = nullptr;
            }
            m_ToolMode = ToolMode::IDLE;
        }

        return;
    }
    else if (m_BrushMode == BrushMode::DELETE_SHAPE)
    {
        int nHandleIdx = -1;
        CShape* pHit = HitTestAll(pt, nHandleIdx);

        if (pHit) {
            auto it = std::find(m_vecShapes.begin(), m_vecShapes.end(), pHit);
            if (it != m_vecShapes.end()) {
                delete* it;
                m_vecShapes.erase(it);
            }

            m_pSelected = nullptr;
        }
        return;
    }
    else if (m_BrushMode == BrushMode::MULTIMOVE)
    {
        m_ToolMode = ToolMode::MOVING;
        return;
    }

    m_bPreview = true;
    m_PreviewPT = pt;

    switch (m_BrushMode) 
    {
    case BrushMode::DOT: {
        CDotShape* s = new CDotShape(pt);
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
        if (m_vecDrawPTs.size() == 2)
        {
            if(CTriangleShape::IsCollinear(m_vecDrawPTs[0], m_vecDrawPTs[1], pt))
                break;
        }

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

    if (m_ToolMode == ToolMode::MOVING) {
        if (m_BrushMode == BrushMode::MULTIMOVE)
        {
            int dx = pt.x - m_LastMousePT.x;
            int dy = pt.y - m_LastMousePT.y;
            for (auto& pShape : m_vecMultiMove)
            {
                pShape->Move(dx, dy);
            }
        }
        else if(m_pSelected)
        {
            int dx = pt.x - m_LastMousePT.x;
            int dy = pt.y - m_LastMousePT.y;
            m_pSelected->Move(dx, dy);
        }
    }
    else if (m_ToolMode == ToolMode::EDIT && m_pSelected) {
        m_pSelected->MoveHandle(m_CurrentHandleIdx, pt);
    }

    m_LastMousePT = pt;
}

CShape* CCanvas::HitTestAll(POINT pt, int& nHandleIdx)
{
    //핸들 가져오기
    if (m_pSelected) {
        auto handles = m_pSelected->GetHandles();
        for (const auto& h : handles) {
            if (abs(pt.x - h.pos.x) <= HIT_TOLERANCE && abs(pt.y - h.pos.y) <= HIT_TOLERANCE) {
                nHandleIdx = h.index;
                return m_pSelected;
            }
        }
    }

    // 역순으로 마지막에 생성된 애들부터
    for (int i = m_vecShapes.size() - 1; i >= 0; --i) {
        if (m_vecShapes[i]->IsHit(pt))
            return m_vecShapes[i];
    }

    return nullptr;
}

void CCanvas::OnLeftClickUp(POINT pt) 
{
    if (m_BrushMode == BrushMode::MULTIMOVE)
    {
        int nHandleIdx = -1;
        CShape* pHit = HitTestAll(pt, nHandleIdx);

        if (pHit) {
            if (pHit->IsSelected())
            {
                pHit->SetSelected(false);
                auto it = std::find(m_vecMultiMove.begin(), m_vecMultiMove.end(), pHit);
                if (it != m_vecMultiMove.end()) {
                    m_vecMultiMove.erase(it);
                }
            }
            else
            {
                pHit->SetSelected(true);
                m_vecMultiMove.push_back(pHit);
            }
        }
    }

    if (m_ToolMode == ToolMode::MOVING || m_ToolMode == ToolMode::EDIT) {
        m_ToolMode = ToolMode::IDLE;
    }
}

void CCanvas::FinalizeShape() 
{
    CShape* pShape = nullptr;
    switch (m_BrushMode) 
    {
    case BrushMode::LINE:
        pShape = new CLineShape(m_vecDrawPTs[0], m_vecDrawPTs[1]);
        break;
    case BrushMode::CIRCLE: 
    {
        double dx = m_vecDrawPTs[1].x - m_vecDrawPTs[0].x;
        double dy = m_vecDrawPTs[1].y - m_vecDrawPTs[0].y;
        int r = (int)sqrt(dx * dx + dy * dy);
        pShape = new CCircleShape(m_vecDrawPTs[0], r);
        break;
    }
    case BrushMode::TRIANGLE: 
    {
        CTriangleShape* tri = new CTriangleShape(m_vecDrawPTs[0], m_vecDrawPTs[1], m_vecDrawPTs[2]);
        pShape = tri;
        break;
    }
    case BrushMode::RECTANGLE: 
    {
        CRectShape* rect = new CRectShape(m_vecDrawPTs[0], m_vecDrawPTs[1]);
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
    m_pSelected = nullptr;
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

