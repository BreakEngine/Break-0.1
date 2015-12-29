#ifndef FILE_HPP
#define FILE_HPP

#include "Globals.hpp"
#include <string>

namespace Break
{
	namespace Infrastructure
	{
		enum class AccessPermission:u8{READ, WRITE, READ_WRITE};

		class BREAK_API File
		{
			///file size
			u32 m_size;
			///file read cursor
			u32 m_readCursor;
			///open flag to check whether the file is opened or not
			bool m_open;
			///filepath
			std::string m_path;
			///filename
			std::string m_name;
			///native file handle
			void* m_handle;
			///access permission
			AccessPermission m_accessPermission;
		public:
			File();

			File(const std::string& path,
				AccessPermission permission = AccessPermission::READ);
			~File();


			void open(const std::string& path,
				AccessPermission permission = AccessPermission::READ);

			void create(const std::string& path,
				AccessPermission permission = AccessPermission::WRITE);

			void close();

			byte* read(u32 amount, byte* buffer = nullptr);

			void* getNativeHandle() const;

			std::string getName() const;

			std::string getPath() const;

			AccessPermission getAccessPermission() const;

			u32 getSize() const;

			u32 getReadCursor() const;

			bool write(void* data, u32 writtenSize);

			bool write(const std::string& str);

			static bool Exists(const std::string& path);

			static bool Rename(const std::string& filePath, std::string newFilePath);

			static bool Copy(const std::string& fileName, const std::string& copyName,
				bool overwriteFlag);

			static bool Move(const std::string& currentLocation,
				const std::string& newLocation);

			static bool Delete(const std::string& path);
		};
		typedef std::shared_ptr<File> FilePtr;
	}
}

#endif
