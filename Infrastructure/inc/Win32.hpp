//
// Created by Moustapha on 30/09/2015.
//

#ifndef BREAK_0_1_WIN32_HPP
#define BREAK_0_1_WIN32_HPP

#include "Globals.hpp"

#ifdef OS_WINDOWS

#include "OS.hpp"
#include "Window.hpp"
#include <windows.h>
#include <dsound.h>
#include <string>

namespace Break{
    namespace Infrastructure {
        class BREAK_API Win32: public OS{

            static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

      			GetAudioCallback m_pullAudio;
      			LPDIRECTSOUND m_DSoundDevice;
      			LPDIRECTSOUNDBUFFER m_DSoundBuffer;
      			SoundDevice* m_soundDevice;
            public:

            Win32();

            ~Win32();

            WindowPtr createWindow(const u32 width, const u32 height, const std::string &title) override;

            real64 getTime() override;

      			void initSound(Window* win, AudioFormat format) override;

      			void pullSound(AudioFormat format) override;

      			bool fileExists(const std::string& fileName) override;

      			void* openFile(const std::string& fileName, const AccessPermission permission, u64& out_size) override;

      			void* createFile(const std::string& fileName, const AccessPermission permission) override;

      			std::string getAbsolutePath(const std::string& fileName) override;

      			bool readFile(const void* handle, void* buffer, u32 buffer_size) override;

      			void closeFile(const void* handle) override;

      			void moveFile(std::string currentLocation,std::string newLocation) override;
				
				void renameFile(std::string fileName,std::string newName) override;

				void* getNativeWindowHandle(Window* win) override;
				
				void  makeCopy(std::string fileName,std::string copyName,bool overWrite ) override;
      			
				void setPullAudioCallback(GetAudioCallback function, SoundDevice* this_ptr) override;

				 void* creatDirectoryFolder(std::string name, std::string path) override ;
				 bool Exists(std::string path) override ;
				 bool changeCurrentDirectory(std::string newPath) override ;
				 void ListDirectoryContents(std::vector<string> &out,std::string path) override;
		};
    }
}

#endif //windows if
#endif //BREAK_0_1_WIN32_HPP
