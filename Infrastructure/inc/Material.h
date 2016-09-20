#pragma once
#include "Globals.hpp"
#include <GL/glew.h>
#include <Image.hpp>
#include <Texture2D.hpp>
#include "../deps/assimp/include/scene.h"

namespace Break
{
	namespace Infrastructure
	{
		class BREAK_API Material
		{

		public:
			int width, height;
			int numComponents;
			unsigned char * data;
			int texUnit;
			void useMaterial();
			Material();
			Material(std::string fileName, int texUnit);
			virtual ~Material();
		protected:
		private:
			Material(const Material& Material) {}
			void operator=(const Material& Material) {}
			GLuint m_Material;
		};

	}
}