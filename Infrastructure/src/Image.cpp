//
// Created by Moustapha on 02/10/2015.
//

#include "Image.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Image::Image()
        :Asset("Image", Image::Type){
    m_width = 0;
    m_height = 0;
    m_depth = 0;
	m_size = 0;
    m_data = nullptr;
}

Image::~Image(){
    if(m_data)
		delete[] m_data;
}

Image::Image(Pixel* data,unsigned int width,unsigned int height/* =1 */, unsigned int depth/* =1 */)
        :Asset("Image", Image::Type)
{
    m_width = width;
    m_height = height;
    m_depth = depth;
	m_size = m_width*m_height*m_depth*sizeof(Pixel);
	m_data = new Pixel[m_size];
	memcpy(m_data,data,m_size);
}

Image::Image(unsigned int width,unsigned int height/* =1 */, unsigned int depth/* =1 */)
        :Asset("Image", Image::Type)
{
    m_width = width;
    m_height = height;
    m_depth = depth;

    m_size = m_width *m_height * m_depth*sizeof(Pixel);
    m_data = new Pixel[m_size];
}

Pixel* Image::getPixels(){
    return m_data;
}

u32 Image::getWidth(){
    return m_width;
}

u32 Image::getHeight(){
    return m_height;
}

u32 Image::getDepth(){
    return m_depth;
}

u32 Image::getSize()
{
    return m_size;
}

void Image::setWidth(u32 val){
    m_width = val;
}

void Image::setHeight(u32 val){
    m_height = val;
}

void Image::setDepth(u32 val){
    m_depth = val;
}

Pixel& Image::getPixel(unsigned int x,unsigned int y, unsigned int z){
    unsigned long ix = x+m_width *(y+m_depth*z);
    return m_data[ix];
}

ImagePtr Image::clone(){
	Pixel* clone_pixels = new Pixel[m_size];
	memcpy(clone_pixels,m_data,m_size);
    return std::make_shared<Image>(clone_pixels,m_width,m_height,m_depth);
}