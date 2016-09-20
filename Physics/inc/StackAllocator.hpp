#pragma once
#include "Globals.hpp"

namespace Break
{
	namespace Physics
	{

		const s32 stackSize = 100 * 1024;	// 100k
		const s32 maxStackEntries = 32;

		struct BREAK_API StackEntry
		{
			char* data;
			s32 size;
			bool usedMalloc;
		};


		// This is a stack allocator used for fast per step allocations.
		// You must nest allocate/free pairs. The code will assert
		// if you try to interleave multiple allocate/free pairs.
		class BREAK_API StackAllocator
		{
		public:
			StackAllocator();
			~StackAllocator();

			void* Allocate(s32 size);
			void Free(void* p);

			s32 GetMaxAllocation() const;

		private:

			char m_data[stackSize];
			s32 m_index;

			s32 m_allocation;
			s32 m_maxAllocation;

			StackEntry m_entries[maxStackEntries];
			s32 m_entryCount;
		};

	}

}