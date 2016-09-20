//
// Created by Moustapha on 09/10/2015.
//

#ifndef BREAK_0_1_SPRITE_HPP
#define BREAK_0_1_SPRITE_HPP

#include <Globals.hpp>
#include <Texture2D.hpp>
#include <Pixel.hpp>
#include "Entity.hpp"
#include "SpriteBatch.hpp"
#include <Rect.hpp>

namespace Break{
    namespace Graphics{
        class BREAK_API Sprite: public Entity{

        public:
            virtual void input() override{
                Entity::input();
            }
            virtual void update(Infrastructure::TimeStep tick) override{

                Entity::update(tick);
            }
            virtual void draw() override
            {
				if(spriteBatch){
					if(texture)
						this->spriteBatch->draw(texture.get(),dest,src,angle,origin,color);
					else
						this->spriteBatch->draw(NULL,dest,src,angle,origin,color);
				}

                Entity::draw();
            }

        protected:
            ///spirte texture
            Infrastructure::Texture2DPtr texture;

            ///destenation rectangle, the rectangle of the screen that this sprite will be drawn to
            Infrastructure::Rect dest;
            ///source rectangle, the rectangle of the texture that this sprite will be mapped to
            Infrastructure::Rect src;
            ///angle of rotation
            real32 angle;

			glm::vec2 origin;
            ///color tint of this sprite
            Infrastructure::Color color;
            ///sprite batch handle
            SpriteBatch* spriteBatch;
        public:
            ///default constrcutor
            Sprite(SpriteBatch* batcher)
            {
                spriteBatch = batcher;
                texture = nullptr;
                dest = Infrastructure::Rect(0,0,0,0);
                src =  Infrastructure::Rect(0,0,0,0);
				origin =  glm::vec2(0,0);
                angle = 0;
                color = Infrastructure::Color(255,255,255,255);
            }

            Sprite(const Sprite& other)
            {
                this->texture = other.texture;
                this->spriteBatch = other.spriteBatch;
                this->color = other.color;
                this->angle = other.angle;
                this->dest = other.dest;
                this->src = other.src;
				this->origin = other.origin;
            }

            ///texture init constructor
            Sprite(SpriteBatch* batcher, Infrastructure::Texture2DPtr tex)
            {
                spriteBatch = batcher;

                texture = tex;

                dest = Infrastructure::Rect(0,0,texture->getWidth(),texture->getHeight());
                src =  Infrastructure::Rect(0,0,texture->getWidth(),texture->getHeight());
                angle = 0;
				origin =  glm::vec2(0,0);
                color = Infrastructure::Color(255,255,255,255);
            }

            Infrastructure::Color getColor()
            {
                return color;
            }

            void setColor(Infrastructure::Color c)
            {
                color =c;
            }
            void setRotation(float val)
            {
                angle = val;
            }

            float getRotation()
            {
                return angle;
            }

			void rotate(float theta)
            {
	            angle+=theta;
            }

            glm::vec2 getPosition()
            {
                return glm::vec2(dest.x,dest.y);
            }

            void setPosition(float x, float y)
            {
                dest.x =x;
                dest.y = y;
            }

			void move(float x, float y)
            {
	            dest.x+=x;
				dest.y+=y;
            }

            glm::vec2 getSize()
            {
                return glm::vec2(dest.width,dest.height);
            }

            void setSize(float x,float y)
            {
                dest.width = x;
                dest.height = y;
            }

            Infrastructure::Texture2D* getTexture()
            {
                return texture.get();
            }

            void setTexture(Infrastructure::Texture2DPtr tex)
            {
                texture = tex;
            }

            Infrastructure::Rect getSourceRect(){
                return src;
            }

            void setSourceRect(Infrastructure::Rect val)
            {
                src = val;
            }

			glm::vec2 getOrigin()
            {
	            return origin;
            }

			void setOrigin(float x,float y)
            {
	            origin = glm::vec2(x,y);
            }

            virtual ~Sprite()
            {
                texture = nullptr;
            }
        };
        typedef std::shared_ptr<Sprite> SpritePtr;
    }
}
#endif //BREAK_0_1_SPRITE_HPP
