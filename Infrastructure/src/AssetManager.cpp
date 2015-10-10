//
// Created by Moustapha on 09/10/2015.
//

#include <API.hpp>
#include <Vertex2DPosColorTex.hpp>
#include "AssetManager.hpp"
#include "GPUProgram.hpp"
#include "Services.hpp"
using namespace std;
using namespace Break;
using namespace Break::Infrastructure;

AssetManager::AssetManager() {
    this->populateDefaultAssets();
}

AssetManager::~AssetManager(){
    this->cleanUp();
}

void AssetManager::populateDefaultAssets() {
    /************************ Default Shaders ************************/

    std::string glVertexShape2D = "#version 330\n"
            "layout(location = 0) in vec2 position;\n"
            "layout(location = 1) in vec4 color;\n"
            "layout(location = 2) in vec2 texCoord;\n"

            "uniform MatrixBuffer{\n"
            "mat4 model;\n"
            "mat4 view;\n"
            "mat4 projection;\n"
            "vec4 hasTexture;\n"
            "};\n"
            "out vec4 ocolor;\n"
            "out vec2 otex;\n"
            "out vec4 ohasTex;\n"
            "void main(){\n"
            "gl_Position = projection*view*model*vec4(position.xy,0,1);\n"
            //"gl_Position = vec4(position.xy,0,1);\n"
            "ocolor = color;\n"
            "otex = texCoord;\n"
            "ohasTex = hasTexture;\n"
            "}\n"
    ;
    std::string glPixelShape2D = "#version 330\n"
            "in vec4 ocolor;\n"
            "in vec2 otex;\n"
            "in vec4 ohasTex;\n"
            "uniform sampler2D spTex;\n"
            "void main(){\n"
            "if(ohasTex.x==1){"
            "gl_FragColor = ocolor*texture(spTex,otex);\n"
            "}else\n"
            "gl_FragColor = ocolor;\n"
            //"gl_FragColor = vec4(1,1,1,1);\n"
            "if(gl_FragColor.w==0)\n"
            "discard;"
            "}\n"
    ;

    std::string dxVertexShape2D = "#pragma pack_matrix( row_major )\n"
            "struct VertexInputType\n"
            "{\n"
            "float2 position : POSITION;\n"
            "float4 color : COLOR0;\n"
            "float2 texCoord : TEXCOORD0;\n"
            "};\n"
            "cbuffer MatrixBuffer\n"
            "{\n"
            "matrix model;\n"
            "matrix view;\n"
            "matrix projection;\n"
            "float4 hasTexture;\n"
            "};\n"
            "struct PixelInputType\n"
            "{\n"
            "float4 position : SV_POSITION;\n"
            "float4 color : COLOR0;\n"
            "float2 texCoord : TEXCOORD0;\n"
            "float4 ohasTex : TEXCOORD1;\n"
            "};\n"

            "PixelInputType main(VertexInputType input)\n"
            "{\n"
            "PixelInputType output;\n"

            "output.position = float4(input.position.xy,0,1);\n"
            "output.position = mul(output.position,model);\n"
            "output.position = mul(output.position,view);\n"
            "output.position = mul(output.position,projection);\n"

            "output.color = input.color;\n"
            "output.texCoord = input.texCoord;\n"
            "output.ohasTex = hasTexture;\n"

            "return output;\n"
            "}\n";

    std::string dxPixelShape2D = "#pragma pack_matrix( row_major )\n"
            "struct PixelInputType\n"
            "{\n"
            "float4 position : SV_POSITION;\n"
            "float4 color : COLOR0;\n"
            "float2 texCoord : TEXCOORD0;\n"
            "float4 ohasTex : TEXCOORD1;\n"
            "};\n"
            "Texture2D spTex;\n"
            "SamplerState SampleType;\n"
            "float4 main(PixelInputType input) : SV_TARGET\n"
            "{\n"
            "float4 endColor;\n"
            "if(input.ohasTex.x==1)\n"
            "endColor = input.color *spTex.Sample(SampleType,input.texCoord);\n"
            "else\n"
            "endColor = input.color;\n"
            "if (endColor.w==0)\n"
            "discard;\n"
            "return endColor;\n"
            "}\n";

    GPUProgramPtr shape2DShader;
    if(Services::getEngine()->getAPI() == API::OpenGL3_3)
        shape2DShader = make_shared<GPUProgram>(glVertexShape2D,glPixelShape2D,Vertex2DPosColorTex::getDescription());
    else if(Services::getEngine()->getAPI() == API::DirectX11)
        shape2DShader = make_shared<GPUProgram>(dxVertexShape2D,dxPixelShape2D,Vertex2DPosColorTex::getDescription());

    shape2DShader->registerUniformBlock("MatrixBuffer",(64*3)+16,0,GPU_ISA::VERTEX_SHADER);
    shape2DShader->registerUniform("model","MatrixBuffer",0,64);
    shape2DShader->registerUniform("view","MatrixBuffer",64,64);
    shape2DShader->registerUniform("projection","MatrixBuffer",2*64,64);
    shape2DShader->registerUniform("hasTexture","MatrixBuffer",3*64,16);
    shape2DShader->registerSampler("spTex",0,make_shared<SamplerState>(),GPU_ISA::PIXEL_SHADER);


    shape2DShader->id = "_shape2DShader";
    m_assetTable["_shape2DShader"] = shape2DShader;
}

void AssetManager::cleanUp() {
    m_assetTable.clear();
}

Asset* AssetManager::find(std::string id){
    auto res = m_assetTable.find(id);
    if(res == m_assetTable.end())
        return nullptr;
    else
        return res->second.get();
}

void AssetManager::add(AssetPtr res){
    if(res){
        m_assetTable[res->id] = res;
    }
}