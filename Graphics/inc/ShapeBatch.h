#pragma once

#include <Globals.hpp>
#include <Geometry.hpp>
#include <VertexSet.hpp>
#include <IndexSet.hpp>
#include <Vertex2DPosColorTex.hpp>
#include <glm/common.hpp>
#include <GPUProgram.hpp>
#include <Texture2D.hpp>

namespace Break
{
	namespace Graphics
	{
		
		class BREAK_API ShapeBatch
		{
			static const u32 limit = 10500;

			u32 m_count, m_vCount, m_iCount;

			Infrastructure::VertexSet<Infrastructure::Vertex2DPosColorTex> m_vertices;

			Infrastructure::IndexSet m_indices;

			Infrastructure::GeometryPtr m_geometry;

			Infrastructure::Texture2D* m_texture;

			Infrastructure::GPUProgram* m_shader;

			bool m_customShader;

		public:

			ShapeBatch(Infrastructure::GPUProgramPtr shader = nullptr);
			~ShapeBatch();

			void begin();
			void draw(std::vector<Infrastructure::Vertex2DPosColorTex> vertices);
			void end();
			void flush();

		};
	}
}
