//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_DXSAMPLERHANDLE_HPP
#define BREAK_0_1_DXSAMPLERHANDLE_HPP

#include "Globals.hpp"

#ifdef OS_WINDOWS

#include <d3d11.h>
#include "GPUHandle.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API DXSamplerHandle: public GPUHandle{
        public:
            ID3D11SamplerState *sampler;

            DXSamplerHandle(){
                sampler = nullptr;
            }

            virtual ~DXSamplerHandle(){
                sampler = nullptr;
            }
        };
    }
}

#endif
#endif //BREAK_0_1_DXSAMPLERHANDLE_HPP
