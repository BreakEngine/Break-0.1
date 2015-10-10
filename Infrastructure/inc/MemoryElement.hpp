//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_MEMORYELEMENT_HPP
#define BREAK_0_1_MEMORYELEMENT_HPP

#include "Globals.hpp"
#include <string>
#include <glm/glm.hpp>

namespace Break{
    namespace Infrastructure{

        /**
		 * \brief represents an input layout memory element
		 * \author Moustapha Saad
		 */
        class BREAK_API MemoryElement{
        public:

            ///type of the element
            enum Type{
                VEC2=0, VEC3, VEC4, FLOAT, INT, BOOL, MAT3, MAT4, NONE
            };

            ///type of the vertex element
            Type type;
            ///element start offset in bytes
            u32 offset;
            ///element size in bytes
            u32 size;
            ///element component count
            u32 components;
            /// semantic of the input layout
            std::string semantic;

            /**
             * \brief init constructor
             * \param _offset offset inside layout
             * \param _size size of this element
             * \param _components number of components that this element consists of
             * \param _type type of this element (OPTIONAL)
             * \param _semantic semantic of this element (OPTIONAL)
             * \author Moustapha Saad
             */
            MemoryElement(u32 _offset, u32 _size, u32 _components, Type _type = NONE,std::string _semantic=""){
                offset = _offset;
                size = _size;
                components = _components;
                type = _type;
                semantic = _semantic;
            }

            /**
             * \brief size deduced type constructor
             * \param _type type of this element
             * \param _semantic semantic of this element
             * \author Moustapha Saad
             */
            MemoryElement(Type _type, std::string _semantic){
                type = _type;
                offset = 0;
                semantic = _semantic;
                switch (type)
                {
                    case MemoryElement::VEC2:
                        size = sizeof(glm::vec2);
                        components = 2;
                        break;
                    case MemoryElement::VEC3:
                        size = sizeof(glm::vec3);
                        components = 3;
                        break;
                    case MemoryElement::VEC4:
                        size = sizeof(glm::vec4);
                        components = 4;
                        break;
                    case MemoryElement::FLOAT:
                        size = sizeof(float);
                        components = 1;
                        break;
                    case MemoryElement::INT:
                        size = sizeof(int);
                        components = 1;
                        break;
                    case MemoryElement::BOOL:
                        size = sizeof(bool);
                        components = 1;
                        break;
                    case MemoryElement::MAT3:
                        size = sizeof(float)*9;
                        components = 1;
                        break;
                    case MemoryElement::MAT4:
                        size = sizeof(float)*16;
                        components = 1;
                        break;
                    case MemoryElement::NONE:
                        size = 0;
                        components = 0;
                        break;
                    default:
                        break;
                }
            }

            /**
             * \brief comparision function
             * \param val const reference to element
             * \return true in case of equality false otherwise
             * \author Moustapha Saad
             */
            bool equals(const MemoryElement& val)
            {
                bool res = false;

                if(	offset == val.offset &&
                       size == val.size	 &&
                       components == val.components &&
                       type == val.type &&
                       semantic == val.semantic
                        )
                {
                    res = true;
                }

                return res;

            }
            ///copy constructor
            MemoryElement(const MemoryElement& val){
                offset = val.offset;
                size = val.size;
                components = val.components;
                type = val.type;
                semantic = val.semantic;
            }

            ///default destructor
            ~MemoryElement(){

            }
        };
    }
}
#endif //BREAK_0_1_MEMORYELEMENT_HPP
