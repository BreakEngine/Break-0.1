//
// Created by Moustapha on 06/10/2015.
//

#include "UniformBuffer.hpp"
#include "GPUIns.hpp"
#include "Services.hpp"
#include "ServiceException.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

UniformBuffer::UniformBuffer():Object("UniformBuffer",UniformBuffer::Type){
    m_buffer = nullptr;
}

UniformBuffer::UniformBuffer(u32 size, u32 slot, GPU_ISA type):Object("UniformBuffer",UniformBuffer::Type)
{
    if(type != GPU_ISA::VERTEX_SHADER && type != GPU_ISA::PIXEL_SHADER)
        throw ServiceException("unexpected GPU_ISA parameter");
    m_shader = type;
    m_slot = slot;
    m_buffer = std::make_shared<RAMBuffer>(size);

    GPUIns ins;
    ins.instruction = GPU_ISA::GEN;
    ins.args.push(GPU_ISA::UNIFORM_BUFFER);
    ins.args.push(m_buffer->getCapacity());
    ins.args.push(m_buffer->getData());
    ins.args.push(m_slot);

    try{
        m_handle = Services::getGPU_VM()->execute(ins);
    }catch(ServiceException e)
    {
        std::cerr<<e.what()<<std::endl;
    }
}

UniformBuffer::UniformBuffer(const UniformBuffer& val){
    m_buffer = val.m_buffer;
    m_slot = val.m_slot;
    m_shader = val.m_shader;
    m_handle = val.m_handle;
    m_type = val.m_type;
}

UniformBuffer::~UniformBuffer(){
    GPUIns ins;
    ins.instruction = GPU_ISA::DEL;
    ins.args.push(GPU_ISA::UNIFORM_BUFFER);
    ins.args.push(m_handle.get());

    Services::getGPU_VM()->execute(ins);

    m_buffer = nullptr;
}

void UniformBuffer::use(){

	//if buffer changed then move those changes to memory
	if(m_buffer->isChanged()){
		flush();
		m_buffer->setChanged(false);
	}

    //use code
    GPUIns ins;
    ins.instruction = GPU_ISA::BIND;
    ins.args.push(GPU_ISA::UNIFORM_BUFFER);
    ins.args.push(m_handle.get());
    ins.args.push(m_shader);


    ins.args.push(m_slot);

    try{
        Services::getGPU_VM()->execute(ins);
    }catch(ServiceException e)
    {
        std::cerr<<e.what()<<std::endl;
    }
}

u32 UniformBuffer::getSlot(){
    return m_slot;
}

GPU_ISA UniformBuffer::getShaderType(){
    return m_shader;
}

void UniformBuffer::flush(){
    ///update code
    GPUIns ins;
    ins.instruction = GPU_ISA::MAP;
    ins.args.push(GPU_ISA::UNIFORM_BUFFER);
    ins.args.push(m_handle.get());
    ins.args.push(m_buffer->getCapacity());
    ins.args.push(m_buffer->getData());

    Services::getGPU_VM()->execute(ins);
}

RAMBuffer* UniformBuffer::getBuffer(){
    return m_buffer.get();
}