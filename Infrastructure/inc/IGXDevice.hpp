//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_IGXMANAGER_HPP
#define BREAK_0_1_IGXMANAGER_HPP

#include "Globals.hpp"
#include "Window.hpp"
#include <memory>
#include "GPU_ISA.hpp"
#include "GPUHandle.hpp"
#include "MemoryLayout.hpp"
#include "ServiceException.hpp"
#include "Image.hpp"
#include "SamplerState.hpp"
#include "Primitive.hpp"

namespace Break{
    namespace Infrastructure{

        /**
         * represents a graphics device manager
         */
        class BREAK_API IGXDevice{
        protected:
            ///calls engine gameloop function
            void gameloop();
        public:

            ///virtual destructor
            virtual ~IGXDevice(){}

            /**
             * \brief initialize graphics device
             * \param window Window pointer of the current device screen
             * \author Moustapha Saad
             */
            virtual void init(Window* window) = 0;

            /**
             * \brief starts the window loop of the application
             * \param window window pointer
             * \author Moustapha Saad
             */
            virtual void start(Window* window) = 0;

            /**
             * \brief clears buffer of this graphics device
             * \author Moustapha Saad
             */
            virtual void clearBuffer()=0;

            /**
             * \brief swaps buffer of this graphics device
             * \author Moustapha Saad
             */
            virtual void swapBuffer(Window* window)=0;

            /*!
			 * \function void setCursorPosition(glm::uvec2 val)
			 *
			 * \brief set cursor position based on the library used
			 * \param val cursor position
			 *
			 * \author Moustapha Saad
			 */
            virtual void setCursorPostion(int x, int y) =0;

			virtual void updateViewport(u32 width, u32 height) = 0;

            /*!
			 * \function virtual GPUHandlePtr vm_createVertexBuffer(GPU_ISA type, u32 size, void* data)
			 *
			 * \brief creates a vertex buffer
			 * \param buffer pointer to buffer instance
			 *
			 * \author Moustapha Saad
			 */
            virtual GPUHandlePtr vm_createVertexBuffer(GPU_ISA type, u32 size, void* data){
                throw ServiceException("unimplemented function");
            };

            /*!
			 * \function virtual GPUHandlePtr vm_createIndexBuffer(GPU_ISA type, u32 size, void* data)
			 *
			 * \brief creates an index buffer
			 * \param buffer pointer to buffer instance
			 *
			 * \author Moustapha Saad
			 */
            virtual GPUHandlePtr vm_createIndexBuffer(GPU_ISA type, u32 size, void* data){
                throw ServiceException("unimplemented function");
            }

            /*!
			 * \function virtual void vm_bindVertexBuffer(GPUHandle* _handle, u32 stride)
			 *
			 * \brief binds vertex buffer
			 * \param buffer pointer to buffer instance
			 *
			 * \author Moustapha Saad
			 */
            virtual void vm_bindVertexBuffer(GPUHandle* _handle, u32 stride){
                throw ServiceException("unimplemented function");
            }

            /*!
			 * \function virtual void vm_bindIndexBuffer(GPUHandle* _handle)
			 *
			 * \brief binds index buffer
			 * \param buffer pointer to buffer instance
			 *
			 * \author Moustapha Saad
			 */
            virtual void vm_bindIndexBuffer(GPUHandle* _handle){
                throw ServiceException("unimplemented function");
            }

            /*!
             * \function virtual void vm_mapVertexBuffer(GPUHandle* handle, u32 size, void* data)
             *
             * \brief updates the vertex buffer
             * \param buffer pointer to buffer instance
             * \param offset offset inside buffer memory to start with
             * \param size size of the mapped range
             *
             * \author Moustapha Saad
             */
            virtual void vm_mapVertexBuffer(GPUHandle* handle, u32 size, void* data){
                throw ServiceException("unimplemented function");
            }

            /*!
			 * \function virtual void vm_mapIndexBuffer(GPUHandle* handle, u32 size, void* data)
			 *
			 * \brief updates the index buffer
			 * \param buffer pointer to buffer instance
			 * \param offset offset inside buffer memory to start with
			 * \param size size of the mapped range
			 *
			 * \author Moustapha Saad
			 */
            virtual void vm_mapIndexBuffer(GPUHandle* handle, u32 size, void* data){
                throw ServiceException("unimplemented function");
            }

            /*!
			 * \function virtual bool vm_deleteBuffer(GPUHandle* handle)=0;
			 *
			 * \brief destroys the vertex buffer
			 * \param buffer pointer to buffer instance
			 *
			 * \author Moustapha Saad
			 */
            virtual void vm_deleteBuffer(GPUHandle* handle){
                throw ServiceException("unimplemented function");
            }

            /*!
             * \function virtual GPUHandlePtr vm_createUniformBuffer(u32 size, void* data, u32 slot)
             *
             * \brief creates uniform buffer
             * \param buffer pointer to buffer instance
             *
             * \author Moustapha Saad
             */
            virtual GPUHandlePtr vm_createUniformBuffer(u32 size, void* data, u32 slot){
                throw ServiceException("unimplemented function");
            }

            /*!
			 * \function virtual void vm_mapUniformBuffer(GPUHandle* _handle,u32 size, void* data)
			 *
			 * \brief updates uniform buffer
			 * \param buffer pointer to buffer instance
			 * \param offset offset inside buffer memory to start with
			 * \param size size of the mapped range
			 *
			 * \author Moustapha Saad
			 */
            virtual void vm_mapUniformBuffer(GPUHandle* _handle,u32 size, void* data){
                throw ServiceException("unimplemented function");
            }

            /*!
			 * \function virtual void vm_bindUniformBuffer(GPUHandle* _handle, GPU_ISA shader, u32 slot)
			 *
			 * \brief uses uniform buffer
			 * \param buffer pointer to buffer instance
			 *
			 * \author Moustapha Saad
			 */
            virtual void vm_bindUniformBuffer(GPUHandle* _handle, GPU_ISA shader, u32 slot){
                throw ServiceException("unimplemented function");
            }

            /*!
             * \function virtual GPUHandlePtr vm_createProgram(std::string vertex,std::string pixel, MemoryLayout* inputLayout)
             * \brief creates a shader
             * \param shader pointer to shader instance
             * \author Moustapha Saad
             */
            virtual GPUHandlePtr vm_createProgram(std::string vertex,std::string pixel, MemoryLayout* inputLayout){
                throw ServiceException("unimplemented function");
            }

            /*!
             * \function virtual bool useShader(GPUResource*)
             * \brief uses a shader
             * \param shader pointer to shader instance
             * \author Moustapha Saad
             */
            virtual void vm_bindShader(GPUHandle* _handle){
                throw ServiceException("unimplemented function");
            }

            /*!
			 * \function virtual void vm_deleteShader(GPUHandle* _handle)
			 * \brief deletes a shader
			 * \param shader pointer to shader instance
			 * \author Moustapha Saad
			 */
            virtual void vm_deleteShader(GPUHandle* _handle){
                throw ServiceException("unimplemented function");
            }

            /**
             * \brief creates a texture2D object
             * \param tex pointer to texture instance
             * \param img reference to image object
             * \author Moustapha Saad
             */
            virtual GPUHandlePtr vm_createTexture2D(Image& img, bool mipmaps){
                throw ServiceException("unimplemented function");
            }

            /**
			 * \brief updates a texture2D object
			 * \param tex pointer to texture instance
			 * \param img reference to image object
			 * \author Moustapha Saad
			 */
            virtual void vm_mapTexture2D(GPUHandle* _handle, Image& img){
                throw ServiceException("unimplemented function");
            }

            /**
			 * \brief deletes a texture2D object
			 * \param tex pointer to texture instance
			 * \author Moustapha Saad
			 */
            virtual void vm_deleteTexture2D(GPUHandle* _handle){
                throw ServiceException("unimplemented function");
            }

            /**
			 * \brief binds a texture2D object
			 * \param tex pointer to texture instance
			 * \param unit sampler unit in shader
			 * \param type type of the binded to shader
			 * \author Moustapha Saad
			 */
            virtual void vm_bindTexture2D(GPUHandle* _handle, GPU_ISA type, u32 unit){
                throw ServiceException("unimplemented function");
            }

            /**
             * \brief creates a geometry object
             * \param g pointer to geometry instance
             * \author Moustapha Saad
             */
            virtual GPUHandlePtr vm_createGeometry(GPUHandle* vertex, GPUHandle* pixel, MemoryLayout* input_layout){
                throw ServiceException("unimplemented function");
            }


            /**
			 * \brief draw a geometry object, performs a draw call
			 * \param g pointer to geometry instance
			 * \param mode drawing mode, normal, instanced, indexed .. etc
			 * \author Moustapha Saad
			 */
            virtual void vm_draw(Primitive type,GPUHandle* geometry, GPUHandle* vertex_buffer, u32 vertices_count, MemoryLayout* input_layout){
                throw ServiceException("unimplemented function");
            }
            virtual void vm_drawIndexed(Primitive type,GPUHandle* geometry, GPUHandle* vertex_buffer, GPUHandle* index_buffer, u32 indices_count, MemoryLayout* input_layout){
                throw ServiceException("unimplemented function");
            }

            /**
			 * \brief deletes a geometry object
			 * \param g pointer to geometry instance
			 * \author Moustapha Saad
			 */
            virtual void vm_deleteGeometry(GPUHandle* _handle){
                throw ServiceException("unimplemented function");
            }

            /**
			 * \brief creates a sampler object
			 * \param s pointer to sampler instance
			 * \author Moustapha Saad
			 */
            virtual GPUHandlePtr vm_createSampleState(TextureAddressMode U, TextureAddressMode V,
                                                       TextureAddressMode W, TextureFilter filter,
                                                       CompareFunction func, Color* color){
                throw ServiceException("unimplemented function");
            }

            /**
			 * \brief binds a sampler object
			 * \param s pointer to sampler instance
			 * \param slot sampler unit in the shader
			 * \param type type of the binded to shader
			 * \author Moustapha Saad
			 */
            virtual void vm_bindSampler(GPUHandle* _handle, GPU_ISA shader, u32 slot){
                throw ServiceException("unimplemented function");
            }

            /**
             * \brief deletes a sampler object
             * \param s pointer to sampler instance
             * \author Moustapha Saad
             */
            virtual void vm_deleteSampler(GPUHandle* _handle){
                throw ServiceException("unimplemented function");
            }

            /**
             * \brief applies a sampler object to texture
             * \param sampler pointer to sampler instance
             * \param texture pointer to texture instance
             * \author Moustapha Saad
             */
            virtual void vm_applySamplerTexture2D(GPUHandle* sampler, GPUHandle* texture, bool mipmaps, TextureAddressMode U,
                                                  TextureAddressMode V, TextureFilter filter, CompareFunction func,
                                                  Color border_color){
                throw ServiceException("unimplemented function");
            }
        };
        typedef std::shared_ptr<IGXDevice> IGXDevicePtr;
    }
}
#endif //BREAK_0_1_IGXMANAGER_HPP
