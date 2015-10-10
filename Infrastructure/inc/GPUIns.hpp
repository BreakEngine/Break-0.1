//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_GPUINS_HPP
#define BREAK_0_1_GPUINS_HPP

#include "Globals.hpp"
#include "Argument.hpp"
#include "GPU_ISA.hpp"
#include <queue>

namespace Break{
    namespace Infrastructure{

        class BREAK_API GPUIns: public Object{
        public:
            RTTI(GPUIns);

            GPU_ISA instruction;
            std::queue<Arg> args;

            GPUIns():Object("GPUIns", GPUIns::Type){

            }
        };
    }
}
#endif //BREAK_0_1_GPUINS_HPP
