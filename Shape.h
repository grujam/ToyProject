
#pragma once
#include <windows.h>
#include <vector>

#define HIT_TOLERANCE 4

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
    ShapeHandle(POINT pt, int idx)
        : pos(pt), index(idx)
    {

    }

    POINT pos;
    int index;
};

class CCollider abstract
{
public:
    virtual ~CCollider() = default;
    virtual bool IsHit(POINT pt) const = 0;
};

class CShape abstract : public CCollider
{
public:
    virtual ~CShape() = default;
    virtual ShapeType GetType() const = 0;
    virtual void Draw(HDC hdc) const = 0;
    virtual void Move(int dx, int dy) = 0;

    bool IsSelected() const { return m_bSelected; }
    void SetSelected(bool bSelected) { m_bSelected = bSelected; }
    
    bool IsDotHit(POINT dotpt, POINT pt) const;

    void DrawHandles(HDC hdc) const;
   
    virtual std::vector<ShapeHandle> GetHandles() const { return {}; }
    virtual void MoveHandle(int index, POINT newPT) = 0;

protected:
    bool m_bSelected = false;
};

class CDotShape : public CShape 
{
public:
    POINT m_pt;
    CDotShape(POINT p) : m_pt(p) {}
    
    ShapeType GetType() const override { return ShapeType::DOT; }

    void Draw(HDC hdc) const override;
    void Move(int dx, int dy) override;
    bool IsHit(POINT pt) const override;

    std::vector<ShapeHandle> GetHandles() const override
    {
        return { {m_pt, 0} };
    }

    void MoveHandle(int index, POINT newPT);
};

class CLineShape : public CShape 
{
public:
    POINT pt1, pt2;
    CLineShape(POINT a, POINT b) : pt1(a), pt2(b) {}

    ShapeType GetType() const override { return ShapeType::LINE; }

    void Draw(HDC hdc) const override;
    void Move(int dx, int dy) override;
    bool IsHit(POINT pt) const override;

    std::vector<ShapeHandle> GetHandles() const override
    {
        return { {pt1, 0}, {pt2, 1} };
    }

    void MoveHandle(int index, POINT newPT);
};

class CCircleShape : public CShape 
{
public:
    POINT ptCenter;
    int radius;

    CCircleShape(POINT c, int r) : ptCenter(c), radius(r) {}

    ShapeType GetType() const override { return ShapeType::CIRCLE; }

    void Draw(HDC hdc) const override;
    void Move(int dx, int dy) override;
    bool IsHit(POINT pt) const override;

    std::vector<ShapeHandle> GetHandles() const override
    {
        POINT ptRadius = ptCenter;
        ptRadius.x += radius;
        return { {ptCenter, 0}, {ptRadius, 1} };
    }

    void MoveHandle(int index, POINT newPT);
};

class CTriangleShape : public CShape 
{
public:
    POINT pts[3];
    CTriangleShape(POINT a, POINT b, POINT c) 
    { 
        pts[0] = a; pts[1] = b; pts[2] = c; 
    }
    ShapeType GetType() const override { return ShapeType::TRIANGLE; }
    
    void Draw(HDC hdc) const override;
    void Move(int dx, int dy) override;
    bool IsHit(POINT pt) const override;

    std::vector<ShapeHandle> GetHandles() const override
    {
        return { {pts[0], 0}, {pts[1], 1}, {pts[2], 2}};
    }

    void MoveHandle(int index, POINT newPT);

public:
    static bool IsCollinear(POINT a, POINT b, POINT c)
    {
        double slope1 = (double)(a.x - b.x) / (a.y - b.y);
        double slope2 = (double)(a.x - c.x) / (a.y - c.y);

        
        return abs(slope1 - slope2) < 0.1;
    }
};

class CRectShape : public CShape 
{
public:
    POINT pts[4];
    CRectShape(POINT a, POINT b) 
    {
        CreateFromTwoPoints(a, b);
    }

    void CreateFromTwoPoints(POINT a, POINT b);

    ShapeType GetType() const override { return ShapeType::RECTANGLE_SHAPE; }

    void Draw(HDC hdc) const override;
    void Move(int dx, int dy) override;
    bool IsHit(POINT pt) const override;

    std::vector<ShapeHandle> GetHandles() const override
    {
        return { {pts[0], 0}, {pts[1], 1}, {pts[2], 2}, {pts[3], 3} };
    }

    void MoveHandle(int index, POINT newPT);

public:
    static bool IsConvex(POINT a, POINT b, POINT c, POINT d)
    {
        // 볼록 사각형 조건 → 모든 외적이 같은 부호
        double sign = 0;
        auto ccw = [&](POINT p1, POINT p2, POINT p3) {
            return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
        };

        double ccw1 = ccw(a, b, c) * ccw(a, b, d);
        double ccw2 = ccw(b, c, d) * ccw(b, c, a);
        double ccw3 = ccw(c, d, a) * ccw(c, d, b);
        double ccw4 = ccw(d, a, b) * ccw(d, a, c);

        if (ccw1 > 0 && ccw2 > 0 && ccw3 > 0 && ccw4 > 0) {
            return true;
        }

        return false;
    }
};
