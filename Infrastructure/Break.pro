TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS = -lGL -lGLU -lGLEW -lglfw -lpthread -lfreeimage -lportaudio

INCLUDEPATH += $$PWD/deps/glm/include $$PWD/deps/glew-1.10.0/include $$PWD/inc

DISTFILES += \
    Break_Infrastructure.sdf \
    Break_Infrastructure.v11.suo \
    Break_Infrastructure.vcxproj \
    Break_Infrastructure.vcxproj.filters \
    CMakeLists.txt

HEADERS += \
    inc/API.hpp \
    inc/Application.hpp \
    inc/Argument.hpp \
    inc/Asset.hpp \
    inc/AssetManager.hpp \
    inc/Block.h \
    inc/Directory.hpp \
    inc/DXBufferHandle.hpp \
    inc/DXDevice.hpp \
    inc/DXKeyboard.hpp \
    inc/DXMouse.hpp \
    inc/DXSamplerHandle.hpp \
    inc/DXShaderHandle.hpp \
    inc/DXTexture2DHandle.hpp \
    inc/Engine.hpp \
    inc/File.hpp \
    inc/Geometry.hpp \
    inc/GLDevice.hpp \
    inc/GLHandle.hpp \
    inc/GLKeyboard.hpp \
    inc/GLMouse.hpp \
    inc/Globals.hpp \
    inc/GLShaderHandle.hpp \
    inc/GPU_ISA.hpp \
    inc/GPU_VM.hpp \
    inc/GPUException.hpp \
    inc/GPUHandle.hpp \
    inc/GPUIns.hpp \
    inc/GPUProgram.hpp \
    inc/GPUResource.hpp \
    inc/IGXDevice.hpp \
    inc/Image.hpp \
    inc/IndexBuffer.hpp \
    inc/IndexSet.hpp \
    inc/Infrastructure.hpp \
    inc/InputDevice.hpp \
    inc/ISet.hpp \
    inc/Keyboard.hpp \
    inc/MathUtils.hpp \
    inc/MemoryElement.hpp \
    inc/MemoryLayout.hpp \
    inc/Mouse.hpp \
    inc/Object.hpp \
    inc/OS.hpp \
    inc/Pixel.hpp \
    inc/Primitive.hpp \
    inc/RAMBuffer.hpp \
    inc/Rect.hpp \
    inc/ResourceLoader.hpp \
    inc/RingCursor.hpp \
    inc/SamplerState.hpp \
    inc/ServiceException.hpp \
    inc/Services.hpp \
    inc/SoundDevice.hpp \
    inc/SoundEffect.hpp \
    inc/Texture.hpp \
    inc/Texture2D.hpp \
    inc/TimeManager.hpp \
    inc/TimeStep.hpp \
    inc/UniformBuffer.hpp \
    inc/Utils.hpp \
    inc/Vertex2DPos.hpp \
    inc/Vertex2DPosColor.hpp \
    inc/Vertex2DPosColorTex.hpp \
    inc/VertexBuffer.hpp \
    inc/VertexSet.hpp \
    inc/Win32.hpp \
    inc/Window.hpp \
    TestApplication.hpp \
    inc/Linux32.hpp

SOURCES += \
    src/Application.cpp \
    src/AssetManager.cpp \
    src/Directory.cpp \
    src/DXDevice.cpp \
    src/DXKeyboard.cpp \
    src/DXMouse.cpp \
    src/Engine.cpp \
    src/File.cpp \
    src/GLDevice.cpp \
    src/GLKeyboard.cpp \
    src/GLMouse.cpp \
    src/Globals.cpp \
    src/GPU_VM.cpp \
    src/GPUProgram.cpp \
    src/IGXDevice.cpp \
    src/Image.cpp \
    src/Keyboard.cpp \
    src/MathUtils.cpp \
    src/MemoryLayout.cpp \
    src/Mouse.cpp \
    src/Object.cpp \
    src/RAMBuffer.cpp \
    src/Rect.cpp \
    src/ResourceLoader.cpp \
    src/SamplerState.cpp \
    src/Services.cpp \
    src/SoundDevice.cpp \
    src/SoundEffect.cpp \
    src/Texture.cpp \
    src/Texture2D.cpp \
    src/TimeManager.cpp \
    src/UniformBuffer.cpp \
    src/Win32.cpp \
    src/Window.cpp \
    main.cpp \
    src/Linux32.cpp
