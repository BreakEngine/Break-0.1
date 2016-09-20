#pragma once

#include "Globals.hpp"
#include <string>
#include <vector>

namespace Break{
    namespace Infrastructure{
        class BREAK_API Directory{
        public:
            static bool changeDirectory(const std::string& newPath);
            static bool Exists(const std::string& path);
            static bool createDirectory(const std::string& folder);
            static std::string getCurrentDirectory();
            static std::vector<std::string> listContents();
            static bool Delete(const std::string& path);
        };
    }
}
