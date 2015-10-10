//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_SERVICEEXCEPTION_HPP
#define BREAK_0_1_SERVICEEXCEPTION_HPP
#include "Globals.hpp"
#include <exception>

namespace Break{
    namespace Infrastructure{
        /**
		 * \brief Service exception that represents an exception on Service level
		 * \author Moustapha Saad
		 */
        class BREAK_API ServiceException : public std::exception{
        private:
            //exception side
            std::string _msg;
        public:
            /**
             * \brief a initializer constructor
             * \param message c-style string of the message
             * \author Moustapha Saad
             */
            explicit ServiceException(const char* message):_msg(message){

            }

            /**
             * \brief a initializer constructor
             * \param message cpp-style string of the message
             * \author Moustapha Saad
             */
            explicit ServiceException(const std::string& message):_msg(message){

            }

            /**
             * \brief virtual default destructor
             * \author Moustapha Saad
             */
            virtual ~ServiceException()throw(){

            }

            /**
             * \brief returns a message of this exception
             * \return c-style string of the exception message
             * \author Moustapha Saad
             */
            virtual const char* what()const throw(){
                return _msg.c_str();
            }
        };
    }
}
#endif //BREAK_0_1_SERVICEEXCEPTION_HPP
