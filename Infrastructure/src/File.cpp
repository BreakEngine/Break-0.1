#include "File.hpp"
#include "Services.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

File::File()
{
	m_size = 0;
	m_readCursor = 0;
	m_open = false;
	m_path = "";
	m_name = "";
	m_handle = nullptr;
}

File::File(const std::string& path)
{
	open(path);
}

File::~File()
{
	m_size = 0;
	m_readCursor = 0;
	m_open = false;
	m_path = "";
	m_name = "";
	m_handle = nullptr;
}

void File::open(const std::string& path)
{
	//OS Open and fill data
	m_handle = Services::getPlatform()->openFile(path);
}

void File::close()
{
	//OS close
	Services::getPlatform()->closeFile(m_handle);
}

byte* File::read(u32 amount)
{
	//read amount of bytes
	Services::getPlatform()->readFile(m_handle);
}

void* File::getNativeHandle() const
{
	return m_handle;
}

std::string File::getName() const
{
	return m_name;
}

std::string File::getPath() const
{
	return m_path;
}

AccessPermission File::getAccessPermission() const
{
	return m_accessPermission;
}

u32 File::getSize() const
{
	return m_size;
}

u32 File::getReadCursor() const
{
	return m_readCursor;
}