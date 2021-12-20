#include "EntryPoint.h"

#include <vector>
#include <iostream>

#include "Log.h"
#include "MemoryManager.h"
#include "InputManager.h"
#include "GraphicsManager.h"

#include "Application/Application.h"

#include "Interface/IRuntimeModule.h"

using namespace VKT;

int main(int argc, char **argv)
{
    Log::Init();

    if (g_App->Initialize() != 0)
    {
        std::cerr << "Application Initialization Failed." << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<IRuntimeModule*> runtimeModules;

    runtimeModules.push_back(g_MemoryManager);
    runtimeModules.push_back(g_InputManager);
    runtimeModules.push_back(g_GraphicsManager);

    // Initialize Runtime Modules
    for (auto it = runtimeModules.begin(); it != runtimeModules.end(); ++it)
    {
        if ((*it)->Initialize() != 0)
        {
            std::cerr << "Runtime Modules Initialization Failed." << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Main Loop
    while (g_App->IsRunning())
    {
        g_App->Tick();
        for (auto &module : runtimeModules)
            module->Tick();
    }

    // Shutdown Runtime Modules
    for (auto it = runtimeModules.rbegin(); it != runtimeModules.rend(); ++it)
    {
        (*it)->ShutDown();
    }

    g_App->ShutDown();

    return EXIT_SUCCESS;

}

