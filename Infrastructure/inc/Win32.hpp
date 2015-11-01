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

			std::string getAbsolutePath(const std::string& fileName) override;

			void readFile(const void* handle) override;

			void closeFile(const void* handle) override;

			void* getNativeWindowHandle(Window* win) override;

			void setPullAudioCallback(GetAudioCallback function, SoundDevice* this_ptr) override;
        };
    }
}

#endif //windows if
#endif //BREAK_0_1_WIN32_HPP
