//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_VERTEXBUFFER_HPP
#define BREAK_0_1_VERTEXBUFFER_HPP

#include "Globals.hpp"
#include "Object.hpp"
#include "GPU_ISA.hpp"
#include "GPUIns.hpp"
#include "VertexSet.hpp"
#include "MemoryLayout.hpp"
#include "RAMBuffer.hpp"
#include <memory>
#include <iostream>
#include "Services.hpp"

namespace Break{
    namespace Infrastructure{

        class BREAK_API VertexBuffer: public GPUResource, public Object{
        protected:

            ///RAMBuffer
            RAMBufferPtr m_buffer;

            ///static dynamic flag
            GPU_ISA m_bufferSD;

            ///vertex shader input layout
            MemoryLayout m_inputLayout;
        public:

            RTTI(VertexBuffer);

            VertexBuffer():Object("VertexBuffer",VertexBuffer::Type){
                m_buffer = nullptr;
            }

            VertexBuffer(RAMBuffer* buffer, MemoryLayout layout, GPU_ISA type):Object("VertexBuffer",VertexBuffer::Type){
                m_bufferSD = type;
                m_inputLayout = layout;
                if(type != GPU_ISA::DYNAMIC && type != GPU_ISA::STATIC)
                {
                    throw ServiceException("unexpected GPU_ISA parameter");
                }
                m_buffer = RAMBufferPtr(buffer);
                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::VERTEX_BUFFER);
                ins.args.push(m_bufferSD);
                ins.args.push(m_buffer->getCapacity());
                ins.args.push(m_buffer->getData());

                try{
                    m_handle = Services::getGPU_VM()->execute(ins);
                }catch(ServiceException e)
                {
                    std::cerr<<e.what()<<std::endl;
                }
            }

            ///copy constrcutor
            VertexBuffer(const VertexBuffer& val):Object(val){
                m_buffer = val.m_buffer;
                m_bufferSD = val.m_bufferSD;
                m_inputLayout= val.m_inputLayout;
                m_handle = val.m_handle;
            }

            ///constructor for initializing an empty buffer based on type
            VertexBuffer(u32 size,GPU_ISA type,MemoryLayout layout):Object("VertexBuffer",VertexBuffer::Type){
                m_bufferSD = type;
                m_inputLayout = layout;
                if(type != GPU_ISA::DYNAMIC && type != GPU_ISA::STATIC)
                {
                    throw ServiceException("unexpected GPU_ISA parameter");
                }
                m_buffer = std::make_shared<RAMBuffer>(size);

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::VERTEX_BUFFER);
                ins.args.push(m_bufferSD);
                ins.args.push(m_buffer->getCapacity());
                ins.args.push(m_buffer->getData());

                try{
                    m_handle = Services::getGPU_VM()->execute(ins);
                }catch(ServiceException e)
                {
                    std::cerr<<e.what()<<std::endl;
                }

            }

            ///constructor for initializing an empty buffer based on type
            VertexBuffer(GPU_ISA type,MemoryLayout layout):Object("VertexBuffer",VertexBuffer::Type){
                m_bufferSD = type;
                m_inputLayout = layout;
                if(type == GPU_ISA::STATIC)
                {
                    m_buffer = std::make_shared<RAMBuffer>(STATIC_OPTIMAL_SIZE);
                }else if(type == GPU_ISA::DYNAMIC){
                    m_buffer = std::make_shared<RAMBuffer>(DYNAMIC_OPTIMAL_SIZE);
                }else
                {
                    throw ServiceException("unexpected GPU_ISA parameter");
                }

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::VERTEX_BUFFER);
                ins.args.push(m_bufferSD);
                ins.args.push(m_buffer->getCapacity());
                ins.args.push(m_buffer->getData());

                try{
                    m_handle = Services::getGPU_VM()->execute(ins);
                }catch(ServiceException e)
                {
                    std::cerr<<e.what()<<std::endl;
                }
            }

            ///constructor for initializing from a vertex set, the managed way
            VertexBuffer(ISet& set,GPU_ISA type):Object("VertexBuffer",VertexBuffer::Type){
                m_buffer = set.copyData();
                m_bufferSD = type;
                m_inputLayout = set.getMemoryLayout();

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::VERTEX_BUFFER);
                ins.args.push(m_bufferSD);
                ins.args.push(m_buffer->getCapacity());
                ins.args.push(m_buffer->getData());

                try{
                    m_handle = Services::getGPU_VM()->execute(ins);
                }catch(ServiceException e)
                {
                    std::cerr<<e.what()<<std::endl;
                }
            }

            virtual ~VertexBuffer(){
                GPUIns ins;
                ins.instruction = GPU_ISA::DEL;
                ins.args.push(GPU_ISA::VERTEX_BUFFER);
                ins.args.push(m_handle.get());

                Services::getGPU_VM()->execute(ins);

                m_buffer = nullptr;

            }

            ///appends set to the buffer
            void append(ISet& set){
                m_buffer->append(*set.copyHandle());
            }

            ///binds buffer to graphics context
            void use(){
                if(m_buffer->isChanged()) {
                    flush();
                    m_buffer->setChanged(false);
                }
                GPUIns ins;
                ins.instruction = GPU_ISA::BIND;
                ins.args.push(GPU_ISA::VERTEX_BUFFER);
                ins.args.push(m_handle.get());
                ins.args.push(m_inputLayout.getSize());

                Services::getGPU_VM()->execute(ins);
            }

            void flush(){
                //update code
                GPUIns ins;
                ins.instruction = GPU_ISA::MAP;
                ins.args.push(GPU_ISA::VERTEX_BUFFER);
                ins.args.push(m_handle.get());
                ins.args.push(m_buffer->getCapacity());
                ins.args.push(m_buffer->getData());

                Services::getGPU_VM()->execute(ins);
            }

            RAMBuffer* getBuffer(){
                return m_buffer.get();
            }

            MemoryLayout& getMemoryLayout(){
                return m_inputLayout;
            }
        };
        typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;
    }
}
#endif //BREAK_0_1_VERTEXBUFFER_HPP
