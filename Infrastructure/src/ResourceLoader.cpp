#include "ResourceLoader.hpp"
#include "Image.hpp"
#include <FreeImage.h>
#include <ServiceException.hpp>

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

namespace Break
{
	namespace Infrastructure
	{
		ImagePtr ResourceLoader::load(std::string file)
		{
			FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

			FIBITMAP* dib(0);

			BYTE* bits(0);

			unsigned int width(0), height(0);

			fif = FreeImage_GetFileType(file.c_str(),0);

			if(fif==FIF_UNKNOWN)
			{
				fif = FreeImage_GetFIFFromFilename(file.c_str());
			}

			if(fif == FIF_UNKNOWN)
			{
				throw ServiceException("Cannot load file named: "+file);
			}

			if(FreeImage_FIFSupportsReading(fif))
				dib = FreeImage_Load(fif,file.c_str());

			if(!dib)
				throw ServiceException("Cannot load file named: "+file);

			//FreeImage_ConvertToRGBAF()
			dib = FreeImage_ConvertTo32Bits(dib);
			FreeImage_FlipVertical(dib);
			bits = FreeImage_GetBits(dib);
			width = FreeImage_GetWidth(dib);
			height = FreeImage_GetHeight(dib);

			if((bits == 0)||(width == 0)||(height == 0))
				throw ServiceException("Cannot load file named: "+file);

			Pixel* p = reinterpret_cast<Pixel*>(bits);
			ImagePtr res = make_shared<Image>(p,width,height);
			FreeImage_Unload(dib);
			return res;
		}

	}
}