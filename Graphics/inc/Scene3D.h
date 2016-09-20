#pragma once
#include "Graphics.hpp"
#include "Model_3D.h"
#include "FP_Camera.h"
#include <ShaderHelper.h>


namespace Break
{
	namespace Graphics
	{

		class BREAK_API Scene3D
		{
		public:
			FP_Camera *default_camera;

			 
			Scene3D();
			~Scene3D();

			void AddModel(std::string Dir , std::string ModelName,std::string Tag);

			Model_3D* FindModel(std::string Tag);


			void Draw_Scene();
			void Draw_Scene_with_Camera(glm::mat4 CameraMatrix);

			void Update_Inputs();



		private:
			std::vector<Model_3D*> Models;
			glm::mat4 View_Proj_Matrix;
			glm::vec2 tmp_mouse_pos;

			Infrastructure::ShaderHelper* shaderProgram;


			void init_Camera_Mat();

		};


	}
}

