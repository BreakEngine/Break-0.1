//
// Created by Moustapha on 29/09/2015.
//

#ifndef BREAK_0_1_OS_HPP
#define BREAK_0_1_OS_HPP

#include "Globals.hpp"
#include "Window.hpp"
#include "SoundDevice.hpp"
#include "File.hpp"
#include <memory>
namespace Break{
    namespace Infrastructure {

	    /**
         * represents an OS of specific platform and you can call OS services
         */
        class BREAK_API OS{
        public:

            OS(){}

            virtual ~OS(){}

            /**
             * \brief creates a window
             * \param width width of the window
             * \param height height of the window
             * \param title title of the window
             * \return returns a WindowPtr
             * \author Moustapha Saad
             */
            virtual WindowPtr createWindow(const u32 width, const u32 height, const std::string &title)=0;

            /**
             * \brief gets time from the os
             * \return real64 represents time in nanoseconds
             */
            virtual real64 getTime()=0;

			/**
			 * \brief initializes platform sound
			 * \param win window that sound will be rendered for
			 * \author Moustapha Saad
			 */
			virtual void initSound(AudioFormat format)=0;

			/**
			 * \brief sets getAudio callback function to set samples to be rendered
			 * \param function the audio callback function
			 * \author Moustapha Saad
			 */
			virtual void setPullAudioCallback(GetAudioCallback function)=0;

			/**
             * \brief check if file exists or not
             * \param fileName path of the file
			 * \return true if file exists, false otherwise
             * \author Moustapha Saad
             */
			virtual bool fileExists(const std::string& fileName)=0;

			/**
             * \brief Opens a file for an operation
             * \param fileName path of the file
			 * \param out_size out argument will contain the size of the opened file [OPTIONAL]
			 * \return a handle to that file
             * \author Moustapha Saad
             */
			virtual void* openFile(const std::string& fileName,const AccessPermission permission, u64& out_size)=0;

			/**
             * \brief Creates a file for an operation
             * \param fileName path of the file
			 * \param permission access permission of this file
			 * \return a handle to that file
             * \author Moustapha Saad
             */
			virtual void* createFile(const std::string& fileName, const AccessPermission permission) = 0;

			/**
			 * \brief returns the absolute file path of the relative path
			 * \param fileName path of the file
			 * \return absolute file path
			 * \author Moustapha Saad
			 */
			virtual std::string getAbsolutePath(const std::string& fileName) = 0;

			/**
			 * \brief Reads a file
			 * \param handle a handle to file opened
			 * \return bool if succeeded returns true, false otherwise
			 * \author Moustapha Saad
			 */
			virtual bool readFile(void* handle, void* buffer, u32 buffer_size)=0;

            /**
			 * \brief Writes to a file
			 * \param handle a handle to file opened
             * \param buffer buffer to be written
             * \param buffer_size size to be written
			 * \return bool if succeeded returns true, false otherwise
			 * \author Moustapha Saad
			 */
			virtual bool writeFile(void* handle, void* buffer, u32 buffer_size)=0;

			/**
			 * \brief Closes a file
			 * \param handle a handle to file opened
			 * \author Moustapha Saad
			 */
			virtual void closeFile(void* handle)=0;

			/**
			 * \brief returns a native handle of the Window class handle
			 * \param win pointer to the window that you need native handle to
			 * \param handle a generic pointer to native handle [output]
			 * \author Moustapha Saad
			 */
			virtual void* getNativeWindowHandle(Window* win)=0;

            /**
             * \brief copies a file from a path to another
             * \param path path of the file to be copied
             * \param newPath target path that the file will be copied to
             * \param overwriteFlag flag to indicate overwrite in case of newPath exists
             * \return true if succeeded false otherwise
             * \author Adhaam Ehab
             */
            virtual bool copyFile(const std::string& path, const std::string& newPath,
                bool overwriteFlag) =0;

            /**
             * \brief moves a file from a path to another
             * \param path path of the file to be moved
             * \param newPath target path that the file will be moved to
             * \return true if succeeded false otherwise
             * \author Adhaam Ehab
             */
            virtual bool moveFile(const std::string& path, const std::string& newPath) =0;

            /**
             * \brief rename a file
             * \param path old name of the file
             * \param newPath new name of the file
             * \return true if succeeded false otherwise
             * \author Adhaam Ehab
             */
            virtual bool renameFile(const std::string& path, const std::string& newPath) =0;

            /**
             * \brief deletes a file
             * \param path path of the file to be deleted
             * \return true if succeeded false otherwise
             * \author Moustapha Saad
             */
             virtual bool deleteFile(const std::string& path)=0;

             /**
              * \brief changes the Directory
              * \param newPath path of the directory to navigate to
              * \return true if succeeded false otherwise
              * \author Adhaam Ehab
              */
             virtual bool changeDirectory(const std::string& newPath)=0;

             /**
              * \brief checks if the Directory exits
              * \param path path of the directory to navigate to
              * \return true if succeeded false otherwise
              * \author Adhaam Ehab
              */
             virtual bool directoryExists(const std::string& path)=0;

             /**
              * \brief create a directory
              * \param path path of the directory to be created
              * \return true if succeeded false otherwise
              * \author Adhaam Ehab
              */
             virtual bool createDirectory(const std::string& path)=0;

             /**
              * \brief get the current directory
              * \return current directory the process is in
              * \author Adhaam Ehab
              */
             virtual std::string getCurrentDirectory()=0;

             /**
              * \brief list contents of the current directory
              * \return list of directory content
              * \author Adhaam Ehab
              */
             virtual std::vector<std::string> listDirContents()=0;

             /**
              * \brief deletes a given directory
              * \return true if succeeded false otherwise
              * \author Adhaam Ehab
              */
             virtual bool deleteDirectory(const std::string& path)=0;
        };
        typedef std::shared_ptr<OS> OSPtr;
    }
}
#endif //BREAK_0_1_OS_HPP
