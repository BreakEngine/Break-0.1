//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_TESTAPPLICATION_HPP
#define BREAK_0_1_TESTAPPLICATION_HPP
#include "Infrastructure/inc/Infrastructure.hpp"
#include <Graphics.hpp>
#include <glm/common.hpp>
#include <fstream>
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;
using namespace glm;
using namespace std;

class tv{
public:
    glm::vec4 _a;
    glm::vec4 _b;
    glm::vec2 _c;
    tv(glm::vec4 a,glm::vec4 b,glm::vec2 t){
        _a = a;
        _b = b;
        _c = t;
    }
    static MemoryLayout getDeclaration() {
        MemoryLayout ret;
        ret.append(MemoryElement(MemoryElement::VEC4,"POSITION"));
        ret.append(MemoryElement(MemoryElement::VEC4,"COLOR"));
        ret.append(MemoryElement(MemoryElement::VEC2,"TEXCOORD"));
        return ret;
    }
};


class TestApplication: public Application{
public:
    GeometryPtr geo;
    GPUProgramPtr shader;
    SpriteBatch* sp;
	Texture2DPtr tex;
	SpritePtr sprite;
    TestApplication();

    ~TestApplication();

    void init() override;

    void loadResources() override;

    void setupScene() override;

    void cleanUp() override;

    void input() override;

    void update(/*TimeStep time*/) override;

    void render() override;

};
#endif //BREAK_0_1_TESTAPPLICATION_HPP

TestApplication::TestApplication() : Application() {
    window = std::make_shared<Window>(800,600,"Test");
}

TestApplication::~TestApplication() {
    delete sp;
}

void TestApplication::init() {
    sp = new SpriteBatch();
    Application::init();
}

void TestApplication::loadResources() {
    Services::getEngine()->setType(Time::UNLIMITED);

    VertexSet<tv> veve(tv::getDeclaration());
    IndexSet soso;

    veve.append(tv(vec4(1,-1,1,1)*0.5f,vec4(1,0,0,1),vec2(1,1)));
    veve.append(tv(vec4(1,1,1,1)*0.5f,vec4(0,1,0,1),vec2(1,0)));
    veve.append(tv(vec4(-1,-1,1,1)*0.5f,vec4(0,0,1,1),vec2(0,1)));
    veve.append(tv(vec4(-1,1,1,1)*0.5f,vec4(1,0,1,1),vec2(0,0)));

    //Triangles
    soso.append(0);
    soso.append(1);
    soso.append(2);
    soso.append(2);
    soso.append(1);
    soso.append(3);

    geo = make_shared<Geometry>(&veve,&soso,Primitive::TRIANGLES);

    std::string glVertex = "#version 330\n"
            "layout(location = 0) in vec4 position;\n"
            "layout(location = 1) in vec4 color;\n"
            "layout(location = 2) in vec2 tex;\n"
            "uniform MatrixBuffer{\n"
            "mat3 worldMatrix;\n"
            "vec4 ucolor;\n"
            "mat4 viewMatrix;\n"
            "mat4 projectionMatrix;\n"
            "};\n"
            "out vec4 ocolor;\n"
            "out vec2 otex;\n"
            "void main(){\n"
            "ocolor = ucolor;\n"
            "otex = tex;\n"
            "gl_Position = vec4(position.xyz,1);\n"
            "}\n"
    ;
    std::string glPixel = "#version 330\n"
            "in vec4 ocolor;\n"
            "in vec2 otex;\n"
            "uniform sampler2D diffuseS;\n"
            "void main(){\n"
            "gl_FragColor = texture(diffuseS,otex);\n"
            "gl_FragColor = vec4(1,0,0,1);\n"
            "}\n"
    ;
    std::string dxVertex = "struct VertexInputType\n"
            "{\n"
            "float4 position : POSITION;\n"
            "float4 color : COLOR;\n"
            "float2 tex : TEXCOORD0;\n"
            "};\n"

            "struct PixelInputType\n"
            "{\n"
            "float4 position : SV_POSITION;\n"
            "float4 color : COLOR;\n"
            "float2 tex : TEXCOORD0;\n"
            "};\n"
            "cbuffer MatrixBuffer\n"
            "{\n"
            "float3x3 worldMatrix;\n"
            "float4 ucolor;\n"
            "matrix viewMatrix;\n"
            "matrix projectionMatrix;\n"
            "};\n"
            "PixelInputType main(VertexInputType input)\n"
            "{\n"
            "PixelInputType output;\n"


            // Change the position vector to be 4 units for proper matrix calculations.
            //"input.position.w = 1.0f;\n"

            // Calculate the position of the vertex against the world, view, and projection matrices.
            //"output.position = mul(input.position, worldMatrix);\n"
            //"output.position = mul(output.position, viewMatrix);\n"
            //"output.position = mul(output.position, projectionMatrix);\n"
            "output.position = float4(input.position.xyz,1);\n"

            // Store the input color for the pixel shader to use.
            "output.color = ucolor;\n"
            "output.tex = input.tex;\n"

            "return output;\n"
            "}\n";

    std::string dxPixel = "struct PixelInputType\n"
            "{\n"
            "float4 position : SV_POSITION;\n"
            "float4 color : COLOR;\n"
            "float2 tex : TEXCOORD0;\n"
            "};\n"
            "Texture2D shaderTexture;\n"
            "SamplerState diffuseS;\n"
            "float4 main(PixelInputType input) : SV_TARGET\n"
            "{\n"
            "float4 textureColor;\n"
            "textureColor = shaderTexture.Sample(diffuseS,input.tex);\n"
            "return float4(1,0,0,1);\n"
            "}\n";
    if(Services::getEngine()->getAPI()==API::OpenGL3_3)
        shader = make_shared<GPUProgram>(glVertex,glPixel,tv::getDeclaration());
    else
        shader = make_shared<GPUProgram>(dxVertex,dxPixel,tv::getDeclaration());
    shader->registerUniformBlock("MatrixBuffer",(2*64)+16+48,0,GPU_ISA::VERTEX_SHADER);
    shader->registerUniform("worldMatrix","MatrixBuffer",0,36);
    shader->registerUniform("ucolor","MatrixBuffer",1*48,16);
    //shader->registerUniform("viewMatrix","MatrixBuffer",*48,16);
    //shader->registerUniform("projectionMatrix","MatrixBuffer",1*48,16);
    shader->registerSampler("diffuseS",0,std::make_shared<SamplerState>(),GPU_ISA::PIXEL_SHADER);
    auto idmat = mat4(1);
    shader->setUniform("worldMatrix",&idmat);
    //shader->setUniform("viewMatrix",&idmat);
    //shader->setUniform("projectionMatrix",&idmat);
    auto globalColor = vec4(1,1,1,1);
    shader->setUniform("ucolor",&globalColor);
    cout<<"HONE"<<endl;
    shader->use();

	ImagePtr img = ResourceLoader::load("res/tex/02.jpg");
	tex = make_shared<Texture2D>(img);
	sprite = make_shared<Sprite>(sp,tex);
	sprite->setPosition(100,100);
	sprite->setSize(100,100);
	sprite->setOrigin(50,50);
	//sprite->setRotation(45);

    Application::loadResources();
}

void TestApplication::setupScene() {
    Application::setupScene();
}

void TestApplication::cleanUp() {
    Application::cleanUp();
}

void TestApplication::input() {
    auto m_pos = Mouse::getPosition();
    //std::cout<<m_pos.x<<" || "<<m_pos.y<<std::endl;

    if(Mouse::getButton(Mouse::Left_Button) == Mouse::State_Down){
        std::cout<<"left button Pressed"<<std::endl;
    }

    if(Mouse::getButton(Mouse::Left_Button) == Mouse::State_Up){
        std::cout<<"left button Released"<<std::endl;
    }

    if(Keyboard::getKey(Keyboard::A) == Keyboard::State_Down){
        std::cout<<"A button Pressed"<<std::endl;
    }

    if(Keyboard::getKey(Keyboard::A) == Keyboard::State_Up){
        std::cout<<"A button Released"<<std::endl;
    }

    if(Keyboard::getKey(Keyboard::Esc) == Keyboard::State_Down){
        shutdown();
    }
    Application::input();
}

void TestApplication::update() {
	
    Application::update();
}

void TestApplication::render() {
	static float angle = 0;
	angle+=0.3;
	sprite->setRotation(angle);
    //shader->use();
    //geo->draw();
    
    sp->begin();
	for(int i=0;i<0;i++)
	{
		sp->draw(NULL,rand()%800,rand()%600,10,10,Color(255,150,150,255));
	}
    //sp->draw(tex.get(),100,100,411,336,Color(255,255,255,255));

	//sp->draw(tex.get(),Rect(0,0,100,100),45,Color(255,255,255,255));
	sprite->draw();
    //sp->draw(NULL,0,0,1,1,Color(255,255,255,255));
    //sp->draw(NULL,-1,-1,1,1,Color(255,255,255,255));
    sp->end();

    Application::render();
}
