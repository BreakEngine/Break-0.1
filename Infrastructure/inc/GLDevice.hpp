//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_GLDEVICE_HPP
#define BREAK_0_1_GLDEVICE_HPP
#include <GL/glew.h>
#include "Globals.hpp"
#include "IGXDevice.hpp"
#include <glm/glm.hpp>

struct GLFWwindow;

namespace Break{
    namespace Infrastructure{
        class BREAK_API GLDevice: public IGXDevice{
        protected:

			static void resizeWindowFunc(GLFWwindow* window, s32 width, s32 height);
            /**
			 * \brief applys filter to a texture based on our enums
			 * \param filter filter that will be applied
			 * \param mipmap true if using mipmaps
			 * \param target target of the texture that being used
			 * \author Moustapha Saad
			 */
            void applyFilter2D(TextureFilter filter,bool mipmap,GLenum target);

            /**
             * \brief converts from TextureAddressMode enum to OpenGL address mode
             * \param mode mode being applied to texture
             * \return OpenGL Flag
             * \author Moustapha Saad
             */
            int getAddressMode(TextureAddressMode mode);

            /**
             * \brief converts from enum of CompareFunction to OpenGL Flag
             * \param func function that will be used
             * \return OpenGL Flag
             * \author Moustapha Saad
             */
            int getCompareFunc(CompareFunction func);
        public:
            ~GLDevice();

            void init(Window* window) override;

            void start(Window* window) override;

            void clearBuffer() override;

            void swapBuffer(Window* window) override;

			void updateViewport(u32 width, u32 height) override;


            void setCursorPostion(int x, int y) override;

            virtual GPUHandlePtr vm_createVertexBuffer(GPU_ISA type, u32 size, void* data) override;

            virtual GPUHandlePtr vm_createIndexBuffer(GPU_ISA type, u32 size, void* data) override;

            virtual void vm_mapVertexBuffer(GPUHandle* handle, u32 size, void* data) override;

            virtual void vm_mapIndexBuffer(GPUHandle* handle, u32 size, void* data)override;

            virtual void vm_deleteBuffer(GPUHandle* handle)override;

            virtual void vm_bindVertexBuffer(GPUHandle* _handle, u32 stride)override;

            virtual void vm_bindIndexBuffer(GPUHandle* _handle) override;

            virtual GPUHandlePtr vm_createUniformBuffer(u32 size, void* data, u32 slot)override;

            virtual void vm_bindUniformBuffer(GPUHandle* _handle, GPU_ISA shader, u32 slot)override;

            virtual void vm_mapUniformBuffer(GPUHandle* _handle,u32 size, void* data)override;

            virtual GPUHandlePtr vm_createProgram(std::string vertex,std::string pixel, MemoryLayout* inputLayout)override;

            virtual void vm_bindShader(GPUHandle* _handle)override;

            virtual void vm_deleteShader(GPUHandle* _handle)override;

            virtual GPUHandlePtr vm_createTexture2D(Image& img, bool mipmaps)override;

            virtual void vm_mapTexture2D(GPUHandle* _handle, Image& img)override;

            virtual void vm_deleteTexture2D(GPUHandle* _handle)override;

            virtual void vm_bindTexture2D(GPUHandle* _handle, GPU_ISA type, u32 unit)override;

            //geometry functions
            virtual GPUHandlePtr
                    vm_createGeometry(GPUHandle* vertex, GPUHandle* pixel, MemoryLayout* input_layout)override;


            virtual void
                    vm_draw(Primitive type, GPUHandle* geometry,
                            GPUHandle* vertex_buffer, u32 vertices_count,
                            MemoryLayout* input_layout)override;

            virtual void
                    vm_drawIndexed(Primitive type, GPUHandle* geometry,
                                   GPUHandle* vertex_buffer, GPUHandle* index_buffer,
                                   u32 indices_count, MemoryLayout* input_layout)override;


            virtual void vm_deleteGeometry(GPUHandle* _handle)override;

            //sampler functions
            virtual GPUHandlePtr vm_createSampleState(TextureAddressMode U, TextureAddressMode V,
                                                      TextureAddressMode W, TextureFilter filter,
                                                      CompareFunction func, Color* color)override;

            virtual void vm_bindSampler(GPUHandle* _handle, GPU_ISA shader, u32 slot)override;

            virtual void vm_deleteSampler(GPUHandle* _handle)override;

            virtual void vm_applySamplerTexture2D(GPUHandle* sampler, GPUHandle* texture, bool mipmaps, TextureAddressMode U,
                                                  TextureAddressMode V, TextureFilter filter, CompareFunction func,
                                                  Color border_color
            ) override;
        };
    }
}
#endif //BREAK_0_1_GLDEVICE_HPP
