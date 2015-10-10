//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_DXMOUSE_HPP
#define BREAK_0_1_DXMOUSE_HPP

#include "Globals.hpp"
#include "Mouse.hpp"

namespace Break{
    namespace Infrastructure{
        class Win32;
        class BREAK_API DXMouse: public Mouse{
            friend class Win32;
        private:
            /**
			 * @fn	static void DXMouse::mouseButton(int button, int state);
			 *
			 * @brief	Mouse button.
			 *
			 * @author	Moustapha Saad
			 * @date	29/01/2015
			 *
			 * @param	button	The button.
			 * @param	state 	The state.
			 */

            static void mouseButton(int button, int state);

            /**
             * @fn	static void DXMouse::mouseMove(int x, int y);
             *
             * @brief	Mouse move.
             *
             * @author	Moustapha Saad
             * @date	29/01/2015
             *
             * @param	x	The x coordinate.
             * @param	y	The y coordinate.
             */

            static void mouseMove(int x, int y);

        protected:
            /**
			 * @fn	void DXMouse::update();
			 *
			 * @brief	Updates this object.
			 *
			 * @author	Moustapha Saad
			 * @date	29/01/2015
			 */

            void update();
        };
    }
}
#endif //BREAK_0_1_DXMOUSE_HPP
