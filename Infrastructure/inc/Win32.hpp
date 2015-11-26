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
#include <portaudio.h>

namespace Break{
    namespace Infrastructure {
        class BREAK_API Win32: public OS{

            static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

			static int patestCallback(const void* inputBuffer, void* outputBuffer,
				unsigned long framesPerBuffer,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags,
				void* userData);

  			GetAudioCallback m_pullAudio;

			PaStream* stream;
            public:

            Win32();

            ~Win32();

            WindowPtr createWindow(const u32 width, const u32 height, const std::string &title) override;

            real64 getTime() override;

  			void initSound(AudioFormat format) override;

  			bool fileExists(const std::string& fileName) override;

  			void* openFile(const std::string& fileName, const AccessPermission permission, u64& out_size) override;

  			void* createFile(const std::string& fileName, const AccessPermission permission) override;

  			std::string getAbsolutePath(const std::string& fileName) override;

  			bool readFile(const void* handle, void* buffer, u32 buffer_size) override;

  			void closeFile(const void* handle) override;

  			void* getNativeWindowHandle(Window* win) override;

  			void setPullAudioCallback(GetAudioCallback function) override;
        };
    }
}

#endif //windows if
#endif //BREAK_0_1_WIN32_HPP
