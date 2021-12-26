#include <iostream>
#include <filesystem>

#include <Core/FileSystem.h>

namespace VKT {
    FileSystem *g_FileSystem = new FileSystem();
}

using namespace VKT;

int main(int, char**)
{
    g_FileSystem->Initialize();

    std::string root = g_FileSystem->GetRoot();

    for (const auto & entry : std::filesystem::directory_iterator(root))
        std::cout << entry.path() << std::endl;

    return 0;
}