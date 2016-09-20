#include "BoxBody.hpp"
#include <World2D.hpp>
#include <Body2D.hpp>
#include <Fixture.hpp>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;

BoxBody::BoxBody(World *world,Rect position ,  bool isDynamic)
{
	_world = world;
	R =position;
	bodyDef.position = glm::vec2(position.x  , position.y );

	bodyDef.angle = 0;

	if(isDynamic)
		bodyDef.type = dynamicBody;  

	Physics = world->CreateBody(&bodyDef);
	glm::vec2 origin = glm::vec2(position.width - position.width  / 2 ,position.height - position.height/2);


	polyshape.SetAsBox(position.width / 2 ,  position.height / 2 ,origin , bodyDef.angle);

	FixtureDef fixturedef;
	fixturedef.shape=&polyshape;
	fixturedef.density=1.0f;

	Physics->CreateFixture(&fixturedef);
}

void BoxBody::SetAngle(real32 angle)
{
	bodyDef.angle = angle;
}


Rect BoxBody::GetRect()
{
	R = Rect(Physics->GetPosition().x,Physics->GetPosition().y,R.width, R.height);
	return R;
}

double BoxBody::GetAngle() 
{
	double ang = Physics->GetAngle();
	ang  = (ang * 180) / glm::pi<double>();

	return ang;
}

glm::vec2 BoxBody::GetOrigin()
{
	return glm::vec2(0,0);
}