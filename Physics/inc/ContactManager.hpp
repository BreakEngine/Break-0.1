#pragma once
#include "BroadPhase.hpp"

namespace Break
{
	namespace Physics
	{

		class BREAK_API Contact;
		class BREAK_API ContactFilter;
		class BREAK_API ContactListener;
		class BREAK_API BlockAllocator;

		// Delegate of World.
		class BREAK_API ContactManager
		{
		public:
			ContactManager();

			// Broad-phase callback.
			void AddPair(void* proxyUserDataA, void* proxyUserDataB);

			void FindNewContacts();

			void Destroy(Contact* c);

			void Collide();

			BroadPhase m_broadPhase;
			Contact* m_contactList;
			s32 m_contactCount;
			ContactFilter* m_contactFilter;
			ContactListener* m_contactListener;
			BlockAllocator* m_allocator;
		};



	}

}
