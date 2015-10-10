//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_GLMOUSE_HPP
#define BREAK_0_1_GLMOUSE_HPP

#include <GLFW/glfw3.h>
#include "Globals.hpp"
#include "Mouse.hpp"

namespace Break{
    namespace Infrastructure{
        class GLDevice;
        class BREAK_API GLMouse: public Mouse{
            friend class GLDevice;

        private:
            /**
			 * @fn	static void GLMouse::keyboardDownFunc(unsigned char k, int x, int y);
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
            static void mouseFunc(GLFWwindow* window, int key, int scancode, int action);

            static void mouseMotion(GLFWwindow* window,double x, double y);

        protected:
            /**
			 * @fn	void GLMouse::update();
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
#endif //BREAK_0_1_GLMOUSE_HPP
