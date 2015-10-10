//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_SPRITEBATCH_HPP
#define BREAK_0_1_SPRITEBATCH_HPP

#include <Globals.hpp>
#include <Geometry.hpp>
#include <VertexSet.hpp>
#include <IndexSet.hpp>
#include <Vertex2DPosColorTex.hpp>
#include <Texture2D.hpp>
#include <glm/common.hpp>
#include "Rect.hpp"
#include <GPUProgram.hpp>

namespace Break{
    namespace Graphics{
        class Sprite;

        class BREAK_API SpriteBatch{

            ///batch limit
            static const u32 limit = 10500;

            u32 m_count,m_vCount, m_iCount;

            ///vertices set
            Infrastructure::VertexSet<Infrastructure::Vertex2DPosColorTex> m_vertices;
            ///index set
            Infrastructure::IndexSet m_indices;

            Infrastructure::GeometryPtr m_geometry;

            Infrastructure::Texture2D* m_texture;

            Infrastructure::GPUProgram* m_shader;

            ///indicates custom shaders
            bool m_customShader;

            ///checks if it need to flush sprites to GPU
            void checkFlush(Infrastructure::Texture2D* texture);
            ///rotates a point
            glm::vec2 rotatePoint(glm::vec2 point, float angle);
        public:

            ///default constructor
            SpriteBatch(Infrastructure::GPUProgramPtr shader = nullptr);
            ~SpriteBatch();

            ///creates a new sprite instance
            std::shared_ptr<Sprite> newSprite(Infrastructure::Texture2DPtr tex = nullptr);

            ///begin drawing batch
            void begin();

            /**
			 * \brief draw a sprite
			 * \param tex texture of the sprite
			 * \param x x position of the sprite
			 * \param y y postiion of the sprite
			 * \param color color of the sprite
			 * \auhtor Moustapha Saad
			 */
            void draw(Infrastructure::Texture2D*,int x, int y,Infrastructure::Color color);

            /**
             * \brief draw a sprite
             * \param tex texture of the sprite
             * \param x x position of the sprite
             * \param y y postiion of the sprite
             * \param width width of the sprite
             * \param height height of the sprite
             * \param color color of the sprite
             * \auhtor Moustapha Saad
             */
            void draw(Infrastructure::Texture2D*,int x, int y,int width, int height,Infrastructure::Color color);

            /**
             * \brief draw a sprite
             * \param tex texture of the sprite
             * \param dest destenation rectangle on screen
             * \param angle angle of the sprite
             * \param color color of the sprite
             * \auhtor Moustapha Saad
             */
            void draw(Infrastructure::Texture2D*,Rect dest,float angle,Infrastructure::Color color);

            /**
             * \brief draw a sprite
             * \param tex texture of the sprite
             * \param dest destenation rect on screen
             * \param src source rect from the texture
             * \angle angle of rotation of the sprite
             * \param color color of the sprite
             * \auhtor Moustapha Saad
             */
            void draw(Infrastructure::Texture2D*,Rect,Rect,float,Infrastructure::Color);

            ///flushes the current sprites
            void flush();
            ///ends the current batch
            void end();
        };
    }
}
#endif //BREAK_0_1_SPRITEBATCH_HPP
