//
// Created by Moustapha on 04/10/2015.
//

#include "GPUProgram.hpp"
#include "GPUIns.hpp"
#include "Services.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

GPUProgram::GPUProgram(string vs,string ps,MemoryLayout inputLayout):Asset("GPUProgram",GPUProgram::Type),m_vs(vs),m_ps(ps),m_inputLayout(inputLayout){
    GPUIns ins;
    ins.instruction = GPU_ISA::GEN;

    ins.args.push(GPU_ISA::PROGRAM);
    ins.args.push(&vs);
    ins.args.push(&ps);
    ins.args.push(&inputLayout);

    m_handle = Services::getGPU_VM()->execute(ins);

    m_vs.clear();
    m_ps.clear();
}

GPUProgram::GPUProgram(const GPUProgram& val):GPUResource(val){
    m_inputLayout=val.m_inputLayout;
    m_uniformBlocks = val.m_uniformBlocks;
    m_uniformsTable = val.m_uniformsTable;
    m_samplersTable = val.m_samplersTable;

    m_vs = val.m_vs;
    m_ps = val.m_ps;
}

GPUProgram::~GPUProgram(){
    //Break::Infrastructure::Engine::Instance->GraphicsDevice->deleteShader(this);
    GPUIns ins;
    ins.instruction = GPU_ISA::DEL;

    ins.args.push(GPU_ISA::PROGRAM);
    ins.args.push(m_handle.get());

    Services::getGPU_VM()->execute(ins);

    if(m_uniformsTable.size()>0)
        m_uniformsTable.clear();
    if(m_uniformBlocks.size()>0)
        m_uniformBlocks.clear();
    if(m_samplersTable.size()>0)
        m_samplersTable.clear();
    if(m_vs.size()>0)
        m_vs.clear();
    if(m_ps.size()>0)
        m_ps.clear();
}

void GPUProgram::use(){
	flushUniforms();

    GPUIns ins;
    ins.instruction = GPU_ISA::BIND;

    ins.args.push(GPU_ISA::PROGRAM);
    ins.args.push(m_handle.get());

    Services::getGPU_VM()->execute(ins);
}

void GPUProgram::setUniform(string name, void* ptr){
    //set uniform
    auto uniform = m_uniformsTable[name];
    m_uniformBlocks[uniform.m_blockName]->getBuffer()->map(ptr,uniform.m_size,uniform.m_offset);
}

void GPUProgram::setTexture(std::string sampler, Texture* tex)
{
    auto sRow = m_samplersTable[sampler];
    if(tex->m_sampler != sRow.m_state.get()){
        //Break::Infrastructure::Engine::Instance->GraphicsDevice->applySamplerStateToTexture2D(sRow.m_state.get(),tex);
        GPUIns ins;
        ins.instruction = GPU_ISA::APPLY;
        ins.args.push(GPU_ISA::SAMPLER);
        ins.args.push(GPU_ISA::TEXTURE2D);
        ins.args.push(sRow.m_state->getHandle());
        ins.args.push(tex->getHandle());
        ins.args.push(tex->usingMipMaps());
        ins.args.push(sRow.m_state->addressU);
        ins.args.push(sRow.m_state->addressV);
        ins.args.push(sRow.m_state->filter);
        ins.args.push(sRow.m_state->compareFunction);
        ins.args.push(&sRow.m_state->borderColor);

        Services::getGPU_VM()->execute(ins);
        tex->m_sampler = sRow.m_state.get();
    }

    //Break::Infrastructure::Engine::Instance->GraphicsDevice->useSamplerState(sRow.m_state.get(),sRow.m_slot,(Shader::Type)sRow.m_shader);
    GPUIns ins;
    ins.instruction = GPU_ISA::BIND;
    ins.args.push(GPU_ISA::SAMPLER);
    ins.args.push(sRow.m_state->getHandle());
    ins.args.push(sRow.m_shader);
    ins.args.push(sRow.m_slot);
    Services::getGPU_VM()->execute(ins);

    tex->use(sRow.m_shader,sRow.m_slot);
}


//bool GPUProgram::createGPUResource(){
//    //engine->graphicsDevice->create();
//    //return Break::Infrastructure::Engine::Instance->GraphicsDevice->createShader(this);
//    GPUIns ins;
//    ins.instruction = GPU_ISA::GEN;
//
//    ins.args.push(Arg(GPU_ISA::PROGRAM));
//    ins.args.push(Arg(&m_vs));
//    ins.args.push(Arg(&m_ps));
//    ins.args.push(Arg(&m_inputLayout));
//
//    m_handle = Services::getGPU_VM()->execute(ins);
//    return true;
//}

void GPUProgram::registerUniform(std::string name,std::string blockName,unsigned int offset, unsigned int size){
    uniformRow var;
    var.m_blockName = blockName;
    var.m_size = size;
    var.m_offset = offset;
    m_uniformsTable[name] = var;
}

void GPUProgram::registerSampler(std::string name, unsigned slot, SamplerStatePtr state,GPU_ISA shader)
{
    samplersRow var;
    var.m_state = state;
    var.m_shader = shader;
    var.m_slot = slot;
    //Break::Infrastructure::Engine::Instance->GraphicsDevice->createSamplerState(state.get());
    GPUIns ins;
    ins.instruction = GPU_ISA::GEN;
    ins.args.push(GPU_ISA::SAMPLER);
    ins.args.push(state->addressU);
    ins.args.push(state->addressV);
    ins.args.push(state->addressW);
    ins.args.push(state->filter);
    ins.args.push(state->compareFunction);
    ins.args.push(&state->borderColor);
    state->setHandle(Services::getGPU_VM()->execute(ins));
    m_samplersTable[name] = var;
}

void GPUProgram::flushUniforms()
{
    for(auto& buffer : m_uniformBlocks){
        buffer.second->use();
    }
}

void GPUProgram::registerUniformBlock(std::string name,unsigned int size,unsigned int slot,GPU_ISA shader){
    m_uniformBlocks[name] = make_shared<UniformBuffer>(size,slot,shader);
}

std::string GPUProgram::getVertexShader(){
    return m_vs;
}

std::string GPUProgram::getPixelShader(){
    return m_ps;
}

MemoryLayout GPUProgram::getInputLayout(){
    return m_inputLayout;
}