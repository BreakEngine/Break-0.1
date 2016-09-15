#include "FTRenderer.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Services.hpp>
#include <ResourceLoader.hpp>
using namespace std;
using namespace Break;
using namespace Break::Graphics;

namespace Break
{
	namespace Infrastructure
	{
		template<>
		std::shared_ptr<FTRenderer> ResourceLoader<FTRenderer>::load(std::string file){
			
			return nullptr;
		}
	}
}

FTEngine::FTEngine()
{
	FT_Error error = FT_Init_FreeType(&library);
	if(error)
		std::cerr<<"CANNOT INIT LIB"<<std::endl;
}

FTEngine::~FTEngine()
{
	FT_Done_FreeType(library);
}

void FTRenderer::copyBitmap(FT_Bitmap* src, FT_Bitmap* dst)
{
	dst->width = src->width;
	dst->rows = src->rows;
	dst->pitch = src->pitch;
	dst->num_grays = src->num_grays;
	dst->pixel_mode = src->pixel_mode;
	dst->palette_mode = src->palette_mode;
	dst->buffer = new unsigned char[dst->width*dst->rows];
	dst->palette = nullptr;
	memcpy(dst->buffer,src->buffer,dst->width*dst->rows);
}

void FTRenderer::deleteBitmap(FT_Bitmap* bmp)
{
	if(bmp->buffer)
		delete[] bmp->buffer;
	bmp = nullptr;
}

FTRenderer::FTRenderer():Object("FTRenderer",FTRenderer::Type)
{
	auto width = Services::getEngine()->getApplication()->getWindow()->getWidth();
	auto height = Services::getEngine()->getApplication()->getWindow()->getHeight();
	m_image = make_shared<Infrastructure::Image>(width,height);
	FT_Error error = FT_Init_FreeType(&library);
	if(error)
        std::cerr<<"Cannot initialize freetype."<<std::endl;

	error = FT_New_Face(library,
		"res/fonts/Arial.ttf",
		0,
		&face);

	if(error == FT_Err_Unknown_File_Format)
	{
        std::cerr<<"Font file not supported."<<std::endl;
	}else if(error)
	{
        std::cerr<<"So"
                   "mething's wrong with freetype."<<std::endl;
	}

    error = FT_Set_Char_Size(face,0,16*64,300,300);
//	auto glyph_index = FT_Get_Char_Index(face,'a');
//	error = FT_Load_Glyph(face,glyph_index,FT_LOAD_DEFAULT);
//	FT_Render_Glyph(face->glyph,FT_RENDER_MODE_NORMAL);
}

void FTRenderer::render(std::string str)
{
	FT_GlyphSlot slot = face->glyph;
	int pen_x,pen_y,n;

	pen_x = 0;
	pen_y = 0;
	FT_Error error;
	for(n = 0; n<str.size();n++)
	{
		pen_y = 0;

		FT_UInt glyph_index;

		glyph_index = FT_Get_Char_Index(face, str[n]);

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if(error)
			continue;

		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if(error)
			continue;

		Infrastructure::Pixel p;
		//render into image
		
		FT_Int x_max = slot->bitmap.width;
		FT_Int y_max = slot->bitmap.rows;

		const int line = 50;
		pen_y += line-(slot->metrics.height>>6);
		for(auto i=pen_x, gi=0; i<m_image->getWidth() && gi < x_max; i++, gi++)
		{
			for(auto j=pen_y, gj =0; j<m_image->getHeight()&&gj<y_max; j++, gj++)
			{
				//m_image->setPixel()
				p.A = p.R = p.G = p.B =slot->bitmap.buffer[gj*x_max+gi];
				
				m_image->setPixel(p,i,j);
			}
		}

		pen_x += slot->advance.x >> 6;
		pen_x -= slot->metrics.horiBearingX>>6;
		pen_y += slot->advance.y >> 6;
	}
}

Infrastructure::ImagePtr FTRenderer::draw(FT_FaceRec_* font, std::string str)
{
	FT_Bitmap *letters = new FT_Bitmap[str.size()];
	std::vector<std::pair<int, int> > positions;
	std::vector<signed long> heights;
	Infrastructure::ImagePtr img = nullptr;

	int height = 0;
	int width = 0;

	FT_GlyphSlot slot = font->glyph;
	int pen_x,pen_y,n;

	pen_x = 0;
	pen_y = 0;
	FT_Error error;
	for(n = 0; n<str.size();n++)
	{
		pen_y = 0;

		FT_UInt glyph_index;

		glyph_index = FT_Get_Char_Index(font, str[n]);

		error = FT_Load_Glyph(font, glyph_index, FT_LOAD_DEFAULT);
		if(error)
			continue;

		error = FT_Render_Glyph(font->glyph, FT_RENDER_MODE_NORMAL);
		if(error)
			continue;

		Infrastructure::Pixel p;
		//render into image
		
		FT_Int x_max = slot->bitmap.width;
		FT_Int y_max = slot->bitmap.rows;

		height = max(height, y_max);

		copyBitmap(&slot->bitmap,&letters[n]);

		positions.push_back(make_pair(pen_x, pen_y));

		heights.push_back(slot->metrics.height>>6);

//		const int line = 50;
//		pen_y += line-(slot->metrics.height>>6);
//		for(auto i=pen_x, gi=0; i<m_image->getWidth() && gi < x_max; i++, gi++)
//		{
//			for(auto j=pen_y, gj =0; j<m_image->getHeight()&&gj<y_max; j++, gj++)
//			{
//				//m_image->setPixel()
//				p.A = p.R = p.G = p.B =slot->bitmap.buffer[gj*x_max+gi];
//				
//				m_image->setPixel(p,i,j);
//			}
//		}

		pen_x += slot->advance.x >> 6;
		pen_x -= slot->metrics.horiBearingX>>6;
		pen_y += slot->advance.y >> 6;
	}
	width = pen_x;

	img = make_shared<Infrastructure::Image>(width, height);

	for(int n=0;n<str.size();n++){

		pen_x = positions[n].first;
		pen_y = positions[n].second;
		pen_y += height - heights[n];
		Infrastructure::Pixel p;
		auto x_max = letters[n].width;
		auto y_max = letters[n].rows;

		for(auto i=pen_x, gi=0; i<img->getWidth() && gi < x_max; i++, gi++)
		{
			for(auto j=pen_y, gj =0; j<img->getHeight()&&gj<y_max; j++, gj++)
			{
				//m_image->setPixel()
				p.A = p.R = p.G = p.B =letters[n].buffer[gj*x_max+gi];
				
				img->setPixel(p,i,j);
			}
		}

		deleteBitmap(&letters[n]);
	}
	delete[] letters;
	return img;
}
