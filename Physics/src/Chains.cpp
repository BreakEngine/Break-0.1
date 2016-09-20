#include "Chains.hpp"
#include <RevoluteJoint.hpp>
#include "World2D.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;

ChainBody::ChainBody(World *world,Body *ground)
{
	Rect def = Rect(200,100,20,10);
	Rect def3 = Rect(600,100,20,10);

	Rect def2 = Rect(200,120,20,10);

	box.push_back(new BoxBody(world,def,false));
	box.push_back(new BoxBody(world,def2,true));

	//set up the common properties of the joint before entering the loop
	RevoluteJointDef revoluteJointDef;
	revoluteJointDef.localAnchorA = glm::vec2(20,10);
	revoluteJointDef.localAnchorB = glm::vec2(0,0);

	revoluteJointDef.bodyA = box[0]->Physics;
	revoluteJointDef.bodyB = box[1]->Physics;
	///revoluteJointDef.collideConnected = true;
    world->CreateJoint( &revoluteJointDef );




	///loopp on
	for(int i=0; i < 8 ; ++i)
	{
		box.push_back(new BoxBody(world,def2,true));


		revoluteJointDef.localAnchorA = glm::vec2(20,5);
		revoluteJointDef.localAnchorB = glm::vec2(0,5);
		revoluteJointDef.bodyA = box[i+1]->Physics;
		revoluteJointDef.bodyB = box[i+2]->Physics;
	//	revoluteJointDef.collideConnected = true;
		world->CreateJoint( &revoluteJointDef );

	}


	//box.push_back(new BoxBody(world,def3,false));

	//revoluteJointDef.localAnchorA.Set(20,5);
	//revoluteJointDef.localAnchorB.Set(0,15);

	//revoluteJointDef.bodyA = box[box.size()-2]->Physics;
	//revoluteJointDef.bodyB = box[box.size()-1]->Physics;
	/////revoluteJointDef.collideConnected = true;
 //   world->CreateJoint( &revoluteJointDef );







}

ChainBody::~ChainBody()
{
}