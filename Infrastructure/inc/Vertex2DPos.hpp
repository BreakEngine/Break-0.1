//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_VERTEX2DPOS_HPP
#define BREAK_0_1_VERTEX2DPOS_HPP

#include "Globals.hpp"
#include "MemoryLayout.hpp"
#include <glm/common.hpp>

namespace Break{
    namespace Infrastructure{
        class BREAK_API Vertex2DPos{
        public:
            glm::vec2 position;

            explicit Vertex2DPos(glm::vec2 pos):position(pos){}

            static MemoryLayout getDescription(){
                MemoryLayout res;
                res.append(MemoryElement(MemoryElement::VEC2,"POSITION0"));
                return res;
            }
        };
    }
}
#endif //BREAK_0_1_VERTEX2DPOS_HPP
