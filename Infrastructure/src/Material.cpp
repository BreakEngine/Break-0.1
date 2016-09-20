#include "Material.h"
#include "stb_image.h"

using namespace Break;
using namespace Break::Infrastructure;
#include <cstdlib>
#include <ResourceLoader.hpp>

Material::Material()
{
	texUnit = -1;
}
Material::Material(std::string fileName, int texUnit_)
{
    unsigned char* data = stbi_load((fileName).c_str(), &width, &height, &numComponents, 4);
	texUnit = texUnit_;
    if(data == NULL)
		printf( "Unable to load texture \n" );

	glActiveTexture(texUnit); 
    glGenTextures(1, &m_Material);
    glBindTexture(GL_TEXTURE_2D, m_Material); 
    
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
  
	glBindTexture(GL_TEXTURE_2D, 0); 

    stbi_image_free(data);

}


Material::~Material()
{
	glDeleteTextures(1, &m_Material);
}

void Material::useMaterial()
{
	if(texUnit != -1)
	{
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, m_Material);
	}
}

