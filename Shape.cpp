#include "stdafx.h"
#include "Shape.h"

void CDotShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    Ellipse(hdc, m_pt.x - HIT_TOLERANCE, m_pt.y - HIT_TOLERANCE, m_pt.x + HIT_TOLERANCE, m_pt.y + HIT_TOLERANCE);

    DeleteObject(hPen);
    DeleteObject(hBrush);

    if (m_bSelected)
        DrawHandles(hdc);
}

void CDotShape::Move(int dx, int dy)
{
    m_pt.x += dx;
    m_pt.y += dy;
}

bool CDotShape::IsHit(POINT pt) const
{
    return IsDotHit(m_pt, pt);
}

void CDotShape::MoveHandle(int index, POINT newPT)
{
    m_pt = newPT;
}

bool CShape::IsDotHit(POINT dotpt, POINT pt) const
{
    if (pt.x <= dotpt.x + HIT_TOLERANCE
        && pt.x >= dotpt.x - HIT_TOLERANCE
        && pt.y <= dotpt.y + HIT_TOLERANCE
        && pt.y >= dotpt.y - HIT_TOLERANCE)
        return true;

    return false;
}

void CShape::DrawHandles(HDC hdc) const
{
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 120, 215));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 70, 150));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    std::vector<ShapeHandle> vecHandles = GetHandles();
    for (const auto& h : vecHandles) {
        Rectangle(hdc, h.pos.x - HIT_TOLERANCE, h.pos.y - HIT_TOLERANCE, h.pos.x + HIT_TOLERANCE, h.pos.y + HIT_TOLERANCE);
    }
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void CLineShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, pt1.x, pt1.y, NULL);
    LineTo(hdc, pt2.x, pt2.y);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    if (m_bSelected)
        DrawHandles(hdc);
}

void CLineShape::Move(int dx, int dy)
{
    pt1.x += dx;
    pt1.y += dy;
    pt2.x += dx;
    pt2.y += dy;
}

bool CLineShape::IsHit(POINT pt) const
{
    double dx = pt2.x - pt1.x;
    double dy = pt2.y - pt1.y;
    double len = dx * dx + dy * dy;

    double t = ((pt.x - pt1.x) * dx + (pt.y - pt1.y) * dy) / len;
    t = max(0.0, min(1.0, t));

    double cx = pt1.x + t * dx;
    double cy = pt1.y + t * dy;

    double ddx = pt.x - cx;
    double ddy = pt.y - cy;
    return (ddx * ddx + ddy * ddy) <= HIT_TOLERANCE * HIT_TOLERANCE;
}

void CLineShape::MoveHandle(int index, POINT newPT)
{
    if (index == 0)
        pt1 = newPT;
    else if (index == 1)
        pt2 = newPT;
}

void CCircleShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    Ellipse(hdc, ptCenter.x - radius, ptCenter.y - radius, ptCenter.x + radius, ptCenter.y + radius);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    if (m_bSelected)
        DrawHandles(hdc);
}

void CCircleShape::Move(int dx, int dy)
{
    ptCenter.x += dx;
    ptCenter.y += dy;
}

bool CCircleShape::IsHit(POINT pt) const
{
    double dx = pt.x - ptCenter.x;
    double dy = pt.y - ptCenter.y;
    double dist = dx * dx + dy * dy;

    return dist <= ((double)radius * radius + HIT_TOLERANCE);
}

void CCircleShape::MoveHandle(int index, POINT newPT)
{
    if (index == 0)
        ptCenter = newPT;
    else if (index == 1)
    {
        double dx = newPT.x - ptCenter.x;
        double dy = newPT.y - ptCenter.y;
        int r = (int)sqrt(dx * dx + dy * dy);
        
        if(r > 0)
            radius = r;
    }
}

void CTriangleShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    POINT poly[3] = { pts[0], pts[1], pts[2] };
    Polygon(hdc, poly, 3);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    if (m_bSelected)
        DrawHandles(hdc);
}

void CTriangleShape::Move(int dx, int dy)
{
    for (int i = 0; i < 3; i++)
    {
        pts[i].x += dx;
        pts[i].y += dy;
    }
}

bool CTriangleShape::IsHit(POINT pt) const
{
    for (int i = 0; i < 3; i++)
    {
        int j = (i + 1) % 3;
        double cross = (double)(pts[j].x - pts[i].x) * (pt.y - pts[i].y) - (double)(pts[j].y - pts[i].y) * (pt.x - pts[i].x);

        if (cross < 0) 
            return false;
    }

    return true;
}

void CTriangleShape::MoveHandle(int index, POINT newPT)
{
    if (index < 0 || index > 2)
        return;

    POINT tmp = pts[index];
    pts[index] = newPT;

    if (IsCollinear(pts[0], pts[1], pts[2]))
    {
        pts[index] = tmp;
        return;
    }
}

void CRectShape::CreateFromTwoPoints(POINT a, POINT b)
{
    POINT tl = {min(a.x,b.x), min(a.y,b.y)};
    POINT tr = {max(a.x,b.x), min(a.y,b.y)};
    POINT br = {max(a.x,b.x), max(a.y,b.y)};
    POINT bl = {min(a.x,b.x), max(a.y,b.y)};

    pts[0] = tl; pts[1] = tr; pts[2] = br; pts[3] = bl;
}

void CRectShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    POINT poly[4] = { pts[0],pts[1],pts[2],pts[3] };
    Polygon(hdc, poly, 4);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    if (m_bSelected)
        DrawHandles(hdc);
}

void CRectShape::Move(int dx, int dy)
{
    for (int i = 0; i < 4; i++)
    {
        pts[i].x += dx;
        pts[i].y += dy;
    }
}

bool CRectShape::IsHit(POINT pt) const
{
    for (int i = 0; i < 4; i++)
    {
        int j = (i + 1) % 4;
        double cross = (double)(pts[j].x - pts[i].x) * (pt.y - pts[i].y) - (double)(pts[j].y - pts[i].y) * (pt.x - pts[i].x);

        if (cross < 0) 
            return false;
    }

    return true;
}

void CRectShape::MoveHandle(int index, POINT newPT)
{
    if (index < 0 || index > 3)
        return;

    POINT tmp = pts[index];
    pts[index] = newPT;

    if (!IsConvex(pts[0], pts[1], pts[2], pts[3]))
    {
        pts[index] = tmp;
        return;
    }
}