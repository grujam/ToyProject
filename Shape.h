
#pragma once
#include <windows.h>
#include <vector>

enum class ShapeType 
{
    DOT,
    LINE,
    CIRCLE,
    TRIANGLE,
    RECTANGLE_SHAPE
};

struct ShapeHandle 
{
    POINT pos;
    int index;
};

class Shape abstract 
{
public:
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual void Draw(HDC hdc) const = 0;

    bool IsSelected() const { return m_bSelected; }
    void SetSelected(bool bSelected) { m_bSelected = bSelected; }

protected:
    bool m_bSelected = false;
};

class DotShape : public Shape 
{
public:
    POINT pt;
    DotShape(POINT p) : pt(p) {}
    ShapeType GetType() const override { return ShapeType::DOT; }

    void Draw(HDC hdc) const override;
};

class LineShape : public Shape 
{
public:
    POINT pt1, pt2;
    LineShape(POINT a, POINT b) : pt1(a), pt2(b) {}
    ShapeType GetType() const override { return ShapeType::LINE; }

    void Draw(HDC hdc) const override;
};

class CircleShape : public Shape 
{
public:
    POINT ptCenter;
    int radius;
    CircleShape(POINT c, int r) : ptCenter(c), radius(r) {}
    ShapeType GetType() const override { return ShapeType::CIRCLE; }

    void Draw(HDC hdc) const override;
};

class TriangleShape : public Shape 
{
public:
    POINT pts[3];
    TriangleShape(POINT a, POINT b, POINT c) { pts[0] = a; pts[1] = b; pts[2] = c; }
    ShapeType GetType() const override { return ShapeType::TRIANGLE; }
    
    //TODO: 직선 탐지 추가

    void Draw(HDC hdc) const override;
};

class RectShape : public Shape 
{
public:
    POINT pts[4];
    RectShape(POINT a, POINT b) 
    {
        CreateFromTwoPoints(a, b);
    }

private:
    void CreateFromTwoPoints(POINT a, POINT b);

    //TODO: 사각형 조건 추가

    ShapeType GetType() const override { return ShapeType::RECTANGLE_SHAPE; }

    void Draw(HDC hdc) const override;
};
