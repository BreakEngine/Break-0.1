#include "File.hpp"
#include "Services.hpp"
#include <Utils.hpp>
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

File::File(const std::string& path, AccessPermission permission)
{
	open(path, permission);
}

File::~File()
{
	if(m_open)
		close();

	m_size = 0;
	m_readCursor = 0;
	m_open = false;
	m_path = "";
	m_name = "";
	m_handle = nullptr;
}

void File::open(const std::string& path, AccessPermission permission)
{
	//OS Open and fill data
	u64 win_size = 0;
	m_handle = Services::getPlatform()->openFile(path,permission,win_size);
	m_accessPermission = permission;
	if(m_handle)
		m_open = true;
	m_path = path;
	std::vector<char> delimiter;
	delimiter.push_back('/');
	delimiter.push_back('\\');
	auto splitted = Utils::split(path,delimiter);
	m_name = splitted.back();
	m_size = win_size;
	m_readCursor = 0;
}

void File::create(const std::string& path, AccessPermission permission)
{
	//creates file
	m_handle = Services::getPlatform()->createFile(path,permission);
	m_accessPermission = permission;
	if(m_handle)
		m_open = true;
	m_path = path;
	std::vector<char> delimiter;
	delimiter.push_back('/');
	delimiter.push_back('\\');
	auto splitted = Utils::split(path,delimiter);
	m_name = splitted.back();
	m_size = 0;
	m_readCursor = 0;
}

void File::close()
{
	//OS close
	Services::getPlatform()->closeFile(m_handle);
	m_open = false;
}

byte* File::read(u32 amount, byte* buffer)
{
	if(!m_open)
		return nullptr;

	if(m_readCursor == m_size)
		return nullptr;

	byte* write_buffer;
	if(buffer == nullptr)
		write_buffer = new byte[amount];
	else
		write_buffer = buffer;

	//read amount of bytes
	bool res = Services::getPlatform()->readFile(m_handle,write_buffer,amount);
	if(res == false)
		return nullptr;
	else{
		m_readCursor += amount;
		return write_buffer;
	}
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

bool File::Exists(const std::string& path)
{
	return Services::getPlatform()->fileExists(path);
}
