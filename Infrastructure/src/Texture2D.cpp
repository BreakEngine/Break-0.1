//
// Created by Moustapha on 05/10/2015.
//

#include "GPUIns.hpp"
#include "GPU_ISA.hpp"
#include "Services.hpp"
#include "Texture2D.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Texture2D::Texture2D(ImagePtr src,bool mipmaps):Texture(mipmaps){
    m_image = src;

    GPUIns ins;
    ins.instruction = GPU_ISA::GEN;

    ins.args.push(GPU_ISA::TEXTURE2D);
    ins.args.push(m_image.get());
    ins.args.push(m_mipmaps);

    m_handle = Services::getGPU_VM()->execute(ins);

}

Texture2D::~Texture2D(){
    //Engine delete texture
    //Infrastructure::Engine::Instance->GraphicsDevice->deleteTexture2D(this);
    GPUIns ins;
    ins.instruction = GPU_ISA::DEL;

    ins.args.push(GPU_ISA::TEXTURE2D);
    ins.args.push(m_handle.get());

    Services::getGPU_VM()->execute(ins);
}

void Texture2D::update(ImagePtr src){
    if(m_image != src)
        m_image = src;
    //Infrastructure::Engine::Instance->GraphicsDevice->updateTexture2D(this,*_image);
    GPUIns ins;
    ins.instruction = GPU_ISA::MAP;
    ins.args.push(GPU_ISA::TEXTURE2D);
    ins.args.push(m_handle.get());
    ins.args.push(m_image.get());

    Services::getGPU_VM()->execute(ins);
}

ImagePtr Texture2D::readImage(){
    return m_image;
}

void Texture2D::use(GPU_ISA type,unsigned int unit){
    //Engine use texture
    //Infrastructure::Engine::Instance->GraphicsDevice->useTexture2D(this,unit,type);
    GPUIns ins;
    ins.instruction = GPU_ISA::BIND;
    ins.args.push(GPU_ISA::TEXTURE2D);

    ins.args.push(m_handle.get());

    ins.args.push(type);

    ins.args.push(unit);

    Services::getGPU_VM()->execute(ins);
}

/*
bool Texture2D::createGPUResource(){
    //Engine create Texture
    //return Infrastructure::Engine::Instance->GraphicsDevice->createTexture2D(this,*_image);
    Renderer::GPUIns ins;
    ins.instruction = Renderer::GPU_ISA::GEN;

    ins.args.push(Renderer::GPU_ISA::TEXTURE2D);
    ins.args.push(_image.get());
    ins.args.push(_mipmaps);

    _handle = Services::getGPU_VM()->execute(ins);
    return true;
}
 */