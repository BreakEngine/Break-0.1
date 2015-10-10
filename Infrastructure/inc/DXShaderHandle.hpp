//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_DXSHADERHANDLE_HPP
#define BREAK_0_1_DXSHADERHANDLE_HPP

#include "Globals.hpp"

#ifdef OS_WINDOWS

#include <d3d11.h>
#include "GPUHandle.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API DXShaderHandle: public GPUHandle{
        public:

            ID3D11VertexShader* vertexShader;
            ID3D11PixelShader* pixelShader;
            ID3D11InputLayout* inputLayout;

            DXShaderHandle(){
                vertexShader = nullptr;
                pixelShader = nullptr;
                inputLayout = nullptr;
            }

            virtual ~DXShaderHandle(){
                vertexShader = nullptr;
                pixelShader = nullptr;
                inputLayout = nullptr;
            }
        };
    }
}

#endif
#endif //BREAK_0_1_DXSHADERHANDLE_HPP
