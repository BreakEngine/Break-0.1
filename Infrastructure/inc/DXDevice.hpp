//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_DXDEVICE_HPP
#define BREAK_0_1_DXDEVICE_HPP

#include "Globals.hpp"

#ifdef OS_WINDOWS
#include "IGXDevice.hpp"
#include <d3d11.h>
#include <memory>
#include <glm/glm.hpp>

namespace Break{
    namespace Infrastructure{
        class BREAK_API DXDevice: public IGXDevice{
        protected:
			static void resizeWindowFunc(s32 width, s32 height);

            ///swap chain to control frame buffer
            IDXGISwapChain* m_swapChain;

            ///DX device
            ID3D11Device* m_device;

            ///DX context device
            ID3D11DeviceContext* m_deviceContext;

            ///get properties of render target view
            ID3D11RenderTargetView* m_renderTargetView;
            ///handle props of texture resources
            ID3D11Texture2D* m_depthStencilBuffer;
            ///control depth buffer
            ID3D11DepthStencilState* m_depthStencilState;
            ///getter get depth-stencil view
            ID3D11DepthStencilView* m_depthStencilView;
            ///get description for rasterizer state that you used to create raster-state object
            ID3D11RasterizerState* m_rasterState;
            D3D11_RASTERIZER_DESC m_rasterStateDesc;
            //GPU Info
            bool m_vsync;
            unsigned int m_videoCardMemory;
            char m_videoCardDescription[128];

			bool m_inited;


            //takes element type and returns element DX format
            DXGI_FORMAT getFormat(MemoryElement& element);
            D3D11_TEXTURE_ADDRESS_MODE getAddressMode(TextureAddressMode address);
            D3D11_COMPARISON_FUNC getCompareFunc(CompareFunction func);
            D3D11_FILTER getFilter(TextureFilter filter);
        public:
			DXDevice();
            ~DXDevice();

            void init(Window* window) override;

            void start(Window* window) override;

            void clearBuffer() override;

            void swapBuffer(Window* window) override;

            void setCursorPostion(int x, int y) override;

			void updateViewport(u32 width, u32 height) override;

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
        typedef std::shared_ptr<DXDevice> DXDevicePtr;
    }
}

#endif //windows if
#endif //BREAK_0_1_DXDEVICE_HPP
