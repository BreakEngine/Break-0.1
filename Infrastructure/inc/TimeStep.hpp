//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_TIMESTEP_HPP
#define BREAK_0_1_TIMESTEP_HPP

#include "Globals.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API TimeStep{
        public:
            real64  delta, elapsedTime;
            TimeStep(real64 d=0, real64 e=0)
                    :delta(d), elapsedTime(e)
            {

            }
        };
    }
}
#endif //BREAK_0_1_TIMESTEP_HPP
