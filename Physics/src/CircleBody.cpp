#include "CircleBody.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Graphics;
using namespace Break::physics;


CircleBody::CircleBody(World *world,Rect position , int radius,  bool isDynamic)
{
	_world = world;
	R = position;
	bodyDef.position = glm::vec2(position.x , position.y);

	if(isDynamic)
		bodyDef.type = dynamicBody;



	//create body in the world.. todo
	Physics = _world->CreateBody(&bodyDef);

	circleshape.m_radius = radius;

	fixturedef.shape=&circleshape;
	fixturedef.density=5.0f;
	fixturedef.restitution = 0.5f;
	fixturedef.friction = 0.3f;
	Physics->CreateFixture(&fixturedef);
}

Rect CircleBody::GetRect()
{
	R = Rect(Physics->GetWorldCenter().x - R.width / 2,Physics->GetWorldCenter().y  -R.height/2 ,R.width, R.height);
	return R;
}

double CircleBody::GetAngle()
{
	float ang = Physics->GetAngle();
	ang  = (ang * 180) / glm::pi<float>();

	return ang;
}

glm::vec2 CircleBody::GetOrigin()
{
	return glm::vec2(GetRect().width / 2,GetRect().height / 2);
}