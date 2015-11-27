#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP
#include "Globals.hpp"
#include <string>
#include <vector>
namespace Break{
	namespace Infrastructure{
		class BREAK_API Directory{
		
			std:: string m_name;
			std::string m_path;
		    bool m_opened;
		
		public:
			
			Directory();
			Directory(const std::string name, const std::string path);
			~Directory();
			bool close();
			bool open();
			std::string getPath();
			std::string getName();
			bool isOpened();
			void*  createDirectory(const std::string name, const  std::string path);
			static  bool isExist(const std::string path);
			bool changeDirectory(const std::string newPath);
			void getContent(std::vector<std::string>, std::string path);
		
		};
	};
};
#endif