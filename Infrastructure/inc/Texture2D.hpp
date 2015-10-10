//
// Created by Moustapha on 04/10/2015.
//

#ifndef BREAK_0_1_TEXTURE2D_HPP
#define BREAK_0_1_TEXTURE2D_HPP

#include "Globals.hpp"
#include "Texture.hpp"
#include "Image.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{
        class BREAK_API Texture2D: public Texture{
        public:
            RTTI(Texture2D);

            Texture2D(ImagePtr src, bool mipmaps=false);
            ~Texture2D();

            void update(ImagePtr src)override;

            ImagePtr readImage();

            void use(GPU_ISA,u32 unit=0) override;

        };
        typedef std::shared_ptr<Texture2D> Texture2DPtr;
    }
}
#endif //BREAK_0_1_TEXTURE2D_HPP
