#pragma once

#include <string>
#include <fstream>
#include <vector>

namespace VKT {

    std::vector<char> ReadFile(const std::string &path, std::ios::openmode = std::ios::binary);
}