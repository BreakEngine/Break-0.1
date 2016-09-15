#include "Linux32.hpp"
#ifdef __linux__
#include <sys/mman.h>
#include <sys/time.h>
#include <bits/time.h>
#include <time.h>
#include "ServiceException.hpp"
#define NANOSECONDS_PER_SECOND 1000000000L

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Linux32::Linux32(){

}

Linux32::~Linux32(){

}

Block Linux32::virtualAllocate(size_t size, void* loc){
    Block x;
    x.ptr = static_cast<byte*>(mmap(loc, size, PROT_READ|PROT_WRITE, MAP_FIXED,MAP_EXECUTABLE,0));
    x.size = size;
    return x;
}

void Linux32::virtualFree(void* ptr, size_t size){
    munmap(ptr, size);
    return;
}

WindowPtr Linux32::createWindow(const u32 width, const u32 height, const std::string &title){
    return nullptr;
}

real64 Linux32::getTime(){
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)(((long) ts.tv_sec * NANOSECONDS_PER_SECOND) + ts.tv_nsec)/((double)(NANOSECONDS_PER_SECOND));
}

int Linux32::patestCallback(const void* inputBuffer, void* outputBuffer,
                   unsigned long framesPerBuffer,
                   const PaStreamCallbackTimeInfo* timeInfo,
                   PaStreamCallbackFlags statusFlags,
                   void* userData)
{
    GetAudioCallback m_pullAudio = reinterpret_cast<GetAudioCallback>(userData);

    if(m_pullAudio)
        m_pullAudio(static_cast<byte*>(outputBuffer),framesPerBuffer,userData);

    return paContinue;
}

void Linux32::initSound(AudioFormat format){
    PaStreamParameters output_parameters;
    PaError err;

    err = Pa_Initialize();
    if(err != paNoError)
        throw ServiceException("Failed to init sound");

    auto numDevices = Pa_GetDeviceCount();
    printf("numDevices = %d\n",numDevices);
    const   PaDeviceInfo *deviceInfo;
    int i;
    for(i=0;i<numDevices;i++){
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("name = %s\n",deviceInfo->name);
    }
    output_parameters.device = Pa_GetDefaultOutputDevice();
    //output_parameters.device = 4;
    printf("i = %d\n",i);
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
        (void*)m_pullAudio
        );

    if(err != paNoError)
        throw ServiceException("OS can't open audio output stream");

    err = Pa_StartStream(stream);

    if(err != paNoError)
        throw ServiceException("OS can't start audio stream");
    return;
}

void Linux32::setPullAudioCallback(GetAudioCallback function){
    m_pullAudio = function;
    return;
}

bool Linux32::fileExists(const std::string& fileName){
    if (access( fileName.c_str(), F_OK ) != -1)
        return true;
    return false;
}

void* Linux32::openFile(const std::string& fileName, const AccessPermission permission, u64& out_size){
    return nullptr;
}

void* Linux32::createFile(const std::string& fileName, const AccessPermission permission){
    std::ofstream* file = new std::ofstream(fileName);
    if(file)
        return file;
    std::cerr << "no file opened :'(" << std::endl;
}

std::string Linux32::getAbsolutePath(const std::string& fileName){
    if(fileName[0] == '/')
        if (this->fileExists(fileName))
            return fileName;
        else
            return "";
    char* dummy = nullptr;
    dummy = realpath( fileName.c_str(), dummy);
    //std::string tmp = std::string( realpath( fileName.c_str(), dummy) );
    if (dummy == nullptr)
        return "";
    return std::string(dummy);
}

bool Linux32::readFile(void* handle, void* buffer, u32 buffer_size){
    return false;
}

bool Linux32::writeFile(void* handle, void* buffer, u32 buffer_size){
    if(!handle)
        return false;
    std::ofstream *file = static_cast<std::ofstream*>(handle);
    char* data = static_cast<char*>(buffer);
    //file->write(data, buffer_size);
    *file << data;
    file->flush();
    return true;
}

void Linux32::closeFile(void* handle){
    if(!handle)
        return;
    static_cast<std::ofstream*>(handle)->close();
    delete handle;
}

void* Linux32::getNativeWindowHandle(Window* win){
    return nullptr;
}

bool Linux32::copyFile(const std::string& path, const std::string& newPath,
    bool overwriteFlag)
    {
        return false;
    }

bool Linux32::moveFile(const std::string& path, const std::string& newPath){
    return false;
}

bool Linux32::renameFile(const std::string& path, const std::string& newPath){
    return false;
}

bool Linux32::deleteFile(const std::string& path){
    return false;
}

bool Linux32::changeDirectory(const std::string& newPath){
    return false;
}

bool Linux32::directoryExists(const std::string& path)
{
    struct stat st;
    stat(path.c_str(), &st);
    return S_ISDIR(st.st_mode);
}

bool Linux32::createDirectory(const std::string& path){
    return false;
}

std::string Linux32::getCurrentDirectory(){
    return "";
}

std::vector<std::string> Linux32::listDirContents(){
    return vector<string>();
}

bool Linux32::deleteDirectory(const std::string& path){
    return false;
}
#endif
