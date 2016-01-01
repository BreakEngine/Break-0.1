//
// Created by Moustapha on 02/10/2015.
//

#ifndef BREAK_0_1_IMAGE_HPP
#define BREAK_0_1_IMAGE_HPP

#include "Globals.hpp"
#include "Asset.hpp"
#include "Pixel.hpp"
#include "RAMBuffer.hpp"

namespace Break{
    namespace Infrastructure{

        /**
		 * \brief represents an image resource
		 * \author Moustapha Saad
		 */
        class BREAK_API Image:public Asset{

            ///pointer to image pixels
            Pixel* m_data;

            u32 m_width, m_height, m_depth, m_size;

            Image(const Image& val){};

            /**
			 * \brief width setter
			 * \param val new width
			 * \author Moustapha Saad
			 */
            void setWidth(u32 val);

            /**
             * \brief height setter
             * \param val new height
             * \author Moustapha Saad
             */
            void setHeight(u32 val);

            /**
             * \brief height setter
             * \param val new height
             * \author Moustapha Saad
             */
            void setDepth(u32 val);

        public:

            RTTI(Image);

            ///defualt constructor
            Image();

            /**
             * \brief init constructor
             * \param data pointer to image pixels
             * \param width width of the image
             * \param height height of the image (OPTIONAL, in case of 1D image)
             * \param depth depth of the image (OPTIONAL, in case of 2D image)
             * \author Moustapha Saad
             */
            Image(Pixel* pixels,unsigned int width,unsigned int height=1, unsigned int depth=1);

            /**
             * \brief empty image constructor
             * \param width width of the image
             * \param height height of the image (OPTIONAL, in case of 1D image)
             * \param depth depth of the image (OPTIONAL, in case of 2D image)
             * \author Moustapha Saad
             */
            Image(unsigned int width,unsigned int height=1, unsigned int depth=1);

            ///virtual default destructor
            virtual ~Image();

            /**
			 * \brief gets pixels of the image
			 * \author Moustapha Saad
			 */
            Pixel* getPixels();

            /**
             * \brief width getter
             * \return width of the image
             * \author Moustapha Saad
             */
            u32 getWidth();

            /**
             * \brief height getter
             * \return height of the image
             * \author Moustapha Saad
             */
            u32 getHeight();

            /**
             * \brief depth getter
             * \return depth of the image
             * \author Moustapha Saad
             */
            u32 getDepth();

            /**
             * \brief pixel count getter
             * \return pixel count in this image
             * \author Moustapha Saad
             */
            u32 getSize();

            /**
             * \brief single pixel getter
             * \param x x index of the image
             * \param y y index of the image (OPTIONAL, in case of 1D image)
             * \param z z index of the image (OPTIONAL, in case of 2D image)
             * \return pixel reference
             * \author Moustapha Saad
             */
            Pixel& getPixel(unsigned int x, unsigned int y=0, unsigned int z=0);

			/**
             * \brief single pixel setter
			 * \param pixel pixel value to be set
             * \param x x index of the image
             * \param y y index of the image (OPTIONAL, in case of 1D image)
             * \param z z index of the image (OPTIONAL, in case of 2D image)
             * \author Moustapha Saad
             */
			
			void setPixel(Pixel& pixel, u32 x, u32 y=0, u32 z= 0);

            /**
             * \brief returns a new image object with the same data
             * \author Moustapha Saad
             */
            std::shared_ptr<Image> clone();
        };
        typedef std::shared_ptr<Image> ImagePtr;
    }
}
#endif //BREAK_0_1_IMAGE_HPP
