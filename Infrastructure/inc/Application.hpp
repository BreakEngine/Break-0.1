//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_APPLICATION_HPP
#define BREAK_0_1_APPLICATION_HPP

#include "Globals.hpp"
#include "Window.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{
	    class TimeStep;

	    class BREAK_API Application{
        protected:
            ///window of this application
            WindowPtr window;

            /**
             * \brief closes the Application
             * \author Moustapha Saad
             */
            void shutdown();
        public:

            /**
			 * \brief default constructor
			 * \author Moustapha Saad
			 */
            Application();

            /**
             * \brief default virtual destructor
             * \author Moustapha Saad
             */
            virtual ~Application();

            /**
             * \brief returns the window of this application
             * \return std::shared_ptr<Window>
             * \author Moustapha Saad
             */
            Window* getWindow();

            /**
             * \brief window pointer setter
             * \param window window smart pointer
             * \author Moustapha Saad
             */
            void setWindow(WindowPtr val);

            /**
             * \brief initialization function called only once at start
             * \author Moustapha Saad
             */
            virtual void init();

            /**
             * \brief resource loading function called only once at start
             * \author Moustapha Saad
             */
            virtual void loadResources();

            /**
             * \brief setup function called only once at start
             * \author Moustapha Saad
             */
            virtual void setupScene();

            /**
             *
             * \brief	Clean up resources used in this application.
             *
             * \author	Moustapha Saad
             */

            virtual void cleanUp();


            /**
             * \brief input function called once every frame
             * \author Moustapha Saad
             */
            virtual void input();

            /**
             * \brief update function called once every frame
             * \param time time step that has info about delta time and elapsed time
             * \author Moustapha Saad
             */
            virtual void update(TimeStep time);

            /**
             * \brief render function called once every frame
             * \author Moustapha Saad
             */
            virtual void render();
        };
        typedef std::shared_ptr<Application> ApplicationPtr;
    }
}
#endif //BREAK_0_1_APPLICATION_HPP
