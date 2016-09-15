//
// Created by Moustapha on 09/10/2015.
//
#define GLM_FORCE_RADIANS
#include "SpriteBatch.hpp"
#include <MathUtils.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Services.hpp>
#include "Sprite.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;

void SpriteBatch::checkFlush(Texture2D* texture){
    if(texture!=m_texture)
        flush();
    m_texture = texture;
}

glm::vec2 SpriteBatch::rotatePoint(glm::vec2 point, real32 angle){
    auto mat = glm::rotate(glm::mat4(1),MathUtils::toRadians(angle),glm::vec3(0,0,1));
    auto result = mat*glm::vec4(point.x,point.y,0,1);
    return glm::vec2(result.x,result.y);
}

glm::vec2 SpriteBatch::rotatePoint(glm::vec2 point, real32 angle, glm::vec2 origin,Rect dest){
	auto mat = glm::translate(glm::mat4(1),glm::vec3(-(origin.x),-(origin.y),0));
    mat = glm::rotate(mat,MathUtils::toRadians(angle),glm::vec3(0,0,1));
	mat = glm::translate(mat,glm::vec3((origin.x),(origin.y),0));
    auto result = mat*glm::vec4(point.x,point.y,0,1);
    return glm::vec2(result.x,result.y);
}

SpriteBatch::SpriteBatch(GPUProgramPtr shader) {
    m_vertices.resize(limit*4);
    m_indices.resize(limit*6);

    if(shader)
    {
        m_shader = shader.get();
        m_customShader = true;
    }else {
        m_shader = static_cast<GPUProgram *>(Services::getAssetManager()->find("_shape2DShader"));
        m_customShader = false;
    }

    m_geometry = make_shared<Geometry>(
            make_shared<VertexBuffer>(RAMBuffer::asHandle(&m_vertices[0],m_vertices.count()*sizeof(Vertex2DPosColorTex)),
                                      Vertex2DPosColorTex::getDescription(),
                                      GPU_ISA::DYNAMIC),
            make_shared<IndexBuffer>(RAMBuffer::asHandle(&m_indices[0],m_indices.count()*sizeof(u32)),
                                     GPU_ISA::DYNAMIC),
            Primitive::TRIANGLES
   );

    m_count = 0;
    m_iCount = 0;
    m_vCount =0;
}

SpriteBatch::~SpriteBatch() {
    m_geometry = nullptr;
    if(m_customShader)
        delete m_shader;
}

void SpriteBatch::begin() {
    m_iCount = 0;
    m_vCount = 0;
    m_count = 0;

    auto idmat = glm::mat4(1);
    auto proj = glm::ortho(0.0f,(float)Services::getEngine()->getApplication()->getWindow()->getWidth(),(float)Services::getEngine()->getApplication()->getWindow()->getHeight(),0.0f,-10.0f,10.0f);

    glm::mat4* dummy = new glm::mat4(1);
    m_shader->setUniform("model", dummy);
    m_shader->setUniform("view", dummy);
    m_shader->setUniform("projection",&proj);
    delete dummy;

    m_shader->use();
}

void SpriteBatch::draw(Texture2D* texture, int x, int y, Color color)
{
    draw(texture,Rect(x,y,texture->getWidth(),texture->getHeight()),0,glm::vec2(0,0),color);
}

void SpriteBatch::draw(Texture2D* texture, int x, int y, int width, int height, Color color)
{
	if(texture)
		draw(texture,Rect(x,y,width,height),Rect(0,0,texture->getWidth(),texture->getHeight()),0,glm::vec2(0,0),color);
	else
		draw(texture,Rect(x,y,width,height),Rect(0,0,1,1),0,glm::vec2(0,0),color);
}

void SpriteBatch::draw(Texture2D* texture, Rect dest, float angle,glm::vec2 origin, Color color)
{
    if(texture)
        draw(texture,dest,Rect(0,0,texture->getWidth(),texture->getHeight()),angle,origin,color);
    else
        draw(texture,dest,Rect(0,0,1,1),angle,origin,color);
}

void SpriteBatch::draw(Texture2D* texture, Rect dest, Rect src, float angle,glm::vec2 origin, Color color)
{
	origin.x *=-1;
	origin.y *=-1;
    checkFlush(texture);

    if(m_count>=limit)
        flush();

	
    auto p1 = rotatePoint(glm::vec2(0,0),angle,origin,dest)+glm::vec2(dest.x,dest.y);
    auto p2 = rotatePoint(glm::vec2(0,dest.height),angle,origin,dest)+glm::vec2(dest.x,dest.y);
    auto p3 = rotatePoint(glm::vec2(dest.width,0),angle,origin,dest)+glm::vec2(dest.x,dest.y);
    auto p4 = rotatePoint(glm::vec2(dest.width,dest.height),angle,origin,dest)+glm::vec2(dest.x,dest.y);
	

	/*
	glm::vec2 p1,p2,p3,p4;

	float cosA = cos(MathUtils::toRadians(angle));
	float sinA = sin(MathUtils::toRadians(angle));
	p1.x = dest.x+origin.x*cosA-origin.y*sinA;
	p1.y = dest.y+origin.x*sinA+origin.y*cosA;

	p2.x = dest.x+(origin.x+dest.width)*cosA-origin.y*sinA;
	p2.y = dest.y+(origin.x+dest.width)*sinA+origin.y*cosA;

	p3.x = dest.x+origin.x*cosA-(origin.y+dest.height)*sinA;
	p3.y = dest.y+origin.x*sinA+(origin.y+dest.height)*cosA;

	p4.x = dest.x+(origin.x+dest.width)* cosA-(origin.y+dest.height)*sinA;
	p4.y = dest.y+(origin.x+dest.width)* sinA+(origin.y+dest.height)*cosA;
	*/


    glm::vec2 t1,t2,t3,t4;
    if(texture){
        t1 = glm::vec2(src.x/texture->getWidth(),src.y/texture->getHeight());
        t2 = glm::vec2(src.x/texture->getWidth(),(src.y+src.height)/texture->getHeight());
        t3 = glm::vec2((src.x+src.width)/texture->getWidth(),src.y/texture->getHeight());
        t4 = glm::vec2((src.x+src.width)/texture->getWidth(),(src.y+src.height)/texture->getHeight());
    }else
    {
        t1 = glm::vec2(0,0);
        t2 = glm::vec2(0,1);
        t3 = glm::vec2(1,0);
        t4 = glm::vec2(1,1);
    }

    m_vertices[m_vCount] = Vertex2DPosColorTex(p1,color,t1);
    m_vertices[m_vCount+1] = Vertex2DPosColorTex(p2,color,t2);
    m_vertices[m_vCount+2] = Vertex2DPosColorTex(p3,color,t3);
    m_vertices[m_vCount+3] = Vertex2DPosColorTex(p4,color,t4);

    m_indices[m_iCount] = m_vCount+0;
    m_indices[m_iCount+1] = m_vCount+1;
    m_indices[m_iCount+2] = m_vCount+2;
    m_indices[m_iCount+3] = m_vCount+2;
    m_indices[m_iCount+4] = m_vCount+1;
    m_indices[m_iCount+5] = m_vCount+3;

    m_vCount+=4;
    m_iCount+=6;
    m_count++;
}

void SpriteBatch::flush()
{
    if(m_count<=0)
        return;

    if(m_texture){
        m_shader->setTexture("spTex",m_texture);
        glm::vec4 hasTex = glm::vec4(1,1,1,1);
        m_shader->setUniform("hasTexture",&hasTex);
        m_shader->use();
    }else
    {
        glm::vec4 hasTex = glm::vec4(0,1,1,1);
        m_shader->setUniform("hasTexture",&hasTex);
        m_shader->use();
    }

    //_geometry->getGeometryData().vertices->fromHandle(&_vertices[0],_vertices.count()*_vertices.declaration.getSize());
    //_geometry->getGeometryData().indices->fromHandle(&_indices[0],_indices.count()*4);
    m_geometry->m_vertexBuffer->flush();
    m_geometry->m_indexBuffer->flush();
    m_geometry->m_verticesCount = m_vCount;
    m_geometry->m_indicesCount = m_iCount;
    m_geometry->draw();
    //_geometry->getGeometryData().vertices->clear();
    //_geometry->getGeometryData().indices->clear();
    m_geometry->m_verticesCount = 0;
    m_geometry->m_indicesCount = 0;
    //_vertices.clear();
    //_indices.clear();

    m_count = 0;
    m_vCount = 0;
    m_iCount = 0;
}

void SpriteBatch::end()
{
    if(m_count>0)
        flush();
}
