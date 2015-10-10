//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_VERTEX2DPOSCOLORTEX_H
#define BREAK_0_1_VERTEX2DPOSCOLORTEX_H

#include "Globals.hpp"
#include <glm/common.hpp>
#include "MemoryLayout.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API Vertex2DPosColorTex{
        public:
            glm::vec2 position;
            glm::vec4 color;
            glm::vec2 texCoord;

            Vertex2DPosColorTex(){

            }

            Vertex2DPosColorTex(glm::vec2 pos,glm::vec4 _color, glm::vec2 _tex):position(pos), color(_color),
                                                                                texCoord(_tex){}

            static MemoryLayout getDescription(){
                MemoryLayout res;
                res.append(MemoryElement(MemoryElement::VEC2,"POSITION"));
                res.append(MemoryElement(MemoryElement::VEC4,"COLOR"));
                res.append(MemoryElement(MemoryElement::VEC2,"TEXCOORD"));
                return res;
            }
        };
    }
}
#endif //BREAK_0_1_VERTEX2DPOSCOLORTEX_H
