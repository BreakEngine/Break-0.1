//
// Created by Moustapha on 07/10/2015.
//

#include "Rect.hpp"

using namespace Break;
using namespace Break::Graphics;

Rect::Rect(){x=0;y=0;width=0;height=0;}

Rect::Rect(real64 _x,real64 _y, real64 _width, real64 _height)
{
    x=_x;
    y=_y;
    width=_width;
    height = _height;
}

