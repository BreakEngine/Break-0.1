#include "StackAllocator.hpp"
#include <assert.h>
#include <glm/glm.hpp>
using namespace Break;
using namespace Break::Physics;



StackAllocator::StackAllocator()
{
	m_index = 0;
	m_allocation = 0;
	m_maxAllocation = 0;
	m_entryCount = 0;
}

StackAllocator::~StackAllocator()
{
	assert(m_index == 0);
	assert(m_entryCount == 0);
}

void* StackAllocator::Allocate(s32 size)
{
	assert(m_entryCount < maxStackEntries);

	StackEntry* entry = m_entries + m_entryCount;
	entry->size = size;
	if (m_index + size > stackSize)
	{
		entry->data = (char*)malloc(size);
		entry->usedMalloc = true;
	}
	else
	{
		entry->data = m_data + m_index;
		entry->usedMalloc = false;
		m_index += size;
	}
	
	m_allocation += size;
	m_maxAllocation = glm::max(m_maxAllocation, m_allocation);
	++m_entryCount;

	return entry->data;
}

void StackAllocator::Free(void* p)
{
	assert(m_entryCount > 0);
	StackEntry* entry = m_entries + m_entryCount - 1;
	assert(p == entry->data);
	if (entry->usedMalloc)
	{
		free(p);
	}
	else
	{
		m_index -= entry->size;
	}
	m_allocation -= entry->size;
	--m_entryCount;

	p = NULL;
}

s32 StackAllocator::GetMaxAllocation() const
{
	return m_maxAllocation;
}