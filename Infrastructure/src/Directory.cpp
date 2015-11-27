#include "Directory.hpp"
#include "Services.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

bool Directory::changeDirectory(const std::string& newPath){
    return Services::getPlatform()->changeDirectory(newPath);
}

bool Directory::Exists(const std::string& path){
    return Services::getPlatform()->directoryExists(path);
}

bool Directory::createDirectory(const std::string& folder){
    return Services::getPlatform()->createDirectory(folder);
}

std::string Directory::getCurrentDirectory(){
    return Services::getPlatform()->getCurrentDirectory();
}

std::vector<std::string> Directory::listContents(){
    return Services::getPlatform()->listDirContents();
}

bool Directory::Delete(const std::string& path){
    return Services::getPlatform()->deleteDirectory(path);
}
