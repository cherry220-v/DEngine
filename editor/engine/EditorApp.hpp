#pragma once	

#include <core/Application.h>

class EditorApp
{
public:
    static Application& instance()
    {
        return *s_app;
    }

    static void init(Application* app)
    {
        s_app = app;
    }

private:
    static Application* s_app;
};

