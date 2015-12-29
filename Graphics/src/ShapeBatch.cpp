#include "ShapeBatch.hpp"
#include "Shape.hpp"
#include <Vertex2DPosColor.hpp>
#include <Vertex2DPosColorTex.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;

ShapeBatch::ShapeBatch(GPUProgramPtr shader){
	m_Vertices.resize(lmt);
	m_Indices.resize(lmt);
	m_ICount = 0;
	m_VCount = 0;
	m_Count = 0;

	  if(shader)
    {
        m_Shader = shader.get();
        m_CustomShader = true;
    }
	 else {
        m_Shader = static_cast<GPUProgram *>(Services::getAssetManager()->find("_shape2DShader"));
        m_CustomShader = false;
    }

    m_Geometry = make_shared<Geometry>(
            make_shared<VertexBuffer>(RAMBuffer::asHandle(&m_Vertices[0],m_Vertices.count()*sizeof(Vertex2DPosColor)),
                                      Vertex2DPosColor::getDescription(),
                                      GPU_ISA::DYNAMIC),
            make_shared<IndexBuffer>(RAMBuffer::asHandle(&m_Indices[0],m_Indices.count()*sizeof(u32)),
                                     GPU_ISA::DYNAMIC),
            Primitive::TRIANGLES
   );
}

ShapeBatch::~ShapeBatch(){
	m_Geometry ==nullptr;
	if(m_CustomShader)
		delete m_Shader;
}

void ShapeBatch::Begin() {
	
	m_VCount = 0;
	m_Count = 0;
	m_ICount = 0;
	
	auto idmat = glm::mat4(1);
	auto proj = glm::ortho(0.0f,(float)Services::getEngine()->getApplication()->getWindow()->getWidth(),(float)Services::getEngine()->getApplication()->getWindow()->getHeight(),0.0f,-10.0f,10.0f);

	m_Shader->setUniform("model", &glm::mat4(1));
	m_Shader->setUniform("view", &glm::mat4(1));
	m_Shader->setUniform("projection",&proj);
	m_Shader->use();
}

void ShapeBatch::Draw(std::vector<Infrastructure::Vertex2DPosColor> vertices/* glm::mat4 transformed_mat */){
	 
	
	if (m_Count > 1 )
		Flush();

	u32 indicesNum= 0, verticesNum = 0;

	//insert vertices into vertexSet
	for(u32 i = 0; i < vertices.size(); i++){
		
		if(i+ m_VCount == m_Vertices.count())
			m_Vertices.resize(2 * (i + m_VCount));
		
		m_Vertices[i + m_VCount] = vertices[i];
		verticesNum ++;
		std::cout <<i+m_VCount<<" ";
	}
	
	std :: cout << "\n";
	// insert indices into IndexSet
	for(u32 i = 1 ; i<vertices.size()- 1; i++){
		
		if(m_ICount + 3 >= m_Indices.count() )
			m_Indices.resize(2 * m_ICount);

		m_Indices[m_ICount++] = m_VCount ;
		m_Indices[m_ICount++] = m_VCount + i;
		m_Indices[m_ICount++] = m_VCount + i + 1 ;
		
		indicesNum ++;
		cout << m_VCount<<" "<< m_VCount + i << m_VCount + i + 1<<"\n";
	}

	m_VCount+=verticesNum;
	m_ICount+=indicesNum;
    m_Count ++;
	

}

void ShapeBatch::End(){
	if(m_Count>0)
		Flush();
}

void ShapeBatch::Flush(){

	    if(m_Count<=0)
        return;

       glm::vec4 hasTex = glm::vec4(0,1,1,1);
        m_Shader->setUniform("hasTexture",&hasTex);
        m_Shader->use();
    

    m_Geometry->m_vertexBuffer->flush();
    m_Geometry->m_indexBuffer->flush();
    m_Geometry->m_verticesCount = m_VCount;
    m_Geometry->m_indicesCount = m_ICount;
    m_Geometry->draw();
    m_Geometry->m_verticesCount = 0;
    m_Geometry->m_indicesCount = 0;
   
    m_Count = 0;
    m_VCount = 0;
    m_ICount = 0;
}
