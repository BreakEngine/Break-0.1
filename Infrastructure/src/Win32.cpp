//
// Created by Moustapha on 30/09/2015.
//
#include "Win32.hpp"
#include "Services.hpp"

#ifdef OS_WINDOWS
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <exception>
#include <windowsx.h>
#include "ServiceException.hpp"
#include "DXMouse.hpp"
#include "DXKeyboard.hpp"
#include "SoundDevice.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Win32::Win32(){
	stream = nullptr;
	m_pullAudio = nullptr;
}

Win32::~Win32(){
	auto err = Pa_StopStream(stream);
	if(err != paNoError)
		throw ServiceException("OS can't stop stream");
	err = Pa_CloseStream(stream);
	if(err != paNoError)
		throw ServiceException("OS can't close stream");
	Pa_Terminate();
	stream = nullptr;
	m_pullAudio = nullptr;
}

LRESULT Win32::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch(message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        //break;
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

struct paTestData
{
	float sine[64];
	int left_phase;
	int right_phase;
};
int Win32::patestCallback(const void* inputBuffer, void* outputBuffer,
				   unsigned long framesPerBuffer,
				   const PaStreamCallbackTimeInfo* timeInfo,
				   PaStreamCallbackFlags statusFlags,
				   void* userData)
{
	GetAudioCallback m_pullAudio = static_cast<GetAudioCallback>(userData);

	if(m_pullAudio)
		m_pullAudio(static_cast<byte*>(outputBuffer),framesPerBuffer,userData);

	return paContinue;
}
void Win32::initSound(AudioFormat format)
{
	PaStreamParameters output_parameters;
	PaError err;

	paTestData* data = new paTestData();

	for(int i=0;i<64;i++)
	{
		data->sine[i] = (real32) sinf((real32)i/(real32)64 * 3.145 * 2.0);
	}
	data->left_phase = data->right_phase = 0;
	err = Pa_Initialize();
	if(err != paNoError)
		throw ServiceException("Failed to init sound");

	output_parameters.device = Pa_GetDefaultOutputDevice();
	if(output_parameters.device == paNoDevice)
		throw ServiceException("OS can't find audio device");

	output_parameters.channelCount = format.Channels;
	output_parameters.sampleFormat = paInt16;
	output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
	output_parameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(
		&stream,
		NULL,
		&output_parameters,
		format.SamplesPerSec,
		512,
		paClipOff,
		patestCallback,
		m_pullAudio
		);

	if(err != paNoError)
		throw ServiceException("OS can't open audio output stream");

	err = Pa_StartStream(stream);

	if(err != paNoError)
		throw ServiceException("OS can't start audio stream");

}

bool Win32::fileExists(const std::string& fileName)
{
	OFSTRUCT file_info;
	HFILE file = OpenFile(fileName.c_str(),&file_info,OF_EXIST);
	if(file!=-1)
		return true;
	else
		return false;
}

void* Win32::openFile(const std::string& fileName, const AccessPermission permission, u64& out_size)
{
	DWORD win_permission = 0;
	if(permission == AccessPermission::READ)
	{
		win_permission = GENERIC_READ;
	}else if(permission == AccessPermission::WRITE)
	{
		win_permission = GENERIC_WRITE;
	}else if(permission == AccessPermission::READ_WRITE)
	{
		win_permission = GENERIC_READ | GENERIC_WRITE;
	}

	HANDLE hFile = CreateFile(fileName.c_str(),win_permission,
		FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		throw ServiceException("Cannot open file named: "+fileName);

	LARGE_INTEGER fileSize;
	if(!GetFileSizeEx(hFile,&fileSize)){
		throw ServiceException("Cannot query file size which named: "+fileName);
	}
	out_size = fileSize.QuadPart;

	return hFile;
}

void* Win32::createFile(const std::string& fileName, const AccessPermission permission)
{
	DWORD win_permission = 0;
	if(permission == AccessPermission::READ)
	{
		win_permission = GENERIC_READ;
	}else if(permission == AccessPermission::WRITE)
	{
		win_permission = GENERIC_WRITE;
	}else if(permission == AccessPermission::READ_WRITE)
	{
		win_permission = GENERIC_READ | GENERIC_WRITE;
	}

	HANDLE hFile = CreateFile(fileName.c_str(),win_permission,
		FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(hFile == INVALID_HANDLE_VALUE)
		throw ServiceException("Cannot open file named: "+fileName);

	return hFile;
}

std::string Win32::getAbsolutePath(const std::string& fileName)
{
	TCHAR abs[4096];
	TCHAR* pfileName;
	u32 res = GetFullPathName(fileName.c_str(),4096,abs,&pfileName);
	if(res > 0)
	{
		return string(abs);
	}else
	{
		throw ServiceException("Cannot retrieve full file name of a file named: "+fileName);
	}
}

bool Win32::readFile(const void* handle, void* buffer,u32 buffer_size)
{
	DWORD actual_read = 0;
	auto res = ReadFile(const_cast<void*>(handle), buffer, buffer_size, &actual_read, NULL);
	if(actual_read == 0)
		return false;
	if(FAILED(res)){
		throw ServiceException("unable to read from file");
	}
	return true;
}

void Win32::closeFile(const void* handle)
{
	if(!CloseHandle(const_cast<void*>(handle)))
		throw ServiceException("Cannot Close a file");
}

void* Win32::getNativeWindowHandle(Window* win)
{
	if(!win){
		return nullptr;
	}

	if(Services::getEngine()->getAPI() == API::OpenGL3_3){
		return glfwGetWin32Window(win->getHandle<GLFWwindow*>());
	}else if(Services::getEngine()->getAPI() == API::DirectX11){
		return win->getHandle<HWND>();
	}else{
		return nullptr;
	}
}

void Win32::setPullAudioCallback(GetAudioCallback function)
{
	m_pullAudio = function;
	//m_soundDevice = this_ptr;
}
#endif
