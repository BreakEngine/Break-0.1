#ifndef FILE_HPP
#define FILE_HPP

#include "Globals.hpp"

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
			File(const std::string& path, AccessPermission permission = AccessPermission::READ_WRITE);
			~File();

			void open(const std::string& path, AccessPermission permission = AccessPermission::READ_WRITE);

			void close();

			byte* read(u32 amount);

			void* getNativeHandle() const;

			std::string getName() const;

			std::string getPath() const;

			AccessPermission getAccessPermission() const;

			u32 getSize() const;

			u32 getReadCursor() const;

			/*
			To be Added
			void rename();
			void copy();
			void move();
			static Exists();
			*/
		};
		typedef std::shared_ptr<File> FilePtr;
	}
}

#endif