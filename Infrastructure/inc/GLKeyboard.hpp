//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_GLKEYBOARD_HPP
#define BREAK_0_1_GLKEYBOARD_HPP

#include "Globals.hpp"
#include "Keyboard.hpp"
#include <GLFW/glfw3.h>
namespace Break{
    namespace Infrastructure{
        class GLDevice;
        class BREAK_API GLKeyboard: public Keyboard{
            friend class GLDevice;

            /**
			 * @fn	static void GLKeyboard::keyboardDownFunc(unsigned char k, int x, int y);
			 *
			 * @brief	Keyboard down function to indicate that a key is down at this moment
			 *
			 * @author	Moustapha Saad
			 * @date	23/01/2015
			 *
			 * @param	k	The unsigned char to process.
			 * @param	x	The x coordinate.
			 * @param	y	The y coordinate.
			 */
            static void keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods);

        protected: 
            /**
			 * @fn	void GLKeyboard::update();
			 *
			 * @brief	Updates this object and key pressing map.
			 *
			 * @author	Moustapha Saad
			 * @date	23/01/2015
			 */
            void update();
        };
    }
}
#endif //BREAK_0_1_GLKEYBOARD_HPP
