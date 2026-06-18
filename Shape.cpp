#include "stdafx.h"
#include "Shape.h"

void DotShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    Ellipse(hdc, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);

    DeleteObject(hPen);
    DeleteObject(hBrush);
}

void LineShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, pt1.x, pt1.y, NULL);
    LineTo(hdc, pt2.x, pt2.y);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void CircleShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    Ellipse(hdc, ptCenter.x - radius, ptCenter.y - radius, ptCenter.x + radius, ptCenter.y + radius);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void TriangleShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    POINT poly[3] = { pts[0], pts[1], pts[2] };
    Polygon(hdc, poly, 3);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

void RectShape::CreateFromTwoPoints(POINT a, POINT b)
{
    POINT tl = {min(a.x,b.x), min(a.y,b.y)};
    POINT tr = {max(a.x,b.x), min(a.y,b.y)};
    POINT br = {max(a.x,b.x), max(a.y,b.y)};
    POINT bl = {min(a.x,b.x), max(a.y,b.y)};

    pts[0] = tl; pts[1] = tr; pts[2] = br; pts[3] = bl;
}

void RectShape::Draw(HDC hdc) const
{
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    POINT poly[4] = { pts[0],pts[1],pts[2],pts[3] };
    Polygon(hdc, poly, 4);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}
