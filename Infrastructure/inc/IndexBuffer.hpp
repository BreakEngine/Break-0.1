//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_INDEXBUFFER_HPP
#define BREAK_0_1_INDEXBUFFER_HPP

#include "Globals.hpp"
#include "Object.hpp"
#include "GPU_ISA.hpp"
#include "IndexSet.hpp"
#include "RAMBuffer.hpp"
#include <memory>

namespace Break{
    namespace Infrastructure{

        class BREAK_API IndexBuffer: public Object, public GPUResource{
        protected:

            ///RAMBuffer
            RAMBufferPtr m_buffer;

            ///static dynamic switch
            GPU_ISA m_bufferSD;

        public:

            RTTI(IndexBuffer);

            IndexBuffer():Object("IndexBuffer",IndexBuffer::Type)
            {
                m_buffer = nullptr;
            }

            IndexBuffer(RAMBuffer* buffer, GPU_ISA type):Object("IndexBuffer",IndexBuffer::Type){
                m_bufferSD = type;

                if(type != GPU_ISA::DYNAMIC && type != GPU_ISA::STATIC)
                {
                    throw ServiceException("unexpected GPU_ISA parameter");
                }
                m_buffer = RAMBufferPtr(buffer);
                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::INDEX_BUFFER);
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

            IndexBuffer(const IndexBuffer& val):Object(val){
                m_buffer = val.m_buffer;
                m_bufferSD = val.m_bufferSD;
                m_handle = val.m_handle;
            }

            IndexBuffer(u32 size, GPU_ISA type):Object("IndexBuffer",IndexBuffer::Type)
            {
                m_bufferSD = type;
                if(type != GPU_ISA::DYNAMIC && type != GPU_ISA::STATIC)
                {
                    throw ServiceException("unexcpected GPU_ISA parameter");
                }
                m_buffer = std::make_shared<RAMBuffer>(size);

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::INDEX_BUFFER);
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

            IndexBuffer(GPU_ISA type):Object("IndexBuffer",IndexBuffer::Type)
            {
                m_bufferSD = type;
                if(type == GPU_ISA::STATIC){
                    m_buffer = std::make_shared<RAMBuffer>(STATIC_OPTIMAL_SIZE);
                }else if(type == GPU_ISA::DYNAMIC){
                    m_buffer = std::make_shared<RAMBuffer>(DYNAMIC_OPTIMAL_SIZE);
                }else{
                    throw ServiceException("unexcpected GPU_ISA parameter");
                }

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::INDEX_BUFFER);
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

            IndexBuffer(IndexSet& set, GPU_ISA type):Object("IndexBuffer",IndexBuffer::Type)
            {
                m_buffer = set.copyData();
                m_bufferSD = type;

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;
                ins.args.push(GPU_ISA::INDEX_BUFFER);
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

            virtual ~IndexBuffer(){
                GPUIns ins;
                ins.instruction = GPU_ISA::DEL;
                ins.args.push(GPU_ISA::INDEX_BUFFER);
                ins.args.push(m_handle.get());

                Services::getGPU_VM()->execute(ins);

                m_buffer = nullptr;
            }

            void append(IndexSet& set){
                m_buffer->append(*set.copyHandle());
            }

            void use(){
                if(m_buffer->isChanged()){
                    flush();
                    m_buffer->setChanged(false);
                }
                //use code
                GPUIns ins;
                ins.instruction = GPU_ISA::BIND;
                ins.args.push(GPU_ISA::INDEX_BUFFER);
                ins.args.push(m_handle.get());

                Services::getGPU_VM()->execute(ins);
            }

            void flush(){
                //update code
                GPUIns ins;
                ins.instruction = GPU_ISA::MAP;
                ins.args.push(GPU_ISA::INDEX_BUFFER);
                ins.args.push(m_handle.get());
                ins.args.push(m_buffer->getCapacity());
                ins.args.push(m_buffer->getData());

                Services::getGPU_VM()->execute(ins);
            }

            RAMBuffer* getBuffer(){
                return m_buffer.get();
            }
        };
        typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;
    }
}
#endif //BREAK_0_1_INDEXBUFFER_HPP
