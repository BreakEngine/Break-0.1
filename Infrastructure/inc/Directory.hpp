#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP
#include "Globals.hpp"
#include <string>
#include <vector>
namespace Break{
	namespace Infrastructure{
		enum class Permission:u8{READ,EDIT,FULL_PERMiSSION};
		class BREAK_API Directory{
			std:: string m_name;
			Permission m_accessPermission;
			std::string m_path;
			 bool m_opened;
		public:
			Directory();
			Directory(const std::string name,const std::string path,const  Permission permission=Permission::READ);
			 bool  createDirectory(const std::string name,const  std::string path,const  Permission permission);
			 bool createDirectory(const std::string path);
			// premission Edit/*insider path permissions*/
			static  bool isExist(const std::string path);
			bool changeDirectory(const std::string newPath);
			~Directory();
			/////////////////////////////////////////////////////////
			//use handle and clean up and small functions 
			

				bool openDirectory(std::string path);
				void closeDirctory();
		};
	};
};
#endif