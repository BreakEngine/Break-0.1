#include "Joint2D.hpp"
#include "DistanceJoint.hpp"
#include "WheelJoint.hpp"
#include "MouseJoint.hpp"
#include "RevoluteJoint.hpp"
#include "PrismaticJoint.hpp"
#include "PullyJoint.hpp"
#include "GearJoint.hpp"
#include "WeldJoint.hpp"
#include "FrictionJoint.hpp"
#include "RopeJoint.hpp"
#include "MotorJoint.hpp"
#include "Body2D.hpp"
#include "World2D.hpp"
#include "BlockAllocator.hpp"

#include <new>

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;



Joint* Joint::Create(const JointDef* def, BlockAllocator* allocator)
{
	Joint* joint = NULL;

	switch (def->type)
	{
	case distanceJoint:
		{
			void* mem = allocator->Allocate(sizeof(DistanceJoint));
			joint = new (mem) DistanceJoint(static_cast<const DistanceJointDef*>(def));
		}
		break;

	case mouseJoint:
		{
			void* mem = allocator->Allocate(sizeof(MouseJoint));
			joint = new (mem) MouseJoint(static_cast<const MouseJointDef*>(def));
		}
		break;

	case prismaticJoint:
		{
			void* mem = allocator->Allocate(sizeof(PrismaticJoint));
			joint = new (mem) PrismaticJoint(static_cast<const PrismaticJointDef*>(def));
		}
		break;

	case revoluteJoint:
		{
			void* mem = allocator->Allocate(sizeof(RevoluteJoint));
			joint = new (mem) RevoluteJoint(static_cast<const RevoluteJointDef*>(def));
		}
		break;

	case pulleyJoint:
		{
			void* mem = allocator->Allocate(sizeof(PulleyJoint));
			joint = new (mem) PulleyJoint(static_cast<const PulleyJointDef*>(def));
		}
		break;

	case gearJoint:
		{
			void* mem = allocator->Allocate(sizeof(GearJoint));
			joint = new (mem) GearJoint(static_cast<const GearJointDef*>(def));
		}
		break;

	case wheelJoint:
		{
			void* mem = allocator->Allocate(sizeof(WheelJoint));
			joint = new (mem) WheelJoint(static_cast<const WheelJointDef*>(def));
		}
		break;

	case weldJoint:
		{
			void* mem = allocator->Allocate(sizeof(WeldJoint));
			joint = new (mem) WeldJoint(static_cast<const WeldJointDef*>(def));
		}
		break;

	case frictionJoint:
		{
			void* mem = allocator->Allocate(sizeof(FrictionJoint));
			joint = new (mem) FrictionJoint(static_cast<const FrictionJointDef*>(def));
		}
		break;

	case ropeJoint:
		{
			void* mem = allocator->Allocate(sizeof(RopeJoint));
			joint = new (mem) RopeJoint(static_cast<const RopeJointDef*>(def));
		}
		break;

	case motorJoint:
		{
			void* mem = allocator->Allocate(sizeof(MotorJoint));
			joint = new (mem) MotorJoint(static_cast<const MotorJointDef*>(def));
		}
		break;

	default:
		assert(false);
		break;
	}

	return joint;
}

void Joint::Destroy(Joint* joint, BlockAllocator* allocator)
{
	joint->~Joint();
	switch (joint->m_type)
	{
	case distanceJoint:
		allocator->Free(joint, sizeof(DistanceJoint));
		break;

	case mouseJoint:
		allocator->Free(joint, sizeof(MouseJoint));
		break;

	case prismaticJoint:
		allocator->Free(joint, sizeof(PrismaticJoint));
		break;

	case revoluteJoint:
		allocator->Free(joint, sizeof(RevoluteJoint));
		break;

	case pulleyJoint:
		allocator->Free(joint, sizeof(PulleyJoint));
		break;

	case gearJoint:
		allocator->Free(joint, sizeof(GearJoint));
		break;

	case wheelJoint:
		allocator->Free(joint, sizeof(WheelJoint));
		break;

	case weldJoint:
		allocator->Free(joint, sizeof(WeldJoint));
		break;

	case frictionJoint:
		allocator->Free(joint, sizeof(FrictionJoint));
		break;

	case ropeJoint:
		allocator->Free(joint, sizeof(RopeJoint));
		break;

	case motorJoint:
		allocator->Free(joint, sizeof(MotorJoint));
		break;

	default:
		assert(false);
		break;
	}
}

Joint::Joint(const JointDef* def)
{
	assert(def->bodyA != def->bodyB);

	m_type = def->type;
	m_prev = NULL;
	m_next = NULL;
	m_bodyA = def->bodyA;
	m_bodyB = def->bodyB;
	m_index = 0;
	m_collideConnected = def->collideConnected;
	m_islandFlag = false;
	m_userData = def->userData;

	m_edgeA.joint = NULL;
	m_edgeA.other = NULL;
	m_edgeA.prev = NULL;
	m_edgeA.next = NULL;

	m_edgeB.joint = NULL;
	m_edgeB.other = NULL;
	m_edgeB.prev = NULL;
	m_edgeB.next = NULL;
}

bool Joint::IsActive() const
{
	return m_bodyA->IsActive() && m_bodyB->IsActive();
}


