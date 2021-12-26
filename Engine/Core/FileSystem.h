#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include "Buffer.h"

#include "Interface/IRuntimeModule.h"

namespace VKT {

    enum class OpenMode
    {
        TEXT   = 0, /// Open In Text Mode
        BINARY = 1, /// Open In Binary Mode
    };

    enum class SeekBase
    {
        SET = 0, /// SEEK_SET
        CUR = 1, /// SEEK_CUR
        END = 2  /// SEEK_END
    };

    class FileSystem : public IRuntimeModule
    {
    public:
        FileSystem() = default;
        ~FileSystem() override = default;

        int Initialize() override;
        void ShutDown() override {}

        void Tick() override {}

        std::string GetRoot() const { return m_ProjectRoot; }

        std::vector<char> ReadFile(const std::string &path, std::ios::openmode = std::ios::binary);

        std::string ParentPath(const std::string &path);
        std::string Append(const std::string &first, const std::string &second);

    private:
        std::string m_ProjectRoot;

    };

    extern FileSystem *g_FileSystem;
}