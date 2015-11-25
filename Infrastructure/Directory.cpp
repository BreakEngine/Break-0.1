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
Directory::Directory(std::string name, std::string path ){
	m_name=name;
	createDirectory(name, path);
	
}

Directory::~Directory(){

	m_name="";
	m_path="";
	m_opened=false;

}

void* Directory::createDirectory(const std::string name ,const  std:: string path){
	m_opened=true;
	Services::getPlatform()->creatDirectoryFolder(name,path);
	// if created return True
	// invalid path || Already Exist return false 
}

bool Directory::isExist(const std::string path){
	return (Services::getPlatform()->Exists(path));
	// incalid path || does not Exist return false
	// exist rreturn true
}
bool Directory::changeDirectory(const std::string newPath){
	return (Services:: getPlatform()->changeCurrentDirectory(newPath));
}
bool Directory::close(){
	m_opened=false;
return changeDirectory(nullptr);
}

bool Directory::open(){
	m_opened=true;
	return changeDirectory(m_path);
}
std::string Directory::getName(){
return m_name;
}

std::string Directory::getPath(){
return m_path;
}

bool Directory::isOpened(){
return m_opened;
}

void Directory::getContent(std::vector<std::string> filesList,std::string path){
	Services::getPlatform()->ListDirectoryContents (filesList, path);
}
