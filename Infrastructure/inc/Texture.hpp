//
// Created by Moustapha on 04/10/2015.
//

#ifndef BREAK_0_1_TEXTURE_HPP
#define BREAK_0_1_TEXTURE_HPP

#include "Globals.hpp"
#include "Asset.hpp"
#include "Image.hpp"
#include "GPUProgram.hpp"
#include "GPUResource.hpp"
#include "SamplerState.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{
        /**
		 * \brief represents a generic texture object
		 */
        class BREAK_API Texture:public GPUResource, public Asset{
        friend class GPUProgram;
        public:

            RTTI(Texture);

            /**
			 * \brief init constructor
			 * \param mipmaps true in case of using mipmaps
			 * \author Moustapha Saad
			 */
            Texture(bool mipmaps=false);
            virtual ~Texture();


            ///returns true if using mipmaps
            bool usingMipMaps()
            {
                return m_mipmaps;
            }

            /**
             * \brief binds a texture to shader sampler unit
             * \param type shader type this texture will be binded to
             * \param unit sampler unit to be binded to
             * \author Moustapha Saad
             */
            virtual void use(GPU_ISA,unsigned int unit=0)=0;

            ///updates the texture with an image and sync with GPU
            virtual void update(ImagePtr src)=0;

            ///return width of the texture
            unsigned int getWidth();
            ///return height of the texture
            unsigned int getHeight();
            ///return depth of the texture
            unsigned int getDepth();

        protected:
            ///textreu image
            ImagePtr m_image;
            ///mipmaps flag
            bool m_mipmaps;
            ///applied sampler to this texture to avoid reapplying same sampler again
            SamplerState* m_sampler;
        };
        typedef std::shared_ptr<Texture> TexturePtr;
    }
}

#endif //BREAK_0_1_TEXTURE_HPP
