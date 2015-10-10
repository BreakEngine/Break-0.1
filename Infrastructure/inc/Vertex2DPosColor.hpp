//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_VERTEX2DPOSCOLOR_HPP
#define BREAK_0_1_VERTEX2DPOSCOLOR_HPP

#include "Globals.hpp"
#include "MemoryLayout.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API Vertex2DPosColor{
        public:
            glm::vec2 position;
            glm::vec4 color;

            explicit Vertex2DPosColor(glm::vec2 pos, glm::vec4 c):position(pos), color(c){}

            static MemoryLayout getDescription(){
                MemoryLayout res;
                res.append(MemoryElement(MemoryElement::VEC2,"POSITION0"));
                res.append(MemoryElement(MemoryElement::VEC4,"COLOR0"));
                return res;
            }
        };
    }
}
#endif //BREAK_0_1_VERTEX2DPOSCOLOR_HPP
