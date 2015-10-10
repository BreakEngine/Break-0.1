//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_DXKEYBOARD_HPP
#define BREAK_0_1_DXKEYBOARD_HPP

#include "Globals.hpp"
#include "Keyboard.hpp"

namespace Break{
    namespace Infrastructure{
        class Win32;
        class BREAK_API DXKeyboard: public Keyboard{
            friend class Win32;

            /**
			 * @fn	static void DXKeyboard::keyboardDown(unsigned int k);
			 *
			 * @brief	Keyboard down.
			 *
			 * @author	Moustapha Saad
			 * @date	24/01/2015
			 *
			 * @param	k	The unsigned int of the down key.
			 */

            static void keyboardDown(unsigned int k);

            /**
             * @fn	static void DXKeyboard::keyboardUp(unsigned int k);
             *
             * @brief	Keyboard up.
             *
             * @author	Moustapha Saad
             * @date	24/01/2015
             *
             * @param	k	The unsigned int of the up key.
             */

            static void keyboardUp(unsigned int k);
        protected:

            /**
             * @fn	void DXKeyboard::update();
             *
             * @brief	Updates this object.
             *
             * @author	Moustapha Saad
             * @date	24/01/2015
             */

            void update();
        };
    }
}
#endif //BREAK_0_1_DXKEYBOARD_HPP
