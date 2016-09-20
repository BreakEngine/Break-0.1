//
//
// Created by Moustapha on 01/10/2015.
//

#include "DXDevice.hpp"
#include "GPUException.hpp"
#ifdef OS_WINDOWS

#include <windows.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <Services.hpp>
#include <memory>
#include <DXShaderHandle.hpp>
#include <d3dcompiler.h>
#include <DXTexture2DHandle.hpp>
#include <DXSamplerHandle.hpp>
#include "DXBufferHandle.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

DXDevice::~DXDevice() {
    if(m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if(m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = 0;
    }

    if(m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = 0;
    }

    if(m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = 0;
    }

    if(m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = 0;
    }

    if(m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }

    if(m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = 0;
    }

    if(m_device)
    {
        m_device->Release();
        m_device = 0;
    }

    if(m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = 0;
    }
}
void DXDevice::start(Window* window){
    MSG msg;

    while(TRUE){

        if(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessageA(&msg);

            if(msg.message == WM_QUIT){
                break;
            }
        }else{

        }

        if(Services::getEngine()->getShutdown())
            break;
        //call engine gameloop
        IGXDevice::gameloop();
    }
}


void DXDevice::init(Window *window) {
    HRESULT  result;
    IDXGIFactory* factory;

    IDXGIAdapter* adapter;

    IDXGIOutput* adapterOutput;

    unsigned int numModes, i, numerator, denominator, stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_VIEWPORT viewport;
    float fieldOfView, screenAspect;

    // Store the vsync setting.
    m_vsync = window->getVSync();

    // Create a DirectX graphics interface factory.
    //to create swapchain..
    #ifdef MSVC
        result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    #else
        result = CreateDXGIFactory(IID_IDXGIFactory, (void**)&factory);
    #endif


    if(FAILED(result))
    {
        throw GPUException("Cannot Create DXGIFactory");
    }

    // Use the factory to create an adapter for the primary graphics interface (video card).
    //enumrate video adabter
    result = factory->EnumAdapters(0, &adapter);
    if(FAILED(result))
    {
        throw GPUException("Cannot get Adapters from factory object");
    }


    // Enumerate the primary adapter output (monitor).
    result = adapter->EnumOutputs(0, &adapterOutput);
    if(FAILED(result))
    {
        throw GPUException("Cannot get output from adapter object");
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if(FAILED(result))
    {
        throw GPUException("Cannot get DisplayModeList from adapter object");
    }

    // Create a list to hold all the possible display modes for this monitor, video card combination.
    displayModeList = new DXGI_MODE_DESC[numModes];

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if(FAILED(result))
    {
        throw GPUException("Cannot get DisplayModeList from adapter object");
    }

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    for(i=0; i<numModes; i++)
    {
        if(displayModeList[i].Width == (unsigned int)window->getWidth())
        {
            if(displayModeList[i].Height == (unsigned int)window->getHeight())
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // Get the adapter (video card) description.
    result = adapter->GetDesc(&adapterDesc);
    if(FAILED(result))
    {
        throw GPUException("Cannot get adapter description from adapter object");
    }

    // Store the dedicated video card memory in megabytes.
    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // Convert the name of the video card to a character array and store it.
    error = wcstombs_s((size_t*)&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if(error != 0)
    {
        throw GPUException("Cannot get video card description");
    }

    // Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;


    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = window->getWidth();
    swapChainDesc.BufferDesc.Height = window->getHeight();

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


    // Set the refresh rate of the back buffer.
    if(m_vsync)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = window->getHandle<HWND>();

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    if(window->getFullscreen())
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Set the feature level to DirectX 11.
    featureLevel = D3D_FEATURE_LEVEL_11_0;

    result = D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE, nullptr,0, nullptr,0,D3D11_SDK_VERSION,&m_device, nullptr,&m_deviceContext);
    if(FAILED(result))
    {
        throw GPUException("Cannot create Graphics Device");
    }

    result = factory->CreateSwapChain(m_device,&swapChainDesc,&m_swapChain);
    if(FAILED(result))
    {
        throw GPUException("Cannot create swap chain");
    }

    // Get the pointer to the back buffer.
    #ifdef MSVC
        result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    #else
        result = m_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (LPVOID*)&backBufferPtr);
    #endif

    if(FAILED(result))
    {
        throw GPUException("Cannot get swap chain buffer");
    }

    // Create the render target view with the back buffer pointer.
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if(FAILED(result))
    {
        throw GPUException("Cannot create RenderTargetView");
    }

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;


    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = window->getWidth();
    depthBufferDesc.Height = window->getHeight();
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;



    // Create the texture for the depth buffer using the filled out description.
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if(FAILED(result))
    {
        throw GPUException("Cannot create depth buffer");
    }

    // Initialize the description of the stencil state.
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


    // Create the depth stencil state.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if(FAILED(result))
    {
        throw GPUException("Cannot create DepthStencilState");
    }

    // Set the depth stencil state.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


    // Initailze the depth stencil view.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if(FAILED(result))
    {
        throw GPUException("Cannot create DepthStencilView");
    }

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // Setup the raster description which will determine how and what polygons will be drawn.
    m_rasterStateDesc.AntialiasedLineEnable = false;
    m_rasterStateDesc.CullMode = D3D11_CULL_BACK;
    m_rasterStateDesc.DepthBias = 0;
    m_rasterStateDesc.DepthBiasClamp = 0.0f;
    m_rasterStateDesc.DepthClipEnable = true;
    m_rasterStateDesc.FillMode = D3D11_FILL_SOLID;
    m_rasterStateDesc.FrontCounterClockwise = true;
    m_rasterStateDesc.MultisampleEnable = true;
    m_rasterStateDesc.ScissorEnable = false;
    m_rasterStateDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    result = m_device->CreateRasterizerState(&m_rasterStateDesc, &m_rasterState);
    if(FAILED(result))
    {
        throw GPUException("Cannot create RasterizerState");
    }

    // Now set the rasterizer state.
    m_deviceContext->RSSetState(m_rasterState);


    // Setup the viewport for rendering.
    viewport.Width = (float)window->getWidth();
    viewport.Height = (float)window->getHeight();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

    // Setup the projection matrix.
    fieldOfView = (float)(22/7) / 4.0f;
    screenAspect = (float)window->getWidth() / (float)window->getHeight();

    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ID3D11BlendState* blendState;
    m_device->CreateBlendState(&blendDesc,&blendState);
    m_deviceContext->OMSetBlendState(blendState,NULL,0xFFFFFF);

    // Release the display mode list.
    delete [] displayModeList;
    displayModeList = 0;

    // Release the adapter output.
    adapterOutput->Release();
    adapterOutput = 0;

    // Release the adapter.
    adapter->Release();
    adapter = 0;

    // Release the factory.
    factory->Release();
    factory = 0;
	m_inited = true;
}

DXGI_FORMAT DXDevice::getFormat(MemoryElement& element){
    switch (element.type)
    {
        case MemoryElement::BOOL:
            return DXGI_FORMAT_R32_UINT;
            break;
        case  MemoryElement::FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
            break;
        case MemoryElement::INT:
            return DXGI_FORMAT_R32_SINT;
            break;
        case MemoryElement::VEC2:
            return DXGI_FORMAT_R32G32_FLOAT;
            break;
        case MemoryElement::VEC3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case MemoryElement::VEC4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        case MemoryElement::MAT3:
            return DXGI_FORMAT_UNKNOWN;
            break;
        case MemoryElement::MAT4:
            return DXGI_FORMAT_UNKNOWN;
            break;
        default:
            break;
    }
}

D3D11_TEXTURE_ADDRESS_MODE DXDevice::getAddressMode(TextureAddressMode address)
{
    switch(address)
    {
        case TextureAddressMode::WRAP:
            return D3D11_TEXTURE_ADDRESS_WRAP;
            break;
        case TextureAddressMode::CLAMP:
            return D3D11_TEXTURE_ADDRESS_CLAMP;
            break;
        case TextureAddressMode::MIRROR:
            return D3D11_TEXTURE_ADDRESS_MIRROR;
            break;
        case TextureAddressMode::BORDER:
            return D3D11_TEXTURE_ADDRESS_BORDER;
            break;
        default: break;
    }
}

D3D11_COMPARISON_FUNC DXDevice::getCompareFunc(CompareFunction func)
{
    switch (func)
    {
        case CompareFunction::ALWAYS:
            return D3D11_COMPARISON_ALWAYS;
            break;
        case CompareFunction::NEVER:
            return D3D11_COMPARISON_NEVER;
            break;
        case CompareFunction::LESS:
            return D3D11_COMPARISON_LESS;
            break;
        case CompareFunction::LESS_EQUAL:
            return D3D11_COMPARISON_LESS_EQUAL;
            break;
        case CompareFunction::EQUAL:
            return D3D11_COMPARISON_EQUAL;
            break;
        case CompareFunction::GREATER_EQUAL:
            return D3D11_COMPARISON_GREATER_EQUAL;
            break;
        case CompareFunction::GREATER:
            return D3D11_COMPARISON_GREATER;
            break;
        case CompareFunction::NOT_EQUAL:
            return D3D11_COMPARISON_NOT_EQUAL;
            break;
        default: break;
    }
}

D3D11_FILTER DXDevice::getFilter(TextureFilter filter)
{
    switch(filter)
    {
        case TextureFilter::LINEAR:
            return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case TextureFilter::POINT:
            return D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
        case TextureFilter::ANISOTROPIC:
            return D3D11_FILTER_ANISOTROPIC;
            break;
        case TextureFilter::LINEAR_MIP_POINT:
            return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case TextureFilter::POINT_MIP_LINEAR:
            return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            break;
        case TextureFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            break;
        case TextureFilter::MIN_LINEAR_MAG_POINT_MIP_POINT:
            return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            break;
        case TextureFilter::MIN_POINT_MAG_LINEAR_MIP_LINEAR:
            return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            break;
        case TextureFilter::MIN_POINT_MAG_LINEAR_MIP_POINT:
            return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            break;
        default:
            return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
    }
}

DXDevice::DXDevice()
{
	m_inited = false;
}

void DXDevice::clearBuffer() {
    float c[4] = {0,0,0,0};
    m_deviceContext->ClearRenderTargetView(m_renderTargetView,c);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView,D3D11_CLEAR_DEPTH,1.0f,0);
}

void DXDevice::swapBuffer(Window* window) {
    if(m_vsync)
        m_swapChain->Present(1,0);
    else
        m_swapChain->Present(0,0);
}

void DXDevice::setCursorPostion(int x, int y){
    Window* win = Services::getEngine()->getApplication()->getWindow();
    HWND hnd = win->getHandle<HWND>();
    POINT pt;
    pt.x = x;
    pt.y = y;
    if(hnd){
        ClientToScreen(hnd,&pt);
        SetCursorPos(pt.x,pt.y);
    }
}

void DXDevice::updateViewport(u32 width, u32 height)
{
	if(m_inited){
	
		m_deviceContext->OMSetRenderTargets(0,0,0);

		m_renderTargetView->Release();

		HRESULT hr;

		hr = m_swapChain->ResizeBuffers(0,0,0, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D* m_buffer;
	
		hr = m_swapChain->GetBuffer(0,__uuidof(ID3D11Texture2D), (void**)&m_buffer);

		m_device->CreateRenderTargetView(m_buffer,NULL,&m_renderTargetView);

		m_buffer->Release();

		m_deviceContext->OMSetRenderTargets(1,&m_renderTargetView,NULL);

		D3D11_VIEWPORT vp;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		m_deviceContext->RSSetViewports(1,&vp);
	}
}

GPUHandlePtr DXDevice::vm_createVertexBuffer(GPU_ISA type, u32 size, void *data) {
    auto handle = make_shared<DXBufferHandle>();

    CD3D11_BUFFER_DESC bufferDESC;
    bufferDESC.ByteWidth = size;
    bufferDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDESC.MiscFlags = 0;

    switch (type)
    {
        //dynamic
        case GPU_ISA::DYNAMIC:
            bufferDESC.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bufferDESC.Usage = D3D11_USAGE_DYNAMIC;
            break;
            //static
        case GPU_ISA::STATIC:
            bufferDESC.CPUAccessFlags = 0;
            bufferDESC.Usage = D3D11_USAGE_IMMUTABLE;
            break;
        default:
            return nullptr;
            break;
    }

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = data;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    HRESULT res = m_device->CreateBuffer(&bufferDESC,&initData,&handle->DXBuffer);


    if(FAILED(res))
        return nullptr;
    else
        return handle;
}

GPUHandlePtr DXDevice::vm_createIndexBuffer(GPU_ISA type, u32 size, void *data) {
    auto handle = make_shared<DXBufferHandle>();

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.MiscFlags = 0;

    switch (type)
    {
        //dynamic
        case GPU_ISA::DYNAMIC:
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            break;
            //static
        case GPU_ISA::STATIC:
            bufferDesc.CPUAccessFlags = 0;
            bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
            break;
        default:
            break;
    }

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = data;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    HRESULT res = m_device->CreateBuffer(&bufferDesc,&initData,&handle->DXBuffer);

    if(FAILED(res))
        return nullptr;
    else
        return handle;
}

void DXDevice::vm_mapVertexBuffer(GPUHandle *_handle, u32 size, void *data) {
    DXBufferHandle* handle = dynamic_cast<DXBufferHandle*>(_handle);

    D3D11_MAPPED_SUBRESOURCE mappedData;
    m_deviceContext->Map(handle->DXBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

    //memcpy(mappedData.pData,VBuffer->getData(offset),size);
    auto cpySize = size;
    memcpy(mappedData.pData,data,cpySize);

    m_deviceContext->Unmap(handle->DXBuffer,0);
}

void DXDevice::vm_mapIndexBuffer(GPUHandle *_handle, u32 size, void *data) {
    DXBufferHandle* handle = dynamic_cast<DXBufferHandle*>(_handle);

    D3D11_MAPPED_SUBRESOURCE mappedData;
    m_deviceContext->Map(handle->DXBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

    //memcpy(mappedData.pData,IBuffer->getData(offset),size);
    auto cpySize = size;
    memcpy(mappedData.pData,data,cpySize);

    m_deviceContext->Unmap(handle->DXBuffer,0);
}

void DXDevice::vm_deleteBuffer(GPUHandle *_handle) {
    auto handle = dynamic_cast<DXBufferHandle*>(_handle);

    handle->DXBuffer->Release();
}

void DXDevice::vm_bindVertexBuffer(GPUHandle *_handle, u32 _stride) {
    auto handle = dynamic_cast<DXBufferHandle*>(_handle);

    unsigned int stride = _stride;
    unsigned int offset = 0;
    m_deviceContext->IASetVertexBuffers(0,1,&handle->DXBuffer,&stride,&offset);
}

void DXDevice::vm_bindIndexBuffer(GPUHandle *_handle) {
    auto handle = dynamic_cast<DXBufferHandle*>(_handle);
    m_deviceContext->IASetIndexBuffer(handle->DXBuffer,DXGI_FORMAT_R32_UINT,0);
}

GPUHandlePtr DXDevice::vm_createProgram(std::string vertex, std::string pixel, MemoryLayout *inputLayout) {
    auto handle = make_shared<DXShaderHandle>();

    HRESULT res;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC* layout;
    unsigned int nElements;

    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    res = D3DCompile(vertex.c_str(),vertex.size(),"VS",NULL,NULL,"main",
                     "vs_4_0",D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY,NULL,&vertexShaderBuffer,&errorMessage);

    if(FAILED(res)){

        throw ServiceException((char*)errorMessage->GetBufferPointer());
    }

    res = D3DCompile(pixel.c_str(),pixel.size(),"PS",NULL,NULL,"main",
                     "ps_4_0",D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY,NULL,&pixelShaderBuffer,&errorMessage);

    if(FAILED(res)){
        throw ServiceException((char*)errorMessage->GetBufferPointer());
    }

    res = m_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),vertexShaderBuffer->GetBufferSize(),
                                      NULL, &handle->vertexShader);

    if(FAILED(res)){
        throw ServiceException("Cannot generate vertex shader");
    }

    res = m_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),pixelShaderBuffer->GetBufferSize(),NULL,&handle->pixelShader);
    if(FAILED(res)){
        throw ServiceException("Cannot generate pixel shader");
    }

    layout = new D3D11_INPUT_ELEMENT_DESC[inputLayout->getElementCount()];

    for(int i=0;i<inputLayout->getElementCount();i++){
        layout[i].SemanticName = inputLayout->elements[i].semantic.c_str();
        layout[i].SemanticIndex = 0;
        layout[i].Format = getFormat(inputLayout->elements[i]);
        layout[i].InputSlot = 0;
        layout[i].AlignedByteOffset = inputLayout->elements[i].offset;
        layout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[i].InstanceDataStepRate = 0;
    }

    res = m_device->CreateInputLayout(layout,inputLayout->getElementCount(),vertexShaderBuffer->GetBufferPointer(),vertexShaderBuffer->GetBufferSize(),&handle->inputLayout);

    if(FAILED(res)){
        throw ServiceException("Cannot create input layout");
    }

    vertexShaderBuffer->Release();
    vertexShaderBuffer =0;
    pixelShaderBuffer->Release();
    pixelShaderBuffer=0;

    return handle;
}

void DXDevice::vm_bindShader(GPUHandle *_handle) {
    auto handle = dynamic_cast<DXShaderHandle*>(_handle);

    m_deviceContext->IASetInputLayout(handle->inputLayout);
    m_deviceContext->VSSetShader(handle->vertexShader,NULL,0);
    m_deviceContext->PSSetShader(handle->pixelShader,NULL,0);
}

void DXDevice::vm_deleteShader(GPUHandle *_handle) {
    auto handle = dynamic_cast<DXShaderHandle*>(_handle);

    if(handle->inputLayout)
    {
        handle->inputLayout->Release();
        handle->inputLayout = 0;
    }
    if(handle->vertexShader)
    {
        handle->vertexShader->Release();
        handle->vertexShader = 0;
    }
    if(handle->pixelShader)
    {
        handle->pixelShader->Release();
        handle->pixelShader = 0;
    }
}

GPUHandlePtr DXDevice::vm_createUniformBuffer(u32 size, void *data, u32 slot) {
    auto handle = make_shared<DXBufferHandle>();

    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = size;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;


    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = data;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    auto res = m_device->CreateBuffer(&desc,&initData,&handle->DXBuffer);

    if(FAILED(res))
        return nullptr;
    else
        return handle;
}

void DXDevice::vm_mapUniformBuffer(GPUHandle *_handle, u32 size, void *data) {
    DXBufferHandle* handle = dynamic_cast<DXBufferHandle*>(_handle);

    D3D11_MAPPED_SUBRESOURCE mappedData;
    m_deviceContext->Map(handle->DXBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

    if(!mappedData.pData)
        throw ServiceException("Cannot map uniform buffer");
    //memcpy(mappedData.pData,UBuffer->getData(offset),size);
    memcpy(mappedData.pData,data,size);

    m_deviceContext->Unmap(handle->DXBuffer,0);
}

void DXDevice::vm_bindUniformBuffer(GPUHandle *_handle, GPU_ISA shader, u32 slot) {
    auto handle = dynamic_cast<DXBufferHandle*>(_handle);

    if(shader == GPU_ISA::VERTEX_SHADER){
        m_deviceContext->VSSetConstantBuffers(slot,1,&handle->DXBuffer);
    }else if(shader == GPU_ISA::PIXEL_SHADER){
        m_deviceContext->PSSetConstantBuffers(slot,1,&handle->DXBuffer);
    }else
        throw ServiceException("Cannot bind uniform buffer to shader");
}

GPUHandlePtr DXDevice::vm_createTexture2D(Image &img, bool mipmaps) {
    auto handle = make_shared<DXTexture2DHandle>();

    D3D11_TEXTURE2D_DESC desc;
    desc.ArraySize = 1;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MipLevels = 1;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.Width = img.getWidth();
    desc.Height = img.getHeight();
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    D3D11_SUBRESOURCE_DATA descData;
    //ZeroMemory(&descData,sizeof(D3D11_SUBRESOURCE_DATA));
    descData.pSysMem = img.getPixels();
    descData.SysMemPitch = img.getWidth()*sizeof(Pixel);
    descData.SysMemSlicePitch = img.getSize();
    auto res = m_device->CreateTexture2D(&desc,&descData,&handle->texture);
    if(FAILED(res)){
        throw ServiceException("Cannot create texture2D");
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    res = m_device->CreateShaderResourceView(handle->texture,&srvDesc,&handle->resourceView);
    if(FAILED(res)){
        throw ServiceException("Cannot create shader resource from texture2D");
    }
    if(mipmaps)
        m_deviceContext->GenerateMips(handle->resourceView);

    return handle;
}

void DXDevice::vm_mapTexture2D(GPUHandle *_handle, Image &img) {
    auto handle = dynamic_cast<DXTexture2DHandle*>(_handle);

    D3D11_MAPPED_SUBRESOURCE mappedData;
    m_deviceContext->Map(handle->texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

    if(!mappedData.pData)
        throw ServiceException("Cannot map texture2D");
	//mappedData.RowPitch = img.getWidth()*sizeof(Pixel);
	//mappedData.DepthPitch = img.getWidth()*sizeof(Pixel)*img.getHeight();
    //memcpy(mappedData.pData,UBuffer->getData(offset),size);
	auto row_size = img.getWidth()*sizeof(Pixel);
	byte* write_ptr = static_cast<byte*>(mappedData.pData);
	byte* read_ptr = reinterpret_cast<byte*>(img.getPixels());
	for(int i=0;i<img.getHeight();i++)
	{
		memcpy(write_ptr,read_ptr,row_size);
		read_ptr += row_size;
		write_ptr += mappedData.RowPitch;
	}
    //memcpy(mappedData.pData,img.getPixels(),img.getSize());

    m_deviceContext->Unmap(handle->texture,0);
}

void DXDevice::vm_deleteTexture2D(GPUHandle *_handle) {
    auto handle = dynamic_cast<DXTexture2DHandle*>(_handle);

    if(handle->texture)
    {
        handle->texture->Release();
        handle->texture = 0;
    }

    if(handle->resourceView)
    {
        handle->resourceView->Release();
        handle->resourceView = 0;
    }
}

void DXDevice::vm_bindTexture2D(GPUHandle *_handle, GPU_ISA type, u32 unit) {
    auto handle = dynamic_cast<DXTexture2DHandle*>(_handle);

    switch(type)
    {
        case GPU_ISA::VERTEX_SHADER:
            m_deviceContext->VSSetShaderResources(unit,1,&handle->resourceView);
            break;
        case GPU_ISA::PIXEL_SHADER:
            m_deviceContext->PSSetShaderResources(unit,1,&handle->resourceView);
            break;
        default:
            throw ServiceException("unidentified shader type to bind texture to it");
            break;
    }
}

GPUHandlePtr DXDevice::vm_createGeometry(GPUHandle *vertex, GPUHandle *pixel, MemoryLayout *input_layout) {
    return make_shared<GPUHandle>();
}

void DXDevice::vm_draw(Primitive type, GPUHandle *geometry, GPUHandle *vertex_buffer, u32 vertices_count,
                       MemoryLayout *input_layout) {

    if((u8)type == 0)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    if((u8)type == 1)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    if((u8)type == 2)
        m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

    if((u8)type == 3)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    if((u8)type == 4)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if((u8)type == 5)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    if((u8)type == 6)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ);

    vm_bindVertexBuffer(vertex_buffer,input_layout->getSize());

    m_deviceContext->Draw(vertices_count,0);
}


void DXDevice::vm_drawIndexed(Primitive type, GPUHandle *geometry, GPUHandle *vertex_buffer, GPUHandle *index_buffer,
                              u32 indices_count, MemoryLayout *input_layout) {
    if((u8)type == 0)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    if((u8)type == 1)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    if((u8)type == 2)
        m_deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

    if((u8)type == 3)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    if((u8)type == 4)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if((u8)type == 5)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    if((u8)type == 6)
        m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ);




    vm_bindVertexBuffer(vertex_buffer,input_layout->getSize());
    vm_bindIndexBuffer(index_buffer);
    m_deviceContext->DrawIndexed(indices_count,0,0);
}

void DXDevice::vm_deleteGeometry(GPUHandle *_handle) {
    return;
}

GPUHandlePtr DXDevice::vm_createSampleState(TextureAddressMode U, TextureAddressMode V, TextureAddressMode W,
                                            TextureFilter filter, CompareFunction func, Color *color) {
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.AddressU = getAddressMode(U);
    samplerDesc.AddressV = getAddressMode(V);
    samplerDesc.AddressW = getAddressMode(W);
    samplerDesc.ComparisonFunc = getCompareFunc(func);
    samplerDesc.Filter = getFilter(filter);
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.BorderColor[0] = color->R;
    samplerDesc.BorderColor[1] = color->G;
    samplerDesc.BorderColor[2] = color->B;
    samplerDesc.BorderColor[3] = color->A;
    //samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    auto handle = make_shared<DXSamplerHandle>();

    auto result = m_device->CreateSamplerState(&samplerDesc,&handle->sampler);
    if(FAILED(result))
        throw ServiceException("Cannot create sampler");

    return handle;
}

void DXDevice::vm_bindSampler(GPUHandle *_handle, GPU_ISA shader, u32 slot) {
    auto handle = dynamic_cast<DXSamplerHandle*>(_handle);

    if(shader == GPU_ISA::VERTEX_SHADER)
        m_deviceContext->VSSetSamplers(slot,1,&handle->sampler);
    else if(shader == GPU_ISA::PIXEL_SHADER)
        m_deviceContext->PSSetSamplers(slot,1,&handle->sampler);
}

void DXDevice::vm_applySamplerTexture2D(GPUHandle *sampler, GPUHandle *texture, bool mipmaps, TextureAddressMode U,
                                        TextureAddressMode V, TextureFilter filter, CompareFunction func,
                                        Color border_color) {
    return;
}

void DXDevice::vm_deleteSampler(GPUHandle *_handle) {
    auto handle = dynamic_cast<DXSamplerHandle*>(_handle);
    handle->sampler->Release();
    handle->sampler = 0;
}
#endif
