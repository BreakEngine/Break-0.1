#pragma once
#include "Globals.hpp"
#include <GL/glew.h>

namespace Break
{
	namespace Infrastructure
	{

		class BREAK_API ShaderHelper
		{
		public:
			ShaderHelper();
			~ShaderHelper();

			GLuint programID;

			GLuint modelMatrixID;
			GLuint vpMatrixID;

			void BindModelMatrix(GLfloat* value);
			void BindVPMatrix(GLfloat* value);

			//calls glUseProgram.
			void UseModelShaderProgram();

			//load shaders from files.
			void LoadModelShaderProgram();

		private:
			GLuint ReadShadersFiles(const char * vertex_file_path,const char * fragment_file_path);

		};
	}
}