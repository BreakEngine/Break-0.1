#pragma once
#include <Globals.hpp>
#include <Object.hpp>
#include <memory>
#include <string>

struct FT_FaceRec_;

namespace Break
{
	namespace Graphics
	{
		class BREAK_API FontFace: public Object
		{
		protected:
			FT_FaceRec_* m_font;
			u32 m_size;
			u32 m_DPI;
		public:
			RTTI(FontFace);

			FontFace();
			virtual ~FontFace();

			void setFont(FT_FaceRec_* val);

			FT_FaceRec_* getFont();

			std::string getName();

			void setSize(u32 val);
			u32 getSize();

			void setDPI(u32 val);
			u32 getDPI();
		};
		typedef std::shared_ptr<FontFace> FontFacePtr;
	}
}