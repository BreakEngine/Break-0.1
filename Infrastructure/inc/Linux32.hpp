#ifndef BREAK_0_1_LINUX32_HPP
#define BREAK_0_1_LINUX32_HPP

#include "Globals.hpp"

#ifdef __linux__

#include "OS.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include "Window.hpp"
#include <portaudio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace Break{
namespace Infrastructure {
class BREAK_API Linux32 : public OS
{
    static int patestCallback(const void* inputBuffer, void* outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void* userData);

    GetAudioCallback m_pullAudio;

    PaStream* stream;
public:

    Linux32();
    ~Linux32();

    Block virtualAllocate(size_t size, void* loc = 0) override;

    void virtualFree(void* ptr, size_t size) override;

    WindowPtr createWindow(const u32 width, const u32 height, const std::string &title) override;

    real64 getTime() override;

    void initSound(AudioFormat format) override;

    void setPullAudioCallback(GetAudioCallback function) override;

    bool fileExists(const std::string& fileName) override;

    void* openFile(const std::string& fileName, const AccessPermission permission, u64& out_size) override;

    void* createFile(const std::string& fileName, const AccessPermission permission) override;

    std::string getAbsolutePath(const std::string& fileName) override;

    bool readFile(void* handle, void* buffer, u32 buffer_size) override;

    bool writeFile(void* handle, void* buffer, u32 buffer_size) override;

    void closeFile(void* handle) override;

    void* getNativeWindowHandle(Window* win) override;

    bool copyFile(const std::string& path, const std::string& newPath,
                  bool overwriteFlag) override;

    bool moveFile(const std::string& path, const std::string& newPath) override;

    bool renameFile(const std::string& path, const std::string& newPath) override;

    bool deleteFile(const std::string& path) override;

    bool changeDirectory(const std::string& newPath) override;

    bool directoryExists(const std::string& path) override;

    bool createDirectory(const std::string& path) override;

    std::string getCurrentDirectory() override;

    std::vector<std::string> listDirContents() override;

    bool deleteDirectory(const std::string& path) override;
};
}
}

#endif // __linux__
#endif // BREAK_0_1_LINUX32_HPP
