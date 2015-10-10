//
// Created by Moustapha on 29/09/2015.
//

#ifndef BREAK_0_1_OS_HPP
#define BREAK_0_1_OS_HPP

#include "Globals.hpp"
#include "Window.hpp"
#include <memory>
namespace Break{
    namespace Infrastructure {

        /**
         * represents an OS of specific platform and you can call OS services
         */
        class BREAK_API OS{
        public:

            OS(){}

            virtual ~OS(){}

            /**
             * \brief creates a window
             * \param width width of the window
             * \param height height of the window
             * \param title title of the window
             * \return returns a WindowPtr
             * \author Moustapha Saad
             */
            virtual WindowPtr createWindow(const u32 width, const u32 height, const std::string &title)=0;

            /**
             * \brief gets time from the os
             * \return real64 represents time in nanoseconds
             */
            virtual real64 getTime()=0;
        };
        typedef std::shared_ptr<OS> OSPtr;
    }
}
#endif //BREAK_0_1_OS_HPP
