#include "FileSystem.h"

#include <filesystem>
#include <fstream>

#include "Buffer.h"

namespace fs = std::filesystem;

namespace VKT {

    int FileSystem::Initialize()
    {
        // Search upward to find the project root directory
        // The directory that contains directory Resource is defined as project root directory
        fs::path path = fs::absolute(fs::current_path());

        for (int32_t i = 0; i < 10; i++)
        {
            if (std::filesystem::is_directory(path / "Resource"))
            {
                m_ProjectRoot = path.string();
                return 0;
            }

            path /= "..";
        }

        return -1;
    }

    std::vector<char> FileSystem::ReadFile(const std::string &path, std::ios::openmode openmode)
    {
        std::ifstream file(path, std::ios::ate | openmode);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    std::string FileSystem::ParentPath(const std::string &path)
    {
        std::filesystem::path p = path;
        return p.parent_path().string();
    }

    std::string FileSystem::Append(const std::string &first, const std::string &second)
    {
        std::filesystem::path p = first;
        return (p / second).string();
    }

}