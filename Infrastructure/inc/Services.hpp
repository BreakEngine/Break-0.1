//
// Created by Moustapha on 29/09/2015.
//

#ifndef BREAK_0_1_SERVICES_HPP
#define BREAK_0_1_SERVICES_HPP
#include "Globals.hpp"
#include "Engine.hpp"
#include "OS.hpp"
#include "IGXDevice.hpp"
#include "GPU_VM.hpp"
#include "AssetManager.hpp"
#include "SoundDevice.hpp"

namespace Break{
    class BREAK_API Services{
    friend class Infrastructure::Engine;
    private:
        static Infrastructure::Engine* m_engine;
        static Infrastructure::OS* m_platform;
        static Infrastructure::IGXDevice* m_graphicsDevice;
        static Infrastructure::GPU_VM* m_GPU_VM;
        static Infrastructure::AssetManager* m_assetManager;
		static Infrastructure::ISoundDevice* m_soundDevice;

        static void
        registerEngine(Infrastructure::Engine* val);

        static void registerPlatform(Infrastructure::OS* val);

        static void registerGXDevice(Infrastructure::IGXDevice* val);

        static void registerGPU_VM(Infrastructure::GPU_VM* val);

        static void registerAssetManager(Infrastructure::AssetManager* val);

		static void registerSoundDevice(Infrastructure::ISoundDevice* val);
    public:

        static Infrastructure::Engine* getEngine();

        static Infrastructure::OS* getPlatform();

        static Infrastructure::IGXDevice* getGraphicsDevice();

        static Infrastructure::GPU_VM* getGPU_VM();

        static Infrastructure::AssetManager* getAssetManager();

		static Infrastructure::ISoundDevice* getSoundDevice();
    };
}
#endif //BREAK_0_1_SERVICES_HPP
