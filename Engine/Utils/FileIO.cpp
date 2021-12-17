#include "FileIO.h"

#include <fstream>

namespace VKT {

    std::vector<char> ReadFile(const std::string &path, std::ios::openmode openmode)
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
}