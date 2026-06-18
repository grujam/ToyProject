#pragma once
#include "Shape.h"

#include <vector>
#include <memory>

enum class BrushMode
{
    SELECT,
    DOT,
    LINE,
    CIRCLE,
    TRIANGLE,
    RECTANGLE,
    DELETE_SHAPE
};

enum class ToolMode 
{
    IDLE,
    DRAWING,
    MOVING,
    EDIT
};

class CCanvas 
{
public:
    CCanvas();
    ~CCanvas();
    void EraseAll();

    void Draw(HDC hdc, int width, int height);

    void SetBrushMode(BrushMode mode);

    void OnLeftClickDown(POINT pt);
    void OnLeftClickUp(POINT pt);

    void OnMouseMove(POINT pt);

private:
    std::vector<Shape*> m_vecShapes;
    Shape* m_bSelected = nullptr;
    BrushMode m_BrushMode = BrushMode::SELECT;
    ToolMode m_ToolMode = ToolMode::IDLE;

    std::vector<POINT> m_vecDrawPTs;

    POINT m_PreviewPT = {};
    bool m_bPreview = false;

    void FinalizeShape();
    void DoubleBuffering(HDC hdc, int w, int h);

    HBITMAP m_Bitmap;
    HDC m_DCHandle;
    int m_offW;
    int m_offH;
};
