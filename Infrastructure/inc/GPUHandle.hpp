//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_IGPUHANDLE_HPP
#define BREAK_0_1_IGPUHANDLE_HPP

#include "Globals.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{
        class BREAK_API GPUHandle{
        public:

            virtual ~GPUHandle(){

            }
        };
        typedef std::shared_ptr<GPUHandle> GPUHandlePtr;
    }
}
#endif //BREAK_0_1_IGPUHANDLE_HPP
