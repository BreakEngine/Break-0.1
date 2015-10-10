//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_DXBUFFERHANDLE_HPP
#define BREAK_0_1_DXBUFFERHANDLE_HPP

#include "Globals.hpp"

#ifdef OS_WINDOWS
#include <d3d11.h>
#include "GPUHandle.hpp"

namespace Break{
    namespace Infrastructure{
        class DXBufferHandle: public GPUHandle{
        public:
            ID3D11Buffer* DXBuffer;

            DXBufferHandle(){
                DXBuffer = nullptr;
            }

            virtual ~DXBufferHandle(){
                DXBuffer = nullptr;
            }
        };
    }
}
#endif
#endif //BREAK_0_1_DXBUFFERHANDLE_HPP
