//
// Created by Moustapha on 06/10/2015.
//

#include <cassert>
#include "GPU_VM.hpp"
#include "ServiceException.hpp"
#include "Services.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

Arg GPU_VM::pop(std::queue < Arg > &args) {
    if(args.size()<=0)
        throw ServiceException("stack is out of arguments");

    Arg arg = args.front();

    args.pop();
    return arg;
}

GPU_VM::GPU_VM():Object("GPU_VM",GPU_VM::Type)
{

}

GPU_VM::~GPU_VM()
{

}

GPUHandlePtr GPU_VM::execute(GPUIns& ins){
    if(ins.instruction == GPU_ISA::GEN)
    {

        GPU_ISA tag_01 = pop(ins.args);
        if(tag_01 == GPU_ISA::VERTEX_BUFFER){
            GPU_ISA type = pop(ins.args);
            u32 size = pop(ins.args);
            void* data = pop(ins.args);

            auto res = Services::getGraphicsDevice()->vm_createVertexBuffer(type,size,data);
            if(res == nullptr)
            {
                throw ServiceException("Graphics Device failed to generate a Vertex Buffer");
            }else
            {
                return res;
            }
        }else if(tag_01 == GPU_ISA::INDEX_BUFFER)
        {
            GPU_ISA type = pop(ins.args);
            u32 size = pop(ins.args);
            void* data = pop(ins.args);

            auto res = Services::getGraphicsDevice()->vm_createIndexBuffer(type,size,data);
            if(res == nullptr)
            {
                throw ServiceException("Graphics Device failed to generate a Index Buffer");
            }else
            {
                return res;
            }
        }else if(tag_01 == GPU_ISA::UNIFORM_BUFFER)
        {
            u32 size = pop(ins.args);
            void* data = pop(ins.args);
            u32 slot = pop(ins.args);

            auto res = Services::getGraphicsDevice()->vm_createUniformBuffer(size,data,slot);
            if(res == nullptr)
            {
                throw ServiceException("Graphics Device failed to generate a Uniform Buffer");
            }else
            {
                return res;
            }
        }else if(tag_01 == GPU_ISA::PROGRAM)
        {
            std::string* vertex = pop(ins.args); 
            std::string* pixel = pop(ins.args);
            MemoryLayout* input_layout = pop(ins.args);

            auto res = Services::getGraphicsDevice()->vm_createProgram(*vertex,*pixel,input_layout);
            return res;
        }else if(tag_01 == GPU_ISA::TEXTURE2D)
        {
            Image* img = pop(ins.args);
            bool mipmaps = pop(ins.args);

            auto res = Services::getGraphicsDevice()->vm_createTexture2D(*img,mipmaps);
            return res;

        }else if(tag_01 == GPU_ISA::GEOMETRY)
        {
            GPUHandle* vertex = pop(ins.args);
            GPUHandle* index = pop(ins.args);
            MemoryLayout* input = pop(ins.args);

            auto res = Services::getGraphicsDevice()->vm_createGeometry(vertex,index,input);
            return res;
        }else if(tag_01 == GPU_ISA::SAMPLER)
        {
            TextureAddressMode V,U,W;
            U = pop(ins.args);
            V = pop(ins.args);
            W = pop(ins.args);

            TextureFilter filter = pop(ins.args);

            CompareFunction func = pop(ins.args);

            Color* color = pop(ins.args);

            auto res = Services::getGraphicsDevice()->vm_createSampleState(U,V,W,filter,func,color);
            return res;
        }else
        {
            throw ServiceException("unifentified argument for gen instruction");
        }

    }else if(ins.instruction == GPU_ISA::MAP)
    {

        GPU_ISA tag_01 = pop(ins.args);
        if(tag_01 == GPU_ISA::VERTEX_BUFFER)
        {

            GPUHandle* handle = pop(ins.args);
            u32 size = pop(ins.args);
            void* data = pop(ins.args);

            Services::getGraphicsDevice()->vm_mapVertexBuffer(handle,size,data);
            //auto res = std::shared_ptr<GPUHandle>(handle);
            return nullptr;

        }else if(tag_01 == GPU_ISA::INDEX_BUFFER)
        {

            GPUHandle* handle = pop(ins.args);
            u32 size = pop(ins.args);
            void* data = pop(ins.args);

            Services::getGraphicsDevice()->vm_mapIndexBuffer(handle,size,data);
            //auto res = std::shared_ptr<GPUHandle>(handle);
            return nullptr;

        }else if(tag_01 == GPU_ISA::UNIFORM_BUFFER)
        {
            GPUHandle* handle = pop(ins.args);
            u32 size = pop(ins.args);
            void* data = pop(ins.args);

            Services::getGraphicsDevice()->vm_mapUniformBuffer(handle,size,data);
            //auto res = std::shared_ptr<GPUHandle>(handle);
            return nullptr;
        }else if(tag_01 == GPU_ISA::TEXTURE2D)
        {
            GPUHandle* handle = pop(ins.args);
            Image* img = pop(ins.args);

            Services::getGraphicsDevice()->vm_mapTexture2D(handle,*img);
            return nullptr;
        }else
        {
            throw ServiceException("unifentified argument for map instruction");
        }

    }else if(ins.instruction == GPU_ISA::DEL)
    {

        GPU_ISA tag_01 = pop(ins.args);
        if(tag_01 == GPU_ISA::VERTEX_BUFFER)
        {

            GPUHandle* handle = pop(ins.args);

            Services::getGraphicsDevice()->vm_deleteBuffer(handle);
            //auto res = std::shared_ptr<GPUHandle>(handle);
            return nullptr;

        }else if(tag_01 == GPU_ISA::INDEX_BUFFER)
        {

            GPUHandle* handle = pop(ins.args);

            Services::getGraphicsDevice()->vm_deleteBuffer(handle);
            //auto res = std::shared_ptr<GPUHandle>(handle);
            return nullptr;

        }else if(tag_01 == GPU_ISA::UNIFORM_BUFFER)
        {
            GPUHandle* handle = pop(ins.args);

            Services::getGraphicsDevice()->vm_deleteBuffer(handle);
            return nullptr;
        }else if(tag_01 == GPU_ISA::PROGRAM)
        {
            GPUHandle* handle = pop(ins.args);
            Services::getGraphicsDevice()->vm_deleteShader(handle);
            return nullptr;
        }else if(tag_01 == GPU_ISA::TEXTURE2D)
        {
            GPUHandle* handle = pop(ins.args);
            Services::getGraphicsDevice()->vm_deleteTexture2D(handle);
            return nullptr;
        }else if(tag_01 == GPU_ISA::GEOMETRY)
        {
            GPUHandle* handle = pop(ins.args);
            Services::getGraphicsDevice()->vm_deleteGeometry(handle);
            return nullptr;
        }else if(tag_01 == GPU_ISA::SAMPLER)
        {
            GPUHandle* handle = pop(ins.args);
            Services::getGraphicsDevice()->vm_deleteSampler(handle);
            return nullptr;
        }else
        {
            throw ServiceException("unifentified argument for map instruction");
        }

    }else if(ins.instruction == GPU_ISA::BIND)
    {
        GPU_ISA tag_01 = pop(ins.args);
        if(tag_01 == GPU_ISA::VERTEX_BUFFER)
        {

            GPUHandle* handle = pop(ins.args);
            u32 stride = pop(ins.args);

            Services::getGraphicsDevice()->vm_bindVertexBuffer(handle,stride);
            //auto res = std::shared_ptr<GPUHandle>(handle);
            return nullptr;

        }else if(tag_01 == GPU_ISA::INDEX_BUFFER)
        {

            GPUHandle* handle = pop(ins.args);

            Services::getGraphicsDevice()->vm_bindIndexBuffer(handle);
            //auto res = std::shared_ptr<GPUHandle>(handle);
            return nullptr;

        }else if(tag_01 == GPU_ISA::UNIFORM_BUFFER)
        {
            GPUHandle* handle = pop(ins.args);
            GPU_ISA type = pop(ins.args);
            u32 slot = pop(ins.args);

            Services::getGraphicsDevice()->vm_bindUniformBuffer(handle,type,slot);
            return nullptr;
        }else if(tag_01 == GPU_ISA::PROGRAM)
        {
            GPUHandle* handle = pop(ins.args);

            Services::getGraphicsDevice()->vm_bindShader(handle);
            return nullptr;
        }else if(tag_01 == GPU_ISA::TEXTURE2D)
        {
            GPUHandle* handle = pop(ins.args);
            GPU_ISA type = pop(ins.args);
            u32 unit = pop(ins.args);

            Services::getGraphicsDevice()->vm_bindTexture2D(handle,type,unit);
            return nullptr;
        }else if(tag_01 == GPU_ISA::SAMPLER)
        {
            GPUHandle* handle = pop(ins.args);
            GPU_ISA type = pop(ins.args);
            u32 slot = pop(ins.args);

            Services::getGraphicsDevice()->vm_bindSampler(handle,type,slot);
            return nullptr;
        }else
        {
            throw ServiceException("unifentified argument for map instruction");
        }
    }else if(ins.instruction == GPU_ISA::DRAW)
    {
        Primitive type = pop(ins.args);
        GPUHandle* g_handle = pop(ins.args);
        GPUHandle* vertex = pop(ins.args);
        u32 vertices_count = pop(ins.args);
        MemoryLayout* input = pop(ins.args);

        Services::getGraphicsDevice()->vm_draw(type,g_handle,vertex,
                                                vertices_count,input);
        return nullptr;
    }else if(ins.instruction == GPU_ISA::DRAW_INDEXED)
    {
        Primitive type = pop(ins.args);
        GPUHandle* g_handle = pop(ins.args);
        GPUHandle* vertex = pop(ins.args);
        GPUHandle* index = pop(ins.args);
        u32 indices_count = pop(ins.args);
        MemoryLayout* input = pop(ins.args);

        Services::getGraphicsDevice()->vm_drawIndexed(type,g_handle,
                                                       vertex,index,indices_count,input);
        return nullptr;
    }else if(ins.instruction == GPU_ISA::APPLY)
    {
        GPU_ISA tag_01 = pop(ins.args);
        if(tag_01 == GPU_ISA::SAMPLER)
        {
            GPU_ISA tag_02 = pop(ins.args);
            if(tag_02 == GPU_ISA::TEXTURE2D)
            {
                GPUHandle* sampler = pop(ins.args);
                GPUHandle* texture = pop(ins.args);
                bool mipmaps = pop(ins.args);
                TextureAddressMode U = pop(ins.args);
                TextureAddressMode V = pop(ins.args);
                TextureFilter filter = pop(ins.args);
                CompareFunction func = pop(ins.args);
                Color* border_color = pop(ins.args);

                Services::getGraphicsDevice()->vm_applySamplerTexture2D(sampler,
                                                                         texture,mipmaps,U,V,filter,func,*border_color);
                return nullptr;
            }else
            {
                throw ServiceException("unidentfied APPLY SAMPLER instruction tag");
            }

        }else
        {
            throw ServiceException("unidentfied APPLY instruction tag");
        }
    }else
    {
        throw ServiceException("unidentfied instruction");
        return nullptr;
    }
}