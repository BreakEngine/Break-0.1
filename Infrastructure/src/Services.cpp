//
// Created by Moustapha on 29/09/2015.
//

#include "Services.hpp"
#include <iostream>

using namespace Break;
using namespace Break::Infrastructure;

Engine* Services::m_engine = nullptr;
OS* Services::m_platform = nullptr;
IGXDevice* Services::m_graphicsDevice = nullptr;
GPU_VM* Services::m_GPU_VM = nullptr;
AssetManager* Services::m_assetManager = nullptr;
SoundDevice* Services::m_soundDevice = nullptr;


void Services::registerEngine(Infrastructure::Engine *val) {
    if(m_engine)
        delete m_engine;

    m_engine = val;
}

Infrastructure::Engine *Services::getEngine() {
    if(!m_engine)
        m_engine = new Engine();

    return m_engine;
}

void Services::registerPlatform(Infrastructure::OS *val) {

    m_platform = val;
}

Infrastructure::OS *Services::getPlatform() {
    return m_platform;
}

void Services::registerGXDevice(Infrastructure::IGXDevice *val) {

    m_graphicsDevice = val;
}

Infrastructure::IGXDevice *Services::getGraphicsDevice() {
    return m_graphicsDevice;
}

void Services::registerGPU_VM(Infrastructure::GPU_VM* val){
    m_GPU_VM = val;
}

GPU_VM* Services::getGPU_VM() {
    return m_GPU_VM;
}

void Services::registerAssetManager(Infrastructure::AssetManager* val){
    m_assetManager = val;
}

void Services::registerSoundDevice(Infrastructure::SoundDevice* val)
{
	m_soundDevice = val;
}

Infrastructure::AssetManager* Services::getAssetManager(){
    return m_assetManager;
}

Infrastructure::SoundDevice* Services::getSoundDevice()
{
	return m_soundDevice;
}