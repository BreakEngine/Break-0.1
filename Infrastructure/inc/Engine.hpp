//
// Created by Moustapha on 29/09/2015.
//

#ifndef BREAK_0_1_ENGINE_HPP
#define BREAK_0_1_ENGINE_HPP

#include <memory>
#include <thread>
#include <vector>
#include "Globals.hpp"
#include "API.hpp"
#include "OS.hpp"
#include "IGXDevice.hpp"
#include "Application.hpp"
#include "InputDevice.hpp"
#include "TimeManager.hpp"
#include "AssetManager.hpp"
#include "GPU_VM.hpp"

namespace Break{
    namespace Infrastructure{

		class SoundDevice;
        class BREAK_API Engine{
            friend class IGXDevice;
        public:
            /**
			 * default constructor of the engine
			 */
            Engine();

            /**
             * default destructor of the engine
             */
            virtual ~Engine();

            /**
			 * \brief setups the engine with the needed objects to run
			 *
			 * \param app shared pointer to an instance of an application
			 * \param api choosen api to run
			 * \param renderer shared pointer to an instance of the used renderer (OPTIONAL)
			 *
			 * \author Moustapha Saad
			 */
            void setup(ApplicationPtr app, API api, void* renderer = NULL,
                ISoundDevicePtr device = nullptr);

            /*!
			 * \function start()
			 *
			 * \brief starts the engine
			 *
			 * \author Moustapha Saad
			 */
            void start();

            /*!
             * \function join(bool)
             *
             * \brief determines if the engine thread will be joined or not
             *
             * \author Moustapha Saad
             */
            void join(bool val=true);

            /*!
             * \function void shutdown()
             *
             * \brief terminates the whole engine
             *
             * \author Moustapha Saad
             */
            void shutdown();

            /**
             * \brief returns shutdown flag
             * \return bool shutdown flag
             * \author Moustapha Saad
             */
            bool getShutdown()const{ return m_shutdown; }

            /**
             * \brief gets application run
             * \return Application pointer
             * \author Moustapha Saad
             */
            Application* getApplication();

			void setSoundDevice(ISoundDevicePtr device);

            API getAPI(){
                return m_api;
            }

            TimeStep getTimeStep(){
                return m_time._previousStep;
            }

            Time::Type getType(){
                return m_time._type;
            };

            void setType(Time::Type val){
                m_time._type = val;
            }

            u32 getFPS(){
                return m_time.FPS;
            }

            void setFrameLimit(u32 val){
                if(val>0)
                    m_time._frameLimit = val;
            }

            u32 getFrameLimit(){
                return m_time._frameLimit;
            }
        private:

			void soundTest();

            void init();

            void gameloop();

            void input();

            void update();

            void render();

            ///chosen API that will application run
            API m_api;

            ///OS object for OS operation calls
            Infrastructure::OSPtr m_platform;

            ///graphics device object
            IGXDevicePtr m_device;

            ///application pointer that's running right now
            ApplicationPtr m_app;

            ///shutdown flag
            bool m_shutdown;

            ///engine working thread
            std::thread* m_worker;

            ///attachable flag of the worker thread
            bool m_joinWorker;

            ///List of Input devices
            std::vector<InputDevicePtr> m_inputDevices;

            ///time manager
            Time m_time;

            ///asset manager
            AssetManagerPtr m_assetManager;

            ///GPU VM
            GPU_VMPtr m_GPU_VM;

			///sound device pointer
			std::shared_ptr<ISoundDevice> m_soundDevice;

        };

    }
}
#endif //BREAK_0_1_ENGINE_HPP
