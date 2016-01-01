#include <FontFace.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <ResourceLoader.hpp>
#include <FTRenderer.hpp>

using namespace Break;
using namespace Break::Graphics;
using namespace std;

namespace Break
{
	namespace Infrastructure
	{
		template<>
		std::shared_ptr<FontFace> ResourceLoader<FontFace>::load(std::string file)
		{
			FontFacePtr res = make_shared<FontFace>();
			FT_Face face;
			FT_Error error = FT_New_Face(FTEngine::getInstance()->library, file.c_str(),0,&face);
			
			if(error == FT_Err_Unknown_File_Format)
			{
				throw ServiceException("File not supported");
			}else if(error)
			{
				throw ServiceException("Can't load font file");
			}

			error = FT_Set_Char_Size(face,0,16*64,96,96);
			//error = FT_Set_Pixel_Sizes(face,5,5);
			//error = FT_Set_Pixel_Sizes(face,0,5);

			if(error)
				throw ServiceException("Can't initialize font face");

			res->setFont(face);
			
			return res;
		}
	}
}

FontFace::FontFace():Object("FontFace",FontFace::Type)
{
	m_font = nullptr;
	m_DPI = 96;
	m_size = 16;
}

FontFace::~FontFace()
{
	FT_Done_Face(m_font);
	m_font = nullptr;
}

void FontFace::setFont(FT_FaceRec_* val)
{
	m_font = val;
}

FT_FaceRec_* FontFace::getFont()
{
	return m_font;
}

std::string FontFace::getName()
{
	return m_font->family_name;
}

void FontFace::setSize(u32 val)
{
	if(m_size != val)
	{
		FT_Set_Char_Size(m_font,0,val*64,m_DPI,m_DPI);
	}
	m_size = val;
}

u32 FontFace::getSize()
{
	return m_size;
}

void FontFace::setDPI(u32 val)
{
	if(m_DPI != val)
	{
		FT_Set_Char_Size(m_font,0,m_size*64,val,val);
	}
	m_DPI = val;
}

u32 FontFace::getDPI()
{
	return m_DPI;
}