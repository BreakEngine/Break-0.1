//
//#pragma once
//#include <Globals.hpp>
//#include "Entity.hpp"
//#include "ShapeBatch.hpp"
//#include <Vertex2DPosColor.hpp>
//#include "Transform.hpp"
//
//namespace Break{
//	namespace Graphics{
//		class BREAK_API Shape: public Entity{
//
//			ShapeBatch* batcher;
//
//			Infrastructure::Color m_color;
//
//			transformation transform;
//
//		public :
//
//			Shape(Infrastructure::Color col, ShapeBatch* bat, 	std::vector<Infrastructure::Vertex2DPosColor> vert){
//				batcher = bat;
//				m_color = col;
//				vertices=vert;
//
//			}
//			std::vector<Infrastructure::Vertex2DPosColor> vertices;
//
//			void addVertex(Infrastructure::Vertex2DPosColor vertex){
//				vertices.push_back(vertex);
//			};
//
//			void setColor(Infrastructure::Color color){
//				m_color = color;
//			};
//
//			Infrastructure::Color getColor(){
//				return m_color;
//			};
//
//			virtual void draw() override 
//			{
//
//				batcher->Draw(vertices, transform->getMatrix());
//
//			}
//
//
//		};
//	}
//}