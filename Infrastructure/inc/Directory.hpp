#pragma once
#include "Globals.hpp"
#include <string>
#include <vector>
namespace Break
{
	namespace Infrastructure
	{
		class BREAK_API Directory{
        public:
            Directory();
            ~Directory();
            void openD();
            void openD(std::string path);
            void closeD();
            static bool isExist();
            static bool isExist(std::string path);
            bool cd(std:: newPath);
            static createD();
            static createD(std::string path);
            bool isThere(std:: string fileName);
            std::vector<std:: string> getFiles();
			std::string getFilePath(std:: fileName)
        }
    }
}
