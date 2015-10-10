//
// Created by Moustapha on 04/10/2015.
//

#include "Texture.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Texture::Texture(bool mipmaps):Asset("Texture",Texture::Type),m_mipmaps(mipmaps){
    m_image = nullptr;
}

Texture::~Texture(){
    m_image = nullptr;
}


unsigned Texture::getWidth()
{
    if(m_image)
        return m_image->getWidth();
    else
        return 0;
}

unsigned Texture::getHeight()
{
    if(m_image)
        return m_image->getHeight();
    else
        return 0;
}

unsigned Texture::getDepth()
{
    if(m_image)
        return m_image->getDepth();
    else
        return 0;
}