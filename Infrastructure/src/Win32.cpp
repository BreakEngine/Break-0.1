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
	m_DSoundDevice = nullptr;
	m_DSoundBuffer = nullptr;
	m_pullAudio = nullptr;
	m_soundDevice = nullptr;
}

Win32::~Win32(){
	m_DSoundDevice->Release();
	m_DSoundDevice = nullptr;
	m_DSoundBuffer->Release();
	m_DSoundBuffer = nullptr;
	m_soundDevice = nullptr;
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

void Win32::initSound(Window* win, AudioFormat format)
{
	if(!win)
		return;

	s32 bufferSize = format.AvgBytePerSec;

	if(FAILED(DirectSoundCreate(NULL, &m_DSoundDevice, NULL)))
		throw ServiceException("Failed to init Directsound");

	HWND window = (HWND) getNativeWindowHandle(win);

	if(FAILED(m_DSoundDevice->SetCooperativeLevel(window, DSSCL_PRIORITY)))
		throw ServiceException("Failed to set Cooperative Level for Directsound");

	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd,sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;
	dsbd.lpwfxFormat = NULL;

	LPDIRECTSOUNDBUFFER primary_buffer;
	if(FAILED(m_DSoundDevice->CreateSoundBuffer(&dsbd,&primary_buffer,NULL)))
		throw ServiceException("Failed to create Directsound's primary buffer");

	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat,sizeof(WAVEFORMATEX));
	waveFormat.nSamplesPerSec = format.SamplesPerSec;
	waveFormat.cbSize = format.Size;
	waveFormat.nAvgBytesPerSec = format.AvgBytePerSec;
	waveFormat.nBlockAlign = format.BlockAlign;
	waveFormat.nChannels = format.Channels;
	waveFormat.wBitsPerSample = format.BitsPerSample;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;

	if(FAILED(primary_buffer->SetFormat(&waveFormat)))
		throw ServiceException("Failed to set Directsound wave format");

	DSBUFFERDESC secDB;
	ZeroMemory(&secDB,sizeof(DSBUFFERDESC));
	secDB.dwSize = sizeof(DSBUFFERDESC);
	secDB.dwFlags = 0;
	secDB.dwBufferBytes = bufferSize;
	secDB.lpwfxFormat = &waveFormat;

	if(FAILED(m_DSoundDevice->CreateSoundBuffer(&secDB,&m_DSoundBuffer,0)))
		throw ServiceException("Failed to create Directsound's secondary buffer");

	if(FAILED(m_DSoundBuffer->Play(0,0,DSBPLAY_LOOPING)))
		throw ServiceException("Failed to play Directsound buffer");
}

void Win32::pullSound(AudioFormat format)
{
	if(!m_pullAudio || !m_soundDevice)
		return;

	DWORD PlayCursor, WriteCursor;

	if(FAILED(m_DSoundBuffer->GetCurrentPosition(&PlayCursor,&WriteCursor)))
		throw ServiceException("Cannot get Directsound playing position");

	DWORD BytesToLock = format.BlockAlign % format.AvgBytePerSec;
	DWORD BytesToWrite;
	if(BytesToLock > PlayCursor)
	{
		BytesToWrite = format.AvgBytePerSec - BytesToLock;
		BytesToWrite += PlayCursor;
	}else{
		BytesToWrite = PlayCursor - BytesToLock;
	}

	VOID* region1;
	DWORD region1Size;
	VOID* region2;
	DWORD region2Size;

	auto res = m_DSoundBuffer->Lock(BytesToLock, BytesToWrite, &region1, &region1Size, &region2, &region2Size,0);
	if(FAILED(res))
		throw ServiceException("Failed to lock Directsound buffer write pointer");

	if(region1Size>0)
	{
		m_pullAudio((byte*)region1,region1Size,m_soundDevice);
	}

	if(region2Size>0)
	{
		m_pullAudio((byte*)region2,region2Size,m_soundDevice);
	}

	m_DSoundBuffer->Unlock(region1,region1Size,region2,region2Size);
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

void Win32::setPullAudioCallback(GetAudioCallback function,SoundDevice* this_ptr)
{
	m_pullAudio = function;
	m_soundDevice = this_ptr;
}
#endif
