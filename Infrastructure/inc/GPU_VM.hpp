//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_GPU_VM_HPP
#define BREAK_0_1_GPU_VM_HPP

#include "Globals.hpp"
#include "GPUIns.hpp"
#include "GPUHandle.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{
        class BREAK_API GPU_VM: public Object{
            Arg pop(std::queue<Arg>& args);
        public:

            RTTI(GPU_VM);

            GPU_VM();
            ~GPU_VM();
            GPUHandlePtr execute(GPUIns& ins);
        };
        typedef std::shared_ptr<GPU_VM> GPU_VMPtr;
    }
}
#endif //BREAK_0_1_GPU_VM_HPP
