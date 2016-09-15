TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE -= -O3

LIBS = -lGL -lGLU -lGLEW -lglfw -lpthread -lfreeimage -lportaudio -lfreetype

INCLUDEPATH += $$PWD/deps/glm/include $$PWD/Infrastructure/inc $$PWD/Graphics/inc $$PWD/Physics/inc \
               $$PWD/Graphics/deps/freetype/include

SOURCES += main.cpp \
    Graphics/src/FontFace.cpp \
    Graphics/src/FTRenderer.cpp \
    Graphics/src/ShapeBatch.cpp \
    Graphics/src/SpriteBatch.cpp \
    Graphics/src/Text.cpp \
    Graphics/src/Transform.cpp \
    Infrastructure/src/Application.cpp \
    Infrastructure/src/AssetManager.cpp \
    Infrastructure/src/Directory.cpp \
    Infrastructure/src/DXDevice.cpp \
    Infrastructure/src/DXKeyboard.cpp \
    Infrastructure/src/DXMouse.cpp \
    Infrastructure/src/Engine.cpp \
    Infrastructure/src/File.cpp \
    Infrastructure/src/GLDevice.cpp \
    Infrastructure/src/GLKeyboard.cpp \
    Infrastructure/src/GLMouse.cpp \
    Infrastructure/src/Globals.cpp \
    Infrastructure/src/GPU_VM.cpp \
    Infrastructure/src/GPUProgram.cpp \
    Infrastructure/src/IGXDevice.cpp \
    Infrastructure/src/Image.cpp \
    Infrastructure/src/Keyboard.cpp \
    Infrastructure/src/Linux32.cpp \
    Infrastructure/src/MathUtils.cpp \
    Infrastructure/src/MemoryLayout.cpp \
    Infrastructure/src/Mouse.cpp \
    Infrastructure/src/Object.cpp \
    Infrastructure/src/RAMBuffer.cpp \
    Infrastructure/src/Rect.cpp \
    Infrastructure/src/ResourceLoader.cpp \
    Infrastructure/src/SamplerState.cpp \
    Infrastructure/src/Services.cpp \
    Infrastructure/src/SoundDevice.cpp \
    Infrastructure/src/SoundEffect.cpp \
    Infrastructure/src/Texture.cpp \
    Infrastructure/src/Texture2D.cpp \
    Infrastructure/src/TimeManager.cpp \
    Infrastructure/src/UniformBuffer.cpp \
    Infrastructure/src/Win32.cpp \
    Infrastructure/src/Window.cpp \
    Physics/src/BlockAllocator.cpp \
    Physics/src/Body2D.cpp \
    Physics/src/BodyIsland.cpp \
    Physics/src/BoxBody.cpp \
    Physics/src/BroadPhase.cpp \
    Physics/src/ChainCircleContact.cpp \
    Physics/src/ChainPolygonContact.cpp \
    Physics/src/Chains.cpp \
    Physics/src/ChainShape.cpp \
    Physics/src/CircleBody.cpp \
    Physics/src/CircleCollision.cpp \
    Physics/src/CircleContact.cpp \
    Physics/src/CircleShape.cpp \
    Physics/src/Collision.cpp \
    Physics/src/Contact2D.cpp \
    Physics/src/ContactManager.cpp \
    Physics/src/ContactSolver.cpp \
    Physics/src/Distance.cpp \
    Physics/src/DistanceJoint.cpp \
    Physics/src/DynamicTree.cpp \
    Physics/src/EdgeCircleContact.cpp \
    Physics/src/EdgeCollision.cpp \
    Physics/src/EdgePolygonContact.cpp \
    Physics/src/EdgeShape.cpp \
    Physics/src/Fixture.cpp \
    Physics/src/FrictionJoint.cpp \
    Physics/src/GearJoint.cpp \
    Physics/src/Joint2D.cpp \
    Physics/src/MotorJoint.cpp \
    Physics/src/MouseJoint.cpp \
    Physics/src/PolygonCircleContact.cpp \
    Physics/src/PolygonCollision.cpp \
    Physics/src/PolygonContact.cpp \
    Physics/src/PolygonShape.cpp \
    Physics/src/PrismaticJoint.cpp \
    Physics/src/PullyJoint.cpp \
    Physics/src/RevoluteJoint.cpp \
    Physics/src/RopeJoint.cpp \
    Physics/src/StackAllocator.cpp \
    Physics/src/TimeOfImpact.cpp \
    Physics/src/WeldJoint.cpp \
    Physics/src/WheelJoint.cpp \
    Physics/src/World2D.cpp \
    Physics/src/WorldCallBacks.cpp

HEADERS += \
    Graphics/inc/Component.hpp \
    Graphics/inc/Entity.hpp \
    Graphics/inc/FontFace.h \
    Graphics/inc/FTRenderer.hpp \
    Graphics/inc/Graphics.hpp \
    Graphics/inc/ShapeBatch.h \
    Graphics/inc/Sprite.hpp \
    Graphics/inc/SpriteBatch.hpp \
    Graphics/inc/Text.h \
    Graphics/inc/Transform.hpp \
    Infrastructure/inc/API.hpp \
    Infrastructure/inc/Application.hpp \
    Infrastructure/inc/Argument.hpp \
    Infrastructure/inc/Asset.hpp \
    Infrastructure/inc/AssetManager.hpp \
    Infrastructure/inc/Block.h \
    Infrastructure/inc/Directory.hpp \
    Infrastructure/inc/DXBufferHandle.hpp \
    Infrastructure/inc/DXDevice.hpp \
    Infrastructure/inc/DXKeyboard.hpp \
    Infrastructure/inc/DXMouse.hpp \
    Infrastructure/inc/DXSamplerHandle.hpp \
    Infrastructure/inc/DXShaderHandle.hpp \
    Infrastructure/inc/DXTexture2DHandle.hpp \
    Infrastructure/inc/Engine.hpp \
    Infrastructure/inc/File.hpp \
    Infrastructure/inc/Geometry.hpp \
    Infrastructure/inc/GLDevice.hpp \
    Infrastructure/inc/GLHandle.hpp \
    Infrastructure/inc/GLKeyboard.hpp \
    Infrastructure/inc/GLMouse.hpp \
    Infrastructure/inc/Globals.hpp \
    Infrastructure/inc/GLShaderHandle.hpp \
    Infrastructure/inc/GPU_ISA.hpp \
    Infrastructure/inc/GPU_VM.hpp \
    Infrastructure/inc/GPUException.hpp \
    Infrastructure/inc/GPUHandle.hpp \
    Infrastructure/inc/GPUIns.hpp \
    Infrastructure/inc/GPUProgram.hpp \
    Infrastructure/inc/GPUResource.hpp \
    Infrastructure/inc/IGXDevice.hpp \
    Infrastructure/inc/Image.hpp \
    Infrastructure/inc/IndexBuffer.hpp \
    Infrastructure/inc/IndexSet.hpp \
    Infrastructure/inc/Infrastructure.hpp \
    Infrastructure/inc/InputDevice.hpp \
    Infrastructure/inc/ISet.hpp \
    Infrastructure/inc/Keyboard.hpp \
    Infrastructure/inc/Linux32.hpp \
    Infrastructure/inc/MathUtils.hpp \
    Infrastructure/inc/MemoryElement.hpp \
    Infrastructure/inc/MemoryLayout.hpp \
    Infrastructure/inc/Mouse.hpp \
    Infrastructure/inc/Object.hpp \
    Infrastructure/inc/OS.hpp \
    Infrastructure/inc/Pixel.hpp \
    Infrastructure/inc/Primitive.hpp \
    Infrastructure/inc/RAMBuffer.hpp \
    Infrastructure/inc/Rect.hpp \
    Infrastructure/inc/ResourceLoader.hpp \
    Infrastructure/inc/RingCursor.hpp \
    Infrastructure/inc/SamplerState.hpp \
    Infrastructure/inc/ServiceException.hpp \
    Infrastructure/inc/Services.hpp \
    Infrastructure/inc/SoundDevice.hpp \
    Infrastructure/inc/SoundEffect.hpp \
    Infrastructure/inc/Texture.hpp \
    Infrastructure/inc/Texture2D.hpp \
    Infrastructure/inc/TimeManager.hpp \
    Infrastructure/inc/TimeStep.hpp \
    Infrastructure/inc/UniformBuffer.hpp \
    Infrastructure/inc/Utils.hpp \
    Infrastructure/inc/Vertex2DPos.hpp \
    Infrastructure/inc/Vertex2DPosColor.hpp \
    Infrastructure/inc/Vertex2DPosColorTex.hpp \
    Infrastructure/inc/VertexBuffer.hpp \
    Infrastructure/inc/VertexSet.hpp \
    Infrastructure/inc/Win32.hpp \
    Infrastructure/inc/Window.hpp \
    Physics/inc/BlockAllocator.hpp \
    Physics/inc/Body2D.hpp \
    Physics/inc/BodyIsland.hpp \
    Physics/inc/BoxBody.hpp \
    Physics/inc/Break2D.hpp \
    Physics/inc/BroadPhase.hpp \
    Physics/inc/ChainCircleContact.hpp \
    Physics/inc/ChainPolygonContact.hpp \
    Physics/inc/Chains.hpp \
    Physics/inc/ChainShape.hpp \
    Physics/inc/CircleBody.hpp \
    Physics/inc/CircleContact.hpp \
    Physics/inc/CircleShape.hpp \
    Physics/inc/Collision.hpp \
    Physics/inc/Contact2D.hpp \
    Physics/inc/ContactManager.hpp \
    Physics/inc/ContactSolver.hpp \
    Physics/inc/Distance.hpp \
    Physics/inc/DistanceJoint.hpp \
    Physics/inc/DynamicTree.hpp \
    Physics/inc/EdgeCircleContact.hpp \
    Physics/inc/EdgePolygonContact.hpp \
    Physics/inc/EdgeShape.hpp \
    Physics/inc/Fixture.hpp \
    Physics/inc/FrictionJoint.hpp \
    Physics/inc/GearJoint.hpp \
    Physics/inc/GrowableStack.hpp \
    Physics/inc/Joint2D.hpp \
    Physics/inc/MotorJoint.hpp \
    Physics/inc/MouseJoint.hpp \
    Physics/inc/Physics.hpp \
    Physics/inc/PhysicsGlobals.hpp \
    Physics/inc/PolygonCircleContact.hpp \
    Physics/inc/PolygonContact.hpp \
    Physics/inc/PolygonShape.hpp \
    Physics/inc/PrismaticJoint.hpp \
    Physics/inc/Profile.hpp \
    Physics/inc/PTimeStep.hpp \
    Physics/inc/PullyJoint.hpp \
    Physics/inc/RevoluteJoint.hpp \
    Physics/inc/RopeJoint.hpp \
    Physics/inc/Rotation2D.hpp \
    Physics/inc/Shape.hpp \
    Physics/inc/StackAllocator.hpp \
    Physics/inc/Sweep.hpp \
    Physics/inc/TimeOfImpact.hpp \
    Physics/inc/Transform2D.hpp \
    Physics/inc/WeldJoint.hpp \
    Physics/inc/WheelJoint.hpp \
    Physics/inc/World2D.hpp \
    Physics/inc/WorldCallBacks.hpp \
    NBody.h \
    TestApplication.hpp

DISTFILES += \
    README.md
