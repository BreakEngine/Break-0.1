//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_INPUTDEVICE_HPP
#define BREAK_0_1_INPUTDEVICE_HPP

#include "Globals.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{

        class Engine;
        ///represents an input device
        class BREAK_API InputDevice{
            friend class Engine; 
        public:
            InputDevice(){}

            virtual ~InputDevice(){}

        protected:

            /*!
			 * \function virtual void update()
			 *
			 * \brief used to invoke update function of the device
			 *
			 * \author Moustapha Saad
			 */
            virtual void update(){}
        };
        typedef std::shared_ptr<InputDevice> InputDevicePtr;
    }
}
#endif //BREAK_0_1_INPUTDEVICE_HPP
