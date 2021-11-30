#pragma once

#include "Application.h"

extern VKT::Application *VKT::CreateApplication();

int main(int argc, char **argv)
{
    auto app = VKT::CreateApplication();

    app->Run();

    delete app;
}
