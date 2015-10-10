//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_GPU_ISA_HPP
#define BREAK_0_1_GPU_ISA_HPP

#include "Globals.hpp"

namespace Break{
    namespace Infrastructure{
        enum class GPU_ISA : u8
        {
            GEN = 0, VERTEX_BUFFER = 1, INDEX_BUFFER = 2,
            STATIC = 3, DYNAMIC = 4, MAP = 5,
            DEL = 6, BIND = 7, UNIFORM_BUFFER = 8,
            VERTEX_SHADER = 9, PIXEL_SHADER =10, PROGRAM = 11,
            TEXTURE2D = 12, GEOMETRY = 13, DRAW = 14,
            DRAW_INDEXED = 15, DRAW_MULTIPLE = 16, DRAW_MULTIPLE_INDEXED = 17,
            SAMPLER = 18, APPLY = 19
        };
    }
}
#endif //BREAK_0_1_GPU_ISA_HPP
