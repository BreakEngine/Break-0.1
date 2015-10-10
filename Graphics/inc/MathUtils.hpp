//
// Created by Moustapha on 07/10/2015.
//

#ifndef BREAK_0_1_MATHUTILS_HPP
#define BREAK_0_1_MATHUTILS_HPP

#include <Globals.hpp>

namespace Break{
    namespace Graphics{
        class BREAK_API MathUtils{
        public:
            static const real32 PI;

            static inline real32 toDegrees(real32 rad)
            {
                return rad * (180/PI);
            }
            static inline real32 toRadians(real32 deg)
            {
                return deg * (PI/180);
            }
        };
    }
}
#endif //BREAK_0_1_MATHUTILS_HPP
