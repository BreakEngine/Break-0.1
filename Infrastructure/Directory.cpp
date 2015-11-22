#include "Directory.hpp"
#include <string>
#include "Services.hpp"

using namespace std;
using namespace Break;
using namespace Infrastructure;

Directory::Directory(){
	m_name="";
	m_accessPermission=Permission::READ;
	m_path="";
	m_opened=false;
}
Directory::Directory(std::string name, std::string path, Permission permission ){
	m_name=name;
	createDirectory(name, path, permission);
	
}

Directory::~Directory(){

	m_name="";
	m_accessPermission=Permission::READ;
	m_path="";
	m_opened=false;

}

bool Directory::createDirectory(const std::string name ,const  std:: string path,const Permission permission){
	m_opened=false;
	m_accessPermission=permission;
	return(Services::getPlatform()->creatDirectoryFolder(name,path));
}

bool Directory::createDirectory(const std::string path){
	m_opened=false;
	m_accessPermission=Permission::READ;
	return(Services::getPlatform()->creatDirectoryFolder(path));
} 
bool Directory::isExist(const std::string path){
	return (Services::getPlatform()->Exists(path));
}
bool Directory::changeDirectory(const std::string newPath){
	return (Services:: getPlatform()->changeCurrentDirectory(newPath));
}

