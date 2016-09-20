#include "BlockAllocator.hpp"
#include <limits.h>
#include <memory.h>
#include <stddef.h>
#include <assert.h>

using namespace Break;
using namespace Break::Physics;


s32 BlockAllocator::s_blockSizes[blockSizes] =
{
	16,		// 0
	32,		// 1
	64,		// 2
	96,		// 3
	128,	// 4
	160,	// 5
	192,	// 6
	224,	// 7
	256,	// 8
	320,	// 9
	384,	// 10
	448,	// 11
	512,	// 12
	640,	// 13
};
u8 BlockAllocator::s_blockSizeLookup[maxBlockSize + 1];
bool BlockAllocator::s_blockSizeLookupInitialized;

BlockAllocator::BlockAllocator()
{
	assert(blockSizes < UCHAR_MAX);

	m_chunkSpace = chunkArrayIncrement;
	m_chunkCount = 0;
	m_chunks = (Chunk*)malloc(m_chunkSpace * sizeof(Chunk));

	memset(m_chunks, 0, m_chunkSpace * sizeof(Chunk));
	memset(m_freeLists, 0, sizeof(m_freeLists));

	if (s_blockSizeLookupInitialized == false)
	{
		s32 j = 0;
		for (s32 i = 1; i <= maxBlockSize; ++i)
		{
			assert(j < blockSizes);
			if (i <= s_blockSizes[j])
			{
				s_blockSizeLookup[i] = (u8)j;
			}
			else
			{
				++j;
				s_blockSizeLookup[i] = (u8)j;
			}
		}

		s_blockSizeLookupInitialized = true;
	}
}

BlockAllocator::~BlockAllocator()
{
	for (s32 i = 0; i < m_chunkCount; ++i)
	{
		free(m_chunks[i].blocks);
	}

	free(m_chunks);
}

void* BlockAllocator::Allocate(s32 size)
{
	if (size == 0)
		return NULL;

	assert(0 < size);

	if (size > maxBlockSize)
	{
		return malloc(size);
	}

	s32 index = s_blockSizeLookup[size];
	assert(0 <= index && index < blockSizes);

	if (m_freeLists[index])
	{
		Block* block = m_freeLists[index];
		m_freeLists[index] = block->next;
		return block;
	}
	else
	{
		if (m_chunkCount == m_chunkSpace)
		{
			Chunk* oldChunks = m_chunks;
			m_chunkSpace += chunkArrayIncrement;
			m_chunks = (Chunk*)malloc(m_chunkSpace * sizeof(Chunk));
			memcpy(m_chunks, oldChunks, m_chunkCount * sizeof(Chunk));
			memset(m_chunks + m_chunkCount, 0, chunkArrayIncrement * sizeof(Chunk));
			free(oldChunks);
		}

		Chunk* chunk = m_chunks + m_chunkCount;
		chunk->blocks = (Block*)malloc(chunkSize);
#if defined(_DEBUG)
		memset(chunk->blocks, 0xcd, chunkSize);
#endif
		s32 blockSize = s_blockSizes[index];
		chunk->blockSize = blockSize;
		s32 blockCount = chunkSize / blockSize;
		assert(blockCount * blockSize <= chunkSize);
		for (s32 i = 0; i < blockCount - 1; ++i)
		{
			Block* block = (Block*)((s8*)chunk->blocks + blockSize * i);
			Block* next = (Block*)((s8*)chunk->blocks + blockSize * (i + 1));
			block->next = next;
		}
		Block* last = (Block*)((s8*)chunk->blocks + blockSize * (blockCount - 1));
		last->next = NULL;

		m_freeLists[index] = chunk->blocks->next;
		++m_chunkCount;

		return chunk->blocks;
	}
}

void BlockAllocator::Free(void* p, s32 size)
{
	if (size == 0)
	{
		return;
	}

	assert(0 < size);

	if (size > maxBlockSize)
	{
		free(p);
		return;
	}

	s32 index = s_blockSizeLookup[size];
	assert(0 <= index && index < blockSizes);

#ifdef _DEBUG
	// Verify the memory address and size is valid.
	s32 blockSize = s_blockSizes[index];
	bool found = false;
	for (s32 i = 0; i < m_chunkCount; ++i)
	{
		Chunk* chunk = m_chunks + i;
		if (chunk->blockSize != blockSize)
		{
			assert(	(s8*)p + blockSize <= (s8*)chunk->blocks ||
				(s8*)chunk->blocks + chunkSize <= (s8*)p);
		}
		else
		{
			if ((s8*)chunk->blocks <= (s8*)p && (s8*)p + blockSize <= (s8*)chunk->blocks + chunkSize)
			{
				found = true;
			}
		}
	}

	assert(found);

	memset(p, 0xfd, blockSize);
#endif

	Block* block = (Block*)p;
	block->next = m_freeLists[index];
	m_freeLists[index] = block;
}

void BlockAllocator::Clear()
{
	for (s32 i = 0; i < m_chunkCount; ++i)
	{
		free(m_chunks[i].blocks);
	}

	m_chunkCount = 0;
	memset(m_chunks, 0, m_chunkSpace * sizeof(Chunk));

	memset(m_freeLists, 0, sizeof(m_freeLists));
}
