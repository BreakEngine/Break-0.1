//
// Created by Moustapha on 07/10/2015.
//

#ifndef BREAK_0_1_RECT_HPP
#define BREAK_0_1_RECT_HPP

#include <Globals.hpp>

namespace Break{
    namespace Infrastructure{
        struct BREAK_API Rect{
            real64 x,y,width,height;

            Rect();
            Rect(real64 _x,real64 _y, real64 _width, real64 _height);
        };
    }
}
#endif //BREAK_0_1_RECT_HPP
