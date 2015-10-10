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
#include <string>

namespace Break{
    namespace Infrastructure {
        class BREAK_API Win32: public OS{

            static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        public:

            Win32();

            ~Win32();

            WindowPtr createWindow(const u32 width, const u32 height, const std::string &title) override;

            real64 getTime() override;

        };
    }
}

#endif //windows if
#endif //BREAK_0_1_WIN32_HPP
