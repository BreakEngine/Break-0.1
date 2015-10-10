//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_PRIMITIVE_HPP
#define BREAK_0_1_PRIMITIVE_HPP

#include "Globals.hpp"
namespace Break{
    namespace Infrastructure{

        enum class Primitive : u8{
            POINTS = 0,
            LINES = 1,
            LINE_STRIP = 2,
            TRIANGLES = 4
        };

        enum class DrawingMode: u8{
            NORMAL = 0,
            INDEXED = 1,
            NORMAL_INSTANCED = 2,
            INDEX_INSTANCED = 3
        };
    }
}
#endif //BREAK_0_1_PRIMITIVE_HPP
