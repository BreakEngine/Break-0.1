//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_IMOUSE_HPP
#define BREAK_0_1_IMOUSE_HPP

#include "Globals.hpp"
#include "InputDevice.hpp"
#include <glm/glm.hpp>

namespace Break{
    namespace Infrastructure{
        class BREAK_API Mouse: public InputDevice{
        public:
            /**
			 * @fn	IMouse::IMouse();
			 *
			 * @brief	Default constructor.
			 *
			 * @author	Moustapha Saad
			 * @date	24/01/2015
			 */

            Mouse();

            /**
             * @fn	IMouse::~IMouse();
             *
             * @brief	Destructor.
             *
             * @author	Moustapha Saad
             * @date	24/01/2015
             */

            virtual ~Mouse();

            /**
             * @enum	Buttons
             *
             * @brief	Values that represent buttons.
             */

            enum Buttons{
                Left_Button=0, Right_Button=2, Middle_Button=1
            };

            /**
             * @enum	ButtonState
             *
             * @brief	Values that represent button states.
             */

            enum ButtonState{ State_Down, State_Up, State_None};

            /**
             * @fn	static ButtonState IMouse::getButton(Buttons b);
             *
             * @brief	Gets a button.
             *
             * @author	Moustapha Saad
             * @date	24/01/2015
             *
             * @param	b	The Buttons to process.
             *
             * @return	The button state.
             */

            static ButtonState getButton(Buttons b);

            /**
             * @fn	static glm::uvec2 IMouse::getPosition();
             *
             * @brief	Gets the position.
             *
             * @author	Moustapha Saad
             * @date	29/01/2015
             *
             * @return	The position.
             */

            static glm::uvec2 getPosition();

            /**
             * @fn	static void IMouse::setPosition(glm::uvec2 val);
             *
             * @brief	Sets the position.
             *
             * @author	Moustapha Saad
             * @date	29/01/2015
             *
             * @param	val	The value.
             */

            static void setPosition(glm::uvec2 val);

        protected:
            /**
			 * @fn	virtual void IMouse::update();
			 *
			 * @brief	Updates this object.
			 *
			 * @author	Moustapha Saad
			 * @date	24/01/2015
			 */
            virtual void update();

            /** @brief	The data map. */
            static std::map<Buttons,ButtonState> m_data;

            /** @brief	The position. */
            static glm::uvec2 m_position;
        };
    }
}

#endif //BREAK_0_1_IMOUSE_HPP
