//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_GLSHADERHANDLE_HPP
#define BREAK_0_1_GLSHADERHANDLE_HPP

#include "Globals.hpp"
#include "GPUHandle.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API GLShaderHandle: public GPUHandle{
        public:
            u32 vertexShader;
            u32 pixelShader;
            u32 program;

            GLShaderHandle(){

            }

            virtual ~GLShaderHandle(){

            }
        };
    }
}
#endif //BREAK_0_1_GLSHADERHANDLE_HPP
