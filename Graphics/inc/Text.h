#pragma once
#include <Globals.hpp>
#include <Object.hpp>
#include "FontFace.h"
#include <Image.hpp>
#include <Texture2D.hpp>
#include <glm/common.hpp>
#include "SpriteBatch.hpp"

namespace Break
{
	namespace Graphics
	{
		class IFTRenderer;

		class BREAK_API Text: public Object
		{
		protected:
			Infrastructure::ImagePtr m_image;
			Infrastructure::Texture2DPtr m_texture;
			bool m_needDraw;
			std::string m_str;
			FontFacePtr m_font;
			IFTRenderer* m_renderer;
		public:

			Infrastructure::Color color;
			real64 angle;
			u32 size;
			u32 DPI;
			glm::vec2 origin;
			glm::vec2 position;

			RTTI(Text);

			Text(IFTRenderer* renderer,FontFacePtr font, std::string str);
			virtual ~Text();

			void setText(std::string str);
			std::string getText();

			void setFont(FontFacePtr font);
			FontFace* getFont();

			Infrastructure::Image* getImage();
			Infrastructure::Texture2D* getTexture();

			void draw(SpriteBatch* batcher);
		};
	}
}