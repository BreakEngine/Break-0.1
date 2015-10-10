//
// Created by Moustapha on 30/09/2015.
//
#include "Win32.hpp"

#ifdef OS_WINDOWS
#include <windows.h>
#include <exception>
#include <windowsx.h>
#include "ServiceException.hpp"
#include "DXMouse.hpp"
#include "DXKeyboard.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Win32::Win32(){

}

Win32::~Win32(){

}

LRESULT Win32::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch(message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        break;
        case WM_KEYDOWN:
            DXKeyboard::keyboardDown(wParam);
            break;
        case WM_KEYUP:
            DXKeyboard::keyboardUp(wParam);
            break;
        case WM_LBUTTONDOWN:
            DXMouse::mouseButton(0,0);
            break;
        case WM_LBUTTONUP:
            DXMouse::mouseButton(0,1);
            break;
        case WM_RBUTTONDOWN:
            DXMouse::mouseButton(2,0);
            break;
        case WM_RBUTTONUP:
            DXMouse::mouseButton(2,1);
            break;
        case WM_MBUTTONDOWN:
            DXMouse::mouseButton(1,0);
            break;
        case WM_MBUTTONUP:
            DXMouse::mouseButton(1,1);
            break;
        case WM_MOUSEMOVE:
            DXMouse::mouseMove(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
            break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}

WindowPtr Win32::createWindow(const u32 width, const u32 height, const std::string &title)
{
    auto ret = std::make_shared<Window>();
    HWND hWnd;
    WNDCLASSEXA wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &Win32::WindowProc;
    wc.hInstance = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass1";

    RegisterClassExA(&wc);

    RECT wr = { 0, 0, (LONG)width, (LONG)height };    // set the size, but not the position
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

    hWnd = CreateWindowExA(NULL,
                          "WindowClass1",
                          title.c_str(),
                          WS_OVERLAPPEDWINDOW,
                          100,
                          100,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          NULL,
                          NULL,
                          NULL,
                          NULL);
    ShowWindow(hWnd, SW_SHOW);

    ret->setWidth(width);
    ret->setHeight(height);
    ret->setTitle(title);
    ret->setHandle(hWnd);

    return ret;
}

real64 Win32::getTime(){
    static bool init = false;
    static real64 freq;
    LARGE_INTEGER li;
    if(!init){
        if(!QueryPerformanceFrequency((&li)))
            throw ServiceException("Can't initialize the frequency timer");
        freq = double(li.QuadPart);
        init = true;
    }

    if(!QueryPerformanceCounter((&li))){
        throw ServiceException("QueryPerformanceCounter can't get the time");
    }
    return double(li.QuadPart)/freq;
}

#endif
