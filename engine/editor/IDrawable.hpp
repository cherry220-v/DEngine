#pragma once

struct EditorContext;

class IDrawable
{
public:
    virtual void draw(EditorContext&) = 0;
    virtual void update(EditorContext&) {};
};