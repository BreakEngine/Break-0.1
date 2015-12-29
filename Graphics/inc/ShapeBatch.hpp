#ifndef BREAK_0_1_SHAPEBATCH_HPP
#define BREAK_0_1_SHAPEBATCH_HPP

#include <Globals.hpp>
#include <Geometry.hpp>
#include <Vertex2DPosColor.hpp>
#include <VertexSet.hpp>
#include <IndexSet.hpp>

namespace Break{
	namespace Graphics{
		class Shape;

		class BREAK_API ShapeBatch{

			
			Infrastructure::VertexSet<Infrastructure::Vertex2DPosColor> m_Vertices;


			Infrastructure::GeometryPtr m_Geometry;

			 u32 m_VCount, m_ICount, m_Count;

			Infrastructure::IndexSet m_Indices;

			Infrastructure::GPUProgram* m_Shader;

			static const u32 lmt =1000;// any constant 

			bool m_CustomShader;

		public:
			
			//default constructor
			ShapeBatch(Infrastructure::GPUProgramPtr shader = nullptr);

			//default destructor
			~ShapeBatch();


			//begin drawing shapes
			void Begin();

			// end the current batch
			void End();

			// flushs the current  shapes
			void Flush();
			/**
			* \brief draw a shape
			* \param vertices a vertices to be drawn 
			* \param mat 4X4 matrix
			*/
			void Draw(std::vector<Infrastructure::Vertex2DPosColor> vertices/* glm::mat4 transformed_mat*/);
		};
	}
}
#endif