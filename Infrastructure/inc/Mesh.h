#pragma once
#include "Globals.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.h"

namespace Break
{
	namespace Infrastructure
	{

		class BREAK_API Mesh
		{
		public:
			Mesh();
			~Mesh();

			void DrawMesh();
			void initBuffers();

			std::vector<glm::vec3> VertexData;
			std::vector<glm::vec3> ColorData;
			std::vector<unsigned short> IndicesData;
			std::vector<glm::vec2> UVData;
			std::vector<glm::vec3> NormalsData;
			Material* Textures;

		private:

			//VAO
			GLuint mVertexArrayObjectID;

			//VBOs
			GLuint mVertexDataBufferID;
			GLuint mColorDataBufferID;
			GLuint mIndicesDataBufferID;
			GLuint mUVDataBufferID;
			GLuint mNormalsBufferID;

		};


	}
}