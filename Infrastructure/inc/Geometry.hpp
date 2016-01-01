//
// Created by Moustapha on 06/10/2015.
//

#ifndef BREAK_0_1_GEOMETRY_HPP
#define BREAK_0_1_GEOMETRY_HPP

#include "Globals.hpp"
#include "Object.hpp"
#include "GPUResource.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Primitive.hpp"

namespace Break{
    namespace Infrastructure{
        class BREAK_API Geometry: public Object, public GPUResource{
        public:

            VertexBufferPtr m_vertexBuffer;
            IndexBufferPtr m_indexBuffer;

            u32 m_indicesCount, m_verticesCount, m_instanceCount;

            Primitive m_primitive;

            RTTI(Geometry);

            Geometry(ISet* vertices, IndexSet* indices, Primitive type):Object("Geometry",Geometry::Type)
            {
                if(vertices){
                    m_vertexBuffer = std::make_shared<VertexBuffer>(*vertices,GPU_ISA::STATIC);
                    m_verticesCount = vertices->count();
                }

                if(indices){
                    m_indexBuffer = std::make_shared<IndexBuffer>(*indices,GPU_ISA::STATIC);
                    m_indicesCount = indices->count();
                }

                m_primitive = type;

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;

                ins.args.push(GPU_ISA::GEOMETRY);
                ins.args.push(m_vertexBuffer->getHandle());
                ins.args.push(m_indexBuffer->getHandle());
                MemoryLayout layout = vertices->getMemoryLayout();
                ins.args.push(&layout);
                m_handle = Services::getGPU_VM()->execute(ins);
            }

            Geometry(VertexBufferPtr vb, IndexBufferPtr ib, Primitive type){
                m_vertexBuffer = vb;
                m_indexBuffer = ib;
                m_primitive = type;
                m_indicesCount = 0;
                m_verticesCount = 0;
                m_instanceCount = 0;

                GPUIns ins;
                ins.instruction = GPU_ISA::GEN;

                ins.args.push(GPU_ISA::GEOMETRY);
                ins.args.push(m_vertexBuffer->getHandle());
				if(m_indexBuffer != nullptr)
					ins.args.push(m_indexBuffer->getHandle());
				else
					ins.args.push(NULL);
                MemoryLayout layout = m_vertexBuffer->getMemoryLayout();
                ins.args.push(&layout);
                m_handle = Services::getGPU_VM()->execute(ins);
            }

            Geometry(const Geometry& val):Object(val){
                m_vertexBuffer = val.m_vertexBuffer;
                m_verticesCount = val.m_verticesCount;
                m_indexBuffer = val.m_indexBuffer;
                m_indicesCount = val.m_indicesCount;
                m_instanceCount = val.m_instanceCount;
                m_primitive = val.m_primitive;
                m_handle = val.m_handle;
            }

            virtual ~Geometry()
            {
                GPUIns ins;
                ins.instruction = GPU_ISA::DEL;
                ins.args.push(GPU_ISA::GEOMETRY);
                ins.args.push(m_handle.get());

                Services::getGPU_VM()->execute(ins);

                m_vertexBuffer = nullptr;
                m_indexBuffer = nullptr;
                m_verticesCount = 0;
                m_indicesCount = 0;
                m_instanceCount = 0;

            }

            MemoryLayout& getMemoryLayout(){
                return m_vertexBuffer->getMemoryLayout();
            }

            void draw(){
                ///draw code
                if(m_vertexBuffer && m_indexBuffer){
                    //Break::Infrastructure::Engine::Instance->GraphicsDevice->drawGeometry(this,Primitive::Mode::INDEXED);
                    GPUIns ins;
                    ins.instruction = GPU_ISA::DRAW_INDEXED;

                    ins.args.push(m_primitive);
                    ins.args.push(m_handle.get());
                    ins.args.push(m_vertexBuffer->getHandle());
                    ins.args.push(m_indexBuffer->getHandle());
                    ins.args.push(m_indicesCount);
                    ins.args.push(&m_vertexBuffer->getMemoryLayout());

                    Services::getGPU_VM()->execute(ins);
                }else if(m_vertexBuffer){
                    //Break::Infrastructure::Engine::Instance->GraphicsDevice->drawGeometry(this,Primitive::Mode::NORMAL);
                    GPUIns ins;
                    ins.instruction = GPU_ISA::DRAW;

                    ins.args.push(m_primitive);
                    ins.args.push(m_handle.get());
                    ins.args.push(m_vertexBuffer->getHandle());
                    ins.args.push(m_verticesCount);
                    ins.args.push(&m_vertexBuffer->getMemoryLayout());

                    Services::getGPU_VM()->execute(ins);
                }
            }
        };
        typedef std::shared_ptr<Geometry> GeometryPtr;
    }
}
#endif //BREAK_0_1_GEOMETRY_HPP
