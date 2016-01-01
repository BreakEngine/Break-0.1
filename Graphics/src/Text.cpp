#include "Text.h"
#include "FTRenderer.hpp"
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;

Text::Text(IFTRenderer* renderer,FontFacePtr font, std::string str):Object("Text",Text::Type)
{
	m_image = nullptr;
	m_needDraw = true;
	m_str = str;
	m_font = font;
	m_renderer = renderer;
	position = glm::vec2(0,0);
	origin = glm::vec2(0,0);
	DPI = 96;
	size = 16;
	angle = 0;
	color = Color(255,255,255,255);

}

Text::~Text()
{
	m_image = nullptr;
	m_font = nullptr;
	m_needDraw = false;
	m_str.clear();
}

void Text::setText(std::string str)
{
	m_str = str;
	m_needDraw = true;
}

std::string Text::getText()
{
	return m_str;
}

void Text::setFont(FontFacePtr font)
{
	m_font = font;
	m_needDraw = true;
}

FontFace* Text::getFont()
{
	return m_font.get();
}

Infrastructure::Image* Text::getImage()
{
	if(m_needDraw && m_renderer){
		m_needDraw = false;
		m_font->setSize(size);
		m_font->setDPI(DPI);
		m_image = m_renderer->draw(m_font->getFont(),m_str);
	}
	return m_image.get();
}

Infrastructure::Texture2D* Text::getTexture()
{
	if(m_needDraw && m_renderer){
		m_needDraw = false;
		m_font->setSize(size);
		m_font->setDPI(DPI);
		m_image = m_renderer->draw(m_font->getFont(),m_str);
		if(m_texture)
			m_texture->update(m_image);
		else
			m_texture = std::make_shared<Texture2D>(m_image);
	}
	return m_texture.get();
}

void Text::draw(SpriteBatch* batcher)
{
	auto tex = getTexture();
	batcher->draw(tex,Rect(position.x,position.y,m_image->getWidth(),m_image->getHeight()),angle,origin,color);
}