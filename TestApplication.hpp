//
// Created by Moustapha on 01/10/2015.
//

#ifndef BREAK_0_1_TESTAPPLICATION_HPP
#define BREAK_0_1_TESTAPPLICATION_HPP
#include "Infrastructure/inc/Infrastructure.hpp"
#include <Graphics.hpp>
#include <glm/common.hpp>
#include <fstream>
#include <Physics.hpp>
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;
using namespace Break::Physics;

using namespace std;

inline u16 readLittleEndian16(u16 x){
    u16 __x = x;
    __x = ((((__x) >> 8) & 0xff) | (((__x) & 0xff) << 8));
    return __x;
}

inline u32 readLittleEndian32(u32 x){
    u32 __x = x;
    __x = ((((__x) & 0xff000000) >> 24) | (((__x) & 0x00ff0000) >>  8) |
    (((__x) & 0x0000ff00) <<  8) | (((__x) & 0x000000ff) << 24));
    return __x;
}
struct WAVHeader{
    u32 ChunckID; //big
    u32 ChunckSize; //little
    u32 Format; //big
    u32 subChunck1ID; //big
    u32 subChunck1Size; //little
    u16 AudioFormat; //little
    u16 NumChannels; //little
    u32 SampleRate; //little
    u32 ByteRate; //little
    u16 BlockAlign; //little
    u16 BitsPerSample; //little
    u32 SubChunck2ID; //big
    u32 SubChunck2Size; //big
};


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
	ShapeBatch* bat;
	Texture2DPtr tex;
	SpritePtr sprite;
	RAMBufferPtr sine;
	SoundEffectPtr musicEffect;
	SoundEffectPtr musicEffect2, loadedMusic, loadedMusic2;

	vector<Vertex2DPosColorTex> tri;

	FTRenderer* renderer;
	FontFacePtr font;
	Text* txt;

	vector<glm::vec2> posii;

	Texture2DPtr p_tex, p_tex2, p_tex3, t_tex;
	ImagePtr p_img, p_img2, p_img3;

	World* world;

	Rect statPosition;
	BoxBody* staticBody;

	vector<BoxBody*> boxBody;
	Rect BoxPosition;

	vector<CircleBody*> circleBody;
	Rect circlePosition;

	ChainBody *chain;

    TestApplication();

    ~TestApplication();

    void init() override;

    void loadResources() override;

    void setupScene() override;

    void cleanUp() override;

    void input() override;

    void update(TimeStep time) override;

    void render() override;

};
#endif //BREAK_0_1_TESTAPPLICATION_HPP

TestApplication::TestApplication() : Application() {
    window = std::make_shared<Window>(800,600,"Test");
	world = nullptr;
}

TestApplication::~TestApplication() {
    delete sp;
	delete world;
	musicEffect = nullptr;
	musicEffect2 = nullptr;
	loadedMusic = nullptr;
	font = nullptr;
	delete txt;
	delete renderer;
	delete bat;
}

void TestApplication::init() {
	bat = new ShapeBatch();
	renderer = new FTRenderer();
	font = ResourceLoader<FontFace>::load("res/fonts/Arial.ttf");
	txt = new Text(renderer,font,"KoKo WaWa");
	txt->setText("Break KoKo");
	txt->size = 24;
	txt->angle = 30;
	//txt->color = Color(255,0,0,255);
	world = new World(glm::vec2(0.0,9.18f));
	statPosition = Rect(0,450,500,50);
	staticBody = new BoxBody(world, statPosition, false);

	chain = new ChainBody(world, staticBody->Physics);

    FilePtr wr = make_shared<File>();
	wr->create("KOKOTEST.txt");
	wr->write("KOKO file is here");

	bool ansD = Directory::Exists("res");
	auto ansList = Directory::listContents();
	ansD = Directory::createDirectory("kokoTest");
	ansD = Directory::changeDirectory("res");
	ansList = Directory::listContents();
	ansD = Directory::Delete("../kokoTest");
	ansD = Directory::changeDirectory("../");
	auto pwd = Directory::getCurrentDirectory();

    sp = new SpriteBatch();
	sine = make_shared<RAMBuffer>(48000*4);

//	s16* input = (s16*)sine->getData();
//	for(int i=0;i<48000;i+=2)
//	{
//		input[i] = i%2==0?8000:-8000;
//		input[i+1] = i%2==0?8000:-8000;
//	}
//	Services::getPlatform()->playSound(sine.get());
	File f,n;
    File music, music2;
    music.open("res/music/Elipse.wav");
	music2.open("res/music/my_village.wav");
    loadedMusic = ResourceLoader<SoundEffect>::load("res/music/Elipse.wav");
    loadedMusic2 = ResourceLoader<SoundEffect>::load("res/music/village.wav");
    //loadedMusic2->play(true);
	//loadedMusic->play();
	n.create("TEST.txt");
	f.open("res/tex/02.jpg",AccessPermission::READ);

    Break::byte* bufferb = new Break::byte[sizeof(WAVHeader)];
    music.read(sizeof(WAVHeader),bufferb);

	Break::byte* buffer2 = new Break::byte[sizeof(WAVHeader)];
	music2.read(sizeof(WAVHeader),buffer2);



	void* buffer = bufferb;

    //Left Right
    WAVHeader* musicHeader = reinterpret_cast<WAVHeader*>(buffer);
    musicHeader->ChunckID = readLittleEndian32(musicHeader->ChunckID);
    musicHeader->Format = readLittleEndian32(musicHeader->Format);
    musicHeader->subChunck1ID = readLittleEndian32(musicHeader->subChunck1ID);
    musicHeader->SubChunck2ID = readLittleEndian32(musicHeader->SubChunck2ID);

	WAVHeader* musicHeader2 = reinterpret_cast<WAVHeader*>(buffer2);
    musicHeader2->ChunckID = readLittleEndian32(musicHeader2->ChunckID);
    musicHeader2->Format = readLittleEndian32(musicHeader2->Format);
    musicHeader2->subChunck1ID = readLittleEndian32(musicHeader2->subChunck1ID);
    musicHeader2->SubChunck2ID = readLittleEndian32(musicHeader2->SubChunck2ID);

    // musicHeader.ChunckSize = readLittleEndian32(*(buffer+4));
    // musicHeader.Format = *(buffer+8);
    // musicHeader.subChunck1ID = *(buffer+12);
    // musicHeader.subChunck1Size = readLittleEndian32(*(buffer+16));
    // musicHeader.AudioFormat = readLittleEndian16(*(buffer+20));
    // musicHeader.NumChannels = readLittleEndian16(*(buffer+22));
    // musicHeader.SampleRate = readLittleEndian32(*(buffer+24));
    // musicHeader.ByteRate = readLittleEndian32(*(buffer+28));
    // musicHeader.BlockAlign = readLittleEndian16(*(buffer+32));
    // musicHeader.BitsPerSample = readLittleEndian16(*(buffer+34));
    // musicHeader.SubChunck2ID = *(buffer+36);
    // musicHeader.SubChunck2Size = readLittleEndian32(*(buffer+40));

    Break::byte* music_buffer = new Break::byte[musicHeader->ChunckSize];
    music.read(musicHeader->ChunckSize,music_buffer);

	Break::byte* music_buffer2 = new Break::byte[musicHeader2->ChunckSize];
	memset(music_buffer2,0,musicHeader2->ChunckSize);
    music2.read(musicHeader2->ChunckSize,music_buffer2);
    //SoundEffect* musicEffect=new SoundEffect(music_buffer,musicHeader->ChunckSize);
	musicEffect = make_shared<SoundEffect>(music_buffer,musicHeader->ChunckSize);
	musicEffect2 = make_shared<SoundEffect>(music_buffer2,musicHeader2->ChunckSize);
	musicEffect->setVolume(0.1);
	//musicEffect2->
	musicEffect2->setLooping(true);
	//Services::getSoundDevice()->play(musicEffect);
	//Services::getSoundDevice()->play(musicEffect2);
	//musicEffect->play();
	//musicEffect2->play();
    //soundDevice->play(buffer);

	cout<<f.getSize()<<endl;
	cout<<f.getName()<<endl;
	cout<<Services::getPlatform()->getAbsolutePath("res/tex/02.jpg")<<endl;

    Application::init();
}

void TestApplication::loadResources() {
	p_img = ResourceLoader<Image>::load("res/tex/ball.png");
	p_tex = make_shared<Texture2D>(p_img);

	p_img2 = ResourceLoader<Image>::load("res/tex/ground.png");
	p_tex2 = make_shared<Texture2D>(p_img2);

	p_img3 = ResourceLoader<Image>::load("res/tex/box.png");
	p_tex3 = make_shared<Texture2D>(p_img3);

    Services::getEngine()->setType(Time::UNLIMITED);

    VertexSet<tv> veve(tv::getDeclaration());
    IndexSet soso;

    veve.append(tv(glm::vec4(1,-1,1,1)*0.5f, glm::vec4(1,0,0,1), glm::vec2(1,1)));
    veve.append(tv(glm::vec4(1,1,1,1)*0.5f, glm::vec4(0,1,0,1), glm::vec2(1,0)));
    veve.append(tv(glm::vec4(-1,-1,1,1)*0.5f, glm::vec4(0,0,1,1), glm::vec2(0,1)));
    veve.append(tv(glm::vec4(-1,1,1,1)*0.5f, glm::vec4(1,0,1,1), glm::vec2(0,0)));

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
    auto idmat = glm::mat4(1);
    shader->setUniform("worldMatrix",&idmat);
    //shader->setUniform("viewMatrix",&idmat);
    //shader->setUniform("projectionMatrix",&idmat);
    auto globalColor = glm::vec4(1,1,1,1);
    shader->setUniform("ucolor",&globalColor);
    cout<<"HONE"<<endl;
    shader->use();

	ImagePtr img = ResourceLoader<Image>::load("res/tex/02.jpg");
	tex = make_shared<Texture2D>(img);
	sprite = make_shared<Sprite>(sp,tex);
	sprite->setPosition(100,100);
	sprite->setSize(100,100);
	sprite->setOrigin(50,50);
	//sprite->setRotation(45);

    Application::loadResources();
}

void TestApplication::setupScene() {
	//t_tex = make_shared<Texture2D>(txt->m_image);
	
	tri.push_back(Vertex2DPosColorTex(glm::vec2(0,0),Color(255,255,255,255),glm::vec2(0)));
	tri.push_back(Vertex2DPosColorTex(glm::vec2(100,0),Color(255,255,255,255),glm::vec2(0)));
	tri.push_back(Vertex2DPosColorTex(glm::vec2(100,100),Color(255,255,255,255),glm::vec2(0)));
	tri.push_back(Vertex2DPosColorTex(glm::vec2(0,100),Color(255,255,255,255),glm::vec2(0)));
	tri.push_back(Vertex2DPosColorTex(glm::vec2(200,200),Color(255,255,255,255),glm::vec2(0)));
	tri.push_back(Vertex2DPosColorTex(glm::vec2(100,200),Color(255,255,255,255),glm::vec2(0)));

    Application::setupScene();
}

void TestApplication::cleanUp() {
    Application::cleanUp();
}

void TestApplication::input() {
    auto m_pos = Mouse::getPosition();
    //std::cout<<m_pos.x<<" || "<<m_pos.y<<std::endl;

    if(Mouse::getButton(Mouse::Left_Button) == Mouse::State_Down)
	{
		circlePosition = Rect(m_pos.x,m_pos.y,30,30);
		circleBody.push_back(new CircleBody(world,circlePosition,15,true));
	}

	if(Mouse::getButton(Mouse::Right_Button) == Mouse::State_Down)
	{
		std::cout<<m_pos.x<<" || "<<m_pos.y<<std::endl;
		BoxPosition = Rect(m_pos.x,m_pos.y,50,50);
		boxBody.push_back(new BoxBody(world,BoxPosition,true));
	}

	if(Mouse::getButton(Mouse::Middle_Button) == Mouse::State_Down)
	{
		posii.push_back(glm::vec2(m_pos.x,m_pos.y));
	}

    if(Keyboard::getKey(Keyboard::A) == Keyboard::State_Down){
        std::cout<<"A button Pressed"<<std::endl;
    }

	if(Keyboard::getKey(Keyboard::R) == Keyboard::State_Up){
        cout<<"Total: "<<boxBody.size()+circleBody.size()<<endl;
		cout<<"Circles: "<<circleBody.size()<<endl;
		cout<<"Boxes: "<<boxBody.size()<<endl;
    }

    if(Keyboard::getKey(Keyboard::A) == Keyboard::State_Up){
        std::cout<<"A button Released"<<std::endl;
    }

	auto volume = Services::getSoundDevice()->getVolume();
	if(Keyboard::getKey(Keyboard::Up) == Keyboard::State_Down){
		volume += 0.1;
		Services::getSoundDevice()->setVolume(volume);
	}

	if(Keyboard::getKey(Keyboard::Down) == Keyboard::State_Down && volume > 0)
	{
		volume -= 0.1;
		Services::getSoundDevice()->setVolume(volume);
	}

	int speed = 4;
    if(Keyboard::getKey(Keyboard::Right) == Keyboard::State_Down)
    {
	    sprite->move(speed,0);
    }else if(Keyboard::getKey(Keyboard::Left) == Keyboard::State_Down)
    {
	    sprite->move(-speed,0);
    }else if(Keyboard::getKey(Keyboard::Up) == Keyboard::State_Down)
    {
	    sprite->move(0,-speed);
    }else if(Keyboard::getKey(Keyboard::Down) == Keyboard::State_Down)
    {
	    sprite->move(0,speed);
    }

	if(Keyboard::getKey(Keyboard::Space) == Keyboard::State_Down)
	{
		if(musicEffect2->isPlaying())
			musicEffect2->pause();
		else
			musicEffect2->play();
	}

	if(Keyboard::getKey(Keyboard::S) == Keyboard::State_Down)
	{
		musicEffect2->stop();
	}

    if(Keyboard::getKey(Keyboard::Esc) == Keyboard::State_Down){
        shutdown();
    }
    Application::input();
}

void TestApplication::update(TimeStep tick) {
	//cout<<tick.delta<<" || "<<tick.elapsedTime<<" || "<<Services::getEngine()->getFPS()<<endl;
	sprite->rotate(1);
	world->Step(tick.delta * 5,7,3);

    Application::update(tick);
}

void TestApplication::render() {
//	renderer->render("Break moka moka");
//	t_tex->update(renderer->m_image);




    shader->use();
    geo->draw();

	bat->begin();
	bat->draw(tri);
	bat->end();

    sp->begin();
	for(int i=0;i<10;i++){
		txt->position = glm::vec2(300,300);
		txt->angle = rand()%360;
		txt->draw(sp);
	}

	for(int i=0;i<0;i++)
	{
		sp->draw(NULL,rand()%800,rand()%600,10,10,Color(255,150,150,255));
	}
    //sp->draw(tex.get(),100,100,411,336,Color(255,255,255,255));

	//sp->draw(tex.get(),Rect(0,0,100,100),45,Color(255,255,255,255));
	sprite->draw();
    //sp->draw(NULL,0,0,1,1,Color(255,255,255,255));
    //sp->draw(NULL,-1,-1,1,1,Color(255,255,255,255));

	for(int i=0;i<posii.size();i++)
	{
		//sp->draw(p_tex.get(),Rect(posii[i].x,posii[i].y,p_tex->getWidth(),p_tex->getHeight()),Rect(0,0,p_tex->getWidth(),p_tex->getHeight()),0,glm::vec2(p_tex->getWidth()/2,p_tex->getHeight()/2),Color(255,255,255,255));
		//sp->draw(tex.get(), posii[i].x,posii[i].y,10,10,Color(255,255,255,255));
		sp->draw(NULL, posii[i].x,posii[i].y,10,10,Color(100,255,100,255));
	}

	for(int i =0 ; i < boxBody.size(); ++i)
	{
		Rect r = boxBody[i]->GetRect();
		sp->draw(p_tex3.get(),boxBody[i]->GetRect(),boxBody[i]->GetAngle(),boxBody[i]->GetOrigin(),Color(255,255,255,255));
	}


	//Circles draw..
    for(int i =0 ; i < circleBody.size(); ++i)
	{
		sp->draw(p_tex.get(),circleBody[i]->GetRect(),circleBody[i]->GetAngle(),circleBody[i]->GetOrigin(),Color(255,255,255,255));
	}

	//chain draw..
	for(int i =0 ; i < chain->box.size(); ++i)
	{
		sp->draw(p_tex3.get(),chain->box[i]->GetRect(),chain->box[i]->GetAngle(),chain->box[i]->GetOrigin(),Color(255,255,255,255));
	}

	//ground draw..
	sp->draw(p_tex2.get(), staticBody->GetRect(), 0, glm::vec2(0,0) , Color(255,255,255,255));

    sp->end();

    Application::render();
}
