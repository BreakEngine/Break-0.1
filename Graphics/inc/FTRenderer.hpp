#pragma once

#include <Globals.hpp>
#include <Object.hpp>
#include <string>
#include <ServiceException.hpp>
#include <iostream>
#include <Image.hpp>

struct FT_LibraryRec_;
struct FT_FaceRec_;
struct FT_Bitmap_;

namespace Break
{
	namespace Graphics
	{
		class BREAK_API FTEngine
		{
		protected:
		public:
			FT_LibraryRec_* library;
			FTEngine();
			~FTEngine();

			static FTEngine* getInstance()
			{
				static std::shared_ptr<FTEngine> instance = std::make_shared<FTEngine>();
				return instance.get();
			}
		};

		class BREAK_API IFTRenderer
		{
		public:

			virtual Infrastructure::ImagePtr draw(FT_FaceRec_* font, std::string str) = 0;
		};

		class BREAK_API FTRenderer: public Object, public IFTRenderer
		{
		protected:
			void copyBitmap(FT_Bitmap_* src, FT_Bitmap_* dst);
			void deleteBitmap(FT_Bitmap_* bmp);
		public:
			RTTI(FTRenderer);

			FT_LibraryRec_* library;
			FT_FaceRec_* face;
			Infrastructure::ImagePtr m_image;

			FTRenderer();

			void render(std::string str);

			Infrastructure::ImagePtr draw(FT_FaceRec_* font, std::string str) override;
		};
	}
}