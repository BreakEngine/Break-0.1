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
			virtual void initSound(Window* win, AudioFormat format)=0;

            /**
             * \brief inquires sound from user mixer or whatever he uses
             * \author Moustapha Saad
             */
            virtual void pullSound(AudioFormat format)=0;

			/**
			 * \brief sets getAudio callback function to set samples to be rendered
			 * \param function the audio callback function
			 * \author Moustapha Saad
			 */
			virtual void setPullAudioCallback(GetAudioCallback function, SoundDevice* this_ptr)=0;

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
			 * \return a file in binary format
			 * \author Moustapha Saad
			 */
			virtual bool readFile(const void* handle, void* buffer, u32 buffer_size)=0;

			/**
			 * \brief Closes a file
			 * \param handle a handle to file opened
			 * \author Moustapha Saad
			 */
			virtual void closeFile(const void* handle)=0;

			/**
			 * \brief returns a native handle of the Window class handle
			 * \param win pointer to the window that you need native handle to
			 * \param handle a generic pointer to native handle [output]
			 * \author Moustapha Saad
			 */
			virtual void* getNativeWindowHandle(Window* win)=0;
        };
        typedef std::shared_ptr<OS> OSPtr;
    }
}
#endif //BREAK_0_1_OS_HPP
