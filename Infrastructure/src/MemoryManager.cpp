#include "MemoryManager.h"
#include "Services.hpp"
#include "ServiceException.hpp"
#include <unordered_map>

using namespace Break;
using namespace Break::Infrastructure;


Memory::Memory(Block mem_handle):m_memory(mem_handle)
{
	m_allocated = m_free = m_memory.size;
	m_memoryRoot = new MemoryNode();
	m_memoryRoot->free = false;
	m_memoryRoot->block = m_memory;

	auto startNode = new MemoryNode();
	startNode->block = m_memory;
	m_memoryRoot->next = startNode;
}

Memory::~Memory()
{
	if (m_memory.ptr != nullptr)
		Services::getPlatform()->virtualFree(m_memory.ptr, m_memory.size);

	if (m_memoryRoot)
		delete m_memoryRoot;
}

Block Memory::allocateBlock(size_t size)
{
	Block res;
	try {
		res = Allocation_Algorithm(size);
	}catch(ServiceException ex)
	{
		//implement merge techniques
		throw;
	}
	return res;
}

void Memory::deallocateBlock(Block mem)
{
	if(owns(mem.ptr))
	{
		auto it = m_memoryRoot;
		while(it)
		{
			if(!it->free &&	it->block.ptr == mem.ptr)
			{
				it->free = true;
				auto next_node = it->next;
				auto prev_node = it->prev;

				if(next_node && prev_node && next_node->free && prev_node->free)
				{
					prev_node->block.size += it->block.size + next_node->block.size;
					prev_node->next = next_node->next;

					if (next_node->next)
						next_node->next->prev = prev_node;

					it->next = nullptr;
					next_node->next = nullptr;
					it->prev = nullptr;
					next_node->prev = nullptr;

					delete it;
					delete next_node;
				}else if(next_node && next_node->free)
				{
					it->free = true;
					it->block.size += next_node->block.size;
					it->next = next_node->next;

					if (next_node->next)
						next_node->next->prev = it;

					next_node->prev = nullptr;
					next_node->next = nullptr;

					delete next_node;
				}else if(prev_node && prev_node->free)
				{
					prev_node->block.size += it->block.size;
					prev_node->next = it->next;

					if(it->next)
						it->next->prev = prev_node;

					it->next = nullptr;
					it->prev = nullptr;

					delete it;
				}

				break;
			}
			it = it->next;
		}
	}
}

bool Memory::owns(void* ptr)
{
	if (ptr >= m_memory.ptr && ptr < m_memory.ptr + m_memory.size)
		return true;
	return false;
}

void Memory::deallocateAll()
{
	
}

Block Memory::Allocation_Algorithm(size_t size)
{
	Block res;

	auto it = m_memoryRoot;
	while(it)
	{
		if(it->free &&
			it->block.size >= size)
		{
			MemoryNode* new_node = new MemoryNode();
			new_node->free = false;
			new_node->block.ptr = it->block.ptr+it->block.size-size;
			new_node->block.size = size;

			it->block.ptr += size;
			it->block.size -= size;

			new_node->prev = it;
			new_node->next = it->next;

			if (it->next)
				it->next->prev = new_node;

			it->next = new_node;
			res = new_node->block;
			break;
		}
		it = it->next;
	}

	if (res.ptr == nullptr)
		throw ServiceException("Unable to allocate memory of size: " + std::to_string(size));

	return res;
}