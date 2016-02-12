//
// Created by Moustapha on 29/09/2015.
//

#include "Engine.hpp"
#include "Services.hpp"
#include "Win32.hpp"
#include "DXDevice.hpp"
#include "GLDevice.hpp"
#include "TimeManager.hpp"
#include "DXMouse.hpp"
#include "GLMouse.hpp"
#include "GLKeyboard.hpp"
#include "DXKeyboard.hpp"
#include <memory>
#include <ServiceException.hpp>
#include <iostream>
#include "SoundDevice.hpp"
#include "MemoryManager.h"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Engine::Engine() {
    #ifdef OS_WINDOWS
    m_platform = make_shared<Win32>();
    #endif

    m_device = nullptr;
    m_shutdown = false;
    m_app = nullptr;
    m_worker = nullptr;
    m_assetManager = nullptr;
    m_GPU_VM = nullptr;
	m_soundDevice = nullptr;
	m_memorySize = MEMORY_DEFAULT_SIZE;

	Services::registerSoundDevice(m_soundDevice.get());
    Services::registerPlatform(m_platform.get());
}

Engine::~Engine() {
    if(m_worker && !m_joinWorker)
        m_worker->detach();

    m_app = nullptr;
    m_device = nullptr;
    m_platform = nullptr;
    m_assetManager = nullptr;
    m_GPU_VM = nullptr;
	m_soundDevice = nullptr;
}

void Engine::setup(ApplicationPtr app, API api, void *renderer, ISoundDevicePtr soundDevice) {
    m_api = api;
    m_app = app;

    if(soundDevice){
        m_soundDevice = soundDevice;
        Services::registerSoundDevice(m_soundDevice.get());
    }else{
        m_soundDevice = make_shared<SoundDevice>();
		m_soundDevice->setFormat(AudioFormat());
		Services::registerSoundDevice(m_soundDevice.get());
    }

    if(api == API::DirectX11){
        m_device = make_shared<DXDevice>();
        Services::registerGXDevice(m_device.get());
        m_inputDevices.push_back(make_shared<DXMouse>());
        m_inputDevices.push_back(make_shared<DXKeyboard>());
    }else if(api == API::OpenGL3_3){
        m_device = make_shared<GLDevice>();
        Services::registerGXDevice(m_device.get());
        m_inputDevices.push_back(make_shared<GLMouse>());
        m_inputDevices.push_back(make_shared<GLKeyboard>());
    }else{
        throw invalid_argument("Unidentified API value");
    }

    m_GPU_VM = make_shared<GPU_VM>();
    Services::registerGPU_VM(m_GPU_VM.get());
	m_memory = std::make_shared<Memory>(m_platform->virtualAllocate(m_memorySize));
	Services::registerMemory(m_memory.get());
}

void Engine::start() {
    m_worker = new thread(&Engine::init,this);

    if(m_joinWorker)
        m_worker->join();
    return;
}

void Engine::join(bool val) {
    m_joinWorker = val;
}

void Engine::shutdown() {
    m_shutdown = true;
}

Application* Engine::getApplication(){
    return m_app.get();
}

void Engine::setSoundDevice(ISoundDevicePtr device)
{
    m_soundDevice = nullptr;
    m_soundDevice = device;
    Services::registerSoundDevice(m_soundDevice.get());
}

void Engine::init() {
    if(!m_app->getWindow())
        throw ServiceException("Application window is null, you should init it in constructor");

    if(m_api == API::DirectX11){
        WindowPtr window = m_platform->createWindow(m_app->getWindow()->getWidth(),
                                                    m_app->getWindow()->getHeight(), m_app->getWindow()->getTitle());

        m_app->setWindow(window);
        Services::getGraphicsDevice()->init(m_app->getWindow());
        m_assetManager = make_shared<AssetManager>();
        Services::registerAssetManager(m_assetManager.get());
		m_platform->setPullAudioCallback(m_soundDevice->getAudioFeedCallback());
		m_platform->initSound(m_soundDevice->getFormat());

        m_app->init();
        m_app->loadResources();
        m_app->setupScene();
        Services::getGraphicsDevice()->start(m_app->getWindow());
    }else if(m_api == API::OpenGL3_3){
        Services::getGraphicsDevice()->init(m_app->getWindow());
		m_platform->setPullAudioCallback(m_soundDevice->getAudioFeedCallback());
		m_platform->initSound(m_soundDevice->getFormat());
        m_assetManager = make_shared<AssetManager>();
        Services::registerAssetManager(m_assetManager.get());
        m_app->init();
        m_app->loadResources();
        m_app->setupScene();
		auto handle = m_platform->getNativeWindowHandle(m_app->getWindow());
        Services::getGraphicsDevice()->start(m_app->getWindow());
    }
    return;
}

void Engine::gameloop() {
    if(m_shutdown)
        return;

    //calculating delta
    double current = m_platform->getTime();
    if(m_time._lastTime == 0)
        m_time._lastTime = current;
    double delta = current - m_time._lastTime;
    m_time._lastTime = current;
    m_time._totalElapsedTime += delta;
    //increase the counter by the delta time
    m_time._counter += delta;
    m_time._secondTick += delta;
    //bool to indicate whether to render or not
    bool needRender = false;
    //check if a second passed
    if (m_time._secondTick >= 1){
        //reset second tick
        m_time._secondTick = 0;
        //setting the FPS
        m_time.FPS = m_time._frameCounter;
        //reset frame counter
        m_time._frameCounter = 0;
    }
    if (m_time._type == Time::Type::LIMITED){
        //check if counter reached the frame limit in milliseconds
        if (m_time._counter > 1.0/m_time._frameLimit ){
            //set counter to 0 and
            m_time._counter = 0;
            needRender = true;
        }
    }
    else if (m_time._type == Time::Type::UNLIMITED){
        //doesn't matter we always render as much as possible
        needRender = true;
    }
    //calling the processing functions every loop
    input();
    m_time._previousStep.delta = delta;
    m_time._previousStep.elapsedTime = m_time._totalElapsedTime;

    update();
    //if need render then render the scene
    if (needRender)
    {
        //increase frame count by one
        m_time._frameCounter++;
        render();
    }
    else
    {
        //this_thread::sleep_for(chrono::duration<int,milli>(1));
    }
}

void Engine::input() {
    //do input stuff
    m_app->input();
    for(auto device : m_inputDevices)
        device->update();

    return;
}

void Engine::update() {
    //do update stuff
    m_app->update(m_time.getTimeStep());
    return;
}

void Engine::render() {
    //do render stuff
    m_device->clearBuffer();
    m_app->render();
    m_device->swapBuffer(m_app->getWindow());
    return;
}
