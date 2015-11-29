#pragma once
#include "Globals.hpp"

namespace Break
{
	namespace Physics
	{

		const s32 chunkSize = 16 * 1024;
		const s32 maxBlockSize = 640;
		const s32 blockSizes = 14;
		const s32 chunkArrayIncrement = 128;

		struct BREAK_API Block
		{
			Block* next;
		};
		struct BREAK_API Chunk
		{
			s32 blockSize;
			Block* blocks;
		};


		class BREAK_API BlockAllocator
		{
		public:
			BlockAllocator();
			~BlockAllocator();

			/// Allocate memory. This will use Alloc if the size is larger than _maxBlockSize.
			void* Allocate(s32 size);

			/// Free memory. This will use Free if the size is larger than _maxBlockSize.
			void Free(void* p, s32 size);

			void Clear();

		private:

			Chunk* m_chunks;
			s32 m_chunkCount;
			s32 m_chunkSpace;

			Block* m_freeLists[blockSizes];

			static s32 s_blockSizes[blockSizes];
			static u8 s_blockSizeLookup[maxBlockSize + 1];
			static bool s_blockSizeLookupInitialized;
		};

	}

}
