#include "ShapeBatch.h"
#include <glm/gtc/matrix_transform.inl>
#include <stack>
using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;
using namespace std;

ShapeBatch::ShapeBatch(Infrastructure::GPUProgramPtr shader)
{
	m_vertices.resize(limit);
    m_indices.resize(limit);

	if(shader)
	{
		m_shader = shader.get();
		m_customShader = true;
	}else
	{
		m_shader = static_cast<GPUProgram*>(Services::getAssetManager()->find("_shape2DShader"));
		m_customShader = false;
	}

	m_geometry = make_shared<Geometry>(
		make_shared<VertexBuffer>(RAMBuffer::asHandle(&m_vertices[0],m_vertices.count()*sizeof(Vertex2DPosColorTex)),
		Vertex2DPosColorTex::getDescription(),
		GPU_ISA::DYNAMIC),
//		make_shared<IndexBuffer>(RAMBuffer::asHandle(&m_indices[0],m_indices.count()*sizeof(u32)), GPU_ISA::DYNAMIC),
		nullptr,
		Primitive::TRIANGLES
		);

	m_count = 0;
    m_iCount = 0;
    m_vCount =0;
	m_texture = nullptr;
}

ShapeBatch::~ShapeBatch()
{
	m_geometry = nullptr;
	if(m_customShader)
		delete m_shader;
}

void ShapeBatch::begin()
{
	m_iCount = 0;
    m_vCount = 0;
    m_count = 0;

	auto idmat = glm::mat4(1);

	auto proj = glm::ortho(0.0f,(float)Services::getEngine()->getApplication()->getWindow()->getWidth(),(float)Services::getEngine()->getApplication()->getWindow()->getHeight(),0.0f,-10.0f,10.0f);

	m_shader->setUniform("model", &idmat);
    m_shader->setUniform("view", &idmat);
    m_shader->setUniform("projection",&proj);

	m_shader->use();
}

void ShapeBatch::draw(std::vector<Infrastructure::Vertex2DPosColorTex> vertices)
{
	if(m_count>=limit)
		flush();

	//int three = 0;
	//stack<u32> indices_stack;
	for(int i=0;i<vertices.size();i++)
	{
		m_vertices[m_vCount++] = vertices[i];
		//indices_stack.push(m_vCount-1);
		//if(indices_stack.size()>=3)
		//{
		//	auto first = indices_stack.top();
		//	indices_stack.pop();
		//	auto second = indices_stack.top();
		//	indices_stack.pop();
		//	auto third = indices_stack.top();
		//	indices_stack.pop();


		//	m_indices[m_iCount++] = third;

		//	m_indices[m_iCount++] = second;

		//	m_indices[m_iCount++] = first;

		//	indices_stack.push(third);
		//	indices_stack.push(first);
		//}
		m_count++;
	}

	//while(indices_stack.size()>=3)
	//{
	//	auto first = indices_stack.top();
	//		indices_stack.pop();
	//		auto second = indices_stack.top();
	//		indices_stack.pop();
	//		auto third = indices_stack.top();
	//		indices_stack.pop();


	//		m_indices[m_iCount++] = third;

	//		m_indices[m_iCount++] = second;

	//		m_indices[m_iCount++] = first;

	//		indices_stack.push(third);
	//		indices_stack.push(first);
	//}

}

void ShapeBatch::end()
{
	flush();
}

void ShapeBatch::flush()
{
	if(m_count<=0)
		return;

	if(m_texture)
	{
		//set texture
	}else
	{
		//set no texture
		glm::vec4 hasTex = glm::vec4(0,1,1,1);
        m_shader->setUniform("hasTexture",&hasTex);
        m_shader->use();
	}

	m_geometry->m_vertexBuffer->flush();
	//m_geometry->m_indexBuffer->flush();
	m_geometry->m_verticesCount = m_vCount;
	//m_geometry->m_indicesCount = m_iCount;
	m_geometry->draw();

	m_geometry->m_verticesCount = 0;
	m_geometry->m_indicesCount = 0;

	m_count = 0;
	m_vCount = 0;
	m_iCount = 0;

}