//
// Created by Moustapha on 01/10/2015.
//
#include "GLDevice.hpp"
#include <GLFW/glfw3.h>
#include <ServiceException.hpp>
#include <Services.hpp>
#include <GLShaderHandle.hpp>
#include "GLMouse.hpp"
#include "GLKeyboard.hpp"
#include "GLHandle.hpp"

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

GLDevice::~GLDevice(){

}

void GLDevice::init(Window* my_window){
	m_clearColor = Color(0, 0, 0, 0);
    GLFWwindow* window;
    if( !glfwInit() )
    {
        throw ServiceException("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //OpenGL version 3.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //If requesting an OpenGL version below 3.2, GLFW_OPENGL_ANY_PROFILE


    // Open a window and create its OpenGL context
    window = glfwCreateWindow(my_window->getWidth(), my_window->getHeight(),my_window->getTitle().c_str() , NULL, NULL);
    //WindowPtr win = Services::getPlatform()->createWindow(my_window->getWidth(), my_window->getHeight(),my_window->getTitle());
    //glfwSetWindowUserPointer(window, win->getHandle<void*>());
    if( window == nullptr ){
        glfwTerminate();
        throw ServiceException("Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        throw ServiceException("Failed to initialize GLEW");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH);
    glEnable(GL_RGBA);
    glEnable(GL_DOUBLE);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(5);
    glClearColor(0,0,0,0);
    //glViewport(0,0,my_window->getWidth(),my_window->getHeight());

    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    my_window->setHandle<GLFWwindow*>(window);
}

void GLDevice::start(Window* window){

    glfwSetKeyCallback(window->getHandle<GLFWwindow*>(),&GLKeyboard::keyboardFunc);
    glfwSetMouseButtonCallback(window->getHandle<GLFWwindow*>(),&GLMouse::mouseFunc);
    glfwSetCursorPosCallback(window->getHandle<GLFWwindow*>(),&GLMouse::mouseMotion);
	glfwSetWindowSizeCallback(window->getHandle<GLFWwindow*>(),&GLDevice::resizeWindowFunc);

    while(!glfwWindowShouldClose(window->getHandle<GLFWwindow*>())){
        IGXDevice::gameloop();
        //check if the engine is shutting down
        if(Services::getEngine()->getShutdown())
            break;
        glfwPollEvents();
        //glfwWaitEvents();
    }
}

void GLDevice::setClearColor(Infrastructure::Color color) {
	m_clearColor = color;
	glm::vec4 c = color;
	glClearColor(c.r, c.g, c.b, c.a);
}

void GLDevice::clearBuffer(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLDevice::swapBuffer(Window* window){
    glfwSwapBuffers(window->getHandle<GLFWwindow*>());
}

void GLDevice::updateViewport(u32 width, u32 height)
{
	glViewport(0,0,width,height);
}

void GLDevice::setCursorPostion(int x, int y){
    Window* win = Services::getEngine()->getApplication()->getWindow();
    glfwSetCursorPos(win->getHandle<GLFWwindow*>(),(double)x,(double)y);
}

void GLDevice::resizeWindowFunc(GLFWwindow* window, s32 width, s32 height)
{
	Window* win = Services::getEngine()->getApplication()->getWindow();
	Services::getGraphicsDevice()->updateViewport(width,height);
	win->setWidth((u32)width);
	win->setHeight((u32)height);
}

void GLDevice::applyFilter2D(TextureFilter filter, bool mipmap,GLenum target)
{
    switch (filter)
    {
        case TextureFilter::POINT:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            break;
        case TextureFilter::LINEAR:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            break;
        case TextureFilter::ANISOTROPIC:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            break;
        case TextureFilter::LINEAR_MIP_POINT:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            break;
        case TextureFilter::POINT_MIP_LINEAR:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            break;
        case TextureFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            break;
        case TextureFilter::MIN_LINEAR_MAG_POINT_MIP_POINT:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            break;
        case TextureFilter::MIN_POINT_MAG_LINEAR_MIP_LINEAR:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            break;
        case TextureFilter::MIN_POINT_MAG_LINEAR_MIP_POINT:
            if(mipmap)
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_NEAREST);
            else
                glTexParameteri(target,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(target,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            break;
        default: break;
    }
}

int GLDevice::getAddressMode(TextureAddressMode mode)
{
    switch (mode)
    {
        case TextureAddressMode::WRAP:
            return GL_REPEAT;
            break;
        case TextureAddressMode::CLAMP:
            return GL_CLAMP_TO_EDGE;
            break;
        case TextureAddressMode::MIRROR:
            return GL_MIRRORED_REPEAT;
            break;
        case TextureAddressMode::BORDER:
            return GL_CLAMP_TO_BORDER;
            break;
        default:
            return -1;
            break;
    }
}

int GLDevice::getCompareFunc(CompareFunction func)
{
    switch (func)
    {
        case CompareFunction::ALWAYS:
            return GL_ALWAYS;
            break;
        case CompareFunction::NEVER:
            return GL_NEVER;
            break;
        case CompareFunction::LESS:
            return GL_LESS;
            break;
        case CompareFunction::LESS_EQUAL:
            return GL_LEQUAL;
            break;
        case CompareFunction::EQUAL:
            return GL_EQUAL;
            break;
        case CompareFunction::GREATER_EQUAL:
            return GL_GEQUAL;
            break;
        case CompareFunction::GREATER:
            return GL_GREATER;
            break;
        case CompareFunction::NOT_EQUAL:
            return GL_NOTEQUAL;
            break;
        default: break;
    }
}

GPUHandlePtr GLDevice::vm_createVertexBuffer(GPU_ISA type, u32 size, void* data)
{

    auto handle = make_shared<GLHandle>();

    GLuint id = -1;
    glGenBuffers(1,&id);
    if (id == -1)
        throw ServiceException("Cannot generate vertex buffer");
    glBindBuffer(GL_ARRAY_BUFFER,id);


    switch (type)
    {

        //static
        case GPU_ISA::STATIC:
            glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
            break;

            //dynamic
        case GPU_ISA::DYNAMIC:
            glBufferData(GL_ARRAY_BUFFER,size,data,GL_DYNAMIC_DRAW);
            break;

        default:
            return nullptr;
            break;
    }

    handle->ID = id;

    glBindBuffer(GL_ARRAY_BUFFER,0);
    return handle;
}

GPUHandlePtr GLDevice::vm_createIndexBuffer(GPU_ISA type, u32 size, void* data)
{

    auto handle = make_shared<GLHandle>();

    GLuint id=-1;
    glGenBuffers(1,&id);
    if (id == -1)
        throw ServiceException("Cannot generate index buffer");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,id);

    switch (type)
    {

        //static
        case GPU_ISA::STATIC:
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
            break;
            //dynamic
        case GPU_ISA::DYNAMIC:
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_DYNAMIC_DRAW);
            break;

        default:
            return nullptr;
            break;
    }

    handle->ID = id;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    return handle;
}

void GLDevice::vm_mapVertexBuffer(GPUHandle* _handle, u32 size, void* data)
{

    auto handle = dynamic_cast<GLHandle*>(_handle);

    glBindBuffer(GL_ARRAY_BUFFER,handle->ID);
    //glBufferData(GL_ARRAY_BUFFER,VBuffer->getSize(),NULL,GL_DYNAMIC_DRAW);
    void* GPUPtr = NULL;
    GPUPtr = glMapBufferRange(GL_ARRAY_BUFFER,0,size,GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_RANGE_BIT );

    if(GPUPtr == NULL)
        throw ServiceException("Cannot Map Vertex Buffer: Failed to get buffer pointer");

    memcpy(GPUPtr,data,size);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void GLDevice::vm_mapIndexBuffer(GPUHandle* _handle, u32 size, void* data)
{

    auto handle = dynamic_cast<GLHandle*>(_handle);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,handle->ID);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,IBuffer->getSize(),NULL,GL_DYNAMIC_DRAW);
    void* GPUPtr = NULL;
    GPUPtr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0,size,GL_MAP_WRITE_BIT |GL_MAP_INVALIDATE_RANGE_BIT );

    if(GPUPtr == NULL)
        throw ServiceException("Cannot Map Index Buffer: Failed to get buffer pointer");

    memcpy(GPUPtr,data,size);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void GLDevice::vm_deleteBuffer(GPUHandle* _handle)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);
    glDeleteBuffers(1,&handle->ID);
}

void GLDevice::vm_bindVertexBuffer(GPUHandle* _handle, u32 stride)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);

    glBindBuffer(GL_ARRAY_BUFFER,handle->ID);
}

void GLDevice::vm_bindIndexBuffer(GPUHandle* _handle)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,handle->ID);
}

GPUHandlePtr GLDevice::vm_createProgram(std::string vertex, std::string pixel, MemoryLayout* inputLayout)
{

    auto handle = make_shared<GLShaderHandle>();

    handle->vertexShader = glCreateShader(GL_VERTEX_SHADER);

    char* error = new char[1024];
    const GLchar* p[1];
    p[0] = vertex.c_str();
    GLint lengths[1];
    lengths[0] = vertex.size();
    glShaderSource(handle->vertexShader,1,p,lengths);
    glCompileShader(handle->vertexShader);

    GLint res = 0;
    glGetShaderiv(handle->vertexShader,GL_COMPILE_STATUS,&res);

    if(res == GL_FALSE){
        glGetShaderInfoLog(handle->vertexShader,1024,NULL,error);
        throw ServiceException("Cannot compile vertex shader");
    }

    handle->pixelShader = glCreateShader(GL_FRAGMENT_SHADER);

    p[0] = pixel.c_str();
    lengths[0] = pixel.size();

    glShaderSource(handle->pixelShader,1,p,lengths);
    glCompileShader(handle->pixelShader);

    glGetShaderiv(handle->pixelShader,GL_COMPILE_STATUS,&res);

    if(res == GL_FALSE){
        glGetShaderInfoLog(handle->pixelShader,1024,NULL,error);
        throw ServiceException("Cannot compile pixel shader");
    }

    handle->program = glCreateProgram();

    glAttachShader(handle->program,handle->vertexShader);
    glAttachShader(handle->program,handle->pixelShader);

    glLinkProgram(handle->program);
    glGetProgramiv(handle->program,GL_LINK_STATUS,&res);

    if(res == GL_FALSE){
        glGetProgramInfoLog(handle->program,1024,NULL,error);
        throw ServiceException("Cannot link GPU program");
    }
    glValidateProgram(handle->program);
    glGetProgramiv(handle->program,GL_VALIDATE_STATUS,&res);

    if(res == GL_FALSE){
        glGetProgramInfoLog(handle->program,1024,NULL,error);
        throw ServiceException("GPU program validation error");
    }

    delete[] error;
    return handle;
}

void GLDevice::vm_deleteShader(GPUHandle* _handle)
{
    auto handle = dynamic_cast<GLShaderHandle*>(_handle);

    if(handle->vertexShader)
        glDeleteShader(handle->vertexShader);
    if(handle->pixelShader)
        glDeleteShader(handle->pixelShader);
    if(handle->program)
        glDeleteProgram(handle->program);
}

void GLDevice::vm_bindShader(GPUHandle* _handle)
{
    auto handle = dynamic_cast<GLShaderHandle*>(_handle);

    glUseProgram(handle->program);
}

GPUHandlePtr GLDevice::vm_createUniformBuffer(u32 size, void* data, u32 slot)
{

    auto handle = make_shared<GLHandle>();

    glGenBuffers(1,&handle->ID);
    glBindBuffer(GL_UNIFORM_BUFFER,handle->ID);
    glBufferData(GL_UNIFORM_BUFFER,size,data,GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER,slot,handle->ID);

    glBindBuffer(GL_UNIFORM_BUFFER,0);

    return handle;
}

void GLDevice::vm_mapUniformBuffer(GPUHandle* _handle, u32 size, void* data)
{

    auto handle = dynamic_cast<GLHandle*>(_handle);

    glBindBuffer(GL_UNIFORM_BUFFER,handle->ID);
    //glBufferData(GL_UNIFORM_BUFFER,UBuffer->getSize(),NULL,GL_DYNAMIC_DRAW);
    void* GPUPtr = NULL;
    GPUPtr = glMapBufferRange(GL_UNIFORM_BUFFER,0,size,GL_MAP_WRITE_BIT |GL_MAP_INVALIDATE_RANGE_BIT );

    if(GPUPtr == NULL)
        throw ServiceException("Cannot Map Uniform Buffer: Failed to get buffer pointer");

    memcpy(GPUPtr,data,size);
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindBuffer(GL_UNIFORM_BUFFER,0);
}

void GLDevice::vm_bindUniformBuffer(GPUHandle* _handle, GPU_ISA shader, u32 slot)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);
    glBindBuffer(GL_UNIFORM_BUFFER,handle->ID);
    glBindBufferBase(GL_UNIFORM_BUFFER,slot,handle->ID);
}

GPUHandlePtr GLDevice::vm_createTexture2D(Image& img, bool mipmaps)
{
    auto handle = make_shared<GLHandle>();

    glGenTextures(1,&handle->ID);
    glBindTexture(GL_TEXTURE_2D,handle->ID);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img.getWidth(),img.getHeight(),0,GL_BGRA,GL_UNSIGNED_BYTE,img.getPixels());
    if(mipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D,0);
    return handle;
}

void GLDevice::vm_mapTexture2D(GPUHandle* _handle, Image& img)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);

    glBindTexture(GL_TEXTURE_2D,handle->ID);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img.getWidth(),img.getHeight(),0,GL_BGRA,GL_UNSIGNED_BYTE,img.getPixels());
    glBindTexture(GL_TEXTURE_2D,0);

}

void GLDevice::vm_deleteTexture2D(GPUHandle* _handle)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);

    glDeleteTextures(1,&handle->ID);
}

void GLDevice::vm_bindTexture2D(GPUHandle* _handle, GPU_ISA type, u32 unit)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);

    glActiveTexture(GL_TEXTURE0+unit);
    glBindTexture(GL_TEXTURE_2D,handle->ID);
}

GPUHandlePtr GLDevice::vm_createGeometry(GPUHandle* vertex, GPUHandle* index, MemoryLayout* input_layout)
{

    auto handle = make_shared<GLHandle>();

    glGenVertexArrays(1,&handle->ID);
    glBindVertexArray(handle->ID);

    //geo->_geometryData.vertices->use();
    vm_bindVertexBuffer(vertex,input_layout->getSize());

    MemoryLayout layout = *input_layout;

    for(int i=0;i<layout.getElementCount();i++)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,layout.elements[i].components,GL_FLOAT,GL_FALSE,layout.getSize(),reinterpret_cast<void*>(layout.elements[i].offset));
    }
    if(index)
        vm_bindIndexBuffer(index);

    glBindVertexArray(0);


    return handle;
}

void GLDevice::vm_draw(Primitive type,GPUHandle* geometry_handle,
                        GPUHandle* vertex_buffer, u32 vertices_count,
                        MemoryLayout* input_layout)
{

    auto handle = dynamic_cast<GLHandle*>(geometry_handle);

    glBindVertexArray(handle->ID);

    vm_bindVertexBuffer(vertex_buffer,input_layout->getSize());
    if((u8)type == 0)
        glDrawArrays(GL_POINTS,0,vertices_count);
    else if((u8)type == 1)
        glDrawArrays(GL_LINES,0,vertices_count);
    else if((u8)type == 2)
        glDrawArrays(GL_LINE_STRIP,0,vertices_count);
    else if((u8)type == 3)
        glDrawArrays(GL_LINE_LOOP,0,vertices_count);
    else if((u8)type == 4)
        glDrawArrays(GL_TRIANGLES,0,vertices_count);
    else if((u8)type == 5)
        glDrawArrays(GL_TRIANGLE_STRIP,0,vertices_count);
    else if((u8)type == 6)
        glDrawArrays(GL_TRIANGLE_FAN,0,vertices_count);
    else
        throw ServiceException("Cannot identify primitive type");

}

void GLDevice::vm_drawIndexed(Primitive type, GPUHandle* geometry, GPUHandle* vertex_buffer, GPUHandle* index_buffer, u32 indices_count, MemoryLayout* input_layout)
{

    auto handle = dynamic_cast<GLHandle*>(geometry);

    glBindVertexArray(handle->ID);

    vm_bindVertexBuffer(vertex_buffer,input_layout->getSize());

    vm_bindIndexBuffer(index_buffer);
    if((u8)type == 0)
        glDrawElements(GL_POINTS , indices_count , GL_UNSIGNED_INT ,(void*)0);
    else if((u8)type == 1)
        glDrawElements(GL_LINES , indices_count , GL_UNSIGNED_INT ,(void*)0);

    else if((u8)type == 2)
        glDrawElements(GL_LINE_STRIP , indices_count , GL_UNSIGNED_INT ,(void*)0);

    else if((u8)type == 3)
        glDrawElements(GL_LINE_LOOP , indices_count , GL_UNSIGNED_INT ,(void*)0);

    else if((u8)type == 4)
        glDrawElements(GL_TRIANGLES , indices_count , GL_UNSIGNED_INT ,(void*)0);

    else if((u8)type == 5)
        glDrawElements(GL_TRIANGLE_STRIP , indices_count , GL_UNSIGNED_INT ,(void*)0);

    else if((u8)type == 6)
        glDrawElements(GL_TRIANGLE_FAN , indices_count , GL_UNSIGNED_INT ,(void*)0);
    else
        throw ServiceException("Cannot identify primitive type");
}

void GLDevice::vm_deleteGeometry(GPUHandle* _handle)
{
    auto handle = dynamic_cast<GLHandle*>(_handle);

    glDeleteVertexArrays(1,&handle->ID);
}

GPUHandlePtr GLDevice::vm_createSampleState(TextureAddressMode U, TextureAddressMode V, TextureAddressMode W, TextureFilter filter, CompareFunction func, Color* color)
{
    return make_shared<GLHandle>();
}

void GLDevice::vm_bindSampler(GPUHandle* _handle, GPU_ISA shader, u32 slot)
{
    return;
}

void GLDevice::vm_deleteSampler(GPUHandle* _handle)
{
    return;
}

void GLDevice::vm_applySamplerTexture2D(GPUHandle* sampler, GPUHandle* texture, bool mipmaps, TextureAddressMode U, TextureAddressMode V, TextureFilter filter, CompareFunction func, Color border_color)
{
    auto texHandle = dynamic_cast<GLHandle*>(texture);

    glBindTexture(GL_TEXTURE_2D,texHandle->ID);
    applyFilter2D(filter,mipmaps,GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,getAddressMode(U));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,getAddressMode(V));
    float border[4] = {border_color.R,border_color.G,border_color.B,border_color.A};
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,border);
    if(func != CompareFunction::NEVER)
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,getCompareFunc(func));
    glBindTexture(GL_TEXTURE_2D,0);
}