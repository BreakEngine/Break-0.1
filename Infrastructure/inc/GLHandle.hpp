//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_GLHANDLE_HPP
#define BREAK_0_1_GLHANDLE_HPP

#include "Globals.hpp"
#include "GPUHandle.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API GLHandle: public GPUHandle{
        public:
            u32 ID;
        };
    }
}
#endif //BREAK_0_1_GLHANDLE_HPP
