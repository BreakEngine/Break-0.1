//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_DXTEXTURE2DHANDLE_HPP
#define BREAK_0_1_DXTEXTURE2DHANDLE_HPP

#include "Globals.hpp"

#ifdef OS_WINDOWS
#include <d3d11.h>
#include "GPUHandle.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API DXTexture2DHandle: public GPUHandle{
        public:
            ID3D11Texture2D* texture;
            ID3D11ShaderResourceView* resourceView;

            DXTexture2DHandle(){
                texture = nullptr;
                resourceView = nullptr;
            }

            virtual ~DXTexture2DHandle(){
                texture = nullptr;
                resourceView = nullptr;
            }
        };
    }
}
#endif
#endif //BREAK_0_1_DXTEXTURE2DHANDLE_HPP
